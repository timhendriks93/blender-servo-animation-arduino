/*
  Using the standard Arduino servo library to send servo positions.

  Note the namespace BlenderServoAnimation which helps to distinguish
  between the standard library servo class (Servo) and the servo
  class of this library (BlenderServoAnimation::Servo).
*/

#include <BlenderServoAnimation.h>
#include <SD.h>

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

// Animation object to represent the original Blender animation
BlenderServoAnimation::Animation animation;

void setup() {
  SD.begin(4);
  File myFile = SD.open("test.txt", FILE_READ);

  // Attach the servo to pin 12
  myServo.attach(12);

  // Set the position callback
  animation.onPositionChange(move);

  // Add a scene with the File stream
  animation.addScene(myFile, 30, 100);

  // Trigger the animation loop mode
  animation.loop();
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
