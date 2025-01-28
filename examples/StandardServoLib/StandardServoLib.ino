/*
  Using the standard Arduino servo library to send servo positions.

  The setup requires nothing but a micro controller and a single servo. The animation is played in a
  loop.
*/

#include "simple.h"
#include <BlenderServoAnimation.h>

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

#define SERVO_PIN 3

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

// Animation object to control the animation
BlenderServoAnimation animation;

void setup() {
  // Attach the servo to the defined servo pin
  myServo.attach(SERVO_PIN);

  // Set the position callback
  animation.onPositionChange(move);

  // Add a scene based on PROGMEM data
  animation.addScene(ANIMATION_DATA, LENGTH, FPS, FRAMES);

  // Trigger the animation loop mode
  animation.loop();
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
