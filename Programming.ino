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

Note:
This file needs no #includes because it has no file suffix (it's a .ino)
the Arduino IDE will automatically concat all non-suffixed files
into one code file before compiling, so this already has all includes

*/

void programmingMode() {
  // OperatingMode = MODE_PROGRAMMING
  
  // track whether we've turned on the LED yet
  int initialLED;
  
  // initial setup for programming mode
  if( Prog_WaitForTriggerRelease ) {
    TriggerState = getTriggerState();
    
    if( TRIGGER_STATE_RELEASED == TriggerState ||  TRIGGER_STATE_WAITING == TriggerState ) {
      Prog_WaitForTriggerRelease = false;
      DEBUG_PRINTLN("Trigger released, Entering programming mode...");
    }
    
    Prog_Mode = PROG_MODE_MENU;
    Prog_TriggerDownStart = 0;
    PriorTriggerState = TriggerState = 0;
    initialLED = false;
    
    // use specific debounce # for programming mode
    Debounce_MinTriggerDownCount = 5; //Prog_Mode_Debounce;
  }
 
  // don't attach this to the logic above with an else and waste a cycle
  // not that a cycle amounts to a whole lot, but...
  if( !Prog_WaitForTriggerRelease ) {
    // actual programming mode 
    
    // save off old trigger state and read in current trigger state
    PriorTriggerState = TriggerState;
    TriggerState = getTriggerState();
 
    if( PROG_MODE_MENU == Prog_Mode ) {
      // only do this once
      if( !initialLED ) {
        setLEDRegisterColor(Prog_CurrentRegister);
        initialLED = true;
      }
        
      // DEBUG_PRINTLN("Main Programming Menu");
      
      // NOTE: change this code to note when a NEW pull started (after the pull that turned on programming mode)
      // this will make it more like Tadao
      
      // first time trigger goes down track the MS so we can see how long the user pulled the trigger
      // >= 1s = enter register programming mode
      // < 1s = change register
      if( TRIGGER_STATE_PULLED == TriggerState || TRIGGER_STATE_HELD ==  TriggerState ) {
        if( 0 == Prog_TriggerDownStart ) {
          Prog_TriggerDownStart = millis();
        } else {
          
          // a little cheat here
          // still turn off the LED if trigger is held for longer than 1s and not released
          // to indicate to user that they have entered programming mode for that register
          if( (millis() - Prog_TriggerDownStart) > 1000 ) {
            ledOff();
          }
        }
        
      } else if( Prog_TriggerDownStart > 0 &&
          (TRIGGER_STATE_RELEASED == TriggerState ||  
          TRIGGER_STATE_WAITING == TriggerState ||
          TRIGGER_STATE_HELD == TriggerState)  ) {
            
        int currentMS = millis();
        int pullLength = currentMS - Prog_TriggerDownStart;
        Prog_TriggerDownStart = 0;
        
        //DEBUG_PRINT("trigger up, pull length = "); DEBUG_PRINTLN(pullLength); 
        
        if( pullLength < 1000 ) { 
          // move to next register
          Prog_CurrentRegister++;
          if( Prog_CurrentRegister >= REGISTER_COUNT ) Prog_CurrentRegister = REGISTER_DEBOUNCE;
          
          // short blink
          ledOff();
          // Note: add a delay here, then?
          
          DEBUG_PRINT("Changed to register ");DEBUG_PRINTLN(Prog_CurrentRegister);
        } else {
          DEBUG_PRINT("Programming register: ");DEBUG_PRINTLN(Prog_CurrentRegister);
          Prog_Mode = PROG_MODE_REGISTER;
          ledOff(); 
        }
      }
      else
      {
        // nothing here
      }
      
    } else {
      // Prog_Mode = PROG_MODE_REGISTER 
      DEBUG_PRINT("Programming Register ");DEBUG_PRINTLN(Prog_CurrentRegister);
      
      // wait for trigger release
      do {
        DEBUG_PRINTLN("waiting for trigger");
        delay(1);
      } while( getTriggerState() != TRIGGER_STATE_WAITING );
      
      // read in current value
      byte registerValue = EEPROM.read(Prog_CurrentRegister);
      
      // NOTE: temp cheat since some unset registers read 255
      if( registerValue > 100 ) registerValue = 10;
      
      DEBUG_PRINT("Register ");
      DEBUG_PRINT(Prog_CurrentRegister);
      DEBUG_PRINT(" value is ");
      DEBUG_PRINTLN(registerValue);
      
      // output current value to user
      for(int i = 0; i < registerValue; i++ ) {
        ledOff();
        delay(200);
        setLEDRegisterColor(Prog_CurrentRegister);
        delay(200);
      }
      
      ledOff();
      
      // Note: don't do this delay... it throws off the user. no way to notify them to wait. bad interface.
      //       leaving this note in here so that I remember
      //// pause before accepting input
      //delay(1000);
      
      // now read in new setting from trigger
      // this will time out after PROG_TRIGGER_TIMEOUT seconds
      int keepReadingTriggerPulls = true;
      int triggerDown = false;
      unsigned long triggerUpStart = millis();
      byte newValue = 0;
      byte maxValue = getMaxRegisterValue(Prog_CurrentRegister);
      int trigState;
      int priorTrigState;
      
      while ( keepReadingTriggerPulls ) {
        priorTrigState = trigState;
        trigState = getTriggerState();

        if( !triggerDown ) {
          if( TRIGGER_STATE_PULLED == trigState || TRIGGER_STATE_HELD == trigState ) {
            triggerDown = true;              
            newValue++;
            if( newValue > maxValue ) newValue = maxValue;
            if( newValue < 1 ) newValue = 1;
            
            DEBUG_PRINT("Increment register: "); DEBUG_PRINTLN(newValue);
 
            // Note: on second though, don't do this. it's annoying and wastes time
            // Note: this should be a #define item i nconfig.h
            bool blinkOutNewValueForEachIncrement = false;
            
            if( blinkOutNewValueForEachIncrement ) {
              // blink out new value
              progOutputRegisterValue(newValue, Prog_CurrentRegister);
            } else {
              // blink once
              setLEDRegisterColor(Prog_CurrentRegister);
              delay(250);
              ledOff();
            }
            
            // reset this here since the blinking takes up more than PROG_TRIGGER_TIMEOUT most likely
            triggerUpStart = millis();
          }
          
        } else {
          // triggerDown
          if( TRIGGER_STATE_RELEASED == trigState || (TRIGGER_STATE_WAITING == trigState && priorTrigState) ) {
            //DEBUG_PRINTLN("reset timeout");
            triggerUpStart = millis();
            triggerDown = false;
          }
        }
       
        if( (millis() - triggerUpStart) > PROG_TRIGGER_TIMEOUT ) {
          //DEBUG_PRINTLN("trigger input timeout");
          keepReadingTriggerPulls = false;
        }
      }
      
      if( newValue == 0 ) {
        // no value can be set to zero. if it is, then the user made no input, so make no change. 
        // this can't really occur, but have this here just in case
      } else {
        // save
        registerValue = newValue;
        EEPROM.write(Prog_CurrentRegister, registerValue);
        DEBUG_PRINT("Set new value for register "); DEBUG_PRINT(Prog_CurrentRegister); DEBUG_PRINT(" to "); DEBUG_PRINTLN(registerValue);
      }
      
      // Note: this should be a #define item in config.h
      bool blinkOutNewValueWhenFinished = true;
      
      if( blinkOutNewValueWhenFinished ) {
        // blink out new value
        progOutputRegisterValue(registerValue, Prog_CurrentRegister);
        
        // additional delay after we blink to differentiate between the blinked out value 
        // and the solid light of the color when we go back to the main menu
        ledOff();
        delay(500);
      }
      
      // testing, reset to main
      Prog_Mode = PROG_MODE_MENU;
      DEBUG_PRINTLN("Returning to main menu");
    }   
  }
}

void progOutputRegisterValue(byte value, int registerNum) {
  for(int i = 0; i < value; i++ ) {
    ledOff();
    delay(200);
    setLEDRegisterColor(registerNum);
    delay(200);
  }
}
