#include "ServoArm.h"
#include <Arduino.h>

ServoArm::ServoArm(uint8_t pin, uint32_t freq, uint8_t resolution)
{
	_servoPin = pin;
	_servoFreq = freq;		
	_servoResolution = resolution;

}

void ServoArm::turnOn()
{
	ledcAttach(_servoPin, _servoFreq, _servoResolution);
}

void ServoArm::turnOff()
{
	ledcDetach(_servoPin);
}

void ServoArm::rotate90CW()
{
	if (!_isMoving) {

	}
}

void ServoArm::rotate90CCW()
{
}

void ServoArm::rotate180CW()
{
}

void ServoArm::rotate180CCW()
{
}

void ServoArm::evaluateMove(double currentAngle)
{
	if (!_isMoving)
		return; //nothing to do if not moving\

	
}
