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


void ledColor(const byte color[], byte brightnessPercentage) {
  // brightness is a percentage applied to the color values
  if( brightnessPercentage < 1 || brightnessPercentage > 100 ) brightnessPercentage = 100;
  float multiplier = (brightnessPercentage/100.0);
  
  // multiply each value in the RGB color by the percentage
  // to apply the brightness level to it
  byte r = color[0] * multiplier;
  byte g = color[1] * multiplier;
  byte b = color[2] * multiplier;
  
  // sanitize values
  r = r < 1 ? 0 : r;
  g = g < 1 ? 0 : g;
  b = b < 1 ? 0 : b;

  //DEBUG_PRINT("LED: R=");DEBUG_PRINT(r);DEBUG_PRINT(", G=");DEBUG_PRINT(g);DEBUG_PRINT(", B=");DEBUG_PRINTLN(b);
  
  analogWrite(RGBLED_PIN_RED, 255 - r);         
  analogWrite(RGBLED_PIN_GREEN, 255 - g);         
  analogWrite(RGBLED_PIN_BLUE, 255 - b);
}

void ledOff() {
  analogWrite(RGBLED_PIN_RED, 255);         
  analogWrite(RGBLED_PIN_GREEN, 255);         
  analogWrite(RGBLED_PIN_BLUE, 255);    
}


void setLEDRegisterColor(int regNum) {
  byte brightness = 25;
  switch( regNum ) {
    case REGISTER_DEBOUNCE:      ledColor(LED_GREEN, brightness); break;
    case REGISTER_DWELL:         ledColor(LED_PURPLE, brightness); break;
    case REGISTER_LOADER_DELAY:  ledColor(LED_YELLOW, brightness); break;
    case REGISTER_MECH_DEBOUNCE: ledColor(LED_BLUE, brightness); break;
    case REGISTER_FSDO_DWELL:    ledColor(LED_RED, brightness); break;
    case REGISTER_FIRE_MODE:     ledColor(LED_WHITE, brightness); break;
    case REGISTER_ROF_ON:        ledColor(LED_TEAL, brightness); break;
    case REGISTER_ROF_OFF:       ledColor(LED_ORANGE, brightness); break;

    default:                     ledColor(LED_WHITE, brightness);
  }
}

void powerOnLEDBurst(byte mode) {
  switch( mode ) {
    case MODE_PROGRAMMING:
      // 'Murica!
      ledColor(LED_RED, LDB);
      delay(200);
      ledColor(LED_WHITE, LDB);
      delay(200);
      ledColor(LED_BLUE, LDB);
      delay(200);
      ledColor(LED_RED, LDB);
      delay(200);
      ledColor(LED_WHITE, LDB);
      delay(200);
      ledColor(LED_BLUE, LDB);
      delay(200);
      ledOff();
      break;
    case MODE_FIRING:
      for(int c = 0; c < 10; c++ ) {
        ledColor(LED_GREEN, LDB);
        delay(50);
        ledOff();
        delay(50);
      }
      break;
  }
}

void operatingLEDBlink() {
  bool WaitingForTriggerInput = (TRIGGER_STATE_WAITING == TriggerState);
  
  // wait at least 1 second between trigger pulls to activate
  bool NotActivelyShooting = (millis() > (Op_LastPullMS + 1000));
  
  if( LastEyeBlinkOn ) {
    if( (operationTiming - LastEyeBlink) > BLINK_INTERVAL_ON ) {
      ledOff();
      LastEyeBlinkOn = false;
      LastEyeBlink = operationTiming;
    }
  } else {
    bool ItsTimeToBlink = ((operationTiming - LastEyeBlink) > BLINK_INTERVAL_OFF);
    
    // always wait until we're in a state where the gun is not shooting in order
    // to perform this blinking since the LED is in use by the firing mode
    if( WaitingForTriggerInput && NotActivelyShooting && ItsTimeToBlink ) {
      switch( EyeMode ) {
        case EYES_ON: ledColor(LED_EYES_ON, BLINK_BRIGHTNESS); break;
        case EYES_OFF: ledColor(LED_EYES_OFF, BLINK_BRIGHTNESS); break;
        case EYES_BLOCKED: ledColor(LED_EYES_BLOCKED, BLINK_BRIGHTNESS); break;
        default: ledColor(LED_PURPLE, BLINK_BRIGHTNESS);
      }
      
      LastEyeBlinkOn = true;
      LastEyeBlink = operationTiming;
    }
  }
}
