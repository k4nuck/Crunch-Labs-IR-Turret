#include "Turret.h"

Turret::Turret(uint8_t yawPin_, uint8_t pitchPin_, uint8_t rollPin_)
  : yawPin(yawPin_), pitchPin(pitchPin_), rollPin(rollPin_),
    yawStopSpeed(90), yawMoveSpeed(90), pitchMoveSpeed(8),
    rollStopSpeed(90), rollMoveSpeed(90),
    pitchVal(100),
    pitchMax(150), pitchMin(33)
{
}

void Turret::begin() {
  yawServo.attach(yawPin);
  pitchServo.attach(pitchPin);
  rollServo.attach(rollPin);
  home();
  Serial.println("[Turret] Initialized");
}

void Turret::left(int duration_ms) {
  Serial.print("[Turret] LEFT: ");
  Serial.print(duration_ms);
  Serial.println(" ms");
  yawServo.write(yawStopSpeed + yawMoveSpeed);
  delay(duration_ms);
  yawServo.write(yawStopSpeed);
}

void Turret::right(int duration_ms) {
  Serial.print("[Turret] RIGHT: ");
  Serial.print(duration_ms);
  Serial.println(" ms");
  yawServo.write(yawStopSpeed - yawMoveSpeed);
  delay(duration_ms);
  yawServo.write(yawStopSpeed);
}

void Turret::up(int duration_ms) {
  Serial.print("[Turret] UP: ");
  Serial.print(duration_ms);
  Serial.println(" ms");
  if ((pitchVal + pitchMoveSpeed) < pitchMax) {
    pitchVal = pitchVal + pitchMoveSpeed;
    pitchServo.write(pitchVal);
    delay(duration_ms);
  }
}

void Turret::down(int duration_ms) {
  Serial.print("[Turret] DOWN: ");
  Serial.print(duration_ms);
  Serial.println(" ms");
  if ((pitchVal - pitchMoveSpeed) > pitchMin) {
    pitchVal = pitchVal - pitchMoveSpeed;
    pitchServo.write(pitchVal);
    delay(duration_ms);
  }
}

void Turret::fireOne(int duration_ms) {
  Serial.print("[Turret] FIRE: ");
  Serial.print(duration_ms);
  Serial.println(" ms");
  rollServo.write(rollStopSpeed + rollMoveSpeed);
  delay(duration_ms);
  rollServo.write(rollStopSpeed);
}

void Turret::fireAll(int duration_ms) {
  Serial.print("[Turret] FIRE ALL: ");
  Serial.print(duration_ms);
  Serial.println(" ms");
  rollServo.write(rollStopSpeed + rollMoveSpeed);
  delay(duration_ms);
  rollServo.write(rollStopSpeed);
}

void Turret::home() {
  Serial.println("[Turret] HOMING");
  yawServo.write(yawStopSpeed);
  delay(20);
  rollServo.write(rollStopSpeed);
  delay(100);
  pitchVal = 100;
  pitchServo.write(pitchVal);
  delay(100);
  Serial.println("[Turret] HOMING DONE");
}

void Turret::setPitchDeg(int angle) {
  if (angle < pitchMin) angle = pitchMin;
  if (angle > pitchMax) angle = pitchMax;
  pitchVal = angle;
  pitchServo.write(pitchVal);
}

int Turret::getPitchDeg() const {
  return pitchVal;
}

void Turret::logStatus() {
  Serial.print("[Turret] Yaw=");
  Serial.print(yawVal);
  Serial.print(", Pitch=");
  Serial.print(pitchVal);
  Serial.print(", Roll=");
  Serial.println(rollVal);
}
