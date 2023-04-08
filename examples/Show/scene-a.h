/*
  Blender Servo Animation Positions

  FPS: 30
  Frames: 100
  Seconds: 3
  Bones: 1
  Armature: Armature
  Scene: SceneA
  File: scenes.blend
*/

#include <Arduino.h>

namespace SceneA {

const byte FPS = 30;
const int FRAMES = 100;

// Servo ID: 0
const int Bone[FRAMES] PROGMEM = {
  90, 90, 89, 89, 88, 87, 86, 84, 83, 81, 80, 78, 76, 74, 72, 70, 68, 65, 63, 61, 59, 57, 55, 54, 52, 51, 49, 48, 47, 46, 46, 45, 45, 45, 46, 47, 49, 51, 53, 55, 58, 61, 65, 68, 72, 76, 80, 84, 88, 92,
  96, 100, 104, 108, 112, 115, 119, 122, 125, 127, 129, 131, 133, 134, 135, 135, 135, 135, 134, 133, 132, 131, 130, 129, 127, 126, 124, 122, 120, 118, 116, 114, 112, 111, 109, 107, 105, 103, 101, 99, 98, 96, 95, 94, 93, 92, 91, 90, 90, 90,
};

} // namespace SceneA
