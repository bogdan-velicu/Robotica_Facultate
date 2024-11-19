#include <Arduino.h>
#include <SPI.h>
#include "common.h"

// Definirea pinilor pentru LED-uri
const int led_red = 2;
const int led_green = 3;
const int led_blue = 4;

const int rgb_red = 5;
const int rgb_green = 6;
const int rgb_blue = 7;

// Definirea pinului pentru deciderea side-ului activ
const int pin_side = 8;

// Definirea pinilor pentru multiplexarea butoanelor
const int joystick_mux1 = A0;
const int joystick_mux2 = A1;

int joystickState1 = 0;
int joystickState2 = 0;

volatile int recv;
volatile char command;
volatile int button1, button2;
volatile bool commandReceived = false;

void animatieLed() {
  for (int i = 0; i < 4; i++) {
    // Animație pentru LED-uri RGB
    digitalWrite(rgb_red, HIGH);
    delay(200);
    digitalWrite(rgb_red, LOW);
    digitalWrite(rgb_green, HIGH);
    delay(200);
    digitalWrite(rgb_green, LOW);
    digitalWrite(rgb_blue, HIGH);
    delay(200);
    digitalWrite(rgb_blue, LOW);

    // Animație pentru LED-uri simple
    digitalWrite(led_red, HIGH);
    delay(200);
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, HIGH);
    delay(200);
    digitalWrite(led_green, LOW);
    digitalWrite(led_blue, HIGH);
    delay(200);
    digitalWrite(led_blue, LOW);

    // Schimbarea side-ului activ o singură dată per loop
    sideState = !sideState;
    digitalWrite(pin_side, sideState ? HIGH : LOW);
  }
}

int convertButtonState(int buttonState) {
  if (buttonState >= 200 && buttonState <= 350) {
    return 1;
  } else if (buttonState >= 400 && buttonState <= 600) {
    return 2;
  } else if (buttonState >= 900 && buttonState <= 1100) {
    return 3;
  } else {
    return 0;
  }
}

void initSlave() {
  // Configurarea pinilor LED-urilor ca ieșire
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, LOW);
  digitalWrite(led_blue, LOW);

  pinMode(rgb_red, OUTPUT);
  pinMode(rgb_green, OUTPUT);
  pinMode(rgb_blue, OUTPUT);
  digitalWrite(rgb_red, LOW);
  digitalWrite(rgb_green, LOW);
  digitalWrite(rgb_blue, LOW);

  pinMode(pin_side, OUTPUT);
  digitalWrite(pin_side, LOW);

  // Configurarea pinilor pentru multiplexarea butoanelor ca intrare
  pinMode(joystick_mux1, INPUT);
  pinMode(joystick_mux2, INPUT);

  Serial.begin(9600);

  pinMode(miso, OUTPUT); // Asigură-te că MISO este setat ca ieșire
  SPCR |= _BV(SPE); // Activează SPI (Slave)
  SPI.attachInterrupt(); // Activează întreruperile SPI

  // Animație pentru LED-uri RGB și simple
  animatieLed();
}

ISR (SPI_STC_vect)
{
  if (!commandReceived) {
    command = SPDR;
    commandReceived = true;
    Serial.print("Command received: ");
    Serial.println(command);
  } else {
    recv = SPDR;
    // Serial.print("Value received: ");
    // Serial.println(recv);

    if (command == 'a') {
      int color = recv;
      switch (color) {
        case 1:
          digitalWrite(rgb_red, HIGH);
          digitalWrite(rgb_green, LOW);
          digitalWrite(rgb_blue, LOW);
          break;
        case 2:
          digitalWrite(rgb_red, LOW);
          digitalWrite(rgb_green, HIGH);
          digitalWrite(rgb_blue, LOW);
          break;
        case 3:
          digitalWrite(rgb_red, LOW);
          digitalWrite(rgb_green, LOW);
          digitalWrite(rgb_blue, HIGH);
          break;
        default:
          digitalWrite(rgb_red, LOW);
          digitalWrite(rgb_green, LOW);
          digitalWrite(rgb_blue, LOW);
          break;
      }
    } else if (command == 'b') {
      sideState = recv;
      digitalWrite(pin_side, sideState ? HIGH : LOW);
    } else if (command == 'c') {
      int buttonState1 = analogRead(joystick_mux1);
      int buttonState2 = analogRead(joystick_mux2);
      SPDR = convertButtonState(buttonState1);
      delay(10); // Adaugă un mic delay pentru a permite master-ului să citească valoarea
      SPDR = convertButtonState(buttonState2);
    }

    commandReceived = false; // Reset command received flag
  }
}

// Aprinderea LED-urilor în funcție de butonul apăsat
// Returnează 0 dacă nu s-a apăsat niciun buton
// Returnează 1 dacă s-a apăsat butonul 1 - LED-ul roșu
// Returnează 2 dacă s-a apăsat butonul 2 - LED-ul verde
// Returnează 3 dacă s-a apăsat butonul 3 - LED-ul albastru
int handleButtonPress(int buttonState, int side) {
  int convertedState = convertButtonState(buttonState);
  switch (convertedState) {
    case 1:
      digitalWrite(led_red, HIGH);
      digitalWrite(led_green, LOW);
      digitalWrite(led_blue, LOW);
      digitalWrite(pin_side, side ? HIGH : LOW);
      break;
    case 2:
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, HIGH);
      digitalWrite(led_blue, LOW);
      digitalWrite(pin_side, side ? HIGH : LOW);
      break;
    case 3:
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, LOW);
      digitalWrite(led_blue, HIGH);
      digitalWrite(pin_side, side ? HIGH : LOW);
      break;
    default:
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, LOW);
      digitalWrite(led_blue, LOW);
      break;
  }
}

void slaveLoop() {
  joystickState1 = analogRead(joystick_mux1);
  joystickState2 = analogRead(joystick_mux2);

  handleButtonPress(joystickState1, 0);
  handleButtonPress(joystickState2, 1);
}