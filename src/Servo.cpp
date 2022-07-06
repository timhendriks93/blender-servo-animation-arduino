#include <Arduino.h>
#include "Servo.h"

using namespace BlenderServoAnimation;

Servo::Servo(byte id, const int positions[], void (*moveCallback)(byte, int), byte threshold)
{
    this->id = id;
    this->positions = positions;
    this->moveCallback = moveCallback;
    this->threshold = threshold;
    this->neutralPosition = pgm_read_word_near(positions + 0);
    this->currentPosition = this->neutralPosition;
}

void Servo::move(int position, bool force)
{
    if (position == this->currentPosition && force == false) {
        return;
    }

    if (abs(position - this->currentPosition) > this->threshold)
    {
        Serial.print("Position diff for servo ");
        Serial.print(this->id);
        Serial.print(" exceeded: ");
        Serial.print(this->currentPosition);
        Serial.print(" -> ");
        Serial.print(position);
        Serial.println();
        return;
    }

    this->moveCallback(this->id, position);
    this->currentPosition = position;
}

void Servo::moveByStep(int step)
{
    int newPosition = pgm_read_word_near(this->positions + step);

    this->move(newPosition);
}

void Servo::moveTowardsNeutral(bool inSteps)
{
    int newPosition = this->currentPosition;

    if (inSteps == false) {
        this->move(newPosition, true);
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

    this->move(newPosition);
}

bool Servo::isNeutral()
{
    return this->currentPosition == this->neutralPosition;
}

byte Servo::getID()
{
    return this->id;
}
