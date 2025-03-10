#include <Arduino.h>
#include <map>
#include <cctype>

const int ledPin = LED_BUILTIN;  // LED pin
const int dotDuration = 250;     // Duration of a dot
const int dashDuration = 3 * dotDuration;  // Duration of a dash
const int letterDuration = 3 * dotDuration; // Duration between letters

// Morse Code Dictionary
std::map<char, String> morseCode = {
    {'A', ".-"},   {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},
    {'E', "."},    {'F', "..-."}, {'G', "--."},  {'H', "...."},
    {'I', ".."},   {'J', ".---"}, {'K', "-.-"},  {'L', ".-.."},
    {'M', "--"},   {'N', "-."},   {'O', "---"},  {'P', ".--."},
    {'Q', "--.-"}, {'R', ".-."},  {'S', "..."},  {'T', "-"},
    {'U', "..-"},  {'V', "...-"}, {'W', ".--"},  {'X', "-..-"},
    {'Y', "-.--"}, {'Z', "--.."},
    {'0', "-----"},{'1', ".----"},{'2', "..---"},{'3', "...--"},
    {'4', "....-"},{'5', "....."},{'6', "-...."},{'7', "--..."},
    {'8', "---.."},{'9', "----."},
    {' ', "/"}  // Space is represented as "/"
};

// Function to convert a character to Morse Code
String charToMorse(char c) {
    c = toupper(c);  // Convert to uppercase
    if (morseCode.find(c) != morseCode.end()) {
        return morseCode[c];
    }
    return "?"; // Return "?" if character is not found
}

// Function to blink Morse code on the LED
void blinkMorse(const String &morse) {
    for (char symbol : morse) {
        if (symbol == '.') {
            digitalWrite(ledPin, HIGH);
            delay(dotDuration);
        } else if (symbol == '-') {
            digitalWrite(ledPin, HIGH);
            delay(dashDuration);
        }
        digitalWrite(ledPin, LOW);
        delay(dotDuration);  // Short gap between symbols
    }
    delay(letterDuration); // Gap between letters
}

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    delay(1500); // Allow Serial Monitor to initialize

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
}

void loop() {
    Serial.println("Please enter your name: ");
    
    while (Serial.available() == 0) {
        // Wait for user input
    }

    String userInput = Serial.readString();
    userInput.trim(); // Remove any unwanted spaces or newlines

    Serial.print("You entered: ");
    Serial.println(userInput);

    Serial.print("Morse Code: ");
    
    // Convert and display Morse code
    for (char c : userInput) {
        String morse = charToMorse(c);
        Serial.print(morse + " ");
        blinkMorse(morse);  // Blink Morse code on LED
    }

    Serial.println("\nMorse Code sequence completed.");
    delay(5000); // Wait before repeating the sequence
}


