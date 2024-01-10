#include <Arduino.h>

#ifndef BlenderServoAnimation_LiveStream_H
#define BlenderServoAnimation_LiveStream_H

namespace BlenderServoAnimation {

class LiveStream : public Stream {

public:
  int available();
  int read();
  int peek();
  
  size_t write(uint8_t);

  void flush();

private:
  static const byte BUFFER_SIZE = 64;

  byte buffer[BUFFER_SIZE] = {0};
  byte writeIndex = 0;
  byte readIndex = 0;

};

} // namespace BlenderServoAnimation

#endif