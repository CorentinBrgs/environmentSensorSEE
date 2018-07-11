#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_SCD30_Arduino_Library.h>

#include "functions.h"
#include "global.h"

#define BAUDRATE 115200


void setup() 
{
	//while (!Serial); 
		/*mandatory so that the board does wait for the Serial connection to be opened
		  Note : disabled by default because it seems to cause OTAA activation problems for some reason*/
	
	//For random generation only
	//randomSeed(micros());
	//----

	Serial.begin(BAUDRATE);

	Wire.begin();
	sensor.begin(); //measures will occur every 30 sec.
	//My desk is ~1600m above sealevel
	//airSensor.setAltitudeCompensation(1600); //Set altitude of the sensor in m
	//Pressure in Boulder, CO is 24.65inHg or 834.74mBar
	//airSensor.setAmbientPressure(835); //Current ambient pressure in mBar: 700 to 1200

	startModule();
	OTAAConnection();
	lastReadingTime = millis();
}


void loop() 
{
	receivedData = receiveDataOverLora();
	if (getSensorData(&data))
	{
		formatData(&data);
		bool errStatus = sendDataOverLora(READING_INTERVAL, &data);
	}
}
