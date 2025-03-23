#include <SPI.h>
#include <BH1750FVI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <WiFiSSLClient.h>
#include "secrets.h"

// MQTT Broker Credentials
#define MQTT_SERVER "bzdeakinsit210-68wmdo.a03.euc1.aws.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "hivemq.webclient.1742692564802"
#define MQTT_PASSWORD "$i7!:h0zrWyFwV3.P1JB"
#define MQTT_TOPIC "sensor/lighting"

// Creating luminosity variables
bool lum = false;
int dark_count = 0;

// Creating WiFi and MQTT client instances
WiFiSSLClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Creating the Lightsensor instance
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Luminosity Test with WiFi and MQTT");
  
  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // These should be defined in secrets.h
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // Set MQTT Server
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
  // Initialize light sensor
  LightSensor.begin();
  
  Serial.println("Setup complete");
}

void loop() {
  // Ensure MQTT connection
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
  
  // Get light intensity reading
  uint16_t lux = LightSensor.GetLightIntensity();
  Serial.print("Light: ");
  Serial.println(lux);
  
  // Check if it's dark and increment counter if needed
  if (lux <= 20) {
    dark_count += 1;
    Serial.print("Dark count: ");
    Serial.println(dark_count);
  }
  else {
    dark_count = 0;
    lum = false;
  }
  
  // If it's been dark for 10 cycles (for testing, would be 7200 for 2 hours)
  if (dark_count > 10 && !lum) {
    lum = true;
    
    // Publish alert to MQTT
    Serial.println("Publishing darkness alert");
    if (mqttClient.publish(MQTT_TOPIC, "Darkness detected")) {
      Serial.println("Alert sent successfully");
    } else {
      Serial.println("Failed to send alert");
    }
  }
  
  delay(1000);
}

// Connect to MQTT Broker
void connectMQTT() {
  Serial.println("Connecting to MQTT...");
  
  // Create a simple client ID
  String clientId = "ArduinoNanoIoT-";
  clientId += String(random(0xffff), HEX);
  
  if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("Connected to MQTT Broker!");
  } else {
    Serial.print("Failed to connect, error: ");
    Serial.println(mqttClient.state());
  }
}