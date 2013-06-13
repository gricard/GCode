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

Please read the README.txt

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

  // initialize the solenoid pin(s) as an output:
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(SOLENOID2_PIN, OUTPUT);
  
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

  // reset programming if program version was updated
  byte programVersion = EEPROM.read(REGISTER_VERSION);

  if( programVersion != PROGRAM_VERSION ) {
    resetProgrammingToDefault();
  }

  // read in programming register items
  Conf_Debounce = EEPROM.read(REGISTER_DEBOUNCE);
  Conf_Dwell = EEPROM.read(REGISTER_DWELL);
  Conf_LoaderDelay = EEPROM.read(REGISTER_LOADER_DELAY);
  Conf_MechDebounce = EEPROM.read(REGISTER_MECH_DEBOUNCE);
  Conf_FSDODwell = EEPROM.read(REGISTER_FSDO_DWELL);
  Conf_FireMode = EEPROM.read(REGISTER_FIRE_MODE);
  Conf_ROFEyesOnInt = EEPROM.read(REGISTER_ROF_ON_INT);  
  Conf_ROFEyesOnFrac = EEPROM.read(REGISTER_ROF_ON_FRAC);
  Conf_ClosedBoltDwell = EEPROM.read(REGISTER_CLOSED_DWELL);
  Conf_ClosedBoltEyeDelay = EEPROM.read(REGISTER_CLOSED_EYE_DELAY);
  Conf_ClosedBoltBoltDelay = EEPROM.read(REGISTER_CLOSED_BOLT_DELAY);
 
  #ifdef ALLOW_CONFIGURABLE_EYES_OFF_ROF
  Conf_ROFEyesOffInt = EEPROM.read(REGISTER_ROF_OFF_INT);
  Conf_ROFEyesOffFrac = EEPROM.read(REGISTER_ROF_OFF_FRAC);
  #else 
  // default to 8.0 bps
  Conf_ROFEyesOffInt = 8;
  Conf_ROFEyesOffFrac = 1;
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
  if( Conf_ROFEyesOnInt < 1 || Conf_ROFEyesOnInt > REGISTER_ROF_ON_INT_MAX ) Conf_ROFEyesOnInt = DEFAULT_ROF_EYES_ON_INT;
  if( Conf_ROFEyesOnFrac < 1 || Conf_ROFEyesOnFrac > REGISTER_ROF_ON_FRAC_MAX ) Conf_ROFEyesOnFrac = DEFAULT_ROF_EYES_ON_FRAC;
  if( Conf_ClosedBoltDwell < 1 || Conf_ClosedBoltDwell > REGISTER_CLOSED_DWELL_MAX ) Conf_ClosedBoltDwell = DEFAULT_CLOSED_DWELL;
  if( Conf_ClosedBoltEyeDelay < 1 || Conf_ClosedBoltEyeDelay > REGISTER_CLOSED_EYE_DELAY_MAX ) Conf_ClosedBoltEyeDelay = DEFAULT_CLOSED_EYE_DELAY;
  if( Conf_ClosedBoltBoltDelay < 1 || Conf_ClosedBoltBoltDelay > REGISTER_CLOSED_BOLT_DELAY_MAX ) Conf_ClosedBoltBoltDelay = DEFAULT_CLOSED_BOLT_DELAY;
  
  #ifdef ALLOW_CONFIGURABLE_EYES_OFF_ROF
  if( Conf_ROFEyesOffInt < 1 || Conf_ROFEyesOffInt > REGISTER_ROF_OFF_INT_MAX ) Conf_ROFEyesOffInt = DEFAULT_ROF_EYES_OFF_INT;
  if( Conf_ROFEyesOffFrac < 1 || Conf_ROFEyesOffFrac > REGISTER_ROF_OFF_FRAC_MAX ) Conf_ROFEyesOffFrac = DEFAULT_ROF_EYES_OFF_FRAC;
  #endif
  
  // debug print the corrected values
  DEBUG_PRINT("   Debounce=");DEBUG_PRINTLN(Conf_Debounce);
  DEBUG_PRINT("   Dwell=");DEBUG_PRINTLN(Conf_Dwell);
  DEBUG_PRINT("   LoaderDelay=");DEBUG_PRINTLN(Conf_LoaderDelay);
  DEBUG_PRINT("   MechDebounce=");DEBUG_PRINTLN(Conf_MechDebounce);
  DEBUG_PRINT("   FSDODwell=");DEBUG_PRINTLN(Conf_FSDODwell);
  DEBUG_PRINT("   FireMode=");DEBUG_PRINTLN(Conf_FireMode);
  DEBUG_PRINT("   ROFEyesOnInt=");DEBUG_PRINTLN(Conf_ROFEyesOnInt);
  DEBUG_PRINT("   ROFEyesOnFrac=");DEBUG_PRINTLN(Conf_ROFEyesOnFrac);
  DEBUG_PRINT("   ROFEyesOffInt=");DEBUG_PRINTLN(Conf_ROFEyesOffInt);
  DEBUG_PRINT("   ROFEyesOffFrac=");DEBUG_PRINTLN(Conf_ROFEyesOffFrac);
  DEBUG_PRINT("   Conf_ClosedBoltDwell=");DEBUG_PRINTLN(Conf_ClosedBoltDwell);
  DEBUG_PRINT("   Conf_ClosedBoltEyeDelay=");DEBUG_PRINTLN(Conf_ClosedBoltEyeDelay);
  DEBUG_PRINT("   Conf_ClosedBoltBoltDelay=");DEBUG_PRINTLN(Conf_ClosedBoltBoltDelay);
  
  // Convert register values to usable program values
  // setting of 1 is 0ms, so subtract one from register value
  Conf_LoaderDelay -= 1;
  Conf_MechDebounce -= 1;
  Conf_FSDODwell -= 1;
  
  // convert ROF register values to numerical BPS value
  Op_ROFEyesOn = convertROFValue(Conf_ROFEyesOnInt, Conf_ROFEyesOnFrac);
  DEBUG_PRINT("Eyes on ROF = ");DEBUG_PRINT(Op_ROFEyesOn);DEBUG_PRINTLN("bps");
  Op_ROFEyesOff = convertROFValue(Conf_ROFEyesOffInt, Conf_ROFEyesOffFrac);
  DEBUG_PRINT("Eyes off ROF = ");DEBUG_PRINT(Op_ROFEyesOff);DEBUG_PRINTLN("bps");

  // If closed bolt dwell setting is set, change to closed bolt mode
  if( Conf_ClosedBoltDwell > 1 ) {
    Op_GunMode = GUNMODE_CLOSED;
    DEBUG_PRINTLN("Closed bolt mode enabled");
  }

  // set initial dwell
  // may be modified by FSDO dwell
  Op_Dwell = Conf_Dwell;

  // set debounce
  Debounce_MinTriggerDownCount = Conf_Debounce; // for sequential read debounce
  Debounce_DelayTime = Conf_Debounce; // for delay debounce
  
  // setup ROF cap
  Op_UseROFCap = (Op_ROFEyesOn > 1);

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
  
  if( MODE_PROGRAMMING == OperatingMode ) {
    // track pull time so we can reset the board if held for a certain length of time
    Prog_TriggerDownStart = millis();
  }
  
  // start off generic timer
  // want this as close to start of main loop as possible
  operationTiming = millis();
  LastEyeBlinkOn = false;
  LastEyeBlink = operationTiming;
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
    case REGISTER_DEBOUNCE:            return REGISTER_DEBOUNCE_MAX;
    case REGISTER_DWELL:               return REGISTER_DWELL_MAX;
    case REGISTER_LOADER_DELAY:        return REGISTER_LOADER_DELAY_MAX;
    case REGISTER_MECH_DEBOUNCE:       return REGISTER_MECH_DEBOUNCE_MAX;
    case REGISTER_FSDO_DWELL:          return REGISTER_FSDO_DWELL_MAX;
    case REGISTER_FIRE_MODE:           return REGISTER_FIRE_MODE_MAX;
    
    case REGISTER_CLOSED_DWELL:        return REGISTER_CLOSED_DWELL_MAX;
    case REGISTER_CLOSED_EYE_DELAY:    return REGISTER_CLOSED_EYE_DELAY_MAX;
    case REGISTER_CLOSED_BOLT_DELAY:   return REGISTER_CLOSED_BOLT_DELAY_MAX;

    
    case REGISTER_ROF_ON_INT:          return REGISTER_ROF_ON_INT_MAX;
    case REGISTER_ROF_ON_FRAC:         return REGISTER_ROF_ON_FRAC_MAX;
    case REGISTER_ROF_OFF_INT:         return REGISTER_ROF_OFF_INT_MAX;
    case REGISTER_ROF_OFF_FRAC:        return REGISTER_ROF_OFF_FRAC_MAX;
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
float convertROFValueOld(byte ROF) {
  if( ROF > 1 ) {
    ROF -= 2; // starts at 2, so 2 is 8.0  (setting of 19 = 12.25bps, 19 - 2 = 17)
    float actualROF = ROF / 4.0; // convert quarters into wholes (17 / 4.0 = 4.25)
    actualROF += 8; // and re-add the initial 8bps it starts at (4.25 + 8 = 12.25)
    
    return actualROF;
  } else {
    return 1.0;
  }
}

float convertROFValue(byte rofInt, byte rofFrac) {
  if( rofInt > 1 ) {
    float rof = rofInt;
    
    // fractional value is 1-11, 1 = 0.0, 2 = 0.1, etc.
    // subtract the 1 from the value and divide by ten to get the fractional value to add to rof
    rof += ((rofFrac - 1) / 10.0);
    
    return rof;
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
