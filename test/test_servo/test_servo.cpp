#include <unity.h>
#include "Servo.h"

struct positionLog {
    int index;
    int positions[20];
};

positionLog lastPositions[16];

void setUp(void)
{
    for (int id = 0; id < 16; id++)
    {
        lastPositions[id].index = 0;
        
        for (int i = 0; i < 20; i++)
        {
            lastPositions[id].positions[i] = 0;
        }
    }
}

void tearDown(void)
{
    // clean stuff up here
}

void move(byte servoID, int position)
{
    int index = lastPositions[servoID].index;
    lastPositions[servoID].positions[index] = position;
    lastPositions[servoID].index++;
}

const int PROGMEM positions[5] = {350, 340, 330, 340, 350};

void test_move_by_step(void)
{
    BlenderServoAnimation::Servo servo = BlenderServoAnimation::Servo(2, positions, move);
    TEST_ASSERT_EQUAL(2, servo.getID());
    TEST_ASSERT_TRUE(servo.isNeutral());
    servo.moveByStep(1);
    TEST_ASSERT_EQUAL(340, lastPositions[2].positions[0]);
    servo.moveByStep(2);
    TEST_ASSERT_EQUAL(330, lastPositions[2].positions[1]);
    servo.moveByStep(3);
    TEST_ASSERT_EQUAL(340, lastPositions[2].positions[2]);
}

void test_move_towards_neutral(void)
{
    BlenderServoAnimation::Servo servo = BlenderServoAnimation::Servo(0, positions, move);
    TEST_ASSERT_TRUE(servo.isNeutral());
    servo.moveByStep(1);
    TEST_ASSERT_EQUAL(340, lastPositions[0].positions[0]);
    TEST_ASSERT_FALSE(servo.isNeutral());

    for (int i = 0; i < 10; i++)
    {
        servo.moveTowardsNeutral();
        TEST_ASSERT_EQUAL(341 + i, lastPositions[0].positions[1 + i]);
    }

    TEST_ASSERT_TRUE(servo.isNeutral());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_move_by_step);
    RUN_TEST(test_move_towards_neutral);
    UNITY_END();
}