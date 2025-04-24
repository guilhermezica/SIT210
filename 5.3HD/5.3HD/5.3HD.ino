#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

// Firebase details
const char* firebaseHost = "hd-api-3146c-default-rtdb.asia-southeast1.firebasedatabase.app";
const int httpsPort = 443;
const String firebasePath = "/leds.json";

// WiFi and HTTP clients
WiFiSSLClient wifiClient;
HttpClient client = HttpClient(wifiClient, firebaseHost, httpsPort);

// LED pins
const int redLED = 3;
const int greenLED = 4;
const int blueLED = 5;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting up...");

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    wifiAttempts++;
    if (wifiAttempts > 15) {
      Serial.println("\nFailed to connect to WiFi. Rebooting...");
      NVIC_SystemReset(); // Reset board
    }
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.println("Making GET request to Firebase...");

  client.get(firebasePath);
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("HTTP status code: ");
  Serial.println(statusCode);

  if (statusCode != 200) {
    Serial.println("Error retrieving data from Firebase.");
    delay(5000);
    return;
  }

  Serial.print("Firebase response: ");
  Serial.println(response);

  // Parse JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.f_str());
    delay(5000);
    return;
  }

  bool redState = doc["red"];
  bool greenState = doc["green"];
  bool blueState = doc["blue"];

  digitalWrite(redLED, redState ? HIGH : LOW);
  digitalWrite(greenLED, greenState ? HIGH : LOW);
  digitalWrite(blueLED, blueState ? HIGH : LOW);

  Serial.print("Red LED: "); Serial.println(redState ? "ON" : "OFF");
  Serial.print("Green LED: "); Serial.println(greenState ? "ON" : "OFF");
  Serial.print("Blue LED: "); Serial.println(blueState ? "ON" : "OFF");

  Serial.println("-----");

  delay(3000);  // Delay between Firebase polls
}
