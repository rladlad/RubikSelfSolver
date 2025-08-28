#ifndef BLEBOT_H
#define BLEBOT_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Call this in setup()
void initBleBot();

// Optional helper to send messages back to client
void bleSendMessage(const String &msg);

#endif