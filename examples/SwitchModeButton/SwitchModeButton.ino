/*
  Using a button to switch between different modes and control the animation.

  In this example we are using the OneButton library to handle both short and
  long button presses. Note that the animation will be in default mode first,
  meaning that the servo will not move until the button is pressed shortly to
  start the animation. See the comments below to understand what happens when
  pressing the button short or long according to the current mode.
*/

#include "simple.h"
#include <BlenderServoAnimation.h>
#include <OneButton.h>
#include <Servo.h>

// Frames per second - see original Blender animation / simple.h
#define FPS 30

// Total animation frames - see original Blender animation / simple.h
#define FRAMES 100

// The button instance for switching animation modes
OneButton modeButton(2, false, false);

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

// Callback to be triggered on a short button press
void onPressed() {
  // Get the current mode, act accordingly and trigger another mode
  switch (animation.getMode()) {
    // On short press in default mode, we want to start the animation
  case BlenderServoAnimation::Animation::MODE_DEFAULT:
    // Hint: here you could also trigger some audio
    animation.play();
    break;
    // On short press in play mode, we want to pause the animation
  case BlenderServoAnimation::Animation::MODE_PLAY:
    animation.pause();
    // Hint: here you could also pause some audio
    break;
    // On short press in pause mode, we want to resume the animation
  case BlenderServoAnimation::Animation::MODE_PAUSE:
    // Hint: here you could also resume some audio
    animation.play();
    break;
  }
}

// Callback to be triggered on a long button press
void onLongPressed() {
  // Get the current mode, act accordingly and trigger another mode
  switch (animation.getMode()) {
    // On long press in default mode, we want to trigger the live mode
  case BlenderServoAnimation::Animation::MODE_DEFAULT:
    animation.live(Serial);
    break;
    // On long press in any other mode, we want to stop the animation
  case BlenderServoAnimation::Animation::MODE_PLAY:
  case BlenderServoAnimation::Animation::MODE_PAUSE:
  case BlenderServoAnimation::Animation::MODE_LIVE:
    animation.stop();
    break;
  }
}

void setup() {
  // Attach the servo to pin 9
  myServo.attach(9);

  // Attach the callbacks to the mode button
  modeButton.attachClick(onPressed);
  modeButton.attachLongPressStart(onLongPressed);

  // Add the Blender servo object to the animation
  animation.addServo(myBlenderServo);
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
