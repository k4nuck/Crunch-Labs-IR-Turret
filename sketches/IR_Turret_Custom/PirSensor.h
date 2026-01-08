#ifndef PIRSENSOR_H
#define PIRSENSOR_H

#include <Arduino.h>

/**
 * PirSensor class: PIR motion detection driver.
 * 
 * Responsibilities:
 * - Read PIR sensor state
 * - Return true/false for motion detected
 * - Optional: debounce logic (not implemented here; caller can debounce)
 */
class PirSensor {
public:
  PirSensor(uint8_t pin);
  
  // Initialize pin (call once in setup)
  void begin();
  
  // Read motion state (true = motion detected)
  bool isMotionDetected();
  
private:
  uint8_t pin;
};

#endif // PIRSENSOR_H
