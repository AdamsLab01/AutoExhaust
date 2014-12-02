/**********************************************************************************************************************
 * Automatic Exhaust
 * 
 * This sketch was written to in order to automate the exhausting of a small "server" room in my house. The idea is to
 * exhaust the hot server rom air outside, or inside (to another part) of the house depending on if the central furnace 
 * is on or not. If the server room is too hot and needs to be exhausted and the central house heater is on then the system 
 * will open the indoor damer (and close the outdoor one) to exhaust the hot server room air into the main part of the house, 
 * to help heat it. If the central house heater is off then the hot server room air will be exhausted outside.
 * 
 * For more information see - https://adambyers.com/2014/05/automatic-server-room-exhaust/ ‎or ping adam@adambyers.com
 * 
 * All code (except external libraries and third party code) is published under the MIT License.
 * 
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

