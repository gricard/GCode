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

void eyesOn() {
  digitalWrite(EYE_TX_PIN, HIGH);
  
  // reset RGB color to eyes active
  ledColor(EYES_ON_LED_COLOR, LDB);
}

void eyesOff() {
  digitalWrite(EYE_TX_PIN, LOW);
  
  // reset RGB color to eyes inactive
  ledColor(EYES_OFF_LED_COLOR, LDB);
}

int readEyeState() {
  digitalRead(EYE_RX_PIN);
  return (digitalRead(EYE_RX_PIN) == HIGH);
}

void setEyeStatus(byte state) {
  switch( state ) {
    case EYES_ON:
    DEBUG_PRINTLN("EYES_ON");
      Op_EyeStatus = EYES_ON;
      eyesOn();
      break;
      
    case EYES_OFF:
    DEBUG_PRINTLN("EYES_OFF");
      // only disable TX eye if the eyes were turned off manually
      eyesOff();
      Op_EyeStatus = EYES_OFF; 
      // make sure ROF cap is on when eyes are turned off
      Op_UseROFCap = true;
      break;
    
    default:
    case EYES_BLOCKED:
    DEBUG_PRINTLN("EYES_BLOCKED");
      Op_EyeStatus = EYES_BLOCKED;
      // make sure ROF cap is on when eyes are turned off
      Op_UseROFCap = true;
      break;
  }
}

