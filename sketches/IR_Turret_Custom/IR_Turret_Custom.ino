//////////////////////////////////////////////////
              //  LICENSE  //
//////////////////////////////////////////////////
#pragma region LICENSE
/*
  ************************************************************************************
  * MIT License
  *
  * Copyright (c) 2025 Crunchlabs LLC (IRTurret Control Code)
  * Copyright (c) 2020-2022 Armin Joachimsmeyer (IRremote Library)

  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  ************************************************************************************
*/
#pragma endregion LICENSE

//////////////////////////////////////////////////
              //  LIBRARIES  //
//////////////////////////////////////////////////
#pragma region LIBRARIES

#include <Arduino.h>
#include <Servo.h>

// Hardware drivers (class-based architecture)
#include "Turret.h"
#include "Sonar.h"
#include "PirSensor.h"
#include "IRSensor.h"
#include "RingQueue.h"

#pragma endregion LIBRARIES

#pragma region Parameters and Function Prototypes
// AUTO-FIRE CONFIGURATION
int autoFireThresholdInches = 65; // fire if target is closer than this distance (inches)
int autoFireMaxDistanceInches = 65; // don't fire if target is farther than this (safety limit)
int autoFireCooldownMs = 1000; // milliseconds to wait between auto-fire shots
int autoFireConfirmationsNeeded = 2; // consecutive in-range readings required before firing (debounce)
unsigned long lastAutoFireMillis = 0; // timestamp of last auto-fire event
int autoFireConfirmationCount = 0; // counter for consecutive in-range readings

// Sonar auto-fire enable/disable
int sonarEnabled = 0; // 0 = disabled, 1 = auto-fire enabled
unsigned long lastSonarEnabledMillis = 0; // timestamp of last auto-fire event
int sonarEnableCooldownMs = 2000; // milliseconds to wait between sonar enable/disable toggles

// Fire All duration between shots
unsigned long lastFireAllShotMillis = 0; // timestamp of last shot in fireAll()
int fireAllShotDelayMs = 250; // milliseconds between shots in fireAll()

// Objects for hardware drivers
Turret turret(10, 11, 12);  // Yaw=pin10, Pitch=pin11, Roll=pin12
Sonar sonar(2, 3);  // Trig=pin2, Echo=pin3
PirSensor pir(4);  // PIR sensor on pin4
IRSensor irSensor(9);  // IR receiver on pin9// Timing for IR poll

// Ring queue for commands - handleTasks() can dequeue and process
struct Command {
    uint8_t cmd;
    uint8_t meta; // optional metadata
};
RingQueue<Command, 32> commandQueue; // queue size of 32 commands

#define IR_CMD 1 // IR Command Type.  Metadata is the actual command code
#define SONAR_DISTANCE_CMD 2 // Sonar Distance Command Type.  Metadata is the distance in inches
#define PIR_MOTION_CMD 3 // PIR Motion Command Type.  Metadata is 1=motion detected, 0=no motion
#define IR_QUEUE 4 // IR Command Queue Type.  Metadata is unused
#define SONAR_QUEUE 5 // Sonar Distance Command Queue Type.  Metadata is unused
#define PIR_QUEUE 6 // PIR Motion Command Queue Type.  Metadata is unused

// Function prototypes
void handleIRCommands(uint8_t cmd); //function prototype for handling IR commands
void handleIRQueue(); //function prototype for handling IR Queue
void handleSonarSensorDistance(uint8_t distanceInches); //function prototype for handling Sonar Sensor
void handleSonarSensorQueue(); //function prototype for handling Sonar Sensor Queue
void handlePirSensorMovement(uint8_t movementDetected); //function prototype for handling PIR Sensor
void handlePirSensorQueue(); //function prototype for handling PIR Sensor Queue
void handleTasks(); //function prototype for handling Tasks based on sensor values
void shakeHeadYes(int moves = 3); //function prototypes for shakeHeadYes and No for proper compiling
void shakeHeadNo(int moves = 3);
bool hasTimeElapsed(unsigned long& lastMillis, int intervalMs); //utility function to check time elapsed

#pragma endregion Parameters and Function Prototypes

//////////////////////////////////////////////////
              //  S E T U P  //
//////////////////////////////////////////////////
#pragma region SETUP
void setup() { //this is our setup function - it runs once on start up, and is basically where we get everything "set up"
    Serial.begin(9600); // initializes the Serial communication between the computer and the microcontroller

    // Just to know which program is running on my microcontroller
    Serial.println(F("START " __FILE__ " from " __DATE__));
    Serial.println(F("IRremote v4.5.0"));
    Serial.println(F("=== Refactored with Hardware Drivers ==="));

    // Initialize hardware drivers
    turret.begin();
    sonar.begin();
    pir.begin();
    irSensor.begin();
}
#pragma endregion SETUP

