#include <ArduinoBLE.h>

const int trigPin = 9;
const int echoPin = 10;

BLEService parkingService("ABCD");
BLEUnsignedCharCharacteristic distanceChar("2A56", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if (!BLE.begin()) {
    Serial.println("Failed to start BLE!");
    while (1);
  }

  BLE.setLocalName("ArduinoSensor");
  BLE.setDeviceName("ArduinoSensor");

  parkingService.addCharacteristic(distanceChar);
  BLE.addService(parkingService);
  distanceChar.writeValue(0);
  BLE.advertise();

  Serial.println("BLE advertising as 'ArduinoSensor'");
}

void loop() {
  BLE.poll();  // 

  // Measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  distance = constrain(distance, 0, 255);

  distanceChar.writeValue((uint8_t)distance);
  Serial.print("Distance sent: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}
