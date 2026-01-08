#include "IRSensor.h"
#include <IRremote.hpp>

IRSensor::IRSensor(uint8_t recvPin_) : recvPin(recvPin_), lastCommand(0) {
}

void IRSensor::begin() {
  IrReceiver.begin(recvPin, ENABLE_LED_FEEDBACK);
  Serial.print(F("[IRSensor] Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F(""));
}

bool IRSensor::hasCommand() {
  if (IrReceiver.decode()) {
    // Validate protocol (NEC only for this remote)
    if (IrReceiver.decodedIRData.protocol != NEC) {
      IrReceiver.resume();
      return false;  // Invalid protocol
    }
    
    lastCommand = IrReceiver.decodedIRData.command;
    
    // Print debug info
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);
    Serial.println();
    
    // Re-arm receiver for next frame
    IrReceiver.resume();
    return true;
  }
  return false;
}

uint8_t IRSensor::getCommand() {
  return lastCommand;
}
