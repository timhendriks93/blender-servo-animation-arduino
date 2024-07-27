/*
  Sending live servo positions via serial commands.

  This example requires a USB connection to your PC and a running Blender
  instance with the Blender Servo Animation Add-on. We use a single servo which
  is controlled via the standard Arduino servo library.
*/

#include <BlenderServoAnimation.h>

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

#define SERVO_PIN 12

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

// Animation object to control the animation
BlenderServoAnimation::Animation animation;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Attach the servo to the defined servo pin
  myServo.attach(SERVO_PIN);

  // Set the position callback
  animation.onPositionChange(move);

  // Trigger the animation live mode
  animation.live(Serial);
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
