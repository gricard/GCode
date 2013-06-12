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


void setLEDRegisterColor(int regNum, byte colorNum) {
  byte brightness = 25;
  switch( regNum ) {
    case REGISTER_DEBOUNCE:            ledColor(colorNum == 1 ? REGISTER_1_COLOR_1 : REGISTER_1_COLOR_2, brightness); break;
    case REGISTER_DWELL:               ledColor(colorNum == 1 ? REGISTER_2_COLOR_1 : REGISTER_2_COLOR_2, brightness); break;
    case REGISTER_LOADER_DELAY:        ledColor(colorNum == 1 ? REGISTER_3_COLOR_1 : REGISTER_3_COLOR_2, brightness); break;
    case REGISTER_MECH_DEBOUNCE:       ledColor(colorNum == 1 ? REGISTER_4_COLOR_1 : REGISTER_4_COLOR_2, brightness); break;
    case REGISTER_FSDO_DWELL:          ledColor(colorNum == 1 ? REGISTER_5_COLOR_1 : REGISTER_5_COLOR_2, brightness); break;
    case REGISTER_FIRE_MODE:           ledColor(colorNum == 1 ? REGISTER_6_COLOR_1 : REGISTER_6_COLOR_2, brightness); break;
    case REGISTER_ROF_ON_INT:          ledColor(colorNum == 1 ? REGISTER_7_COLOR_1 : REGISTER_7_COLOR_2, brightness); break;
    case REGISTER_ROF_ON_FRAC:         ledColor(colorNum == 1 ? REGISTER_8_COLOR_1 : REGISTER_8_COLOR_2, brightness); break;
    case REGISTER_CLOSED_DWELL:        ledColor(colorNum == 1 ? REGISTER_9_COLOR_1 : REGISTER_9_COLOR_2, brightness); break;
    case REGISTER_CLOSED_EYE_DELAY:    ledColor(colorNum == 1 ? REGISTER_10_COLOR_1 : REGISTER_10_COLOR_2, brightness); break;
    case REGISTER_CLOSED_BOLT_DELAY:   ledColor(colorNum == 1 ? REGISTER_11_COLOR_1 : REGISTER_11_COLOR_2, brightness); break;
    
    case REGISTER_ROF_OFF_INT:         ledColor(colorNum == 1 ? REGISTER_12_COLOR_1 : REGISTER_12_COLOR_2, brightness); break;
    case REGISTER_ROF_OFF_FRAC:        ledColor(colorNum == 1 ? REGISTER_13_COLOR_1 : REGISTER_13_COLOR_2, brightness); break;

    default:                           ledColor(LED_WHITE, brightness);
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

void resetProgrammingLEDBurst() {
  for(int c = 0; c < 10; c++ ) {
    if( c % 2 ) ledColor(LED_PURPLE, LDB);
    else ledColor(LED_ORANGE, LDB);
    delay(50);
    ledOff();
    delay(50);
  }
}

void operatingLEDBlink() {
  byte eyeCheck;
  bool WaitingForTriggerInput = (TRIGGER_STATE_WAITING == TriggerState);
  // wait at least 1 second between trigger pulls to activate
  bool NotActivelyShooting = (millis() > (Op_LastPullMS + 1000));
  
  if( LastEyeBlinkOn ) {
    if( !Op_EyeBlinkSolid && (operationTiming - LastEyeBlink) > BLINK_INTERVAL_ON ) {
      ledOff();
      LastEyeBlinkOn = false;
      LastEyeBlink = operationTiming;
    }
  } else {
    bool ItsTimeToBlink = ((operationTiming - LastEyeBlink) > BLINK_INTERVAL_OFF);
    
    if( Op_EyeBlinkSolid ) ItsTimeToBlink = true;
    
    // always wait until we're in a state where the gun is not shooting in order
    // to perform this blinking since the LED is in use by the firing mode
    if( WaitingForTriggerInput && NotActivelyShooting && ItsTimeToBlink ) {
      switch( Op_EyeStatus ) {
        case EYES_ON: 
          // display one color if we see a ball and another if we don't
          eyeCheck = GET_EYE_READ_STATE();
          if( eyeCheck ) ledColor(LED_EYES_ON_BALL, BLINK_BRIGHTNESS); 
          else ledColor(LED_EYES_ON_EMPTY, BLINK_BRIGHTNESS); 
          break;
          
        case EYES_OFF: ledColor(LED_EYES_OFF, BLINK_BRIGHTNESS); break;
        
        default:
        case EYES_BLOCKED: ledColor(LED_EYES_BLOCKED, BLINK_BRIGHTNESS); break;
      }
      
      if( !Op_EyeBlinkSolid ) {
        LastEyeBlinkOn = true;
        LastEyeBlink = operationTiming;
      }
    }
  }
}
