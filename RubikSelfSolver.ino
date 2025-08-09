#include "Cube.h"
#include "PCA9548A.h"
#include <arduino.h>
#include "ServoArm.h"

using namespace RubikBot;

const int STATE_IDLE = 0;
const int STATE_TESTSOLVE = 1;

//other defines
#define DEV_ADDR 0x06
#define REG_ADDR 0x03   //the other data is 4

//GPIO defines
#define SCLPin 22
#define SDAPin 21
#define ResetPin 2

#define PWM_O  15
#define PWM_G  16
#define PWM_R
#define PWM_B
#define PWM_W
#define PWM_Y



Rubik* pCube{ nullptr };      //the one and only pointer to the rubik
PCA9548A mux(SDAPin,SCLPin,ResetPin);

String inputString = "";      // String to hold incoming data
bool stringComplete = false;  // Whether the string is complete
int state = STATE_IDLE;
int solvecounter{0};

//SERVO ARMS
ServoArm servoOrange(PWM_O);
ServoArm servoGreen(PWM_G);

ServoArm* pServoArm[2]{};

//for testing
uint32_t currentseed{ 1 };
bool isSolving{ false };



//forward declarations
void printFace(int face);
void printCube(Rubik* cube);
void clearSerialMonitor();


void setup() {

  Serial.begin(115200);
  delay(1000);

  // put your setup code here, to run once:
  pCube = new Rubik();
  pCube->initialize();
  state=STATE_TESTSOLVE;
  inputString.reserve(50);

  //initialize the MUX
  mux.begin();
  mux.set();

  //initialize all servo arms
  servoOrange.attach();
  servoOrange.stop();

  servoGreen.attach();
  servoGreen.stop();

  //store it in the array
  pServoArm[1]=&servoOrange;
  pServoArm[0]=&servoGreen;

  //initialize all motors with starting angles
  uint8_t buffer[2];
  for (int i=0;i<2;i++){
    if (!mux.readFromDevice(i+2, DEV_ADDR, REG_ADDR, buffer, 2))
    {
        Serial.println("Reading failed");
    }
    else{
        uint16_t value = ((uint16_t)buffer[0] << 8) | buffer[1]; // Combine MSB/LSB
        value = value >> 2;                                  // remove the last 2 bits
        double angle = (value/16384.0)*360.0;

        pServoArm[i]->setCurrentAngle(angle);
    }
  }
}

