ls#define TRIGGER_PIN  9
#define ECHO_PIN     10
#define MAX_DISTANCE 50
#define PROXIMITY_THRESHOLD 5

const byte White_LED = 3;   // Controlled by sensor
const byte ledPin = 4;      // Controlled by button
const byte interruptPin = 2;

volatile bool ledState = false;     // LED on pin 4 (button-controlled)
bool objectDetected = false;        // Tracks if sensor sees something

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(White_LED, OUTPUT);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), buttonInterrupt, FALLING);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor on Nano 33 IoT
}

void loop() {
  // Always update the button-controlled LED
  digitalWrite(ledPin, ledState);

  // Sensor detection
  long duration, distance;

  // Trigger ultrasonic pulse
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Sensor controls White_LED
  if (distance > 0 && distance <= PROXIMITY_THRESHOLD) {
    digitalWrite(White_LED, HIGH);
    if (!objectDetected) {
      Serial.println("Object detected!");
      objectDetected = true;
    }
  } else {
    digitalWrite(White_LED, LOW);
    objectDetected = false;
  }

  delay(100);
}

void buttonInterrupt() {
  ledState = !ledState;  // Toggle LED state on pin 4
}
