#include <FastLED.h>

#define LED_PIN 6
#define NUM_LEDS 256
#define WIDTH 16
#define HEIGHT 16
#define BRIGHTNESS 60

CRGB leds[NUM_LEDS];

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
  float r = 6.0;          // radio del círculo
  float edge = 1.5;       // grosor del borde difuminado

  float t = (sin(millis() * 0.001) + 1.0) * 0.5;

  CRGB circleColor = blend(CRGB::Yellow, CRGB::Red, (uint8_t)(255 * t));
  CRGB bgColor     = blend(CRGB::Red, CRGB::Yellow, (uint8_t)(255 * t));

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      float dx = x - cx;
      float dy = y - cy;
      float dist = sqrt(dx * dx + dy * dy);

      if (dist < r - edge) {
        drawPixelXY(x, y, circleColor);
      }
      else if (dist <= r) {
        float k = (dist - (r - edge)) / edge;   // 0 a 1 en el borde
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