#include <Arduino.h>
#include "SerialMock.h"

int SerialMock::available()
{
    return this->writeIndex != this->readIndex;
}

int SerialMock::peek()
{
    return this->buffer[this->readIndex];
}

int SerialMock::read()
{
    int value = this->buffer[this->readIndex];
    this->readIndex++;

    if (this->readIndex >= BUFFER_SIZE)
    {
        this->readIndex = 0;
    }

    return value;
}

size_t SerialMock::write(uint8_t value)
{
    this->buffer[this->writeIndex] = value;
    this->writeIndex++;

    if (this->writeIndex >= BUFFER_SIZE)
    {
        this->writeIndex = 0;
    }

    return 1;
}