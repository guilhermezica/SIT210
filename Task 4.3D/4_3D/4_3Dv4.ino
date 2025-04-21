#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USING_TIMER_TC3         true
#define USING_TIMER_TC4         false
#define USING_TIMER_TC5         false
#define USING_TIMER_TCC         false
#define USING_TIMER_TCC1        false
#define USING_TIMER_TCC2        false

#include "SAMDTimerInterrupt.h"
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

// Pins
#define BUTTON_PIN 2
#define LED_BUTTON 3
#define LED_TIMER  4
#define LED_LIGHT  5
#define DHT_PIN    6
#define DHT_TYPE   DHT11

// Global instances
DHT dht(DHT_PIN, DHT_TYPE);
BH1750 lightMeter;

// Timer config
#define TIMER_INTERVAL_MS 1000  // 1s safe interval
#define SELECTED_TIMER TIMER_TC3
SAMDTimer ITimer(SELECTED_TIMER);

// Flags and counters
volatile bool buttonFlag = false;
volatile bool timerTickFlag = false;
volatile int tickCount = 0;

// States
bool buttonLEDState = false;
bool timerLEDState = false;
bool lightLEDState = false;

// ISR: Button press
void handleButtonInterrupt() {
  buttonFlag = true;
}

// ISR: Timer every 1s
void TimerHandler() {
  timerTickFlag = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  // Setup pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUTTON, OUTPUT);
  pinMode(LED_TIMER, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);

  // Interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonInterrupt, RISING);

  // Sensors
  dht.begin();
  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 initialized.");
  } else {
    Serial.println("Failed to init BH1750.");
  }

  // Timer setup
  if (ITimer.attachInterruptInterval_MS(TIMER_INTERVAL_MS, TimerHandler)) {
    Serial.println("Timer interrupt started (1s tick).");
  } else {
    Serial.println("Failed to start timer.");
  }

  Serial.println("System ready.");
}

void loop() {
  // Handle button interrupt
  if (buttonFlag) {
    buttonFlag = false;
    buttonLEDState = !buttonLEDState;
    digitalWrite(LED_BUTTON, buttonLEDState);
    Serial.println("Button interrupt: LED_BUTTON toggled");
  }

  // Handle timer interrupt logic
  if (timerTickFlag) {
    timerTickFlag = false;
    tickCount++;

    // Every 5s toggle LED
    if (tickCount % 5 == 0) {
      timerLEDState = !timerLEDState;
      digitalWrite(LED_TIMER, timerLEDState);
      Serial.println("Timer: LED_TIMER toggled");
    }

    // Every 30s read DHT
    if (tickCount % 30 == 0) {
      float temp = dht.readTemperature();
      float hum = dht.readHumidity();
      if (!isnan(temp) && !isnan(hum)) {
        Serial.print("DHT11 Log: Temp = ");
        Serial.print(temp);
        Serial.print(" °C | Humidity = ");
        Serial.print(hum);
        Serial.println(" %");
      } else {
        Serial.println("DHT11 read failed.");
      }
    }

    // Every 1s check light sensor
    float lux = lightMeter.readLightLevel();
    if (lux >= 0) {
      bool newLightState = lux < 20;
      if (newLightState != lightLEDState) {
        lightLEDState = newLightState;
        digitalWrite(LED_LIGHT, lightLEDState);
        Serial.print("BH1750 Light Sensor: ");
        Serial.print(lux);
        Serial.println(lightLEDState ? " lx → LED_LIGHT ON" : " lx → LED_LIGHT OFF");
      }
    }
  }
}
