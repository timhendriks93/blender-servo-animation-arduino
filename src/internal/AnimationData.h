#include <Arduino.h>

#ifndef BlenderServoAnimation_AnimationData_H
#define BlenderServoAnimation_AnimationData_H

namespace BlenderServoAnimation {

class AnimationData {

public:
  static const byte LINE_BREAK = 0xA;

  AnimationData();
  AnimationData(const byte *data, int dataSize);
  AnimationData(Stream *stream);

  bool isAvailable();

  byte getNextByte();

  void writeByte(byte value);
  void reset();

private:
  static const byte BUFFER_SIZE = 64;

  int dataSize = 0;
  int dataPosition = 0;

  const byte *data = nullptr;

  Stream *stream = nullptr;

  byte buffer[BUFFER_SIZE] = {0};
  byte writeIndex = 0;
  byte readIndex = 0;

  byte readProgmem();
  byte readBuffer();
};

} // namespace BlenderServoAnimation

#endif