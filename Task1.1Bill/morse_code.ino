const int ledPin = LED_BUILTIN; // LED pin
const int dotDuration = 250;    // Duration of a dot
const int dashDuration = 3 * dotDuration; // Duration of a dash
const int letterDuration = 3 * dotDuration; // Duration between letters

void setup() {
  // Initialize serial communication and wait for port to open
  Serial.begin(9600);
  delay(1500); // Delay to allow time for Serial Monitor

  // Set the LED pin as output
  pinMode(ledPin, OUTPUT);
}

void loop() {

  // Write letter B (Morse Code: -...)
  digitalWrite(ledPin, HIGH);
  delay(dashDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  delay(letterDuration); // Delay between letters

  // Write letter I (Morse Code: ..)
  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  delay(letterDuration); // Delay between letters

  // Write letter L (Morse Code: .-..)
  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dashDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  delay(letterDuration); // Delay between letters

  // Write letter L (Morse Code: .-..) again
  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dashDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  digitalWrite(ledPin, HIGH);
  delay(dotDuration);
  digitalWrite(ledPin, LOW);

  delay(letterDuration); // Delay between letters
}