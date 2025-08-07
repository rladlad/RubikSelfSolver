/*
 * Rubik.cpp
 *
 *  Created on: Jan 15, 2020
 *      Author: STC-Design
 */

#include "Cube.h"
#include "element.h"
#include "globals.h"

#define MAXMOVES 18

namespace RubikBot{

    Rubik::Rubik() {
        // TODO Auto-generated constructor stub
        //this->elements=new Element[6*9];

        for (int i=0; i<6 ;i++)
        {
            for (int j=0; j<9; j++)
            {
                //(elements[i*9+j]).setIndex(i,j);
                //create a new element witout color
                elements[i][j]=new Element(i,j,-1);
            }
        }

        initialOrientation=CubeOrientation::UL;

    }

    Rubik::~Rubik() {
        // TODO Auto-generated destructor stub
    }


    void Rubik::initialize()
    {
        int color=0;
        for (int face=0; face<6; face++)
        {

            for (int index=0; index < 9 ; index++)
            {
                //get the current element at the face, index
                Element* el=this->elements[face][index];

                //set its index
                el->setIndex(face,index);
                //set its color
                el->setColor(color);

                //set its vaue
                el->setValue(face*10+index);
                el->recalcPos();

            }
            color++;
        }

        //orientation
        orientation.faceup=(int)Color::WHITE;
        orientation.faceleft=(int)Color::ORANGE;

        f2lInitStruct();

    }

    void Rubik::initialize(int color[])
    {

        //clear the moves
        moves.clear();
        //colorError=0;

        //the color stream must match the color ids of the program
        //it should also be read in sequence face 0 to face 5
        for (int face=0; face<6; face++)
        {
            for (int i=0; i<9; i++)
            {
                //Element* el=new Element(face,i,color[face*9+i]);
                Element* el=elements[face][i];

                el->setIndex(face,i);
                el->setColor(color[face*9+i]);

                //differ the setting of value below
            }
        }

        //revalue everything

        //revalue FACES
        for (int i=0; i<6 ;i++)
        {
            int c=this->getElement(i,4)->getColor();
            //getElement(i,4)->setValue(color*10+4);
            Element* el=getElement(i,4);
            el->setValue(c*10+4);
        }

        //revalue MIDS
        Element* el1=getElement(4,1);
        Element* el2=getElement(3,1);
        this->revalueMids(el1,el2);

        el1=getElement(4,3);
        el2=getElement(0,1);
        this->revalueMids(el1,el2);

        el1=getElement(4,7);
        el2=getElement(1,1);
        this->revalueMids(el1,el2);

        el1=getElement(4,5);
        el2=getElement(2,1);
        this->revalueMids(el1,el2);

        //yellow
        el1=getElement(5,3);
        el2=getElement(0,7);
        this->revalueMids(el1,el2);

        el1=getElement(5,1);
        el2=getElement(1,7);
        this->revalueMids(el1,el2);

        el1=getElement(5,5);
        el2=getElement(2,7);
        this->revalueMids(el1,el2);

        el1=getElement(5,7);
        el2=getElement(3,7);
        this->revalueMids(el1,el2);

        //middle middles
        el1=getElement(0,5);
        el2=getElement(1,3);
        this->revalueMids(el1,el2);

        el1=getElement(1,5);
        el2=getElement(2,3);
        this->revalueMids(el1,el2);

        el1=getElement(2,5);
        el2=getElement(3,3);
        this->revalueMids(el1,el2);

        el1=getElement(3,5);
        el2=getElement(0,3);
        this->revalueMids(el1,el2);

        //revalue CORNERS
        Element* el3;
        el1=getElement(4,6);
        el2=getElement(0,2);
        el3=getElement(1,0);
        revalueCorners(el1,el2,el3);

        el1=getElement(4,8);
        el2=getElement(1,2);
        el3=getElement(2,0);
        revalueCorners(el1,el2,el3);

        el1=getElement(4,2);
        el2=getElement(2,2);
        el3=getElement(3,0);
        revalueCorners(el1,el2,el3);

        el1=getElement(4,0);
        el2=getElement(0,0);
        el3=getElement(3,2);
        revalueCorners(el1,el2,el3);

        el1=getElement(5,0);
        el2=getElement(0,8);
        el3=getElement(1,6);
        revalueCorners(el1,el2,el3);

        el1=getElement(5,2);
        el2=getElement(1,8);
        el3=getElement(2,6);
        revalueCorners(el1,el2,el3);

        el1=getElement(5,8);
        el2=getElement(2,8);
        el3=getElement(3,6);
        revalueCorners(el1,el2,el3);

        el1=getElement(5,6);
        el2=getElement(0,6);
        el3=getElement(3,8);
        revalueCorners(el1,el2,el3);


        /*
        //do a check of the passed colors
        //such that we wont be solving anything that has wrong color parameters
        for (int face=0;face<6;face++)
        {
            for (int i=0; i<9;i++)
            {
                if (elements[face][i]->getValue()==-1)
                {
                    //initialization has failed
                    //tell the USER ABOUT this ERROR
                    //error=true;
                    colorError++;
                }
            }
        }
    */
        //do SETUP orientation move so that white is UP and ORANGE is LEFT
        Element* whiteEl=findElement(44);
        Element* orangeEl=findElement(4);

        //so store the original orientation here
        initialOrientation=(CubeOrientation)(whiteEl->getFace()*10+orangeEl->getFace());

        //update the orientation
        orientation.faceup=whiteEl->getFace();
        orientation.faceleft=orangeEl->getFace();


        f2lInitStruct();

    }

    bool Rubik::getFace(int face, char* pArray3x3){
        if (face>=6)
            return false;

        for (int r=0;r<3;r++){
            for (int c=0;c<3;c++){
                pArray3x3[r * 3 + c]=elements[face][r*3+c]->getColorChar();
            }
        }
        
        return true;   
    }


    void Rubik::revalueMids(Element* el1, Element* el2)
    {
        switch (el1->getColor())
        {

        case (int)Color::WHITE:
            if (el2->getColor()==(int)Color::ORANGE)
            {
                el2->setValue(1);
                el1->setValue(43);
            }
            else if (el2->getColor()==(int)Color::GREEN)
            {
                el2->setValue(11);
                el1->setValue(47);
            }
            else if (el2->getColor()==(int)Color::RED)
            {
                el2->setValue(21);
                el1->setValue(45);
            }
            else if (el2->getColor()==(int)Color::BLUE)
            {
                el2->setValue(31);
                el1->setValue(41);
            }
            break;
        case (int)Color::YELLOW:
            if (el2->getColor()==(int)Color::ORANGE)
            {
                el1->setValue(53);
                el2->setValue(7);
            }
            else if (el2->getColor()==(int)Color::GREEN)
            {
                el1->setValue(51);
                el2->setValue(17);

            }
            else if (el2->getColor()==(int)Color::RED)
            {
                el1->setValue(55);
                el2->setValue(27);
            }
            else if (el2->getColor()==(int)Color::BLUE)
            {
                el1->setValue(57);
                el2->setValue(37);
            }
            break;
        case (int)Color::ORANGE:
            if (el2->getColor()==(int)Color::WHITE)
            {
                el1->setValue(1);
                el2->setValue(43);
            }
            else if (el2->getColor()==(int)Color::GREEN)
            {
                el1->setValue(5);
                el2->setValue(13);

            }
            else if (el2->getColor()==(int)Color::YELLOW)
            {
                el1->setValue(7);
                el2->setValue(53);
            }
            else if (el2->getColor()==(int)Color::BLUE)
            {
                el1->setValue(3);
                el2->setValue(35);
            }
            break;
        case (int)Color::GREEN:
            if (el2->getColor()==(int)Color::WHITE)
            {
                el1->setValue(11);
                el2->setValue(47);
            }
            else if (el2->getColor()==(int)Color::RED)
            {
                el1->setValue(15);
                el2->setValue(23);

            }
            else if (el2->getColor()==(int)Color::YELLOW)
            {
                el1->setValue(17);
                el2->setValue(51);
            }
            else if (el2->getColor()==(int)Color::ORANGE)
            {
                el1->setValue(13);
                el2->setValue(5);
            }
            break;
        case (int)Color::RED:
            if (el2->getColor()==(int)Color::WHITE)
            {
                el1->setValue(21);
                el2->setValue(45);
            }
            else if (el2->getColor()==(int)Color::BLUE)
            {
                el1->setValue(25);
                el2->setValue(33);

            }
            else if (el2->getColor()==(int)Color::YELLOW)
            {
                el1->setValue(27);
                el2->setValue(55);
            }
            else if (el2->getColor()==(int)Color::GREEN)
            {
                el1->setValue(23);
                el2->setValue(15);
            }
            break;
        case (int)Color::BLUE:
            if (el2->getColor()==(int)Color::WHITE)
            {
                el1->setValue(31);
                el2->setValue(41);
            }
            else if (el2->getColor()==(int)Color::ORANGE)
            {
                el1->setValue(35);
                el2->setValue(3);

            }
            else if (el2->getColor()==(int)Color::YELLOW)
            {
                el1->setValue(37);
                el2->setValue(57);
            }
            else if (el2->getColor()==(int)Color::RED)
            {
                el1->setValue(33);
                el2->setValue(25);
            }
            break;
        }
    }
    void Rubik::revalueCorners(Element* el1, Element* el2, Element* el3)
    {
        //check if any of the element has color white or yellow
        Element* t1;
        Element* t2;
        Element* t3;

        if (el1->getColor()==(int)Color::WHITE || el2->getColor()==(int)Color::WHITE || el3->getColor()==(int)Color::WHITE)
        {
            //process white
            if (el1->getColor()==(int)Color::WHITE)
            {
                t1=el1;
                t2=el2;
                t3=el3;
            }
            else if (el2->getColor()==(int)Color::WHITE)
            {
                t1=el2;
                t2=el1;
                t3=el3;

            }
            else if (el3->getColor()==(int)Color::WHITE)
            {
                t1=el3;
                t2=el1;
                t3=el2;
            }

            //renumber
            if (t2->getColor()==(int)Color::ORANGE)
            {
                if (t3->getColor()==(int)Color::BLUE)
                {
                    t1->setValue(40);
                    t2->setValue(0);
                    t3->setValue(32);
                }
                else
                {
                    //this is GREEN
                    t1->setValue(46);
                    t2->setValue(2);
                    t3->setValue(10);

                }
            }
            else if (t2->getColor()==(int)Color::GREEN)
            {
                if (t3->getColor()==(int)Color::ORANGE)
                {
                    t1->setValue(46);
                    t2->setValue(10);
                    t3->setValue(2);
                }
                else
                {
                    //this is red
                    t1->setValue(48);
                    t2->setValue(12);
                    t3->setValue(20);
                }
            }
            else if (t2->getColor()==(int)Color::RED)
            {
                if (t3->getColor()==(int)Color::GREEN)
                {
                    t1->setValue(48);
                    t2->setValue(20);
                    t3->setValue(12);
                }
                else
                {
                    //this is blue
                    t1->setValue(42);
                    t2->setValue(22);
                    t3->setValue(30);

                }
            }
            else if (t2->getColor()==(int)Color::BLUE)
            {
                if (t3->getColor()==(int)Color::RED)
                {
                    t1->setValue(42);
                    t2->setValue(30);
                    t3->setValue(22);
                }
                else
                {
                    //THIS IS ORANGE
                    t1->setValue(40);
                    t2->setValue(32);
                    t3->setValue(0);
                }
            }

        }
        else
        {
            //process yellow
            if (el1->getColor()==(int)Color::YELLOW)
            {
                t1=el1;
                t2=el2;
                t3=el3;
            }
            else if (el2->getColor()==(int)Color::YELLOW)
            {
                t1=el2;
                t2=el1;
                t3=el3;

            }
            else if (el3->getColor()==(int)Color::YELLOW)
            {
                t1=el3;
                t2=el1;
                t3=el2;
            }

            //renumber
            if (t2->getColor()==(int)Color::ORANGE)
            {
                if (t3->getColor()==(int)Color::BLUE)
                {
                    t1->setValue(56);
                    t2->setValue(6);
                    t3->setValue(38);
                }
                else
                {
                    //this is GREEN
                    t1->setValue(50);
                    t2->setValue(8);
                    t3->setValue(16);

                }
            }
            else if (t2->getColor()==(int)Color::GREEN)
            {
                if (t3->getColor()==(int)Color::ORANGE)
                {
                    t1->setValue(50);
                    t2->setValue(16);
                    t3->setValue(8);
                }
                else
                {
                    //this is red
                    t1->setValue(52);
                    t2->setValue(18);
                    t3->setValue(26);
                }
            }
            else if (t2->getColor()==(int)Color::RED)
            {
                if (t3->getColor()==(int)Color::GREEN)
                {
                    t1->setValue(52);
                    t2->setValue(26);
                    t3->setValue(18);
                }
                else
                {
                    //this is blue
                    t1->setValue(58);
                    t2->setValue(28);
                    t3->setValue(36);

                }
            }
            else if (t2->getColor()==(int)Color::BLUE)
            {
                if (t3->getColor()==(int)Color::RED)
                {
                    t1->setValue(58);
                    t2->setValue(36);
                    t3->setValue(28);
                }
                else
                {
                    //THIS IS ORANGE
                    t1->setValue(56);
                    t2->setValue(38);
                    t3->setValue(6);
                }
            }
        }

    }

    void Rubik::testSolution()
    {
        //this is for testing the ROBOT arm
        //we will just fill it with random moves

        //we will fill the moves with temp values on
        moves.clear();

        moves.appendMove((int)Moves::F);
        moves.appendMove((int)Moves::U);
        moves.appendMove((int)Moves::L);
        moves.appendMove((int)Moves::R);
        moves.appendMove((int)Moves::B);
        moves.appendMove((int)Moves::D);
        moves.appendMove((int)Moves::U2);
        moves.appendMove((int)Moves::D2);
        moves.appendMove((int)Moves::B2);
        moves.appendMove((int)Moves::F2);
        moves.appendMove((int)Moves::R2);

    }


    bool Rubik::shuffle(int count)
    {
        //the BOT must have the steppers enabled before calling this function
        //otherwise, it wouldt be scrambled; it must also have a valid cube STATE
        //preferably solved

        if (count>200)
            count=200;

        if (count<0)
            return false;

        // //init the move count to zero
        // moves.clear();

        // //init the random number generator
        // RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,ENABLE);
        // RNG_Cmd(ENABLE);


        // for (int i=0;i<count;i++)
        // {
        // 	//get a random move from 18 different moves
        // 	while (RNG_GetFlagStatus(RNG_FLAG_DRDY)!=SET);

        // 	bool error=false;
        // 	//check that there are no errors in the RNG
        // 	if (RNG_GetFlagStatus(RNG_FLAG_SECS)==SET)
        // 	{
        // 		//reset the RNG
        // 		RNG_ClearFlag(RNG_FLAG_SECS);

        // 		RNG_Cmd(DISABLE);
        // 		RNG_Cmd(ENABLE);
        // 		error=true;
        // 	}

        // 	if (RNG_GetFlagStatus(RNG_FLAG_CECS)==SET)
        // 	{
        // 		//clear the clock signal
        // 		RNG_ClearFlag(RNG_FLAG_CECS);

        // 		//i dont know what else to do here
        // 		error=true;
        // 	}

        // 	if (!error)
        // 	{
        // 		int m=RNG_GetRandomNumber() % MAXMOVES;

        // 		//then execute the move (in software)
        // 		doMove(m);
        // 	}


        // }

        // //turn off the RNg
        // RNG_Cmd(DISABLE);
        // RNG_DeInit();
        // RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,DISABLE);

        //at this point, the state is scrambled and we have the STATE of the cube
        //to be sent to RASPI if needed
        //we MUST retrieve the SHUFFLING MOVES and then EXECUTE it physicall by the BOT

        return true;

    }
    void Rubik::solve()
    {
        //if (colorError)
        //	return;

        moves.clear();	//this line is commented out in the original

        f2lInitStruct();


        solveWhiteCross();
        while (f2lScanPair());
        solveF2L(CornerPiece::WGO);
        while (f2lScanPair());
        solveF2L(CornerPiece::WOB);
        while (f2lScanPair());
        solveF2L(CornerPiece::WBR);
        while (f2lScanPair());
        solveF2L(CornerPiece::WRG);

        solveYellowLayer();

    }

    int Rubik::optimizeMoves()
    {
        return moves.optimizeMoves();
    }

    void Rubik::setElement(int x, int y, Element *element)
    {
        this->elements[x][y]=element;
        //copy the passed elements value to the destination elemnet
        //Element* el=getElement(x,y);
        //el->setColor(element->getColor());
        //el->setValue(element->getValue());
        //el->setMarking(element->getMarking());

        //reset the index of the element
        element->setIndex(x,y);
    }

    Element* Rubik::getElement(int x, int y)
    {
        return elements[x][y];
    }

    void Rubik::getColorsKociemba(char* color)
    {
        //URFDLB

        int faces[]={4,2,1,5,0,3};

        int index=0;

        for (int i=0;i<6;i++)
        {
            for (int j=0;j<9;j++)
            {
                Element* el = getElement(faces[i],j);

                char c;

                switch (el->getColor())
                {
                case 0:
                    c='L';
                    break;
                case 1:
                    c='F';
                    break;
                case 2:
                    c='R';
                    break;
                case 3:
                    c='B';
                    break;
                case 4:
                    c='U';
                    break;
                case 5:
                    c='D';
                    break;

                }

                color[index++]=c;
            }
        }
    }

    void Rubik::f()
    {
        //copy the front elements
        Element* temp[9];
        int face=1;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);


        //store the three top colors
        temp[0]=getElement(4,6);
        temp[1]=getElement(4,7);
        temp[2]=getElement(4,8);

        setElement(4,6,getElement(0,8));
        setElement(4,7,getElement(0,5));
        setElement(4,8,getElement(0,2));
        setElement(0,2,getElement(5,0));
        setElement(0,5,getElement(5,1));
        setElement(0,8,getElement(5,2));
        setElement(5,0,getElement(2,6));
        setElement(5,1,getElement(2,3));
        setElement(5,2,getElement(2,0));
        setElement(2,0,temp[0]);
        setElement(2,3,temp[1]);
        setElement(2,6,temp[2]);

