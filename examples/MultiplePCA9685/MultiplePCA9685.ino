/*
  Using a struct to map Servos to a PWM driver and channel.

  This approach can be useful if you don't want to equate the servo ID with the
  PWM board channel or use multiple PCA9685 boards. The Adafruit PCA9685 PWM
  Servo Driver Library is used to send the servo positions.
*/

#include "ik.h"
#include <Adafruit_PWMServoDriver.h>
#include <BlenderServoAnimation.h>

// Using the namespace to have short class references (Animation and Servo)
using namespace BlenderServoAnimation;

// PWM driver instances to set PWM output
Adafruit_PWMServoDriver pwmA(0x40);
Adafruit_PWMServoDriver pwmB(0x41);

// Animation object to represent the original Blender animation
Animation animation(FPS, FRAMES);

// We use a struct to map a servo to a PCA9685 board and channel
struct servoMap {
  Servo servo;
  Adafruit_PWMServoDriver pwm;
  byte channel;
};

// Forward declare the callback as it will be referenced in the following array
void setPWM(byte servoID, int position);

// Define an array of servo mapsf
servoMap servoMaps[] = {
    // Servo attached to board A on channel 0
    {Servo(0, NeckLeft, setPWM), pwmA, 0},

    // Servo attached to board B on channel 0
    {Servo(1, NeckRight, setPWM), pwmB, 0},
};

// Calculate the amount of servos so that we can easily extend the array
const byte servoAmount = sizeof(servoMaps) / sizeof(servoMaps[0]);

// Callback function which is called whenever a servo needs to be moved
void setPWM(byte servoID, int position) {
  // Iterate through the available servos
  for (int i = 0; i < servoAmount; i++) {
    // Check if the current servo ID matches the target servo ID
    if (servoMaps[i].servo.getID() == servoID) {
      // Get the PWM driver instance and channel from the mapping
      Adafruit_PWMServoDriver pwm = servoMaps[i].pwm;
      byte channel = servoMaps[i].channel;

      // Set the current position as PWM output
      pwm.setPWM(channel, 0, position);

      // Break the loop as we already handled the servo movement
      break;
    }
  }
}

void setup() {
  // Dynamically add the Blender servo objects to the animation
  for (int i = 0; i < servoAmount; i++) {
    animation.addServo(servoMaps[i].servo);
  }

  // Trigger the animation loop mode
  animation.loop();

  // Initialize servo drivers
  pwmA.begin();
  pwmA.setPWMFreq(60);
  pwmB.begin();
  pwmB.setPWMFreq(60);

  delay(10);
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
