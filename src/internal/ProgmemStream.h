#include <Arduino.h>

#ifndef BlenderServoAnimation_ProgmemStream_H
#define BlenderServoAnimation_ProgmemStream_H

namespace BlenderServoAnimation {

class ProgmemStream : public Stream {

public:
  ProgmemStream(const byte *data, size_t size);

  int available();
  int read();
  int peek();

  size_t write(uint8_t);

  void flush();

private:
  const byte *data = nullptr;

  size_t size = 0;
  size_t position = 0;
};

} // namespace BlenderServoAnimation

#endif