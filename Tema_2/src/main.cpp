#include <Arduino.h>

// Pin definitions
const int ledRedPin = 9;
const int ledGreenPin = 10;
const int ledBluePin = 11;
const int buttonStartStopPin = 2;
const int buttonDifficultyPin = 3;

// Variables
volatile bool gameActive = false;
volatile int difficulty = 0; // 0: Easy, 1: Medium, 2: Hard
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 300;
unsigned long roundStartTime = 0;
const unsigned long roundDuration = 30000; // 30 seconds
unsigned long wordStartTime = 0;
const unsigned long easyTimeLimit = 5000; // 5 seconds
const unsigned long mediumTimeLimit = 4000; // 4 seconds
const unsigned long hardTimeLimit = 3000; // 3 seconds
const char* words[] = {"hello", "world", "arduino", "programming", "homework", "robotics", "engineering", "electronics", "computers", "technology", "science", "mathematics",
                        "physics", "chemistry", "biology", "geology", "astronomy", "meteorology", "oceanography", "ecology", "environment", "conservation", "sustainability"};

char currentWord[50];
int currentWordIndex = 0;
int randomIndex = 0;
unsigned long matchedWords = 0;

unsigned long countdownStartTime = 0;
int countdownValue = 0;
bool countdownActive = false;
bool ledState = false;
unsigned long lastLedToggleTime = 0;
const unsigned long ledToggleInterval = 500; // 500 milliseconds

// Function declarations
void setup();
void loop();
void startStopButtonISR();
void difficultyButtonISR();
void setLEDColor(bool red, bool green, bool blue);
void stopRound();
void changeDifficulty();
void displayWord();
bool checkWord();
void readInput();
void handleCountdown();

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize LED pins
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);
  setLEDColor(1, 1, 1); // White color for idle state

  // Initialize button pins
  pinMode(buttonStartStopPin, INPUT_PULLUP);
  pinMode(buttonDifficultyPin, INPUT_PULLUP);

  // Attach interrupts to buttons
  attachInterrupt(digitalPinToInterrupt(buttonStartStopPin), startStopButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonDifficultyPin), difficultyButtonISR, FALLING);
}

void loop() {
  if (countdownActive) {
    handleCountdown();
  } else if (gameActive) {
    unsigned long currentTime = millis();
    if (currentTime - roundStartTime >= roundDuration) {
      stopRound();
      return;
    }
    readInput();
    if (checkWord()) {
      displayWord();
    }
  }
}

void readInput() {
  // Read from serial into currentWord
  if (Serial.available() > 0) {
    char input = Serial.read();
    // print the character
    Serial.print(input);

    // If backspace is pressed
    if (input == 8) {
      if (currentWordIndex > 0) {
        currentWord[currentWordIndex] = '\0';
        currentWordIndex--;
      }
    }
    else {
      currentWord[currentWordIndex] = input;
      currentWordIndex++;
    }

    // Check if the word is correct so far and set the color of the LED accordingly
    if (strncmp(currentWord, words[randomIndex], currentWordIndex) == 0) {
      setLEDColor(0, 1, 0); // Green LED for correct input so far
    } else {
      setLEDColor(1, 0, 0); // Red LED for incorrect input
    }
  }
}

void displayWord() {
  wordStartTime = millis();
  randomIndex = random(0, 23);
  Serial.println("");
  Serial.print("");
  Serial.println(words[randomIndex]);
  // also clear the currentWord
  currentWordIndex = 0;
  for (int i = 0; i < 50; i++) {
    currentWord[i] = '\0';
  }
}

bool checkWord() {
  unsigned long currentTime = millis();
  unsigned long timeLimit;
  switch (difficulty) {
    case 0:
      timeLimit = easyTimeLimit;
      break;
    case 1:
      timeLimit = mediumTimeLimit;
      break;
    case 2:
      timeLimit = hardTimeLimit;
      break;
  }
  if (currentTime - wordStartTime >= timeLimit) {
    return true; // Time limit exceeded, display new word
  }

  if (strcmp(currentWord, words[randomIndex]) == 0) {
    matchedWords++;
    Serial.println("");
    Serial.println("Correct!");
    Serial.print("");
    return true; // Word is correct, display new word
  }
  return false; // Continue checking
}

void startStopButtonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    if (gameActive || countdownActive) {
      stopRound();
    } else {
      countdownActive = true;
      countdownStartTime = millis();
      countdownValue = 3;
      Serial.println(countdownValue);
    }
    lastDebounceTime = currentTime;
  }
}

void difficultyButtonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    changeDifficulty();
    lastDebounceTime = currentTime;
  }
}

void setLEDColor(bool red, bool green, bool blue) {
  digitalWrite(ledRedPin, red);
  digitalWrite(ledGreenPin, green);
  digitalWrite(ledBluePin, blue);
}

void handleCountdown() {
  unsigned long currentTime = millis();
  if (currentTime - countdownStartTime >= 1000) {
    countdownStartTime = currentTime;
    countdownValue--;
    Serial.println(countdownValue);
    if (countdownValue == 0) {
      countdownActive = false;
      gameActive = true;
      roundStartTime = millis();
      setLEDColor(0, 1, 0); // Green color for active state
      Serial.println("Round started!");
      displayWord();
    }
  }

  if (currentTime - lastLedToggleTime >= ledToggleInterval) {
    lastLedToggleTime = currentTime;
    ledState = !ledState;
    setLEDColor(ledState, ledState, ledState); // Flash LED
  }
}

void stopRound() {
  gameActive = false;
  setLEDColor(1, 1, 1); // White color for idle state
  Serial.println("");
  Serial.println("");
  Serial.println("Round stopped!");
  Serial.print("Score: ");
  Serial.println(matchedWords);
  matchedWords = 0;
}

void changeDifficulty() {
  difficulty = (difficulty + 1) % 3;
  switch (difficulty) {
    case 0:
      Serial.println("Easy mode on!");
      break;
    case 1:
      Serial.println("Medium mode on!");
      break;
    case 2:
      Serial.println("Hard mode on!");
      break;
  }
}