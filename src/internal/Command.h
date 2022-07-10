#include <Arduino.h>

#ifndef Command_H
#define Command_H

namespace BlenderServoAnimation {
    class Command
    {
    private:
        static const byte START_MARKER = 0x3C;
        static const byte END_MARKER = 0x3E;

        static const byte LENGTH = 5;
        static const byte BITS = 8;
        
        static const byte INDEX_SERVO_ID = 1;
        static const byte INDEX_POSITION_SIG_BYTE = 2;
        static const byte INDEX_POSITION_BYTE = 3;

        byte receivedBytes[LENGTH];
        byte receivedIndex = 0;

        bool receiving = false;
        bool complete = false;

    public:
        void read(Stream* serial);

        bool isValid();

        bool isComplete();

        byte getServoID();

        int getServoPosition();
    };
}

#endif