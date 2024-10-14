#include <Arduino.h>

// Pin definitions
const int buttonStopPin = 2;
const int buttonStartPin = 3;
const int ledGreenPin = 5;
const int ledBluePin = 6;
const int ledRedPin = 7;
const int ledL1Pin = 8;
const int ledL2Pin = 9;
const int ledL3Pin = 10;
const int ledL4Pin = 11;

// Variables
bool loading = false;
bool stopPressed = false;
unsigned long stopButtonPressTime = 0;
const unsigned long stopPressDuration = 1000;  // 1s press for stop
unsigned long previousMillis = 0;
unsigned long blinkInterval = 500;
int currentStage = 0;
unsigned long stageStartTime = 0;
bool ledsBlinking = false;

// Function declarations
void startLoading();
void handleLoadingProcess(unsigned long currentMillis);
void blinkAllLeds(unsigned long currentMillis);
void stopLoading();
void checkStopButton(unsigned long currentMillis);

// Setup
void setup() {
  pinMode(buttonStopPin, INPUT_PULLUP);
  pinMode(buttonStartPin, INPUT_PULLUP);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledL1Pin, OUTPUT);
  pinMode(ledL2Pin, OUTPUT);
  pinMode(ledL3Pin, OUTPUT);
  pinMode(ledL4Pin, OUTPUT);

  // Initial state: available (green led on)
  digitalWrite(ledGreenPin, HIGH);
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledL1Pin, LOW);
  digitalWrite(ledL2Pin, LOW);
  digitalWrite(ledL3Pin, LOW);
  digitalWrite(ledL4Pin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if start button is pressed (active low)
  if (digitalRead(buttonStartPin) == LOW && !loading) {
    startLoading();
  }

  // Monitor stop button at all times
  checkStopButton(currentMillis);

  // Handle the loading process if active
  if (loading) {
    handleLoadingProcess(currentMillis);
  }
}

void checkStopButton(unsigned long currentMillis) {
  // Check if stop button is pressed (active low) and for how long
  if (digitalRead(buttonStopPin) == LOW) {
    if (stopButtonPressTime == 0) {
      stopButtonPressTime = currentMillis;  // Start counting press duration
    }
    if (currentMillis - stopButtonPressTime >= stopPressDuration && loading) {
      stopPressed = true;  // Stop button held for 1 second
    }
  } else {
    stopButtonPressTime = 0;  // Reset timer if button is not pressed
  }

  // If stop button is pressed during loading, end the process
  if (stopPressed && loading) {
    stopLoading();
    stopPressed = false;
  }
}

void startLoading() {
  loading = true;
  
  // Set availability LED to red
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledRedPin, HIGH);
  
  currentStage = 1;
  stageStartTime = millis();  // Start the first stage
  ledsBlinking = false;       // Reset blinking state
}

void handleLoadingProcess(unsigned long currentMillis) {
  // Check stop button during loading process
  checkStopButton(currentMillis);

  // Handle the blinking process for currentStage
  static unsigned long blinkStartTime = 0;
  static bool ledBlinkState = LOW;
  const unsigned long blinkDuration = 600; // 600ms blink interval

  if (currentStage == 1 && currentMillis - stageStartTime >= 3000) {
    // Stage 1 complete (25%)
    digitalWrite(ledL1Pin, HIGH);  // L1 stays on
    currentStage = 2;
    stageStartTime = currentMillis;
    blinkStartTime = currentMillis;
    ledBlinkState = LOW; // Reset blink state for next stage
  } else if (currentStage == 2) {
    // Blink L2 for 1s, then turn it on
    if (currentMillis - blinkStartTime >= blinkDuration) {
      ledBlinkState = !ledBlinkState;  // Toggle blink state
      digitalWrite(ledL2Pin, ledBlinkState);
      blinkStartTime = currentMillis;
    }
    // After 3 seconds of blinking, move to the next stage
    if (currentMillis - stageStartTime >= 3000) {
      digitalWrite(ledL2Pin, HIGH);  // L2 stays on
      currentStage = 3;
      stageStartTime = currentMillis;
      blinkStartTime = currentMillis;
      ledBlinkState = LOW; // Reset blink state for next stage
    }
  } else if (currentStage == 3) {
    // Blink L3 for 1s, then turn it on
    if (currentMillis - blinkStartTime >= blinkDuration) {
      ledBlinkState = !ledBlinkState;  // Toggle blink state
      digitalWrite(ledL3Pin, ledBlinkState);
      blinkStartTime = currentMillis;
    }
    // After 3 seconds of blinking, move to the next stage
    if (currentMillis - stageStartTime >= 3000) {
      digitalWrite(ledL3Pin, HIGH);  // L3 stays on
      currentStage = 4;
      stageStartTime = currentMillis;
      blinkStartTime = currentMillis;
      ledBlinkState = LOW; // Reset blink state for next stage
    }
  } else if (currentStage == 4) {
    // Blink L4 for 1s, then turn it on
    if (currentMillis - blinkStartTime >= blinkDuration) {
      ledBlinkState = !ledBlinkState;  // Toggle blink state
      digitalWrite(ledL4Pin, ledBlinkState);
      blinkStartTime = currentMillis;
    }
    // After 3 seconds of blinking, move to the final animation
    if (currentMillis - stageStartTime >= 3000) {
      digitalWrite(ledL4Pin, HIGH);  // L4 stays on
      currentStage = 5;
      stageStartTime = currentMillis;
      ledsBlinking = true;
    }
  }

  // Final animation: Blink all LEDs 3 times
  if (currentStage == 5 && ledsBlinking) {
    blinkAllLeds(currentMillis);
  }
}

void blinkAllLeds(unsigned long currentMillis) {
  static bool ledState = false;
  static int blinkCount = 0;
  static unsigned long lastBlinkTime = 0;

  if (currentMillis - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = currentMillis;
    ledState = !ledState;
    digitalWrite(ledL1Pin, ledState);
    digitalWrite(ledL2Pin, ledState);
    digitalWrite(ledL3Pin, ledState);
    digitalWrite(ledL4Pin, ledState);

    if (ledState == HIGH) {
      blinkCount++;
    }

    if (blinkCount >= 4) {
      ledsBlinking = false;  // Stop blinking after 3 cycles
      delay(300);
      stopLoading();
    }
  }
}

void stopLoading() {
  // Reset all LEDs to their default state
  digitalWrite(ledL1Pin, LOW);
  digitalWrite(ledL2Pin, LOW);
  digitalWrite(ledL3Pin, LOW);
  digitalWrite(ledL4Pin, LOW);

  // Set availability LED to green
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, HIGH);

  loading = false;
  currentStage = 0;
}