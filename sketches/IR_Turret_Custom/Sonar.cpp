#include "Sonar.h"

Sonar::Sonar(uint8_t trigPin_, uint8_t echoPin_)
  : trigPin(trigPin_), echoPin(echoPin_), lastDistance(-1),
    pulseTimeout(17857)  // ~75 inches at ~0.0134 in/us
{
}

void Sonar::begin() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  Serial.println("[Sonar] Initialized");
}

long Sonar::getDistanceInches() {
  // Trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read echo
  unsigned long duration = pulseIn(echoPin, HIGH, pulseTimeout);
  
  if (duration == 0) {
    lastDistance = -1;
    return -1;
  }
  
  // Calculate: speed of sound ~0.0134 in/us
  lastDistance = duration * 0.0134 / 2;
  return lastDistance;
}

bool Sonar::isValid() const {
  return lastDistance >= 0;
}

long Sonar::getLastDistanceInches() const {
  return lastDistance;
}
