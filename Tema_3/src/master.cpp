#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <SPI.h>
#include "common.h"

// Definirea pinilor pentru LCD
const int d4 = 7;
const int d5 = 6;
const int d6 = 5;
const int d7 = 4;
const int rs = A1;
const int en = A0;

// Definirea pinilor pentru servo
const int servo_pin = 8;

// Definirea pinului pentru buzzer
const int buzzer = 7;

bool jocFinalizat = false;
bool currentPlayer = false;

int ledColor;

static unsigned long playerStartTime = millis();
unsigned long currentTime = millis();

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Servo servo;

void comunicareSPIMaster() {
  // Trimiterea comenzii pentru setarea RGB-ului
  digitalWrite(ss, LOW);
  SPI.transfer('a');
  delay(100);
  SPI.transfer(ledColor);
  delay(100);

  // Trimiterea comenzii pentru setarea side-ului
  SPI.transfer('b');
  delay(100);
  SPI.transfer(currentPlayer ? 1 : 0);
  delay(100);

  // Trimiterea comenzii pentru primirea stării butoanelor
  SPI.transfer('c');
  delay(100);
  int buttonState1 = SPI.transfer(0);
  delay(20);
  int buttonState2 = SPI.transfer(0);
  delay(100);
  digitalWrite(ss, HIGH);

  Serial.print("Button 1: ");
  Serial.println(buttonState1);

  Serial.print("Button 2: ");
  Serial.println(buttonState2);

  // Verificarea stării butoanelor și incrementarea scorului
  if (buttonState1 == 0 && buttonState2 == 0) 
    return;
  
  if (buttonState1 == ledColor || buttonState2 == ledColor) {
    score[currentPlayer ? 0 : 1]++;
    lcd.clear();
    lcd.print("Scor: ");
    lcd.print(score[0]);
    lcd.print(" - ");
    lcd.print(score[1]);
    tone(buzzer, 1000, 1000);
  } else {
    noTone(buzzer);
  }
}

void initMaster() {
  pinMode(d4, OUTPUT);
  pinMode(d5, OUTPUT);
  pinMode(d6, OUTPUT);
  pinMode(d7, OUTPUT);
  pinMode(rs, OUTPUT);
  pinMode(en, OUTPUT);

  // Configurarea pinilor LCD-ului ca ieșire
  lcd.begin(16, 2);
  lcd.clear();
  
  // Afișarea mesajului "Salutare" cu animație pe LCD
  String message = "Salutare domnule";
  for (int i = 0; i < message.length(); i++) {
    lcd.setCursor(i, 0);
    lcd.print(message[i]);
    delay(150); // Delay pentru animație
  }

  // Configurarea pinilor pentru servo
  servo.attach(servo_pin);
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  pinMode(ss, OUTPUT);
  digitalWrite(ss, HIGH);

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  lcd.clear();
  lcd.print("Scor: ");
  lcd.print(score[0]);
  lcd.print(" - ");
  lcd.print(score[1]);

  startTime = millis();
  currentTime = millis();
  playerStartTime = millis();

  // TESTING
  // char buffer[16] = "testare";
  // digitalWrite(ss, LOW);
  // for (int i = 0; i < 7; i++) {
  //   SPI.transfer(buffer[i]);
  //   delay(100);
  // }
  // digitalWrite(ss, HIGH);
}

void masterLoop() {
  if (jocFinalizat) {
    lcd.clear();
    lcd.print("Joc finalizat!");
    lcd.setCursor(0, 1);
    lcd.print("Scor: ");
    lcd.print(score[0]);
    lcd.print(" - ");
    lcd.print(score[1]);
    return;
  }

  currentTime = millis();

  if (currentTime - playerStartTime >= playerTime) {
    playerStartTime = currentTime;
    currentPlayer = !currentPlayer;
    ledColor = random(1, 4);
  }

  if (currentTime - startTime >= roundTime) {
    jocFinalizat = true;
  }

  servo.write(map(currentTime - startTime, 0, roundTime, 0, 180));

  if (currentTime - playerStartTime <= playerTime) {
    comunicareSPIMaster();
  }
}