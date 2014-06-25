/**********************************************************************************************************************
Automatic Exhaust

This sketch was written to in order to automate the exhausting of a small "server" room in my house. The idea is to
exhaust the hot server rom air outside, or inside (to another part) of the house depending on if the central furnace 
is on or not. If the server room is too hot and needs to be exhausted and the central house heater is on then the system 
will open the indoor damer (and close the outdoor one) to exhaust the hot server room air into the main part of the house, 
to help heat it. If the central house heater is off then the hot server room air will be exhausted outside.

For more information see - https://adambyers.com/2014/05/automatic-server-room-exhaust/ ‎or ping adam@adambyers.com

All code (except external libraries and third party code) is published under the MIT License.

**********************************************************************************************************************/

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

// Vars

// GetTempPreMillis and GetTempInterval are not used by default but put here in case we need to add a delay to keep the system from cycling too often.
unsigned long GetTempPreMillis = 0; // Leave at 0.
unsigned long GetTempInterval = 300000; // Leave at 0 unless a delay is needed.

unsigned long LCDPreMillis = 0; // Leave at 0.
unsigned long LCDUpdateInterval = 2000; // How often we want the LCD to cycle through each status display.

int MaxTemp = 75; // The max temp the system will allow before turning the system ON, must be lower than this for the system to go off.

float temp = 0; // Used in case GetTempInterval is set in order to display temp/hum on the LCD.
float hum = 0; // Used in case GetTempInterval is set in order to display temp/hum on the LCD.

bool BootUp = true; // Used in case GetTempInterval is set to get initial temp reading.

// Used to cycle the LCD through temp, system, and damper status.
bool LCDShowTempStatus = false;
bool ShowTemp = true;
bool ShowStatus = false;

// States
#define S_Auto 1
#define S_ManOut 2
#define S_ManIn 3
#define S_ManStop 4

// Initial state
int state = S_Auto;

void setup() {
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
  switch(state) {   
    case S_Auto:
      F_Auto();
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
  }
}
  
