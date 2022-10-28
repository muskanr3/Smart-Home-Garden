// THINGSPEAK CODE
#include <PubSubClient.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

// ONE M2M  INCLUDES
#include "time.h"
#include <WiFiClient.h>
#include <WebServer.h>
#include "HTTPClient.h"


// BMP DEFINITIONS
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;


// DHT DEFINITIONS
#define DHTTYPE DHT11
const int dht_dpin = 26;
DHT dht(dht_dpin, DHTTYPE);
float bhum = 0;

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
int port = 1883;

// WATER LEVEL
#define powpin 18
#define sigpin 34
int waterLevelThreshold = 500;


// ESTABLISHING THINGSPEAK CONNECTION
WiFiClient client;
PubSubClient mqttClient(client);

// SOIL MOISTURE
int soilMoisturePin = 35;
int soilMoistureThreshold = 30;

// LED AND BUZZER
#define smLEDpin 13
#define BUZZpin 27


// ONE M2M DETAILS
String cse_ip = "192.168.211.224"; // YOUR IP from ipconfig/ifconfig 192.168.211.224
String cse_port = "8080";
String serverom2m = "http://192.168.211.224:8080//in-cse/in-name/"; //+ cse_ip + ":" + cse_port + "//in-cse/in-name/";


// BMP oneM2M
String ae = "bmp";
String cnt = "bmpnode"; // For Pressure
String cnt3 = "bmptemp";

// Soil Moisture oneM2M
String ae1 = "soilMoisture";
String cnt1 = "soilnode";

// DHT oneM2M
String ae2 = "dhtnew";
String cnt2 = "dhtnode";

// Water Level oneM2M
String ae3 = "WaterLevel";
String cnt4 = "waterLvlNode";

void createCI(String val, String ae, String cnt)
{
  HTTPClient http;
  http.begin(serverom2m + ae + "/" + cnt + "/");

  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": \"" + String(val) + "\"}}");

  //Serial.println(code);
  if (code == -1) {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

void setup()
{
  Serial.begin(9600);
  dht.begin();

  pinMode(powpin, OUTPUT);
  digitalWrite(powpin, LOW);
  pinMode(smLEDpin, OUTPUT);
  digitalWrite(smLEDpin, HIGH);
  pinMode(BUZZpin, OUTPUT);
  digitalWrite(BUZZpin, HIGH);

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
  digitalWrite(smLEDpin, LOW);
  digitalWrite(BUZZpin, HIGH);

  //  CHECKING MQTT STATUS
  while (mqttClient.connected() == NULL) {
    Serial.println("Connecting to mqtt...");
    mqttClient.connect(clientID, mqttUserName, mqttPass);
    delay(1000);
  }
  Serial.println("connected");
  mqttClient.loop();

  //   READING FROM DHT

  float hum = dht.readHumidity();
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

  //  READING FROM BMP

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

  //READING FROM WATER LEVEL
  digitalWrite(powpin, HIGH);
  delay(10);
  int WaterLevelValue = analogRead(sigpin);
  digitalWrite(powpin, LOW);

  Serial.print("Water Sensor value: ");
  Serial.println(WaterLevelValue);

  if (WaterLevelValue < waterLevelThreshold)
  {
    digitalWrite(BUZZpin, LOW);
  }

  dataString = "field3=" + String(WaterLevelValue);
  topicString = "channels/" + String( writeChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), dataString.c_str() );

  //READING FROM SOIL MOISTURE
  float soilMoistureValue = analogRead(soilMoisturePin);
  Serial.print("Soil Moisture Percentage: ");
  soilMoistureValue = soilMoistureValue / 4095;
  float Moisture_percentage = soilMoistureValue * 100;
  float percent = 100 - Moisture_percentage;
  Serial.println(percent);

  if (percent < soilMoistureThreshold)
  {
    digitalWrite(smLEDpin, HIGH);
  }

  dataString = "field4=" + String(percent);
  topicString = "channels/" + String( writeChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), dataString.c_str() );

  //   ONE M2M STARTS HERE

  String tempVal = (String)temp;
  createCI(tempVal, ae, cnt3);
  String preVal = (String) (pressure / 100);
  createCI(preVal, ae, cnt);
  String humVal = (String) bhum;
  createCI(humVal, ae2, cnt2);
  String smVal = (String)percent;
  createCI(smVal, ae1, cnt1);
  String wlVal = (String) WaterLevelValue;
  createCI(wlVal, ae3, cnt4);

  Serial.println();

  delay(3000);
}
