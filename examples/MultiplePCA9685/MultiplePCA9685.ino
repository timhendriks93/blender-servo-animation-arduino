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
Animation animation;

// We use a struct to map a servo to a PCA9685 board and channel
struct servoMapping {
  byte id;
  Adafruit_PWMServoDriver pwm;
  byte channel;
};

// Define an array of servo mapsf
servoMapping servoMappings[] = {
    // Servo 0 attached to board A on channel 0
    {0, pwmA, 0},

    // Servo 1 attached to board B on channel 0
    {1, pwmB, 0},
};

// Calculate the amount of servos so that we can easily extend the array
const byte servoAmount = sizeof(servoMaps) / sizeof(servoMaps[0]);

// Callback function which is called whenever a servo needs to be moved
void setPWM(byte servoID, int position) {
  // Iterate through the available servos
  for (int i = 0; i < servoAmount; i++) {
    // Check if the current servo ID matches the target servo ID
    if (servoMappings[i].id == servoID) {
      // Get the PWM driver instance and channel from the mapping
      Adafruit_PWMServoDriver pwm = servoMappings[i].pwm;
      byte channel = servoMappings[i].channel;

      // Set the current position as PWM output
      pwm.setPWM(channel, 0, position);

      // Break the for loop as we already handled the servo movement
      break;
    }
  }
}

void setup() {
  // Set the position callback
  animation.onPositionChange(setPWM);

  // Add a scene based on PROGMEM data
  animation.addScene(ANIMATION_DATA, LENGTH, FPS, FRAMES);

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
