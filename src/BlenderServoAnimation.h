#include <Arduino.h>
#include "Servo.h"
#include "internal/Command.h"

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
        byte stopStepDelay;

        int frame = 0;
        int frames;

        unsigned long currentMillis;
        unsigned long lastMillis;

        Servo* servos[MAX_SERVOS] = {};
        Stream* serial;
        Command command;

        void handlePlayMode();

        void handleStopMode();

        void handleLiveMode();

    public:
        static const byte MODE_PAUSE = 0;
        static const byte MODE_PLAY = 1;
        static const byte MODE_STOP = 2;
        static const byte MODE_LIVE = 3;

        Animation(byte fps, int frames);

        void addServo(Servo servo);

        void addServos(Servo servos[], byte servoAmount);

        void run(unsigned long currentMillis = millis());

        void play();

        void pause();

        void stop(byte stepDelay = 20);

        void live(Stream &serial);

        byte getMode();
    };
}

#endif