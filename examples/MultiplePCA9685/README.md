# Multiple PCA9685

Using two PCA9685 PWM servo drivers to animate 2 servos.

Note that the A0 address jumper has to be soldered on the second driver board. This setup can easily be extended to animate up to 32 servos. If even more servos are needed, you can also add more driver boards to the chain.

We assume the servo ID and the used board channel are equal. Therefore, the servo with the ID 0 has to be connected to channel 0 etc.

## Library Dependencies

- [Adafruit-PWM-Servo-Driver-Library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library)

## Wiring Diagram

![Arduino Nano with 2 PCA9685](../../images/arduino-nano-with-2-PCA9685.png)
