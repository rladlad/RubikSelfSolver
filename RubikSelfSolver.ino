#include "Cube.h"
#include "PCA9548A.h"
#include <arduino.h>
#include "ServoArm.h"
//#include "BluetoothSerial.h"  //for default esp32; but the DEV BOARD might be too big
#include "BLEBOT.h"
#include "MyFunctions.h"
#include "HardwareSerial.h"
#include "CommandParser.h"
using namespace RubikBot;


const int STATE_IDLE = 0;
const int STATE_READY = 1;
const int STATE_MOVING = 2;
const int STATE_SCRAMBLING = 3;
const int STATE_PREPARESOLVE = 4;
const int STATE_WAITINGFORCOLORS=5;
const int STATE_CHECKINGCUBECOLORS = 6;
const int STATE_READYTOSOLVE = 7;
const int STATE_PREPSOLUTION = 8;
const int STATE_SOLVING = 9;
const int STATE_SOLVED = 10;


//other defines
#define DEV_ADDR 0x06
#define REG_ADDR 0x03   //the other data is 4

//GPIO defines
#define UART_TX 2       //this is RX pin in ESP32
#define UART_RX 1       //this is TX pin in ESP32

#define SCLPin 20
#define SDAPin 9        //originally 21
#define ResetPin 7

#define PWM_O  8        //originally 12
#define PWM_G  13
#define PWM_R  14
#define PWM_B  15
#define PWM_W  18
#define PWM_Y  19

#define SERVODRIVER 6
#define BATT_ADC 0


HardwareSerial ss(1);

Rubik* pCube{ nullptr };      //the one and only pointer to the rubik
PCA9548A mux(SDAPin,SCLPin,ResetPin);

String inputString = "";      // String to hold incoming data
//String inputStringBT = "";
bool stringComplete = false;  // Whether the string is complete
//bool stringCompleteBT = false; //

const int MAXBUFFSIZE = 100;
char btInputBuffer[MAXBUFFSIZE];
int btIndex = 0;
bool btInputReady = false;

int state = STATE_IDLE;
int solvecounter{0};

//SERVO ARMS
ServoArm servoOrange(PWM_O);
ServoArm servoGreen(PWM_G);
ServoArm servoRed(PWM_R);
ServoArm servoBlue(PWM_B);
ServoArm servoWhite(PWM_W);
ServoArm servoYellow(PWM_Y);

ServoArm* pServoArm[6]{};

//for testing
uint32_t currentseed{ 1 };
bool isSolving{ false };

//timers
unsigned long battTimer=0;

//GLOBAL state variables
bool isMoveStarted = false;
bool isScramblingStarted = false;

CommandParser parser;


void setup() {
  //setup the serial communications
  Serial.begin(115200);     //we may not need this after testing (this is UART0 of ESP32)
  delay(500);
  Serial.println("Serial Ready");

  //set up the BT Serial to start accepting communications
  Serial1.begin(115200, SERIAL_8N1, UART_RX, UART_TX); //these pins must be tested

  initBleBot();     //initialize the BLUETOOTH LE

  //initialize all pins
  initAllPins();

  //turn on ResetPin
  digitalWrite(ResetPin,HIGH);

  //turn of servo power
  ServoOn(false);

  //initialize the i2c MUX
  mux.begin();
  mux.set();


  //initialize all servo arms; do not send PWM yet; 
  servoOrange.setRotationSpeedCW(1700);
  servoOrange.setRotationSpeedCCW(1300);
  servoGreen.setRotationSpeedCW(1700);
  servoGreen.setRotationSpeedCCW(1300);
  servoRed.setRotationSpeedCW(1700);
  servoRed.setRotationSpeedCCW(1300);
  servoBlue.setRotationSpeedCW(1700);
  servoBlue.setRotationSpeedCCW(1300);
  servoWhite.setRotationSpeedCW(1700);
  servoWhite.setRotationSpeedCCW(1300);
  servoYellow.setRotationSpeedCW(1700);
  servoYellow.setRotationSpeedCCW(1300);

  
  //store it in the array
  pServoArm[0]=&servoOrange;
  pServoArm[1]=&servoGreen;
  pServoArm[2]=&servoRed;
  pServoArm[3]=&servoBlue;
  pServoArm[4]=&servoWhite;
  pServoArm[5]=&servoYellow;

  //setup the rubik cube
  pCube = new Rubik();
  pCube->initialize();
  state=STATE_IDLE;
  inputString.reserve(50);
  //inputStringBT.reserve(50);

  //read the initial battery on startup so we wont be surprised
  float value = readBattery();
  String message = "batt:" + String(value, 2); // 2 decimal place
  bleSendMessage(message);
    
}

