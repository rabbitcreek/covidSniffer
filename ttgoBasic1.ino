
/*
 An example showing linear analogue meter
 */

// Meter colour schemes
#define RED2RED     0
#define GREEN2GREEN 1
#define BLUE2BLUE   2
#define BLUE2RED    3
#define GREEN2RED   4
#define RED2GREEN   5
#define RAINBOW     6

#include <TFT_eSPI.h>
#include <SPI.h>
float total = 0;
float maximum = 1000;
float timerTrue = 0.0;
int bight = 0;
TFT_eSPI tft = TFT_eSPI();

int reading = 0;

void setup(void) {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  timerTrue = millis();
  total = maximum;
}

void loop() {
  
  tft.setRotation(0);
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 //tft.drawCentreString("NUMBER",132,5,4);
 tft.drawNumber(int(total),5,5,7);
 tft.drawNumber(int(maximum),40,220,4);
 tft.setRotation(1);
 int percent = ((total/maximum) * 100);
 tft.drawNumber(int(percent),60,110,4);
  int clocker = (millis() - timerTrue)/1000; 
 tft.drawNumber(clocker,140,110,4);
 reading = map(total,0, maximum, 1,19);
 
  linearMeter(reading, 55,  10, 5, 35, 3, 19, RED2RED);
  linearMeter(reading, 55,  60, 5, 35, 3, 19, RED2RED);
  //linearMeter(reading, 10,  40, 5, 25, 3, 20, GREEN2GREEN);
  //linearMeter(reading, 10,  70, 5, 25, 3, 20, BLUE2BLUE);
  //linearMeter(reading, 10, 100, 5, 25, 3, 20, BLUE2RED);
  //linearMeter(reading, 10, 130, 5, 25, 3, 20, GREEN2RED);
  //linearMeter(reading, 10, 160, 5, 25, 3, 20, RED2GREEN);
 //linearMeter(reading, 10, 190, 5, 25, 3, 20, RAINBOW);
   total = total - int(random(2,60));
Serial.println(total);
  delay (1000);
  tft.fillScreen(TFT_BLACK);
}

// #########################################################################
//  Draw the linear meter
// #########################################################################
// val =  reading to show (range is 0 to n)
// x, y = position of top left corner
// w, h = width and height of a single bar
// g    = pixel gap to next bar (can be 0)
// n    = number of segments
// s    = colour scheme
void linearMeter(int val, int x, int y, int w, int h, int g, int n, byte s)
{
  // Variable to save "value" text colour from scheme and set default
  int colour = TFT_BLUE;
  // Draw n colour blocks
  for (int b = 1; b <= n; b++) {
    if (val > 0 && b <= val) { // Fill in coloured blocks
      switch (s) {
        case 0: colour = TFT_BLUE; break; // Fixed colour
        case 1: colour = TFT_GREEN; break; // Fixed colour
        case 2: colour = TFT_BLUE; break; // Fixed colour
        case 3: colour = rainbowColor(map(b, 0, n, 127,   0)); break; // Blue to red
        case 4: colour = rainbowColor(map(b, 0, n,  63,   0)); break; // Green to red
        case 5: colour = rainbowColor(map(b, 0, n,   0,  63)); break; // Red to green
        case 6: colour = rainbowColor(map(b, 0, n,   0, 159)); break; // Rainbow (red to violet)
      }
      tft.fillRect(x + b*(w+g), y, w, h, colour);
    }
    else // Fill in blank segments
    {
      tft.fillRect(x + b*(w+g), y, w, h, TFT_RED);
    }
  }
}

/***************************************************************************************
** Function name:           rainbowColor
** Description:             Return a 16 bit rainbow colour
***************************************************************************************/
  // If 'spectrum' is in the range 0-159 it is converted to a spectrum colour
  // from 0 = red through to 127 = blue to 159 = violet
  // Extending the range to 0-191 adds a further violet to red band
 
uint16_t rainbowColor(uint8_t spectrum)
{
  spectrum = spectrum%192;
  
  uint8_t red   = 0; // Red is the top 5 bits of a 16 bit colour spectrum
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  uint8_t sector = spectrum >> 5;
  uint8_t amplit = spectrum & 0x1F;

  switch (sector)
  {
    case 0:
      red   = 0x1F;
      green = amplit; // Green ramps up
      blue  = 0;
      break;
    case 1:
      red   = 0x1F - amplit; // Red ramps down
      green = 0x1F;
      blue  = 0;
      break;
    case 2:
      red   = 0;
      green = 0x1F;
      blue  = amplit; // Blue ramps up
      break;
    case 3:
      red   = 0;
      green = 0x1F - amplit; // Green ramps down
      blue  = 0x1F;
      break;
    case 4:
      red   = amplit; // Red ramps up
      green = 0;
      blue  = 0x1F;
      break;
    case 5:
      red   = 0x1F;
      green = 0;
      blue  = 0x1F - amplit; // Blue ramps down
      break;
  }

  return red << 11 | green << 6 | blue;
}