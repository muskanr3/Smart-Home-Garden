#include <WiFi.h>
#include "ThingSpeak.h"

#define soilMoisturePin 13

const char* ssid = "1+";
const char* password = "12345677";
WiFiClient client;
unsigned long myChannelNumber = 1781974;
const char * myWriteAPIKey = "2AMROSJGH0B3XT4I";
//    Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
// Variable to hold temperature readings
// float temperatureC;
// Create a sensor object
Adafruit_BMP280 bmp; //BME280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)
void initBMP() {
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}
void setup()
{ Serial.begin(115200);
  //Initialize serial
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); // Initialize ThingSpeak
}
void loop() {
  if ((millis() - lastTime) > timerDelay) { // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED)
    { Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      Serial.println("\nConnected.");

    } // Get a new temperature reading

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
  }
 

}