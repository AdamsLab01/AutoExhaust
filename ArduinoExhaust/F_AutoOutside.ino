void F_AutoOutside() {
  // Open the outside damper.
  digitalWrite(OutDamperRly, HIGH);

  // When the outside damper is open.
  if (digitalRead(OutDamperSw) == LOW) {
    digitalWrite(InDamperRly, LOW); // Close the inside  damper.
    digitalWrite(FanRly, HIGH); // Turn on the fan.
  }

  SysStatus = "EX OUT  ";      

  F_GetTemp();
  F_LCDUpdate();

  // If we've cooled off enough, go back to monitoring.
  if (temp <= MinTemp) {
    state = S_Monitor;
  }

  // If the furnace comes on we change states to exhaust inside. 
  if (digitalRead(HeaterInput) == LOW) {
    state = S_AutoInside;
  }

  // If temp is greater than EmergencyTemp we assume something is wrong and go into Emergency mode. 
  else if (temp >= EmergencyTemp) {
    state = S_Emergency;
  }

  // If any of the manual buttons are pressed we switch to the defined state.
  F_ReadButtons();
}

