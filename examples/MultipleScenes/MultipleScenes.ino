/*
  Setting up an animation consisting of 2 scenes.
  It's even possible to have different playback rates (fps)
  and frames per animation.
*/

#include "scene-a.h"
#include "scene-b.h"
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

  // Add multiple scenes based on PROGMEM data
  animation.addScene(SceneA::ANIMATION_DATA, SceneA::LENGTH, SceneA::FPS,
                     SceneA::FRAMES);
  animation.addScene(SceneB::ANIMATION_DATA, SceneB::LENGTH, SceneB::FPS,
                     SceneB::FRAMES);

  // Trigger the animation loop mode
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
