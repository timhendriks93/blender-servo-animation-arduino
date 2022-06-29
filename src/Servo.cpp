#include <Arduino.h>
#include "Servo.h"

BlenderServoAnimation::Servo::Servo(byte id, const int *positions, void (*funcptr)(byte, int))
{
    this->id = id;
    this->positions = positions;
    this->funcptr = funcptr;
    neutralPosition = pgm_read_word_near(positions + 0);
    currentPosition = neutralPosition;
}

void BlenderServoAnimation::Servo::move(int pulse, bool force)
{
    if (pulse == this->currentPosition && force == false) {
        return;
    }

    if (abs(pulse - currentPosition) > 20)
    {
        Serial.print("Pulse diff too high for servo ");
        Serial.print(id);
        Serial.print(": ");
        Serial.print(currentPosition);
        Serial.print(" -> ");
        Serial.print(pulse);
        Serial.println();
        return;
    }

    this->funcptr(this->id, pulse);
    this->currentPosition = pulse;
}

void BlenderServoAnimation::Servo::moveByStep(int step)
{
    int newPulse = pgm_read_word_near(this->positions + step);

    move(newPulse);
}

void BlenderServoAnimation::Servo::moveTowardsNeutral(bool inSteps)
{
    int newPosition = this->currentPosition;

    if (inSteps == false) {
        move(newPosition, true);
        return;
    }

    if (this->currentPosition > neutralPosition)
    {
        newPosition--;
    }
    else if (this->currentPosition < neutralPosition)
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
