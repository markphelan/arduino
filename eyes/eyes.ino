#include <FastLED.h>

#define NUM_EYES 10 // The number of PAIRS of eyes (total will be twice this)
#define FADE_SPEED 20 // How quick the eyes fade out (1-255)
#define EYE_LIKELIHOOD 20 // Higher number gives more eyes at one time (1-255)

#define DATA_PIN 2
#define CHIPSET WS2811
#define COLOR_ORDER RGB

CRGB leds[NUM_EYES*2];
CRGB colours[] = {CRGB(100,0,255), CRGB::Blue, CRGB::Green, CRGB::Red};
int numColours = sizeof(colours) / sizeof(colours[0]);

void setup() {
  delay(1000);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_EYES*2);
  
  // Quick palette test
  for (int i=0; i<numColours; i++) {
    FastLED.clear();
    fill_solid( leds, NUM_EYES*2, colours[i]);
    FastLED.show();
    FastLED.delay(500);
  }
  FastLED.clear();
}

void loop() {
  if (random8() < EYE_LIKELIHOOD) {
    int pair = random8(NUM_EYES)*2;
    if (leds[pair].r == 0 && leds[pair].g == 0 && leds[pair].b == 0) {
      CRGB colour = colours[random8(numColours)];
      leds[pair] = colour;
      leds[pair+1] = colour;
    }
  }
  fadeToBlackBy(leds, NUM_EYES*2, FADE_SPEED);
  FastLED.show();
  FastLED.delay(100);
}



