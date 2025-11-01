#include "Cube.h"
#include <arduino.h>
#include "MyFunctions.h"
#include "OverlordTCA.h"
#include "CommandParser.h"
#include <wire.h>
#include <HardwareSerial.h>

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
const int STATE_JOGGING = 11;
const int STATE_TEST = 12;



//GPIO defines
#define UART_TX 20       //this is RX pin in ESP32
#define UART_RX 19       //this is TX pin in ESP32

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
bool isWaitingTimeSet = false;
long maxWaitTime =0;
bool isSolvingStarted=false;

int numberOfMoves =0;
int moveIndex = 0;
int currentMove = 0;

//variables and defines for scrambling
const int MAX_SCRAMBLE_MOVES = 30;
const int NUM_MOVE_TYPES = 18;

int scrambleMoves[MAX_SCRAMBLE_MOVES];
int scrambleCounter = 0;
int currentmove;
int activemotorid;
int scrambleindex;
String activedirection;

//for color parsing
const int COLOR_COUNT = 54;
int colors[COLOR_COUNT];

//timers
unsigned long battTimer=0;
bool battInfoPending = false; //set to true when there is batt info that is needed to be sent
String battInfo;              //the info to be sent

//function prototypes
void checkBTSerial();
bool prepScramble(uint numMoves) ;
void decodeMove(int move, int& motor, int& steps);
void convertMove(int move, int& motor, int& steps);
void printMove();
bool checkCubeColors();
bool parseColors(const char* input) ;

const float ADC_CORRECTION = 7.4 / 2337.0; // ≈ 0.003166


//ALIAS declaration for Serial 
Stream& btSerial = Serial1;  //replace with Serial1 if using BT


void setup() {
  //setup the serial communications
  Serial.begin(115200);  //for the serial monitot
  Serial1.begin(115200, SERIAL_8N1, UART_RX, UART_TX);  //for the HC06 serial
  delay(500);
  Serial.println("Serial Ready");
  btSerial.println("BT Serial Ready");

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
  btSerial.println(message);

  //all motors should be down initially 
  overlord.powerDownMotors(); //power down all motors

}

