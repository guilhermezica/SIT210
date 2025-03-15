#include "arduino_secrets.h"
#include "secrets.h"
#include <ThingSpeak.h>
#include "DHT.h"
#include <WiFiNINA.h>

// Sensor details
#define DHT_PIN 4 
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Wi-Fi details
WiFiClient client;
int status = WL_IDLE_STATUS;

// Channel details
unsigned long myChannelNumber = 2876270;
const char* apiKey = "0VK63394953RZ7FY";

// Initialize values
float h = 0;
float t = 0;

// Project setup
void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Correct the ThingSpeak client initialization
  ThingSpeak.begin(client);
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  
  // If the connection worked, print the status
  printWifiStatus();
  
  Serial.println("\nWiFi connected.");
  dht.begin();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void loop() {
  // Ensure Wi-Fi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected, reconnecting...");
    WiFi.begin(ssid, pass);  // Fixed variable name from 'password' to 'pass'
    delay(5000);
    return;
  }
  
  // Read humidity and temperature from DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Check readings
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT11. Retrying...");
    delay(5000);
    return;
  }
  
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print("°C  Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  
  // Fix spelling mistakes in ThingSpeak functions
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);  // Changed field number from 1 to 2
  
  // Write to ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, apiKey);
  
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  delay(20000);  // Wait 20 seconds before next update
}