// Functions
void F_Auto() { 
  // Set the button LEDs to indicate which state we're in.
  digitalWrite (ManOutLED, LOW);
  digitalWrite (ManInLED, LOW);
  digitalWrite (ManAutoLED, HIGH); // We're in this state.
  digitalWrite (ManStopLED, LOW);
  
  // If we just started the system, get the temps so we can display them and act on them if necessary. This is needed to get initial temp reading if GetTempInterval is set. 
  if (BootUp == true) {
    F_GetTemp();
    BootUp = false;
  }
  
  // If any of the manual buttons are pressed we switch to the defined state.
  if (digitalRead(ManInButton) == LOW) {
    state = S_ManIn;
  }
  
  if (digitalRead(ManOutButton) == LOW) {
    state = S_ManOut;
  }
   
  if (digitalRead(ManStopButton) == LOW) {
    state = S_ManStop;
  }
  
/*
If temp is less than MaxTemp we keep everything off.
*/

  if (temp < MaxTemp) {
    digitalWrite(OutDamperRly, LOW); // Close the outside  damper.
    digitalWrite(InDamperRly, LOW); // Close the inside damper.
    digitalWrite(FanRly, LOW); // Turn the fan off.
    
    // If GetTempInterval is set to something besides 0 then we delay the temp reading. This may be necessary to keep the system from cycling ON/OFF to much.
    // Otherwise the temp is read each time through the loop.  
    if (millis() - GetTempPreMillis > GetTempInterval) {
      F_GetTemp();
      GetTempPreMillis = millis();
    }
    
    // LCD update 
    if (millis() - LCDPreMillis > LCDUpdateInterval) {
      if (LCDShowTempStatus == true)  { 
      LCDShowTempStatus = false;
    } 
    
    else {
      LCDShowTempStatus = true;
    }
    
    if (LCDShowTempStatus == true) {
      if (ShowTemp == true) {
        F_ShowTemp();
        ShowTemp = false;
        ShowStatus = true;
        LCDPreMillis = millis();
      }
      
      else if (ShowStatus == true) {
        lcd.setCursor(0, 0);
        lcd.print("Mode: AUTO      ");
        lcd.setCursor(0, 1);
        lcd.print("Status: ALL OFF ");
        ShowTemp = true;
        LCDPreMillis = millis();
      }
    }
    
    if (LCDShowTempStatus == false) {
      F_DamperStatus();
      LCDPreMillis = millis();
    }
  }
}

/*
If temp is greater than MaxTemp and the furnace is off we exhaust outside.
*/

  if (temp > MaxTemp && digitalRead(HeaterInput) == HIGH) {
    if (digitalRead(OutDamperSw) == HIGH) { // If the outside damper is closed, open it.
      digitalWrite(OutDamperRly, HIGH);
    }
    
  if (digitalRead(OutDamperSw) == LOW) { // If the outside damper is open.
    digitalWrite(InDamperRly, LOW); // Close the inside damper.
    digitalWrite(FanRly, HIGH); // Turn on the fan.
  }
      
  if (millis() - GetTempPreMillis > GetTempInterval) {
    F_GetTemp();
    GetTempPreMillis = millis();
  }
  
  // LCD Update
  if (millis() - LCDPreMillis > LCDUpdateInterval) {
    if (LCDShowTempStatus == true)  { 
      LCDShowTempStatus = false;
    } 
    
    else {
      LCDShowTempStatus = true;
    }
    
    if (LCDShowTempStatus == true) {
      if (ShowTemp == true) {
        F_ShowTemp();
        ShowTemp = false;
        ShowStatus = true;
        LCDPreMillis = millis();
      }
      
      else if (ShowStatus == true) {
        lcd.setCursor(0, 0);
        lcd.print("Mode: AUTO      ");
        lcd.setCursor(0, 1);
        lcd.print("Status: EX OUT  ");
        ShowTemp = true;
        LCDPreMillis = millis();
      }
    }
    
    if (LCDShowTempStatus == false) {
      F_DamperStatus();
      LCDPreMillis = millis();
    }
  }
}

/*
If temp is greater than MaxTemp and the furnace is on we exhaust inside. 
*/

  if (temp > MaxTemp && digitalRead(HeaterInput) == LOW) {
    if (digitalRead(InDamperSw) == HIGH) { // If the inside damper is closed, open it.
      digitalWrite(InDamperRly, HIGH);
    }
    
    if (digitalRead(InDamperSw) == LOW) { // If the inside damper is open.
      digitalWrite(OutDamperRly, LOW); // Close the outside damper.
      digitalWrite(FanRly, HIGH); // Turn on the fan.
    }
    
    if (millis() - GetTempPreMillis > GetTempInterval) {
      F_GetTemp();
      GetTempPreMillis = millis();
    }
    
    // LCD Update
    if (millis() - LCDPreMillis > LCDUpdateInterval) {
      if (LCDShowTempStatus == true) { 
        LCDShowTempStatus = false;
      } 
      
      else {
        LCDShowTempStatus=true;
      }
      
      if (LCDShowTempStatus == true) {
        if (ShowTemp == true) {
          F_ShowTemp();
          ShowTemp = false;
          ShowStatus = true;
          LCDPreMillis = millis();
        }
        
        else if (ShowStatus == true) {
          lcd.setCursor(0, 0);
          lcd.print("Mode: AUTO      ");
          lcd.setCursor(0, 1);
          lcd.print("Status: EX IN   ");
          ShowTemp = true;
          LCDPreMillis = millis();
        }
      }
      
      if (LCDShowTempStatus == false) {
        F_DamperStatus();
        LCDPreMillis = millis();
      }
    }
  }
}
    
void F_ManOut() {
  digitalWrite (ManOutLED, HIGH); // We're in this state.
  digitalWrite (ManInLED, LOW);
  digitalWrite (ManAutoLED, LOW);
  digitalWrite (ManStopLED, LOW);
  
  // If any of the manual buttons are pressed we switch to the defined state.
  if (digitalRead(ManInButton) == LOW) {
    state = S_ManIn;
  }
   
  if (digitalRead(ManStopButton) == LOW) {
    state = S_ManStop;
  }
   
  if (digitalRead(ManAutoButton) == LOW) {
    state = S_Auto;
  }
  
  if (digitalRead(OutDamperSw)) { // If the outside damper is closed, open it.
    digitalWrite(OutDamperRly, HIGH); // Open the outside damper.
  }
    
  if (digitalRead(OutDamperSw) == LOW) {
    digitalWrite(InDamperRly, LOW); // Close the inside damper.
    digitalWrite(FanRly, HIGH); // Turn on the fan.
  }
 
  // Since we're in manual mode we don't care how fast we get the temp so we just get it.
  F_GetTemp();
  
  // LCD Update
  if (millis() - LCDPreMillis > LCDUpdateInterval) {
    if (LCDShowTempStatus == true) { 
      LCDShowTempStatus = false;
    } 
    
    else {
      LCDShowTempStatus = true;
    }
    
    if (LCDShowTempStatus == true) {
      if (ShowTemp == true) {
        F_ShowTemp();
        ShowTemp = false;
        ShowStatus = true;
        LCDPreMillis = millis();
      }
      
      else if (ShowStatus == true) {
        lcd.setCursor(0, 0);
        lcd.print("Mode: MANUAL    ");
        lcd.setCursor(0, 1);
        lcd.print("Status: EX OUT  ");
        ShowTemp = true;
        LCDPreMillis = millis();
      }
    }
    
    if (LCDShowTempStatus == false) {
      F_DamperStatus();
      LCDPreMillis = millis();
    }
  }
}

