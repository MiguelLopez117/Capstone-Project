/*
 * Project Capstone_Project
 * Description: Triangulating sound to find precise location
 * Author:  Miguel Lopez
 * Date: 8/27/20
 */

#include <neopixel.h>
#include <SPI.h>
#include <SdFat.h>

#define PIXEL_PIN A4
#define PIXEL_COUNT 1
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel pixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

const int chipSelect = SS;

#define FILE_BASE_NAME "Data"

SdFat sd;
SdFile file;

const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[13] = FILE_BASE_NAME "00.csv";

int micro1 = A0;
int micro2 = A1;
int micro3 = A2;
int val1;
int val2;
int val3; 
int T1 = 0;
int T2 = 0;
int T3 = 0;

int threshold = 4000;
int startTime;
int lastTime;

int button = A3;
int buttonState;


// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);

  pixel.begin();
  pixel.clear();

  pinMode(button, INPUT);
  pinMode(micro1, INPUT);
  pinMode(micro2, INPUT);
  pinMode(micro3, INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  soundtest();
}

void soundtest()
{
  microphones();
  while(T1 = 0 || T2 = 0 || T3 = 0)
  {
    if(T1 = 0 & val1 > threshold)
    {
      T1 = millis();
    }
    if(T2 = 0 & val2 > threshold)
    {
      T2 = millis - T1;
    }
    if(T3 = 0 & val3 > threshold)
    {
      T3 = millis - T2;
    }
  }
}

void buttonFunction()
{
  buttonState = digitalRead(button);
  if(buttonState == HIGH)
  {
    pixelGreen();
    microphones();
  }
  else
  {
    pixelBlack();
  }
}

void microphones()
{
  val1 = analogRead(micro1);
  val2 = analogRead(micro2);
  val3 = analogRead(micro3);
  //Serial.printf("Microphone 1 = %i | Microphone 2 = %i | Microphone 3 = %i\n", val1, val2, val3);
}

void pixelBlack()
{
  pixel.setPixelColor(0,0,0,0);
  pixel.show();
}

void pixelGreen()
{
  pixel.setPixelColor(0,0,255,0);
  pixel.show();
}