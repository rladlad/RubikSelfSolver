#include "ServoArm.h"
#include <Arduino.h>

ServoArm::ServoArm(uint8_t pin, uint32_t freq, uint8_t resolution)
{
	_servoPin = pin;
	_servoFreq = freq;		
	_servoResolution = resolution;

}

void ServoArm::attach()
{
	ledcAttach(_servoPin, _servoFreq, _servoResolution);
	_isAttached=true;
}

void ServoArm::detach()
{
	ledcDetach(_servoPin);
	_isAttached=false;
}

void ServoArm::stop(){
	ledcWrite(_servoPin,pulseToDuty(1500));
	_isMoving=false;
}

void ServoArm::rotate90CW()
{
	if (!_isMoving) {
		//calculate new target angle
		_targetAngle = _currentAngle + 90;
		if (_targetAngle > 360.0)
			_targetAngle -= 360.0;

		ledcWrite(_servoPin,pulseToDuty(1700));
		_isMoving=true;
		Serial.println("cw called");
	}
}

void ServoArm::rotate90CCW()
{
	if (!_isMoving) {
		//calculate new target angle
		_targetAngle = _currentAngle - 90;
		if (_targetAngle <0.0)
			_targetAngle += 360.0;

		ledcWrite(_servoPin,pulseToDuty(1300));
		_isMoving=true;
	}
}

void ServoArm::rotate180CW()
{
	if (!_isMoving) {
		//calculate new target angle
		_targetAngle = _currentAngle + 180;
		if (_targetAngle > 360.0)
			_targetAngle -= 360.0;

		ledcWrite(_servoPin,pulseToDuty(1700));
		_isMoving=true;
	}
}

void ServoArm::rotate180CCW()
{
}

bool ServoArm::evaluateMove()
{
	if (!_isMoving)
		return false;	//not moving

	float diff = angleDiff(_currentAngle,_targetAngle);
	if (abs(diff)<_errorMargin){
		stop();
		return false;
	}
	//Serial.println(diff);
	return true;	//it is still moving
}
