#include <Adafruit_BMP280.h>
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


#define powpin 19
#define sigpin 14

#define soilMoisturePin 35
#define soilMoistureThreshold 5000
#define ledPin 25
#define buzzPin 18

#define waterLevelThreshold 1000

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

//For wifi connection

const char* ssid = "1+";
const char* password = "12345677";
WiFiClient client;
unsigned long myChannelNumber = 1781974;
const char * myWriteAPIKey = "2AMROSJGH0B3XT4I";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Initialize serial
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); // Initialize ThingSpeak

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

  if ((millis() - lastTime) > timerDelay) { // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED)
    { Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      Serial.println("\nConnected.");

    }

    digitalWrite(buzzPin, HIGH);
    digitalWrite(ledPin, LOW);

    Serial.print("pressure= ");
    float pressure = bmp280.readPressure();
    Serial.println(pressure / 100);
    ThingSpeak.writeField(myChannelNumber, 5, pressure, myWriteAPIKey);


    float hum = dht.readHumidity();
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
      ThingSpeak.writeField(myChannelNumber, 1, hum, myWriteAPIKey);
      ThingSpeak.writeField(myChannelNumber, 2, temp, myWriteAPIKey);
    }

    digitalWrite(powpin, HIGH);
    int WaterLevelValue = analogRead(sigpin);
    digitalWrite(powpin, LOW);

    Serial.print("Water Sensor value: ");
    Serial.println(WaterLevelValue);
    ThingSpeak.writeField(myChannelNumber, 3, WaterLevelValue, myWriteAPIKey);


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

    int x = ThingSpeak.writeField(myChannelNumber, 4, percent, myWriteAPIKey); //uncomment if you want to get temperature in Fahrenheit //int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    } lastTime = millis();

    if (soilMoistureValue < soilMoistureThreshold)
    {
      digitalWrite(ledPin, HIGH);
    }

    Serial.println();
    delay(3000);
  }
}