//////////////////////////////////////////////////
               //  L O O P  //
//////////////////////////////////////////////////
#pragma region LOOP

/****
 * The loop function runs over and over again forever
 */
void loop() {
    // Handle IR commands (poll frequently for responsive input)
    handleIRQueue(); 

    // Handle sonar sensor & auto-fire logic
    handleSonarSensorQueue();

    // Handle PIR sensor
    handlePirSensorQueue();

    // Handle Tasks based on values from sensors and IR commands
    handleTasks();

    // Small delay to avoid overwhelming the CPU
    delay(5);
}

#pragma endregion LOOP

//////////////////////////////////////////////////
               // FUNCTIONS  //
//////////////////////////////////////////////////
#pragma region IR Commands

void handleIRQueue(){
    /*
    * Check if IR receiver has a new command available
    */
    if (!irSensor.hasCommand()) {
        return;  // No new command
    }

    // Get the decoded command from IR sensor
    uint8_t cmd = irSensor.getCommand();

    // Add command to queue.  metadata is the cmd.
    commandQueue.enqueue({IR_CMD, cmd});

} //function prototype for handling IR commands

void handleIRCommands(uint8_t cmd){
    /*
    * Business logic: decide what to do based on the command
    */
    switch(cmd) {

        case IR_UP: //pitch up
          turret.up();
          break;
        
        case IR_DOWN: //pitch down
          turret.down();
          break;

        case IR_LEFT: //fast counterclockwise rotation
          turret.left();
          break;
        
        case IR_RIGHT: //fast clockwise rotation
          turret.right();
          break;
        
        case IR_OK: //firing routine 
          // JB - An example of considering to add a delay so that multiple 
          //      fire commands are not processed too quickly
          turret.fireOne();
          break;
          
        case IR_STAR:
        // JB - Business logic should be handled here ... not in the Turret class
        //      Utilize the queue to buffer multiple fire commands if needed
          turret.fireAll();
          break;

        // JB - Consider queueing multiple nod/shake commands if needed
        case IR_CMD1:
          shakeHeadYes(3);
          break;

        case IR_CMD2:
          shakeHeadNo(3);
          break;

        case IR_HASH:
          // Don't allow toggling sonar too quickly
          if (hasTimeElapsed(lastSonarEnabledMillis, sonarEnableCooldownMs)) {
            if (sonarEnabled == 0){
                sonarEnabled = 1;
                Serial.println(">>> AUTO-FIRE MODE: ENABLED <<<");
            }else{
                sonarEnabled = 0;
                Serial.println(">>> AUTO-FIRE MODE: DISABLED <<<");
                autoFireConfirmationCount = 0; // clear confirmation counter when disarmed
            }
          }
          break;
    }
}

#pragma endregion IR Commands
#pragma region Sonar Sensor
void handleSonarSensorQueue(){

    // MAke sure sonar auto-fire is enabled
    if (sonarEnabled == 0){
        return; //skip the sonar code if it's not enabled
    }

    // Get Sonar Reading
    long distanceInches = sonar.getDistanceInches();

    // Make sure sonar reading is valid
    // A getDistanceInches() must be called before checking validity
    // JB - Might want to consider if this 2 step process is ideal
    if (!sonar.isValid()) {
        return; // No echo received within the timeout period
    }

    // Check if there was movement detected by PIR sensor
    bool movementDetected = pir.isMotionDetected();

    if (!movementDetected) {
        // Serial.println("No movement detected by PIR; skipping sonar queueing.");
        return; // No movement detected, skip sonar processing
    }

    // Add sonar distance to queue.  metadata is the distance in inches.
    commandQueue.enqueue({SONAR_DISTANCE_CMD, static_cast<uint8_t>(distanceInches)});

} //function prototype for handling Sonar Sensor Queue
void handleSonarSensorDistance(uint8_t distanceInches){
    // AUTO-FIRE LOGIC: Check if target is in firing range
    if (distanceInches <= autoFireThresholdInches && distanceInches <= autoFireMaxDistanceInches) {
        autoFireConfirmationCount++; // increment confirmation counter
        
        // Fire if confirmations met and cooldown elapsed
        if (autoFireConfirmationCount >= autoFireConfirmationsNeeded && 
            hasTimeElapsed(lastAutoFireMillis, autoFireCooldownMs)) {
            Serial.println(">>> AUTO-FIRE TRIGGERED <<<");
            Serial.print("Distance: ");
            Serial.print(distanceInches);
            Serial.println(" inches");
            turret.fireOne();
            autoFireConfirmationCount = 0; // reset after firing
        }
    } else {
        autoFireConfirmationCount = 0; // reset confirmation counter if out of range
    }

} //function prototype for handling Sonar Sensor

