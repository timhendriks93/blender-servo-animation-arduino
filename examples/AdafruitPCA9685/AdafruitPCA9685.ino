/*
  Using an Adafruit PCA9685 module to send servo positions.

  This approach is especially useful when the animation is based on multiple servos. We assume the
  servo ID and the used board channel are equal. Therefore, the servo with the ID 0 has to be
  connected to channel 0 etc.
*/

#include "ik.h"
#include <Adafruit_PWMServoDriver.h>
#include <BlenderServoAnimation.h>

// Animation object to control the animation
BlenderServoAnimation animation;

// PWM driver instance to set PWM output
Adafruit_PWMServoDriver pwm;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // We assume the servoID is equal to the used channel on the PCA9685
  pwm.setPWM(servoID, 0, position);
}

void setup() {
  // Set the position callback
  animation.onPositionChange(move);

  // Add a scene based on PROGMEM data
  animation.addScene(ANIMATION_DATA, LENGTH, FPS, FRAMES);

  // Trigger the animation loop mode
  animation.loop();

  // Initialize servo driver
  pwm.begin();
  pwm.setPWMFreq(60);

  delay(10);
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
