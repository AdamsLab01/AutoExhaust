void F_ManOut() {
  // Set the button LEDs to indicate which state we're in.
  digitalWrite (ManOutLED, HIGH); // We're in this state.
  digitalWrite (ManInLED, LOW);
  digitalWrite (ManAutoLED, LOW); 
  digitalWrite (ManStopLED, LOW);

  // Open the outside damper.
  digitalWrite(OutDamperRly, HIGH);

  // When the outside damper is open.
  if (digitalRead(OutDamperSw) == LOW) {
    digitalWrite(InDamperRly, LOW); // In case it's open.
    digitalWrite(FanRly, HIGH); // Turn on the fan.
  }

  SysMode = "MANUAL    ";
  SysStatus = "EX OUT  ";

  F_GetTemp();
  F_LCDUpdate();

  // If temp is greater than EmergencyTemp we assume something is wrong and go into Emergency mode. 
  if (temp >= EmergencyTemp) {
    state = S_Emergency;
  }

  // If any of the manual buttons are pressed we switch to the defined state.
  F_ReadButtons();
}

