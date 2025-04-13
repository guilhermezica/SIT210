#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <NewPing.h>
#include "secrets.h"

const char* your_name = "Bill Zica";

// Ultrasonic sensor pins
#define TRIGGER_PIN  10
#define ECHO_PIN     9
#define MAX_DISTANCE 100  // Maximum detection distance in cm

// LED pin
const int Redled = 2;
const int Greenled = 3;

// Distance thresholds for wave detection
const int waveDetectionDistance = 30;  // Detect hand waves within this distance
const int waveTimeout = 1000;          // Time in ms to prevent multiple detections

// MQTT and WiFi clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Ultrasonic sensor
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Variables to track wave detection
unsigned long lastWaveTime = 0;
bool objectPresent = false;
int waveCount = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  // Configure LED pin as output
  pinMode(Redled, OUTPUT);
  
  // Connect to WiFi
  setupWifi();
  
  // Configure MQTT connection
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

void loop() {
  // Reconnect to MQTT if the connection is lost
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  // Process MQTT messages
  mqttClient.loop();
  
  // Detect waves using the ultrasonic sensor
  detectWave();
  
  // Short delay
  delay(100);
}

void setupWifi() {
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(Redled, HIGH);
    delay(250);
    digitalWrite(Redled, LOW);
    delay(250);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Quick LED flash to indicate WiFi connected
  for (int i = 0; i < 5; i++) {
    digitalWrite(Redled, HIGH);
    delay(100);
    digitalWrite(Redled, LOW);
    delay(100);
  }
}

void reconnectMQTT() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a client ID
    String clientId = "ArduinoNano33IoT-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      
      // Subscribe to the topic
      mqttClient.subscribe(mqtt_topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(mqtt_topic);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void detectWave() {
  // Get distance from ultrasonic sensor
  unsigned int distance = sonar.ping_cm();
  
  // Print distance for debugging
  if (distance > 0) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  
  // Check if a wave is detected
  unsigned long currentTime = millis();
  
  if (distance > 0 && distance < waveDetectionDistance) {
    // Object is present
    if (!objectPresent) {
      objectPresent = true;
      
      // Check if enough time has passed since the last wave
      if (currentTime - lastWaveTime > waveTimeout) {
        Serial.println("Wave detected!");
        lastWaveTime = currentTime;
        waveCount++;
        
        // Publish the message with your name
        String message = your_name;
        mqttClient.publish(mqtt_topic, message.c_str());
        Serial.print("Published to ");
        Serial.print(mqtt_topic);
        Serial.print(": ");
        Serial.println(message);
      }
    }
  } else {
    // No object is present
    objectPresent = false;
  }
}

// MQTT message callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);
  
  // Check if message is a "pat" command
  if (message.indexOf("pat") >= 0) {
    Serial.println("Pat detected! Flashing LED in pat pattern");
    patLedPattern();
  } else {
    // Default to wave pattern
    Serial.println("Wave detected! Flashing LED in wave pattern");
    waveLedPattern();
  }
}

// LED flashing pattern for wave
void waveLedPattern() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(Redled, HIGH);
    delay(500);
    digitalWrite(Redled, LOW);
    delay(500);
  }
}

// LED flashing pattern for pat (different pattern)
void patLedPattern() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(Greenled, HIGH);
    delay(200);
    digitalWrite(Greenled, LOW);
    delay(200);
  }
}