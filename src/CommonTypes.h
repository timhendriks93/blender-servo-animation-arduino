#include <Arduino.h>

#ifndef BlenderServoAnimationLibrary_CommonTypes_H
#define BlenderServoAnimationLibrary_CommonTypes_H

namespace BlenderServoAnimationLibrary {

typedef void (*ModeCallback)(byte, byte); // Mode callback
typedef void (*PositionCallback)(byte, int);  // Position callback
typedef void (*SceneCallback)(byte, byte); // Scene callback

} // BlenderServoAnimationLibrary

#endif
