/*
GCode 
Board code for paintball guns
Copyright (C) 2013 Gabriel Ricard

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


About:
This piece of software is intended to be used with Arduino-compatible boards powering
paintball guns. It may eventually grow beyond that, but right now the focus is on 
Arduino. 

I created this mainly due to my curiosity with how paintball guns work, and well, 
because I could. I've been involved with the Free Software Foundation and other open source
projects (Linux, PHP, etc.) in the past and agree with the ideals of the open source community.

Currently it has only been used in an AKA Viking with a custom board setup based around the Femtoduino board.


License:
This software is currently licensed for use and redistribution under the GNU GPL v3. 
This license does not currently allow derivative works that do not provide full source code.
I may reevaluate this decision later. For now, if you wish to use this code in a closed source
manner, please consult me first by emailing me at sumorai@gmail.com. Thank you.

Notes:
This software is designed for use with the stock Arduino IDE. 
It is broken up into multiple files:
   GCode.ino - main code file
   Firing.ino - code for firing mode 
   Programming.ino - code for programming mode
   Eyes.ino - code for manipulating breakbeam eye systems
   LED.ino - code for manipulating RGB LED
   Trigger.ino - code for handling trigger input
   config.h - primary means of configuring the code
   globals.h - global state variables and related constants/definitions
   debug.h - debugging macros & configuration
   byte.h - a hack to get around a compilation error with the Arduino IDE

Contributing:
Anyone is welcome to contribute to this project, or to fork it and work on it on their own.
I ask that if you intend to contribute code back to the GCode project that you ensure you
follow the same coding style used in this code so there is a uniformity to the codebase.

*/

#include <EEPROM.h>

#include "config.h"
#include "globals.h"
#include "debug.h"

