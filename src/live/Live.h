#include <Arduino.h>

#ifndef BlenderServoAnimation_Live_H
#define BlenderServoAnimation_Live_H

namespace BlenderServoAnimation {

class Live : public Stream {
private:
  static const byte BUFFER_SIZE = 20;

  byte buffer[BUFFER_SIZE];
  byte writeIndex = 0;
  byte readIndex = 0;

public:
  int available();

  int read();

  int peek();

  size_t write(uint8_t);

  void flush();
};

} // namespace BlenderServoAnimation

#endif