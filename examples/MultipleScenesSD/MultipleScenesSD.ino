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

#define SERVO_PIN 3
#define CS_PIN 4

// Servo object to send positions
Servo myServo;

// File object to read animation data
File animationFile;

// We use a struct to map a scene to a filename
struct sceneMapping {
  byte fps;
  int frames;
  String filename;
};

// Define an array of scene maps
sceneMapping sceneMappings[] = {
    // Scene 0 = Scene A
    {30, 100, "scene-a.bin"},

    // Scene 1 = Scene B
    {60, 200, "scene-b.bin"},
};

// Calculate the amount of scenes so that we can easily extend the array
const byte sceneAmount = sizeof(sceneMappings) / sizeof(sceneMappings[0]);

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

void changeSceneFile(byte prevSceneIndex, byte nextSceneIndex) {
  String filename = sceneMappings[nextSceneIndex].filename;

  animationFile.close();
  animationFile = SD.open(filename);

  if (!animationFile) {
    Serial.println("Opening file failed.");
  }
}

// Animation object to represent the original Blender animation
BlenderServoAnimation::Animation animation;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Initializing SD card...");

  if (!SD.begin(CS_PIN)) {
    Serial.println("Initialization failed!");
    while (true);
  }
  Serial.println("Initialization done.");

  // Attach the servo to pin 12
  myServo.attach(SERVO_PIN);

  // Set the position and scene change callback
  animation.onPositionChange(move);
  animation.onSceneChange(changeSceneFile);

  // Add multiple scenes with the same File stream
  for (byte i = 0; i < sceneAmount; i++) {
    byte fps = sceneMappings[i].fps;
    int frames = sceneMappings[i].frames;

    animation.addScene(animationFile, fps, frames);
  }

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
