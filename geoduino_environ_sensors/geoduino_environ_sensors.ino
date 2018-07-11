/*
  This sketch joins the LoRa network in OTAA protocol.
  Then, send sensor informations.
*/
#include <MKRWAN.h>

// Libraries for Dallas Temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// Initialize the Serial communication with LoRaModem
LoRaModem modem(Serial1);

// Configuration for OTAA protocol
//const char *appEui = "70B3D57ED00105B0";
//const char *appKey = "C9F62ED90C41C95689561EFF49D46587";

const char *appEui = "70B3D57ED0010757";
const char *appKey = "C2E610FBF84F2533FEE886C34394C799";

// Temperature sensor
// Data wire is plugged into pin 14 on the Arduino
#define ONE_WIRE_BUS 14
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Protocol parameters
char frame_type = 0;

unsigned long lastReadingTime = 0;		// last time you read the sensor, in milliseconds
const unsigned long ReadingInterval = 30000;//30000L; // delay between 2 readings, in milliseconds

void setup_temperature() {
  // Start up the library
  sensors.begin();
}

float get_temperature() {
  sensors.requestTemperatures(); // Send the command to get temperature readings
  return sensors.getTempCByIndex(0);
  // You can have more than one DS18B20 on the same bus. 
  // 0 refers to the first IC on the wire
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(115200);
//  while (!Serial);
  Serial.println("Welcome to MKRWAN1300 first configuration sketch");
  Serial.println("Register to your favourite LoRa network and we are ready to go!");
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
	Serial.println("Failed to start module");
	while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
	Serial.println("Something went wrong; are you indoor? Move near a window and retry");
	while (1) {}
  }
  else
  {
	Serial.print("connected return: ");
	Serial.println(connected);
  }

  setup_temperature();
  
  lastReadingTime = millis();
}

void loop() {
  float temp;
  short temperature;
  char buffer[2];
  while (modem.available()) {
	Serial.write(modem.read());
  }
  //modem.poll(); 

  if (millis() - lastReadingTime > ReadingInterval) {
	lastReadingTime = millis();
	
	temp = get_temperature();
	Serial.print("Temp: ");
	Serial.println(temp);

	// Led to indicate LoRa frame
	digitalWrite(LED_BUILTIN, HIGH);
	
	modem.beginPacket();
	// Frame type
	modem.write(frame_type);
	// Temperature 1/100°C signed
	temperature = temp*100;
	buffer[0] = (temperature >> 8) & 0xFF;
	buffer[1] = temperature & 0xFF;
	modem.write(buffer,2);
	int err = modem.endPacket(true);
	
	if (err > 0) {
	  Serial.println("Message sent correctly!");
	} 
	else {
	  Serial.println("Error sending message :(");
	}
	digitalWrite(LED_BUILTIN, LOW);
  }
}
