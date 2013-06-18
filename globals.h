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
*/


#ifndef GLOBALS_H
#define GLOBALS_H

// Arduino IDE only seems to handle byte data type properly in main .ino file(s)
// which have the main Arduino.h automatically included, so manually include this here
#include "Arduino.h"

// needed for firing modes
#include "config.h"

// Wrap all global vars in macros so that we can change the get/set methods easily
// Note: don't set the define below. this code is not implemented yet.
// global var access wrappers
#undef GLOBALS_USE_FUNC_CALLS

#ifdef GLOBALS_USE_FUNC_CALLS
  #define SET_PRIOR_EYE_READ_STATE(x) setPriorEyeReadState(x)
  #define GET_PRIOR_EYE_READ_STATE() getPriorEyeReadState()
  #define SET_EYE_READ_STATE(x) setEyeReadState(x)
  #define GET_EYE_READ_STATE() getEyeReadState()
#else
  #define SET_PRIOR_EYE_READ_STATE(x) Op_PriorEyeReadState = x
  #define GET_PRIOR_EYE_READ_STATE() Op_PriorEyeReadState 
  #define SET_EYE_READ_STATE(x) Op_EyeReadState = x
  #define GET_EYE_READ_STATE() Op_EyeReadState 
#endif

// used by both programming mode and trigger mode
byte PriorTriggerState = 0;
byte TriggerState = 0;


// Programming modes
#define PROG_MODE_MENU 0 
#define PROG_MODE_REGISTER 1
#define PROG_TRIGGER_TIMEOUT 3000               // when programming a register, timeout after 3 seconds 
byte Prog_Mode = PROG_MODE_MENU; 
byte Prog_CurrentRegister = FIRST_REGISTER;   // register we're currently programming, starts at 
unsigned long Prog_RegisterTimeoutStart = 0;    // # of ms since we started waiting for input to program a register
unsigned long Prog_TriggerDownStart = 0;
byte Prog_Mode_Debounce = 8;
bool Prog_WaitForTriggerRelease = false;
bool Prog_InitialLED = false;
byte Prog_LastLEDBlinkColor = 0;
unsigned long Prog_LastLEDBlinkMS = 0;
unsigned long Prog_ResetWarningBlinkMS = 0;
byte Prog_ResetWarningLastState = 0;


//// Operating mode
byte Op_PriorEyeReadState = 0;
byte Op_EyeReadState = 0;
byte Op_EyeStatus = EYES_OFF;
byte Op_Dwell = 0;
bool Op_FirstShot = true;
bool Op_EyesBlocked = false;
bool Op_ManualEyeStateChange = false; 
bool Op_FireShot = false;
bool Op_ForceShot = false;
bool Op_ShotWasForced = false;
byte Op_GunMode = GUNMODE_OPEN; // default to open bolt mode

bool Op_UseROFCap = true;
unsigned long Op_LastShotMS = 0;
bool Op_TriggerWasPulled = false;

// ramping items
byte Op_RampStarted = false;
byte Op_RampStartShotCount = 0;
byte Op_RampShotsLeft = 3; // number of queued shots left to take
unsigned long Op_LastPullMS = 0; // last trigger pull timestamp
unsigned long Op_CurPullMS = 0; // current trigger pull timestamp
unsigned long Op_RampLastPullMS = 0;
bool Op_LastShotTaken = false;

// combined final value for ROF settings
float Op_ROFEyesOn;
float Op_ROFEyesOff;


//// Firing mode setup
unsigned long operationTiming = 0; // generic time tracker for firing mode (used to do LED blinking to indicate on and ready)
int pullCount = 0; // NOTE: testing

//// General operating mode
byte OperatingMode = MODE_FIRING;


byte EyeMode = EYES_ON; // Note: may not need this
unsigned long LastEyeBlink = 0;
bool LastEyeBlinkOn = false;
bool Op_EyeBlinkSolid = LED_DISABLE_BLINKING;

// operating variables
byte Conf_Debounce;
byte Conf_Dwell;
byte Conf_LoaderDelay;
byte Conf_MechDebounce;
byte Conf_FSDODwell;
byte Conf_FireMode;
byte Conf_ROFEyesOnInt;
byte Conf_ROFEyesOnFrac;
byte Conf_ROFEyesOffInt;
byte Conf_ROFEyesOffFrac;
byte Conf_ClosedBoltDwell;
byte Conf_ClosedBoltEyeDelay;
byte Conf_ClosedBoltBoltDelay;
byte Conf_ROFOnOff;
byte Conf_BoardMode;


byte Trigger_PinState = -1; // for the actual pin state (HIGH/LOW)
byte Trigger_State = -1; // store the effective state of the trigger here
byte Trigger_PriorState = -1; // last effective trigger state
byte Trigger_PriorReading = -1; // last reading on trigger pin
// each time a pull is registered, track the time it was pulled at so we can
// use that to run code for long pulls (like shutting off eyes)
unsigned long Trigger_PullStartMS = 0; 

//// Debounce items
byte Debounce_MinTriggerDownCount = 8; // min # times in a row trigger pin must read as down before being considered a pull, replaced by Conf_Debounce
byte Debounce_PinSameStateCount = 0;
byte Debounce_PinState = 0;
byte Debounce_DelayTime = 0;

#endif
