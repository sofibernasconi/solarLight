// SUN (Stored Until Needed)
// Sun animation on a 16x16 WS2812B LED matrix (4 x 8x8 panels)

#include <FastLED.h>

#define LED_PIN 2
#define NUM_LEDS 256
#define WIDTH 16
#define HEIGHT 16
#define BRIGHTNESS 60

CRGB leds[NUM_LEDS];

// COLOR PALETTE: change colors here only
CRGB COLOR_YELLOW = CRGB(255, 100, 0);
CRGB COLOR_RED    = CRGB(255, 0, 0);
CRGB COLOR_ORANGE = CRGB(255, 100, 0);

// choose which colors the animation uses
CRGB CIRCLE_COLOR_A = COLOR_YELLOW;
CRGB CIRCLE_COLOR_B = COLOR_RED;

CRGB BG_COLOR_A = COLOR_RED;
CRGB BG_COLOR_B = COLOR_YELLOW;

// maps (x, y) coordinates to the LED index across the four 8x8 panels
uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t panelX = x / 8;
  uint8_t panelY = y / 8;

  uint8_t localX = x % 8;
  uint8_t localY = y % 8;

  uint8_t panelIndex = panelY * 2 + panelX;

  return panelIndex * 64 + localY * 8 + localX;
}

void drawPixelXY(int x, int y, CRGB color) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
  leds[XY(x, y)] = color;
}

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  float cx = 7.5;
  float cy = 7.5;
  float r = 6.5; // circle radius
  float edge = 1.5;

  float t = (sin(millis() * 0.0005) + 1.0) * 0.5; // speed

  CRGB circleColor = blend(CIRCLE_COLOR_A, CIRCLE_COLOR_B, (uint8_t)(255 * t));
  CRGB bgColor     = blend(BG_COLOR_A, BG_COLOR_B, (uint8_t)(255 * t));

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      float dx = x - cx;
      float dy = y - cy;
      float dist = sqrt(dx * dx + dy * dy);

      if (dist < r - edge) {
        drawPixelXY(x, y, circleColor);
      }
      else if (dist <= r) {
        float k = (dist - (r - edge)) / edge;
        CRGB edgeColor = blend(circleColor, bgColor, (uint8_t)(255 * k));
        drawPixelXY(x, y, edgeColor);
      }
      else {
        drawPixelXY(x, y, bgColor);
      }
    }
  }

  FastLED.show();
  delay(20);
}
