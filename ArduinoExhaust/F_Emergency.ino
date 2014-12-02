void F_Emergency() {
  digitalWrite(InDamperRly, HIGH);
  digitalWrite(OutDamperRly, HIGH);
  digitalWrite(FanRly, HIGH);

  digitalWrite (ManOutLED, HIGH); 
  digitalWrite (ManInLED, HIGH);
  digitalWrite (ManAutoLED, HIGH); 
  digitalWrite (ManStopLED, HIGH);

  SysMode = "EMERGENCY ";
  SysStatus = "EX ALL  ";      

  F_GetTemp();
  F_LCDUpdate(); 

  // If any of the manual buttons are pressed we switch to the defined state.
  F_ReadButtons();
}

