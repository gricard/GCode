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


Trigger status is checked in the main loop each cycle. 

The regular progression of trigger status is:

TRIGGER_STATE_WAITING - trigger circuit is open (TRIGGER_OPEN), waiting for a pull
TRIGGER_STATE_DEBOUNCING - detected circuit closure (TRIGGER_CLOSED), but still debouncing it 
TRIGGER_STATE_PULLED - debouncing has finished, trigger has been pulled (normally only one cycle spent like this)
TRIGGER_STATE_HELD - after one cycle where trigger state is pulled, it is switched to the held state
TRIGGER_STATE_RELEASED - trigger circuit is now open again (normally only one cycle spent like this)

*/


unsigned long getTriggerDownMS() {
  if( TRIGGER_STATE_PULLED == Trigger_State || TRIGGER_STATE_HELD == Trigger_State ) {
    return millis() - Trigger_PullStartMS;
  } else {
    return 0;
  }
}

int getTriggerState() {
    // read the state of the pushbutton value:
    int triggerReading = digitalRead(TRIGGER_PIN);
    
    if( TRIGGER_CLOSED == triggerReading ) {
      #ifdef DEBUG_TRIGGER_LOGIC
        DEBUG_PRINTLN("   HIGH ");
      #endif
      
      // trigger appears to be pulled
      if( Trigger_PriorReading == triggerReading ) {
        bool triggerDebounced = false;
        
        switch( Conf_DebounceMode ) {
          case DEBOUNCE_MODE_DELAY:
            triggerDebounced = debounceTriggerDelay();
            break;           
          case DEBOUNCE_MODE_DELAY_FINE:
            triggerDebounced = debounceTriggerDelayFine();
            break;
            
          default:
          case DEBOUNCE_MODE_SEQUENTIAL:
            triggerDebounced = debounceTriggerSequential();
            break;
        }
        
        if( triggerDebounced ) {
          // bouncing has stopped, now perform trigger pull action
          
          if( TRIGGER_STATE_DEBOUNCING == Trigger_State ) {
            Trigger_PriorState = Trigger_State;
            Trigger_State = TRIGGER_STATE_PULLED;

            // track this once, when we first note that the trigger is pulled
            Trigger_PullStartMS = millis();

            #ifdef DEBUG_TRIGGER_LOGIC
            DEBUG_PRINTLN("Trigger=pulled");
            DEBUG_PRINTLN("   debounce end...");
            #endif
          } else if( TRIGGER_STATE_PULLED == Trigger_State ) {
            // already registered the pull, but still getting trigger down signal
            Trigger_State = TRIGGER_STATE_HELD;
            
            #ifdef DEBUG_TRIGGER_LOGIC
            DEBUG_PRINTLN("Trigger=hold");
            #endif
          } else {
            // Trigger_State is still TRIGGER_STATE_HELD
            #ifdef DEBUG_TRIGGER_LOGIC
            DEBUG_PRINTLN("Trigger=holding");
            #endif
          }
          
        } else {
          // Trigger_State is still TRIGGER_STATE_DEBOUNCING
          
          #ifdef DEBUG_TRIGGER_LOGIC
          DEBUG_PRINTLN("   debounce...");
          #endif
        }
      } else {
        // trigger is down, start counting
        Trigger_State = TRIGGER_STATE_DEBOUNCING;
        Trigger_PriorReading = triggerReading;
        
        // track this for DEBOUNCE_MODE_SEQUENTIAL
        Debounce_PinSameStateCount = 1;
        
        #ifdef DEBUG_TRIGGER_LOGIC
        DEBUG_PRINTLN("   debounce start, count=1...");
        DEBUG_PRINTLN("Trigger=debouncing");
        #endif
      }
    } else { 
      // TRIGGER_OPEN == triggerReading
      
      if( TRIGGER_OPEN == triggerReading && TRIGGER_CLOSED == Trigger_PriorReading ) {
        // just let go of trigger
        Trigger_PriorState = Trigger_State;
        Trigger_State = TRIGGER_STATE_RELEASED;
        Trigger_PullStartMS = 0;
        
        #ifdef DEBUG_TRIGGER_LOGIC
        DEBUG_PRINTLN("Trigger=released");
        #endif
      } else {
        // switch to waiting immediately after release
        if( TRIGGER_STATE_RELEASED == Trigger_State ) {
          Trigger_State = TRIGGER_STATE_WAITING;
 
           #ifdef DEBUG_TRIGGER_LOGIC
          DEBUG_PRINTLN("Trigger=waiting");
          #endif
        }
      }
      
      // trigger let go, reset stats
      Trigger_PriorReading = triggerReading;
      Debounce_PinSameStateCount = 0;
    }
    
    return Trigger_State;
}

// this debounce method waits for Debounce_MinTriggerDownCount sequential TRIGGER_CLOSED reads
// you can be sure that after a reasonable number of idential sequential reads that the state is stable
// this is based on code from http://www.ganssle.com/debouncing-pt2.htm
bool debounceTriggerSequential() {
  // increment counter and keep checking
  Debounce_PinSameStateCount++;
        
  if( Debounce_PinSameStateCount >= Debounce_MinTriggerDownCount ) {
    return true;
  }
  
  return false;
}

// this is a generic debounce method that simply waits for a short 
// period of time for the switch to stop bouncing. 
bool debounceTriggerDelay() {
  // delay Debounce_DelayTime ms and return true
  delay(Debounce_DelayTime);
  return true;
}

// this is a generic debounce method that simply waits for a short 
// period of time for the switch to stop bouncing. 
// this version delays in smaller increments
bool debounceTriggerDelayFine() {
  // delay Debounce_DelayTime * 100 ms and return true
  delayMicroseconds(Debounce_DelayTime * 100);
  return true;
}
