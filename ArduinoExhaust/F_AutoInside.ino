void F_AutoInside() {
  // Open the inside damper.
  digitalWrite(InDamperRly, HIGH);

  // When the inside damper is open.
  if (digitalRead(InDamperSw) == LOW) {
    digitalWrite(OutDamperRly, LOW); // Close the outside  damper.
    digitalWrite(FanRly, HIGH); // Turn on the fan.
  }

  SysStatus = "EX IN   ";      

  F_GetTemp();
  F_LCDUpdate();

  // If we've cooled off enough, go back to monitoring.
  if (temp <= MinTemp) {
    state = S_Monitor;
  }

  // If the furnase goes off, we switch states to exhaust outside.
  if (digitalRead(HeaterInput) == HIGH) {
    state = S_AutoOutside;
  }

  // If temp is greater than EmergencyTemp we assume something is wrong and go into Emergency mode. 
  else if (temp >= EmergencyTemp) {
    state = S_Emergency;
  }

  // If any of the manual buttons are pressed we switch to the defined state.
  F_ReadButtons();
}
