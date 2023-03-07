/*
  Blender Servo Animation Positions

  FPS: 60
  Frames: 200
  Seconds: 3
  Bones: 1
  Armature: Armature.001
  Scene: SceneB
  File: scenes.blend
*/

#include <Arduino.h>

namespace SceneB {

const byte FPS = 60;
const int FRAMES = 200;

// Servo ID: 0
const int Bone[FRAMES] PROGMEM = {
  90, 90, 90, 90, 90, 90, 90, 89, 89, 89, 89, 88, 88, 88, 88, 87, 87, 86, 86, 86, 85, 85, 84, 84, 83, 83, 82, 82, 81, 81, 80, 80, 79, 78, 78, 77, 76, 76, 75, 75, 74, 73, 73, 72, 71, 71, 70, 69, 69, 68,
  67, 66, 66, 65, 64, 64, 63, 62, 62, 61, 60, 60, 59, 59, 58, 57, 57, 56, 55, 55, 54, 54, 53, 53, 52, 52, 51, 51, 50, 50, 49, 49, 49, 48, 48, 47, 47, 47, 47, 46, 46, 46, 46, 45, 45, 45, 45, 45, 45, 45,
  45, 45, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 58, 58, 59, 60, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67, 68,
  68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 75, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 87, 88, 88, 88, 88, 89, 89, 89, 89, 90, 90, 90, 90, 90, 90, 90,
};

} // namespace SceneB