#pragma endregion Sonar Sensor

#pragma region PIR Sensor
void handlePirSensorQueue(){
    
    //JB - This is needed when we have peripheral PIR Sensors.
    //     PIR Is tightly coupled with Sonar in this design, so we dont need
    //     Check pir sensor in this function.

    /*
    * Check if PIR sensor is triggered
    */
    // bool movementDetected = pir.isMotionDetected();

    // Add PIR movement state to queue.  metadata is 1 for motion detected, 0 for no motion.
    // commandQueue.enqueue({PIR_MOTION_CMD, static_cast<uint8_t>(movementDetected ? 1 : 0)});

} //function prototype for handling PIR Sensor Queue
void handlePirSensorMovement(uint8_t movementDetected){
    /*
    * Check if PIR sensor is triggered
    */
    if (movementDetected) {
        Serial.println(">>> PIR MOTION DETECTED <<<");
        // JB - Add any additional logic needed when motion is detected
        //      THIS will be handled when we add peripheral PIR sensors
    }
} //function prototype for handling PIR Sensor
#pragma endregion PIR Sensor

#pragma region TASKS
void handleTasks(){

  
    // JB - Some basic logging
    /*
    int count = commandQueue.getCount();
    if(count > 0 ){
      Serial.print("Handling Tasks... Queue Count: ");
      Serial.println(count);
    }*/

    // Warn if queue is full
    if(commandQueue.isFull()){
        Serial.println("WARNING: Command Queue is FULL!");
    }

    /*
    * Process commands from the command queue
    */
    while (!commandQueue.isEmpty()) {
        Command cmd = commandQueue.dequeue();
        
        switch(cmd.cmd) {
            case IR_CMD:
                handleIRCommands(cmd.meta);
                break;
            case SONAR_DISTANCE_CMD:
                handleSonarSensorDistance(cmd.meta);
                break;
            case PIR_MOTION_CMD:
                handlePirSensorMovement(cmd.meta);
                break;
            case IR_QUEUE:
                handleIRQueue();
                break;
            case SONAR_QUEUE:
                handleSonarSensorQueue();
                break;
            case PIR_QUEUE:
                handlePirSensorQueue();
                break;
            default:
                Serial.print("Unknown command in queue: ");
                Serial.println(cmd.cmd);
                break;
        }
    }
} //function prototype for handling Tasks based on sensor values
#pragma endregion TASKS

#pragma region FUNCTIONS

// Note: leftMove, rightMove, upMove, downMove, fire, fireAll, homeServos
// are now encapsulated in the Turret class and called via turret.left(), etc.

void shakeHeadYes(int moves = 3) { //sets the default number of nods to 3, but you can pass in whatever number of nods you want
      Serial.println("YES");

    int startAngle = turret.getPitchDeg();
    int nodAngle = startAngle + 15; // Angle for nodding motion

    // JB - Why arent we using turret.up() and turret.down() here?

    for (int i = 0; i < moves; i++) { // Repeat nodding motion three times
        // Nod up
        for (int angle = startAngle; angle <= nodAngle; angle++) {
            turret.setPitchDeg(angle);
            delay(7); // Adjust delay for smoother motion
        }
        delay(50); // Pause at nodding position
        // Nod down
        for (int angle = nodAngle; angle >= startAngle; angle--) {
            turret.setPitchDeg(angle);
            delay(7); // Adjust delay for smoother motion
        }
        delay(50); // Pause at starting position
    }
}

void shakeHeadNo(int moves = 3) {
    Serial.println("NO");

    for (int i = 0; i < moves; i++) { // Repeat nodding motion three times
        // rotate right, stop, then rotate left, stop
        turret.right(190);
        delay(50);
        turret.left(190);
        delay(50); // Pause at starting position
    }
}

bool hasTimeElapsed(unsigned long& lastMillis, int intervalMs) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis >= intervalMs) {
        lastMillis = currentMillis;
        return true;
    }
    return false;
}
#pragma endregion FUNCTIONS

//////////////////////////////////////////////////
               //  END CODE  //
//////////////////////////////////////////////////
