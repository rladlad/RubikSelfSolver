#ifndef MYFUNCTIONS_H_
#define MYFUNCTIONS_H_

#include "Cube.h"

using namespace RubikBot;

//forward declarations
void initAllPins();
void readAngles();
float readBattery();
void ServoOn(bool ok = true);
void printFace(int face);
void printCube(Rubik* cube);
void clearSerialMonitor();
void executeCommand(String cmd);

#endif
