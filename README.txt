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
        (the internal pull-up resistor will flutter in low input voltage conditions and cause bouncing)
      - Changed RGB LED to use pins 9, 10, 11 in order to use PWM
        (PWM pins: 3, 5, 6, 9, 10, and 11  See: http://arduino.cc/en/Reference/AnalogWrite )
      - Changed operation logic to set RGB LED to EYES_OFF_LED_COLOR when eyes are malfunctioning
      - Changed eyesOn() to set RGB LED back to EYES_ON_LED_COLOR to indicate eyes back on
      - Added current ROF calc to make mech debounce work
      - First prototype board built and operational
      - complete 5/28/13
      
v0.6  - Changes to variables to reduce memory usage
      - Split up code into multiple files to organize it better
      - Added brightness control to ledColor()
      - Reduced default RGB LED brightness
      - Add ramp fire mode
      - Turn off eyes for full auto mode since you can't turn them on/off via trigger, and they won't fault
      - Redo/finalize ROF support (config options should have 1 flash = uncapped, 2 = 8bps, etc.)
      - More code cleanup
      - Added an optional alternative debounce implementation using the delay method (even if it's less effective)
      - Fixed operatingLEDBlink() bug
      - Added license
      - Clean up ramping code
      - complete 6/2/13
      - release!
      
v0.7  - 
     
TODO:

For v0.7:   
      + Wrap all global var access in macros so we can switch between wrapper functions / direct access easily
      + Clean up trigger state tracking (Prog_PriorTriggerState, Prog_TriggerState, Trigger_State)
      + Separate registers for integer and fraction parts of ROF settings (12.5 = one register holds 12, the other holds 5)
      + interrupt handling (timer interrupt + counter  (debounce))
      + Move ROF limit conversion into setup() so ROF values in code are always the actual BPS rate
      + Move printTriggerState() into debug.h and rename that to Debug.ino
      + Clean up use of "TS" and "MS" in var names. pick one.
      + Fix bug with LED staying solid yellow as ramp winds down


     + shot queue?
     + blink eyes when waiting for a trigger pull (like the Droid board)
     + power management / sleep
     + dual solenoid support
     + dual eye logic / bolt tracking
     + auto shut off (may not be possible with boards using an on/off switch)
     + FSDO reset time?
     + HES bolt position tracking
     + LCD display support
     + Support two pin multi color led like on the WAS Viking board?
     + Implement comment-based documentation generation, like doxygen
