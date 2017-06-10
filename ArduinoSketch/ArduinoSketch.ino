// Libraries
#include <Wire.h>
#include <LiquidTWI.h>
#include <DHT.h>

// DHT Sensor config
#define DHTPIN 2 // PIN DHT sensor is on.
#define DHTTYPE DHT22 // The type of DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

LiquidTWI lcd(0); // Set LCD i2c address.

// PINs
int HeaterInput = 3;
int ManAutoButton = 4;
int ManOutButton = 5;
int ManInButton = 6;
int ManStopButton = 7;
int OutDamperRly = 8;
int InDamperRly = 9;
int FanRly = 10;
int ManAutoLED = 11;
int ManOutLED = 12;
int ManInLED = A0;
int ManStopLED = A1;
int OutDamperSw = A2;
int InDamperSw = A3;

// LCD update vars
long prevLCDMillis = 0;
long lcdInt = 2000; // Time to display each "screen."
int numScreen = 2; // How many "screens" we have, less 1.
int screenNum = 0;
bool screenChanged = true;

float temp = 0; // Used in case GetTempInterval is set in order to display temp/hum on the LCD.
float hum = 0; // Used in case GetTempInterval is set in order to display temp/hum on the LCD.
float MaxTemp = 75.00; // The max temp the system will allow before turning the system ON.
float MinTemp = 74.00; // The min temp the system will allow before turning the system OFF.
float EmergencyTemp = 95.00; // If we get to this temp then something is wrong. Sound the alarms!

char* SysStatus = "";
char* SysMode = "";

// LCD update states
#define S_LCDTempStatus 0
#define S_LCDSysStatus 1
#define S_LCDDamperStatus 2
// Exhaust States
#define S_Monitor 3
#define S_AutoInside 4
#define S_AutoOutside 5
#define S_ManOut 6
#define S_ManIn 7
#define S_ManStop 8
#define S_Sleep 9
#define S_Emergency 10

// Initial state
int state = S_Monitor;

void setup() {
  //Serial.begin(9600); // For testing. Comment out in production.
  lcd.begin(16, 2); // Set the number of rows and columns on the LCD.
  dht.begin(); // Start the DHT sensor.

  pinMode(HeaterInput, INPUT);
  pinMode(ManAutoButton, INPUT);
  pinMode(ManOutButton, INPUT);
  pinMode(ManInButton, INPUT);
  pinMode(ManStopButton, INPUT);
  pinMode(OutDamperSw, INPUT);
  pinMode(InDamperSw, INPUT);
  pinMode(OutDamperRly, OUTPUT);
  pinMode(InDamperRly, OUTPUT);
  pinMode(FanRly, OUTPUT);
  pinMode (ManAutoLED, OUTPUT);
  pinMode (ManOutLED, OUTPUT);
  pinMode (ManInLED, OUTPUT);
  pinMode (ManStopLED, OUTPUT);

  // Enable internal resistors for buttons.
  digitalWrite(ManAutoButton, HIGH);
  digitalWrite(ManOutButton, HIGH);
  digitalWrite(ManInButton, HIGH);
  digitalWrite(ManStopButton, HIGH);
  digitalWrite(OutDamperSw, HIGH);
  digitalWrite(InDamperSw, HIGH);
}

void loop() {
  switch (state) {

    case S_Monitor:
      F_Monitor();
      break;

    case S_AutoInside:
      F_AutoInside();
      break;

    case S_AutoOutside:
      F_AutoOutside();
      break;

    case S_ManOut:
      F_ManOut();
      break;

    case S_ManIn:
      F_ManIn();
      break;

    case S_ManStop:
      F_ManStop();
      break;

    case S_Emergency:
      F_Emergency();
      break;
  }
}

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

void F_GetTemp() {
  float t = dht.readTemperature(true);
  float h = dht.readHumidity();

  temp = t;
  hum = h;
}

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
    switch (screenNum)
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

