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
		_targetAngle = _currentAngle - 90;
		if (_targetAngle < 0.0)
			_targetAngle += 360.0;

		ledcWrite(_servoPin,pulseToDuty(_rotationSpeedCW));
		_isMoving=true;

	}
}

void ServoArm::rotate90CCW()
{
	if (!_isMoving) {
		//calculate new target angle
		_targetAngle = _currentAngle + 90;
		if (_targetAngle >= 360.0)
			_targetAngle -= 360.0;

		Serial.print(_targetAngle);
		Serial.print(" ");
		Serial.println(_currentAngle);

		ledcWrite(_servoPin,pulseToDuty(_rotationSpeedCCW));
		_isMoving=true;
	}
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

void ServoArm::rotateBy(float degrees) {
		//degrees = -degrees;
    rotateTo(normalizeAngle(_currentAngle + degrees));
}

void ServoArm::rotate180CW()
{
	if (!_isMoving) {
		//calculate new target angle
		_targetAngle = _currentAngle + 180;
		if (_targetAngle > 360.0)
			_targetAngle -= 360.0;

		Serial.print(_targetAngle);
		Serial.print(" ");
		Serial.println(_currentAngle);

		ledcWrite(_servoPin,pulseToDuty(_rotationSpeedCW));
		_isMoving=true;
	}
}

void ServoArm::rotate180CCW()
{
}

void ServoArm::evaluateMove()
{
	// if (!_isMoving)
	// 	return false;	//not moving

	// if (withinTolerance()){
	// 	stop();
	// 	Serial.print(_targetAngle);
	// 	Serial.print(" ");
	// 	Serial.println(_currentAngle);
	// 	return false;
	// }
	// return true;	//it is still moving

		unsigned long now = millis();
    float dt = (now - lastUpdateTime) / 1000.0; // convert ms to seconds
    if (dt <= 0) dt = 0.001; // avoid division by zero

    //_currentAngle = readEncoderAngle();

    if (_isMoving) {
        float error = fmod((_targetAngle - _currentAngle + 540.0), 360.0) - 180.0;

        // Integral term update with anti-windup
        integral += error * dt;
        // Optionally clamp integral to prevent windup:
        const float integralMax = 100.0;
        if (integral > integralMax) integral = integralMax;
        if (integral < -integralMax) integral = -integralMax;

        // Derivative term
        float derivative = (error - previousError) / dt;

        // PID output
        float output = Kp * error + Ki * integral + Kd * derivative;

        // Clamp output to max speed range
        const float maxOutput = 200.0;
        if (output > maxOutput) output = maxOutput;
        if (output < -maxOutput) output = -maxOutput;

        // Determine PWM pulse based on output
        float pulse = 1500.0 + output; // 1500us is stop, positive output = CCW, negative = CW
        if (pulse > 1700.0) pulse = 1700.0;  // max CCW speed pulse
        if (pulse < 1300.0) pulse = 1300.0;  // max CW speed pulse

        if (fabs(error) <= _errorMargin) {
            ledcWrite(_servoPin, pulseToDuty(1500)); // stop motor
            _isMoving = false;
            integral = 0.0;
            previousError = 0.0;

        } else {
            ledcWrite(_servoPin, pulseToDuty((uint32_t)pulse));
            previousError = error;
        }
    }

    lastUpdateTime = now;
}
