#ifndef TURRET_H
#define TURRET_H

#include <Arduino.h>
#include <Servo.h>

/**
 * Turret class: Hardware driver for servos and movement primitives.
 * 
 * Responsibilities:
 * - Own servo objects and manage movement primitives
 * - Enforce safe bounds (pitch min/max)
 * - Provide safe movement methods: left(), right(), up(), down(), fireOne(), home()
 * - All methods are non-blocking; use timing logic in caller if needed
 */
class Turret {
public:
  Turret(uint8_t yawPin, uint8_t pitchPin, uint8_t rollPin);
  
  // Initialize servos (call once in setup)
  void begin();
  
  #pragma region Movement Primitives

  // Safe movement primitives
  void left(int duration_ms = 150);
  void right(int duration_ms = 150);
  void up(int duration_ms = 50);
  void down(int duration_ms = 50);
  void fireOne(int duration_ms = 158);   // JB - I think we have a Bug here and we need to increase 
                                         //      the fire time to 158ms
  void fireAll(int duration_ms = 948);   // ~6x single fire duration

  #pragma endregion Movement Primitives
  
  void home();
  
  // Pitch control with bounds checking
  void setPitchDeg(int angle);
  int getPitchDeg() const;
  
  // Status logging (optional)
  void logStatus();
  
private:
  Servo yawServo;
  Servo pitchServo;
  Servo rollServo;
  
  uint8_t yawPin;
  uint8_t pitchPin;
  uint8_t rollPin;
  
  // Servo calibration & speed
  int yawStopSpeed;
  int yawMoveSpeed;
  int pitchMoveSpeed;
  int rollStopSpeed;
  int rollMoveSpeed;
  
  // Current servo positions
  int yawVal;
  int pitchVal;
  int rollVal;
  
  // Safety bounds
  int pitchMax;
  int pitchMin;
};

#endif // TURRET_H
