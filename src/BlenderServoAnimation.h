#include <Arduino.h>
#include "Servo.h"

#ifndef BlenderServoAnimation_H
#define BlenderServoAnimation_H

namespace BlenderServoAnimation {
    class Animation
    {
    private:
        static const byte MAX_SERVOS = 255;
        static const int SECOND_IN_MILLIS = 1000;

        byte frameMillis;
        byte mode;
        byte servoThreshold;
        byte stepDelay;

        int frame = 0;
        int frames;

        unsigned long currentMillis;
        unsigned long lastMillis;

        Servo* servos[MAX_SERVOS] = {};

        void handlePlayMode();

        void handleStopMode();

    public:
        static const byte MODE_PAUSE = 0;
        static const byte MODE_PLAY = 1;
        static const byte MODE_STOP = 2;
        static const byte MODE_LIVE = 3;

        Animation(byte fps, int frames, byte servoThreshold = 20, byte stepDelay = 20);

        void addServo(Servo servo);

        void addServos(Servo servos[], byte servoAmount);

        void run(unsigned long currentMillis = millis());

        void play();

        void pause();

        void stop();

        byte getMode();
    };
}

#endif