void loop() {

    
  //report battery voltage every five seconds
  if (millis() >= battTimer + 5000){
      float value = readBattery();

      //report to the BT serial
      String message = "batt:" + String(value, 2); // 2 decimal place
      message.concat("\n");
      bleSendMessage(message);

      battTimer = millis();
  }

  //serialEvent();    //the main serial UART
  checkBTSerial();   //read the Serial for BT comms

  bool cmdHandled = true;   //all commands were handled

  //parse the input command
  if (btInputReady) {
    btInputReady = false;
    bool result = parser.parse(btInputBuffer);
    
    if (result){
      cmdHandled=false;
    }

    //execute the command here
    //for now just reflect in the serial
    Serial.print("Command >> ");
    Serial.println(parser.getCommand());

    if (parser.getParamCount()>0){
      Serial.println("Params");

      for (int i=0; i< parser.getParamCount();i++){
          Serial.print("Param" + i );
          Serial.print(" : ");
          Serial.println(parser.getParam(i));
      }
    }
  }

  

  //the STATE machine; process only the valid commands in each state
  switch (state){
    case STATE_IDLE:
      if (!cmdHandled){
        if (parser.getCommand() == "POWERON"){
          //power the motor
          ServoOn(true);

          //set reference angles on all servos

          Serial.println("Receive PowerOn command");
          //send back the success message
          Serial1.println("Overlord is Powered On");

          cmdHandled = true;
          state = STATE_READY;
        }
      }
      break;
    case STATE_READY:
      if (!cmdHandled){
        if (parser.getCommand() == "MOVE"){

          cmdHandled=true;
          state= isMoveStarted=false;
          state= STATE_MOVING;
        }
        else if(parser.getCommand()=="SCRAMBLE"){

          state = STATE_SCRAMBLING;
          isScramblingStarted=false;
          cmdHandled=true;
        }
        else if (parser.getCommand() == "SOLVECUBE"){
          state = STATE_PREPARESOLVE;
          cmdHandled=true;
        }
        else{
          cmdHandled = false;
        }
      }
      break;
    case STATE_MOVING:
      //no command should be processed here; instead just initiate the start and ending of a MOVE
      if (!isMoveStarted){
        //do the move

        isMoveStarted = true;
      }
      else{
        //move already started ; check if all servos are not moving
        bool isMoving=false;
        for (int i=0; i<6 ; i++){
          if (pServoArm[i]->isMoving()){
            isMoving=true;
            break;
          }
        }

        if (!isMoving){
          //we done moving
          Serial1.println("Move done");
          state = STATE_READY;
        }
      }
      break;
    case STATE_SCRAMBLING:
      if (!isScramblingStarted){

        isScramblingStarted = true;
      }
      else{
        //scrambling is already started; just exahust all moves

      }
      break;
    case STATE_PREPARESOLVE:
      //get the colors from the PC
      Serial1.println("GETCOLORS");
      state=STATE_WAITINGFORCOLORS;
      break;
    case STATE_WAITINGFORCOLORS:
      break;
    case STATE_CHECKINGCUBECOLORS:
      break;
    case STATE_READYTOSOLVE:
      break;
    case STATE_PREPSOLUTION:
      break;
    case STATE_SOLVING:
      break;
    case STATE_SOLVED:
      break;
    default:
      break;
  }

  if (cmdHandled){
    
  }
  else{
    //check for unhandled commands; like abort; or if it is an unknown command
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


void checkBTSerial() {
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      btInputBuffer[btIndex] = '\0';
      btInputReady = true;
      btIndex = 0;
    } else if (btIndex < MAXBUFFSIZE - 1) {
      btInputBuffer[btIndex++] = c;
    } else {
      Serial.println("BT input too long!");
      btIndex = 0;
    }
  }
}


void executeCommandBT(String cmd){
  cmd.trim();
  //write it to the serial monitor for now
  Serial.println(cmd);
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

  pinMode(ResetPin,OUTPUT);
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
  //read the BATYT_ADC pin and return the equivalent value in volts
  int value= analogRead(BATT_ADC); //12 bits adc
  float vbatt = (147.0/47.0) * (value / 4095.0);

  return vbatt;
}


void ServoOn(bool ok){
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
      Serial0.println();
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
