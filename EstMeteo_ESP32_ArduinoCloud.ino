/*
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/39097333-1176-4355-9b74-981227efcb62

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float hum;
  float patm;
  float temp;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"
#include <Arduino.h>
//==TEMPERATURA==//
#include <DHTesp.h>
#include <Adafruit_Sensor.h>

//==PRESIÓN==//
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

//==CONEXIÓN INTERNET==//
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


//==TEMPERATURA==//
DHTesp dht;
int chk;

//==PRESIÓN==//
#define BMP_SCK  (18)
#define BMP_MISO (19)
#define BMP_MOSI (23)
#define BMP_CS   (5)
Adafruit_BMP280 bmp; // I2C


//==WIFI==//
char ssid[] = "TWK";   // your network SSID (name)
char password[] = "0399821-293830";   // your network password

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

void setup_wifi()
{
  int connectTries = 0;

  WiFi.mode(WIFI_OFF);
  delay(200);
  WiFi.mode(WIFI_STA);

  Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");


    if (connectTries > 20) setup_wifi();
    connectTries++;
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(" ");
}

void setup() {
    // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);
  
  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  Serial.println(F("Conexión MeteoTWK"));
  Serial.println(" ");
  setup_wifi();
  
  //TEMPERATURA
  Serial.println(F("DHT22 test!"));
  dht.setup(5, DHTesp::DHT22);


  //PRESIÓN
  while ( !Serial ) delay(250);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bmp.begin(0x76);
  

  
}

void loop() {
  ArduinoCloud.update();

  //LEER DATOS
  hum = dht.getHumidity();
  temp = dht.getTemperature();
  patm = bmp.readPressure() / 100;
 
 Serial.println(hum);
 Serial.print("%");
 Serial.println(temp);
 Serial.print("ºC");
 Serial.println(patm);
 Serial.print("hPa");
 
  delay(2000);


}



/*
  Since Hum is READ_WRITE variable, onHumChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onHumChange()  {
  // Add your code here to act upon Hum change
}

/*
  Since Patm is READ_WRITE variable, onPatmChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onPatmChange()  {
  // Add your code here to act upon Patm change
}

/*
  Since Temp is READ_WRITE variable, onTempChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onTempChange()  {
  // Add your code here to act upon Temp change
}
