
/*
 An example showing linear analogue meter
 */

// Meter colour schemes
#define SECS_PER_MIN  (60UL)
#define RXD2 13
#define TXD2 12
#define PIN 33
#define BLYNK_PRINT Serial
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
const int numReadings = 10;
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Adafruit_PM25AQI.h"
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
char auth[] = "";


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);
int zedLevel = 1; 
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
BlynkTimer timer;
int reading = 0;

void setup(void) {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Blynk.begin(auth, ssid, pass);
  delay(2000);
  timer.setInterval(5000L, blynkPrint);
  tft.begin();
  tft.setRotation(0);
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
  //tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("TOAST",35,20,4);
   tft.drawString("TEST",40,60,4); 
  tft.fillCircle( 70,110,20,TFT_WHITE);
  tft.fillRect(50,100,45,40,TFT_WHITE);
  tft.fillRect(20,120,100,80,TFT_GREEN);
  tft.drawRect(20,120,100,80,TFT_RED);
  
 delay(5000);
  tft.fillScreen(TFT_BLACK);
  strip.begin();
  strip.show(); // initialize all pixels to "off"
  pinMode(0, INPUT_PULLUP);
}

void loop() {
  Blynk.run(); 
  
  strip.show();
  brighten(zedLevel);
  
   PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
    
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }
  if(digitalRead(0) == 0) {
    tell = 0;
    timerTrue = millis();
    maxDust = 1;
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
 if(!tell){
  tft.fillCircle(85,120,70,TFT_RED); 
   tft.setTextColor(TFT_GREEN, TFT_RED);
   
  tft.drawString("INSERT",35,80,4);
   tft.drawString("TOAST",40,140,4);
   
 }
 if(tell){
  timer.run();
  
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
 int flight = map(diff, -100, 100, 1,19);
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
   if(total < 25) zedLevel = 3;
   else if(total > 24 && total < 80) zedLevel = 2;
  else zedLevel = 1;
   
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
void brighten(int q) {
  uint16_t i, j;
  Serial.print("this is q");
  Serial.println(q);
  switch (q) {
  case 1:
    for (j = 5; j < 155; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, j,  0, 0);
    }
    strip.show();
    delay(255/(j+1));
  }

  
  

  for (j = 155; j > 6; j--) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, j, 0, 0);
    }
    strip.show();
    delay(255/(j+1));
    
  }
    break;
  case 2:
    for (j = 5; j < 155; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0,  j, 0);
    }
    strip.show();
    delay(255/(j+1));
  }

  
  

  for (j = 155; j > 6; j--) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, j, 0);
    }
    strip.show();
    delay(255/(j+1));
    
  }
    break;
  case 3:
   for (j = 5; j < 155; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0,  0, j);
    }
    strip.show();
    delay(255/(j+1)); 
  }

  
  

  for (j = 155; j > 6; j--) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, j);
    }
    strip.show();
    delay(255/(j+1));
    
  }
    break;



 
}
}
void blynkPrint(){
  Blynk.virtualWrite(V4, total);
}
