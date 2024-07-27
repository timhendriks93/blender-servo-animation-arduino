/*
  Using two PCA9685 PWM servo drivers to animate 2 servos.

  Note that the A0 address jumper has to be soldered on the second driver board. This setup can
  easily be extended to animate up to 32 servos. If even more servos are needed, you can also add
  more driver boards to the chain.

  We assume the servo ID and the used board channel are equal. Therefore, the servo with the ID 0
  has to be connected to channel 0 etc.
*/

#include "ik.h"
#include <Adafruit_PWMServoDriver.h>
#include <BlenderServoAnimation.h>

#define SERVO_AMOUNT 2

// Animation object to control the animation
BlenderServoAnimation animation;

// PWM driver instances to set PWM output
Adafruit_PWMServoDriver pwmA(0x40);
Adafruit_PWMServoDriver pwmB(0x41);

// We use a struct to map a servo to a PCA9685 board and channel
struct servoMapping {
  byte id;
  Adafruit_PWMServoDriver pwm;
  byte channel;
};

// Define an array of servo maps
servoMapping servoMappings[SERVO_AMOUNT] = {
    // Servo 0 attached to board A on channel 0
    {0, pwmA, 0},

    // Servo 1 attached to board B on channel 0
    {1, pwmB, 0},
};

// Callback function which is called whenever a servo needs to be moved
void setPWM(byte servoID, int position) {
  // Iterate through the available servos
  for (int i = 0; i < SERVO_AMOUNT; i++) {
    // Continue if the current servo ID doesn't match the target servo ID
    if (servoMappings[i].id != servoID) {
      continue;
    }

    // Get the PWM driver instance and channel from the mapping
    Adafruit_PWMServoDriver pwm = servoMappings[i].pwm;
    byte channel = servoMappings[i].channel;

    // Set the current position as PWM output
    pwm.setPWM(channel, 0, position);

    // Break the for loop as we finsihed handling the servo movement
    break;
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
