void F_Monitor() {
  // We just monitor in this mode so we turn the fan off and close the dampers.
  digitalWrite(OutDamperRly, LOW); // Close the outside  damper.
  digitalWrite(InDamperRly, LOW); // Close the inside damper.
  digitalWrite(FanRly, LOW); // Turn the fan off.

  // Set the button LEDs to indicate which state we're in.
  digitalWrite (ManOutLED, LOW);
  digitalWrite (ManInLED, LOW);
  digitalWrite (ManAutoLED, HIGH); // We're in this state.
  digitalWrite (ManStopLED, LOW);

  SysMode = "AUTO      ";
  SysStatus = "MONITOR ";

  F_GetTemp();
  F_LCDUpdate();

  // If temp is greater than MaxTemp and the furnace is off we exhaust outside.
  if (temp >= MaxTemp && digitalRead(HeaterInput) == HIGH) {
    state = S_AutoOutside;
  }

  // If temp is greater than MaxTemp and the furnace is on we exhaust inside. 
  else if (temp >= MaxTemp && digitalRead(HeaterInput) == LOW) {
    state = S_AutoInside;
  }

  // If temp is greater than EmergencyTemp we assume something is wrong and go into Emergency mode. 
  else if (temp >= EmergencyTemp) {
    state = S_Emergency;
  }

  // If any of the manual buttons are pressed we switch to the defined state.
  F_ReadButtons();
}

