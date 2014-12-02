void F_ReadButtons() {
  if (digitalRead(ManInButton) == LOW) {
    state = S_ManIn;
  }
  
  if (digitalRead(ManOutButton) == LOW) {
    state = S_ManOut;
  }
   
  if (digitalRead(ManStopButton) == LOW) {
    state = S_ManStop;
  }
  
  if (digitalRead(ManAutoButton) == LOW) {
    state = S_Monitor;
  }
}
