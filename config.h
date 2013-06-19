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
#define SOLENOID2_PIN         6
#define RGBLED_PIN_RED        9
#define RGBLED_PIN_GREEN      10
#define RGBLED_PIN_BLUE       11
// Note: the RGB LED pins need to support PWM

//-- Programming Register Configuration ---------------------------------------------------
// Register default values
#define DEFAULT_DEBOUNCE               5
#define DEFAULT_DWELL                  10 // 10 sequential blinks, or 10ms 
#define DEFAULT_LOADER_DELAY           1 // off, 2 = 1ms
#define DEFAULT_MECH_DEBOUNCE          1 // off, 2 = 1
#define DEFAULT_FSDO_DWELL             1 // off , 2 = 1ms
#define DEFAULT_FIRE_MODE              1 // semi
#define DEFAULT_ROF_EYES_ON_INT        ROF_UNCAPPED // no cap
#define DEFAULT_ROF_EYES_ON_FRAC       0 // no cap
#define DEFAULT_ROF_EYES_OFF_INT       8 // 8.0bps, integral part of rof cap
#define DEFAULT_ROF_EYES_OFF_FRAC      1 // 8.0bps, fractional part of rof cap (values start at 1 = 0.0, 2 = 0.1, etc.)
#define DEFAULT_CLOSED_DWELL           1 // 1 = 0ff, 2=2ms, 3=3ms, etc.
#define DEFAULT_CLOSED_EYE_DELAY       8 // 160ms
#define DEFAULT_CLOSED_BOLT_DELAY      25 // 25ms
#define DEFAULT_ROF_ONOFF              1 // on , 2 = off
#define DEFAULT_BOARD_MODE             1 // 1 = viking (open bolt), 2 = excalibur (closed bolt)
#define DEFAULT_DEBOUNCE_MODE          1 // 1 = sequential, 2 = delay, 3 = fine-grain delay


// Register max values
#define REGISTER_DEBOUNCE_MAX            10
#define REGISTER_DWELL_MAX               30
#define REGISTER_LOADER_DELAY_MAX        10
#define REGISTER_MECH_DEBOUNCE_MAX       10
#define REGISTER_FSDO_DWELL_MAX          5
#define REGISTER_FIRE_MODE_MAX           3 // Note: change this when you add new modes
#define REGISTER_ROF_ON_INT_MAX          25 // integral part of rof cap
#define REGISTER_ROF_ON_FRAC_MAX         10 // value - 1 = fraction (FRACtion of 10 = .9 , FRAC of 1 = .0)
#define REGISTER_CLOSED_DWELL_MAX        13 // 13ms
#define REGISTER_CLOSED_EYE_DELAY_MAX    10 // 200ms max wait time
#define REGISTER_CLOSED_BOLT_DELAY_MAX   35 // 35ms
#define REGISTER_ROF_OFF_INT_MAX         12
#define REGISTER_ROF_OFF_FRAC_MAX        10
#define REGISTER_ROF_ONOFF_MAX           2
#define REGISTER_BOARD_MODE_MAX          2
#define REGISTER_DEBOUNCE_MODE_MAX       20


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
#define DEBOUNCE_MODE_SEQUENTIAL       1
#define DEBOUNCE_MODE_DELAY            2
#define DEBOUNCE_MODE_DELAY_FINE       3




//-- Eye Configuration --------------------------------------------------------------------
#define OP_EYES_OFF_TRIGGER_TIME         2000
#define OP_FORCE_SHOT_TRIGGER_TIME       500
#define EYES_ON_LED_COLOR                LED_RED
#define EYES_OFF_LED_COLOR               LED_YELLOW
#define EYES_OFF_CLOSED_BOLT_DELAY       160 // 160ms delay waiting for a ball to drop in eyes off mode


//-- LED Configuration ------------------------------------------------------------
#define LED_EYES_ON            LED_GREEN
#define LED_EYES_ON_BALL       LED_GREEN
#define LED_EYES_ON_EMPTY      LED_RED
#define LED_EYES_OFF           LED_ORANGE
#define LED_EYES_BLOCKED       LED_PURPLE

#define LED_DEFAULT_BRIGHTNESS       50
#define LDB                          LED_DEFAULT_BRIGHTNESS

// turn off LED blinking in firing mode
#define LED_DISABLE_BLINKING false

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


//-- Programming Configuration ------------------------------------------------------------
#define PROG_RESET_HOLD_TIME 10000 // hold trigger additional 10 seconds to reset board
#define PROG_RESET_WARNING_TIME 5000 // show warning blink after 5 seconds
#define PROG_RESET_WARNING_BLINK_TIME 200 // how many ms per blink
#define PROG_RESET_COMMIT_BLINK_TIME 100 // how many ms per blink


