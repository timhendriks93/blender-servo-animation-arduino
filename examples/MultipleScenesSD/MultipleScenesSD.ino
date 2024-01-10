/*
  Setting up a show consisting of 2 animations.

  Note the namespaces which are used to distinguish the positions
  of one scene / animation from another. It's even possible to
  have different playback rates (fps) and frames per animation.
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

File animationFile;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

void changeFile(byte prevSceneIndex, byte nextSceneIndex) {
  String filename = "test";
  animationFile.close();
  animationFile = SD.open(filename, FILE_READ);
}

// Animation object to represent the original Blender animation
BlenderServoAnimation::Animation animation;

void setup() {
  SD.begin(4);

  // Attach the servo to pin 12
  myServo.attach(12);

  // Set the position callback
  animation.onPositionChange(move);

  // Set the scene callback
  animation.onSceneChange(changeFile);

  // Add multiple scenes with the same File stream
  animation.addScene(animationFile, 30, 100);
  animation.addScene(animationFile, 60, 200);

  // Trigger the show loop mode
  animation.loop();

  // There are also other playback options
  // show.play(); // Plays all scenes once in the order they have been added
  // show.playRandom(); // Randomly plays scenes in a loop
  // show.playSingle(1); // Play the scene at the given index once
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
