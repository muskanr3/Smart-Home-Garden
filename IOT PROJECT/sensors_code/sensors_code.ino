#include <Adafruit_BMP280.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

//#define BLYNK_TEMPLATE_ID "TMPLkKxhf9B4"
//#define BLYNK_DEVICE_NAME "esp32"
//#define BLYNK_AUTH_TOKEN "pZb2xZwRScrE_BGrYCgqRloeN-1EJTRD"

#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;

#define DHTTYPE DHT11
#define dht_dpin 26
DHT dht(dht_dpin, DHTTYPE);


#define powpin 2
#define sigpin 34

#define soilMoisturePin 35
#define soilMoistureThreshold 5000
#define ledPin 25
#define buzzPin 18

#define waterLevelThreshold 1000

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "1+";
//char pass[] = "12345677";


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //  Blynk.begin(auth, ssid, pass);

  bmp280.begin(BMP280_I2C_ADDRESS);
  dht.begin();
  pinMode(powpin, OUTPUT);
  digitalWrite(powpin, LOW);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(buzzPin, HIGH);
  digitalWrite(ledPin, LOW);

  Serial.print("pressure= ");
  float pressure = bmp280.readPressure();
  Serial.println(pressure / 100);

  float hum = dht.readHumidity();
  float temp = bmp280.readTemperature();
  Serial.print("temperature: ");
  Serial.print(temp);

  if (isnan(hum))
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

  if (soilMoistureValue < soilMoistureThreshold)
  {
    digitalWrite(ledPin, HIGH);
  }

  Serial.println();
  delay(3000);
}
