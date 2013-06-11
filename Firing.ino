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
    
need to separate firing logic into separate units:
- trigger input
- eye input
  - blink eye TX while trigger state is released, waiting or debouncing 
  - make it solid when trigger state is pulled or held
- ROF filtering 
- shot firing

*/

void updateEyeState() {
  byte priorEyeState = GET_EYE_READ_STATE();
  
  // NOTE: need to make constants for the results of this so it's more obvious, or change var name
  SET_EYE_READ_STATE(readEyeState()); // 1 = no ball (can see beam), 0 = ball (can't see beam)
  
  if( priorEyeState != GET_EYE_READ_STATE() ) {
    DEBUG_PRINT("eye1: " );DEBUG_PRINTLN(GET_EYE_READ_STATE());
  }
  
  // clear blocked eye status
  // if eyes are turned on, and were previously blocked, and now we can see a ball
  if( EYES_OFF != Op_EyeStatus && Op_EyesBlocked && GET_EYE_READ_STATE() ) { 
    Op_EyesBlocked = false;
    setEyeStatus(EYES_ON);
    DEBUG_PRINTLN("   Eyes cleared");
  }
}


void firingMode() {    
  bool ShotWasFired = false;

  // need this to check for held trigger for turning eyes off
  PriorTriggerState = TriggerState;
  TriggerState = getTriggerState();
  
  updateEyeState();
  
  if( FIREMODE_AUTO == Conf_FireMode ) {
    if( TRIGGER_STATE_PULLED == TriggerState || TRIGGER_STATE_HELD == TriggerState ) {
      //pullCount++;
      //DEBUG_PRINT("Pull count: ");
      //DEBUG_PRINTLN(pullCount);
      Op_FireShot = true;
    }
  }
  else // semi-auto or ramp
  {
    if( TRIGGER_STATE_HELD == TriggerState ) {
      unsigned long ms = getTriggerDownMS();
    
      if( ms >= OP_EYES_OFF_TRIGGER_TIME ) {
        // only do this if we haven't already changed the state
        if( !Op_ManualEyeStateChange ) {
          if( EYES_ON == Op_EyeStatus ) {
            DEBUG_PRINTLN("   Eyes manually turned off");
            ledOff();
            setEyeStatus(EYES_OFF);
            Op_ManualEyeStateChange = true;
          } else {
            DEBUG_PRINTLN("   Eyes manually turned on");
            setEyeStatus(EYES_ON);
            Op_ManualEyeStateChange = true;
          
            // reset block state
            Op_EyesBlocked = false;
          }
        }
      } else if( ms >= OP_FORCE_SHOT_TRIGGER_TIME ) {
        // make sure we didn't do this already
        if( !Op_ShotWasForced ) {
          DEBUG_PRINTLN("Forced shot");
          Op_ForceShot = true;
        }
      }
    }

    if( TRIGGER_STATE_PULLED == TriggerState ) {
      pullCount++;
      DEBUG_PRINT("Pull count: ");
      DEBUG_PRINTLN(pullCount);
      Op_FireShot = true;
      Op_LastPullMS = Op_CurPullMS;
      Op_CurPullMS = millis();
      Op_TriggerWasPulled = true;
    } else {
      Op_TriggerWasPulled = false;
    }

/*
PSP Rule book 2013:
5.3.6.2. May fire no more than three shots per press and release of the trigger, 
and no more than three shots between presses of the trigger.[50] ...
[50] Shots may not be buffered past trigger pulls - if you pull the trigger, 
the marker fires, and you pull the trigger again, the marker may only fire 
three more times, not five.
*/
        
/* PSP Rulebook:
5.3.6.3. May fire no more than one shot per press and release of the trigger unless
5.3.6.3.1. The trigger has been pressed and released four times, [51] and
5.3.6.3.2. The trigger has been pressed and released at least once per second since the first of the four trigger pulls.
*/    
    if( FIREMODE_RAMP == Conf_FireMode ) {
      if( !Op_RampStarted ) {
        // not currently ramping 
        
        // if we start firing shots, then figure out if we're firing fast enough to enter ramp
        if( Op_FireShot ) {
          // must maintain 1bps for first RAMP_START_MIN_SHOTS shot
          unsigned long nextPullMinMS = Op_LastPullMS + RAMP_INACTIVE_TIME_BETWEEN_PULLS;

          if( Op_CurPullMS <= nextPullMinMS ) {
            DEBUG_PRINT("Ramp init shot "); DEBUG_PRINTLN(Op_RampStartShotCount);
            Op_RampStartShotCount++;
          } else {
            // reset if we aren't meeting RAMP_MIN_BPS
            DEBUG_PRINTLN("Ramp init reset ");
            Op_RampStartShotCount = 0;
          }
          
          if( Op_RampStartShotCount >= RAMP_START_MIN_SHOTS ) {
            DEBUG_PRINTLN("Ramp started");
            Op_RampStarted = true;
            
            // we're taking one shot now, so queue two more shots
            // since we can only have three shots per pull/release
            // and do the same below when we reset the shot queue
            Op_RampShotsLeft = 2;
          }
        }

      } else { // Op_RampStarted
        unsigned long nextPullMinMS = Op_LastPullMS + RAMP_ACTIVE_TIME_BETWEEN_PULLS;

        // reset shot queue on pull
        if( Op_TriggerWasPulled ) {
          DEBUG_PRINTLN("Ramp shot queue reset");
          Op_RampShotsLeft = 2;
        }        
        
        // keep shooting if we're maintaining proper pulls per second
        if( Op_CurPullMS <= nextPullMinMS ) {
          // maintaining min. pulls per second
          // tell board to fire, ROF code will handle rate
          Op_FireShot = true;
          
          //DEBUG_PRINTLN("Ramp shot");
          
          handleQueuedRampShots();
        } else {
          // not maintaining min. pulls per second to keep ramp active
          // handle any remaining queued shots
          handleQueuedRampShots();
        }    
      }
    }  // FIREMODE_RAMP
    
  } // fire mode


  if( Op_FireShot && EYES_ON == Op_EyeStatus && Op_EyesBlocked  ) {
    DEBUG_PRINTLN("Eyes blocked");
    setEyeStatus(EYES_BLOCKED);
    DEBUG_PRINT("EYE STATUS: ");DEBUG_PRINTLN(Op_EyeStatus);
    
    // do NOT cancel the shot for an eye block
    // we automatically switch over to eyes off mode and lower the ROF anyway
    // so just let the shot occur
    //Op_FireShot = false;
  }

  // regular, unforced shot - check eyes
  if( Op_FireShot && !Op_ForceShot ) {
    if( EYES_ON == Op_EyeStatus && GET_EYE_READ_STATE() ) {
      DEBUG_PRINTLN("     No ball, no shot");
      Op_FireShot = false;
    } else {
      DEBUG_PRINTLN("     Yes ball, shoot");
      Op_FireShot = true;
    }
  }

  // a shot has been requested
  if( Op_FireShot || Op_ForceShot ) {
    // "Take the shot" - M
    bool takeTheShot = true;
    
    // figure out current rate of fire
    int msBetweenShots = 0;
    float curROFLimit = (EYES_ON == Op_EyeStatus ? Op_ROFEyesOn : Op_ROFEyesOff);
    
    // ROF limit of 1 is uncapped
    if( curROFLimit > 1  ) {
      msBetweenShots = 1000 / curROFLimit;
    } else {
      msBetweenShots = 0;
    }
    
    unsigned long nextShotMinMS = Op_LastShotMS + msBetweenShots;
    unsigned long ms = millis();
    
    // ROF delay
    if( Op_UseROFCap && Op_LastShotMS > 0 ) {
      // using rof cap, and we've already had the first shot go, so track timing and wait if needed  
      if( ms < nextShotMinMS ) {
        // it's too soon to take another shot
        takeTheShot = false;
      }
      
      //DEBUG_PRINT("ROF Limit=");DEBUG_PRINT(curROFLimit);
      //DEBUG_PRINT(" ms=");DEBUG_PRINT(msBetweenShots);
      //DEBUG_PRINT(" now=");DEBUG_PRINT(ms);
      //DEBUG_PRINT(" next=");DEBUG_PRINT(nextShotMinMS);
      //DEBUG_PRINT(" take=");DEBUG_PRINTLN(takeTheShot);
    }
    
    if( !takeTheShot ) {
      //DEBUG_PRINTLN("     ROF delay");
      Op_LastShotTaken = false;
    } else {
      Op_LastShotTaken = true;
      
      // loader delay
      if( Conf_LoaderDelay > 0 ) {
        delay(Conf_LoaderDelay); // setting of 1 is 0ms, so subtract one from register value
        //DEBUG_PRINT("     Loader Delay MS= ");DEBUG_PRINTLN(Conf_LoaderDelay - 1);
      }
        
      // mech debounce
      int fireRateOver8BPS = false;
      int curROF = 0;
      unsigned long timeSinceLastShot = 0;
      
      if( Op_LastShotMS > 0 ) {
        timeSinceLastShot = ms - Op_LastShotMS;
        curROF = 1000 / timeSinceLastShot;
        fireRateOver8BPS = (curROF > 8);
        DEBUG_PRINT("CurROF = ");DEBUG_PRINTLN(curROF);
      } else {
        curROF = 0;
        fireRateOver8BPS = false;
        DEBUG_PRINTLN("First Shot");
      }
      
      // mech debounce method is based on the CodeX open source board software
      // https://code.google.com/p/paintballcodex/
      if( !fireRateOver8BPS && Conf_MechDebounce > 0 ) {
        delay(Conf_MechDebounce);
        //DEBUG_PRINT("     Mech Debounce MS= ");DEBUG_PRINTLN(Conf_MechDebounce - 1);
      }
        
      // reset dwell to default
      Op_Dwell = Conf_Dwell;
        
      // modify dwell on first shot for FSDO dwell
      if( Op_FirstShot ) {
        Op_FirstShot = false;
          
        // FSDO dwell
        if( Conf_FSDODwell > 0 ) {
          //DEBUG_PRINT("     FSDO Dwell MS= ");DEBUG_PRINTLN(Conf_FSDODwell - 1);
          Op_Dwell += (Conf_FSDODwell);
        }
      }


      // blink RGBLED for each shot
      /*
      if( Op_EyesBlocked ) {
        ledColor(LED_EYES_BLOCKED, 25);
      } else {
        ledColor(LED_EYES_ON, 25);
      }
      */
      switch( Op_EyeStatus ) {
        case EYES_ON: ledColor(LED_EYES_ON, 25); break;
        case EYES_OFF: ledColor(LED_EYES_OFF, 25); break;
        case EYES_BLOCKED: ledColor(LED_EYES_BLOCKED, 25); break;
        default: ledColor(LED_WHITE, 25);
      }

      // and now actually fire the shot
      fireSolenoid(Op_Dwell);
      
      // mark this so that trigger release code doesn't turn off LED
      ShotWasFired = true;

      // blink off
      ledOff();
      
      // track when the last shot occurred
      Op_LastShotMS = millis();
        
      // set this after the shot, then after it goes back through the loop and the eye code updates they'll be unblocked
      Op_EyesBlocked = true;
      
      // reset this
      Op_FireShot = false;
      Op_ForceShot = false;
      
      // set this to true so it will be reset to false by the code that checks for trigger release
      Op_ShotWasForced = true;
    }
  }

  // when trigger is let go, reset a few things
  if( TRIGGER_STATE_RELEASED == TriggerState || TRIGGER_STATE_WAITING == TriggerState ) {
    
    // shot is fired, trigger is let go, turn off the led
    // only do this on release, otherwise it prematurely cuts off operatingLEDBlink();
    if( TRIGGER_STATE_RELEASED == TriggerState && !ShotWasFired && !Op_EyeBlinkSolid ) {
      ledOff();
    }
    
    // reset these here 
    Op_ManualEyeStateChange = false;
    Op_ShotWasForced = false;
  }

  // bottom of the loop, update generic timer
  // do this here since we don't know how much time we spent in this loop
  operationTiming = millis();
}

void fireSolenoid(int dwell) {
  digitalWrite(SOLENOID_PIN, HIGH);    
  delay(dwell);
  digitalWrite(SOLENOID_PIN, LOW);
  DEBUG_PRINT("Fire! Dwell="); DEBUG_PRINTLN(dwell);
}

void handleQueuedRampShots() {
  // only handle this when we actually made a shot on the previous attempt 
  // (in case of ROF limit)
  if( Op_LastShotTaken ) {
    // the last shot attempt was successful, so decrement the queue
    if( Op_RampShotsLeft > 0 ) {
      Op_FireShot = true;
      DEBUG_PRINT("Ramp queued shot: ");DEBUG_PRINTLN(Op_RampShotsLeft);
      Op_RampShotsLeft--;
    } else {
      // BPS rate dropped, switch out of ramp
      Op_RampStartShotCount = 0;
      Op_RampStarted = false;
      Op_FireShot = false;
      DEBUG_PRINTLN("Ramp stopped");
    }
  }
}
