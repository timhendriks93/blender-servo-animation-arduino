/*
  Using the Adafruit PCA9685 PWM Servo Driver Library to send servo positions.

  This approach is especially useful when the animation is based on multiple
  servos. We assume the servo ID and the used board channel are equal.
  Therefore, the servo with the ID 0 has to be connected to channel 0 etc.
*/

#include "ik.h"
#include <Adafruit_PWMServoDriver.h>
#include <BlenderServoAnimation.h>
#include <Wire.h>

// Using the namespace to have short class references (Animation and Servo)
using namespace BlenderServoAnimation;

// Frames per second - see original Blender animation / ik.h
#define FPS 30

// Total animation frames - see original Blender animation / ik.h
#define FRAMES 100

// Servo object to send positions
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // We assume the servoID is equal to the used channel on the PCA9685
  pwm.setPWM(servoID, 0, position);
}

// Animation object to represent the original Blender animation
Animation animation(FPS, FRAMES);

// Servo objects to manage the positions
Servo neckLeftServo(0, NeckLeft, move);
Servo neckRightServo(1, NeckRight, move);

void setup() {
  // Add the Blender servo objects to the animation
  animation.addServo(neckLeftServo);
  animation.addServo(neckRightServo);

  // Trigger the animation play mode
  animation.play();

  // Initialize servo driver
  pwm.begin();
  pwm.setPWMFreq(60);

  delay(10);
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
