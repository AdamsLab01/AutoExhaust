void F_LCDTempStatus() {
  lcd.setCursor(0, 0);
  lcd.print("Temp: "); 
  lcd.print(temp);
  lcd.print("F    ");
  lcd.setCursor(0, 1);
  lcd.print("Hum: "); 
  lcd.print(hum);
  lcd.print("%     ");
}

void F_LCDSysStatus() {
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  lcd.print(SysMode);
  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(SysStatus);
}

void F_LCDDamperStatus() {
  if (digitalRead(InDamperSw) == LOW) {
    lcd.setCursor(0, 0);
    lcd.print("  Inside Damper "); 
    lcd.setCursor(0, 1);
    lcd.print("      Open      ");
  }

  if (digitalRead(OutDamperSw) == LOW) {
    lcd.setCursor(0, 0);
    lcd.print(" Outside Damper "); 
    lcd.setCursor(0, 1);
    lcd.print("     Open       ");
  }

  if (digitalRead(OutDamperSw) == HIGH && digitalRead(InDamperSw) == HIGH) {
    lcd.setCursor(0, 0);
    lcd.print("   All Dampers  "); 
    lcd.setCursor(0, 1);
    lcd.print("     Closed     ");
  }

  if (digitalRead(OutDamperSw) == LOW && digitalRead(InDamperSw) == LOW) {
    lcd.setCursor(0, 0);
    lcd.print("   All Dampers  "); 
    lcd.setCursor(0, 1);
    lcd.print("     Open       ");
  } 
}

