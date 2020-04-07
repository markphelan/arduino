#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <SimpleRotary.h>
#include <DMXSerial.h>

// OLED Config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define TEXT_SIZE 4.5

// Number of Servos
#define NUM_CHANNELS 3

// Encoder connections
const int button = 8;
const int CLK = 7;
const int DT = 6;

// Set Servo pins to map to each DMX channel
const int servoOutputs[NUM_CHANNELS] = {9,10,11};

unsigned long displayTimeout = 0;
unsigned int addr = 0;

// Define encoder
SimpleRotary encoder(DT,CLK,button);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins - 2 & 3 respectively on Pro Micro)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define servo 
Servo servo[NUM_CHANNELS];  

void setup() {
  // Get the DMX address saved in EEPROM
  loadAddress();

  // Initialise the screen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  showAddress();

  // Initialise the DMX receiver
  DMXSerial.init(DMXReceiver);
  
  // Initialise the servos
  for (int i=0; i<NUM_CHANNELS; i++) {
    servo[i].attach(servoOutputs[i]);
  } 
}

void changeAddress() {
  showAddress();
  while (!encoder.push()) {
    static int pos = 0;
    byte i;
    i = encoder.rotate();
    
    if (i == 2) {
      if (addr<512-NUM_CHANNELS) { addr++; }
      else { addr = 1; }
      showAddress();
    }
    
    if (i == 1) {
      if (addr>1) { addr--; }
      else { addr = 513-NUM_CHANNELS; }
      showAddress();
    }
  } // while
  
  saveAddress();
  printMessage("Saved");
  delay(1000);
  showAddress();
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
  display.setTextColor(WHITE);      
  display.setCursor(0,0);           
  display.setTextSize(TEXT_SIZE);

  if (addr < 100) { display.print(0); }
  if (addr < 10) { display.print(0); }
  display.print(addr);
  display.display();
  displayTimeout=millis()+5000;
}

void printMessage(char *msg) {
  display.clearDisplay();
  display.setTextColor(WHITE);      
  display.setCursor(0,0);           
  display.setTextSize(TEXT_SIZE);
  display.print(msg);
  display.display();
  displayTimeout=millis()+5000;
}
void loop() {
  // Read each DMX channel and map the 0-255 value to a 0-180 value to the respective servo pin
  for (int i=0; i<NUM_CHANNELS; i++) {
    servo[i].write(map(DMXSerial.read(addr+i), 0, 255, 0, 180));
  }

  if (encoder.push()) {
      printMessage("Set");
      delay(1000);
      changeAddress();
  }

  if (millis() > displayTimeout && displayTimeout != 0) {
    display.clearDisplay();
    display.display();
    displayTimeout = 0;
  }
   
}
