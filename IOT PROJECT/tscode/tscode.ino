#include <PubSubClient.h>
#include <WiFi.h>
#include <ThingSpeak.h>

#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;


#define DHTTYPE DHT11
const int dht_dpin = 26;
DHT dht(dht_dpin, DHTTYPE);

//For wifi connection
const char* ssid = "1+";
const char* pass = "12345677";

//mqtt details

const char* server = "mqtt3.thingspeak.com";

const char* mqttUserName = "JgcoBjQkGwcsGBUeKhsFIwc";

const char* mqttPass = "tLyTISgz+zmreEX57sCRugYE";

const char* clientID = "JgcoBjQkGwcsGBUeKhsFIwc";

long writeChannelID = 1769144;
const char* writeAPIKey = "44IL3OXGOLTE0NPF";
float bhum = 0;
int port = 1883;

// Define Water Level Pins
#define powpin 2
#define sigpin 34

WiFiClient client;
PubSubClient mqttClient(client);

int soilMoisturePin = 35;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  delay(2000);
  bmp280.begin(BMP280_I2C_ADDRESS);

  //checking wifi status
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("WiFi Connected!");
  mqttClient.setServer(server, port);
}

void loop()
{
  while (mqttClient.connected() == NULL) {
    Serial.println("Connecting to mqtt...");
    mqttClient.connect(clientID, mqttUserName, mqttPass);
    delay(1000);
  }
  Serial.println("connected to mqtt");
  mqttClient.loop();

  float hum = dht.readHumidity();
  //    float temp = dht.readTemperature();


  if (!isnan(hum))
  {
    bhum = hum;
    Serial.print("Humidity = ");
    Serial.print(hum);
    Serial.println("%");

  }
  else {
    Serial.print("Humidity = ");
    Serial.print(bhum);
    Serial.println("%");

  }

  float temp = bmp280.readTemperature();
  float pressure = bmp280.readPressure();

  Serial.print("temp = ");
  Serial.println(temp);
  Serial.print("pressure= ");
  Serial.println(pressure / 100);

  String dataString = "field7=" + String(pressure / 100);
  String topicString = "channels/" + String( writeChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), dataString.c_str() );

  dataString = "field6=" + String(temp);
  topicString = "channels/" + String( writeChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), dataString.c_str() );


  dataString = "field1=" + String(bhum);
  topicString = "channels/" + String( writeChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), dataString.c_str() );

  digitalWrite(powpin, HIGH);
  int WaterLevelValue = analogRead(sigpin);
  digitalWrite(powpin, LOW);

  Serial.print("Water Sensor value: ");
  Serial.println(WaterLevelValue);

  dataString = "field3=" + String(WaterLevelValue);
  topicString = "channels/" + String( writeChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), dataString.c_str() );


  float soilMoistureValue = analogRead(soilMoisturePin);
  Serial.print("Soil Moisture Percentage: ");
  soilMoistureValue = soilMoistureValue / 4095;
  float Moisture_percentage = soilMoistureValue * 100;
  float percent = 100 - Moisture_percentage;
  Serial.println(percent);

  dataString = "field4=" + String(percent);
  topicString = "channels/" + String( writeChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), dataString.c_str() );


  delay(3000);
}