void loop() {

  //report battery voltage every five seconds
  //we need to send this ONLY when MOTORS are not busy (especially if its causing jitters. Not yet done)
  if (millis() >= battTimer + 5000){
      float value = readBattery();

      //report to the BT serial
      battInfo = "batt: " + String(value, 2); // 2 decimal place

      //set info pending to true
      battInfoPending=true;

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
    case STATE_TEST:
        if (strcmp(parser.getCommand(),"TEST")==0){
          pCube->initialize();  //init to solved
          pCube->shuffle(200);
          //pCube->solve();   //the moves will be filled in return
          String s= pCube->getFaceletString();
          Serial.println(s);
        }
        battInfoPending = false;  //dont send batt info
        break;
    case STATE_IDLE:
      //power down if not yet done
      if (isPoweredUp){
          overlord.powerDownMotors();
          btSerial.println("All motors are powered down");
          isPoweredUp=false;
      }


      if (!cmdHandled){ 
        if (strcmp(parser.getCommand(),"POWERON")==0){
          state = STATE_READY;
          
          isPoweredUp=true;
          //send the state to the HC06 for logging
          btSerial.println("Motors are ON.");
          btSerial.println("Switching to READY");

          cmdHandled=true;
        }
      }
      break;
    case STATE_READY:
      if (!cmdHandled){
        if (strcmp(parser.getCommand(),"MOVE")==0){
          //this is for QUARTER and 180 MOVES with format MOVE MotorID CCW; ex MOVE 0 CCW
          btSerial.println("MOVE command received.");
          btSerial.println("Switching to MOVE");
          const char* sid = parser.getParam(0);
          const char* sdir = parser.getParam(1);
          if (sid != nullptr && sdir != nullptr){
            activemotorid = std::atoi(sid);
            steps=0;
            if (strcmp(sdir,"CW")==0){
              steps=512;
            }
            else if (strcmp(sdir,"CCW")==0){
              steps=-512;
            }
            else if (strcmp(sdir,"180")==0){
              steps=1024;  
            }
            else{
              btSerial.println("Parameter error for MOVE(No DIR specified)");
            }

            isMoveStarted=false;
            state= STATE_MOVING;
          }
          else{
            btSerial.println("Parameter error for MOVE (ex MOVE MotorId DIR) ");
          }

          cmdHandled=true;
          
        }
        else if (strcmp(parser.getCommand(),"JOG")==0){
          //this will handle JOG with format JOG MotorIID STEPS; JOG 0 -8
          btSerial.println("JOG command received.");
          btSerial.println("Switching to JOG");
          const char* sid = parser.getParam(0);
          const char* sdir = parser.getParam(1);
          if (sid != nullptr){
            activemotorid = std::atoi(sid);
            steps=8;
            if (sdir != nullptr){
              steps=std::atoi(sdir);
            }
            isMoveStarted=false;
            state= STATE_JOGGING;
          }
          else{
            btSerial.println("Parameter error for JOG (ex JOG MotorId DIR) ");
          }

          cmdHandled=true;
        }
        else if(strcmp(parser.getCommand(),"SCRAMBLE")==0){
          //format: SCRAMBLE 10 ; to scramble to 10 MOVES
          const char* smoves = parser.getParam(0); 
          int nummoves = 20;
          if (smoves != nullptr){
            nummoves = std::atoi(smoves);
          }
          if (smoves)
          if (prepScramble(nummoves))
          {
            //at this point,scrambleMoves is filled up to scrambleCounter
            btSerial.println("Switching to SCRAMBLING");
            state = STATE_SCRAMBLING;
            isScramblingStarted=false;
            cmdHandled=true;
          }
          else{
            //send error to HC06
            btSerial.println("Error in SCRAMBLING occurred.");
            btSerial.println("Switching to IDLE");
          }
        }
        else if (strcmp(parser.getCommand(),"SOLVECUBE")==0){
          
          btSerial.println("SOLVE command recieved.");
          btSerial.println("Switching to PREPARESOLVE");
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
        overlord.moveRelative(activemotorid, steps);
        isMoveStarted = true;
      }
      else{
        //move already started ; check if all servos are not moving
        if (!overlord.isBusy(activemotorid)){
          //move is finished
          btSerial.println("MOVE is DONE");
          btSerial.println("Switching to READY");
          overlord.powerDownMotor(activemotorid);
          state = STATE_READY;
        }
        else
        {
            overlord.update();
        }
      }
      break;
    case STATE_JOGGING:
      //no command should be processed here; instead just initiate the start and ending of a MOVE
      if (!isMoveStarted){
        //do the JOG
        overlord.moveRelative(activemotorid, steps);
        isMoveStarted = true;
      }
      else{
        //JOG already started ; check if all servos are not moving
        if (!overlord.isBusy(activemotorid)){
          //JOG is finished
          btSerial.println("JOG is DONE");
          btSerial.println("Switching to READY");
          overlord.powerDownMotor(activemotorid);
          state = STATE_READY;
        }
        else
        {
            overlord.update();
        }
      }
      break;
    case STATE_SCRAMBLING:
      if (!isScramblingStarted){
        //loop the scrambleMoves up to scrambleCounter and physically rotate the cube
        isScramblingStarted = true;
        scrambleindex=0;
        currentmove = scrambleMoves[scrambleindex++];
 
        decodeMove(currentmove,activemotorid,steps);

        //these lines are for debugging only
        printMove();

        overlord.moveRelative(activemotorid, steps);
      }
      else{
        //scrambling is already started; just exhaust all moves
        if (!overlord.isBusy(activemotorid)){
          //power down the motor
          overlord.powerDownMotor(activemotorid);

          //if there as batt info to send , do it
          if (battInfoPending){
            btSerial.println(battInfo);
            battInfoPending=false;
          }

          if (scrambleindex >= scrambleCounter)
          {
            state = STATE_READY;
            btSerial.println("Scramble complete. Transitioning to READY.");
          }
          else{ //there are still moves left
            currentmove = scrambleMoves[scrambleindex++]; //get the next move

            decodeMove(currentmove,activemotorid,steps);

            //these lines are for debugging only
            printMove();

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
      btSerial.println("GETCOLORS");
      btSerial.println("Switching to WAITINGFORCOLORS");
      state=STATE_WAITINGFORCOLORS;
      isWaitingTimeSet=false;
      break;
    case STATE_WAITINGFORCOLORS:
      if (!isWaitingTimeSet){
        maxWaitTime = millis()+60000; //one minute wait time
        isWaitingTimeSet=true;
      }
      else{
        if (millis()>maxWaitTime){
          btSerial.println("Colors did not arrive on time.");
          btSerial.println("Switching to IDLE");

          state = STATE_IDLE;
          break;
        }

        //check if there are incoming colors command
        if (!cmdHandled){
          //get the command and parameter and process it; the valid command is COLORS
          if(strcmp(parser.getCommand(),"COLORS")==0){
            //parse the colors
            if (!parseColors(parser.getParam(0))){
                btSerial.println("Passed in colors are in error.");
                state=STATE_PREPARESOLVE;
            }
            else{
              //if colors are recieved and parsed; the colors[] array will have the passed in colors
              btSerial.println("54 Colors are recieved.");
              btSerial.println("Switching to CHECKINGCOLORS");
              state = STATE_CHECKINGCUBECOLORS;
            }  
            cmdHandled=true;
          }
        }
      }
      break;
    case STATE_CHECKINGCUBECOLORS:
      if (!checkCubeColors()){
        //the checkCubeColors will be passed in the recieved colors to the cube. It will then check if it can solve the cube using the colors
        //if true; then it will proceed ; else it will raise and error
        btSerial.println("There is an error evaluating the recieved colors.");
        btSerial.println("Switching to PREPARESOLVE");
        state = STATE_PREPARESOLVE;
      }
      else{
        //the cube colors are OK; and the cube should have a SOLUTION;
        //pCube->initialize();  //init to solved
        //pCube->shuffle(200);
        //pCube->solve();   //the moves will be filled in return

        pCube->initialize(colors);
        pCube->solve();
        String s= pCube->getFaceletString();
        Serial.println(s);

        if (pCube->isSolved()){
          //optimize moves
          pCube->optimizeMoves();
          btSerial.print("The cube can be solved in ");
          btSerial.print(pCube->getNumberOfMoves());
          btSerial.println(" moves");

          //SWITCH to READY TO SOLVE to await the GOAHEAD from 
          btSerial.println("Click on SOLVE to go ahead.");
          btSerial.println("Switching to READYTOSOLVE");
          state = STATE_READYTOSOLVE;
          isWaitingTimeSet = false;
        }
        else{
          //unable to solve the cube even if the colors are OK. Strange
          btSerial.println("UNABLE TO SOLVE CUBE");
          btSerial.println("Switching to IDLE");  
          state = STATE_IDLE;
        }
      }
      break;
    case STATE_READYTOSOLVE:
      //wait here until SOLVE is clicked; or timeout of 1 minute
      if (!isWaitingTimeSet){
        maxWaitTime = millis() + 60000; 
        isWaitingTimeSet = true;
      }
      else{
        if (millis() > maxWaitTime){
          btSerial.println("SOLVE button is not clicked on time.");
          btSerial.println("Switching to IDLE");
          state = STATE_IDLE;
          break;
        }

        //check if there is a COMMAND To GOAHEAD and solve
        if (!cmdHandled){
          //get the command and parameter and process it;
          if(strcmp(parser.getCommand(),"GOAHEAD")==0){
         
            //if colors are recieved, switching to CHECKING CUBE COLORS
            btSerial.println("GOAHEAD recieved.");
            btSerial.println("Switching to SOLVING");

            //the cube should have the solution already; so store it somewhere in an array to be exhausted

            isSolvingStarted = false;
            state = STATE_SOLVING;
            cmdHandled=true;
          }
        }

      }
      break;
    case STATE_SOLVING:
      if (!isSolvingStarted){
        //the cube has an array of moves to solve it;
        //get and execute each move
        numberOfMoves= pCube->getNumberOfMoves();
        moveIndex = 0;
        if (numberOfMoves==0){
          //already solved
          btSerial.println("Cube is already solved.");
          state= STATE_SOLVED;
        }
        else{
          currentMove = pCube->getMoveAt(moveIndex++);
          convertMove(currentMove,activemotorid,steps);
          printMove();

          
          overlord.moveRelative(activemotorid, steps);
          isSolvingStarted=true;
        }
      }
      else{
        if (!overlord.isBusy(activemotorid)){
          //send to the btserial for visualization before moving on to the next move
          btSerial.print("move ");
          btSerial.print(activemotorid);
          btSerial.print(" ");
          btSerial.println(activedirection);
          
          //power down
          overlord.powerDownMotor(activemotorid);

          //if there as batt info to send , do it
          if (battInfoPending){
            btSerial.println(battInfo);
            battInfoPending=false;
          }

          if (moveIndex >= numberOfMoves)
          {
            state=STATE_SOLVED;
            btSerial.print("THE CUBE IS SOLVED IN ");
            btSerial.print(numberOfMoves);
            btSerial.println(" MOVES");
          }
          else{ //there are still moves left
            currentMove = pCube->getMoveAt(moveIndex++);
            convertMove(currentMove,activemotorid,steps);
            printMove();
            overlord.moveRelative(activemotorid, steps);
          }
        }
        else
        {
            overlord.update();
        }
     
      }
      break;
    case STATE_SOLVED:
      btSerial.println("Switching to IDLE");
      state=STATE_IDLE;
      break;
    default:
      break;
  }

  //if there is still batt info to be sent
  //send it now
  if (battInfoPending){
    btSerial.println(battInfo);
    battInfoPending=false;
  }


  if (!cmdHandled){
    //check for unhandled commands; like abort; or if it is an unknown command
    if (strcmp(parser.getCommand(),"ABORT")==0)
    {
      btSerial.println("ABORTING!!!");
      state = STATE_IDLE;
      cmdHandled=true;
    }
    else{
      btSerial.println("UNKNOWN COMMAND");
      cmdHandled=true;
    }
  }
}



bool checkCubeColors(){
  //should check the colors[] array for correctness (at least 9 colors of each type);
  int counters[6]={0};

  for (int i=0; i < COLOR_COUNT;i++){
    if (colors[i] > 5 )
      return false; //out of range
    
    counters[colors[i]] += 1;
  }  
  
  for (int i=0 ; i< 6; i++){
    if (counters[i] != 9)
      return false;
  }
  
  return true;
}

void serialEvent() {

}

//replace serial with serial1 later
void checkBTSerial() {
  while (btSerial.available()) {
    char c = btSerial.read();

    if (c == '\r') continue;  // Skip carriage return

    if (c == '\n') {
      btInputBuffer[btIndex] = '\0';
      btInputReady = true;
      btIndex = 0;
    } else if (btIndex < MAXBUFFSIZE - 1) {
      btInputBuffer[btIndex++] = c;
    } else {
      btSerial.println("BT input too long!");
      btIndex = 0;
    }
  }
}



float readBattery(){
  //read the BATYT_ADC pin and return the equivalent value in volts
  int value = analogRead(BATT_ADC); // 0–4095
  float vbatt = value * 0.003166;   // Empirical correction factor

  //btSerial.print("ADC Value:");
  //btSerial.println(value);
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

//used in scrambling
void decodeMove(int move, int& motor, int& steps) {
  int type = move % 3;
  motor = move / 3;
  steps = (type == 0) ? 512 : (type == 1 ? -512 : 1024);
}

  
//convert the ORIGINAL RUBIK MOVE to TCAMOVES
//see Global MOVES
void convertMove(int move, int& motor, int& steps) {
  if (move <12){
    int type = move % 2;
    motor = move / 2;
    steps = (type == 0) ? 512 : -512;

    if (steps > 0){
      activedirection = "cw";
    }
    else
      activedirection = "ccw";
  }
  else{
    motor=move-12;
    steps=1024;
    activedirection = "180";
  }
}

void printMove(){
  Serial.print("MotorID :");
  Serial.print(activemotorid);
  Serial.print(" Steps :");
  Serial.println(steps);
}

bool parseColors(const char* input) {

  // Check length
  if (strlen(input) != COLOR_COUNT) return false;

  for (int i = 0; i < COLOR_COUNT; i++) {
    char c = input[i];
    if (c < '0' || c > '5') return false; // validate range
    colors[i] = c - '0';
  }

  return true;
}
