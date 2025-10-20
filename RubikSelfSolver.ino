#include "Cube.h"
#include <arduino.h>
#include "MyFunctions.h"
#include "OverlordTCA.h"
#include "CommandParser.h"
#include <wire.h>

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



//GPIO defines
#define UART_TX 2       //this is RX pin in ESP32
#define UART_RX 1       //this is TX pin in ESP32

#define SCLPin 14 
#define SDAPin 15 
#define nResetPin 18

#define BATT_ADC 0

Rubik* pCube{ nullptr };      //the one and only pointer to the rubik

//the TCA driver and the overlord controller declaration
TCA6424A tca;
OverlordTCA overlord(tca);
CommandParser parser;

int activeid=0;
int steps=1024;

const int MAXBUFFSIZE = 120;
char btInputBuffer[MAXBUFFSIZE];
int btIndex = 0;
bool btInputReady = false;

//GLOBAL state variables
int state = STATE_IDLE;
bool isMoveStarted = false;
bool isScramblingStarted = false;
bool isPoweredUp = false;           //true if the motors are powered


//variables and defines for scrambling
const int MAX_SCRAMBLE_MOVES = 30;
const int NUM_MOVE_TYPES = 18;

int scrambleMoves[MAX_SCRAMBLE_MOVES];
int scrambleCounter = 0;
int currentmove;
int activemotorid;
int scrambleindex;



//timers
unsigned long battTimer=0;



void setup() {
  //setup the serial communications
  Serial.begin(115200);  //for the serial monitot
  Serial1.begin(115200, SERIAL_8N1, UART_RX, UART_TX);  //for the HC06 serial
  delay(500);
  Serial.println("Serial Ready");

  //initialize i2c controller here to read/write to the TCA
  Wire.begin(SDAPin,SCLPin); //join as master

  //set the nResetPin Hight
  pinMode(nResetPin, OUTPUT);
  digitalWrite(nResetPin,HIGH);

  delay(2000);
  
  //setup the rubik cube; (the logical cube)
  pCube = new Rubik();
  pCube->initialize();    //reset to initial color
  state=STATE_IDLE;       //set initial state of the FSM

  //init TCA and the OverlordController (for the movement)
  tca.begin();
  overlord.setStepsPerRevolution(2048.0f);
  for (int i = 0; i < 6; i++) {
    overlord.registerMotor(i, i * 4);
    overlord.setMaxSpeed(i, 20.0f);
  }

  //read the initial battery on startup so we wont be surprised
  float value = readBattery();
  String message = "batt:" + String(value, 2); // 2 decimal place
  message.concat("\n");
  Serial1.println(message);
    
}

