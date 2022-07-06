#include <Arduino.h>

#ifndef BlenderServoAnimation_Servo_H
#define BlenderServoAnimation_Servo_H

namespace BlenderServoAnimation {
    class Servo
    {
    private:
        byte id;
        byte threshold;

        int neutralPosition;
        int currentPosition;

        const int* positions;

        void (*moveCallback)(byte, int);
    public:
        Servo(byte id, const int positions[], void (*moveCallback)(byte, int), byte threshold = 20);

        void move(int position, bool force = false);

        void moveByStep(int step);

        void moveTowardsNeutral(bool inSteps = true);

        bool isNeutral();

        byte getID();
    };
}

#endif