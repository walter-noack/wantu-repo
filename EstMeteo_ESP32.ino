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


////===DEFINICIONES===////
String Humedad = "humedad";
String Temperatura = "temperatura";
String PresionAtm = "presion-atmosferica";

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

//==WIFI==//
char ssid[] = "TWK";   // your network SSID (name)
char password[] = "0399821-293830";   // your network password

//==CONEXIÓN CON UBIDOTS POR MQTT==//
const char* mqtt_server = "industrial.api.ubidots.com";
#define mqtt_port 1883

const char topic[] = "/v1.6/devices/meteotwk-esp32";
String device = "meteotwk-esp32";
String data_mqtt = "";
char mensaje[110] = "";
char vacio[] = "";
unsigned int len_msg = 0;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

//==FUNCIONES==//

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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    // Crea ID aleatoria
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    const char username[] = "BBFF-3ytUOn6v24PlrRMeC0XtysnOjcLAtN";
    const char pass[] = " ";

    // Prueba de conexión
    if (client.connect(clientId.c_str(), username, pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic, "{\"presion atmosferica\": {\"value\" : 1}}");

      // ... and resubscribe
      client.subscribe("/v1.6/devices/meteotwk-esp32/lv");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(9600);
  Serial.println(F("Conexión MeteoTWK"));
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
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
  //Chequea conexión a broker MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

 //LEER DATOS
  hum = dht.getHumidity();
  temp = dht.getTemperature();
  patm = bme.readPressure() / 100;

  //ENVÍO A UBIDOTS

  strcpy(mensaje, vacio);
  String value = String(random(0, 100));
  String Hum = String(hum);
  String Temp = String(temp);
  String Patm = String(patm);

  data_mqtt = "{\"";
  data_mqtt += Humedad;
  data_mqtt += "\":{\"value\": ";
  data_mqtt += Hum;
  data_mqtt += "},\"";
  data_mqtt += Temperatura;
  data_mqtt += "\":{\"value\": ";
  data_mqtt += Temp;
  data_mqtt += "},\"";
  data_mqtt += PresionAtm;
  data_mqtt += "\":{\"value\": ";
  data_mqtt += Patm;
//  data_mqtt += "}\"";
  data_mqtt += "}}";

  len_msg = data_mqtt.length() + 1;
  data_mqtt.toCharArray(mensaje, len_msg);

  unsigned long now = millis();
  if (now - lastMsg > 80000) {
    lastMsg = now;
    Serial.println(topic);
    Serial.println(mensaje);
    client.publish(topic, mensaje);
    
  }


}
