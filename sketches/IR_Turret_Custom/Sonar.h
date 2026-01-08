#ifndef SONAR_H
#define SONAR_H

#include <Arduino.h>

/**
 * Sonar class: HC-SR04 distance sensor driver.
 * 
 * Responsibilities:
 * - Read HC-SR04 sensor and compute distance
 * - Return distance in inches (caller decides action)
 * - No firing logic; just measurement
 */
class Sonar {
public:
  Sonar(uint8_t trigPin, uint8_t echoPin);
  
  // Initialize pins (call once in setup)
  void begin();
  
  // Read distance in inches; returns -1 if timeout or no echo
  long getDistanceInches();
  
  // Convenience: was last reading valid?
  bool isValid() const;
  
  // Get last reading (cached)
  long getLastDistanceInches() const;
  
private:
  uint8_t trigPin;
  uint8_t echoPin;
  long lastDistance;
  unsigned long pulseTimeout;  // microseconds
};

#endif // SONAR_H
