
#ifndef RUBIKCUBE_H_
#define RUBIKCUBE_H_

#include "element.h"
#include "globals.h"
#include "cornerpiece.h"

namespace RubikBot{


    enum class Color
    {
        ORANGE=0,
        GREEN,
        RED,
        BLUE,
        WHITE,
        YELLOW
    };

    enum class EdgePiece
    {
        GO=0,
        OB,
        BRX,
        RG,
        //WO,
        //WB,
        //WR,
        //WG//,
        //YO,
        //YB,
        //YR,
        //YG
    };

    typedef struct F2LData
    {
        //F2l elements
        Element* corner1;
        Element* corner2;
        Element* corner3;
        Element* mid1;
        Element* mid2;
        Element* activemid;
        Element* oppositemid;
        Element* activecorner;
        Element* oppositecorner;


        bool f2lWGOsolved;
        bool f2lWOBsolved;
        bool f2lWBRsolved;
        bool f2lWRGsolved;

    }F2LData;

    class MoveArray{
    public:
        MoveArray();
        void clear(){movecount=0;}

        void appendMove(int move){moves[movecount++]=move;}
        int getMoveCount(){return movecount;}
        int getMoveAt(int id){return moves[id];}
        void setMoveAt(int id,int value){moves[id]=value;}
        virtual ~MoveArray(){};

        //functions to optimize the moves
        int optimizeMoves();
    protected:
        void eliminateTriples();
        void eliminateOpposites();
        void eliminateDoubles();
    private:
        int moves[200];
        int movecount;
    };

    class Orientation
    {
    public:
        int faceup;
        int faceleft;
    } ;


    class Rubik {
    public:
        Rubik();
        virtual ~Rubik();
        void initialize();					//this one, initializes the cube in a solved state
        void initialize(int color[54]);     //initialize with a color stream; in case were starting with an unsolved cube
                                            //this is used in solving and doing shuffling in the new version
        int getErroredColors();				//must be called to check if zero before calling solve
        bool areColorsOK();					//returns true if ok else false
        //this ORIENTATION is UL unless initialized with COLORS
        //in which case; the actual position of the WHITE ORANGE is stored in initialorientation
        CubeOrientation getInitialCubeOrientation(){return initialOrientation;}

        //Access functions
        bool getFace(int face, char* pArray3x3);      //gets the 3x3 array of the given face and puts it in the pArray

    protected:
        void revalueMids(Element* el1, Element* el2);
        void revalueCorners(Element* el1, Element* el2, Element* el3);
    public:
        bool shuffle(int count);	//shuffle the cube from initial state count number of times
                                    //the state of the cube should be tracked; max 200 count
        void solve();
        int optimizeMoves();
        void setElement(int x, int y, Element* element);
        Element* getElement(int x, int y);

        //kociemba methods
        void getColorsKociemba(char* color);	//gets the colors compatible with kociemba

        void f();
        void b();
        void u();
        void d();
        void l();
        void r();
        void fp();
        void bp();
        void up();
        void dp();
        void lp();
        void rp();
        void f2();
        void b2();
        void u2();
        void d2();
        void l2();
        void r2();

        void rotateLR_CW();
        void rotateLR_CCW();
        //void rotateUD_CW();
        //void rotateUD_CCW();
        void rotateFB_CW();
        void rotateFB_CCW();

        MoveArray* getMoveArray(){return &moves;}

        int getNumberOfMoves(){return moves.getMoveCount();}
        int getMoveAt(int index){return moves.getMoveAt(index);}//no range checking here

        void revert();              //does the moves in opposite to get to original state
    public:
        void solveWhiteCross();
        void solveWhiteGreen(); //this is index4,7 in the elements
        void solveWhiteRed();
        void solveWhiteOrange();
        void solveWhiteBlue();

        void solveWhiteCorners();
        void solveLeftDownCorner();
        void solveRightDownCorner();
        void solveLeftTopCorner();
        void solveRightTopCorner();

        void solveMiddleLayer();
        void solveEdgeGreenRed();
        void solveEdgeOrangeGreen();
        void solveEdgeRedBlue();
        void solveEdgeBlueOrange();

        Element *findElement(int value);
        Element *findCorner(CornerPiece cornerpiece,Color color);     //findCorner(WGO,Green);    //returns the green element of the corner piece
        Element *findEdge(EdgePiece edgepiece,Color color);
        Element *getElementAt(int index);

