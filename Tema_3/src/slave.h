#ifndef SLAVE_H
#define SLAVE_H

int convertButtonState(int buttonState);
int handleButtonPress(int buttonState, int side);
void initSlave();
void slaveLoop();
void animatieLed();

// void comunicareSPI();

#endif // SLAVE_H