#include "ServoArm.h"
#include <Arduino.h>

ServoArm::ServoArm(uint8_t pin, uint32_t freq, uint8_t resolution)
{
	_servoPin = pin;
	_servoFreq = freq;		
	_servoResolution = resolution;

	//setup the quadrant angles;
	_quadrantAngles[0] = normalizeAngle(0);
	_quadrantAngles[1] = normalizeAngle(90);
	_quadrantAngles[2] = normalizeAngle(180);
	_quadrantAngles[3] = normalizeAngle(270);

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
	currentIndex++;
	rotateTo(_quadrantAngles[normalizeIndex()]);
}

void ServoArm::rotate90CCW()
{
	currentIndex--;
	rotateTo(_quadrantAngles[normalizeIndex()]);
}

void ServoArm::rotate180()
{
		currentIndex+=2;
		rotateTo(_quadrantAngles[normalizeIndex()]);
}


void ServoArm::rotateTo(float target) {
    if (!_isMoving) {
        target = normalizeAngle(target);
        float diff = fmod((target - _currentAngle + 540.0), 360.0) - 180.0;

        _targetAngle = target;

        if (diff > 0) {
            ledcWrite(_servoPin, pulseToDuty(_rotationSpeedCCW));
        } else {
            ledcWrite(_servoPin, pulseToDuty(_rotationSpeedCCW));
        }

        _isMoving = true;
    }
}


void ServoArm::setReferenceAngle(float angle){
		_referenceAngle = angle;

		//setup the quadrant angles;
		_quadrantAngles[0] = normalizeAngle(angle);
		_quadrantAngles[1] = normalizeAngle(angle + 90);
		_quadrantAngles[2] = normalizeAngle(angle + 180);
		_quadrantAngles[3] = normalizeAngle(angle + 270);
}

void ServoArm::update()
{
	if (!_isMoving)
		return;

	if (withinTolerance()){
		stop();
		Serial.print(_targetAngle);
		Serial.print(" ");
		Serial.println(_currentAngle);
	}

}
