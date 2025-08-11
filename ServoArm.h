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

	void rotateBy(float degrees);
	void rotate90CW();	//rotate the servo 90 degrees clockwise
	void rotate90CCW();	//rotate the servo 90 degrees counter-clockwise
	void rotate180CW();	//rotate the servo 180 degrees clockwise
	void rotate180CCW();	//rotate the servo 180 degrees counter-clockwise
	void setRotationSpeedCW(uint32_t us){_rotationSpeedCW=us;};	//in micreosendos ; 1500 is the middle
	void setRotationSpeedCCW(uint32_t us){_rotationSpeedCCW=us;}; //should be less the 1500 

	

	void setCurrentAngle(double angle) {_currentAngle = angle;};	//set the current angle of the servo	
	void evaluateMove();	//this function should be called per frame to evalualate a moving servo if it has reached its target angle return false if its not moving ; otherwise true

	double getCurrentAngle(){return _currentAngle;};
	double getTargetAngle(){return _targetAngle;};
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
		
private:
	bool _isAttached{ false };
	int _servoPin;				//the pin the servo is attached to
	uint32_t _servoFreq;		//the frequency of the servo signal
	uint8_t _servoResolution;	//the resolution of the servo signal (in bits)
	uint32_t _rotationSpeedCW{1600};
	uint32_t _rotationSpeedCCW{1400};

	bool _isMoving{ false };	//is the servo moving?	

	//angles
	double _currentAngle{ 0.0 };	//current angle of the servo
	double _targetAngle{ 0.0 };		//target angle of the servo
	RotationDirection _direction{RotationDirection::None };	//direction of the rotation

	const double _errorMargin{ 0.5};	//error margin for angle comparison

	// PID coefficients 
	float Kp = 2.0;     // proportional gain
	float Ki = 0.0;     // integral gain
	float Kd = 0.3;     // derivative gain

	float integral = 0.0;
	float previousError = 0.0;
	unsigned long lastUpdateTime = 0; // for delta time
};

