/*
  Using the standard Arduino servo library to send servo positions.

  Note the namespace BlenderServoAnimation which helps to distinguish
  between the standard library servo class (Servo) and the servo
  class of this library (BlenderServoAnimation::Servo).
*/

#include "ik.h"
#include <BlenderServoAnimation.h>

// Using the namespace to have short class references (Animation and Servo)
using namespace BlenderServoAnimation;

// Frames per second - see original Blender animation / simple.h
#define FPS 30

// Total animation frames - see original Blender animation / simple.h
#define FRAMES 100

#define MOTOR_AMOUNT 2

int frameMicros = floor(1000000 / FPS);

struct Stepper {
  byte motorID;
  byte enPin;
  byte dirPin;
  byte stepPin;
  int stepDelay;
  int lastPosition;
  unsigned long lastMicros;
  bool active;
};

Stepper mySteppers[MOTOR_AMOUNT] = {
  {0, 7, 8, 9},
  {1, 10, 11, 12},
};

// Callback function which is called whenever a servo needs to be moved
void move(byte motorID, int position) {
  int motorIndex;

  for (int i = 0; i < MOTOR_AMOUNT; i++) {
    if (mySteppers[i].motorID == motorID) {
      motorIndex = i;
      break;
    }
  }

  if (!mySteppers[motorIndex].lastPosition) {
    mySteppers[motorIndex].lastPosition = position;
    return;
  }

  int diff = mySteppers[motorIndex].lastPosition - position;
  mySteppers[motorIndex].lastPosition = position;

  if (diff == 0) {
    mySteppers[motorIndex].active = false;
    return;
  }

  if (diff > 0) {
    digitalWrite(mySteppers[motorIndex].dirPin, HIGH);
  } else {
    digitalWrite(mySteppers[motorIndex].dirPin, LOW);
  }

  int delayMicros = floor(frameMicros / abs(diff));

  mySteppers[motorIndex].active = true;
  mySteppers[motorIndex].stepDelay = delayMicros;
}

// Animation object to represent the original Blender animation
Animation animation(FPS, FRAMES);

// Servo objects to manage the positions
Servo neckLeftServo(0, NeckLeft, move);
Servo neckRightServo(1, NeckRight, move);

void setup() {
  for (int i = 0; i < MOTOR_AMOUNT; i++) {
    pinMode(mySteppers[i].enPin, OUTPUT);
    pinMode(mySteppers[i].stepPin, OUTPUT);
    pinMode(mySteppers[i].dirPin, OUTPUT);

    digitalWrite(mySteppers[i].enPin, LOW);
  }

  // Add the Blender servo objects to the animation
  animation.addServo(neckLeftServo);
  animation.addServo(neckRightServo);

  // Trigger the animation loop mode
  animation.loop();
}

void loop() {
  // Update the animation state on each loop
  animation.run();

  unsigned long currentMicros = micros();

  for (int i = 0; i < MOTOR_AMOUNT; i++) {
    if (!mySteppers[i].active || currentMicros - mySteppers[i].lastMicros < mySteppers[i].stepDelay) {
      continue;
    }

    digitalWrite(mySteppers[i].stepPin, HIGH);
    digitalWrite(mySteppers[i].stepPin, LOW);
    mySteppers[i].lastMicros = currentMicros;
  }
}