void F_ManIn() {
  digitalWrite (ManOutLED, LOW);
  digitalWrite (ManInLED, HIGH); // We're in this state.
  digitalWrite (ManAutoLED, LOW);
  digitalWrite (ManStopLED, LOW);
  
  // If any of the manual buttons are pressed we switch to the defined state.
  if (digitalRead(ManOutButton) == LOW) {
    state = S_ManOut;
  }
   
  if (digitalRead(ManStopButton) == LOW) {
    state = S_ManStop;
  }
   
  if (digitalRead(ManAutoButton) == LOW) {
   state = S_Auto;
  }
  
  if (digitalRead(InDamperSw)) {
   digitalWrite(InDamperRly, HIGH); // Open the outside damper.
  }
    
  if (digitalRead(InDamperSw) == LOW) {
   digitalWrite(OutDamperRly, LOW); // Close the inside damper.
   digitalWrite(FanRly, HIGH); // Turn on the fan.
  }
  
  // Since we're in manual mode we don't care how fast we get the temp so we just get it.
  F_GetTemp();
  
  //LCD Update
  if (millis() - LCDPreMillis > LCDUpdateInterval) {
    if (LCDShowTempStatus == true) {
      LCDShowTempStatus = false;
    }
    
    else {
      LCDShowTempStatus = true;
    }
    
    if (LCDShowTempStatus == true) {
      if (ShowTemp == true) {
        F_ShowTemp();
        ShowTemp = false;
        ShowStatus = true;
        LCDPreMillis = millis();
      }
      
      else if (ShowStatus == true) {
        lcd.setCursor(0, 0);
        lcd.print("Mode: MANUAL    ");
        lcd.setCursor(0, 1);
        lcd.print("Status: EX IN   ");
        ShowTemp = true;
        LCDPreMillis = millis();
      }
    }
    
    if (LCDShowTempStatus == false) {
      F_DamperStatus();
      LCDPreMillis = millis();
    }
  }  
}

void F_ManStop() {
  digitalWrite (ManOutLED, LOW);
  digitalWrite (ManInLED, LOW);
  digitalWrite (ManAutoLED, LOW);
  digitalWrite (ManStopLED, HIGH); // We're in this state.
  
  // If any of the manual buttons are pressed we switch to the defined state.
  if (digitalRead(ManOutButton) == LOW) {
     state = S_ManOut;
   }
   
   if (digitalRead(ManInButton) == LOW) {
     state = S_ManIn;
   }
   
   if (digitalRead(ManAutoButton) == LOW) {
        state = S_Auto;
   }
  
  digitalWrite(OutDamperRly, LOW); // Close the outside  damper.
  digitalWrite(InDamperRly, LOW); // Close the inside damper.
  digitalWrite(FanRly, LOW); // Turn the fan off.
  
  // Since we're in manual mode we don't care how fast we get the temp so we just get it.
  F_GetTemp();
  
  //LCD Update
  if (millis() - LCDPreMillis > LCDUpdateInterval) {
    if (LCDShowTempStatus == true)  { 
      LCDShowTempStatus = false;
    } 
    
    else {
      LCDShowTempStatus = true;
    }
    
    if (LCDShowTempStatus == true) {
      if (ShowTemp == true) {
        F_ShowTemp();
        ShowTemp = false;
        ShowStatus = true;
        LCDPreMillis = millis();
      }
      
      else if (ShowStatus == true) {
        lcd.setCursor(0, 0);
        lcd.print("Mode: MANUAL    ");
        lcd.setCursor(0, 1);
        lcd.print("Status: STOPED  ");
        ShowTemp = true;
        LCDPreMillis = millis();
      }
    }
    
    if (LCDShowTempStatus == false) {
      F_DamperStatus();
      LCDPreMillis = millis();
    }
  }
}

void F_GetTemp() {
  float t = dht.readTemperature(true);
  float h = dht.readHumidity();
  
  temp = t;
  hum = h;  
}

void F_ShowTemp() {
  lcd.setCursor(0, 0);
  lcd.print("Temp: "); 
  lcd.print(temp);
  lcd.print("F    ");
  lcd.setCursor(0, 1);
  lcd.print("Hum: "); 
  lcd.print(hum);
  lcd.print("%     ");
}

void F_DamperStatus() {
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
