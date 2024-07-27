/*
  Using a button to switch between different modes and control the animation.

  We are using a button to handle both short and long presses. Note that the servo will not move
  until the button is shortly pressed to start the animation.

  Starting the animation will only play it once, so another button press is required to play it
  again. A long press will stop the animation and slowly move the servo to its neutral position.
*/

#include "simple.h"
#include <BlenderServoAnimation.h>
#include <OneButton.h>

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

#define BUTTON_PIN 2
#define SERVO_PIN 3

// The button instance for switching animation modes
OneButton modeButton(BUTTON_PIN, true, true);

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
  case BlenderServoAnimation::MODE_PLAY:
    if (prevMode == BlenderServoAnimation::MODE_PAUSE) {
      // E.g. resume audio
    } else {
      // E.g. start audio
    }
    break;
  case BlenderServoAnimation::MODE_PAUSE:
    // E.g. pause audio
    break;
  case BlenderServoAnimation::MODE_STOP:
    // E.g. stop audio
    break;
  }
}

// Animation object to control the animation
BlenderServoAnimation animation;

// Callback to be triggered on a short button press
void onPressed() {
  // Get the current mode, act accordingly and trigger another mode
  switch (animation.getMode()) {
    // On short press in default or pause mode, we want to start or resume the
    // animation
  case BlenderServoAnimation::MODE_DEFAULT:
  case BlenderServoAnimation::MODE_PAUSE:
    animation.play();
    break;
    // On short press in play mode, we want to pause the animation
  case BlenderServoAnimation::MODE_PLAY:
    animation.pause();
    break;
  }
}

// Callback to be triggered on a long button press
void onLongPressed() {
  // Get the current mode, act accordingly and trigger another mode
  switch (animation.getMode()) {
    // On long press in play, pause or live mode, we want to stop the animation
  case BlenderServoAnimation::MODE_PLAY:
  case BlenderServoAnimation::MODE_PAUSE:
    animation.stop();
    break;
  }
}

void setup() {
  // Attach the servo to the defined servo pin
  myServo.attach(SERVO_PIN);

  // Attach the callbacks to the mode button
  modeButton.attachClick(onPressed);
  modeButton.attachLongPressStart(onLongPressed);

  // Set the position callback
  animation.onPositionChange(move);

  // Add a scene based on PROGMEM data
  animation.addScene(ANIMATION_DATA, LENGTH, FPS, FRAMES);

  // Register the mode change callback function
  animation.onModeChange(modeChanged);
}

void loop() {
  // Update the mode button state on each loop
  modeButton.tick();

  // Update the animation state on each loop
  animation.run();
}