        void solveYellowCrossPattern(int face);
        void solveYellowCrossOrdering();
        void solveYellowCorner();
        void solveYellowFinal();

        void solveYellowLayer();


        //f2l methods
        void solveF2L(CornerPiece corner);  //solveF2L(WGO);

        void testSolution();

    protected:

    private:
        int getYellowCornerValue(int corner);

    protected:
        //movements with offset; to be used in patterns
        void d(int face);
        void dp(int face);
        void r(int face);
        void rp(int face);
        void l(int face);
        void lp(int face);
        void b(int face);
        void bp(int face);
        void u(int face);
        void up(int face);
        void f(int face);
        void fp(int face);

        void toRightPattern(int face);
        void toLeftPattern(int face);
        void yellowCrossPattern(int face);

        void doMove(int move);	//for shuffling


    public:
        //F2L methods (protected)
        bool isF2LCornerInserted(CornerPiece corner);
        void insertF2LCorner(CornerPiece corner);

        //f2l helper functions
        void updateF2LCornerStatus(CornerPiece corner);
        bool isOppositeDiagonalMadeUp(Element* corner1);
        bool isDirectlyUnderMadeUp(Element* corner1);

        void f2lInitStruct();
        void f2lInitialize(CornerPiece corner);
        void f2lLevel3();               //brings both pieces to level 3 if not yet already
        void f2lLevel3V2();
        void f2lInitActivePieces();
        bool f2lIsSameActiveColor();
        bool f2lIsSideBySide();
        bool f2lIsInsertable();
        void f2lUpdateSolvedCorner(CornerPiece corner);

        //f2l optimizations
        bool f2lScanPair(); //scans the DOWN face for already made up pairs and insert it
                            //returns true when it has detected a pair else return false;
                            //the return value will be used for looping until there are no more pairs
        void f2lInsertPair(CornerPiece corner);

        Face leftFace(Face refFace);
        Face rightFace(Face refFace);

        //oll algorithm
        void ollSolveCross();    //calls the next 3 methods after scanning the CROSS
        void ollLine(int face); //f(R U Rp Up) Fp
        void ollLShape(int face); //fdl, (R U Rp Up) fdlp
        void ollDot(int face);  //F(R, U, Rp, Up) Fp, fdl (R U Rp Up) fdlp

        //after the cross is made,make all the same color

        void ollSolveYellow();

        //rotate so that u see one corner in front but also one yellow facing you in the other corner
        //this is SUNE and anti sune
        void ollSune(int face);         // R U  Rp U R U2 Rp //right version
        void ollAntiSune(int face);     // Lp Up L Up LP U2 L //left version

        //no corners
        void ollH(int face);            //F (R U Rp Up)3 Fp
        void ollPi(int face);           //R U2, (R2, Up R2 Up R2) U2 R

        //2 corners madeup
        void ollDiagonalCorner(int face);       //rotate so that you can see yellow corners in front and right
                                                //X rotate so that WHITE is FACING YOU
                                                //(Rp U R Dp) (Rp UP R D)
        void ollT(int face);                    //two corners are correct but the other 2 are in the sides are opposite each other
                                                //put the made up corner to the right , then rotate so that down is facing fron
                                                //x (L U Rp Up) (Lp U R Up)
        void ollU(int face);            //2 are made up and besdide each other, but the other corners are to the sides but opposite facing
                                        //R2 D (Rp U2 R) Dp (Rp U2 Rp)

        //permutation of the last layer
        void pllSolveCorners();
        void pllDiagonalSwap();
        void pllAdjacentCornerSwap();

        void pllSolveMiddles();
        void pllEdgeCycle1();
        void pllEdgeCycle2();
        void pllOppositeEdgeSwap();
        void pllAdjacentEdgeSwap();



    public:
        void yellowCrossBRMatchPattern(int face);
        void yellowCornerPosPattern(int face);
        void yellowCornerPlacePattern(int face);        //to place the corner to its correct position


    private:
        Element* elements[6][9];
        MoveArray moves;
        Orientation orientation;
        //F2l elements
        F2LData f2lData;
        int colorError;
        CubeOrientation initialOrientation;
    };
}


#endif /* RUBIK_H_ */


