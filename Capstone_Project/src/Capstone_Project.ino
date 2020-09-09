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
#define PIXEL_PIN2 A3
#define PIXEL_COUNT 31
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel pixelX(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
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
int threshold = 3700;                       //Threshold that picks up loud sounds

int i;                                      //Variable for for loop
const int arraySize = 1024;                 //Size of the array
float soundWaveArray[1024][2];              //Two dimensional array capturing timestamp and soundwave
int lastTime;                               //Timestamp

float A, B;                                 //Getting A and B values from micros to seconds and multiplying by speed of sound             

float a, b, c;                              //Values for Quadratic Formula  
float T;                                    //Quadratic Formula
float X, Y;                                 //Position of Sound

float locationX, locationY;                   //Utilizing map-function to displah location using neopixels like on a quadrant
float pointX, pointY;
float longitude, latitude;

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

  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) 
  {
    sd.initErrorHalt();  
  }

  if (BASE_NAME_SIZE > 6) 
  {
    Serial.println("FILE_BASE_NAME too long");
    while(1);
  }
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  showLocationWithNeopixels();
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
  a = (sq(A) + sq(B)-1);            
  b = (((sq(A)-1)*A) + ((sq(B)-1)*B));      
  c = (sq(sq(A)-1)/4) + (sq(sq(B)-1)/4);  
  T = ((-b-sqrt(sq(b)-(4*a*c)))/(2*a)); 
  X = -((A*T) + ((sq(A)-1)/2));
  Y = -((B*T) + ((sq(B)-1)/2));
  Serial.printf("A = %0.6f | B = %0.6f\n", A, B);
  Serial.printf("T = %0.2f\n", T);
  Serial.printf("X = %0.2f | Y = %0.2f\n", X, Y);
  Serial.printf("a = %0.2f | b = %0.2f | c = %0.2f\n",a,b,c);
}

void showLocationWithNeopixels()
{
  recordLongitudeLatitudeData(longitude, latitude);
  getTriangulationOfSound();
  SDCard();
  pointX = X;
  locationX = map(pointX, 0.0, 1.0, 0.0, 31.0);
  longitude = map(pointX, 0.0, 1.0, -106.65136, -106.6496);
  pixelX.clear();
  pixelX.setPixelColor(locationX,255,0,0);
  pixelX.show();

  pointY = Y;
  locationY = map(pointY, 0.0, 1.0, 0.0, 31.0);
  latitude = map(pointX, 0.0, 1.0, 35.0866796, 35.088413);
  pixelY.clear();
  pixelY.setPixelColor(locationY,0,0,255);
  pixelY.show();
}

void recordLongitudeLatitudeData(float longitude, float latitude)
{
  JsonWriterStatic<256> jw;
  {
    JsonWriterAutoObject obj(&jw);

    jw.insertKeyValue("Longitude", longitude);
    jw.insertKeyValue("Latitude", latitude);
  }
  Particle.publish("Noise",jw.getBuffer(), PRIVATE);
  Serial.printf("Longitude: %0.7f| Latitude: %0.7f\n", longitude, latitude);
}

void soundWaveCapture()
{
  if(val1 > threshold)
  {
    for(i =0 ; i < arraySize ; i++)
    {
      while(micros()-lastTime < 500)
      {
        //do nothing
      }
      lastTime = micros();
      getMicrophoneValues();
      soundWaveArray[i][1] = val1;
    }
  }
}

void SDCard()
{
  Serial.printf("Starting Data Logging \n");

  while (sd.exists(fileName)) 
  {
    if (fileName[BASE_NAME_SIZE + 1] != '9') 
    {
      fileName[BASE_NAME_SIZE + 1]++;
    } 
    else if (fileName[BASE_NAME_SIZE] != '9') 
    {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } 
    else 
    {
      Serial.println("Can't create file name");
      while(1);
    }
  }

  if (!file.open(fileName, O_WRONLY | O_CREAT | O_EXCL)) 
  {
    Serial.println("file.open");
  }
  Serial.printf("Logging to: %s \n",fileName);

  //For Loop here for array capturing sound
  if(val1 > threshold)
  {
    for(i = 0 ; i < arraySize ; i++)
    {
      while(micros()-lastTime < 500)
      {
        //do nothing
      }
      lastTime = micros();
      getMicrophoneValues();
      soundWaveArray[i][0] = micros()/1000000.0;
      soundWaveArray[i][1] = val1;
    }
  }
  for(i = 0; i < arraySize; i++)
  {
    file.printf("%0.8f , %0.2f\n",soundWaveArray[i][0], soundWaveArray[i][1]);
  }
  file.close();
  Serial.printf("Done \n");
  delay(2000);
  Serial.printf("Ready for next data log \n");
}