// -------- CONSTANTS ---------------------------------------------------------------------
// These should not be changed at all

//// Eyes
#define EYES_OFF                  0
#define EYES_ON                   1
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

//// Gun modes
#define GUNMODE_OPEN                  0 // cpen bolt
#define GUNMODE_CLOSED                1 // closed bolt

//// Operating Modes
#define MODE_PROGRAMMING              0
#define MODE_FIRING                   1

// Program Version
#define PROGRAM_VERSION               10

// EEPROM register numbers
#define REGISTER_VERSION              0  // not a programmed register, used to store program version

#define REGISTER_FIRE_MODE            1
#define REGISTER_DEBOUNCE             2
#define REGISTER_MECH_DEBOUNCE        3
#define REGISTER_DWELL                4
#define REGISTER_LOADER_DELAY         5
#define REGISTER_FSDO_DWELL           6
#define REGISTER_BOARD_MODE           7

#define REGISTER_CLOSED_DWELL         8    // second solenoid dwell for closed bolt mode, set this to anything other than 1 to turn on close bolt mode (5ms)
#define REGISTER_CLOSED_EYE_DELAY     9    // closed bolt eye delay - how long to wait for ball to enter breech (160ms, 20ms increments)
#define REGISTER_CLOSED_BOLT_DELAY    10   // how long to wait for bolt to close before firing

#define REGISTER_ROF_ONOFF            11
#define REGISTER_ROF_ON_INT           12
#define REGISTER_ROF_ON_FRAC          13
#define REGISTER_ROF_OFF_INT          14
#define REGISTER_ROF_OFF_FRAC         15

#define REGISTER_DEBOUNCE_MODE        16

// not in use yet
#define REGISTER_EYE_MODE
#define REGISTER_BOLT_DELAY
#define REGISTER_CPF

// explicitly set which register programming mode starts at
#define FIRST_REGISTER                REGISTER_FIRE_MODE

// new color scheme for programming registers
// color 1 is first color displayed, color 2 is second color displayed
// solid colors have same color 1 and color 2
// blinking colors have color 1 set to color, and color 2 set to LED_OFF
// alternating colors have differnt color 1 and color 2
#define REGISTER_1_COLOR_1  LED_GREEN
#define REGISTER_1_COLOR_2  LED_GREEN
#define REGISTER_2_COLOR_1  LED_PURPLE
#define REGISTER_2_COLOR_2  LED_PURPLE
#define REGISTER_3_COLOR_1  LED_YELLOW
#define REGISTER_3_COLOR_2  LED_YELLOW
#define REGISTER_4_COLOR_1  LED_BLUE
#define REGISTER_4_COLOR_2  LED_BLUE
#define REGISTER_5_COLOR_1  LED_RED
#define REGISTER_5_COLOR_2  LED_RED
#define REGISTER_6_COLOR_1  LED_WHITE
#define REGISTER_6_COLOR_2  LED_WHITE
#define REGISTER_7_COLOR_1  LED_TEAL
#define REGISTER_7_COLOR_2  LED_TEAL
#define REGISTER_8_COLOR_1  LED_ORANGE
#define REGISTER_8_COLOR_2  LED_ORANGE
#define REGISTER_9_COLOR_1  LED_GREEN
#define REGISTER_9_COLOR_2  LED_BLACK
#define REGISTER_10_COLOR_1  LED_PURPLE
#define REGISTER_10_COLOR_2  LED_BLACK
#define REGISTER_11_COLOR_1  LED_YELLOW
#define REGISTER_11_COLOR_2  LED_BLACK
#define REGISTER_12_COLOR_1  LED_BLUE
#define REGISTER_12_COLOR_2  LED_BLACK
#define REGISTER_13_COLOR_1  LED_RED
#define REGISTER_13_COLOR_2  LED_BLACK
#define REGISTER_14_COLOR_1  LED_WHITE
#define REGISTER_14_COLOR_2  LED_BLACK
#define REGISTER_15_COLOR_1  LED_TEAL
#define REGISTER_15_COLOR_2  LED_BLACK
#define REGISTER_16_COLOR_1  LED_ORANGE
#define REGISTER_16_COLOR_2  LED_BLACK


// uncomment the line below if you want to allow the user to 
// configure the ROF for when the eyes are off
// Note: be sure you have a reasonable value set for 
#define ALLOW_CONFIGURABLE_EYES_OFF_ROF true

// Total # of registers we're using
// Make sure this is updated if new programming registers are added!
#ifdef ALLOW_CONFIGURABLE_EYES_OFF_ROF
#define REGISTER_COUNT                16
#else
#define REGISTER_COUNT                14
#endif



#endif
