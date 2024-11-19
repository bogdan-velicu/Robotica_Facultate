#ifndef COMMON_H
#define COMMON_H

#pragma once

extern int masterSPI;

// Definirea pinilor pentru SPI
const int miso = 12;
const int mosi = 11;
const int sck = 13;
const int ss = 10;

extern int score[2];
extern bool sideState;

extern unsigned long startTime;
extern unsigned long roundTime;
extern unsigned long playerTime;

#endif // COMMON_H