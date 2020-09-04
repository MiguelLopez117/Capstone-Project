/*
 * Project: Capstone_Project
 * Description: Triangulating sound to find precise location
 * Author:  Miguel Lopez
 * Date: 8/27/20
 */

#include <JsonParserGeneratorRk.h>
#include <neopixel.h>
#include <SPI.h>
#include <SdFat.h>

#define PIXEL_PIN A4
#define PIXEL_COUNT 31
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel pixelX(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

#define PIXEL_PIN2 A3
#define PIXEL_COUNT 31
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel pixelY(PIXEL_COUNT, PIXEL_PIN2, PIXEL_TYPE);

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

float A, B;               //Getting A and B values from micros to seconds and multiplying by speed of sound             

float a, b, c;        //Values for Quadratic Formula  
float T;              //Quadratic Formula
float X, Y;           //Position of Sound

int locationX, locationY;       //Utilizing map-function to show location using neopixels like in a quadrant
float pointX, pointY;



// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);

  pixelX.begin();
  pixelY.begin();
  pixelX.clear();
  pixelY.clear();

  pinMode(micro1, INPUT);
  pinMode(micro2, INPUT);
  pinMode(micro3, INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  //simpleSoundTest();
  //getTimingOfSound();
  //getTriangulationOfSound();
  showLocationWithNoepixles();
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
  delay(2000);
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
  A = ((T2 - T1)/1000000.0)*343;               
  B = ((T3 - T1)/1000000.0)*343; 
  //A = -0.196;
  //B = 0.473;              
  a = (sq(A) + sq(B)-1);            
  b = (((sq(A)-1)*A) + ((sq(B)-1)*B));      
  c = (sq(sq(A)-1)/4) + (sq(sq(B)-1)/4);  
  //A = -0.196;
  //B = 0.473;
  //a = -0.737;
 // b = -0.178;
  //c = 0.381;
  T = ((-b-sqrt(sq(b)-(4*a*c)))/(2*a)); 
  X = -((A*T) + ((sq(A)-1)/2));
  Y = -((B*T) + ((sq(B)-1)/2));
  Serial.printf("A = %0.6f | B = %0.6f\n", A, B);
  Serial.printf("T = %0.2f\n", T);
  Serial.printf("X = %0.2f | Y = %0.2f\n", X, Y);
  Serial.printf("a = %0.2f | b = %0.2f | c = %0.2f\n",a,b,c);
}

void showLocationWithNoepixles()
{
  somethingGreat(X, Y);
  getTriangulationOfSound();
  pointX = X;
  locationX = map(pointX,0.0,1.0,0.0,31.0);
  pixelX.clear();
  pixelX.setPixelColor(locationX,255,0,0);
  pixelX.show();

  pointY = Y;
  locationY = map(pointY,0.0,1.0,0.0,31.0);
  pixelY.clear();
  pixelY.setPixelColor(locationY,0,0,255);
  pixelY.show();
}

void somethingGreat(float X, float Y)
{
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);

    jw.insertKeyValue("Longitud", X);
    jw.insertKeyValue("Latitude", Y);
  }
  Particle.publish("Noise",jw.getBuffer(), PRIVATE);
}