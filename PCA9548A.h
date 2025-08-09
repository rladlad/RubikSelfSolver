#pragma once
#include <Arduino.h>
#include <Wire.h> // Include the Wire library for I2C communication

#define MUX_ADDR 0x70  // PCA9548A default, check A0–A2


class PCA9548A
{
public:
	PCA9548A(uint8_t SDAPin, uint8_t SCLPin, uint8_t ResetPin) 
		: _SDAPin(SDAPin), _SCLPin(SCLPin), _ResetPin(ResetPin) {
	}
	
	void begin() {
		// Initialize I2C communication and set up the device
		Wire.begin(_SDAPin,_SCLPin);
	}

	void end(){
		Wire.end();
	}


	void reset() {
		// Reset the PCA9548A device
		digitalWrite(_ResetPin,0);
	}

	void set() {
		// Reset the PCA9548A device
		digitalWrite(_ResetPin,1);
	}

	//channel is 0 to 7
	void selectChannel(uint8_t channel) {
			if (channel > 7) return;

			Wire.beginTransmission(MUX_ADDR);
			Wire.write(1 << channel);  // Select channel (bit mask)
			Wire.endTransmission();
	}

	// Write a register + data to a device behind the multiplexer
	bool writeToDevice(uint8_t channel, uint8_t devAddr, uint8_t regAddr, const uint8_t *data, uint8_t len) {
			selectChannel(channel); // Enable the right bus

			Wire.beginTransmission(devAddr);
			Wire.write(regAddr); // First byte = register address

			for (uint8_t i = 0; i < len; i++) {
				Wire.write(data[i]); // Write each data byte
			}
		return (Wire.endTransmission() == 0); // 0 means success
	}

	// Overload for writing a single byte
	bool writeToDevice(uint8_t channel, uint8_t devAddr, uint8_t regAddr, uint8_t value) {

		return writeToDevice(channel, devAddr, regAddr, &value, 1);

	}

	uint8_t readRegister(uint8_t devAddr, uint8_t regAddr) {

			Wire.beginTransmission(devAddr);
			Wire.write(regAddr);         // Register address
			Wire.endTransmission(false); // Send restart, not stop
			Wire.requestFrom(devAddr, (uint8_t)1);
			if (Wire.available()) {
				return Wire.read();
			}
			return 0; // error or no data
	}

	uint16_t readRegister16(uint8_t devAddr, uint8_t regAddr) {

			Wire.beginTransmission(devAddr);
			Wire.write(regAddr);         // Register address
			Wire.endTransmission(false); // Restart instead of stop
			Wire.requestFrom(devAddr, (uint8_t)2);

			uint16_t value = 0;
			if (Wire.available() >= 2) {
				uint8_t highByte = Wire.read();
				uint8_t lowByte  = Wire.read();
				value = ((uint16_t)highByte << 8) | lowByte;
			}
			return value;
	}

	// Read N bytes from a device register behind the multiplexer
	bool readFromDevice(uint8_t channel, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len) {

		selectChannel(channel); // Enable correct channel

		// Step 1: Tell the device which register we want
		Wire.beginTransmission(devAddr);
		Wire.write(regAddr);
		if (Wire.endTransmission(false) != 0) { // Send restart, not stop
			return false; // Device didn't ACK
		}

		// Step 2: Request data
		uint8_t bytesRead = Wire.requestFrom(devAddr, len);
		if (bytesRead != len) {
			return false; // Didn't get expected bytes
		}

		for (uint8_t i = 0; i < len; i++) {
			buffer[i] = Wire.read();
		}
		return true;
	}



private:
	uint8_t _SDAPin;
	uint8_t _SCLPin;
	uint8_t _ResetPin;
	
};

