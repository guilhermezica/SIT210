#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Servo.h>
#include <ArduinoLowPower.h>
#include "secrets.h"

#define DHTPIN 2
#define DHTTYPE DHT11
#define LED_PIN LED_BUILTIN
#define SERVO_PIN 3

DHT dht(DHTPIN, DHTTYPE);
Servo motor;

const char* broker = "9c8938d3b2404aa98dafd50aa5ddd1b1.s1.eu.hivemq.cloud";
const int port = 8883;
const char* topic_temp = "kitchen/temperature";
const char* topic_control = "kitchen/sensor/control";
const char* topic_status = "kitchen/sensor/status";
const char* topic_feedback = "kitchen/feedback/response";

WiFiSSLClient net;
PubSubClient client(net);

bool sensorEnabled = true;
bool sleepMode = false;
bool wifiConnected = false;
bool mqttConnected = false;

unsigned long lastSend = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastConnectionCheck = 0;

const unsigned long HEARTBEAT_INTERVAL = 15000;
const unsigned long TEMP_INTERVAL = 3000;
const unsigned long CONNECTION_CHECK_INTERVAL = 30000;

void swingMotor(int swings) {
  for (int i = 0; i < swings; i++) {
    motor.write(180); delay(300);
    motor.write(0);   delay(300);
  }
  motor.write(90);  // Stop motor
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();

  Serial.print("MQTT Message [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  String t = String(topic);

  if (t == topic_control) {
    if (msg.equalsIgnoreCase("off") || msg.equalsIgnoreCase("sleep")) {
      sensorEnabled = false;
      sleepMode = true;
      Serial.println("Entering low-power mode...");
      if (client.connected()) {
        client.publish(topic_status, "sleeping");
        delay(100);
      }
      digitalWrite(LED_PIN, LOW);
    } else if (msg.equalsIgnoreCase("on") || msg.equalsIgnoreCase("wake")) {
      sensorEnabled = true;
      sleepMode = false;
      Serial.println("Exiting low-power mode...");
      if (client.connected()) {
        client.publish(topic_status, "active");
      }
      digitalWrite(LED_PIN, HIGH);
    }
  } else if (t == topic_feedback) {
    msg.toLowerCase();
    if (msg == "poor") swingMotor(1);
    else if (msg == "average") swingMotor(2);
    else if (msg == "good") swingMotor(3);
    else if (msg == "excellent") swingMotor(4);
    else Serial.println("Unknown feedback command.");
  }
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    return;
  }

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    Serial.print(".");
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\nWiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    Serial.println("\nWiFi connection failed");
  }
}

void connectMQTT() {
  if (!wifiConnected) {
    mqttConnected = false;
    return;
  }

  client.setServer(broker, port);
  client.setCallback(messageReceived);

  if (client.connected()) {
    mqttConnected = true;
    return;
  }

  Serial.print("Connecting to MQTT... ");
  String clientId = "Nano33IoTClient_" + String(random(0xffff), HEX);

  if (client.connect(clientId.c_str(), "BillZZ", "BillZZ92")) {
    Serial.println("Connected");
    mqttConnected = true;
    client.subscribe(topic_control);
    client.subscribe(topic_feedback);
    const char* status = sleepMode ? "sleeping" : (sensorEnabled ? "active" : "inactive");
    client.publish(topic_status, status);
  } else {
    Serial.print("Failed (rc=");
    Serial.print(client.state());
    Serial.println(")");
    mqttConnected = false;
  }
}

void checkConnections() {
  unsigned long now = millis();
  if (now - lastConnectionCheck < CONNECTION_CHECK_INTERVAL) return;
  lastConnectionCheck = now;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected - attempting reconnect");
    wifiConnected = false;
    connectWiFi();
  }

  if (wifiConnected && !client.connected()) {
    Serial.println("MQTT disconnected - attempting reconnect");
    mqttConnected = false;
    connectMQTT();
  }
}

void sendHeartbeat() {
  if (!mqttConnected) return;

  unsigned long now = millis();
  if (now - lastHeartbeat > HEARTBEAT_INTERVAL) {
    const char* status = sleepMode ? "sleeping" : (sensorEnabled ? "active" : "inactive");
    if (client.publish(topic_status, status)) {
      Serial.print("Heartbeat sent: ");
      Serial.println(status);
    }
    lastHeartbeat = now;
  }
}

void readAndSendTemperature() {
  if (!sensorEnabled || !mqttConnected) return;

  unsigned long now = millis();
  if (now - lastSend >= TEMP_INTERVAL) {
    float temp = dht.readTemperature();
    if (!isnan(temp)) {
      String payload = String(temp, 1);
      if (client.publish(topic_temp, payload.c_str())) {
        Serial.print("Temperature sent: ");
        Serial.println(payload);
      } else {
        Serial.println("Failed to publish temperature");
      }
    } else {
      Serial.println("Failed to read DHT sensor");
    }
    lastSend = now;
  }
}

void enterLowPowerMode() {
  Serial.println("Entering low-power mode (reduced activity)");

  static unsigned long lastBlink = 0;
  static bool ledState = false;

  if (millis() - lastBlink > 2000) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    lastBlink = millis();
  }

  if (mqttConnected) {
    client.loop();
  }

  delay(500);
}

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 5000);

  Serial.println("=== Starting Nano 33 IoT Cooking Monitor ===");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  motor.attach(SERVO_PIN);
  motor.write(90);  // Stop FS90R

  dht.begin();
  connectWiFi();
  connectMQTT();

  Serial.println("Setup complete. Entering main loop...");
}

void loop() {
  if (sleepMode) {
    enterLowPowerMode();
    return;
  }

  digitalWrite(LED_PIN, HIGH);

  checkConnections();

  if (mqttConnected) {
    client.loop();
  }

  sendHeartbeat();
  readAndSendTemperature();

  delay(100);
}
