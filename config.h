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


#ifndef CONFIG_H
#define CONFIG_H

// Arduino IDE only seems to handle byte data type properly in main .ino file(s)
// which have the main Arduino.h automatically included, so manually include this here
#include "Arduino.h"


//-- Pin Configuration --------------------------------------------------------------------
#define TRIGGER_PIN           2
#define EYE_TX_PIN            3
#define EYE_RX_PIN            4
#define SOLENOID_PIN          5
#define RGBLED_PIN_RED        9
#define RGBLED_PIN_GREEN      10
#define RGBLED_PIN_BLUE       11
// Note: the RGB LED pins need to support PWM

//-- Programming Register Configuration ---------------------------------------------------
// Register default values
#define DEFAULT_DEBOUNCE               3
#define DEFAULT_DWELL                  8 // 8 sequential blinks, or 8ms 
#define DEFAULT_LOADER_DELAY           1 // off, 2 = 1ms
#define DEFAULT_MECH_DEBOUNCE          1 // off, 2 = 1
#define DEFAULT_FSDO_DWELL             1 // off , 2 = 1ms
#define DEFAULT_FIRE_MODE              1 // semi
#define DEFAULT_ROF_EYES_ON_INT        ROF_UNCAPPED // no cap
#define DEFAULT_ROF_EYES_ON_FRAC       0 // no cap
#define DEFAULT_ROF_EYES_OFF_INT       8 // 8.0bps, integral part of rof cap
#define DEFAULT_ROF_EYES_OFF_FRAC      1 // 8.0bps, fractional part of rof cap (values start at 1 = 0.0, 2 = 0.1, etc.)

// Register max values
#define REGISTER_DEBOUNCE_MAX          10
#define REGISTER_DWELL_MAX             30
#define REGISTER_LOADER_DELAY_MAX      10
#define REGISTER_MECH_DEBOUNCE_MAX     10
#define REGISTER_FSDO_DWELL_MAX        5
#define REGISTER_FIRE_MODE_MAX         3 // Note: change this when you add new modes
#define REGISTER_ROF_ON_INT_MAX        25 // integral part of rof cap
#define REGISTER_ROF_ON_FRAC_MAX       10 // value - 1 = fraction (FRACtion of 10 = .9 , FRAC of 1 = .0)
#define REGISTER_ROF_OFF_INT_MAX       12
#define REGISTER_ROF_OFF_FRAC_MAX      10


//-- Rate of Fire Limiting Configuration --------------------------------------------------
#define ROF_MAX               25 // This should be a reasonable value
#define ROF_UNCAPPED          1 


//-- Rate of Fire Limiting Configuration --------------------------------------------------
#define RAMP_START_MIN_SHOTS                   3
#define RAMP_MIN_BPS                           4
// # of ms allowed between pulls to maintain ramp
#define RAMP_INACTIVE_TIME_BETWEEN_PULLS       1000 
// # of ms allowed between pulls to start ramp
#define RAMP_ACTIVE_TIME_BETWEEN_PULLS         1000 / RAMP_MIN_BPS 

//-- Debounce Configuration ---------------------------------------------------------------
// only set one of these
#define DEBOUNCE_MODE_SEQUENTIAL       true
#define DEBOUNCE_MODE_DELAY            false


//-- Eye Configuration --------------------------------------------------------------------
#define OP_EYES_OFF_TRIGGER_TIME         2000
#define OP_FORCE_SHOT_TRIGGER_TIME       500
#define EYES_ON_LED_COLOR                LED_RED
#define EYES_OFF_LED_COLOR               LED_YELLOW
// Note: these are duped! LED_EYES_ON = EYES_ON_LED_COLOR


//-- LED Configuration ------------------------------------------------------------
#define LED_EYES_ON            LED_BLUE
#define LED_EYES_OFF           LED_RED
#define LED_EYES_BLOCKED       LED_YELLOW

#define LED_DEFAULT_BRIGHTNESS       50
#define LDB                          LED_DEFAULT_BRIGHTNESS

// these control the blinking of the RGB LED to indicate operational status
#define BLINK_INTERVAL_ON       100
#define BLINK_INTERVAL_OFF      1000
#define BLINK_BRIGHTNESS        15


//-- RGB LED Colors -----------------------------------------------------------------------
const byte LED_RED[]       = { 255, 0, 0 };
const byte LED_GREEN[]     = { 0, 255, 0 };
const byte LED_BLUE[]      = { 0, 0, 255 };
const byte LED_ORANGE[]    = { 175, 75, 0 };
const byte LED_YELLOW[]    = { 255, 255, 0 };
const byte LED_PURPLE[]    = { 150, 0, 150 };
const byte LED_WHITE[]     = { 255, 255, 255 };
const byte LED_TEAL[]      = {  0, 128, 128 };
const byte LED_PINK[]      = {  255, 120, 185 };
const byte LED_LIGHTBLUE[] = {  160, 208, 252 };
const byte LED_BLACK[]     = {  0, 0, 0 };


//-- Trigger Configuration ----------------------------------------------------------------
// don't change these unless you physically alter the trigger circuit
#define TRIGGER_CLOSED     LOW
#define TRIGGER_OPEN       HIGH



// -------- CONSTANTS ---------------------------------------------------------------------
// These should not be changed at all

//// Eyes
#define EYES_ON                   1
#define EYES_OFF                  0
#define EYES_BLOCKED              2

//// Trigger
#define TRIGGER_STATE_WAITING     0
#define TRIGGER_STATE_DEBOUNCING  1
#define TRIGGER_STATE_PULLED      2
#define TRIGGER_STATE_HELD        3
#define TRIGGER_STATE_RELEASED    4

//// Fire modes
#define FIREMODE_SEMI             1
#define FIREMODE_AUTO             2
#define FIREMODE_RAMP             3

//// Operating Modes
#define MODE_PROGRAMMING          0
#define MODE_FIRING               1

// EEPROM register numbers
#define REGISTER_DEBOUNCE         0
#define REGISTER_DWELL            1
#define REGISTER_LOADER_DELAY     2
#define REGISTER_MECH_DEBOUNCE    3
#define REGISTER_FSDO_DWELL       4
#define REGISTER_FIRE_MODE        5
#define REGISTER_ROF_ON_INT       6
#define REGISTER_ROF_ON_FRAC      7
#define REGISTER_ROF_OFF_INT      8
#define REGISTER_ROF_OFF_FRAC     9

// uncomment the line below if you want to allow the user to 
// configure the ROF for when the eyes are off
// Note: be sure you have a reasonable value set for 
//#define ALLOW_CONFIGURABLE_EYES_OFF_ROF

// Total # of registers we're using
// Make sure this is updated if new programming registers are added!
#ifdef ALLOW_CONFIGURABLE_EYES_OFF_ROF
#define REGISTER_COUNT            10
#else
#define REGISTER_COUNT            8
#endif



#endif
