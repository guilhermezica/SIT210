const int ledPin = LED_BUILTIN; // LED pin
const int dotDuration = 250;    // Duration of a dot
const int dashDuration = 3 * dotDuration; // Duration of a dash
const int letterDuration = 3 * dotDuration; // Duration between letters

void setup() {
  Serial.begin(9600);
  delay(1500); // Delay to allow Serial Monitor to initialize
  pinMode(ledPin, OUTPUT);

  Serial.println("Morse code sequence is about to start...");

  // Alert signal: 10 quick flashes, followed by a long blink
  for (int i = 0; i < 10; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }

  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
}

void loop() {
  Serial.println("Writing 'BILL' in Morse Code...");

  // Write letter B (Morse Code: -...)
  flashMorse(dashDuration);
  flashMorse(dotDuration);
  flashMorse(dotDuration);
  flashMorse(dotDuration);
  delay(letterDuration);

  // Write letter I (Morse Code: ..)
  flashMorse(dotDuration);
  flashMorse(dotDuration);
  delay(letterDuration);

  // Write letter L (Morse Code: .-..)
  flashMorse(dotDuration);
  flashMorse(dashDuration);
  flashMorse(dotDuration);
  flashMorse(dotDuration);
  delay(letterDuration);

  // Write letter L (Morse Code: .-..)
  flashMorse(dotDuration);
  flashMorse(dashDuration);
  flashMorse(dotDuration);
  flashMorse(dotDuration);
  delay(letterDuration);

  Serial.println("Morse Code sequence completed.");
  delay(5000); // Wait before repeating the sequence
}

// Function to flash LED for given duration
void flashMorse(int duration) {
  digitalWrite(ledPin, HIGH);
  delay(duration);
  digitalWrite(ledPin, LOW);
  delay(dotDuration); // Short pause between signals
}
