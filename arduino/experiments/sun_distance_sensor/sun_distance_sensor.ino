// SUN (Stored Until Needed): distance sensor experiment
// The sun emerges from the center when someone approaches,
// using a VL53L0X time-of-flight distance sensor.
// This version was tested but not used in the final piece.

#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// ---------- LED MATRIX ----------
#define LED_PIN 2
#define NUM_LEDS 256
#define WIDTH 16
#define HEIGHT 16
#define BRIGHTNESS 60

CRGB leds[NUM_LEDS];

// ---------- SENSOR ----------
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Useful sensor range
#define DISTANCE_FAR_MM 600    // from here and further: off
#define DISTANCE_NEAR_MM 120   // here it is fully active

float filteredDistance = DISTANCE_FAR_MM;

// smoothing for the sun's appearance
float smoothedPresence = 0.0;

// ---------- COLORS ----------
CRGB COLOR_YELLOW = CRGB(255, 100, 0);
CRGB COLOR_RED    = CRGB(255, 0, 0);

CRGB CIRCLE_COLOR_A = COLOR_YELLOW;
CRGB CIRCLE_COLOR_B = COLOR_RED;

CRGB BG_COLOR_A = COLOR_RED;
CRGB BG_COLOR_B = COLOR_YELLOW;

// ---------- MAPPING ----------
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

// ---------- READ SENSOR ----------
int readDistance() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    return measure.RangeMilliMeter;
  } else {
    return DISTANCE_FAR_MM + 200;
  }
}

// ---------- EMERGING SUN ANIMATION ----------
void sunAnimation(float presence) {
  FastLED.clear();

  float cx = 7.5;
  float cy = 7.5;

  // The sun grows from the center to its full size
  float rMax = 6.5;
  float r = presence * rMax;

  float edge = 1.5;

  // Original color change
  float t = (sin(millis() * 0.0005) + 1.0) * 0.5;

  CRGB circleColor = blend(CIRCLE_COLOR_A, CIRCLE_COLOR_B, (uint8_t)(255 * t));
  CRGB bgColor     = blend(BG_COLOR_A, BG_COLOR_B, (uint8_t)(255 * t));

  // Overall brightness of the sun as it appears
  uint8_t sunBrightness = presence * 255;

  // The background appears later, once the sun is more formed
  float bgAmount = constrain((presence - 0.55) / 0.45, 0.0, 1.0);
  uint8_t bgBrightness = bgAmount * 180;

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {

      float dx = x - cx;
      float dy = y - cy;
      float dist = sqrt(dx * dx + dy * dy);

      if (dist < r - edge) {
        CRGB c = circleColor;
        c.nscale8(sunBrightness);
        drawPixelXY(x, y, c);
      }
      else if (dist <= r && r > edge) {
        float k = (dist - (r - edge)) / edge;
        CRGB edgeColor = blend(circleColor, bgColor, (uint8_t)(255 * k));
        edgeColor.nscale8(sunBrightness);
        drawPixelXY(x, y, edgeColor);
      }
      else {
        CRGB bg = bgColor;
        bg.nscale8(bgBrightness);
        drawPixelXY(x, y, bg);
      }
    }
  }
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  if (!lox.begin()) {
    Serial.println("SENSOR ERROR");
    while (1);
  }

  Serial.println("Sensor ready");
}

// ---------- LOOP ----------
void loop() {
  int distance = readDistance();

  filteredDistance = filteredDistance * 0.85 + distance * 0.15;

  // converts distance into presence:
  // far = 0, near = 1
  float presence = map(filteredDistance, DISTANCE_FAR_MM, DISTANCE_NEAR_MM, 0, 1000) / 1000.0;
  presence = constrain(presence, 0.0, 1.0);

  // smooths the appearance
  smoothedPresence = smoothedPresence * 0.97 + presence * 0.03;

  Serial.print("Distance: ");
  Serial.print(filteredDistance);
  Serial.print(" | Presence: ");
  Serial.println(smoothedPresence);

  sunAnimation(smoothedPresence);

  FastLED.show();
  delay(20);
}
