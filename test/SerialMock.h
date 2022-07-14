#include <Arduino.h>
#include <Print.h>

#ifndef BlenderServoAnimation_Serial_Mock_H
#define BlenderServoAnimation_Serial_Mock_H

class SerialMock : public Stream {
private:
  static const byte BUFFER_SIZE = 10;

  byte buffer[BUFFER_SIZE];
  byte readIndex = 0;
  byte writeIndex = 0;

public:
  int available();

  int read();

  int peek();

  size_t write(uint8_t);
};

#endif