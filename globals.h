/*
 * globals.h
 *
 *  Created on: Feb 18, 2020
 *      Author: Ramil-Laptop
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#ifdef __cplusplus
extern "C" {
#endif

//THIS are SOFTWARE MOVES(ie to solve the cube)
enum class  Moves
{
    L=0,
    Lp=1,
    F=2,
    Fp=3,
    R=4,
    Rp=5,
    B=6,
    Bp=7,
		U=8,
    Up=9,
    D=10,
    Dp=11,
    L2=12,
    F2=13,
    R2=14,
    B2=15,
    U2=16,
    D2=17
};


enum class Face
{
    LEFT=0,
    FRONT,
    RIGHT,
    BACK,
    UP,
    DOWN
};

enum class CubeOrientation
{
	//WHITE,ORANGE orientation
	//WHITE FACE*10+ORANGE FACE
	UL=40,
	UF,
	UR,
	UB,
	DL=50,
	DF,
	DR,
	DB,
	FL=10,
	FU=14,
	FR=12,
	FD=15,
	BL=30,
	BU=34,
	BRX=32, //used to be BR 
	BD=35,
	LU=4,
	LB=3,
	LD=5,
	LF=1,
	RU=24,
	RB=23,
	RD=25,
	RF=21
};

//used to get equivalent moves
enum class RotationMove
{
	Turn_CW,
	Turn_CCW,
	HALFTURN,
	CLOSE,
	OPEN,
	Axial_CW,
	Axial_CCW,
	Axial_HALFTURN,
	Axial_OPEN,
	Axial_CLOSE
};

//enum for the bot arms
enum class BOT_Arm
{
	NoArm=0,
	LeftArm,
	FrontArm,
	RightArm,
	BackArm

};


enum class RotationDirection
{
	None=0,
	Clockwise=1,
	CounterClockwise=2
};



#ifdef __cplusplus
}
#endif



#endif /* GLOBALS_H_ */
