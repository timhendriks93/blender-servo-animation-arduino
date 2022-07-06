#include <Arduino.h>
#include "BlenderServoAnimation.h"
#include "Servo.h"

using namespace BlenderServoAnimation;

Animation::Animation(byte fps, int frames, byte servoThreshold, byte stepDelay)
{
    this->frameMillis = Animation::SECOND_IN_MILLIS / fps;
    this->frames = frames;
    this->servoThreshold = servoThreshold;
    this->stepDelay = stepDelay;
    this->mode = Animation::MODE_PAUSE;
}

void Animation::addServo(Servo servo)
{
    byte id = servo.getID();
    this->servos[id] = &servo;
}

void Animation::addServos(Servo servos[], byte servoAmount)
{
    for (int i = 0; i < servoAmount; i++)
    {
        byte id = servos[i].getID();
        this->servos[id] = &servos[i];
    }
}

void Animation::run(unsigned long currentMillis)
{
    this->currentMillis = currentMillis;

    switch (mode)
    {
    case MODE_PLAY:
        this->handlePlayMode();
        break;
    case MODE_STOP:
        this->handleStopMode();
    case MODE_PAUSE:
    default:
        break;
    }
}

void Animation::handlePlayMode()
{
    if (this->currentMillis - this->lastMillis < this->frameMillis)
    {
        return;
    }

    this->lastMillis = this->currentMillis;
    this->frame++;

    if (this->frame >= this->frames)
    {
        this->frame = 0;
    }

    for (int i = 0; i < MAX_SERVOS; i++)
    {
        Servo* servo = this->servos[i];

        if (servo)
        {
            servo->moveByStep(this->frame);
        }
    }
}

void Animation::handleStopMode()
{
    for (int i = 0; i < MAX_SERVOS; i++)
    {
        Servo* servo = this->servos[i];

        if (!servo || servo->isNeutral())
        {
            continue;
        }

        servo->moveTowardsNeutral();
    }

    if (this->stepDelay > 0)
    {
        delay(this->stepDelay);
    }
}

void Animation::play()
{
    this->mode = Animation::MODE_PLAY;
    this->currentMillis = millis();
    this->lastMillis = this->currentMillis;
}

void Animation::pause()
{
    this->mode = Animation::MODE_PAUSE;
}

void Animation::stop()
{
    this->mode = Animation::MODE_STOP;
}

byte Animation::getMode()
{
    return this->mode;
}
