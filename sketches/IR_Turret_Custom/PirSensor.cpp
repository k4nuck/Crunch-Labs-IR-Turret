#include "PirSensor.h"

PirSensor::PirSensor(uint8_t pin_) : pin(pin_) {
}

void PirSensor::begin() {
    pinMode(pin, INPUT);
    Serial.print("[PirSensor] Initialized: Pin:");
    Serial.println(pin);
}

bool PirSensor::isMotionDetected() {
    /*Serial.print("[PirSensor] Motion detected on pin ");
    Serial.print(pin);
    Serial.print(": ");
    Serial.println(digitalRead(pin) == HIGH ? "YES" : "NO");*/
  return digitalRead(pin) == HIGH;
}
