#include <FastLED.h>

#define NUM_LEDS 35
#define DATA_PIN 2
const int button = 19;
int mode = 0;
const int maxMode = 8;
unsigned long debounceTime=0;
CRGB leds[NUM_LEDS];

void setup() {
	 	pinMode(button, INPUT_PULLUP);
	 	myDelay(1000);
    FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
   switch (mode) {
    case 0:
      scroll(0,255,20);
      scroll(0,0,20);
      break;
    case 1:
      sparkle();
      break;
    case 2:
      colourFade();
      break;
    case 3:
      blueAndWhite();
      break;
    case 4:
      scroll(180,255,40);
      scrollReverse(180,255,40);
      break;
    case 5:
      redAndGreen();
      break;
    case 6:
      flash(25);
      break;
    case 7:
      glow(0,64);
      break;
    case 8:
      black();
      break;
   }
}

bool myDelay(int t) {
  unsigned long waitTime=millis();

  while(waitTime + t > millis()) {
  if (digitalRead(button)==LOW && debounceTime < millis()) {
      debounceTime=millis()+500;
      if (mode < maxMode) { mode++; }
      else { mode = 0; }
      black();
      return true;
    }
  }
  return false;
}

void black() {
  for(int i=0; i < NUM_LEDS; i++) {
    leds[i]=CRGB::Black;
  }
  FastLED.show();
  myDelay(500);
}

void flash(int hue) {
  for(int flash=0; flash < 3; flash++) {
    for(int i=0; i < NUM_LEDS; i++) {
      if (i%4 == 0) { leds[i] = CRGB::Black; }
      else { leds[i] = CHSV(hue, 255, 255); }
    }
    FastLED.show();
    myDelay(30);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    myDelay(30);
  }
  myDelay(500);
}

void glow(int hue, int brightness) {
  fill_solid(leds, NUM_LEDS, CHSV(hue, 255, brightness));
  FastLED.show();
  myDelay(500);
}

void redAndGreen() {
  for(int i=0; i < NUM_LEDS; i++) {
    if (i % 2 == 0) {
      leds[i] = CRGB::Red;
    } else {
      leds[i] = CRGB::Green;
    }
  }
  FastLED.show();
  if (myDelay(500)) { return; }
  
  for(int i=0; i < NUM_LEDS; i++) {
    if (i % 2 == 0) {
      leds[i] = CRGB::Green;
    } else {
      leds[i] = CRGB::Red;
    }
  }
  FastLED.show();
  if (myDelay(500)) { return; }

}

void blueAndWhite() {
  int offset=0;
  CRGB palette[] = { CRGB::Blue, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black };
  int index=0;
  for(int k=0; k<6; k++) {
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = palette[index];
      index++;
      if (index >= 6) { index=0; }
    }
    FastLED.show();
    if (myDelay(500)) { return; }

    for (int j=0; j<16; j++) {
      fadeToBlackBy( leds, NUM_LEDS, 32);
      FastLED.show();
      if ( myDelay(25) ) { return; }
    }
  }
}

void scroll(int hue, int sat, int speed) {
 for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, sat, 255);
    FastLED.show(); 
    // fade all LEDs
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(192); }
    if (myDelay(speed)) { return; }
  }
}

void scrollReverse(int hue, int sat, int speed) {
 for(int i = NUM_LEDS-1; i >= 0; i--) {
    leds[i] = CHSV(hue, sat, 255);
    FastLED.show(); 
    // fade all LEDs
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(192); }
    if (myDelay(speed)) { return; }
  }
}

void sparkle() {
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( 160, (random16(2) ? 200 : 0), 255);
  FastLED.show();
  if (myDelay(50)) { return; }
}

void colourFade() {
  for (int h = 0; h < 255; h++) {
    fill_solid(leds, NUM_LEDS, CHSV(h, 255, 255));
    FastLED.show();
    if (myDelay(75)) { return; }
  }
}
