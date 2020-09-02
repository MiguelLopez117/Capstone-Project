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
void simpleSoundTest();
void getTimingOfSound();
void getMicrophoneValues();
void getTriangulationOfSound();
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

int micro1 = A0, micro2 = A1, micro3 = A2;  //Microphone analog inputs
int val1, val2, val3;                       //AnalogRead values
int T1 = 0, T2 = 0, T3 = 0;                 //Timing of sound for each microphone
int threshold = 4000;                       //Threshold that picks up loud sounds

int A;               //Getting A and B values from micros to seconds and multiplying by speed of sound
int B;               

int a; 
int b;      
int c;  
int T;     //Quadratic Formula pluging a,b,c values
int x;
int y;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);

  pixel.begin();
  pixel.clear();

  pinMode(micro1, INPUT);
  pinMode(micro2, INPUT);
  pinMode(micro3, INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  //simpleSoundTest();
  //getTimingOfSound();
  getTriangulationOfSound();
}

void simpleSoundTest()
{
  getMicrophoneValues();
   if(val1 > threshold || val2 > threshold || val3 > threshold)
   {
     Serial.printf("mic1 = %i | mic2 = %i | mic3 = %i | Time = %i\n",val1,val2,val3);
   }
}

void getTimingOfSound()
{
  T1 = 0;
  T2 = 0;
  T3 = 0;
  Serial.println("waiting for sound");
  while((T1 == 0) || (T2 == 0) || (T3 == 0))
  {
    getMicrophoneValues();
    if((T1 == 0) && (val1 > threshold))
    {
      T1 = micros();
    }
    if((T2 == 0) && (val2 > threshold))
    {
      T2 = micros();
    }
    if((T3 == 0) && (val3 > threshold))
    {
      T3 = micros();
    }
  }
  Serial.printf("T1 = %i | T2 = %i | T3 = %i\n",T1, T2, T3);
}

void getMicrophoneValues()
{
  val1 = analogRead(micro1);
  val2 = analogRead(micro2);
  val3 = analogRead(micro3);
  //Serial.printf("Microphone 1 = %i | Microphone 2 = %i | Microphone 3 = %i\n", val1, val2, val3);
}

void getTriangulationOfSound()
{
  getTimingOfSound();
  A = (T2 - T1)*343;               
  B = (T3 - T1)*343;               
  a = (sq(A) + sq(B)-1);            
  b = (((sq(A)-1)*A) + ((sq(B)-1)*B));      
  c = (sq(sq(A)-1)/4) + (sq(sq(B)-1)/4);  
  T = (-b-sqrt(sq(b)-(4*a*c))/(2*a)); 
  x = -((A*T) + ((sq(A)-1)/2));
  y = -((B*T) + ((sq(B)-1)/2));
  Serial.printf("A = %i | B = %i\n", A, B);
  Serial.printf("T = %i\n", T);
  Serial.printf("X = %i | Y = %i\n", x, y);
}