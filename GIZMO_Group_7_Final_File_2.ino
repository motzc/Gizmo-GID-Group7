#include <MPR121.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include "Compiler_Errors.h"


#define numElectrodes 12
#define NUMPIXELS      51
#define PIN            8

#define LED_PIN     8
#define NUM_LEDS    51
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB +
                           NEO_KHZ800);

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup()
{

  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;




  pixels.begin(); // This initializes the NeoPixel library.
  pixels.clear();
  pixels.show();
  pixels.setBrightness(10);

  //  for (int i = 0; i < NUMPIXELS; i++) {
  //    pixels.setPixelColor(i, pixels.Color(0,250,0)); // Moderately bright green color.
  //    pixels.show();
  //  }


  Serial.begin(9600);
  while (!Serial); // only needed if you want serial feedback with the
  // Arduino Leonardo or Bare Touch Board

  // 0x5C is the MPR121 I2C address on the Bare Touch Board
  if (!MPR121.begin(0x5A)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }

  // pin 4 is the MPR121 interrupt on the Bare Touch Board
  MPR121.setInterruptPin(6);

  // this is the touch threshold - setting it low makes it more like a proximity trigger
  // default value is 40 for touch
  MPR121.setTouchThreshold(20);

  // this is the release threshold - must ALWAYS be smaller than the touch threshold
  // default value is 20 for touch
  MPR121.setReleaseThreshold(10);

  // initial data update
  MPR121.updateTouchData();
}

void loop()
{
  ChangePalettePeriodically();

  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */

  FillLEDsFromPaletteColors( startIndex);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);


  if (MPR121.touchStatusChanged()) {
    MPR121.updateTouchData();
    //    for(int i=0; i<numElectrodes; i++){
    if (MPR121.isNewTouch(0)) {
      Serial.print("electrode ");
      Serial.print(0, DEC);
      Serial.println(" was just touched");
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 0, 255)); // Moderately bright green color.
        pixels.show();
      }
      delay(2000);
    }

    else if (MPR121.isNewTouch(1)) {
      Serial.print("electrode ");
      Serial.print(1, DEC);
      Serial.println(" was just touched");
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(135, 206, 250)); // Moderately bright green color.
        pixels.show();
      }
      delay(2000);

    }
    else if (MPR121.isNewTouch(2)) {
      Serial.print("electrode ");
      Serial.print(2, DEC);
      Serial.println(" was just touched");
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 255, 0)); // Moderately bright green color.
        pixels.show();
      }
      delay(2000);

    }
    else if (MPR121.isNewTouch(3)) {
      Serial.print("electrode ");
      Serial.print(3, DEC);
      Serial.println(" was just touched");
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 100, 0)); // Moderately bright green color.
        pixels.show();
      }
      delay(2000);

    }
    else if (MPR121.isNewRelease(0)) {
      Serial.print("electrode ");
      Serial.print(0, DEC);
      Serial.println(" was just released");
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    else if (MPR121.isNewRelease(1)) {
      Serial.print("electrode ");
      Serial.print(1, DEC);
      Serial.println(" was just released");
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    else if (MPR121.isNewRelease(2)) {
      Serial.print("electrode ");
      Serial.print(2, DEC);
      Serial.println(" was just released");
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    else if (MPR121.isNewRelease(3)) {
      Serial.print("electrode ");
      Serial.print(3, DEC);
      Serial.println(" was just released");
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if ( lastSecond != secondHand) {
    lastSecond = secondHand;
    if ( secondHand ==  0)  {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
  }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
  for ( int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV( random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;

}
