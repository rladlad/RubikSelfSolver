#pragma once
#include <cstdint>
#include "globals.h"
#include <math.h>

class ServoArm
{
public:
	ServoArm(uint8_t pin, uint32_t freq=50, uint8_t resolution=16);

	void attach();		//attach the pwm signal to the servo
	void detach();		//detach the pwm signal to the servo
	void stop();			//stop the motor (1500us)

	bool isMoving() { return _isMoving; };	//is the servo moving?

					

	//use rotateBy within the function
	void rotate90CW();	//rotate the servo 90 degrees clockwise
	void rotate90CCW();	//rotate the servo 90 degrees counter-clockwise
	void rotate180();	//rotate the servo 180 degrees clockwise


	void setRotationSpeedCW(uint32_t us){_rotationSpeedCW=us;};	//in micreosendos ; 1500 is the middle
	void setRotationSpeedCCW(uint32_t us){_rotationSpeedCCW=us;}; //should be less the 1500 

	void setCurrentAngle(double angle) {_currentAngle = angle;};	//set the current angle of the servo	
	void update();	//this function should be called per frame to update the servos position relative to its target

	float getCurrentAngle(){return _currentAngle;};
	float getTargetAngle(){return _targetAngle;};
	void 	setReferenceAngle(float angle);	//called during startup to set the REFERENCE/REST angle

protected:
	float angleDiff(float current, float target) {
  	  float diff = fmod((target - current + 540.0), 360.0) - 180.0;
    	return diff;  // Range: -180 to +180 degrees
	}
	void rotateTo(float target);
	int pulseToDuty(int micros) {
			// Converts microsecond pulse 
			return (micros * 1<<_servoResolution) / (1000000/_servoFreq); 
	}

	//check if the internal angles are within tolerance
	bool withinTolerance() {
    // Wrap difference into 0–360
    float diff = fmod((_currentAngle - _targetAngle + 360.0f), 360.0f);

    // If difference is less than tol, or within tol of 360
    return (diff <= _errorMargin || diff >= 360.0f - _errorMargin);
	}

	float normalizeAngle(float angle) {
    while (angle >= 360.0) angle -= 360.0;
    while (angle < 0.0)    angle += 360.0;
    return angle;
	}

	//void rotateBy(float degrees);		

	int normalizeIndex(){
			return (currentIndex % 4 + 4) % 4;
	}
		
private:
	bool _isAttached{ false };
	int _servoPin;				//the pin the servo is attached to
	uint32_t _servoFreq;		//the frequency of the servo signal
	uint8_t _servoResolution;	//the resolution of the servo signal (in bits)
	uint32_t _rotationSpeedCW{1600};
	uint32_t _rotationSpeedCCW{1400};

	bool _isMoving{ false };	//is the servo moving?	

	//angles
	float _currentAngle{ 0.0 };	//current angle of the servo
	float _targetAngle{ 0.0 };		//target angle of the servo
	RotationDirection _direction{RotationDirection::None };	//direction of the rotation

	const double _errorMargin{ 0.5};	//error margin for angle comparison

	float _referenceAngle{0.0};
	int currentIndex{0};					//0 to 3; which determines the correct angle
	float _quadrantAngles[4];			//the target quarter angles set during initialization to minimize errors;
	

};

