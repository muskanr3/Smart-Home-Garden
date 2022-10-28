#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include "time.h"
#include <Wire.h>

#include "WiFi.h"
#include <WiFiClient.h>
#include <WebServer.h>
#include "HTTPClient.h"
//#include <ArduinoJson.h>

// Define DHT
#define DHTpin 26
#define DHTTYPE DHT11
DHT dht(DHTpin, DHTTYPE);

// define BMP
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;

// define Soil Moisture Pin
#define soilMoisturePin 35

// Define Water Level Pins
#define powpin 2
#define sigpin 34

// Wifi
char ssid[] = "1+";
char password[] = "12345677";
String cse_ip = "192.168.211.224"; // YOUR IP from ipconfig/ifconfig 192.168.211.224
String cse_port = "8080";
String server = "http://192.168.211.224:8080//in-cse/in-name/"; //+ cse_ip + ":" + cse_port + "//in-cse/in-name/";
WiFiClient client;
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
float bhum;
void createCI(String val, String ae, String cnt)
{
  HTTPClient http;
  http.begin(server + ae + "/" + cnt + "/");

  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": \"" + String(val) + "\"}}");

  //Serial.println(code);
  if (code == -1) {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  bmp280.begin(BMP280_I2C_ADDRESS);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting...");
  }
  Serial.println("");
  Serial.println("Successfully connected to WiFi.");
  Serial.println("IP address is : ");
  Serial.println(WiFi.localIP());
}

void loop() {

  delay(2000);
  float temp = bmp280.readTemperature();
  float pressure = bmp280.readPressure();
  float humidity = dht.readHumidity();

  if (!isnan(humidity))
  {
    bhum = humidity;
    Serial.print("Humidity = ");
    Serial.println(humidity);
    Serial.print("%");

  }
  else {
    Serial.print("Humidity = ");
    Serial.println(bhum);
    Serial.print("%");

  }
  float soilMoistureValue = analogRead(soilMoisturePin);
  soilMoistureValue = soilMoistureValue / 4095;
  float Moisture_percentage = soilMoistureValue * 100;
  float percent = 100 - Moisture_percentage;


  digitalWrite(powpin, HIGH);
  int WaterLevelValue = analogRead(sigpin);
  digitalWrite(powpin, LOW);

  Serial.print("Temperature = ");
  Serial.println(temp);
  Serial.print("Pressure = ");
  Serial.println(pressure / 100);

  Serial.print("Soil Moisture Percentage: ");
  Serial.println(percent);
  Serial.print("Water Sensor value: ");
  Serial.println(WaterLevelValue);

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

  delay(1000);

}
