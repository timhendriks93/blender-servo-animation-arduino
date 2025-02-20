/*
  Reading animation data from an SD card.

  The setup requires nothing but a micro controller and a single servo. It uses the standard Arduino
  servo library to send servo positions.
*/

#include <BlenderServoAnimation.h>
#include <SD.h>

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

#define SERVO_PIN 3
#define CS_PIN 4
#define FPS 30
#define FRAMES 100

// Servo object to send positions
Servo myServo;

// File object to read animation data
File animationFile;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

// Callback function which is called whenever a scene is being changed
void resetFile(byte prevSceneIndex, byte nextSceneIndex) {
  // Seek back to the first position in the file
  animationFile.seek(0);
}

// Animation object to control the animation
BlenderServoAnimation animation;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  };

  Serial.println("Initializing SD card...");

  if (!SD.begin(CS_PIN)) {
    Serial.println("Initialization failed!");
    while (true) {
    };
  }
  Serial.println("Initialization done.");

  animationFile = SD.open("simple.bin");

  if (animationFile) {
    Serial.println("File opened successfully.");
  } else {
    Serial.println("Opening file failed.");
  }

  // Attach the servo to the defined servo pin
  myServo.attach(SERVO_PIN);

  // Set the position and scene change callback
  animation.onPositionChange(move);
  animation.onSceneChange(resetFile);

  // Add a scene with the File stream
  animation.addScene(animationFile, FPS, FRAMES);

  // Trigger the animation loop mode
  animation.loop();
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
