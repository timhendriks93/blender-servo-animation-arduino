#include <Arduino.h>
#include "Servo.h"

BlenderServoAnimation::Servo::Servo(byte id, const int *positions, void (*funcptr)(byte, int), byte threshold)
{
    this->id = id;
    this->positions = positions;
    this->funcptr = funcptr;
    this->threshold = threshold;
    neutralPosition = pgm_read_word_near(positions + 0);
    currentPosition = neutralPosition;
}

void BlenderServoAnimation::Servo::move(int position, bool force)
{
    if (position == this->currentPosition && force == false) {
        return;
    }

    if (abs(position - this->currentPosition) > this->threshold)
    {
        Serial.print("Pulse diff too high for servo ");
        Serial.print(id);
        Serial.print(": ");
        Serial.print(this->currentPosition);
        Serial.print(" -> ");
        Serial.print(position);
        Serial.println();
        return;
    }

    this->funcptr(this->id, position);
    this->currentPosition = position;
}

void BlenderServoAnimation::Servo::moveByStep(int step)
{
    int newPosition = pgm_read_word_near(this->positions + step);

    move(newPosition);
}

void BlenderServoAnimation::Servo::moveTowardsNeutral(bool inSteps)
{
    int newPosition = this->currentPosition;

    if (inSteps == false) {
        move(newPosition, true);
        return;
    }

    if (this->currentPosition > this->neutralPosition)
    {
        newPosition--;
    }
    else if (this->currentPosition < this->neutralPosition)
    {
        newPosition++;
    }

    move(newPosition);
}

bool BlenderServoAnimation::Servo::isNeutral()
{
    return this->currentPosition == this->neutralPosition;
}

byte BlenderServoAnimation::Servo::getID()
{
    return this->id;
}
