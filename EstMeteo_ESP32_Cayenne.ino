/*
This example shows how to connect to Cayenne using an ESP32 and send/receive sample data.

The CayenneMQTT Library is required to run this sketch. If you have not already done so you can install it from the Arduino IDE Library Manager.

Steps:
1. If you have not already installed the ESP32 Board Package install it using the instructions here: https://github.com/espressif/arduino-esp32/blob/master/README.md#installation-instructions.
2. Select your ESP32 board from the Tools menu.
3. Set the Cayenne authentication info to match the authentication info from the Dashboard.
4. Set the network name and password.
5. Compile and upload the sketch.
6. A temporary widget will be automatically generated in the Cayenne Dashboard. To make the widget permanent click the plus sign on the widget.
*/

#include <Arduino.h>
//==TEMPERATURA==//
#include <DHTesp.h>
#include <Adafruit_Sensor.h>

//==PRESIÓN==//
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>


//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP32.h>

//==TEMPERATURA==//
DHTesp dht;
int chk;
float hum;
float temp;

//==PRESIÓN==//
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)
Adafruit_BMP280 bme; // I2C
float patm;

unsigned long lastMsg = 0;

// WiFi network info.
char ssid[] = "TWK";
char wifiPassword[] = "0399821-293830";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "2571a310-cf24-11ed-b72d-d9f6595c5b9d";
char password[] = "2b4b1b1eef8ea02c8e1fdb357fe3880a7acf7d42";
char clientID[] = "8abbf2f0-cf2b-11ed-b0e7-e768b61d6137";


void setup() {
	Serial.begin(9600);
  Serial.println(F("Conexión MeteoTWK"));
	Cayenne.begin(username, password, clientID, ssid, wifiPassword);

  //TEMPERATURA
  Serial.println(F("DHT22 test!"));
  dht.setup(5, DHTesp::DHT22);


  //PRESIÓN
  while ( !Serial ) delay(250);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bme.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
}

void loop() {
	Cayenne.loop();
 
 //LEER DATOS
  hum = dht.getHumidity();
  temp = dht.getTemperature();
  patm = bme.readPressure() / 100;


 
}

// Default function for sending sensor data at intervals to Cayenne.
// You can also use functions for specific channels, e.g CAYENNE_OUT(1) for sending channel 1 data.
CAYENNE_OUT_DEFAULT()
{
	// Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
	//Cayenne.virtualWrite(0, millis());
	// Some examples of other functions you can use to send data.
unsigned long now = millis();
if (now - lastMsg > 60000) {
    lastMsg = now;
    Cayenne.virtualWrite(1, temp);
    Cayenne.virtualWrite(2, hum);
    Cayenne.virtualWrite(3, patm);
  }
 
	
	//Cayenne.luxWrite(2, 700);
	//Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
}

// Default function for processing actuator commands from the Cayenne Dashboard.
// You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
	CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
	//Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
