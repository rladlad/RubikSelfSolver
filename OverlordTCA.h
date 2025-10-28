#include "TCA6424.h"
#include <map>

#define NUM_MOTORS 6

class OverlordTCA {
public:

  struct MotorState {
    int currentStep = 0;
    int targetStep = 0;
    int direction = 1;
    unsigned long lastStepTime = 0;
    float stepIntervalMs = 10.0;
    uint8_t gpioStart = 0;
  };

  OverlordTCA(TCA6424A& driver) : tca(driver) {}

  void setStepsPerRevolution(float steps) {
    stepsPerRev = steps;
  }

  void registerMotor(uint8_t motorId, uint8_t gpioStart) {
    motors[motorId].gpioStart = gpioStart;
  }

  void setMaxSpeed(uint8_t motorId, float rpm) {
    motors[motorId].stepIntervalMs = 60000.0 / (stepsPerRev * rpm);
  }

  void moveToAngle(uint8_t motorId, float angle, int dir) {
    int steps = (int)(stepsPerRev * (angle / 360.0));
    motors[motorId].direction = dir;
    motors[motorId].targetStep = motors[motorId].currentStep + (steps * dir);
  }

  void moveRelative(uint8_t motorId, int steps) {
    motors[motorId].direction = (steps >= 0) ? 1 : -1;
    motors[motorId].targetStep = motors[motorId].currentStep + steps;
  }

  void jog(uint8_t motorId, int dir) {
    moveRelative(motorId, dir);
  }

  void powerDownMotors(){
    for (int i=0; i<NUM_MOTORS; i++){
      tca.powerDownMotor(i);
    }
  }

  void powerDownMotor(uint8_t motorId){
    tca.powerDownMotor(motorId);
  }

  bool isBusy(uint8_t motorId) {
    return motors[motorId].currentStep != motors[motorId].targetStep;
  }

  void update() {
    unsigned long now = millis();
    for (auto& kv : motors) {
      MotorState& m = kv.second;
      if (m.currentStep == m.targetStep) continue;
      if (now - m.lastStepTime >= m.stepIntervalMs) {
        m.currentStep += m.direction;
        m.lastStepTime = now;
        tca.moveRelative(kv.first, m.direction); // calls TCA6424A wrapper
      }
    }
  }

private:
  TCA6424A& tca;
  float stepsPerRev = 2048.0;
  std::map<uint8_t, MotorState> motors;
};
