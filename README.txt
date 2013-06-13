GCode v0.9
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


About
------------------------------------------------------------------------------------------
This piece of software is intended to be used with Arduino-compatible boards powering
paintball guns. It may eventually grow beyond that, but right now the focus is on 
Arduino. 

I created this mainly due to my curiosity with how paintball guns work, and well, 
because I could. I've been involved with the Free Software Foundation and other open 
source projects (Linux, PHP, etc.) in the past and agree with the ideals of the open 
source community.

Currently it has only been used in an AKA Viking with a custom board setup based around 
the Femtoduino board.

Follow the development of this software here: http://simonized.com/showthread.php?702

License
------------------------------------------------------------------------------------------
This software is currently licensed for use and redistribution under the GNU GPL v3. 
This license does not currently allow derivative works that do not provide full source 
code. I may reevaluate this decision later. For now, if you wish to use this code in a 
closed source manner, please consult me first by emailing me at sumorai@gmail.com. 
Thank you.

Notes
------------------------------------------------------------------------------------------
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

Contributing
------------------------------------------------------------------------------------------
Anyone is welcome to contribute to this project, or to fork it and work on it on their 
own. I ask that if you intend to contribute code back to the GCode project that you ensure
you follow the same coding style used in this code so there is a uniformity to the 
codebase.

Disclaimer
------------------------------------------------------------------------------------------
Use at your own risk! I assume no responsibility for anything that occurs while using this
code. Paintball guns are dangerous if handled improperly. Always wear a mask around a
paintball gun and treat it as if the safety is off. Don't be dumb. 




Version History
      
v0.0  - started 4/22/13

v0.1  - full trigger & debounce logic
      - programmable debounce & dwell  
      - semi-auto, uncapped only
      - complete 4/29/13

v0.2  - Rewrote debug code to use ifdefs
      - added basic breakbeam eye input
      - complete 4/29/13

v0.3  - Added Mech Debounce
      - Added Loader Delay
      - Added FSDO Dwell
      - complete 4/29/13

v0.4  - Add ability to hold trigger to force a shot after 1/2 second
      - Add ability to turn off eyes after holding trigger for 2 seconds
      - Add code to check eye malfunctions
      - complete 4/30/13

v0.5  - Fixed some eye code issues
      - Initial ROF cap
      - Moved back to physical 10K pull-up resistor on trigger thanks to Will's advice
        (the internal pull-up resistor will flutter in low input voltage conditions and 
        cause bouncing)
      - Changed RGB LED to use pins 9, 10, 11 in order to use PWM
        (PWM pins: 3, 5, 6, 9, 10, and 11  
        See: http://arduino.cc/en/Reference/AnalogWrite )
      - Changed operation logic to set RGB LED to EYES_OFF_LED_COLOR when eyes are 
        malfunctioning
      - Changed eyesOn() to set RGB LED back to EYES_ON_LED_COLOR to indicate eyes back on
      - Added current ROF calc to make mech debounce work
      - First prototype board built and operational
      - complete 5/28/13
      
v0.6  - Changes to variables to reduce memory usage
      - Split up code into multiple files to organize it better
      - Added brightness control to ledColor()
      - Reduced default RGB LED brightness
      - Add ramp fire mode
      - Turn off eyes for full auto mode since you can't turn them on/off via trigger, and
        they won't fault
      - Redo/finalize ROF support 
        (config options should have 1 flash = uncapped, 2 = 8bps, etc.)
      - More code cleanup
      - Added an optional alternative debounce implementation using the delay method 
        (even if it's less effective)
      - Fixed operatingLEDBlink() bug
      - Added license
      - Clean up ramping code
      - complete 6/2/13
      - release!
      
v0.7  - Clean up use of "TS" and "MS" in var names. 
      - Move ROF limit conversion into setup() so ROF values in code are always the actual
        BPS rate
      - Clean up trigger state tracking 
        (Prog_PriorTriggerState, Prog_TriggerState, Trigger_State)
      - Separate registers for integer and fraction parts of ROF settings 
        (12.5 = one register holds 12, the other holds 5)
      - Fix bug with LED staying solid color as ramp winds down
      - Fix bug with LED flashing when holding down trigger in main menu to select 
        register
      - complete 6/7/13

v0.8  - Enabled ROF eyes off programming
      - Fixed eye logic bug causing eyes to keep turning off
      - Renamed getEyeState() to readEyeState() for clarity
      - Fixed setEyeStatus() to properly set eye status, and to only turn off
        eye TX LED when eyes are actually turned off, not blocked
      - Split eye update code out into updateEyeState()
      - Fixed bug where eyes would not be turned back on when a block was cleared
      - Fixed code that updates LED color to indicate eye state
      - Added ability to configure whether LED blinks or not in firing mode
      - Split up firingMode() code into sub-units
      - Board reset (hold trigger down to get into programming mode and keep holding it      
        until light starts flashing red, then release to reset board)
      - Change register 0 to store code version
      - Reset all registers to default values if code version is old
      - Added programming registers for closed bolt mode
      - Added LED alternating/blinking color support for programming mode registers

v0.9
      - Added blinking yellow warning light for board reset when trigger held > 5s 
        at startup & made red reset commit light blink faster
      + Dual solenoid support (closed bolt mode)

TODO:

For v0.9:
      + Fix operationalLEDBlink() so that it doesn't delay when eyes are off/blocked, or 
        there is no ball in the breech
      + Fix ramping code to go by shots fired timing, not trigger pull timing
         (you could have pulls with the eyes off that initiate ramp and it will fire in
          ramp when a ball lands in the breech)
      + Add event hooks to code (eye state change, trigger state change, shot firing, etc.)
           + Move things like LED changes into these hooks
           + Eventually these will also be used to update the LCD/OLED as well
      + Weird bug in programming mode where it skips to the next register too quickly
      + Re-enable eyes for full auto
      + Add separate setting for ROF on/off (& remove "unlimited" setting from ROF caps)

General:
     + Debounce trigger release
     + Shot queueing (allowed to have one shot queued)
     + OLED/LCD display support
     + Stats collection
     + Speaker + audio output support
     + Wrap all global var access in macros so we can switch between wrapper functions 
       / direct access easily
     + Add testing mode for debounce code that includes a register to change debounce 
       schemes (sequential/delay)
     + Blink eyes when waiting for a trigger pull (like the Droid board)
     + Single color LED blinkenlights programming mode
     + Battery level detection
     + dual eye logic / bolt tracking
     + Interrupt handling (timer interrupt + counter  (debounce))
     + HES bolt position tracking
     + Implement comment-based documentation generation, like doxygen

Not sure about:
     + auto shut off (may not be possible with boards using an on/off switch)
     + FSDO reset time
     + Support for two pin multi color led like on the WAS Viking board
     + power management / sleep 
