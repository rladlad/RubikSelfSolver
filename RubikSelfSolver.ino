#include "Cube.h"

using namespace RubikBot;

Rubik* pCube {nullptr};
String inputString = "";      // String to hold incoming data
bool stringComplete = false; // Whether the string is complete

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


  inputString.reserve(50);

  
}

void loop() {

  if (stringComplete) {
    executeCommand(inputString);
    inputString = "";
    stringComplete = false;
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
  cmd.trim(); // Remove whitespace and newline

  if (cmd=="print"){
    clearSerialMonitor();
    printCube(pCube);
  }
  else if (cmd=="shuffle"){
    pCube->shuffle(50);
  }
  else if (cmd=="init"){
    pCube->initialize();
  }
  else if (cmd=="solve"){
    clearSerialMonitor();
    printCube(pCube);
    pCube->solve();
    printCube(pCube);
  }
  else if (cmd=="d"){	
	  pCube->d();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="dp"){	
    pCube->dp();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="l"){	
    pCube->l();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="lp"){	
    pCube->lp();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="f"){	
    pCube->f();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="fp"){	
    pCube->fp();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="r"){	
    pCube->r();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="rp"){	
    pCube->rp();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="b"){	
    pCube->b();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="bp"){	
    pCube->bp();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="u"){	
    pCube->u();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="up"){	
    pCube->up();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="d2"){	
    pCube->d2();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="l2"){	
    pCube->l2();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="f2"){	
    pCube->f2();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="r2"){	
    pCube->r2();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="b2"){	
    pCube->b2();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else if (cmd=="u2"){	
    pCube->u2();
    clearSerialMonitor();
    printCube(pCube);
  }	
  else{
    Serial.println("Unknown Command");
  }


}





//local functions

void clearSerialMonitor() {
  for (int i = 0; i < 50; i++) {
    Serial.println();
  }
}

void printFace(int face){
    char arr[3][3];

    if (pCube->getFace(face, &arr[0][0])==true){

        for (int i=0; i<3; i++){
          for (int j=0; j<3 ; j++){
              Serial.print(arr[i][j]);
              Serial.print(' ');
          }
          Serial.println();
        }
    }
}

void printCube(Rubik* cube){
    char arr[9][12];
    char temp[3][3];

    //initialize the array
    for (int i=0; i<9; i++){
      for (int j=0; j<12;j++)
      {
        arr[i][j]=' ';
      }
    }

    cube->getFace(0, &temp[0][0]);
    for  (int i=3;i<6;i++){
      for (int j=0;j<3;j++){
        arr[i][j]=temp[i-3][j];
      }
    }

    cube->getFace(1, &temp[0][0]);
    for  (int i=3;i<6;i++){
      for (int j=3;j<6;j++){
        arr[i][j]=temp[i-3][j-3];
      }
    }

    cube->getFace(2, &temp[0][0]);
    for  (int i=3;i<6;i++){
      for (int j=6;j<9;j++){
        arr[i][j]=temp[i-3][j-6];
      }
    }

    cube->getFace(3, &temp[0][0]);
    for  (int i=3;i<6;i++){
      for (int j=9;j<12;j++){
        arr[i][j]=temp[i-3][j-9];
      }
    }

    cube->getFace(4, &temp[0][0]);
    for  (int i=0;i<3;i++){
      for (int j=3;j<6;j++){
        arr[i][j]=temp[i][j-3];
      }
    }

    cube->getFace(5, &temp[0][0]);
    for  (int i=6;i<9;i++){
      for (int j=3;j<6;j++){
        arr[i][j]=temp[i-6][j-3];
      }
    }

    //print 
    for (int i=0; i<9; i++){
        for (int j=0; j<12 ; j++){
            Serial.print(arr[i][j]);
            Serial.print(' ');
        }
        Serial.println();
      }
    
}