// this function is called once upon device start
void setup() {
  #ifdef DEBUG_MODE
  // enable serial console
  Serial.begin(9600);
  #endif;

  // initialize the LED pin as an output:
  pinMode(SOLENOID_PIN, OUTPUT);  
  
  // initialize the pushbutton pin as an input:
  pinMode(TRIGGER_PIN, INPUT);
  
  // don't need this. using physical pull-up to avoid potential bounce caused by internal pull-up in low voltage situations
  // turn on internal 20kOhm pull-up resistor
  //digitalWrite(TRIGGER_PIN, HIGH);
  
  // initialize the eye tranceiver pin as an output:
  pinMode(EYE_TX_PIN, OUTPUT);  
  
  // initialize the eye receiver pin as an input:
  pinMode(EYE_RX_PIN, INPUT);  
  
  // Note: RGB LED pins do not need initialization since they're handled with analogWrite()

  // Note: do a raw read here , don't use triggerRead() until we're in the main loop
  byte pinState = digitalRead(TRIGGER_PIN);

  if( TRIGGER_CLOSED == pinState ) {
    DEBUG_PRINTLN("Programming mode...");
    OperatingMode = MODE_PROGRAMMING;
    
    // at the start of the programming mode, wait for button release if it's still held
    Prog_WaitForTriggerRelease = true;
  } else {
    DEBUG_PRINTLN("Firing mode...");
    OperatingMode = MODE_FIRING;
  }

  // read in programming register items
  Conf_Debounce = EEPROM.read(REGISTER_DEBOUNCE);
  Conf_Dwell = EEPROM.read(REGISTER_DWELL);
  Conf_LoaderDelay = EEPROM.read(REGISTER_LOADER_DELAY);
  Conf_MechDebounce = EEPROM.read(REGISTER_MECH_DEBOUNCE);
  Conf_FSDODwell = EEPROM.read(REGISTER_FSDO_DWELL);
  Conf_FireMode = EEPROM.read(REGISTER_FIRE_MODE);
  Conf_ROFEyesOn = EEPROM.read(REGISTER_ROF_ON);  
 
  #ifdef ALLOW_CONFIGURABLE_EYES_OFF_ROF
  Conf_ROFEyesOff = EEPROM.read(REGISTER_ROF_OFF);
  #else 
  Conf_ROFEyesOff = 2; // 8bps
  #endif
  
  // double check min/max config values
  // just in case a register gets corrupted, we don't want to have crazy values in here
  // arduino boards seem to default to 255 for unset registers
  if( Conf_Debounce < 1 || Conf_Debounce > REGISTER_DEBOUNCE_MAX ) Conf_Debounce = DEFAULT_DEBOUNCE;
  if( Conf_Dwell < 1 || Conf_Dwell > REGISTER_DWELL_MAX ) Conf_Dwell = DEFAULT_DWELL;
  if( Conf_LoaderDelay < 1 || Conf_LoaderDelay > REGISTER_LOADER_DELAY_MAX ) Conf_LoaderDelay = DEFAULT_LOADER_DELAY;
  if( Conf_MechDebounce < 1 || Conf_MechDebounce > REGISTER_MECH_DEBOUNCE_MAX ) Conf_MechDebounce = DEFAULT_MECH_DEBOUNCE;
  if( Conf_FSDODwell < 1 || Conf_FSDODwell > REGISTER_FSDO_DWELL_MAX ) Conf_FSDODwell = DEFAULT_FSDO_DWELL;
  if( Conf_FireMode < 1 || Conf_FireMode > REGISTER_FIRE_MODE_MAX ) Conf_FireMode = DEFAULT_FIRE_MODE;
  if( Conf_ROFEyesOn < 1 || Conf_ROFEyesOn > REGISTER_ROF_ON_MAX ) Conf_ROFEyesOn = DEFAULT_ROF_EYES_ON;
  
  #ifdef ALLOW_CONFIGURABLE_EYES_OFF_ROF
  if( Conf_ROFEyesOff < 1 || Conf_ROFEyesOff > REGISTER_ROF_OFF_MAX ) Conf_ROFEyesOff = DEFAULT_ROF_EYES_OFF;
  #endif
  
  // debug print the corrected values
  #ifdef DEBUG_MODE
  DEBUG_PRINT("   Debounce=");DEBUG_PRINTLN(Conf_Debounce);
  DEBUG_PRINT("   Dwell=");DEBUG_PRINTLN(Conf_Dwell);
  DEBUG_PRINT("   LoaderDelay=");DEBUG_PRINTLN(Conf_LoaderDelay);
  DEBUG_PRINT("   MechDebounce=");DEBUG_PRINTLN(Conf_MechDebounce);
  DEBUG_PRINT("   FSDODwell=");DEBUG_PRINTLN(Conf_FSDODwell);
  DEBUG_PRINT("   FireMode=");DEBUG_PRINTLN(Conf_FireMode);
  DEBUG_PRINT("   ROFEyesOn=");DEBUG_PRINTLN(Conf_ROFEyesOn);
  DEBUG_PRINT("   ROFEyesOff=");DEBUG_PRINTLN(Conf_ROFEyesOff);
  #endif

  // Convert register values to usable program values
  // setting of 1 is 0ms, so subtract one from register value
  Conf_LoaderDelay -= 1;
  Conf_MechDebounce -= 1;
  Conf_FSDODwell -= 1;
  
  // convert ROF register values to numerical BPS value
  Conf_ROFEyesOn = convertROFValue(Conf_ROFEyesOn);
  DEBUG_PRINT("Eyes on ROF = ");DEBUG_PRINT(Conf_ROFEyesOn);DEBUG_PRINTLN("bps");
  Conf_ROFEyesOff = convertROFValue(Conf_ROFEyesOff);
  DEBUG_PRINT("Eyes off ROF = ");DEBUG_PRINT(Conf_ROFEyesOff);DEBUG_PRINTLN("bps");
  
  // set initial dwell
  // may be modified by FSDO dwell
  Op_Dwell = Conf_Dwell;

  // set debounce
  Debounce_MinTriggerDownCount = Conf_Debounce; // for sequential read debounce
  Debounce_DelayTime = Conf_Debounce; // for delay debounce
  
  // start off generic timer
  operationTiming = millis();
  LastEyeBlinkOn = false;
  LastEyeBlink = operationTiming;
  
  // setup ROF cap
  Op_UseROFCap = (Conf_ROFEyesOn > 1);

  // set default trigger state
  Trigger_PriorState = Trigger_State = TRIGGER_STATE_WAITING;
  
  // turn eyes on
  setEyeStatus(EYES_ON);
  
  // disable eyes in full auto mode
  // we have no way to turn them on/off while running (yet), so just leave them off  
  if( FIREMODE_AUTO == Conf_FireMode ) {
    // turn off eyes
    eyesOff();
    setEyeStatus(EYES_OFF);
    
    // turn on ROF cap regardless of ROF register settings
    // full auto will use eyes off ROF cap (for now)
    Op_UseROFCap = true;
  }
  
  // do a little power on color burst like Tadao board does
  powerOnLEDBurst(OperatingMode);
}