void loop() {

  //report battery voltage every five seconds
  if (millis() >= battTimer + 5000){
      float value = readBattery();

      //report to the BT serial
      String message = "batt:" + String(value, 2); // 2 decimal place
      message.concat("\n");
      Serial1.println(message);

      battTimer = millis();
  }

  //read the Serial from HC06
  checkBTSerial();  

  bool cmdHandled = true;   //all commands were handled

  //parse the input command
  if (btInputReady) {
    btInputReady = false;
    bool result = parser.parse(btInputBuffer);
    
    if (result){
      cmdHandled=false;
    }

    //the state machine will handle it if its not handled
  }

  //NOTE: comparison should be
  //strcmp(parser.getCommand(),"l")==0

  //the STATE machine; process only the valid commands in each state
  switch (state){
    case STATE_IDLE:
      //power down if not yet done
      if (isPoweredUp){
          overlord.powerDownMotors();
          Serial1.println("All motors are powered down");
          isPoweredUp=false;
      }


      if (!cmdHandled){ 
        if (parser.getCommand() == "POWERON"){
          state = STATE_READY;
          
          isPoweredUp=true;
          //send the state to the HC06 for logging
          Serial1.println("Motors are ON.");
          Serial1.println("Switching to READY");
        }
      }
      break;
    case STATE_READY:
      if (!cmdHandled){
        if (parser.getCommand() == "MOVE"){

          cmdHandled=true;
          isMoveStarted=false;
          state= STATE_MOVING;
        }
        else if(parser.getCommand()=="SCRAMBLE"){
          //format: SCRAMBLE 10 ; to scramble to 10 MOVES
          if (prepScramble(20))
          {
            //at this point,scrambleMoves is filled up to scrambleCounter
            state = STATE_SCRAMBLING;
            isScramblingStarted=false;
            cmdHandled=true;
          }
          else{
            //send error to HC06
            Serial1.println("Error in SCRAMBLING occurred.");
            Serial1.println("Switching to IDLE");
          }
        }
        else if (parser.getCommand() == "SOLVECUBE"){
          
          Serial1.println("SOLVE command recieved.");
          Serial1.println("Switching to PREPARESOLVE");
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
      }
      break;
    case STATE_SCRAMBLING:
      if (!isScramblingStarted){
        //loop the scrambleMoves up to scrambleCounter and physically rotate the cube
        isScramblingStarted = true;
        scrambleindex=0;
        currentmove = scrambleMoves[scrambleindex++];
 
        decodeMove(currentmove,activemotorid,steps);
        overlord.moveRelative(activemotorid, steps);
      }
      else{
        //scrambling is already started; just exhaust all moves
        if (!overlord.isBusy(activemotorid)){
          if (scrambleindex >= scrambleCounter)
          {
            state = STATE_READY;
            Serial1.println("Scramble complete. Transitioning to READY.");
          }
          else{ //there are still moves left
            currentmove = scrambleMoves[scrambleindex++]; //get the next move

            decodeMove(currentmove,activemotorid,steps);
            overlord.moveRelative(activemotorid, steps);
          }
        }
        else
        {
            overlord.update();
        }
      }
      break;
    case STATE_PREPARESOLVE:
      //get the colors from the PC ; send a command to the pc
      Serial1.println("COMMAND GETCOLORS");
      Serial1.println("Switching to WAITINGFORCOLORS");
      state=STATE_WAITINGFORCOLORS;
      waitingTimeSet=false;
      break;
    case STATE_WAITINGFORCOLORS:
      if (!waitingTimeSet){
        maxWaitTime = millis()+60000; //one minute wait time
        waitingTimeSet=true;
      }
      else{
        if (millis()>maxWaitTime){
          Serial1.println("Colors did not arrive on time.");
          Serial1.println("Switching to IDLE");

          state = STATE_IDLE;
          break;
        }

        //check if there are incoming colors command
        if (!cmdHandled){
          //get the command and parameter and process it; the valid command is COLORS

          //if colors are recieved, switching to CHECKING CUBE COLORS
          Serial1.println("Colors are recieved.");
          Serial1.printn("Switching to CHECKINGCOLORS");
          state = STATE_CHECKINGCUBECOLORS;
        }
      }
      break;
    case STATE_CHECKINGCUBECOLORS:
      if (!checkCubeColors()){
        //the checkCubeColors will be passed in the recieved colors to the cube. It will then check if it can solve the cube using the colors
        //if true; then it will proceed ; else it will raise and error
        Serial1.println("There is an error evaluating the recieved colors.");
        Serial1.println("Switching to PREPARESOLVE");
        state = STATE_PREPARESOLVE;
      }
      else{
        //the cube colors are OK; and the cube should have a SOLUTION;
        //SWITCH to READY TO SOLVE to await the GOAHEAD from 
        Serial1.println("Click on SOLVE to go ahead.");
        Serial1.println("Switching to READYTOSOLVE");
        state = STATE_READYTOSOLVE;
        waitingTimeSet = false;
      }
      break;
    case STATE_READYTOSOLVE:
      //wait here until SOLVE is clicked; or timeout of 1 minute
      if (!waitingTimeSet){
        maxWaitTime = millis() + 60000; 
        waitingTimeSet = true;
      }
      else{
        if (millis() maxWaitTime){
          Serial1.println("SOLVE button is not clicked on time.");
          Serial1.println("Switching to IDLE");
          state = STATE_IDLE;
          break;
        }

        //check if there is a COMMAND To GOAHEAD and solve
        if (!cmdHandled){
          //get the command and parameter and process it; the valid command is COLORS

          //if colors are recieved, switching to CHECKING CUBE COLORS
          Serial1.println("GOAHEAD recieved.");
          Serial1.printn("Switching to SOLVING");

          //the cube should have the solution already; so store it somewhere in an array to be exhausted

          isSolvingStarted = false;
          state = STATE_SOLVING;
        }

      }
      break;
    case STATE_SOLVING:
      if (!isSolvingStarted){

      }
      else{
        
      }
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

}


void checkBTSerial() {
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\r') continue;  // Skip carriage return

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
 
}

float readBattery(){
  //read the BATYT_ADC pin and return the equivalent value in volts
  int value= analogRead(BATT_ADC); //12 bits adc
  float vbatt = (147.0/47.0) * (value / 4095.0);

  return vbatt;
}

//prepare a random movements to scramble the cube
//count is how many many moves to do; should be maximum 20 scrambling moves 
//the moves are grouped into 3; there should be no serial moves that are from the same group
//like l and lp or l180 and l180



bool prepScramble(uint numMoves) {
  if (numMoves > MAX_SCRAMBLE_MOVES)
    numMoves = MAX_SCRAMBLE_MOVES;

  int index = 0;
  int lastGroup = -1;

  while (index < numMoves) {
    int move = random(NUM_MOVE_TYPES);
    int group = move / 3;

    if (group != lastGroup) {
      scrambleMoves[index++] = move;
      lastGroup = group;
    }
  }

  scrambleCounter = index;
  return true;
}

void decodeMove(int move, int& motor, int& steps) {
  int type = move % 3;
  motor = move / 3;
  steps = (type == 0) ? 512 : (type == 1 ? -512 : 1024);
}

