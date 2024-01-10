#include <Arduino.h>

#ifndef BlenderServoAnimation_Command_H
#define BlenderServoAnimation_Command_H

namespace BlenderServoAnimation {

class Command {

public:
  static const byte LINE_BREAK = 0xA;

  void write(byte value);

  bool isValid();

  byte getServoID();

  int getServoPosition();

private:
  static const byte START_MARKER = 0x3C;
  static const byte END_MARKER = 0x3E;

  static const byte LENGTH = 5;
  static const byte BITS = 8;

  static const byte INDEX_START_MARKER = 0;
  static const byte INDEX_SERVO_ID = 1;
  static const byte INDEX_POSITION_SIG_BYTE = 2;
  static const byte INDEX_POSITION_BYTE = 3;
  static const byte INDEX_END_MARKER = 4;

  byte values[LENGTH] = {0};
  byte index = 0;

  void reset();

};

} // namespace BlenderServoAnimation

#endif