        moves.appendMove((int)Moves::F);

    }

    void Rubik::b()
    {
        //copy the front elements
        Element* temp[9];
        int face=3;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);

        //store the three top colors
        temp[0]=getElement(4,2);
        temp[1]=getElement(4,1);
        temp[2]=getElement(4,0);

        setElement(4,0,getElement(2,2));
        setElement(4,1,getElement(2,5));
        setElement(4,2,getElement(2,8));
        setElement(2,2,getElement(5,8));
        setElement(2,5,getElement(5,7));
        setElement(2,8,getElement(5,6));
        setElement(5,8,getElement(0,6));
        setElement(5,7,getElement(0,3));
        setElement(5,6,getElement(0,0));
        setElement(0,0,temp[0]);
        setElement(0,3,temp[1]);
        setElement(0,6,temp[2]);

        moves.appendMove((int)Moves::B);
    }

    void Rubik::u()
    {
        //copy the front elements
        Element* temp[9];
        int face=4;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);

        //store the three top colors
        temp[0]=getElement(3,2);
        temp[1]=getElement(3,1);
        temp[2]=getElement(3,0);

        setElement(3,0,getElement(0,0));
        setElement(3,1,getElement(0,1));
        setElement(3,2,getElement(0,2));
        setElement(0,0,getElement(1,0));
        setElement(0,1,getElement(1,1));
        setElement(0,2,getElement(1,2));
        setElement(1,0,getElement(2,0));
        setElement(1,1,getElement(2,1));
        setElement(1,2,getElement(2,2));
        setElement(2,0,temp[2]);
        setElement(2,1,temp[1]);
        setElement(2,2,temp[0]);

        moves.appendMove((int)Moves::U);
    }

    void Rubik::d()
    {
        //copy the front elements
        Element* temp[9];
        int face=5;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);


        //store the three top colors
        temp[0]=getElement(1,6);
        temp[1]=getElement(1,7);
        temp[2]=getElement(1,8);

        setElement(1,6,getElement(0,6));
        setElement(1,7,getElement(0,7));
        setElement(1,8,getElement(0,8));
        setElement(0,6,getElement(3,6));
        setElement(0,7,getElement(3,7));
        setElement(0,8,getElement(3,8));
        setElement(3,6,getElement(2,6));
        setElement(3,7,getElement(2,7));
        setElement(3,8,getElement(2,8));
        setElement(2,6,temp[0]);
        setElement(2,7,temp[1]);
        setElement(2,8,temp[2]);

        moves.appendMove((int)Moves::D);
    }

    void Rubik::dp()
    {
        //copy the front elements
        Element* temp[9];
        int face=5;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //store the three top colors
        temp[0]=getElement(1,6);
        temp[1]=getElement(1,7);
        temp[2]=getElement(1,8);

        setElement(1,6,getElement(2,6));
        setElement(1,7,getElement(2,7));
        setElement(1,8,getElement(2,8));
        setElement(2,6,getElement(3,6));
        setElement(2,7,getElement(3,7));
        setElement(2,8,getElement(3,8));
        setElement(3,6,getElement(0,6));
        setElement(3,7,getElement(0,7));
        setElement(3,8,getElement(0,8));
        setElement(0,6,temp[0]);
        setElement(0,7,temp[1]);
        setElement(0,8,temp[2]);

        moves.appendMove((int)Moves::Dp);
    }

    void Rubik::up()
    {
        //copy the front elements
        Element* temp[9];
        int face=4;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //store the three top colors
        temp[0]=getElement(3,2);
        temp[1]=getElement(3,1);
        temp[2]=getElement(3,0);

        setElement(3,0,getElement(2,0));
        setElement(3,1,getElement(2,1));
        setElement(3,2,getElement(2,2));
        setElement(2,0,getElement(1,0));
        setElement(2,1,getElement(1,1));
        setElement(2,2,getElement(1,2));
        setElement(1,0,getElement(0,0));
        setElement(1,1,getElement(0,1));
        setElement(1,2,getElement(0,2));
        setElement(0,0,temp[2]);
        setElement(0,1,temp[1]);
        setElement(0,2,temp[0]);

        moves.appendMove((int)Moves::Up);
    }

    void Rubik::bp()
    {
        //copy the front elements
        Element* temp[9];
        int face=3;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //store the three top colors
        temp[0]=getElement(4,2);
        temp[1]=getElement(4,1);
        temp[2]=getElement(4,0);

        setElement(4,0,getElement(0,6));
        setElement(4,1,getElement(0,3));
        setElement(4,2,getElement(0,0));
        setElement(0,0,getElement(5,6));
        setElement(0,3,getElement(5,7));
        setElement(0,6,getElement(5,8));
        setElement(5,6,getElement(2,8));
        setElement(5,7,getElement(2,5));
        setElement(5,8,getElement(2,2));
        setElement(2,2,temp[2]);
        setElement(2,5,temp[1]);
        setElement(2,8,temp[0]);

        moves.appendMove((int)Moves::Bp);

    }

    void Rubik::l()
    {
        //copy the front elements
        Element* temp[9];
        int face=0;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);


        //store the three top colors
        temp[0]=getElement(4,0);
        temp[1]=getElement(4,3);
        temp[2]=getElement(4,6);

        setElement(4,0,getElement(3,8));
        setElement(4,3,getElement(3,5));
        setElement(4,6,getElement(3,2));
        setElement(3,2,getElement(5,6));
        setElement(3,5,getElement(5,3));
        setElement(3,8,getElement(5,0));
        setElement(5,0,getElement(1,0));
        setElement(5,3,getElement(1,3));
        setElement(5,6,getElement(1,6));
        setElement(1,0,temp[0]);
        setElement(1,3,temp[1]);
        setElement(1,6,temp[2]);

        moves.appendMove((int)Moves::L);

    }

    void Rubik::r()
    {
        //copy the front elements
        Element* temp[9];
        int face=2;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);

        //store the three top colors
        temp[0]=getElement(4,2);
        temp[1]=getElement(4,5);
        temp[2]=getElement(4,8);

        setElement(4,2,getElement(1,2));
        setElement(4,5,getElement(1,5));
        setElement(4,8,getElement(1,8));
        setElement(1,2,getElement(5,2));
        setElement(1,5,getElement(5,5));
        setElement(1,8,getElement(5,8));
        setElement(5,2,getElement(3,6));
        setElement(5,5,getElement(3,3));
        setElement(5,8,getElement(3,0));
        setElement(3,0,temp[2]);
        setElement(3,3,temp[1]);
        setElement(3,6,temp[0]);

        moves.appendMove((int)Moves::R);

    }
    /*
    void Rubik::rotateFB_CW()
    {
        //FRONT HANDED ROTATION
        //copy the LEFT elements
        Element* temp[9];
        int face=0;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        //left gets down
        setElement(face,0,getElement(5,6));
        setElement(face,1,getElement(5,3));
        setElement(face,2,getElement(5,0));
        setElement(face,3,getElement(5,7));
        setElement(face,4,getElement(5,4));
        setElement(face,5,getElement(5,1));
        setElement(face,6,getElement(5,8));
        setElement(face,7,getElement(5,5));
        setElement(face,8,getElement(5,2));

        //down gets right
        face=5;

        setElement(face,0,getElement(2,6));
        setElement(face,1,getElement(2,3));
        setElement(face,2,getElement(2,0));
        setElement(face,3,getElement(2,7));
        setElement(face,4,getElement(2,4));
        setElement(face,5,getElement(2,1));
        setElement(face,6,getElement(2,8));
        setElement(face,7,getElement(2,5));
        setElement(face,8,getElement(2,2));

        //right gets top
        face=2;

        setElement(face,0,getElement(4,6));
        setElement(face,1,getElement(4,3));
        setElement(face,2,getElement(4,0));
        setElement(face,3,getElement(4,7));
        setElement(face,4,getElement(4,4));
        setElement(face,5,getElement(4,1));
        setElement(face,6,getElement(4,8));
        setElement(face,7,getElement(4,5));
        setElement(face,8,getElement(4,2));

        //top gets left
        face=4;

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);

        //FRONT ROTATION
        face=1;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);

        //CCW in this case
        face=3;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        moves.appendMove(OrientY_CW);

        //UPDATE THE ORIENTATION
        orientation.faceup=findElement(44)->getFace();
        orientation.faceleft=findElement(4)->getFace();
    }
    void Rubik::rotateFB_CCW()
    {
        //FRONT HANDED ROTATION
        //copy the DOWN elements
        Element* temp[9];
        int face=5;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        //down gets left
        setElement(face,0,getElement(0,2));
        setElement(face,1,getElement(0,5));
        setElement(face,2,getElement(0,8));
        setElement(face,3,getElement(0,1));
        setElement(face,4,getElement(0,4));
        setElement(face,5,getElement(0,7));
        setElement(face,6,getElement(0,0));
        setElement(face,7,getElement(0,3));
        setElement(face,8,getElement(0,6));

        //left gets top
        face=0;

        setElement(face,0,getElement(4,2));
        setElement(face,1,getElement(4,5));
        setElement(face,2,getElement(4,8));
        setElement(face,3,getElement(4,1));
        setElement(face,4,getElement(4,4));
        setElement(face,5,getElement(4,7));
        setElement(face,6,getElement(4,0));
        setElement(face,7,getElement(4,3));
        setElement(face,8,getElement(4,6));

        //top gets right
        face=4;

        setElement(face,0,getElement(2,2));
        setElement(face,1,getElement(2,5));
        setElement(face,2,getElement(2,8));
        setElement(face,3,getElement(2,1));
        setElement(face,4,getElement(2,4));
        setElement(face,5,getElement(2,7));
        setElement(face,6,getElement(2,0));
        setElement(face,7,getElement(2,3));
        setElement(face,8,getElement(2,6));

        //right gets down
        face=2;

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //FRONT ROTATION CCW
        face=1;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //CW in this case
        face=3;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);

        moves.appendMove(OrientY_CCW);

        //UPDATE THE ORIENTATION
        orientation.faceup=findElement(44)->getFace();
        orientation.faceleft=findElement(4)->getFace();
    }
    */

    void Rubik::rp()
    {
        //copy the front elements
        Element* temp[9];
        int face=2;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //store the three top colors
        temp[0]=getElement(4,2);
        temp[1]=getElement(4,5);
        temp[2]=getElement(4,8);

        setElement(4,2,getElement(3,6));
        setElement(4,5,getElement(3,3));
        setElement(4,8,getElement(3,0));
        setElement(3,0,getElement(5,8));
        setElement(3,3,getElement(5,5));
        setElement(3,6,getElement(5,2));
        setElement(5,2,getElement(1,2));
        setElement(5,5,getElement(1,5));
        setElement(5,8,getElement(1,8));
        setElement(1,2,temp[0]);
        setElement(1,5,temp[1]);
        setElement(1,8,temp[2]);

        moves.appendMove((int)Moves::Rp);
    }

    void Rubik::f2()
    {
        f();
        f();
    }

    void Rubik::b2()
    {
        b();
        b();
    }

    void Rubik::u2()
    {
        u();
        u();
    }

    void Rubik::d2()
    {
        d();
        d();

    }

    void Rubik::l2()
    {
        l();
        l();
    }

    void Rubik::r2()
    {
        r();
        r();

    }
    /*
    void Rubik::rotateLR_CW()
    {
        //RIGHT HANDED ROTATION
        //copy the TOP elements
        Element* temp[9];
        int face=4;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        //front to UP
        setElement(face,0,getElement(1,0));
        setElement(face,1,getElement(1,1));
        setElement(face,2,getElement(1,2));
        setElement(face,3,getElement(1,3));
        setElement(face,4,getElement(1,4));
        setElement(face,5,getElement(1,5));
        setElement(face,6,getElement(1,6));
        setElement(face,7,getElement(1,7));
        setElement(face,8,getElement(1,8));

        //down to front
        face=1;

        setElement(face,0,getElement(5,0));
        setElement(face,1,getElement(5,1));
        setElement(face,2,getElement(5,2));
        setElement(face,3,getElement(5,3));
        setElement(face,4,getElement(5,4));
        setElement(face,5,getElement(5,5));
        setElement(face,6,getElement(5,6));
        setElement(face,7,getElement(5,7));
        setElement(face,8,getElement(5,8));

        //back to down
        face=5;

        setElement(face,0,getElement(3,8));
        setElement(face,1,getElement(3,7));
        setElement(face,2,getElement(3,6));
        setElement(face,3,getElement(3,5));
        setElement(face,4,getElement(3,4));
        setElement(face,5,getElement(3,3));
        setElement(face,6,getElement(3,2));
        setElement(face,7,getElement(3,1));
        setElement(face,8,getElement(3,0));

        //top to back
        face=3;

        setElement(face,0,temp[8]);
        setElement(face,1,temp[7]);
        setElement(face,2,temp[6]);
        setElement(face,3,temp[5]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[3]);
        setElement(face,6,temp[2]);
        setElement(face,7,temp[1]);
        setElement(face,8,temp[0]);

        //copy the right elements
        face=2;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);

        face=0;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        moves.appendMove(OrientX_CW);

        //UPDATE THE ORIENTATION
        orientation.faceup=findElement(44)->getFace();
        orientation.faceleft=findElement(4)->getFace();
    }

    void Rubik::rotateLR_CCW()
    {
        //RIGHT HANDED ROTATION

        //copy the top elements
        Element* temp[9];
        int face=4;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }


        //back to top
        setElement(face,0,getElement(3,8));
        setElement(face,1,getElement(3,7));
        setElement(face,2,getElement(3,6));
        setElement(face,3,getElement(3,5));
        setElement(face,4,getElement(3,4));
        setElement(face,5,getElement(3,3));
        setElement(face,6,getElement(3,2));
        setElement(face,7,getElement(3,1));
        setElement(face,8,getElement(3,0));


        //down to back
        face=3;

        setElement(face,0,getElement(5,8));
        setElement(face,1,getElement(5,7));
        setElement(face,2,getElement(5,6));
        setElement(face,3,getElement(5,5));
        setElement(face,4,getElement(5,4));
        setElement(face,5,getElement(5,3));
        setElement(face,6,getElement(5,2));
        setElement(face,7,getElement(5,1));
        setElement(face,8,getElement(5,0));

        //front to down
        face=5;

        setElement(face,0,getElement(1,0));
        setElement(face,1,getElement(1,1));
        setElement(face,2,getElement(1,2));
        setElement(face,3,getElement(1,3));
        setElement(face,4,getElement(1,4));
        setElement(face,5,getElement(1,5));
        setElement(face,6,getElement(1,6));
        setElement(face,7,getElement(1,7));
        setElement(face,8,getElement(1,8));


        //up to FRONT
        face=1;

        setElement(face,0,temp[0]);
        setElement(face,1,temp[1]);
        setElement(face,2,temp[2]);
        setElement(face,3,temp[3]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[5]);
        setElement(face,6,temp[6]);
        setElement(face,7,temp[7]);
        setElement(face,8,temp[8]);

        //rotate the right elements
        //copy the right elements
        face=2;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //rotate the LEFT elements
        face=0;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);

        moves.appendMove(OrientX_CCW);


        //UPDATE THE ORIENTATION
        orientation.faceup=findElement(44)->getFace();
        orientation.faceleft=findElement(4)->getFace();
    }
    */
    /*
    void Rubik::rotateUD_CCW()
    {
        //copy the front elements
        Element* temp[9];
        int face=3;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,getElement(2,0));
        setElement(face,1,getElement(2,1));
        setElement(face,2,getElement(2,2));
        setElement(face,3,getElement(2,3));
        setElement(face,4,getElement(2,4));
        setElement(face,5,getElement(2,5));
        setElement(face,6,getElement(2,6));
        setElement(face,7,getElement(2,7));
        setElement(face,8,getElement(2,8));

        face=2;

        setElement(face,0,getElement(1,0));
        setElement(face,1,getElement(1,1));
        setElement(face,2,getElement(1,2));
        setElement(face,3,getElement(1,3));
        setElement(face,4,getElement(1,4));
        setElement(face,5,getElement(1,5));
        setElement(face,6,getElement(1,6));
        setElement(face,7,getElement(1,7));
        setElement(face,8,getElement(1,8));

        face=1;

        setElement(face,0,getElement(0,0));
        setElement(face,1,getElement(0,1));
        setElement(face,2,getElement(0,2));
        setElement(face,3,getElement(0,3));
        setElement(face,4,getElement(0,4));
        setElement(face,5,getElement(0,5));
        setElement(face,6,getElement(0,6));
        setElement(face,7,getElement(0,7));
        setElement(face,8,getElement(0,8));

        face=0;

        setElement(face,0,temp[0]);
        setElement(face,1,temp[1]);
        setElement(face,2,temp[2]);
        setElement(face,3,temp[3]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[5]);
        setElement(face,6,temp[6]);
        setElement(face,7,temp[7]);
        setElement(face,8,temp[8]);

        //copy the top elements
        face=4;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        face=5;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);
    }

    */

    /*
    void Rubik::rotateUD_CW()
    {

        //copy the front elements
        Element* temp[9];
        int face=0;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,getElement(1,0));
        setElement(face,1,getElement(1,1));
        setElement(face,2,getElement(1,2));
        setElement(face,3,getElement(1,3));
        setElement(face,4,getElement(1,4));
        setElement(face,5,getElement(1,5));
        setElement(face,6,getElement(1,6));
        setElement(face,7,getElement(1,7));
        setElement(face,8,getElement(1,8));

        face=1;

        setElement(face,0,getElement(2,0));
        setElement(face,1,getElement(2,1));
        setElement(face,2,getElement(2,2));
        setElement(face,3,getElement(2,3));
        setElement(face,4,getElement(2,4));
        setElement(face,5,getElement(2,5));
        setElement(face,6,getElement(2,6));
        setElement(face,7,getElement(2,7));
        setElement(face,8,getElement(2,8));

        face=2;

        setElement(face,0,getElement(3,0));
        setElement(face,1,getElement(3,1));
        setElement(face,2,getElement(3,2));
        setElement(face,3,getElement(3,3));
        setElement(face,4,getElement(3,4));
        setElement(face,5,getElement(3,5));
        setElement(face,6,getElement(3,6));
        setElement(face,7,getElement(3,7));
        setElement(face,8,getElement(3,8));

        face=3;

        setElement(face,0,temp[0]);
        setElement(face,1,temp[1]);
        setElement(face,2,temp[2]);
        setElement(face,3,temp[3]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[5]);
        setElement(face,6,temp[6]);
        setElement(face,7,temp[7]);
        setElement(face,8,temp[8]);

        //copy the top elements
        face=4;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[6]);
        setElement(face,1,temp[3]);
        setElement(face,2,temp[0]);
        setElement(face,3,temp[7]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[1]);
        setElement(face,6,temp[8]);
        setElement(face,7,temp[5]);
        setElement(face,8,temp[2]);


        face=5;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

    }
    */

    /*
    QString Rubik::getMoveString()
    {
    QString str="";
    for (int i=0; i<moves.count(); i++)
    {
        switch (moves.at(i)){
        case 0:
            str+="L";
            break;
        case 1:
            str+="Lp";
            break;
        case 2:
            str+="F";
            break;
        case 3:
            str+="Fp";
            break;
        case 4:
            str+="R";
            break;
        case 5:
            str+="Rp";
            break;
        case 6:
            str+="B";
            break;
        case 7:
            str+="Bp";
            break;
        case 8:
            str+="U";
            break;
        case 9:
            str+="Up";
            break;
        case 10:
            str+="D";
            break;
        case 11:
            str+="Dp";
            break;
        case 12:
            str+="L2";
            break;
        case 13:
            str+="F2";
            break;
        case 14:
            str+="R2";
            break;
        case 15:
            str+="B2";
            break;
        case 16:
            str+="U2";
            break;
        case 17:
            str+="D2";
            break;
        }
        str+=",";
    }

    return str;
    }

    */
    /*
    void Rubik::copyMoves(QVector<int>& moves)
    {
        for (int i=0; i< this->moves.count(); i++)
        {
            moves.append(this->moves.at(i));
        }
    }*/

    void Rubik::revert()
    {
        /*
        QVector<int> oppositemoves;
        for (int i=moves.count()-1; i>=0; --i)
        {
            int move=moves.at(i);

            int opposite;
            if (move<12)
            {
                if (move % 2)
                {
                    //this is an ODD move
                    opposite = move - 1;    //integer division
                }
                else
                {
                    opposite = move + 1;    //the next move
                }
            }
            else
                opposite=move;

            oppositemoves.append(opposite);
        }

        for (int i=0; i< oppositemoves.count(); i++)
        {
            int opposite = oppositemoves.at(i);
            //apply the reverse move
            switch (opposite){
            case 0:
                l();
                break;
            case 1:
                lp();
                break;
            case 2:
                f();
                break;
            case 3:
                fp();
                break;
            case 4:
                r();
                break;
            case 5:
                rp();
                break;
            case 6:
                b();
                break;
            case 7:
                bp();
                break;
            case 8:
                u();
                break;
            case 9:
                up();
                break;
            case 10:
                d();
                break;
            case 11:
                dp();
                break;
            case 12:
                l2();
                break;
            case 13:
                f2();
                break;
            case 14:
                r2();
                break;
            case 15:
                b2();
                break;
            case 16:
                u2();
                break;
            case 17:
                d2();
                break;


            }
        }

        moves.clear();
    */
    }

    void Rubik::solveWhiteCross()
    {
        solveWhiteGreen();
        solveWhiteOrange();
        solveWhiteRed();
        solveWhiteBlue(); //there are instances that this isnt solve???
    }

    void Rubik::solveWhiteGreen()
    {
        //get the current location face of the WHITE 7 or 47
        Face face;

        Element* wg=findElement(47);

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());


        switch (face) {
        case Face::UP:
            if (wg->getY()==1)
            {
                this->up();
                this->up();
            }
            else if (wg->getY()==3)
            {
                this->up();
            }
            else if (wg->getY()==5)
            {
                this->u();
            }
            break;
        case Face::LEFT:
            if (wg->getY()==1)
            {
                this->l();
            }
            else if (wg->getY()==3)
            {
                this->l();
                this->l();
            }
            else if (wg->getY()==7)
            {
                this->lp();
            }
            this->f();

            break;
        case Face::RIGHT:
            if (wg->getY()==1)
            {
                this->rp();
            }
            else if (wg->getY()==5)
            {
                this->rp();
                this->rp();
            }
            else if (wg->getY()==7)
            {
                this->r();
            }
            this->fp();

            break;
        case Face::DOWN:
            if (wg->getY()==3)
            {
                this->d();

            }
            else if (wg->getY()==5)
            {
                this->dp();

            }
            else if (wg->getY()==7)
            {
                this->dp();
                this->dp();
            }
            this->fp();
            this->fp();

            break;
        case Face::BACK:
            if (wg->getY()==1)
            {
                b();
                l();
                up();
            }
            else if (wg->getY()==5)
            {
                l();
                up();
            }
            else if (wg->getY()==3)
            {
                rp();
                u();
            }
            else
            {
                b();
                rp();
                u();
            }

            break;
        case Face::FRONT:
            if (wg->getY()==1)
            {
                fp();
                lp();
                up();
            }
            else if (wg->getY()==3)
            {
                lp();
                up();

            }
            else if (wg->getY()==5)
            {
                r();
                u();
            }
            else if (wg->getY()==7)
            {
                fp();
                r();
                u();
            }
            break;
        }
    }

    void Rubik::solveWhiteRed()
    {
        Face face;

        Element* wg=findElement(45);

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());


        switch (face) {
        case Face::UP:
            if (wg->getY()==1)
            {
                //only one possibility where its out of place
                bp();
                up();
                b();
                u();

            }
            break;
        case Face::DOWN:
            if (wg->getY()==1)
            {
                d();
                r();
                r();
            }
            else if(wg->getY()==3)
            {
                d();
                d();
                r();
                r();
            }
            else if (wg->getY()==7)
            {
                dp();
                r();
                r();

            }
            else if (wg->getY()==5)
            {
                r();
                r();

            }

            break;
        case Face::LEFT:

            if(wg->getY()==3)
            {
                up();
                bp();
                u();

            }
            else if (wg->getY()==5)
            {
                u();
                f();
                up();

            }
            else if (wg->getY()==7)
            {
                up();
                l();
                bp();
                u();
            }
            break;
        case Face::RIGHT:
            if(wg->getY()==1)
            {
                r();
                up();
                b();
                u();

            }
            else if (wg->getY()==7)
            {
                rp();
                up();
                b();
                u();
            }
            else if (wg->getY()==3)
            {
                u();
                fp();
                up();
            }
            else if (wg->getY()==5)
            {
                up();
                b();
                u();
            }

            break;
        case Face::FRONT:
            if(wg->getY()==5)
            {
                r();

            }
            else if (wg->getY()==7)
            {
                fp();
                r();
                f();
            }
            else if (wg->getY()==3)
            {
                u();
                u();
                lp();
                up();
                up();



            }
            break;
        case Face::BACK:
            if(wg->getY()==1)
            {
                bp();
                rp();

            }
            else if (wg->getY()==7)
            {
                b();
                rp();
            }
            else if (wg->getY()==3)
            {
                rp();

            }
            else if (wg->getY()==5)
            {
                b();
                b();
                rp();
            }
            break;
        }
    }

    void Rubik::solveWhiteOrange()
    {
        Face face;

        Element* wg=findElement(43);

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());


        switch (face) {
        case Face::UP:
            if (wg->getY()==1)
            {
                f();
                up();
                fp();
            }
            else if (wg->getY()==5)
            {
                f();
                u();
                u();
                fp();
            }
            break;
        case Face::DOWN:
            if (wg->getY()==1)
            {
                dp();
            }
            else if (wg->getY()==5)
            {
                dp();
                dp();
            }
            else if (wg->getY()==7)
            {
                d();
            }
            lp();
            lp();

            break;
        case Face::LEFT:
            if (wg->getY()==1)
            {
                l();
            }
            else if (wg->getY()==3)
            {
                l();
                l();
            }
            else if (wg->getY()==7)
            {
                lp();
            }

            up();
            f();
            u();

            break;
        case Face::RIGHT:
            if (wg->getY()==1)
            {
                rp();
            }
            else if (wg->getY()==5)
            {
                r();
                r();
            }
            else if (wg->getY()==7)
            {
                r();
            }

            up();
            fp();
            u();

            break;
        case Face::FRONT:

            //there is no posibilty of one

            if (wg->getY()==5)
            {
                up();
                fp();
                fp();
                u();
                lp();
            }
            else if (wg->getY()==7)
            {
                up();
                f();
                u();
                lp();
            }
            else if(wg->getY()==3)
            {
                lp();
            }


            break;
        case Face::BACK:
            if (wg->getY()==1)
            {
                b();
                l();
            }
            else if (wg->getY()==5)
            {
                l();
            }
            else if (wg->getY()==3)
            {
                bp();
                bp();
                l();
            }
            else if (wg->getY()==7)
            {
                bp();
                l();
            }
            break;


        }
    }

    void Rubik::solveWhiteBlue()
    {
        Face face;

        Element* wg=findElement(41);

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());


        switch (face) {
        case Face::UP:
            break;
        case Face::DOWN:
            if (wg->getY()==1)
            {
                d();
                d();
                bp();
                bp();
            }
            else if (wg->getY()==5)
            {
                d();
                bp();
                bp();
            }
            else if (wg->getY()==7)
            {
                bp();
                bp();
            }
            else if (wg->getY()==3)
            {
                dp();
                bp();
                bp();
            }

            break;
        case Face::LEFT:
            if (wg->getY()==3)
            {
                bp();
            }
            else if (wg->getY()==5)
            {
                up();
                l();
                l();
                u();
                bp();
            }
            else if (wg->getY()==7)
            {
                up();
                l();
                u();
                bp();
            }

            break;
        case Face::RIGHT:
            if (wg->getY()==3)
            {
                u();
                r();
                r();
                up();
                b();
            }
            else if (wg->getY()==5)
            {
                b();
            }
            else if (wg->getY()==7)
            {
                u();
                rp();
                up();
                b();
            }

            break;
        case Face::FRONT:

            if (wg->getY()==3)
            {
                u();
                u();
                fp();
                fp();
                up();
                r();
                up();
            }
            else if (wg->getY()==5)
            {
                u();
                r();
                up();

            }
            else if (wg->getY()==7)
            {
                u();
                u();
                fp();
                up();
                r();
                up();
            }

            break;
        case Face::BACK:
            if (wg->getY()==1)
            {
                bp();
                u();
                rp();
                up();
            }
            else if (wg->getY()==3)
            {
                u();
                rp();
                up();
            }
            else if (wg->getY()==5)
            {
                bp();
                bp();
                u();
                rp();
                up();


            }
            else if (wg->getY()==7)
            {
                b();
                u();
                rp();
                up();

            }

            break;


        }

    }

    void Rubik::solveWhiteCorners()
    {
        solveLeftDownCorner();
        solveRightDownCorner();
        solveLeftTopCorner();
        solveRightTopCorner();

    }

    void Rubik::solveLeftDownCorner()
    {
        Face face;

        Element* wg=findElement(46);

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());


        switch (face) {
        case Face::UP:
            if (wg->getY()==8)
            {
                rp();
                d();
                fp();
                dp();
                dp();
                f();
                r();
            }
            else if (wg->getY()==2)
            {
                bp();
                fp();
                dp();
                dp();
                f();
                b();
            }
            else if (wg->getY()==0)
            {
                b();
                d();
                d();
                bp();
                l();
                dp();
                lp();
            }
            break;
        case Face::DOWN:
            if (wg->getY()==8)
            {
                dp();
                rp();
                fp();
                fp();
                r();
                f();
                f();
                r();

            }
            else if (wg->getY()==2)
            {
                rp();
                fp();
                fp();
                r();
                f();
                f();
                r();
            }
            else if (wg->getY()==6)
            {
                b();
                l();
                l();
                bp();
                lp();
                lp();
                bp();

            }
            else if (wg->getY()==0)
            {
                d();
                rp();
                fp();
                fp();
                r();
                f();
                f();
                r();
            }
            break;
        case Face::LEFT:
            if (wg->getY()==8)
            {
                dp();
                fp();
                d();
                f();

            }
            else if (wg->getY()==2)
            {
                l();
                d();
                d();
                lp();
                fp();
                d();
                d();
                f();

            }
            else if (wg->getY()==6)
            {
                dp();
                l();
                dp();
                dp();
                lp();
            }
            else if (wg->getY()==0)
            {

                b();
                dp();
                bp();
                dp();
                l();
                dp();
                dp();
                lp();

            }
            break;
        case Face::RIGHT:
            if (wg->getY()==8)
            {
                fp();
                dp();
                dp();
                f();

            }
            else if (wg->getY()==2)
            {
                r();
                fp();
                dp();
                dp();
                f();
                rp();
            }
            else if (wg->getY()==6)
            {
                l();
                dp();
                lp();

            }
            else if (wg->getY()==0)
            {
                rp();
                l();
                dp();
                lp();
                r();
            }
            break;
        case Face::FRONT:
            if (wg->getY()==8)
            {
                d();
                fp();
                dp();
                dp();
                f();
            }
            else if (wg->getY()==2)
            {
                f();
                d();
                fp();
                fp();
                dp();
                dp();
                f();

            }
            else if (wg->getY()==6)
            {
                d();
                l();
                dp();
                lp();

            }
            else if (wg->getY()==0)
            {
                fp();
                d();
                d();
                f();
                l();
                dp();
                dp();
                lp();

            }
            break;
        case Face::BACK:
            if (wg->getY()==8)
            {
                fp();
                d();
                f();
            }
            else if (wg->getY()==2)
            {
                b();
                dp();
                dp();
                bp();
                d();
                fp();
                dp();
                dp();
                f();
            }
            else if (wg->getY()==6)
            {
                l();
                dp();
                dp();
                lp();
            }
            else if (wg->getY()==0)
            {
                bp();
                l();
                dp();
                dp();
                lp();
                b();
            }
            break;
        }

    }

    void Rubik::solveRightDownCorner()
    {
        Face face;

        Element* wg=findElement(48);

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());

        switch (face) {
        case Face::UP:
            if (wg->getY()==2)
            {
                r();
                d();
                rp();
                f();
                dp();
                dp();
                fp();
            }
            else if (wg->getY()==0)
            {
                b();
                f();
                dp();
                dp();
                fp();
                bp();
            }
            break;
        case Face::DOWN:
            if (wg->getY()==0)
            {
                d();

            }
            else if (wg->getY()==6)
            {
                dp();
                dp();
            }
            else if (wg->getY()==8)
            {
                dp();
            }
            //common for d2
            rp();
            d();
            r();
            f();
            dp();
            dp();
            fp();

            break;
        case Face::LEFT:
            if (wg->getY()==8)
            {
                rp();
                d();
                r();

            }
            else if (wg->getY()==6)
            {
                f();
                d();
                d();
                fp();
            }
            else if (wg->getY()==0)
            {
                lp();
                f();
                d();
                d();
                fp();
                l();
            }
            break;
        case Face::RIGHT:
            if (wg->getY()==0)
            {
                rp();
                d();
                d();
                r();
                f();
                dp();
                dp();
                fp();
            }
            else if (wg->getY()==2)
            {
                r();
                d();
                r();
                r();
                d();
                d();
                r();

            }
            else if (wg->getY()==6)
            {
                d();
                f();
                dp();
                fp();
            }
            else if (wg->getY()==8)
            {
                d();
                rp();
                d();
                d();
                r();
            }
            break;
        case Face::FRONT:
            if (wg->getY()==8)
            {
                dp();
                rp();
                d();
                r();

            }
            else if (wg->getY()==2)
            {
                rp();
                dp();
                r();
                d();
                f();
                dp();
                fp();
                rp();
                d();
                d();
                r();

            }
            else if (wg->getY()==6)
            {
                dp();
                f();
                d();
                d();
                fp();
            }

            break;
        case Face::BACK:
            if (wg->getY()==0)
            {
                bp();
                f();
                dp();
                fp();
                b();
            }
            else if (wg->getY()==2)
            {
                b();
                rp();
                dp();
                dp();
                r();
                bp();

            }
            else if (wg->getY()==6)
            {
                f();
                dp();
                fp();
            }
            else if (wg->getY()==8)
            {
                rp();
                dp();
                dp();
                r();
            }
            break;
        }

    }

    void Rubik::solveLeftTopCorner()
    {
        Face face;

        Element* wg=findElement(40);

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());


        switch (face) {
        case Face::UP:
            if (wg->getY()==2)
            {
                bp();
                dp();
                b();
                lp();
                dp();
                dp();
                l();
            }
            break;
        case Face::DOWN:
            if (wg->getY()==8)
            {
                bp();
                dp();
                dp();
                b();
                b();
                d();
                d();
                bp();

            }
            else if (wg->getY()==2)
            {
                d();
                r();
                dp();
                rp();
                d();
                lp();
                d();
                d();
                l();
            }
            else if (wg->getY()==6)
            {
                b();
                dp();
                bp();
                lp();
                d();
                d();
                l();
            }
            else if (wg->getY()==0)
            {
                dp();
                b();
                dp();
                bp();
                lp();
                d();
                d();
                l();
            }
            break;
        case Face::LEFT:
            if (wg->getY()==0)
            {
                lp();
                d();
                d();
                l();
                b();
                d();
                d();
                bp();

            }
            else if (wg->getY()==6)
            {
                d();
                b();
                dp();
                bp();
            }
            else if (wg->getY()==8)
            {
                d();
                lp();
                d();
                d();
                l();

            }
            break;
        case Face::RIGHT:
            if (wg->getY()==8)
            {
                lp();
                d();
                l();
            }
            else if (wg->getY()==2)
            {
                r();
                lp();
                d();
                l();
                rp();
            }
            else if (wg->getY()==6)
            {
                b();
                d();
                d();
                bp();
            }

            break;
        case Face::FRONT:
            if (wg->getY()==8)
            {
                lp();
                d();
                d();
                l();
            }
            else if (wg->getY()==6)
            {
                b();
                dp();
                bp();
            }
            break;
        case Face::BACK:
            if (wg->getY()==8)
            {
                dp();
                lp();
                d();
                l();
            }
            else if (wg->getY()==2)
            {
                b();
                dp();
                dp();
                bp();
                lp();
                d();
                d();
                l();

            }
            else if (wg->getY()==6)
            {
                dp();
                b();
                d();
                d();
                bp();
            }
            else if (wg->getY()==0)
            {
                bp();
                dp();
                b();
                b();
                d();
                d();
                bp();

            }
            break;
        }

    }

    void Rubik::solveRightTopCorner()
    {
        Face face;

        Element* wg=findElement(42);

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());

        switch (face) {
        case Face::UP:
            break;
        case Face::DOWN:
            if (wg->getY()==8)
            {
                r();
                dp();
                rp();
                bp();
                d();
                d();
                b();
            }
            else if (wg->getY()==2)
            {
                d();
                r();
                dp();
                rp();
                bp();
                d();
                d();
                b();
            }
            else if (wg->getY()==6)
            {
                dp();
                r();
                dp();
                rp();
                bp();
                d();
                d();
                b();
            }
            else if (wg->getY()==0)
            {
                d();
                d();
                r();
                dp();
                rp();
                bp();
                d();
                d();
                b();
            }
            break;
        case Face::LEFT:
            if (wg->getY()==8)
            {
                bp();
                d();
                d();
                b();
            }
            else if (wg->getY()==6)
            {
                r();
                dp();
                rp();

            }
            break;
        case Face::RIGHT:
            if (wg->getY()==8)
            {
                dp();
                bp();
                d();
                b();
            }
            else if (wg->getY()==6)
            {
                dp();
                r();
                d();
                d();
                rp();
            }
            else if (wg->getY()==2)
            {
                r();
                dp();
                dp();
                rp();
                bp();
                d();
                d();
                b();
            }

            break;
        case Face::FRONT:
            if (wg->getY()==8)
            {
                bp();
                d();
                b();
            }
            else if (wg->getY()==6)
            {
                r();
                d();
                d();
                rp();
            }

            break;
        case Face::BACK:
            if (wg->getY()==8)
            {
                d();
                bp();
                d();
                d();
                b();
            }
            else if (wg->getY()==6)
            {
                d();
                r();
                dp();
                rp();
            }
            else if (wg->getY()==0)
            {
                bp();
                dp();
                dp();
                b();
                r();
                d();
                d();
                rp();
            }
            break;
        }
    }

    void Rubik::solveMiddleLayer()
    {
        solveEdgeOrangeGreen();
        solveEdgeGreenRed();
        solveEdgeRedBlue();
        solveEdgeBlueOrange();
    }

    void Rubik::solveEdgeGreenRed()
    {
        Element* wg=findElement(15); //15

        //first check if its already in position
        if (wg->inPosition())
            return; //already in position

        Face face;
        Face targetFace;

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());
        targetFace=Face::FRONT;

        if (face==Face::DOWN)
        {
            //process the other side of the edge instead
            wg=findElement(23);
            face=static_cast<Face>(wg->getFace());
            targetFace=Face::RIGHT;
        }

        //check if in row 2
        int row=wg->getY()/3;
        if (row==1)
        {
            //get the face and column
            int column=wg->getY()%3;

            if (column==0)
            {
                //apply the left pattern
                toLeftPattern((int)face);
            }
            else
            {
                //apply the right pattern;
                toRightPattern((int)face);
            }

            //we must work on the opposite edge in this case
            wg=findElement(23);
            face=static_cast<Face>(wg->getFace());
            targetFace=Face::RIGHT;
        }

        //the piece should now be in row 3;either the origpiece or the opposite piece

        //align the tile to the target face
        if (targetFace==Face::FRONT)
        {
            if (face!=Face::FRONT)
            {
                if (face==Face::RIGHT)
                    dp();
                else if (face==Face::BACK)
                {
                    dp();
                    dp();
                }
                else
                {
                    d();
                }
            }

            toRightPattern((int)Face::FRONT);
        }
        else if (targetFace==Face::RIGHT)
        {
            if (face!=Face::RIGHT)
            {
                if (face==Face::FRONT)
                    d();
                else if (face==Face::BACK)
                {
                    dp();
                }
                else
                {
                    d();
                    d();
                }
            }

            toLeftPattern((int)Face::RIGHT);
        }
    }

    void Rubik::solveEdgeOrangeGreen()
    {
        Element* wg=findElement(5); //05

        //first check if its already in position
        if (wg->inPosition())
            return; //already in position

        Face face;
        Face targetFace;

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());
        targetFace=Face::LEFT;

        if (face==Face::DOWN)
        {
            //process the other side of the edge instead
            wg=findElement(13);
            face=static_cast<Face>(wg->getFace());
            targetFace=Face::FRONT;
        }

        //check if in row 2
        int row=wg->getY()/3;
        if (row==1)
        {
            //get the face and column
            int column=wg->getY()%3;

            if (column==0)
            {
                //apply the left pattern
                toLeftPattern((int)face);
            }
            else
            {
                //apply the right pattern;
                toRightPattern((int)face);
            }

            //we must work on the opposite edge in this case
            wg=findElement(13);
            face=static_cast<Face>(wg->getFace());
            targetFace=Face::FRONT;
        }

        //the piece should now be in row 3;either the origpiece or the opposite piece

        //align the tile to the target face
        if (targetFace==Face::LEFT)
        {
            if (face!=Face::LEFT)
            {
                if (face==Face::FRONT)
                    dp();
                else if (face==Face::RIGHT)
                {
                    dp();
                    dp();
                }
                else
                {
                    d();
                }
            }

            toRightPattern((int)Face::LEFT);
        }
        else if (targetFace==Face::FRONT)
        {
            if (face!=Face::FRONT)
            {
                if (face==Face::LEFT)
                    d();
                else if (face==Face::RIGHT)
                {
                    dp();
                }
                else
                {
                    d();
                    d();
                }
            }

            toLeftPattern((int)Face::FRONT);
        }
    }

    void Rubik::solveEdgeRedBlue()
    {
        Element* wg=findElement(25); //15

        //first check if its already in position
        if (wg->inPosition())
            return; //already in position

        Face face;
        Face targetFace;

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());
        targetFace=Face::RIGHT;

        if (face==Face::DOWN)
        {
            //process the other side of the edge instead
            wg=findElement(33);
            face=static_cast<Face>(wg->getFace());
            targetFace=Face::BACK;
        }

        //check if in row 2
        int row=wg->getY()/3;
        if (row==1)
        {
            //get the face and column
            int column=wg->getY()%3;

            if (column==0)
            {
                //apply the left pattern
                toLeftPattern((int)face);
            }
            else
            {
                //apply the right pattern;
                toRightPattern((int)face);
            }

            //we must work on the opposite edge in this case
            wg=findElement(33);
            face=static_cast<Face>(wg->getFace());
            targetFace=Face::BACK;
        }

        //the piece should now be in row 3;either the origpiece or the opposite piece

        //align the tile to the target face
        if (targetFace==Face::RIGHT)
        {
            if (face!=Face::RIGHT)
            {
                if (face==Face::BACK)
                    dp();
                else if (face==Face::LEFT)
                {
                    dp();
                    dp();
                }
                else
                {
                    d();
                }
            }

            toRightPattern((int)Face::RIGHT);
        }
        else if (targetFace==Face::BACK)
        {
            if (face!=Face::BACK)
            {
                if (face==Face::RIGHT)
                    d();
                else if (face==Face::LEFT)
                {
                    dp();
                }
                else
                {
                    d();
                    d();
                }
            }

            toLeftPattern((int)Face::BACK);
        }

    }

    void Rubik::solveEdgeBlueOrange()
    {
        Element* wg=findElement(35); //15

        //first check if its already in position
        if (wg->inPosition())
            return; //already in position

        Face face;
        Face targetFace;

        //the face is just the index x of the element
        face = static_cast<Face>(wg->getFace());
        targetFace=Face::BACK;

        if (face==Face::DOWN)
        {
            //process the other side of the edge instead
            wg=findElement(3);
            face=static_cast<Face>(wg->getFace());
            targetFace=Face::LEFT;
        }

        //check if in row 2
        int row=wg->getY()/3;
        if (row==1)
        {
            //get the face and column
            int column=wg->getY()%3;

            if (column==0)
            {
                //apply the left pattern
                toLeftPattern((int)face);
            }
            else
            {
                //apply the right pattern;
                toRightPattern((int)face);
            }

            //we must work on the opposite edge in this case
            wg=findElement(03);
            face=static_cast<Face>(wg->getFace());
            targetFace=Face::LEFT;
        }

        //the piece should now be in row 3;either the origpiece or the opposite piece

        //align the tile to the target face
        if (targetFace==Face::BACK)
        {
            if (face!=Face::BACK)
            {
                if (face==Face::LEFT)
                    dp();
                else if (face==Face::FRONT)
                {
                    dp();
                    dp();
                }
                else
                {
                    d();
                }
            }

            toRightPattern((int)Face::BACK);
        }
        else if (targetFace==Face::LEFT)
        {
            if (face!=Face::LEFT)
            {
                if (face==Face::BACK)
                    d();
                else if (face==Face::FRONT)
                {
                    dp();
                }
                else
                {
                    d();
                    d();
                }
            }

            toLeftPattern((int)Face::LEFT);
        }
    }

    Element *Rubik::findElement(int value)
    {
        for (int i=0; i< 6 ;i++)
        {
            for (int j=0; j<9; j++)
            {
                if (elements[i][j]->getValue()==value)
                    return elements[i][j];
            }
        }

        return 0;
    }

    Element *Rubik::findCorner(CornerPiece cornerpiece, Color color)
    {
        int value=-1;
        switch (cornerpiece)
        {
        case CornerPiece::WGO:
            if (color==RubikBot::Color::WHITE)
                value=46;
            else if (color==RubikBot::Color::GREEN)
                value=10;
            else if(color==RubikBot::Color::ORANGE)
                value=2;
            break;
        case CornerPiece::WOB:
            if (color==RubikBot::Color::WHITE)
                value=40;
            else if (color==RubikBot::Color::ORANGE)
                value=0;
            else if(color==RubikBot::Color::BLUE)
                value=32;
            break;
        case CornerPiece::WBR:
            if (color==RubikBot::Color::WHITE)
                value=42;
            else if (color==RubikBot::Color::BLUE)
                value=30;
            else if(color==RubikBot::Color::RED)
                value=22;
            break;
        case CornerPiece::WRG:
            if (color==RubikBot::Color::WHITE)
                value=48;
            else if (color==RubikBot::Color::RED)
                value=20;
            else if(color==RubikBot::Color::GREEN)
                value=12;
            break;

        default:
            break;
        }

        return findElement(value);
    }

    Element *Rubik::findEdge(EdgePiece edgepiece, Color color)
    {
        int value=-1;
        switch (edgepiece) {
        case RubikBot::EdgePiece::GO:
            if (color==RubikBot::Color::GREEN)
                value=13;
            else if (color==RubikBot::Color::ORANGE)
                value=5;
            break;
        case RubikBot::EdgePiece::OB:
            if (color==RubikBot::Color::BLUE)
                value=35;
            else if (color==RubikBot::Color::ORANGE)
                value=3;

            break;
        case RubikBot::EdgePiece::BRX:
            if (color==RubikBot::Color::BLUE)
                value=33;
            else if (color==RubikBot::Color::RED)
                value=25;
            break;

        case RubikBot::EdgePiece::RG:
            if (color==RubikBot::Color::RED)
                value=23;
            else if (color==RubikBot::Color::GREEN)
                value=15;
            break;

        }
        return findElement(value);
    }

    Element *Rubik::getElementAt(int index)
    {
        return elements[index/10][index%10];
    }

    void Rubik::solveYellowCrossPattern(int face)
    {
        //yellowCrossPattern(face);
        //return;

        //this should not return until the cross is formed

        Element* edge1;
        Element* edge3;
        Element* edge5;
        Element* edge7;

        while  (true)
        {
            int count=0;
            int face=(int)Face::FRONT;

            //get the yellow edges count
            edge1=getElement(5,1);
            edge3=getElement(5,3);
            edge5=getElement(5,5);
            edge7=getElement(5,7);

            if (edge1->getColor()==5)
                count++;
            if (edge3->getColor()==5)
                count++;
            if (edge5->getColor()==5)
                count++;
            if (edge7->getColor()==5)
                count++;

            if (count==0)
            {
                face=(int)Face::FRONT;
                this->yellowCrossPattern(face);
            }
            else if (count==2)
            {
                //is it inline
                if (edge1->getColor()==5 && edge7->getColor()==5)
                {
                    face=(int)Face::LEFT;
                }
                else if (edge3->getColor()==5 && edge5->getColor()==5)
                {
                    face=(int)Face::FRONT;
                }
                else
                {
                    //this is an L SHAPE
                    if (edge1->getColor()==5)
                    {
                        if (edge3->getColor()==5)
                        {
                            face=(int)Face::BACK;
                        }

                        if (edge5->getColor()==5)
                            face=(int)Face::LEFT;
                    }

                    if (edge7->getColor()==5)
                    {
                        if (edge3->getColor()==5)
                        {
                            face=(int)Face::RIGHT;
                        }

                        if (edge5->getColor()==5)
                            face=(int)Face::FRONT;
                    }
                }

                //apply pattern
                yellowCrossPattern(face);

            }
            else if (count==4)
                break;          //we have yellow cross

        }//end while

    }

    void Rubik::d(int face)
    {
        switch (face)
        {
        case (int)Face::FRONT:
            d();
            break;
        case (int)Face::BACK:
            d();
            break;
        case (int)Face::LEFT:
            d();
            break;
        case (int)Face::RIGHT:
            d();
            break;
        case (int)Face::UP:
            f();
            break;
        case (int)Face::DOWN:
            b();
            break;

        }

    }

    void Rubik::dp(int face)
    {
        switch (face)
        {
        case (int)Face::FRONT:
        case (int)Face::BACK:
        case (int)Face::LEFT:
        case (int)Face::RIGHT:
            dp();
            break;
        case (int)Face::UP:
            fp();
            break;
        case (int)Face::DOWN:
            bp();
            break;

        }



    }

    void Rubik::r(int face)
    {
        switch (face)
        {
        case (int)Face::UP:
        case (int)Face::DOWN:
        case (int)Face::FRONT:
            r();
            break;
        case (int)Face::BACK:
            l();
            break;
        case (int)Face::LEFT:
            f();
            break;
        case (int)Face::RIGHT:
            b();
            break;

        }
    }

    void Rubik::rp(int face)
    {
        switch (face)
        {
        case (int)Face::UP:
        case (int)Face::DOWN:
        case (int)Face::FRONT:
            rp();
            break;
        case (int)Face::BACK:
            lp();
            break;
        case (int)Face::LEFT:
            fp();
            break;
        case (int)Face::RIGHT:
            bp();
            break;

        }

    }

    void Rubik::l(int face)
    {
        switch (face)
        {
        case (int)Face::UP:
        case (int)Face::DOWN:
        case (int)Face::FRONT:
            l();
            break;
        case (int)Face::BACK:
            r();
            break;
        case (int)Face::LEFT:
            b();
            break;
        case (int)Face::RIGHT:
            f();
            break;

        }

    }

    void Rubik::lp(int face)
    {
        switch (face)
        {
        case (int)Face::UP:
        case (int)Face::DOWN:
        case (int)Face::FRONT:
            lp();
            break;
        case (int)Face::BACK:
            rp();
            break;
        case (int)Face::LEFT:
            bp();
            break;
        case (int)Face::RIGHT:
            fp();
            break;

        }
    }

    void Rubik::b(int face)
    {
        switch (face)
        {
        case (int)Face::FRONT:
            b();
            break;
        case (int)Face::BACK:
            f();
            break;
        case (int)Face::LEFT:
            r();
            break;
        case (int)Face::RIGHT:
            l();
            break;
        case (int)Face::UP:
            d();
            break;
        case (int)Face::DOWN:
            u();

        }

    }

    void Rubik::bp(int face)
    {
        switch (face)
        {
        case (int)Face::FRONT:
            bp();
            break;
        case (int)Face::BACK:
            fp();
            break;
        case (int)Face::LEFT:
            rp();
            break;
        case (int)Face::RIGHT:
            lp();
            break;
        case (int)Face::UP:
            dp();
            break;
        case (int)Face::DOWN:
            up();
        }
    }

    void Rubik::u(int face)
    {
        switch (face)
        {

        case (int)Face::FRONT:
        case (int)Face::BACK:
        case (int)Face::LEFT:
        case (int)Face::RIGHT:
            u();
            break;
        case (int)Face::UP:
            b();
            break;
        case (int)Face::DOWN:
            f();
            break;

        }
    }

    void Rubik::up(int face)
    {
        switch (face)
        {
        case (int)Face::FRONT:
        case (int)Face::BACK:
        case (int)Face::LEFT:
        case (int)Face::RIGHT:
            up();
            break;
        case (int)Face::UP:
            bp();
            break;
        case (int)Face::DOWN:
            fp();
            break;
        }
    }

    void Rubik::f(int face)
    {
        switch (face)
        {

        case (int)Face::FRONT:
            f();
            break;
        case (int)Face::BACK:
            b();
            break;
        case (int)Face::LEFT:
            l();
            break;
        case (int)Face::RIGHT:
            r();
            break;
        case (int)Face::DOWN:
            d();
            break;
        case (int)Face::UP:
            u();
        }
    }

    void Rubik::fp(int face)
    {
        switch (face)
        {
        case (int)Face::FRONT:
            fp();
            break;
        case (int)Face::BACK:
            bp();
            break;
        case (int)Face::LEFT:
            lp();
            break;
        case (int)Face::RIGHT:
            rp();
            break;
        case (int)Face::DOWN:
            dp();
            break;
        case (int)Face::UP:
            up();

        }

    }
    void Rubik::lp()
    {

        //copy the front elements
        Element* temp[9];
        int face=0;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //store the three top colors
        temp[0]=getElement(4,0);
        temp[1]=getElement(4,3);
        temp[2]=getElement(4,6);

        setElement(4,0,getElement(1,0));
        setElement(4,3,getElement(1,3));
        setElement(4,6,getElement(1,6));
        setElement(1,0,getElement(5,0));
        setElement(1,3,getElement(5,3));
        setElement(1,6,getElement(5,6));
        setElement(5,0,getElement(3,8));
        setElement(5,3,getElement(3,5));
        setElement(5,6,getElement(3,2));
        setElement(3,2,temp[2]);
        setElement(3,5,temp[1]);
        setElement(3,8,temp[0]);

        moves.appendMove((int)Moves::Lp);

    }


    void Rubik::fp()
    {
        //copy the front elements
        Element* temp[9];
        int face=1;
        for (int i=0; i<9;i++)
        {
            temp[i]=getElement(face,i);
        }

        setElement(face,0,temp[2]);
        setElement(face,1,temp[5]);
        setElement(face,2,temp[8]);
        setElement(face,3,temp[1]);
        setElement(face,4,temp[4]);
        setElement(face,5,temp[7]);
        setElement(face,6,temp[0]);
        setElement(face,7,temp[3]);
        setElement(face,8,temp[6]);

        //store the three top colors
        temp[0]=getElement(4,6);
        temp[1]=getElement(4,7);
        temp[2]=getElement(4,8);

        setElement(4,6,getElement(2,0));
        setElement(4,7,getElement(2,3));
        setElement(4,8,getElement(2,6));
        setElement(2,0,getElement(5,2));
        setElement(2,3,getElement(5,1));
        setElement(2,6,getElement(5,0));
        setElement(5,0,getElement(0,2));
        setElement(5,1,getElement(0,5));
        setElement(5,2,getElement(0,8));
        setElement(0,2,temp[2]);
        setElement(0,5,temp[1]);
        setElement(0,8,temp[0]);

        moves.appendMove((int)Moves::Fp);


    }

    void Rubik::toRightPattern(int face)
    {
        dp();
        rp(face);
        d();
        r(face);
        d();
        f(face);
        dp();
        fp(face);
    }

    void Rubik::toLeftPattern(int face)
    {
        d();
        l(face);
        dp();
        lp(face);
        dp();
        fp(face);
        d();
        f(face);
    }

    void Rubik::yellowCrossPattern(int face)
    {
        //this pattern is repeatedly called until the yellow cross is formed
        f(face);
        l(face);
        d();
        lp(face);
        dp();
        fp(face);
    }

    bool Rubik::isF2LCornerInserted(CornerPiece corner)
    {
        Element* element;
        switch (corner) {
        case CornerPiece::WGO:
            element=findCorner(corner,RubikBot::Color::WHITE);
            if (element->getFace()!=(int)RubikBot::Color::WHITE)
                return false;

            element=findCorner(corner,RubikBot::Color::GREEN);
            if (element->getFace()!=(int)RubikBot::Color::GREEN)
                return false;

            element=findCorner(corner,RubikBot::Color::ORANGE);
            if (element->getFace()!=(int)RubikBot::Color::ORANGE)
                return false;

            //check for the edge piece
            element=findEdge(RubikBot::EdgePiece::GO,RubikBot::Color::GREEN);
            if (element->getFace()!=(int)RubikBot::Color::GREEN)
                return false;

            element=findEdge(RubikBot::EdgePiece::GO,RubikBot::Color::ORANGE);
            if (element->getFace()!=(int)RubikBot::Color::ORANGE)
                return false;

            return true;
        case CornerPiece::WOB:
            element=findCorner(corner,RubikBot::Color::WHITE);
            if (element->getFace()!=(int)RubikBot::Color::WHITE)
                return false;

            element=findCorner(corner,RubikBot::Color::ORANGE);
            if (element->getFace()!=(int)RubikBot::Color::ORANGE)
                return false;

            element=findCorner(corner,RubikBot::Color::BLUE);
            if (element->getFace()!=(int)RubikBot::Color::BLUE)
                return false;

            //check for the edge piece
            element=findEdge(RubikBot::EdgePiece::OB,RubikBot::Color::ORANGE);
            if (element->getFace()!=(int)RubikBot::Color::ORANGE)
                return false;

            element=findEdge(RubikBot::EdgePiece::OB,RubikBot::Color::BLUE);
            if (element->getFace()!=(int)RubikBot::Color::BLUE)
                return false;

            return true;
        case CornerPiece::WBR:
            element=findCorner(corner,RubikBot::Color::WHITE);
            if (element->getFace()!=(int)RubikBot::Color::WHITE)
                return false;

            element=findCorner(corner,RubikBot::Color::BLUE);
            if (element->getFace()!=(int)RubikBot::Color::BLUE)
                return false;

            element=findCorner(corner,RubikBot::Color::RED);
            if (element->getFace()!=(int)RubikBot::Color::RED)
                return false;

            //check for the edge piece
            element=findEdge(RubikBot::EdgePiece::BRX,RubikBot::Color::BLUE);
            if (element->getFace()!=(int)RubikBot::Color::BLUE)
                return false;

            element=findEdge(RubikBot::EdgePiece::BRX,RubikBot::Color::RED);
            if (element->getFace()!=(int)RubikBot::Color::RED)
                return false;

            return true;
        case CornerPiece::WRG:
            element=findCorner(corner,RubikBot::Color::WHITE);
            if (element->getFace()!=(int)RubikBot::Color::WHITE)
                return false;

            element=findCorner(corner,RubikBot::Color::RED);
            if (element->getFace()!=(int)RubikBot::Color::RED)
                return false;

            element=findCorner(corner,RubikBot::Color::GREEN);
            if (element->getFace()!=(int)RubikBot::Color::GREEN)
                return false;

            //check for the edge piece
            element=findEdge(RubikBot::EdgePiece::RG,RubikBot::Color::RED);
            if (element->getFace()!=(int)RubikBot::Color::RED)
                return false;

            element=findEdge(RubikBot::EdgePiece::RG,RubikBot::Color::GREEN);
            if (element->getFace()!=(int)RubikBot::Color::GREEN)
                return false;

            return true;

        }

        return false;
    }

    void Rubik::updateF2LCornerStatus(CornerPiece corner)
    {
        //mark that the WGOSOLVED=TRUE;
        if (corner==CornerPiece::WGO)
            f2lData.f2lWGOsolved=true;

        if (corner==CornerPiece::WOB)
            f2lData.f2lWOBsolved=true;

        if (corner==CornerPiece::WBR)
            f2lData.f2lWBRsolved=true;

        if (corner==CornerPiece::WRG)
            f2lData.f2lWRGsolved=true;
    }

    bool Rubik::isOppositeDiagonalMadeUp(Element* corner1)
    {
        //corner1 is always the whitecorner
        //corner2 is should be facing DOWN
        //corner3 is the oppositecorner

        int whiteindex=corner1->getY(); //with this 2 info we can now deduce the diagonal opposite
        int face=corner1->getFace();

        if (face==(int)Face::LEFT)
        {
            if (whiteindex==8)
            {
                return f2lData.f2lWOBsolved;
            }
            else if (whiteindex==6)
            {
                return f2lData.f2lWGOsolved;
            }
        }
        else if (face==(int)Face::FRONT)
        {
            if (whiteindex==8)
            {
                return f2lData.f2lWGOsolved;
            }
            else if (whiteindex==6)
            {
                return f2lData.f2lWRGsolved;
            }
        }
        else if (face==(int)Face::RIGHT)
        {
            if (whiteindex==8)
            {
                return f2lData.f2lWRGsolved;
            }
            else if (whiteindex==6)
            {
                return f2lData.f2lWBRsolved;
            }
        }
        else if (face==(int)Face::BACK)
        {
            if (whiteindex==8)
            {
                return f2lData.f2lWBRsolved;
            }
            else if (whiteindex==6)
            {
                return f2lData.f2lWOBsolved;
            }

        }

        return true;

    }

    bool Rubik::isDirectlyUnderMadeUp(Element *corner1)
    {
        //this assumes that corner1 is facing DOWN and could
        //be in ANY of the four corners
        if (corner1->getFace()!=(int)Face::DOWN)
            return false;

        int index=corner1->getY();

        if (index==0)
        {
            return f2lData.f2lWGOsolved;
        }
        else if (index==2)
        {
            return f2lData.f2lWRGsolved;
        }
        else if (index==6)
        {
            return f2lData.f2lWOBsolved;
        }
        else
        {
            return f2lData.f2lWBRsolved;
        }

    }

    void Rubik::f2lInitStruct()
    {
    f2lData.activecorner=0;
    f2lData.activemid=0;
    f2lData.corner1=0;
    f2lData.corner2=0;
    f2lData.corner3=0;
    f2lData.mid1=0;
    f2lData.mid2=0;
    f2lData.oppositecorner=0;
    f2lData.oppositemid=0;

    f2lData.f2lWBRsolved=false;
    f2lData.f2lWGOsolved=false;
    f2lData.f2lWOBsolved=false;
    f2lData.f2lWRGsolved=false;
    }

    void Rubik::f2lInitialize(CornerPiece corner)
    {
        switch (corner)
        {
        case CornerPiece::WGO:
            f2lData.corner1=findElement(46);
            f2lData.corner2=findElement(10);
            f2lData.corner3=findElement(2);
            f2lData.mid1=findElement(13);
            f2lData.mid2=findElement(5);
            break;
        case CornerPiece::WOB:
            f2lData.corner1=findElement(40);
            f2lData.corner2=findElement(0);
            f2lData.corner3=findElement(32);
            f2lData.mid1=findElement(3);
            f2lData.mid2=findElement(35);
            break;
        case CornerPiece::WBR:
            f2lData.corner1=findElement(42);
            f2lData.corner2=findElement(30);
            f2lData.corner3=findElement(22);
            f2lData.mid1=findElement(33);
            f2lData.mid2=findElement(25);
            break;
        case CornerPiece::WRG:
            f2lData.corner1=findElement(48);
            f2lData.corner2=findElement(20);
            f2lData.corner3=findElement(12);
            f2lData.mid1=findElement(23);
            f2lData.mid2=findElement(15);
            break;
        default:
            break;
        }

    }

    void Rubik::f2lLevel3()
    {

        //check if the corner is in LOWEST LEVEL
        int y=f2lData.corner1->getY();

        //get the level of the corner piece
        if (f2lData.corner1->getFace()==(int)Face::UP)
        {
            //bring it up to 3rd level
            //correct facing
            if (y==0)
            {
                lp();
                dp();
                l();
            }
            else if (y==2)
            {
                r();
                d();
                rp();
            }
            else if (y==6)
            {
                l();
                d();
                lp();
            }
            else if (y==8)
            {
                rp();
                dp();
                r();
            }
        }
        else if (f2lData.corner1->getFace()==(int)Face::DOWN)
        {
            //rotate DOWN until the center is DIRECTLY under an unmade corner
            while (isDirectlyUnderMadeUp(f2lData.corner1))
                d();

            //either corner will do
            //corner2 or corner 3 face
            //this will depend on optimization later
        //but for now; do the corner2
            int face=f2lData.corner2->getFace();
            y=f2lData.corner2->getY();

            //bring it up to 3rd level
            //correct facing
            /*if (y==0)
            {
                up(DOWN);
                f(DOWN);
                u(DOWN);
            }
            else if (y==2)
            {
                u(DOWN);
                fp(DOWN);
                up(DOWN);
            }
            else*/
            if (y==6)
            {
                l(face);
                dp(face);
                lp(face);
            }
            else if (y==8)
            {
                rp(face);
                d(face);
                r(face);
            }
        }
        else
        {
            //the white side is facing OGRB which is ideal but
            //we need to bring it up to the third level if not already
            if (y==0)
            {
                int face=f2lData.corner1->getFace();
                fp(face);
                dp(face);
                f(face);
            }
            else if (y==2)
            {
                int face=f2lData.corner1->getFace();
                f(face);
                d(face);
                fp(face);
            }

            //all  else its on level 3 already
        }



        //get the level of the edgepiece
        if (f2lData.mid1->getFace()==(int)Face::DOWN || f2lData.mid2->getFace()==(int)Face::DOWN)
        {

        }
        else
        {


            //we need to bring it to level 3
            //but we need to make sure that the corner piece is not affected
            //when doing so

            //NOTE if the corner and the edge share 2 FACES then they are on top of each other
            int sharedcount=0;

            int mid1face=f2lData.mid1->getFace();
            int mid2face=f2lData.mid2->getFace();

            if (mid1face==f2lData.corner1->getFace())
                sharedcount++;
            if (mid1face==f2lData.corner2->getFace())
                sharedcount++;
            if (mid1face==f2lData.corner3->getFace())
                sharedcount++;

            if (mid2face==f2lData.corner1->getFace())
                sharedcount++;
            if (mid2face==f2lData.corner2->getFace())
                sharedcount++;
            if (mid2face==f2lData.corner3->getFace())
                sharedcount++;

            //

            if (sharedcount==2 || sharedcount==0)
            {
                //they are directly on top of each other
                //or comepletely not sharing any sides
                d();
            }

            //find the unshared FACE with respect to the MID PIECE
            int unshared;
            Element* activepiece;

            if (mid1face == f2lData.corner1->getFace() || mid1face == f2lData.corner2->getFace()
                    ||mid1face==f2lData.corner3->getFace())
            {
                unshared=mid2face;
                activepiece=f2lData.mid2;

            }
            else
            {
                unshared=mid1face;
                activepiece=f2lData.mid1;
            }


            //process mid2 piece
            if (activepiece->getY()==5)
            {
                //right process
                f(unshared);
                d(unshared);
                fp(unshared);
            }
            else if (activepiece->getY()==3)
            {
                //left process
                fp(unshared);
                dp(unshared);
                f(unshared);

            }

        }


    }

    void Rubik::f2lLevel3V2()
    {
        //check if the corner is in LOWEST LEVEL
        int y=f2lData.corner1->getY();
        int cornerlevel;
        int midlevel;

        if (f2lData.corner1->getFace()==(int)Face::UP)
        {
            cornerlevel=1;
        }
        else if (f2lData.corner1->getFace()==(int)Face::DOWN)
        {
            cornerlevel=3;
        }
        else
        {
            if (y==0 || y==2)
            {
                cornerlevel=1;
            }
            else
                cornerlevel=3;
        }

        //check the mid level;
        if (f2lData.mid1->getFace()==(int)Face::DOWN || f2lData.mid2->getFace()==(int)Face::DOWN)
        {
            midlevel=3;
        }
        else
            midlevel=2;


        //if corner level is 1 and midlevel is 3 then
        //we can do optimized move to bring both in level 3 and in the correct position
        if (cornerlevel==1)
        {
            if (f2lData.corner1->getFace()==(int)Face::UP)
            {
                if (midlevel==3)
                {
                    //optimize the move
                    //we should always consider OPPOSITE COLOR in finger test
                    //so that we can reposition the mid element without conflict
                    Element* activecorner;
                    Element* activemid;

                    //first find the active midelement;
                    if (f2lData.mid1->getFace()==(int)Face::DOWN)
                        activemid=f2lData.mid1;
                    else
                        activemid=f2lData.mid2;

                    //the active corner will be the opposite of that color;
                    if (f2lData.corner2->getColor()==activemid->getColor())
                    {
                        activecorner=f2lData.corner3;
                    }
                    else
                        activecorner=f2lData.corner2;

                    int idealy;

                    //facing the active corner; do the moves
                    if (activecorner->getY()==0)
                    {
                        //this is left hand
                        if (activecorner->getFace()==(int)Face::FRONT)
                        {
                            idealy=5;
                        }
                        else if (activecorner->getFace()==(int)Face::LEFT)
                        {
                            idealy=1;
                        }
                        else if (activecorner->getFace()==(int)Face::BACK)
                        {
                            idealy=3;
                        }
                        else
                            idealy=7;
                    }
                    else
                    {
                        //this is right hand;
                        if (activecorner->getFace()==(int)Face::FRONT)
                        {
                            idealy=3;
                        }
                        else if (activecorner->getFace()==(int)Face::LEFT)
                        {
                            idealy=7;
                        }
                        else if (activecorner->getFace()==(int)Face::BACK)
                        {
                            idealy=5;
                        }
                        else
                            idealy=1;
                    }

                    //rotate down until idealy
                    while(activemid->getY()!=idealy)
                        dp();

                    if (activecorner->getY()==0)
                    {
                        //left
                        int face=activecorner->getFace();
                        l(face);
                        d();
                        lp(face);
                    }
                    else
                    {
                        //right
                        int face=activecorner->getFace();
                        rp(face);
                        dp();
                        r(face);
                    }


                }
                else //mid is not in level 3
                {
                    //we can still do optimization by not hitting it when turning
                    //nothing to do

                    //white corner is UP and MID is in level 2

                    //THIS IS NOT OPTIMIZED YET
                    int face=f2lData.corner2->getFace();

                    if (f2lData.corner2->getY()==0)
                    {
                        l(face);
                        d();
                        lp(face);
                    }
                    else
                    {
                        rp(face);
                        dp();
                        r(face);

                    }

                }
            }
            else
            {
                //corner1 is not UP but sides level 1
                //in this case; we can do optimization either opposites or samecolor
                if (midlevel==3)
                {

                    Element* activecorner;
                    Element* activemid;

                    //first find the active midelement;
                    if (f2lData.mid1->getFace()==(int)Face::DOWN)
                        activemid=f2lData.mid1;
                    else
                        activemid=f2lData.mid2;

                    //the active corner will be the corner that is not facing UP
                    if (f2lData.corner2->getFace()==(int)Face::UP)
                    {
                        activecorner=f2lData.corner3;
                    }
                    else
                        activecorner=f2lData.corner2;


                    int idealy;
                    bool samecolor=false;

                    if (activecorner->getColor()==activemid->getColor())
                        samecolor=true;


                    //facing the active corner; do the moves
                    if (activecorner->getY()==0)
                    {
                        //this is left hand
                        if (activecorner->getFace()==(int)Face::FRONT)
                        {
                            if (samecolor)
                            {
                                idealy=1;
                            }
                            else
                                idealy=7;
                        }
                        else if (activecorner->getFace()==(int)Face::LEFT)
                        {
                            if (samecolor)
                            {
                                idealy=3;
                            }
                            else
                                idealy=5;
                        }
                        else if (activecorner->getFace()==(int)Face::BACK)
                        {
                            if (samecolor)
                            {
                                idealy=7;
                            }
                            else
                                idealy=1;
                        }
                        else
                            if (samecolor)
                            {
                                idealy=5;
                            }
                            else
                                idealy=3;
                    }
                    else
                    {
                        //this is right hand;
                        if (activecorner->getFace()==(int)Face::FRONT)
                        {
                            if (samecolor)
                            {
                                idealy=1;
                            }
                            else
                                idealy=7;
                        }
                        else if (activecorner->getFace()==(int)Face::LEFT)
                        {
                            if (samecolor)
                            {
                                idealy=3;
                            }
                            else
                                idealy=5;
                        }
                        else if (activecorner->getFace()==(int)Face::BACK)
                        {
                            if (samecolor)
                            {
                                idealy=7;
                            }
                            else
                                idealy=1;
                        }
                        else
                            if (samecolor)
                            {
                                idealy=5;
                            }
                            else
                                idealy=3;
                    }

                    //rotate down until idealy
                    while(activemid->getY()!=idealy)
                        dp();

                    if (activecorner->getY()==0)
                    {
                        //left
                        int face=activecorner->getFace();
                        l(face);
                        d2();
                        lp(face);
                    }
                    else
                    {
                        //right
                        int face=activecorner->getFace();
                        rp(face);
                        d2();
                        r(face);
                    }
                }
                else //midlevel 2 corner is 1 and in the side
                {
                    //just rotate it once facing the face of the white corner
                    if (f2lData.corner1->getY()==0)
                    {
                        int face=f2lData.corner1->getFace();
                        fp(face);
                        dp();
                        f(face);
                    }
                    else
                    {
                        int face=f2lData.corner1->getFace();
                        f(face);
                        d();
                        fp(face);
                    }

                }

            }

        }
        else    //cornerlevel 3; in which case we take action only when its facing down
        {
            if (f2lData.corner1->getFace()==(int)Face::DOWN)
            {
                //this is not optimized YET

                //this must be put on level3 on it sides
                if (midlevel==2)
                {
                    //rotate the
                    while (isDirectlyUnderMadeUp(f2lData.corner1))
                        d();

                    if (f2lData.corner2->getY()==5)
                    {
                        int face=f2lData.corner2->getFace();
                        l(face);
                        dp();
                        lp(face);
                    }
                    else if (f2lData.corner2->getY()==8)
                    {
                        int face=f2lData.corner2->getFace();
                        rp(face);
                        d();
                        r(face);
                    }


                }
                else //midlevel is in 3
                {
                    //are they side by side?
                    while (isDirectlyUnderMadeUp(f2lData.corner1))
                        d();

                    int face=f2lData.corner2->getFace();

                    if (f2lData.corner2->getY()==6)
                    {
                        l(face);
                        dp();
                        lp(face);
                    }
                    else
                    {
                        rp(face);
                        d();
                        r(face);
                    }

                }
            }
        }

        //the corner is now on level 3
        //if the MID is in LEVEL2 at this point. bring it up
        //without hitting corner which is already on 3
        //there is a ptential for optimization here

        //there is a chance that midlevel was moved somehow
        //so reupdate it
        //check the mid level;
        if (f2lData.mid1->getFace()==(int)Face::DOWN || f2lData.mid2->getFace()==(int)Face::DOWN)
        {
            midlevel=3;
        }
        else
            midlevel=2;


        if (midlevel==2)
        {
            //optimize level 2
            //it is in this case that we must do OPPOSITE colors only
            //becuase same color is not possible without messing up the corner
            Element* activecorner;
            Element* activemid;
            Element* oppositemid;
            Element* oppositecorner;

            if (f2lData.corner2->getFace()==(int)Face::DOWN)
            {
                activecorner=f2lData.corner2;
                oppositecorner=f2lData.corner3;
            }
            else
            {
                activecorner=f2lData.corner3;
                oppositecorner=f2lData.corner2;
            }

            //find the active mid
            if (f2lData.mid1->getColor()==activecorner->getColor())
            {
                oppositemid=f2lData.mid1;
                activemid=f2lData.mid2;
            }
            else
            {
                activemid=f2lData.mid1;
                oppositemid=f2lData.mid2;
            }
            int futuremidpos;

            if (activemid->getFace()==(int)Face::FRONT)
            {
                if (activemid->getY()==5)
                    futuremidpos=5;
                else
                    futuremidpos=3;
            }
            else if (activemid->getFace()==(int)Face::LEFT)
            {
                if (activemid->getY()==5)
                    futuremidpos=1;
                else
                    futuremidpos=7;
            }
            else if (activemid->getFace()==(int)Face::BACK)
            {
                if (activemid->getY()==5)
                    futuremidpos=3;
                else
                    futuremidpos=5;
            }
            else
            {
                if (activemid->getY()==5)
                    futuremidpos=7;
                else
                    futuremidpos=1;
            }

            //the oppositemid is the basis of the turn
            //that is we get the opposite face of the opposite mid and turn d until the opposit corner is the same
            int oppositeface;
            if (oppositemid->getFace()==(int)Face::FRONT)
            {
                oppositeface=(int)Face::BACK;
            }
            else if (oppositemid->getFace()==(int)Face::LEFT)
                oppositeface=(int)Face::RIGHT;

            else if (oppositemid->getFace()==(int)Face::RIGHT)
                oppositeface=(int)Face::LEFT;
            else
                oppositeface=(int)Face::FRONT;

            //rotate D until the opposite corner faces the opposite face
            while (oppositecorner->getFace()!=oppositeface)
                d();

            //now do the rotation to bring mid to 3rd layer
            if (activemid->getY()==5)
            {
                int face=activemid->getFace();
                rp(face);
                dp();
                r(face);
            }
            else if (activemid->getY()==3)
            {
                int face=activemid->getFace();
                l(face);
                d();
                lp(face);
            }
        }
    }

    void Rubik::f2lInitActivePieces()
    {
        if (f2lData.mid1->getFace()==(int)Face::DOWN)
        {
            f2lData.activemid=f2lData.mid1;
            f2lData.oppositemid=f2lData.mid2;
        }
        else
        {
            f2lData.activemid=f2lData.mid2;
            f2lData.oppositemid=f2lData.mid1;
        }

        if (f2lData.corner2->getFace()==(int)Face::DOWN)
        {
            f2lData.activecorner=f2lData.corner2;
            f2lData.oppositecorner=f2lData.corner3;
        }
        else
        {
            f2lData.activecorner=f2lData.corner3;
            f2lData.oppositecorner=f2lData.corner2;
        }

    }

    bool Rubik::f2lIsSameActiveColor()
    {
        //questionable
        if (f2lData.activemid->getColor()==f2lData.activecorner->getColor())
            return true;

        else return false;

    }

    bool Rubik::f2lIsSideBySide()
    {

        int midy=f2lData.activemid->getY();
        int sidebyside=false;
        if (f2lData.activecorner->getY()==0)
        {
            if (midy==1 || midy== 3)
                sidebyside=true;
        }
        else if (f2lData.activecorner->getY()==2)
        {
            if (midy==1 || midy== 5)
                sidebyside=true;
        }
        else if (f2lData.activecorner->getY()==6)
        {
            if (midy==3 || midy== 7)
                sidebyside=true;

        }
        else if (f2lData.activecorner->getY()==8)
        {
            if (midy==7 || midy== 5)
                sidebyside=true;
        }

        return sidebyside;
    }

    bool Rubik::f2lIsInsertable()
    {
        bool sidebyside=f2lIsSideBySide();
        bool samecolor=f2lIsSameActiveColor();

        if (!sidebyside || !samecolor)
            return false;

        //there is potential that it can be readily inserted
        int oppmidface;
        int oppcornerface;

        if (f2lData.mid1==f2lData.activemid)
        {
            oppmidface=f2lData.mid2->getFace();
        }
        else
            oppmidface=f2lData.mid1->getFace();

        if (f2lData.corner2==f2lData.activecorner)
        {
            oppcornerface=f2lData.corner3->getFace();
        }
        else
            oppcornerface=f2lData.corner2->getFace();

        if (oppmidface==oppcornerface)
        {
            return true;
        }
        else
            return false;
    }

    void Rubik::f2lUpdateSolvedCorner(CornerPiece corner)
    {
        if (corner==CornerPiece::WGO)
            f2lData.f2lWGOsolved=true;
        if (corner==CornerPiece::WOB)
            f2lData.f2lWOBsolved=true;
        if (corner==CornerPiece::WBR)
            f2lData.f2lWBRsolved=true;
        if (corner==CornerPiece::WRG)
            f2lData.f2lWRGsolved=true;

    }

    bool Rubik::f2lScanPair()
    {
        //valid white corners
        //40 42 46 48
        //wob,wbr,wg0,wrg

        int corners[4]={40,42,46,48};
        CornerPiece cpiece[4]={CornerPiece::WOB,CornerPiece::WBR,CornerPiece::WGO,CornerPiece::WRG};


        CornerPiece piece=CornerPiece::WXX;


        for (int i=0; i< 4; i++)
        {
            //find the corners in turn
            Element* whitecorner=findElement(corners[i]);

            if (whitecorner->getFace()!=(int)Face::UP && whitecorner->getFace()!=(int)Face::DOWN)
            {
                int whiteface=whitecorner->getFace();

                if (whitecorner->getY()==6)
                {
                    switch (whiteface) {
                    case (int)Face::FRONT:
                        if (getElement((int)Face::DOWN,0)->getValue()+3==getElement((int)Face::DOWN,3)->getValue())
                        {
                            //this is insertable
                            piece=cpiece[i];
                        }
                        break;
                    case (int)Face::LEFT:
                        if (getElement((int)Face::DOWN,6)->getValue()+3==getElement((int)Face::DOWN,7)->getValue())
                        {
                            //this is insertable
                            piece=cpiece[i];
                        }
                        break;
                    case (int)Face::RIGHT:
                        if (getElement((int)Face::DOWN,2)->getValue()+3==getElement((int)Face::DOWN,1)->getValue())
                        {
                            //this is insertable
                            piece=cpiece[i];
                        }
                        break;
                    case (int)Face::BACK:
                        if (getElement((int)Face::DOWN,8)->getValue()+3==getElement((int)Face::DOWN,5)->getValue())
                        {
                            //this is insertable
                            piece=cpiece[i];
                        }
                        break;

                    }
                }
                else if (whitecorner->getY()==8)
                {

                    switch (whiteface) {
                    case (int)Face::FRONT:
                        if (getElement((int)Face::DOWN,2)->getValue()+3==getElement((int)Face::DOWN,5)->getValue())
                        {
                            //this is insertable
                            piece=cpiece[i];
                        }
                        break;
                    case (int)Face::LEFT:
                        if (getElement((int)Face::DOWN,0)->getValue()+3==getElement((int)Face::DOWN,1)->getValue())
                        {
                            //this is insertable

                            piece=cpiece[i];

                        }
                        break;
                    case (int)Face::RIGHT:
                        if (getElement((int)Face::DOWN,8)->getValue()+3==getElement((int)Face::DOWN,7)->getValue())
                        {
                            //this is insertable
                            piece=cpiece[i];
                        }
                        break;
                    case (int)Face::BACK:
                        if (getElement((int)Face::DOWN,6)->getValue()+3==getElement((int)Face::DOWN,3)->getValue())
                        {
                            //this is insertable
                            piece=cpiece[i];
                        }
                        break;

                    }

                }
            }

            if (piece!=CornerPiece::WXX)
            {
                f2lInsertPair(piece);
                return true;
            }
        }

        return false;




        /*
        //scan the corners of the YELLOW FACE
        Element* element=getElement(DOWN,0);
        int value=element->getValue();
        int neighbor1=1;
        int neighbor2=3;

        CornerPiece piece=WXX;

        if (getElement(DOWN,neighbor1)->getValue()==value+3 || getElement(DOWN,neighbor2)->getValue()==value+3)
        {
            //insert PAIR in terms of which corner is it
            switch (value)
            {
            case 2:
            case 10:
                //wgo
                piece=WGO;
                break;
            case 12:
            case 20:
                //wrg();
                piece=WRG;
                break;
            case 22:
            case 30:
                //wbr
                piece=WBR;
                break;
            case 32:
            case 0:
                //wob
                piece=WOB;
                break;
            default:
                piece=WXX;
                break;

            }

            //insert here and return true;
            if (piece!=WXX)
            {
                //insert pair here
                f2lInsertPair(piece);

                return true;
            }
        }

        element=getElement(DOWN,2);
        value=element->getValue();
        neighbor1=1;
        neighbor2=5;


        if (getElement(DOWN,neighbor1)->getValue()==value+3 || getElement(DOWN,neighbor2)->getValue()==value+3)
        {
            //insert PAIR in terms of which corner is it
            switch (value)
            {
            case 2:
            case 10:
                //wgo
                piece=WGO;
                break;
            case 12:
            case 20:
                //wrg();
                piece=WRG;
                break;
            case 22:
            case 30:
                //wbr
                piece=WBR;
                break;
            case 32:
            case 0:
                //wob
                piece=WOB;
                break;
            default:
                piece=WXX;
                break;

            }

            //insert here and return true;
            if (piece!=WXX)
            {
                //insert pair here
                f2lInsertPair(piece);

                return true;
            }
        }

        element=getElement(DOWN,6);
        value=element->getValue();
        neighbor1=3;
        neighbor2=7;


        if (getElement(DOWN,neighbor1)->getValue()==value+3 || getElement(DOWN,neighbor2)->getValue()==value+3)
        {
            //insert PAIR in terms of which corner is it
            switch (value)
            {
            case 2:
            case 10:
                //wgo
                piece=WGO;
                break;
            case 12:
            case 20:
                //wrg();
                piece=WRG;
                break;
            case 22:
            case 30:
                //wbr
                piece=WBR;
                break;
            case 32:
            case 0:
                //wob
                piece=WOB;
                break;
            default:
                piece=WXX;
                break;

            }

            //insert here and return true;
            if (piece!=WXX)
            {
                //insert pair here
                f2lInsertPair(piece);

                return true;
            }
        }

        element=getElement(DOWN,8);
        value=element->getValue();
        neighbor1=7;
        neighbor2=5;


        if (getElement(DOWN,neighbor1)->getValue()==value+3 || getElement(DOWN,neighbor2)->getValue()==value+3)
        {


            //insert PAIR in terms of which corner is it
            switch (value)
            {
            case 2:
            case 10:
                //wgo
                piece=WGO;
                break;
            case 12:
            case 20:
                //wrg();
                piece=WRG;
                break;
            case 22:
            case 30:
                //wbr
                piece=WBR;
                break;
            case 32:
            case 0:
                //wob
                piece=WOB;
                break;
            default:
                piece=WXX;
                break;

            }

            //insert here and return true;
            if (piece!=WXX)
            {
                //insert pair here
                f2lInsertPair(piece);

                return true;
            }
        }

        return false;

        */
    }

    void Rubik::f2lInsertPair(CornerPiece corner)
    {
        //assumed that a pair is already existing
        //no checking done here
        int color;
        Element* activecorner;
        Element* oppositecorner;
        int value1;
        int value2;
        int idealy;

        Element* activemid;

        //get the active corner's color
        if (corner==CornerPiece::WGO)
        {
            value1=2;
            value2=10;
        }
        else if (corner==CornerPiece::WRG)
        {
            value1=12;
            value2=20;
        }
        else if (corner==CornerPiece::WBR)
        {
            value1=22;
            value2=30;
        }
        else
        {
            value1=32;
            value2=0;
        }

        if (findElement(value1)->getFace()==(int)Face::DOWN)
        {
            activecorner=findElement(value1);
            activemid=findElement(value1+3);
            oppositecorner=findElement(value2);

        }
        else
        {
            activecorner=findElement(value2);
            activemid=findElement(value2+3);
            oppositecorner=findElement(value1);
        }

        color=activecorner->getColor();

        if (color==(int)Color::ORANGE)
            idealy=3;
        else if (color==(int)Color::GREEN)
            idealy=1;
        else if (color==(int)Color::RED)
            idealy=5;
        else
            idealy=7;

        //here we can do the insertion process
        //position it first
        while (activemid->getY()!=idealy)
            d();

        int face=oppositecorner->getFace();
        int index=oppositecorner->getY();

        if (index==6)
        {
            //insert right hand
            rp(face);
            d(face);
            r(face);

        }
        else if (index==8)
        {
            //insert via left
            l(face);
            dp(face);
            lp(face);
        }

        //
        updateF2LCornerStatus(corner);
    }

    Face Rubik::leftFace(Face refFace)
    {
        switch (refFace) {
        case Face::FRONT:
            return Face::LEFT;
        case Face::LEFT:
            return Face::BACK;
        case Face::BACK:
            return Face::RIGHT;
        case Face::RIGHT:
            return Face::FRONT;
        case Face::UP:
            return Face::LEFT;
        case Face::DOWN:
            return Face::LEFT;
        }
    }

    Face Rubik::rightFace(Face refFace)
    {
        switch (refFace) {
        case Face::FRONT:
            return Face::RIGHT;
        case Face::LEFT:
            return Face::FRONT;
        case Face::BACK:
            return Face::LEFT;
        case Face::RIGHT:
            return Face::BACK;
        case Face::UP:
            return Face::RIGHT;
        case Face::DOWN:
            return Face::RIGHT;

        }
    }

    void Rubik::ollSolveCross()
    {
        //get the YELLOW FACE and check what pattern is FORMED on the top
        int yellowedge[4]={0,0,0,0};
        int counter=0;

        if (this->getElement(5,1)->getColor()==(int)Color::YELLOW)
        {
            yellowedge[0]=1;
            counter++;
        }

        if (this->getElement(5,3)->getColor()==(int)Color::YELLOW)
        {
            yellowedge[1]=1;
            counter++;
        }

        if (this->getElement(5,5)->getColor()==(int)Color::YELLOW)
        {
            yellowedge[2]=1;
            counter++;
        }

        if (this->getElement(5,7)->getColor()==(int)Color::YELLOW)
        {
            yellowedge[3]=1;
            counter++;
        }

        if (counter==0)
        {
            ollDot((int)Face::BACK);
        }
        else if (counter==2)
        {
            //it could either be L or LINE
            if (yellowedge[0]==1 && yellowedge[3]==1)
            {
                //this is a line
                ollLine((int)Face::RIGHT);
            }
            else if (yellowedge[1]==1 && yellowedge[2]==1)
            {
                //this is a line
                ollLine((int)Face::FRONT);
            }
            else if  (yellowedge[0]==1)
            {
                if (yellowedge[1]==1)
                {
                    ollLShape((int)Face::FRONT);
                }
                else if (yellowedge[2]==1)
                {
                    ollLShape((int)Face::RIGHT);
                }
            }
            else if (yellowedge[1]==1)
            {
                if (yellowedge[0]==1)
                {
                    ollLShape((int)Face::FRONT);
                }
                else if (yellowedge[3]==1)
                {
                    ollLShape((int)Face::LEFT);
                }

            }
            else if (yellowedge[2]==1)
            {
                if (yellowedge[0]==1)
                {
                    ollLShape((int)Face::RIGHT);
                }
                else if (yellowedge[3]==1)
                {
                    ollLShape((int)Face::BACK);
                }

            }
            else if (yellowedge[3]==1)
            {
                if (yellowedge[1]==1)
                {
                    ollLShape((int)Face::LEFT);
                }
                else if (yellowedge[2]==1)
                {
                    ollLShape((int)Face::BACK);
                }

            }

        }



    }

    void Rubik::ollLine(int face)
    {
        //f(R U Rp Up) Fp
        f(face);
        l(face);
        d(face);
        lp(face);
        dp(face);
        fp(face);
    }

    void Rubik::ollLShape(int face)
    {
        //fdl, (R U Rp Up) fdlp
        b(face);
        d(face);
        r(face);
        dp(face);
        rp(face);
        bp(face);

    }

    void Rubik::ollDot(int face)
    {
        //F(R, U, Rp, Up) Fp, fdl (R U Rp Up) fdlp
        ollLine(face);
        ollLShape(face);
    }

    void Rubik::ollSolveYellow()
    {
        //count the corners in yellow and find the pattern
        //here to avoid confusion; we must issue an orientation command
        //so that the YELLOW is ON TOP; this way we dont have to reinpterpret the pattern

        //issue an orientation move for yellow here

        int counter=0;
        if (getElement(5,0)->getColor()==(int)Color::YELLOW)
            counter++;
        if (getElement(5,2)->getColor()==(int)Color::YELLOW)
            counter++;
        if (getElement(5,6)->getColor()==(int)Color::YELLOW)
            counter++;
        if (getElement(5,8)->getColor()==(int)Color::YELLOW)
            counter++;

        //find the pattern
        if (counter==1)
        {
            //this is for SUNE and ANTISUNE pattern
            //find that yellow corner facing down
            int index=-1;
            int rightindex;
            int leftindex;

            if (getElement(5,0)->getColor()==(int)Color::YELLOW)
            {
                index=0;
                rightindex=6;
                leftindex=18;
            }
            else if (getElement(5,2)->getColor()==(int)Color::YELLOW)
            {
                index=2;
                rightindex=16;
                leftindex=28;
            }
            else if (getElement(5,6)->getColor()==(int)Color::YELLOW)
            {
                index=6;
                rightindex=36;
                leftindex=8;
            }
            else
            {
                index=8;
                rightindex=26;
                leftindex=38;
            }

            if (getElementAt(rightindex)->getColor()==(int)Color::YELLOW)
            {
                //SUNE
                int face=getElementAt(rightindex)->getFace();
                ollSune(face);

            }
            else
            {
                //ANTISUNE
                int face=getElementAt(leftindex)->getFace();
                ollAntiSune(face);
            }
        }
        else if (counter==0)
        {
            //there are H and PI patterns
            //if there are 2 faces then H
            //if there are 3 different faces then PI
            int faces=0;

            int face=findElement(50)->getFace();
            if (face==0)
                faces=faces | 1;
            else if (face==1)
                faces=faces | 2;
            else if (face==2)
                faces= faces | 4;
            else
                faces=faces | 8;

            face=findElement(52)->getFace();
            if (face==0)
                faces=faces | 1;
            else if (face==1)
                faces=faces | 2;
            else if (face==2)
                faces= faces | 4;
            else
                faces=faces | 8;

            face=findElement(56)->getFace();
            if (face==0)
                faces=faces | 1;
            else if (face==1)
                faces=faces | 2;
            else if (face==2)
                faces= faces | 4;
            else
                faces=faces | 8;

            face=findElement(58)->getFace();
            if (face==0)
                faces=faces | 1;
            else if (face==1)
                faces=faces | 2;
            else if (face==2)
                faces= faces | 4;
            else
                faces=faces | 8;

            //count the faces
            int facecount=0;
            if (faces & 8)
                facecount++;
            if (faces & 4)
                facecount++;
            if (faces & 2)
                facecount++;
            if (faces & 1)
                facecount++;

            if (facecount==2)
            {
                //H
                //just find a valid face in faces
                int face=-1;

                if (faces & 1)
                    face=0;
                else if (faces & 2)
                    face=1;
                else if (faces & 4)
                    face=2;
                else if (faces & 8)
                    face=3;

                ollH(face);
            }
            else
            {
                //PI
                int face=-1;

                //scan the faces without opposite
                if (faces & 1)
                {
                    if (!(faces & 4))
                    {
                        face=0;
                    }
                }

                if (faces &2)
                {
                    if (!(faces & 8))
                        face=1;
                }

                if (faces & 4)
                {
                    if (!(faces  & 1))
                        face=2;
                }

                if (faces & 8)
                {
                    if (!(faces & 2))
                        face=3;
                }

                //the face must be on the left
                //of the actual face
                /*if (face==0)
                {
                    face=FRONT;
                }
                else if (face==1)
                {
                    face=RIGHT;
                }
                else if (face==2)
                {
                    face=BACK;
                }
                else
                {
                    face=LEFT;
                }*/

                if (face==0)
                    d2();
                else if (face==1)
                {
                    d();
                }
                else if(face==3)
                {
                    dp();
                }

                ollPi((int)Face::FRONT);
            }

        }
        else if (counter==2)
        {
            //3 cases; diagonal, t and U
            int face=-1;

            int indeces[2];
            int counter=0;


            int corners[]={50,52,56,58};
            bool isdiagonal=false;

            for (int i=0; i<4; i++)
            {
                if (getElementAt(corners[i])->getColor()==(int)Color::YELLOW)
                {
                    indeces[counter++]=corners[i];
                }
            }

            if (indeces[0]==50)
            {
                if (indeces[1]==58)
                {
                    //this is diagonal
                    isdiagonal=true;
                }
            }
            else if (indeces[0]==52)
            {
                if (indeces[1]==56)
                {
                    isdiagonal=true;
                }
            }
            else if (indeces[0]==56)
            {
                if (indeces[1]==52)
                {
                    isdiagonal=true;
                }
            }
            else if (indeces[0]==58)
            {
                if (indeces[1]==50)
                {
                    isdiagonal=true;
                }
            }

            if (isdiagonal)
            {
                //olldiagonal here
                //just find the correct face
                face=-1;
                switch (indeces[0])
                {
                case 50:
                    if (getElementAt(18)->getColor()==(int)Color::YELLOW)
                    {
                        face=(int)Face::FRONT;
                    }
                    break;
                case 52:
                    if (getElementAt(28)->getColor()==(int)Color::YELLOW)
                    {
                        face=(int)Face::RIGHT;
                    }
                    break;
                case 56:
                    if (getElementAt(8)->getColor()==(int)Color::YELLOW)
                    {
                        face=(int)Face::LEFT;
                    }
                    break;
                case 58:
                    if (getElementAt(38)->getColor()==(int)Color::YELLOW)
                    {
                        face=(int)Face::BACK;
                    }
                    break;
                }

                if (face==-1)
                {

                    switch (indeces[1])
                    {
                    case 50:
                        if (getElementAt(18)->getColor()==(int)Color::YELLOW)
                        {
                            face=(int)Face::FRONT;
                        }
                        break;
                    case 52:
                        if (getElementAt(28)->getColor()==(int)Color::YELLOW)
                        {
                            face=(int)Face::RIGHT;
                        }
                        break;
                    case 56:
                        if (getElementAt(8)->getColor()==(int)Color::YELLOW)
                        {
                            face=(int)Face::LEFT;
                        }
                        break;
                    case 58:
                        if (getElementAt(38)->getColor()==(int)Color::YELLOW)
                        {
                            face=(int)Face::BACK;
                        }
                        break;
                    }

                }

                ollDiagonalCorner(face);

            }
            else
            {
                //not diagonal so it could either be T or U
                int testindex=-1;
                if (indeces[0]==50)
                {
                    if (indeces[1]==52)
                    {
                        face=(int)Face::FRONT;
                        testindex=28;
                    }
                    else if (indeces[1]==56)
                    {
                        face=(int)Face::LEFT;
                        testindex=18;
                    }
                }
                else if (indeces[0]==52)
                {
                    if (indeces[1]==58)
                    {
                        face=(int)Face::RIGHT;
                        testindex=38;
                    }
                    else if (indeces[1]==50)
                    {
                        face=(int)Face::FRONT;
                        testindex=28;
                    }
                }
                else if (indeces[0]==56)
                {
                    if (indeces[1]==50)
                    {
                        face=(int)Face::LEFT;
                        testindex=18;
                    }
                    else if (indeces[1]==58)
                    {
                        face=(int)Face::BACK;
                        testindex=8;
                    }
                }
                else if (indeces[0]==58)
                {
                    if (indeces[1]==52)
                    {
                        face=(int)Face::RIGHT;
                        testindex=38;
                    }
                    else if (indeces[1]==56)
                    {
                        face=(int)Face::BACK;
                        testindex=8;
                    }
                }

                if (getElementAt(testindex)->getColor()==(int)Color::YELLOW)
                {
                    face=getElementAt(testindex)->getFace();
                    ollT(face);
                }
                else
                {

                    //rotate the DOWN until the wrong corners are facing front
                    while(true)
                    {
                        if (getElementAt(16)->getColor()==(int)Color::YELLOW)
                            break;

                        d();
                    }

                    ollU((int)Face::FRONT);
                }
            }

        }



    }

    void Rubik::ollSune(int face)
    {
        // R U  Rp U R U2 Rp //right version
        l(face);
        d(face);
        lp(face);
        d(face);
        l(face);
        d2();
        lp(face);
    }

    void Rubik::ollAntiSune(int face)
    {
        // Lp Up L Up LP U2 L //left version
        rp(face);
        dp(face);
        r(face);
        dp(face);
        rp(face);
        d2();
        r(face);

    }

    void Rubik::ollH(int face)
    {
        //F (R U Rp Up)3 Fp

        f(face);

        for (int i=0; i<3; i++)
        {
            l(face);
            d(face);
            lp(face);
            dp(face);
        }

        fp(face);
    }

    void Rubik::ollPi(int face)
    {
        //R U2, (R2, Up R2 Up R2) U2 R
        l();
        d2();
        l2();
        dp();
        l2();
        dp();
        l2();
        d2();
        l();
    }

    void Rubik::ollDiagonalCorner(int face)
    {
        //X rotate so that WHITE is FACING YOU
        //(Rp U R Dp) (Rp UP R D)
        lp(face);
        f(face);
        l(face);
        bp(face);
        lp(face);
        fp(face);
        l(face);
        b(face);

    }

    void Rubik::ollT(int face)
    {
        //x (L U Rp Up) (Lp U R Up)
        r(face);
        f(face);
        lp(face);
        fp(face);
        rp(face);
        f(face);
        l(face);
        fp(face);

    }

    void Rubik::ollU(int face)
    {
    // R2 D (Rp U2 R) Dp (Rp U2 Rp)
    l2();
    u(face);
    lp(face);
    d2();
    l(face);
    up(face);
    lp(face);
    d2();
    lp(face);
    }

    void Rubik::pllSolveCorners()
    {
        //check the last layer corner and check how many are paired exactly
        //if there are no pairs, do diagonal swap
        //else do adjacent corner swap
        int counter=0;
        int tempindex=-1;;

        if (getElement(0,6)->getColor()==getElement(0,8)->getColor())
        {
            counter++;
            tempindex=6;
        }
        if (getElement(1,6)->getColor()==getElement(1,8)->getColor())
        {
            counter++;
            tempindex=16;
        }
        if (getElement(2,6)->getColor()==getElement(2,8)->getColor())
        {
            counter++;
            tempindex=26;
        }
        if (getElement(3,6)->getColor()==getElement(3,8)->getColor())
        {
            counter++;
            tempindex=36;
        }

        if (counter==0)
        {
            //diagonal swap
            pllDiagonalSwap();
        }
        else if (counter==1)
        {
            //adj corner swap

            //rotate down until the tempindex is on the right face
            Element* el=getElementAt(tempindex);
            while (el->getFace()!=(int)Face::RIGHT)
                d();

            pllAdjacentCornerSwap();

            //rotate d until all corners are aligned to their faces
            //just use the el so that the el color must match face
            while (el->getColor()!=el->getFace())
                d();
        }
        //else it is already solved

    }

    void Rubik::pllDiagonalSwap()
    {
        //f r up rp up r u rp fp r u rp up rp f r fp
        f();
        l();
        dp();
        lp();
        dp();
        l();
        d();
        lp();
        fp();
        l();
        d();
        lp();
        dp();
        lp();
        f();
        l();
        fp();



    }

    void Rubik::pllAdjacentCornerSwap()
    {
        //r u rp up rp f r2 up rp up r u rp fp
        l();
        d();
        lp();
        dp();
        lp();
        f();
        l2();
        dp();
        lp();
        dp();
        l();
        d();
        lp();
        fp();

    }

    void Rubik::pllSolveMiddles()
    {
        //check how many edges piece are already solved
        int counter=0;
        int tempindex=-1;

        if (getElement(0,7)->getColor()==getElement(0,6)->getColor())
        {
            counter++;
            tempindex=7;
        }
        if (getElement(1,7)->getColor()==getElement(1,6)->getColor())
        {
            counter++;
            tempindex=17;
        }
        if (getElement(2,7)->getColor()==getElement(2,6)->getColor())
        {
            counter++;
            tempindex=27;
        }

        if (getElement(3,7)->getColor()==getElement(3,6)->getColor())
        {
            counter++;
            tempindex=37;
        }

        if (counter==1)
        {
            //edge cycle 1 and 2
            Element* el=getElementAt(tempindex);

            while (el->getFace()!=(int)Face::BACK)
                d();

            //which side will move to the other side
            if (getElement(0,7)->getColor()==getElement(2,6)->getColor())
            {
                //use the cycle1
                pllEdgeCycle1();
            }
            else
            {
                //use cycle2
                pllEdgeCycle2();
            }

        }
        else if (counter==0)
        {
            if (getElement(0,7)->getColor()==getElement(2,6)->getColor())
            {
                pllOppositeEdgeSwap();
            }
            else
            {
                /*
                if (getElement(0,7)->getColor()!=getElement(0,4)->getColor()
                        && getElement(1,7)->getColor()!=getElement(1,4)->getColor()
                        && getElement(2,7)->getColor()!=getElement(2,4)->getColor()
                        && getElement(3,7)->getColor()!=getElement(3,4)->getColor()
                        )
                    dp();
                    */
                pllAdjacentEdgeSwap();
            }
        }

        //align it finally
        switch (findElement(7)->getFace())
        {
        case 1:
            dp();
            break;
        case 2:
            d2();
            break;
        case 3:
            d();
            break;
        }
    }

    void Rubik::pllEdgeCycle1()
    {
        //r  up r u r u r up rp up r2
        l();
        dp();
        l();
        d();
        l();
        d();
        l();
        dp();
        lp();
        dp();
        l2();

    }

    void Rubik::pllEdgeCycle2()
    {
        //lp u lp up lp up lp u l u l2
        rp();
        d();
        rp();
        dp();
        rp();
        dp();
        rp();
        d();
        r();
        d();
        r2();
    }

    void Rubik::pllOppositeEdgeSwap()
    {
        //m2 up m2 u2 m2 up m2
        l2();
        r2();

        up();

        l2();
        r2();

        d2();


        l2();
        r2();

        up();

        l2();
        r2();

    }

    void Rubik::pllAdjacentEdgeSwap()
    {
        //mp up m2 up m2 up mp u2 m2
        lp();
        r();

        fp();

        l2();
        r2();

        bp();

        l2();
        r2();

        fp();

        lp();
        r();

        u2();

        l2();
        r2();
    }


    void Rubik::insertF2LCorner(CornerPiece corner)
    {

        bool insertable=false;
        bool samecolor=false;
        bool sidebyside=false;

        samecolor=f2lIsSameActiveColor();
        sidebyside=f2lIsSideBySide();
        insertable=f2lIsInsertable();


        //if insertable; insert it and exit
        //NOTE: active corner is the element that faces the DOWN face
        //the oppositemid is the element that is the opposite  of the activemid

        if (insertable)
        {
            int targetface=-1;
            int activecolor=f2lData.activemid->getColor();

            if (activecolor==(int)Color::GREEN)
            {
                targetface=(int)Face::FRONT;
            }
            else if (activecolor==(int)Color::ORANGE)
            {
                targetface=(int)Face::LEFT;
            }
            else if (activecolor==(int)Color::RED)
            {
                targetface=(int)Face::RIGHT;
            }
            else if (activecolor==(int)Color::BLUE)
            {
                targetface=(int)Face::BACK;
            }

            //rotate the DOWN FACE so that the
            //opposite mid is facing correct targetface

            while (targetface != f2lData.oppositemid->getFace())
            {
                dp();
            }

            //the L is now formed ready for insertion
            //where is the active corner y located?
            bool lefthanded=false;

            if (targetface==(int)Face::LEFT)
            {
                if (f2lData.activecorner->getY()==0)
                    lefthanded=true;
            }
            else if (targetface==(int)Face::FRONT)
            {
                if (f2lData.activecorner->getY()==2)
                    lefthanded=true;
            }
            else if (targetface==(int)Face::RIGHT)
            {
                if (f2lData.activecorner->getY()==8)
                    lefthanded=true;
            }
            else if (targetface==(int)Face::BACK)
            {
                if (f2lData.activecorner->getY()==6)
                    lefthanded=true;
            }

            //insert it now
            if (lefthanded)
            {
                l(targetface);
                dp(targetface);
                lp(targetface);
            }
            else
            {
                //right handed
                rp(targetface);
                d(targetface);
                r(targetface);
            }

            return ;
        }


        //if sidebyside then it should be broken up
        //where is the oopositemid facing
        //from the oppmid gt the left and right elements of it to check
        //where the corner is
        if (sidebyside) //same color or not and not insertable yet
        {
            Element* temp;
            bool righthanded=false;

            temp=getElement(f2lData.oppositemid->getFace(),8);
            if (temp==f2lData.corner1 || temp==f2lData.corner2 || temp==f2lData.corner3 )
                righthanded=true;

            //from this ; we will do a series of mode to break them apart
            //rotate the DOWN FACE until WE FIND an OPPOSITE DIAGONAL
            //that is not made up
            //here we will look at the active corner (the corner where its facing yellow)

            // there should be 2 cases here
            // 1 is when the white is beside the mid1and one is perpendicualr to the mid1

            while (isDirectlyUnderMadeUp(f2lData.activecorner))
            {
                d();
            }

            if (f2lData.corner1->getFace()==f2lData.oppositemid->getFace())
            {

                int face=f2lData.oppositemid->getFace();

                if (righthanded)
                {
                    f(face);
                    dp();
                    fp(face);
                }
                else
                {
                    fp(face);
                    d();
                    f(face);
                }
            }
            else
            {


                //at this point we can now do the routine
                int face=f2lData.oppositemid->getFace();
                if (righthanded)
                {
                    rp(face);
                    d2();
                    r(face);
                }
                else
                {
                    l(face);
                    d2();
                    lp(face);
                }
            }

            //update the active
            f2lInitActivePieces();

            //update if its still same color
            if (f2lData.activecorner->getColor()==f2lData.activemid->getColor())
                samecolor=true;
            else
                samecolor=false;

        }

        //this is wasteful but we must rotate down a few times so that the diagonbal oppiste is free
        while (isOppositeDiagonalMadeUp(f2lData.corner1))
            d();


        //AT this point they are SEPARATED
        //we should now DO the finger test
        //after this test; the corner is ready for insertion
        //we also now if they are the same color or not
        int whitecornerface=f2lData.corner1->getFace();
        int y=f2lData.corner1->getY();

        int idealmidy=-1;

        if (whitecornerface==(int)Face::LEFT)
        {
            if (y==6)
            {
                if (samecolor)
                {
                    idealmidy=7;
                }
                else
                    idealmidy=1;
            }
            else if (y==8)
            {
                if (samecolor)
                    idealmidy=1;
                else
                    idealmidy=7;
            }
        }
        else if (whitecornerface==(int)Face::FRONT)
        {
            if (y==6)
            {
                if (samecolor)
                {
                    idealmidy=3;
                }
                else
                    idealmidy=5;
            }
            else if (y==8)
            {
                if (samecolor)
                    idealmidy=5;
                else
                    idealmidy=3;
            }
        }
        else if (whitecornerface==(int)Face::RIGHT)
        {
            if (y==6)
            {
                if (samecolor)
                {
                    idealmidy=1;
                }
                else
                    idealmidy=7;
            }
            else if (y==8)
            {
                if (samecolor)
                    idealmidy=7;
                else
                    idealmidy=1;
            }
        }
        else if (whitecornerface==(int)Face::BACK)
        {
            if (y==6)
            {
                if (samecolor)
                {
                    idealmidy=5;
                }
                else
                    idealmidy=3;
            }
            else if (y==8)
            {
                if (samecolor)
                    idealmidy=3;
                else
                    idealmidy=5;
            }
        }


        if (f2lData.activemid->getY()!=idealmidy)
        {
            //we need to place it in the correct setup
            //hide the white and then rotate to ideal

            //we can now hide the white
            //facing the WHITECORNER FACE
            bool cw=false;
            if (y==8)
            {
                fp(whitecornerface);
                cw=false;
            }
            else
            {
                f(whitecornerface);
                cw=true;
            }

            //rotate DOWN until the ACTIVEMID is in idealmidy
            while (f2lData.activemid->getY()!=idealmidy)
            {
                dp();
            }

            //return the face back up
            if (cw)
            {
                fp(whitecornerface);
            }
            else
                f(whitecornerface);

            //at this point; if they are same color then
            //they are already sidebyside
            //else do the different color routine below

            //auto insertion should be done here and then exit
            if (samecolor)
            {

            }


        }

        //if they are the same color; then they are already paired
        //else we need to do one more turn
        if (samecolor)
        {
            //at this point, the corner is now paired
            //insert it into the slot
            //first match the active MID to the correct column
            if (f2lData.activemid->getColor()==(int)Color::GREEN)
                idealmidy=1;
            else if (f2lData.activemid->getColor()==(int)Color::RED)
                idealmidy=5;
            else if (f2lData.activemid->getColor()==(int)Color::BLUE)
                idealmidy=7;
            else if (f2lData.activemid->getColor()==(int)Color::ORANGE)
                idealmidy=3;

            while (f2lData.activemid->getY()!=idealmidy)
                d();

            //now the pair is aligned
            //check the left or righthandedness of the position
            //in particular, the location of the oppositecorner
            //with respect to the FACE Its currently facing
            y=f2lData.oppositecorner->getY();
            int face=f2lData.oppositecorner->getFace();

            if (y==6)
            {
                //this is right hand
                rp(face);
                d(face);
                r(face);

            }
            else if (y==8)
            {
                //this is left hand
                l(face);
                dp(face);
                lp(face);

            }
        }
        else
        {
            //not the same color ; ready for insertion in a few steps
            //first align the active corner directly under the point of insertion
            //we can do this by aligning the OPPOSITE CORNER to be the same as the COLOR of the face
            int idealface=-1;

            if (f2lData.oppositecorner->getColor()==(int)Color::ORANGE)
                idealface=(int)Face::LEFT;
            else if (f2lData.oppositecorner->getColor()==(int)Color::GREEN)
                idealface=(int)Face::FRONT;
            else if (f2lData.oppositecorner->getColor()==(int)Color::RED)
                idealface=(int)Face::RIGHT;
            else
                idealface=(int)Face::BACK;

            while (f2lData.oppositecorner->getFace()!=idealface)
                d();

            //get the position of the opposite color
            if (f2lData.oppositecorner->getY()==8)
            {
                //right handed insertion
                rp(idealface);
                dp(idealface);
                r(idealface);
            }
            else
            {
                //left handed insertion
                l(idealface);
                d(idealface);
                lp(idealface);
            }
        }


        //done

    }

    void Rubik::yellowCornerPosPattern(int face)
    {
        d();
        l(face);
        dp();
        rp(face);
        d();
        lp(face);
        dp();
        r(face);

    }

    void Rubik::yellowCornerPlacePattern(int face)
    {
        l(face);
        fp(face);
        lp(face);
        f(face);
        l(face);
        fp(face);
        lp(face);
        f(face);
    }

    void Rubik::solveYellowCrossOrdering()
    {

        //the correct sequence of the values read counterclockwise is
        //51
        //53
        //57
        //55;

        Element* edge1;
        Element* edge3;
        Element* edge5;
        Element* edge7;

        //in order to simplfy the logic of alignment
        //we will find the ELEMENT 5,1 and rotate the cube to align 51 to its correct position
        //in that way 51 is always fixed a


        //at this point element 51 is in the correct position
        //which will be used as reference
        bool ordered=false;

        while (!ordered)
        {
            //align element 51
            Element* ref=this->findElement(51);
            int row=ref->getY()/3;
            int col=ref->getY()%3;

            if (col==0)
                d();
            if (col==2)
                dp();
            else
            {
                if (row==2)
                {
                    d();
                    d();
                }
            }

            //get the elements of the corner
            edge1=getElement(5,1);
            edge3=getElement(5,3);
            edge7=getElement(5,7);
            edge5=getElement(5,5);

            int values[4];
            values[0]=edge1->getValue();
            values[1]=edge3->getValue();
            values[2]=edge7->getValue();
            values[3]=edge5->getValue();


            if (values[0]==51 && values[1]==53 && values[2]==57 && values[3]==55)
            {
                ordered=true;
                break;
            }
            if (!ordered)
            {
                //apply BackRight (BR) pattern on this to make it ordered
                //there can only be 2 elements that are in the correct position
                //but they can either be opposite to each other or
                //a corner from each other
                //if its opposite, APPLY BR ONCE
                //if its L, find the correct FACE , rotate to align and apply BR pattern

                //find the 2 sequence that are valid
                //1,3,7,5,1
                //if this is found then we can apply BR right away
                //else we need to do BR 2 times
                bool found=false;

                if (values[1]==53 || values[3]==55)
                {
                    found=true;
                }
                else if (values[1]==57)
                {
                    if (values[2]==55)
                        found=true;
                }


                //is it found or not
                int face;
                if (!found)
                {
                    //this means that the opposites have been found
                    //find the opposites are and apply a BR to the face perpendicular to that LINE

                    if (values[2]==57)
                    {
                        face=(int)Face::LEFT;
                    }
                    else
                        face=(int)Face::FRONT;

                    this->yellowCrossBRMatchPattern(face);
                }
                else
                {
                    //we found a valid L SEQUENCE

                    Element* ref=this->findElement(51);
                    int row=ref->getY()/3;
                    int col=ref->getY()%3;

                    if (col==0)
                        d();
                    if (col==2)
                        dp();
                    else
                    {
                        if (row==2)
                        {
                            d();
                            d();
                        }
                    }

                    //get the elements of the edges
                    edge1=getElement(5,1);
                    edge3=getElement(5,3);
                    edge7=getElement(5,7);
                    edge5=getElement(5,5);

                    int values[4];
                    values[0]=edge1->getValue();
                    values[1]=edge3->getValue();
                    values[2]=edge7->getValue();
                    values[3]=edge5->getValue();

                    //51 53
                    if (values[1]==53)
                    {
                        this->yellowCrossBRMatchPattern((int)Face::RIGHT);
                    }

                    //55 51
                    if (values[3]==55)
                    {
                        this->yellowCrossBRMatchPattern((int)Face::BACK);
                    }

                    //57 55
                    if (values[1]==57 && values[2]==55)
                    {
                        this->yellowCrossBRMatchPattern((int)Face::FRONT);
                    }

                    if (values[2]==53 && values[3]==57)
                    {
                        this->yellowCrossBRMatchPattern((int)Face::LEFT);
                    }
                }
            }
            else
            {
                //else this is ordered; all we have to do is allign it to correct faces
                break;
            }

        }//end while not ordered
    }

    void Rubik::solveYellowCorner()
    {
        //at this point; we should either have 0, 1 or 4 yellow corner pieces that are in the correct corners
        //this method will make sure that we have all 4 in the correct corners not considering the direction they are facing

        while (true)
        {
            int corner0=getYellowCornerValue(0);
            int corner2=getYellowCornerValue(2);
            int corner6=getYellowCornerValue(6);
            int corner8=getYellowCornerValue(8);

            int count=0;
            int refcorner=0;

            if (corner0==74)
            {
                refcorner=(int)Face::FRONT;
                count++;
            }
            if (corner2==96)
            {
                refcorner=(int)Face::RIGHT;
                count++;
            }
            if (corner6==100)
            {
                refcorner=(int)Face::LEFT;
                count++;
            }
            if (corner8==122)
            {
                refcorner=(int)Face::BACK;
                count++;
            }

            if (count==0)
            {
                this->yellowCornerPosPattern((int)Face::FRONT);
            }
            else if (count==1)
            {
                this->yellowCornerPosPattern(refcorner);

            }
            else if (count==4)
                break;  //we are done

        }
    }

    void Rubik::solveYellowFinal()
    {
        for (int i=0; i < 4; i++)
        {
            //get element on top right 5 0

            while (true)
            {
                if (getElement(5,0)->getColor()==(int)Color::YELLOW)
                {
                    break;
                }
                else
                {
                    this->yellowCornerPlacePattern((int)Face::FRONT);
                }
            }

            d();
        }

        //align element 51
        Element* ref=this->findElement(51);
        int row=ref->getY()/3;
        int col=ref->getY()%3;

        if (col==0)
            d();
        if (col==2)
            dp();
        else
        {
            if (row==2)
            {
                d();
                d();
            }
        }

    }

    void Rubik::solveYellowLayer()
    {
        ollSolveCross();
        ollSolveYellow();
        pllSolveCorners();
        pllSolveMiddles();

    }

    void Rubik::solveF2L(CornerPiece corner)
    {
        f2lInitialize(corner);
        f2lLevel3V2();
        f2lInitActivePieces();
        insertF2LCorner(corner);
        updateF2LCornerStatus(corner);
    }


    int Rubik::getYellowCornerValue(int corner)
    {
        int sum=0;
        switch (corner)
        {
        case 0:
            sum=getElement(5,0)->getValue();
            sum+=getElement(0,8)->getValue();
            sum+=getElement(1,6)->getValue();
            break;
        case 2:
            sum=getElement(5,2)->getValue();
            sum+=getElement(1,8)->getValue();
            sum+=getElement(2,6)->getValue();
            break;
        case 6:
            sum=getElement(5,6)->getValue();
            sum+=getElement(0,6)->getValue();
            sum+=getElement(3,8)->getValue();
            break;
        case 8:
            sum=getElement(5,8)->getValue();
            sum+=getElement(2,8)->getValue();
            sum+=getElement(3,6)->getValue();
            break;

        }

        return sum;
    }

    void Rubik::yellowCrossBRMatchPattern(int face)
    {
        l(face);
        d();
        lp(face);
        d();
        l(face);
        d();
        d();
        lp(face);
        d();
    }

    MoveArray::MoveArray()
    {
        movecount=0;
        for (int i=0; i<200; i++)
            moves[i]=-1;
    }

    int MoveArray::optimizeMoves()
    {
        eliminateTriples();
        eliminateOpposites();
        eliminateDoubles();

        return movecount;
    }

    void MoveArray::eliminateTriples()
    {
        while (true)
        {
            int count=0;
            int current=-1;
            int index=-1;
            bool ismoved=false;

            for (int i=0; i< movecount ; i++)
            {
                if (current==moves[i])
                {
                    count++;

                    if (count==2)
                    {
                        //we have to replace the move at the currentindex
                        //with the opposite move of current
                        //NOTE: even moves are the clockwise moves
                        //odd are the counterpart
                        if (current<12)		//quarter moves only
                        {
                            if (current % 2==1)
                            {
                                //this is odd
                                moves[index]=current-1;

                                //replace the next 2 with negative 1 moves
                                moves[index+1]=-1;
                                moves[index+2]=-1;
                            }
                            else
                            {
                                moves[index]=current+1;

                                moves[index+1]=-1;
                                moves[index+2]=-1;
                            }
                        }
                        else
                        {
                            //half moves and they are triples? just retain the first move
                            moves[index+1]=-1;
                            moves[index+2]=-1;
                        }

                        current=-1;
                        ismoved=true;
                    }
                }
                else
                {
                    index=i;
                    count=0;
                    current=moves[i];
                }
            }

            if (ismoved)
            {
                //adjust the moves list by eliminating all negatives out of the list
                index=0;
                int j=0;
                int negcount=0;
                while (j<movecount)
                {
                    if (moves[j]!=-1)
                    {
                        //copy that move to the index
                        int temp=moves[j];
                        moves[index++]=temp;
                    }
                    else
                        negcount++;

                    j++;
                }

                //adjust the move count
                movecount-=negcount;
            }
            else
                break;
        }
    }
    void MoveArray::eliminateOpposites()
    {
        while (true)
        {
            bool ismoved=false;
            for (int i=0; i<movecount-1; i++)
            {
                if (moves[i]<12)		//quarter moves onle
                {
                    if (moves[i]%2==0)
                    {
                        if (moves[i+1]==moves[i]+1)
                        {
                            //the two must be eliminated
                            moves[i]=-1;
                            moves[i+1]=-1;

                            ismoved=true;

                            //increment i by  2
                            i+=2;
                        }
                    }
                    else
                    {
                        if (moves[i+1]==moves[i]-1)
                        {
                            moves[i]=-1;
                            moves[i+1]=-1;

                            ismoved=true;

                            //increment i by  2
                            i+=2;
                        }
                    }
                }
                //else
                //{
                    //half moves; there is really nothing to do
                //}
            }

            if (ismoved)
            {
                //adjust the moves list by eliminating all negatives out of the list
                int index=0;
                int j=0;
                int negcount=0;
                while (j<movecount)
                {
                    if (moves[j]!=-1)
                    {
                        //copy that move to the index
                        int temp=moves[j];
                        moves[index++]=temp;
                    }
                    else
                        negcount++;

                    j++;
                }
                //adjust the move count
                movecount-=negcount;
            }
            else
                break;


        }

    }
    void MoveArray::eliminateDoubles()
    {
        while (true)
        {
            int current=-1;
            bool ismoved=false;

            for (int i=0; i<movecount;i++)
            {
                if (current==moves[i])
                {
                    if (current<12)	//quarter move
                    {
                        if (current%2==1)
                        {
                            //this is odd
                            moves[i-1]=(current>>1)+12;
                            moves[i]=-1;
                            //example; if this is dp; then its converted to d (ie -1) then added 12 to prduce d2
                        }
                        else
                        {
                            //this is even
                            moves[i-1]=(current>>1)+12;
                            moves[i]=-1;
                        }

                        ismoved=true;
                        current=-1;
                    }
                    else
                    {
                        //half moves;just eliminate both like L2 and L2 should be nothing
                        moves[i-1]=-1;
                        moves[i]=-1;

                        ismoved=true;
                        current=-1;
                    }
                }
                else
                {
                    current=moves[i];
                }

            }

            if (ismoved)
            {
                int index=0;
                int j=0;
                int negcount=0;
                while (j<movecount)
                {
                    if (moves[j]!=-1)
                    {
                        //copy that move to the index
                        int temp=moves[j];
                        moves[index++]=temp;
                    }
                    else
                        negcount++;

                    j++;
                }

                //adjust the move count
                movecount-=negcount;

            }
            else
                break;

        }

    }

    bool Rubik::areColorsOK()
    {
        int colorcounter[6]={0,0,0,0,0,0};

        for (int i=0;i<6;i++)
        {
            for (int j=0; j<9;j++)
            {
                colorcounter[elements[i][j]->getColor()]++;
            }
        }

        //counter must be 9 each
        for (int i=0;i<6;i++)
        {
            if (colorcounter[i]!=9)
                return false;

        }

        return true;
    }

    int Rubik::getErroredColors()
    {
        return colorError;
    }

    void Rubik::doMove(int move)
    {
        switch (move)
        {
        case (int)Moves::L:
            l();
            break;
        case (int)Moves::Lp:
            lp();
            break;
        case (int)Moves::R:
            r();
            break;
        case (int)Moves::Rp:
            rp();
            break;
        case (int)Moves::F:
            f();
            break;
        case (int)Moves::Fp:
            fp();
            break;
        case (int)Moves::B:
            b();
            break;
        case (int)Moves::Bp:
            bp();
            break;
        case (int)Moves::U:
            u();
            break;
        case (int)Moves::Up:
            up();
            break;
        case (int)Moves::D:
            d();
            break;
        case (int)Moves::Dp:
            dp();
            break;
        case (int)Moves::L2:
            l2();
            break;
        case (int)Moves::F2:
            f2();
            break;
        case (int)Moves::R2:
            r2();
            break;
        case (int)Moves::B2:
            b2();
            break;
        case (int)Moves::U2:
            u2();
            break;
        case (int)Moves::D2:
            d2();
            break;
        default:
            break;
        }
    }


}//namespace



