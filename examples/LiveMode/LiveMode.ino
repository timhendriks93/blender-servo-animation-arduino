/*
  Sending live servo positions via serial commands.

  This example requires a USB connection to your PC and a running Blender
  instance with the Blender Servo Animation Add-on. We use a single servo which
  is controlled via the standard Arduino servo library.
*/

#include <BlenderServoAnimation.h>
#include <Servo.h>

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int angle) {
  // Ignore the servoID (there is only one servo) and write the current angle
  myServo.write(angle);
}

// Animation object to manage the servos
// We skip providing fps or frames as we just want to use the live mode
BlenderServoAnimation::Animation animation;

// Servo object to manage the positions
BlenderServoAnimation::Servo myBlenderServo(0, move);

void setup() {
  Serial.begin(115200);

  // Attach the servo to pin 9
  myServo.attach(9);

  // Add the Blender servo object to the animation
  animation.addServo(myBlenderServo);

  // Trigger the animation live mode
  animation.live(Serial);
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
