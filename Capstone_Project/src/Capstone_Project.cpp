/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/User/Documents/IoT/Capstone-Project/Capstone_Project/src/Capstone_Project.ino"
/*
 * Project Capstone_Project
 * Description: Triangulating sound to find precise location
 * Author:  Miguel Lopez
 * Date: 8/27/20
 */

#include <neopixel.h>
#include <SPI.h>
#include <SdFat.h>

void setup();
void loop();
void buttonFunction();
void microphone1();
void microphone2();
void microphone3();
void pixelRed();
void pixelGreen();
#line 12 "c:/Users/User/Documents/IoT/Capstone-Project/Capstone_Project/src/Capstone_Project.ino"
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
int val;

int button = A3;
int buttonState;


// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);

  pixel.begin();
  pixel.clear();

  pinMode(button, INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  buttonFunction();
}

void buttonFunction()
{
  buttonState = digitalRead(button);
  if(buttonState == HIGH)
  {
    pixelGreen();
    //microphone1();
    //microphone2();
    microphone3();
  }
  else
  {
    pixelRed();
  }
}

void microphone1()
{
  val = analogRead(micro1);
  Serial.printf("value = %i\n", val);
}

void microphone2()
{
  val = analogRead(micro2);
  Serial.printf("value = %i\n", val);
}

void microphone3()
{
  val = analogRead(micro3);
  Serial.printf("value = %i\n", val);
}

void pixelRed()
{
  pixel.setPixelColor(0,255,0,0);
  pixel.show();
}

void pixelGreen()
{
  pixel.setPixelColor(0,0,255,0);
  pixel.show();
}