void F_ManStop() {
  // Set the button LEDs to indicate which state we're in.
  digitalWrite (ManOutLED, LOW); 
  digitalWrite (ManInLED, LOW);
  digitalWrite (ManAutoLED, LOW); 
  digitalWrite (ManStopLED, HIGH); // We're in this state.

  digitalWrite(OutDamperRly, LOW); // Close the outside  damper.
  digitalWrite(InDamperRly, LOW); // Close the inside damper.
  digitalWrite(FanRly, LOW); // Turn the fan off.

  SysMode = "MANUAL    ";
  SysStatus = "STOP    ";      

  F_GetTemp();
  F_LCDUpdate(); 

  // If any of the manual buttons are pressed we switch to the defined state.
  F_ReadButtons();
}

