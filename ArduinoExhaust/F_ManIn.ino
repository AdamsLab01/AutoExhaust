void F_ManIn() {
  // Set the button LEDs to indicate which state we're in.
  digitalWrite (ManOutLED, LOW); 
  digitalWrite (ManInLED, HIGH); // We're in this state.
  digitalWrite (ManAutoLED, LOW); 
  digitalWrite (ManStopLED, LOW);

  // Open the inside damper.
  digitalWrite(InDamperRly, HIGH);

  // When the inside damper is open.
  if (digitalRead(InDamperSw) == LOW) {
    digitalWrite(OutDamperRly, LOW); // In case it's open.
    digitalWrite(FanRly, HIGH); // Turn on the fan.
  }

  SysMode = "MANUAL    ";
  SysStatus = "EX IN   ";      

  F_GetTemp();
  F_LCDUpdate();

  // If temp is greater than EmergencyTemp we assume something is wrong and go into Emergency mode. 
  if (temp >= EmergencyTemp) {
    state = S_Emergency;
  }

  // If any of the manual buttons are pressed we switch to the defined state.
  F_ReadButtons();
}

