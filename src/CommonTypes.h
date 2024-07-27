#include <Arduino.h>

#ifndef BlenderServoAnimationLibrary_CommonTypes_H
#define BlenderServoAnimationLibrary_CommonTypes_H

typedef void (*ModeCallback)(byte, byte);
typedef void (*PositionCallback)(byte, int);
typedef void (*SceneCallback)(byte, byte);

#endif // BlenderServoAnimationLibrary_CommonTypes_H
