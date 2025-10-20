#include <Wire.h>

#define NUM_MOTORS 6

class TCA6424A {
public:
  static const uint8_t DEFAULT_ADDR = 0x22;

  TCA6424A(uint8_t addr = DEFAULT_ADDR) : address(addr) {}

  void begin() {
    Wire.begin();
    // Set all pins as outputs (Config registers: 0x0C, 0x0D, 0x0E)
    writeRegister(0x0C, 0x00); // Port 0
    writeRegister(0x0D, 0x00); // Port 1
    writeRegister(0x0E, 0x00); // Port 2
  }

  void moveRelative(uint8_t motorId, int direction) {
    // Each motor uses 4 GPIOs starting at motorId * 4
    uint8_t gpioStart = motorId * 4;
    uint8_t stepIndex = (motorSteps[motorId] + direction + 4) % 4;
    motorSteps[motorId] = stepIndex;

    uint8_t pattern = stepSequence[stepIndex];
    writeMotorPattern(gpioStart, pattern);
  }

  void powerDownMotor(uint8_t motorId){
    if (motorId >= NUM_MOTORS) return;

    uint8_t gpioStart = motorId * 4;
    const uint8_t powerDownPattern = 0b0000;

    writeMotorPattern(gpioStart, powerDownPattern);
    motorSteps[motorId] = 0;
  }

private:
  uint8_t address;
  uint8_t motorSteps[6] = {0}; // up to 6 motors
  const uint8_t stepSequence[4] = {
    0b0001, 0b0010, 0b0100, 0b1000
  };

  void writeMotorPattern(uint8_t gpioStart, uint8_t pattern) {
    uint8_t portMask[3] = {0, 0, 0};

    for (uint8_t i = 0; i < 4; i++) {
      uint8_t gpio = gpioStart + i;
      uint8_t port = gpio / 8;
      uint8_t bit = gpio % 8;
      if (pattern & (1 << i)) {
        portMask[port] |= (1 << bit);
      }
    }

    // Write to output registers: 0x04, 0x05, 0x06
    for (uint8_t p = 0; p < 3; p++) {
      writeRegister(0x04 + p, portMask[p]);
    }
  }

  void writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
  }
};