void loop() {

  if (stringComplete) {
    executeCommand(inputString);
    inputString = "";
    stringComplete = false;
  }

  // switch (state) {
  //   case STATE_IDLE:
  //     break;
  //   case STATE_TESTSOLVE:
  //     if (!isSolving) {
  //       //shuffle using a seed (50 moves)
  //       //currentseed = analogRead(0);// ^ millis();
  //       randomSeed(currentseed++);
  //       pCube->shuffle(20);

  //       //solve
  //       pCube->solve();

  //       //if (!solved) print current seed and print the current cube
  //       if (!pCube->isSolved()) {
  //         clearSerialMonitor;
  //         Serial.println("Failed Solve!!!");
  //         Serial.print("Current Seed : ");
  //         Serial.println(currentseed);
  //         printCube(pCube);
  //         isSolving = false;
  //         state = STATE_IDLE;
  //       } else {
  //         pCube->initialize();
  //         isSolving = false;  //so that another test will be run
  //         solvecounter++;
  //         Serial.println("Solved Cube # :" + String(solvecounter));
  //         if (solvecounter>200000)
  //           state=STATE_IDLE;
  //       }

  //     }
  //     break;
  //   default:
  //     break;
  // }

  //read ALL SENSORS
  uint8_t buffer[2];
  for (int i=0;i<2;i++){
    if (!mux.readFromDevice(i+2, DEV_ADDR, REG_ADDR, buffer, 2))
    {
        Serial.println("Reading failed");
    }
    else{
        uint16_t value = ((uint16_t)buffer[0] << 8) | buffer[1]; // Combine MSB/LSB
        value = value >> 2;                                  // remove the last 2 bits
        double angle = (value/16384.0)*360.0;

        pServoArm[i]->setCurrentAngle(angle);
    }
  }

  //loop all servos and evaluate moves
  for (int i=0;i<2;i++){
    pServoArm[i]->evaluateMove();
  }

  delay(100);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

void executeCommand(String cmd) {
  cmd.trim();  // Remove whitespace and newline

  if (cmd == "print") {
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "shuffle") {
    pCube->shuffle(50);
  } else if (cmd == "init") {
    pCube->initialize();
  } else if (cmd == "solve") {
    clearSerialMonitor();
    printCube(pCube);
    pCube->solve();
    printCube(pCube);
  } else if (cmd == "cw") {
    pServoArm[0]->rotate90CW();
    Serial.println("cw");
  } else if (cmd == "ccw") {
    pServoArm[0]->rotate90CCW();
  
  } else if (cmd == "180") {
    pServoArm[0]->rotate90CCW();
  }
  else if (cmd == "d") {
    pCube->d();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "dp") {
    pCube->dp();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "l") {
    pCube->l();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "lp") {
    pCube->lp();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "f") {
    pCube->f();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "fp") {
    pCube->fp();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "r") {
    pCube->r();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "rp") {
    pCube->rp();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "b") {
    pCube->b();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "bp") {
    pCube->bp();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "u") {
    pCube->u();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "up") {
    pCube->up();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "d2") {
    pCube->d2();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "l2") {
    pCube->l2();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "f2") {
    pCube->f2();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "r2") {
    pCube->r2();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "b2") {
    pCube->b2();
    clearSerialMonitor();
    printCube(pCube);
  } else if (cmd == "u2") {
    pCube->u2();
    clearSerialMonitor();
    printCube(pCube);
  } else {
    Serial.println("Unknown Command");
  }
}





//local functions

void clearSerialMonitor() {
  for (int i = 0; i < 50; i++) {
    Serial.println();
  }
}

void printFace(int face) {
  char arr[3][3];

  if (pCube->getFace(face, &arr[0][0]) == true) {

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        Serial.print(arr[i][j]);
        Serial.print(' ');
      }
      Serial.println();
    }
  }
}

void printCube(Rubik* cube) {
  char arr[9][12];
  char temp[3][3];

  //initialize the array
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 12; j++) {
      arr[i][j] = ' ';
    }
  }

  cube->getFace(0, &temp[0][0]);
  for (int i = 3; i < 6; i++) {
    for (int j = 0; j < 3; j++) {
      arr[i][j] = temp[i - 3][j];
    }
  }

  cube->getFace(1, &temp[0][0]);
  for (int i = 3; i < 6; i++) {
    for (int j = 3; j < 6; j++) {
      arr[i][j] = temp[i - 3][j - 3];
    }
  }

  cube->getFace(2, &temp[0][0]);
  for (int i = 3; i < 6; i++) {
    for (int j = 6; j < 9; j++) {
      arr[i][j] = temp[i - 3][j - 6];
    }
  }

  cube->getFace(3, &temp[0][0]);
  for (int i = 3; i < 6; i++) {
    for (int j = 9; j < 12; j++) {
      arr[i][j] = temp[i - 3][j - 9];
    }
  }

  cube->getFace(4, &temp[0][0]);
  for (int i = 0; i < 3; i++) {
    for (int j = 3; j < 6; j++) {
      arr[i][j] = temp[i][j - 3];
    }
  }

  cube->getFace(5, &temp[0][0]);
  for (int i = 6; i < 9; i++) {
    for (int j = 3; j < 6; j++) {
      arr[i][j] = temp[i - 6][j - 3];
    }
  }

  //print
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 12; j++) {
      Serial.print(arr[i][j]);
      Serial.print(' ');
    }
    Serial.println();
  }
}
