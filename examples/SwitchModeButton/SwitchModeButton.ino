/*
  Using a button to switch between different modes and control the animation.

  In this example we are using the OneButton library to handle both short and
  long button presses. Note that the animation will be in default mode first,
  meaning that the servo will not move until the button is shortly pressed to
  start the animation.

  We also register a callback which is triggered whenever the animation mode
  changes. For example, this can be used to control the animation audio. See the
  comments below to understand what happens on a short or long button press
  based on the current mode.

  Starting the animation will only play it once, so another button press is
  required to play it again. Alternatively, you can also use MODE_LOOP instead
  of MODE_PLAY and loop() instead of play(). This will keep the animation
  running until the button is short or long pressed again, thus pausing or
  stopping the animation.
*/

#include "simple.h"
#include <BlenderServoAnimation.h>
#include <OneButton.h>

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

// The button instance for switching animation modes
OneButton modeButton(2, true, true);

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

// Callback function which is called whenever the animation mode changes
void modeChanged(byte prevMode, byte newMode) {
  switch (newMode) {
  case BlenderServoAnimation::Animation::MODE_PLAY:
    if (prevMode == BlenderServoAnimation::Animation::MODE_PAUSE) {
      // E.g. resume audio
    } else {
      // E.g. start audio
    }
    break;
  case BlenderServoAnimation::Animation::MODE_PAUSE:
    // E.g. pause audio
    break;
  }
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
    animation.play();
    break;
    // On short press in play mode, we want to pause the animation
  case BlenderServoAnimation::Animation::MODE_PLAY:
    animation.pause();
    break;
    // On short press in pause mode, we want to resume the animation
  case BlenderServoAnimation::Animation::MODE_PAUSE:
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
  // Attach the servo to pin 12
  myServo.attach(12);

  // Attach the callbacks to the mode button
  modeButton.attachClick(onPressed);
  modeButton.attachLongPressStart(onLongPressed);

  // Add the Blender servo object to the animation
  animation.addServo(myBlenderServo);

  // Register the mode change callback function
  animation.onModeChange(modeChanged);
}

void loop() {
  // Update the mode button state on each loop
  modeButton.tick();

  // Update the animation state on each loop
  animation.run();
}
