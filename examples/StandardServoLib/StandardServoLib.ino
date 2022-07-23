/*
  Using the standard Arduino servo library to send servo positions.

  Note the namespace BlenderServoAnimation which helps to distinguish
  between the standard library servo class (Servo) and the servo
  class of this library (BlenderServoAnimation::Servo).
*/

#include "simple.h"
#include <BlenderServoAnimation.h>
#include <Servo.h>

// Frames per second - see original Blender animation / simple.h
#define FPS 30

// Total animation frames - see original Blender animation / simple.h
#define FRAMES 100

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int angle) {
  // Ignore the servoID (there is only one servo) and write the current angle
  myServo.write(angle);
}

// Animation object to represent the original Blender animation
BlenderServoAnimation::Animation animation(FPS, FRAMES);

// Servo object to manage the positions
BlenderServoAnimation::Servo myBlenderServo(0, Bone, move);

void setup() {
  // Attach the servo to pin 9
  myServo.attach(9);

  // Add the Blender servo object to the animation
  animation.addServo(myBlenderServo);

  // Trigger the animation play mode
  animation.play();
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
