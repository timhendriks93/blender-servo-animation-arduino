/*
  Setting up a show consisting of 2 animations.

  Note the namespaces which are used to distinguish the positions
  of one scene / animation from another. It's even possible to
  have different playback rates (fps) and frames per animation.
*/

#include "scene-a.h"
#include "scene-b.h"
#include <BlenderServoAnimation.h>
#include <Servo.h>

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int angle) {
  // Ignore the servoID (there is only one servo) and write the current angle
  myServo.write(angle);
}

// Show object to manage all Blender animations
BlenderServoAnimation::Show show;

// Animation objects to represent the original Blender animations
BlenderServoAnimation::Animation animationA(0, SceneA::FPS, SceneA::FRAMES);
BlenderServoAnimation::Animation animationB(1, SceneB::FPS, SceneB::FRAMES);

// Servo objects to manage the positions
BlenderServoAnimation::Servo myBlenderServoA(0, SceneA::Bone, move);
BlenderServoAnimation::Servo myBlenderServoB(0, SceneB::Bone, move);

void setup() {
  // Attach the servo to pin 9
  myServo.attach(9);

  // Add the Blender servo objects to the animations
  animationA.addServo(myBlenderServoA);
  animationB.addServo(myBlenderServoB);

  // Add the Blender animation objects to the show
  show.addAnimation(animationA);
  show.addAnimation(animationB);

  // Trigger the show loop mode
  show.loop();
  
  // There are also other playback options
  // show.play(); // Plays all animations once in the order they have been added
  // show.playRandom(); // Randomly plays animations in a loop
  // show.playSingle(1); // Play the animation with the given ID once

  // Further controls are similar to what we can do on animation objects
  // show.pause();
  // show.stop();
  // show.live(Serial);
}

void loop() {
  // Update the show state on each loop
  show.run();
}
