#include <Arduino.h>

#ifndef BlenderServoAnimation_Servo_H
#define BlenderServoAnimation_Servo_H

namespace BlenderServoAnimation {
    class Servo
    {
    private:
        byte id;

        int neutralPosition;
        int currentPosition;

        const int *positions;

        void (*funcptr)(byte, int);
    public:
        Servo(byte id, const int *positions, void (*funcptr)(byte, int));

        void move(int pulse, bool force = false);

        void moveByStep(int step);

        void moveTowardsNeutral(bool inSteps = true);

        bool isNeutral();

        byte getID();
    };
}

#endif