#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#include "master.h"
#include "slave.h"
#include "common.h"

void setup() {
  // Verificam daca este master sau slave din pinul A5
  pinMode(A5, INPUT);
  masterSPI = digitalRead(A5);

  if (masterSPI) {
    initMaster();
  } else {
    initSlave();
  }

  Serial.begin(9600);
}

void loop() {
  if (masterSPI) {
    masterLoop();
  } else {
    slaveLoop();
  }
}