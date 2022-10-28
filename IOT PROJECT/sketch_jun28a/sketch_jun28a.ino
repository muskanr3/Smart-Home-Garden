#include <Adafruit_BMP280.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <WiFi.h>


#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;

#define DHTTYPE DHT11
#define dht_dpin 26
DHT dht(dht_dpin, DHTTYPE);


#define powpin 21
#define sigpin 14

#define soilMoisturePin 13
#define soilMoistureThreshold 5000
#define ledPin 25
#define buzzPin 18

#define waterLevelThreshold 1000

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "1+";
char pass[] = "12345677";

WiFiClient client;

long writeChannelID = 1781974;
const char* writeAPIKey = "2AMROSJGH0B3XT4I";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  pinMode(powpin, OUTPUT);
  digitalWrite(powpin, LOW);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, HIGH);


  ThingSpeak.begin(client);

}



void loop() {
  // put your main code here, to run repeatedly:
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  Serial.println("WiFi Connected!");
  digitalWrite(buzzPin, HIGH);
  digitalWrite(ledPin, LOW);

  Serial.print("pressure= ");
  float pressure = bmp280.readPressure();
  Serial.println(pressure / 100);

  float hum = dht.readHumidity();
  float temp = bmp280.readTemperature();
  Serial.print("temperature: ");
  Serial.print(temp);

  if (isnan(hum) || isnan(temp))
  {
    Serial.println("Sensor Not Working");
  }

  else
  {
    Serial.print("humidity: ");
    Serial.print(hum);
    Serial.print("%");
    Serial.println();
    Serial.print("temperature: ");
    Serial.print(temp);
    Serial.print("C ");
    Serial.println();
  }

  digitalWrite(powpin, HIGH);
  int WaterLevelValue = analogRead(sigpin);
  digitalWrite(powpin, LOW);

  Serial.print("Water Sensor value: ");
  Serial.println(WaterLevelValue);

  if (WaterLevelValue < waterLevelThreshold)
  {
    digitalWrite(buzzPin, LOW);
  }

  float soilMoistureValue = analogRead(soilMoisturePin);
  Serial.print("Soil Moisture Percentage: ");
  soilMoistureValue = soilMoistureValue / 4095;
  float Moisture_percentage = soilMoistureValue * 100;
  float percent = 100 - Moisture_percentage;
  Serial.println(percent);
  ThingSpeak.writeField(writeChannelID, 4, percent, writeAPIKey);
  Serial.println(percent);



  if (soilMoistureValue < soilMoistureThreshold)
  {
    digitalWrite(ledPin, HIGH);
  }

  Serial.println();
  delay(3000);
}
