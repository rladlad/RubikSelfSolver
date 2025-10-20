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

//MOVE DEFINES that are group into 3
//this will be used to define SCRAMBLING so aas not to make UNNECESSARY SCRAMBLINg like LEFT CW and NEXT is LEFT CCW
#define L 0
#define LP 1
#define L180 2
#define F 3
#define FP 4
#define F180 5
#define R 6
#define RP 7
#define F180 8
#define B 9
#define BP 10
#define B180 11
#define U 12
#define UP 13
#define U180 14
#define D 15
#define DP 16
#define D180 17

#endif
