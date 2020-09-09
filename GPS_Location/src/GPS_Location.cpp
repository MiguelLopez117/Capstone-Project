/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "c:/Users/User/Documents/IoT/GPS_Location/src/GPS_Location.ino"
/*
 * Project GPS_Location
 * Description: Getting GPS location for Capstone Project
 * Author:  Miguel Lopez
 * Date:  9/8/20
 */


#include "Particle.h"
#include "TinyGPS++.h"
#include "Adafruit_SSD1306.h"

void setup();
void loop();
void displayGPSLocation(float lat, float lon, int sat, int hr, int mn);
void displayInfo();
#line 13 "c:/Users/User/Documents/IoT/GPS_Location/src/GPS_Location.ino"
#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

/*
 * GPS Module Connections
 * GPS Unit Rx connects to Argon Pin D9  (UART1_TX)
 * GPS Unit Tx connects to Argon Pin D10 (UART1_RX)
*/

SYSTEM_THREAD(ENABLED);

const unsigned long PUBLISH_PERIOD = 120000;
const unsigned long SERIAL_PERIOD = 5000;
const unsigned long MAX_GPS_AGE_MS = 10000; // GPS location must be newer than this to be considered valid

// The TinyGPS++ object
TinyGPSPlus gps;
const int UTC_offset = -6; 
unsigned long lastSerial = 0;
unsigned long lastPublish = 0;
unsigned long startFix = 0;
bool gettingFix = false;

void setup()
{
	 Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

	// The GPS module initialization
	Serial1.begin(9600);
  startFix = millis();
  gettingFix = true;
}

void loop()
{
	while (Serial1.available() > 0) 
  {
		if (gps.encode(Serial1.read())) 
    {
			displayInfo();
      //displayGPSLocation();
		}
	}
	delay(1000);
}

void displayGPSLocation(float lat, float lon, int sat, int hr, int mn)
{
  display.clearDisplay();
  display.setRotation(0);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.printf("GPS Location\n");
  display.printf("lat: %f, long: %f\n", lat, lon);
	display.printf("Time: %i:%i\n", hr, mn);
  display.printf("sat = %i\n", sat);
  display.display();
}

void displayInfo() 
{
	float lat,lon;
	uint8_t hr,mn,se,sat;
	if (millis() - lastSerial >= SERIAL_PERIOD) 
  {
		lastSerial = millis();

		char buf[128];
		if (gps.location.isValid() && gps.location.age() < MAX_GPS_AGE_MS) 
    {
			lat = gps.location.lat();
			lon = gps.location.lng(); 
			sat = gps.satellites.value();
			hr = gps.time.hour();
			mn = gps.time.minute();
			se = gps.time.second();

			if(hr > 7) 
      {
				hr = hr + UTC_offset;
			}
			else 
      {
				hr = hr + 24 + UTC_offset;
			}
			Serial.printf("Time: %02i:%02i:%02i --- ",hr,mn,se);
			Serial.printf("lat: %f, long: %f \n", lat,lon);
			displayGPSLocation(lat, lon, sat, hr, mn);
			if (gettingFix) 
      {
				gettingFix = false;
				unsigned long elapsed = millis() - startFix;
				Serial.printlnf("%lu milliseconds to get GPS fix", elapsed);
			}
		}
		else 
    {
			strcpy(buf, "no location");
			if (!gettingFix) 
      {
				gettingFix = true;
				startFix = millis();
			}
		}
	}
}