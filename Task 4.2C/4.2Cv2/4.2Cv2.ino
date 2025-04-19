// Arduino sketch with dual interrupts for LED control
// One button interrupt and one timer interrupt

// Pin definitions
const int buttonPin = 2;    // Button connected to pin 2 (interrupt pin)
const int buttonLedPin = 3; // LED controlled by button interrupt
const int timerLedPin = 4;  // LED controlled by timer interrupt

// Variables for debouncing
volatile bool buttonLedState = LOW;
volatile bool timerLedState = LOW;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // Debounce time in ms

void setup() {
  // Initialize pin modes
  pinMode(buttonPin, INPUT_PULLUP);  // Use internal pull-up resistor
  pinMode(buttonLedPin, OUTPUT);
  pinMode(timerLedPin, OUTPUT);
  
  // Set initial LED states
  digitalWrite(buttonLedPin, buttonLedState);
  digitalWrite(timerLedPin, timerLedState);
  
  // Attach button interrupt (triggers on FALLING edge - when button is pressed)
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, FALLING);
  
  // Setup timer interrupt (for SAMD boards like Arduino Zero, MKR series, Nano 33 IoT)
  setupTimerInterrupt();
  
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Dual interrupt system initialized");
}

void loop() {
  // Main loop is empty as everything is handled by interrupts
}

// Button interrupt service routine
void buttonInterrupt() {
  // Debounce
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Toggle button LED state
    buttonLedState = !buttonLedState;
    digitalWrite(buttonLedPin, buttonLedState);
    
    // Print status
    Serial.print("Button pressed! LED on pin ");
    Serial.print(buttonLedPin);
    Serial.println(buttonLedState ? " turned ON" : " turned OFF");
    
    lastDebounceTime = millis();
  }
}

// Timer interrupt service routine
void timerISR() {
  // Toggle timer LED state
  timerLedState = !timerLedState;
  digitalWrite(timerLedPin, timerLedState);
  
  // Print status (only if needed, can be commented out)
  Serial.print("Timer triggered! LED on pin ");
  Serial.print(timerLedPin);
  Serial.println(timerLedState ? " turned ON" : " turned OFF");
}

// Setup timer interrupt for SAMD-based boards (Arduino Zero, MKR, Nano 33 IoT, etc.)
void setupTimerInterrupt() {
  // Configure generic clock generator 4
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |          // Divide by 3
                    GCLK_GENDIV_ID(4);            // Select Generator 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Configure clock generator 4 using internal 8MHz oscillator
  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set duty cycle to 50/50
                     GCLK_GENCTRL_GENEN |         // Enable generator
                     GCLK_GENCTRL_SRC_OSC8M |     // Set source to 8MHz
                     GCLK_GENCTRL_ID(4);          // Select generator 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Connect generic clock to TC4 and TC5
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable clock
                     GCLK_CLKCTRL_GEN_GCLK4 |     // Select Generator 4
                     GCLK_CLKCTRL_ID_TC4_TC5;     // Feed the clock to TC4 and TC5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
  
  // Configure TC4 for a ~5s interrupt
  REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV1024 |   // Set prescaler to 1024
                   TC_CTRLA_WAVEGEN_MFRQ |        // Match frequency mode
                   TC_CTRLA_MODE_COUNT16;         // 16-bit counter mode
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);       // Wait for synchronization
  
  // Set the period (5 seconds)
  // 8MHz / 3 / 1024 ≈ 2604 Hz
  // For 5 second period, need count of 2604 * 5 = 13020
  REG_TC4_COUNT16_CC0 = 13020;
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);       // Wait for synchronization
  
  // Enable interrupt
  NVIC_DisableIRQ(TC4_IRQn);
  NVIC_ClearPendingIRQ(TC4_IRQn);
  NVIC_SetPriority(TC4_IRQn, 0);
  NVIC_EnableIRQ(TC4_IRQn);
  
  // Enable TC4 interrupt
  REG_TC4_INTENSET = TC_INTENSET_MC0;
  
  // Enable TC4
  REG_TC4_CTRLA |= TC_CTRLA_ENABLE;
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);       // Wait for synchronization
}

// Timer interrupt handler
void TC4_Handler() {
  // Check for match counter 0 interrupt
  if (TC4->COUNT16.INTFLAG.bit.MC0) {
    timerISR();
    // Clear the interrupt flag
    TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
  }
}