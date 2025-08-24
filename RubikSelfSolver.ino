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

#define PWM_O  18
#define PWM_G  5
#define PWM_R  17
#define PWM_B  16
#define PWM_W  4
#define PWM_Y  0

#define LED1   23  
#define LED2   15
#define SW1    19
#define SERVODRIVER 27
#define BATT_ADC 34


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

int tempcounter{0};

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
  servoOrange.setRotationSpeedCW(1700);
  servoOrange.setRotationSpeedCCW(1300);
  servoOrange.stop();
  servoOrange.attach();
  
  servoGreen.stop();
  servoGreen.attach();
  
  //store it in the array
  pServoArm[1]=&servoOrange;
  pServoArm[0]=&servoGreen;

  //initialize all motors with reference angles
  uint8_t buffer[2];
  for (int i=0;i<2;i++){
    if (!mux.readFromDevice(i+2, DEV_ADDR, REG_ADDR, buffer, 2))
    {
        Serial.println("Reading failed");
    }
    else{
        uint16_t value = ((uint16_t)buffer[0] << 8) | buffer[1]; // Combine MSB/LSB
        value = value >> 2;                                  // remove the last 2 bits
        float angle = (value/16384.0)*360.0;

        pServoArm[i]->setReferenceAngle(angle);
    }
  }
}

void loop() {

  unsigned long looptimer = micros();

  if (stringComplete) {
    executeCommand(inputString);
    inputString = "";
    stringComplete = false;
  }
  
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

  //loop all servos and update moves
  for (int i=0;i<2;i++){
    pServoArm[i]->update();
  }
  
  while (++tempcounter<10){
    Serial.println(micros()-looptimer);
  }

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
    pServoArm[1]->rotate90CW();
    Serial.println("cw");
  } else if (cmd == "ccw") {
    pServoArm[1]->rotate90CCW();
    Serial.println("ccw");
  } else if (cmd == "180") {
    pServoArm[1]->rotate180();
    Serial.println("180");
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
void initAllPins(){
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);

  pinMode(SERVODRIVER,OUTPUT);
}

void readAngles(){
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

float readBattery(){
  //read the ADC1_CH6 (PIN 34) and return the equivalent value in volts
  int value= analogRead(34); //12 bits adc
  float vbatt = (147.0/47.0) * (value / 4095.0);

  return vbatt;
}

void LED1On(bool ok=true){
  if (ok)
    digitalWrite(LED1,1);
  else
    digitalWrite(LED1,0);
}

void LED2On(bool ok = true){
  if (ok)
    digitalWrite(LED2,1);
  else
    digitalWrite(LED2,0);
}

void ServoOn(bool ok = true){
  if (ok)
    digitalWrite(SERVODRIVER,1);
  else
    digitalWrite(SERVODRIVER,0);
}


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
