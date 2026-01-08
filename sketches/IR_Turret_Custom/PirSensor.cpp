#include "PirSensor.h"

PirSensor::PirSensor(uint8_t pin_) : pin(pin_) {
}

void PirSensor::begin() {
  pinMode(pin, INPUT);
  Serial.println("[PirSensor] Initialized");
}

bool PirSensor::isMotionDetected() {
  return digitalRead(pin) == HIGH;
}
