/*
  Using the standard Arduino servo library to send servo positions.

  Note the namespace BlenderServoAnimation which helps to distinguish
  between the standard library servo class (Servo) and the servo
  class of this library (BlenderServoAnimation::Servo).
*/

#include "simple.h"
#include <BlenderServoAnimation.h>

// Using the namespace to have short class references (Animation and Servo)
using namespace BlenderServoAnimation;

// Frames per second - see original Blender animation / simple.h
#define FPS 30

// Total animation frames - see original Blender animation / simple.h
#define FRAMES 100

#define PIN_EN 10
#define PIN_DIR 11
#define PIN_STEP 12

int lastPos;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current angle
  if (!lastPos) {
    lastPos = position;
    return;
  }

  int diff = lastPos - position;
  lastPos = position;

  if (diff == 0) {
    return;
  }

  if (diff > 0) {
    digitalWrite(PIN_DIR, HIGH);
  } else {
    digitalWrite(PIN_DIR, LOW);
  }

  for (int i = 0; i < abs(diff); i++) {
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(100);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(100);
  }
}

// Animation object to represent the original Blender animation
Animation animation(FPS, FRAMES);

// Servo object to manage the positions
Servo myBlenderServo(0, Bone, move);

void setup() {
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);

  digitalWrite(PIN_EN, LOW);

  // Add the Blender servo object to the animation
  animation.addServo(myBlenderServo);

  // Trigger the animation loop mode
  animation.loop();
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}