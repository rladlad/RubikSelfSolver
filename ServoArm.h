#pragma once
#include <cstdint>
#include "globals.h"

class ServoArm
{
public:
	ServoArm(uint8_t pin, uint32_t freq=50, uint8_t resolution=12);

	void turnOn();		//attach the pwm signal to the servo
	void turnOff();		//detach the pwm signal to the servo

	bool isMoving() { return _isMoving; };	//is the servo moving?

	void rotate90CW();	//rotate the servo 90 degrees clockwise
	void rotate90CCW();	//rotate the servo 90 degrees counter-clockwise
	void rotate180CW();	//rotate the servo 180 degrees clockwise
	void rotate180CCW();	//rotate the servo 180 degrees counter-clockwise

	void setCurrentAngle(double angle) {_currentAngle = angle;	};	//set the current angle of the servo	
	void evaluateMove(double currentAngle);	//this function should be called per frame to evalualate a moving servo if it has reached its target angle
	
private:
	bool _isAttached{ false };
	int _servoPin;				//the pin the servo is attached to
	uint32_t _servoFreq;		//the frequency of the servo signal
	uint8_t _servoResolution;	//the resolution of the servo signal (in bits)

	bool _isMoving{ false };	//is the servo moving?	

	//angles
	double _currentAngle{ 0.0 };	//current angle of the servo
	double _targetAngle{ 0.0 };		//target angle of the servo
	RotationDirection _direction{RotationDirection::None };	//direction of the rotation

	const double _errorMargin{ 0.5 };	//error margin for angle comparison
};

