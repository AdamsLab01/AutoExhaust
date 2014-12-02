void F_LCDUpdate() {
  // Screen update - based off code from 'robtillaart' in the Arduino forums.
  unsigned long currentLCDMillis = millis();
  
  if (currentLCDMillis - prevLCDMillis > lcdInt) {
    prevLCDMillis = currentLCDMillis; 
    screenNum++;
    if (screenNum > numScreen) screenNum = 0;  // If all defined screens have been shown, start over.
      screenChanged = true;
  }

  if (screenChanged == true) {
    screenChanged = false; // Reset for next round.
    switch(screenNum)  
      { 
        case S_LCDTempStatus:
          F_LCDTempStatus();
        break;
        case S_LCDSysStatus:
          F_LCDSysStatus();
         break;
        case S_LCDDamperStatus:
          F_LCDDamperStatus();
        break;
      }
    }
}
