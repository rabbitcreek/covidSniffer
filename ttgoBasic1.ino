
/*
 An example showing linear analogue meter
 */

// Meter colour schemes
#define SECS_PER_MIN  (60UL)
#define RXD2 13
#define TXD2 12
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
const int numReadings = 10;
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Adafruit_PM25AQI.h"
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
int minutes;
int seconds;
int changer = 0;
float total = 0;
float maxDust = 1;
float timerTrue = 0.0;
int bight = 0;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total2 = 0;                  // the running total
int average = 0;                // the average
bool tell = 0;
TFT_eSPI tft = TFT_eSPI();

int reading = 0;

void setup(void) {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  timerTrue = 0;
  total = 0;
  if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }
  Serial.println("PM25 found!");
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
   PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
    
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");
if((data.pm25_standard >= 1000) && (maxDust < data.pm25_standard)) {   
  tell = 1;
  timerTrue = millis();
  maxDust = data.pm25_standard;
  }
  tft.setRotation(0);
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 //total = data.pm25_standard;
 //tft.drawCentreString("NUMBER",132,5,4);
 tft.drawNumber(int(total),5,5,7);
 if(tell){
 tft.drawNumber(int(maxDust),40,220,4);
 tft.setRotation(1);
 int percent = ((total/maxDust) * 100);
 tft.drawNumber(int(percent),60,110,4);
 tft.drawString("%",100,110,4);
  int clocker = (millis() - timerTrue)/1000;
  time(clocker);

 tft.drawNumber(minutes,140,110,4);
 tft.drawString(":",160,110,4);
 tft.drawNumber(seconds,170,110,4);
 reading = map(total,0, maxDust, 20,0);
 int diff = total - data.pm25_standard;
 int flight = map(diff, -20, 20, 1,19);
 linearMeter(flight, 55,  10, 5, 35, 3, 19, 0);
 linearMeter(reading, 55,  60, 5, 35, 3, 19, 0);
 }
    //changer = int(random(-100,100));
  total2 = total2 - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = data.pm25_standard;
  // add the reading to the total:
  total2 = total2 + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  total = total2 / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(total);
  
   
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
      colour = TFT_BLUE;
      tft.fillRect(x + b*(w+g), y, w, h, colour);
    }
    else // Fill in blank segments
    {
      tft.fillRect(x + b*(w+g), y, w, h, TFT_RED);
    }
    tft.fillRect(135,10,5,35,TFT_WHITE);
  }
}


void time(long val){  
 minutes = numberOfMinutes(val);
 seconds = numberOfSeconds(val);
}
