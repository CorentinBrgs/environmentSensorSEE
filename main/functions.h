#include <Arduino.h>
#include <SparkFun_SCD30_Arduino_Library.h>

#include "global.h"

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

bool startModule () 
{
	#ifdef DEBUGMODE
		Serial.println("----Starting MKRWAN configuration----");
		Serial.print("    Starting module with regional band EU868...");
	#endif

	bool moduleStarted = modem.begin(EU868);
	
	#ifdef DEBUGMODE
		if (moduleStarted) 
		{
			Serial.println("Module started with success !");
		}
		else 
		{
			Serial.println("Module failed to start !");
			while (1) {}
		}

		Serial.print("        Module version is: ");
		Serial.println(modem.version());
		Serial.print("        Device EUI is: ");
		Serial.println(modem.deviceEUI());
	#endif

	return moduleStarted;
}

uint8_t OTAAConnection() 
{
	#ifdef DEBUGMODE
		Serial.print("    Connecting through OTAA...");
	#endif

	uint8_t connectionStatus = modem.joinOTAA(appEui, appKey);
	
	#ifdef DEBUGMODE
		if (connectionStatus > 0) 
		{
			Serial.println("done !");
			//Serial.print("returns "")
			//Serial.println(connectionStatus);
		}
		else
		{
			Serial.println("failed !");
			while (1) {}
		}
	#endif

	return connectionStatus;
}

String receiveDataOverLora()
{
	String rcv;
	rcv.reserve(64);	//allocating memory for received data buffer
	while (modem.available()) 
	{
		rcv += (char)modem.read(); //modem.read() returns the first byte of incoming LoRaWAN data available (or -1 if no data is available) 
	}

	#ifdef DEBUGMODE
		if (rcv != "") 
		{
			Serial.print("Received: " + rcv + " - ");
			for (unsigned int i = 0; i < rcv.length(); i++) {
				Serial.print(rcv[i] >> 4, HEX);
				Serial.print(rcv[i] & 0xF, HEX);
				Serial.print(" ");
		    }
		}
	#endif

	return rcv;
}

bool sendDataOverLora(uint32_t readingInterval, environmentData* data)
{
	if (millis() - lastReadingTime > readingInterval) 
	{
		
		#ifdef DEBUGMODE
			Serial.println("Sending sensors data :");
			Serial.print(" - temp : ");
			Serial.println(data->temp);
			Serial.print(" - CO2Rate : ");
			Serial.println(data->rate);
			Serial.print(" - Humidity : ");
			Serial.println(data->humidity);
		#endif
		
		lastReadingTime = millis();
		modem.beginPacket();
		modem.write(frame_type);
		buffer[0] = (data->temp >> 8) & 0xFF;
		buffer[1] = (data->temp & 0xFF);
		buffer[2] = (data->rate >> 8) & 0xFF;
		buffer[3] = (data->rate & 0xFF);
		buffer[4] = data->humidity & 0xFF;
		modem.write(buffer, 5);

		bool err = modem.endPacket(true);
		#ifdef DEBUGMODE
			if (err)
			{
				Serial.println("Data sent correctly!");
			} 
			else
			{
				Serial.println("Error sending Data :(");
			}
		#endif
		return err;
	}
}

void formatData(environmentData* data)
{
	data->temp = round(data->rawTemp * 100);
	data->rate = data->rawRate;
	data->humidity = round(data->rawHumidity);
}


bool getSensorData(environmentData* data)
{

	sensor.setMeasurementInterval(30); //Change number of seconds between measurements: 2 to 1800 (30 minutes)
	// #ifdef DEBUGMODE 
	// #endif 

	if (sensor.dataAvailable())
	{
		data->rawRate = sensor.getCO2();
		data->rawTemp = sensor.getTemperature();
		data->rawHumidity = sensor.getHumidity();

		#ifdef DEBUGMODE

		 	Serial.println("Getting Data drom sensor...");

			Serial.print(" - co2(ppm):");
			Serial.print(data->rawRate);

			Serial.print(" - temp(C):");
			Serial.print(data->rawTemp);

			Serial.print(" - humidity(%):");
			Serial.print(data->rawHumidity);

			Serial.println();
		#endif 

		return true;
	}
	else
	{
		// #ifdef DEBUGMODE
		// 	Serial.print(".");
		// #endif
		return false;
	}
}

#endif 
