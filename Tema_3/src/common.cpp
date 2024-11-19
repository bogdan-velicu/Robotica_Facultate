#include "common.h"
#include <Arduino.h>

int masterSPI = 0;
int score[2] = {0, 0};
bool sideState = false;

unsigned long startTime = millis();
unsigned long roundTime = 30000; // 30 seconds
unsigned long playerTime = 3000; // 3 seconds