// Main Program Loop
void loop(){
   if( MODE_FIRING == OperatingMode ) {
    firingMode();
    
    // blink occasionally to indicate that gun is on
    operatingLEDBlink();
  } else { 
    programmingMode();
  }
}



/****************************************** Misc. Functions ************************************/
byte getMaxRegisterValue(int regNum) {
  switch( regNum ) {
    case REGISTER_DEBOUNCE:      return REGISTER_DEBOUNCE_MAX;
    case REGISTER_DWELL:         return REGISTER_DWELL_MAX;
    case REGISTER_LOADER_DELAY:  return REGISTER_LOADER_DELAY_MAX;
    case REGISTER_MECH_DEBOUNCE: return REGISTER_MECH_DEBOUNCE_MAX;
    case REGISTER_FSDO_DWELL:    return REGISTER_FSDO_DWELL_MAX;
    case REGISTER_FIRE_MODE:     return REGISTER_FIRE_MODE_MAX;
    
    case REGISTER_ROF_ON:        return REGISTER_ROF_ON_MAX;
    case REGISTER_ROF_OFF:       return REGISTER_ROF_OFF_MAX;
    default: 
      DEBUG_PRINT("ERROR: getMaxRegisterValue(): unrecognized register: ");
      DEBUG_PRINTLN(regNum);
  }
}

// convert ROF setting to actual BPS value
// 1 = unlimited
// 2 = 8bps
// 3 = 8.25bps
// 4 = 8.5bps
// 5 = 8.75bps
// etc.
float convertROFValue(byte ROF) {
  if( ROF > 1 ) {
    ROF -= 2; // starts at 2, so 2 is 8.0  (setting of 19 = 12.25bps, 19 - 2 = 17)
    float actualROF = ROF / 4.0; // convert quarters into wholes (17 / 4.0 = 4.25)
    actualROF += 8; // and re-add the initial 8bps it starts at (4.25 + 8 = 12.25)
    
    return actualROF;
  } else {
    return 1.0;
  }
}


// this isn't really used anymore, but could be useful for debugging
#ifdef DEBUG_MODE
void printTriggerState(int state) {
  switch( state ) {
    case TRIGGER_STATE_WAITING: DEBUG_PRINTLN("Trigger=waiting"); break;
    case TRIGGER_STATE_DEBOUNCING: DEBUG_PRINTLN("Trigger=debouncing"); break;
    case TRIGGER_STATE_PULLED: DEBUG_PRINTLN("Trigger=pulled"); break;
    case TRIGGER_STATE_HELD: DEBUG_PRINTLN("Trigger=held"); break;
    case TRIGGER_STATE_RELEASED: DEBUG_PRINTLN("Trigger=released");break;
    default: 
      DEBUG_PRINT("Unknown trigger state: ");
      DEBUG_PRINTLN(state);
  } 
}
#endif
