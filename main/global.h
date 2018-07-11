#include <Arduino.h>
#include <MKRWAN.h>
#include <SparkFun_SCD30_Arduino_Library.h>

#include "structure.h"

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define DEBUGMODE true //change to false if you don't want to activate the Serial message debuging
#define READING_INTERVAL 10000 // delay between 2 readings, in milliseconds


// Initialize the Serial communication with LoRaModem
LoRaModem modem(Serial1);
SCD30 sensor;


// Configuration for OTAA protocol
const char *appEui = "70B3D57ED0010757";
const char *appKey = "C2E610FBF84F2533FEE886C34394C799";

// Protocol parameters
char frame_type = 0;
unsigned long lastReadingTime = 0;	// last time the sensor has been read, in milliseconds

environmentData data;


char buffer[5];

String receivedData; 		//to store received data

#endif 