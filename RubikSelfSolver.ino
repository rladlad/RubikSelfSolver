#include "Cube.h"

using namespace RubikBot;

Rubik* pCube {nullptr};

//forward declarations
void printFace(int face);

void setup() {
  
  Serial.begin(115200);

  delay(1000);
  
  // put your setup code here, to run once:
  pCube = new Rubik();
  pCube->initialize();

  char face[3][3];

  printFace(1);
  printFace(5);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

//local functions
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
    char arr[3][3];
    char blanks[3][3]{' ',' ',' ',' ',' ',' ',' ',' ',' '};

    cube->getFace(0, &arr[0][0]);

    //print top
    
    
}
