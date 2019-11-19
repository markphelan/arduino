#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DMXSerial.h>
#include <EEPROM.h>
#include <Arduino.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// pin assignments
const int ch1 = 6;
const int ch2 = 7;
const int ch3 = 8;
const int ch4 = 9;
const int buttonUp = 5;
const int buttonDown = 4;
const int buttonSet = 3;
const int buttonMode = 10;
// default values
int ch1Val = HIGH;
int ch2Val = HIGH;
int ch3Val = HIGH;
int ch4Val = HIGH;

const int on = LOW;
const int off = HIGH;

// some variables
unsigned int addr = 0;
unsigned long lastScreenChange = 0;
int screenChangeInterval = 5000; // how often the screen alternates between channel status and DMX address
int currentScreen = 0;
unsigned long buttonPressTime = 0;

void setup() {
  DMXSerial.init(DMXReceiver);
  //Serial.begin(9600); / Serial port in use by DMX, so can't use any of this
  delay(1000);
  loadAddress();
    
  pinMode(ch1, OUTPUT);
  pinMode(ch2, OUTPUT);
  pinMode(ch3, OUTPUT);
  pinMode(ch4, OUTPUT);

  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonSet, INPUT_PULLUP);

  digitalWrite(ch1, ch1Val);
  digitalWrite(ch2, ch2Val);
  digitalWrite(ch3, ch3Val);
  digitalWrite(ch4, ch4Val);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();
    
  showAddress();  
  delay(2000);
  display.clearDisplay();
    
}

void changeAddress() {
  // Menu to update the DMX address
  display.clearDisplay();
  display.setTextSize(2);      
  display.setTextColor(WHITE); 
  display.setCursor(0,0);      
  display.println(F("SETTINGS"));
  display.display();
  
  while(digitalRead(buttonSet)==LOW) {
    // Wait for button to be released
  }
  unsigned long debounceTime=0;
  for (;;) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(F("SET DMX:"));
    if (addr < 100) { display.print(0); }
    if (addr < 10) { display.print(0); }
    display.println(addr);
    display.display();
    if (digitalRead(buttonUp)==LOW && debounceTime < millis()) {
      if (addr<512) { addr++; }
      else { addr = 1; }
      debounceTime=millis()+100;
    }
    if (digitalRead(buttonDown)==LOW && debounceTime < millis()) {
      if (addr>1) { addr--; }
      else { addr = 512; }
      debounceTime=millis()+100;
    }
    if (digitalRead(buttonSet)==LOW) {
      saveAddress();
      buttonPressTime = 0;
      break;
    }
  }
}

void saveAddress() {
  int highByte = highByte(addr);
  int lowByte = lowByte(addr);
  EEPROM.put(0,highByte);
  EEPROM.put(1,lowByte);
}

void loadAddress() {
  int highByte = 0;
  int lowByte = 0;
  EEPROM.get(0, highByte);
  EEPROM.get(1, lowByte);
  if (highByte == -1 && lowByte == -1) { // set default address to 1
    highByte = 0;
    lowByte = 1;
    EEPROM.put(0,highByte);
    EEPROM.put(1,lowByte);
  }
  addr = highByte * 256 + lowByte;
}

void showAddress() {
  display.clearDisplay();
  display.setTextSize(3);           
  display.setTextColor(WHITE);      
  display.setCursor(0,0);           
  display.print(F("DMX:"));
  if (addr < 100) { display.print(0); }
  if (addr < 10) { display.print(0); }
  display.println(addr);
  
  display.display();
  
}

void showChannelStates() {
  display.clearDisplay();
  display.setTextSize(3);
  if (ch1Val == on) { 
    display.fillCircle(15, 15, 14, WHITE);
    display.setTextColor(BLACK); 
    display.setCursor(8,5);
    display.print("1");
  } else { 
    display.drawCircle(15, 15, 14, WHITE); 
    display.setTextColor(WHITE); 
    display.setCursor(8,5);
    display.print("1");
  }
  if (ch2Val == on) { 
    display.fillCircle(47, 15, 14, WHITE);
    display.setTextColor(BLACK); 
    display.setCursor(40,5);
    display.print("2");
  } else { 
    display.drawCircle(47, 15, 14, WHITE); 
    display.setTextColor(WHITE); 
    display.setCursor(40,5);
    display.print("2");
  }
  if (ch3Val == on) { 
    display.fillCircle(79, 15, 14, WHITE);
    display.setTextColor(BLACK); 
    display.setCursor(71,5);
    display.print("3");
  } else { 
    display.drawCircle(79, 15, 14, WHITE); 
    display.setTextColor(WHITE); 
    display.setCursor(71,5);
    display.print("3");
  }
  if (ch4Val == on) { 
    display.fillCircle(111, 15, 14, WHITE);
    display.setTextColor(BLACK); 
    display.setCursor(104,5);
    display.print("4");
  } else { 
    display.drawCircle(111, 15, 14, WHITE); 
    display.setTextColor(WHITE); 
    display.setCursor(104,5);
    display.print("4");
  }
 
  display.display();
}

void noDmxMessage() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(WHITE); 
  unsigned long lastPacketSecs = round(DMXSerial.noDataSince() / 1000);
  display.println("No DMX for");
  if (lastPacketSecs < 600) {
    display.print(lastPacketSecs);
    display.print(" secs");
  } else {
    display.print(">10mins");
  }
  display.display();
}

void loop() {
  unsigned long lastPacket = DMXSerial.noDataSince();

  // Handle press & hold of menu/set button
  if (digitalRead(buttonSet)==LOW && buttonPressTime < millis() && buttonPressTime == 0) { // button pressed to start setting
    buttonPressTime = millis()+2000;
  }
  if (digitalRead(buttonSet)==LOW && buttonPressTime != 0 && millis() >= buttonPressTime) { // end of 2 second hold
    changeAddress();
  }
  if (digitalRead(buttonSet)==HIGH && buttonPressTime != 0) { // button released early
    buttonPressTime = 0;
  }
  
  if (lastPacket < 5000) {
    if (DMXSerial.read(addr) > 128) { ch1Val = on; }
    else { ch1Val = off; }
    if (DMXSerial.read(addr+1) > 128) { ch2Val = on; }
    else { ch2Val = off; }
    if (DMXSerial.read(addr+2) > 128) { ch3Val = on; }
    else { ch3Val = off; }
    if (DMXSerial.read(addr+3) > 128) { ch4Val = on; }
    else { ch4Val = off; }

    digitalWrite(ch1, ch1Val);
    digitalWrite(ch2, ch2Val);
    digitalWrite(ch3, ch3Val);
    digitalWrite(ch4, ch4Val);

    // Alternate screen display periodically
    if (millis() - lastScreenChange >= screenChangeInterval) {
      lastScreenChange = millis();
      currentScreen == 0 ? currentScreen = 1 : currentScreen = 0;        
    }
    if (currentScreen == 0) {
      showChannelStates();
    } else {
      showAddress();
    }
  
  } else { // If it's over 5 seconds since DMX was received then show warning
    noDmxMessage();
  }

  
  
}

