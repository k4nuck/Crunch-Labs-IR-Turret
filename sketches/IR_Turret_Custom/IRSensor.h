#ifndef IRSENSOR_H
#define IRSENSOR_H

#include <Arduino.h>

// IR Remote command codes (NEC protocol)
#define IR_LEFT 0x8
#define IR_RIGHT 0x5A
#define IR_UP 0x18
#define IR_DOWN 0x52
#define IR_OK 0x1C
#define IR_CMD1 0x45
#define IR_CMD2 0x46
#define IR_CMD3 0x47
#define IR_CMD4 0x44
#define IR_CMD5 0x40
#define IR_CMD6 0x43
#define IR_CMD7 0x7
#define IR_CMD8 0x15
#define IR_CMD9 0x9
#define IR_CMD0 0x19
#define IR_STAR 0x16
#define IR_HASH 0xD

/**
 * IRSensor class: IR receiver driver.
 * 
 * Responsibilities:
 * - Manage IR receiver polling and decoding
 * - Return raw command codes; no decision-making
 * - Validate protocol (NEC only for this remote)
 */
class IRSensor {
public:
  IRSensor(uint8_t recvPin);
  
  // Initialize receiver (call once in setup)
  void begin();
  
  // Check if a new command is available
  bool hasCommand();
  
  // Get the decoded command (valid only after hasCommand() returns true)
  uint8_t getCommand();
  
private:
  uint8_t recvPin;
  uint8_t lastCommand;
};

#endif // IRSENSOR_H
