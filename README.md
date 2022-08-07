# Blender Servo Animation Arduino Library

This library helps to control servos based on an exported Blender animation. It is specifically designed to work with the [Blender Servo Animation Add-on](https://github.com/timhendriks93/blender-servo-animation).

## Installation

Please refer to the official [Arduino documentation](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries) to see how you can install this library.

## Usage

To start using this library, add the following include statement to your script or sketch:

```ino
#include <BlenderServoAnimation.h>
```

## Namespace

To avoid naming conflicts, the library uses the `BlenderServoAnimation` namespace. For example, this allows to use the standard Arduino Servo library to control the servos while using the `Servo` class of this library to represent the servo within the Blender animation:

```ino
// Standard library servo object
Servo(...);

// Blender servo object
BlenderServoAnimation::Servo(...);

// Blender animation object
BlenderServoAnimation::Animation(...);
```

When not using the standard servo library, you can use the namespace and therefore skip the namespace prefix:

```ino
using namespace BlenderServoAnimation;

// Blender servo object
Servo(...);

// Blender animation object
Animation(...);
```

## Defining Servos

Before we can play back and control an animation, we first have to create and attach representations of the animated servos. There are 4 possible syntaxes to create a new servo object:

```ino
Servo(id, positions, callback);
Servo(id, positions, callback, threshold);
Servo(id, callback);
Servo(id, callback, threshold);
```

> Note: servos without positions will only be considered when in live mode.

### Servo Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| id | byte | Unique servo ID as specified via the Add-on |
| positions | const&#160;int[] | Exported positions per frame |
| callback | void&#160;(byte,&#160;int) | Function to trigger when a servo is moved |
| threshold | byte | Max allowed position diff (default=20) |

### Callback Function

The callback function is used to specify what should happen when a servo needs to be moved to a new position. It will be automatically triggered by the animation instance and receives 2 arguments - the servo ID as `byte` and the new position as `int`:

```ino
void myServoCallback(byte servoID, int position) {
  // Do something
}
```

This allows to implement any kind of logic to handle the actual servo control. When using this library outside of the Arduino IDE, it is recommended to define the callback function first before passing it as an argument when creating a servo object:

```ino
#include "simple.h"
#include <BlenderServoAnimation.h>

using namespace BlenderServoAnimation;

void move(byte servoID, int position) {
  // Do something
}

Servo myBlenderServo(0, Bone, move);
```

## Defining an Animation

The animation object serves as a control instance to play back the servo movement. Just like in Blender, an animation can be played, paused and stopped. To do so, we need to provide information about the speed and length of the animation. To just use control servos via the live mode (serial connection), we can omit this information. Therefore, an animation can be created via the following 2 syntaxes:

```ino
Animation();
Animation(fps, frames);
```

> Note: animations without fps and frames will only be able to handle the live mode.

### Animation Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| fps | byte | Frames per second as specified in Blender |
| frames | int | Total amount of frames as specified in Blender |

In general, the provided values should align with the Blender animation you would like to export or work on. Given that the Blender animation consists of 1000 frames and plays back at a rate of 30 frames per second, the animation object should be created as follows:

```ino
Animation myBlenderAnimation(30, 1000);
```

### Registering Servos

To actually trigger servo movement, the animation needs to know about the individual servos. After defining the servos as mentioned above, we therefore have to register them to the animation by calling the `addServo` method:

```ino
myBlenderAnimation.addServo(myBlenderServo);
```

This is usually done inside the `setup` function after the servo objects have been defined globally (outside of any function like `setup` or `loop`).

Alternatively, we can also create an array of servos and call the `addServos` method instead:

```ino
Servo myBlenderServos[] = {
  Servo(0, BoneA, move),
  Servo(1, BoneB, move),
  Servo(2, BoneC, move),
  ...
}

Animation myBlenderAnimation(30, 1000);

void setup() {
  myBlenderAnimation.addServos(myBlenderServos);
}
```

### Updating the Animation State

The animation needs to be triggered regularly in order to update its state and check if any servos have to be moved. We therefore need to call the `run` method during each `loop`:

```ino
void loop() {
  myBlenderAnimation.run();
}
```

### Animation Modes

At first, an animation will be in the default mode. In this mode, the animation is simply not doing anything and waits until the mode has changed.

| Constant | Method | Description |
|----------|--------|-------------|
| MODE_DEFAULT | | Not playing / waiting |
| MODE_PLAY | play() | Start or resume playing the animation once |
| MODE_PAUSE | pause() | Pausing the animation at the current frame |
| MODE_STOP | stop() | Slowly moving the servos to their neutral position |
| MODE_LOOP | loop() | Start or resume playing the animation in a loop |
| MODE_LIVE | live(stream) | Reading serial commands to move the servos in real-time |

The modes can be changed or triggered by calling the above methods on the animation object:

```ino
myBlenderAnimation.play();
myBlenderAnimation.pause();
myBlenderAnimation.loop();
myBlenderAnimation.stop();
myBlenderAnimation.live(stream);
```

> Note: the default mode can not be triggered as it is only handled internally.

When calling the `stop` method, it is possible to pass a delay in milliseconds. This delay will be used when the servos are moved to their neutral position during the stop mode. To get to the neutral position, the current position will either be increased or decreased by 1. The delay therefore controls how fast or smooth this movement will take place. The default value for this parameter is `20`.

```ino
myBlenderAnimation.stop(); // Default reset speed applied
myBlenderAnimation.stop(30); // Servos will reset slower
```

To use the `live` method, we have to pass a stream instance which will be used for reading serial commands. For example, we can pass `Serial` if we want to use the standard USB connection of an Arduino compatible board:

```ino
void setup() {
  Serial.begin(115200);
  myBlenderAnimation.live(Serial);
}
```

To get the current animation mode, we can simply call the `getMode` method. This will return a `byte` representing one of the mode constants mentioned in the table above. We can then compare the return value to those constants to act according to the current mode:

```ino
byte currentMode = myBlenderAnimation.getMode();

switch (currentMode) {
case Animation::MODE_DEFAULT:
  // Do something
  break;
case Animation::MODE_PLAY:
  // Do something else
  break;
...
}
```

On top of manually checking the animation mode, we can also register a callback function which is triggered as soon as the animation mode has changed. The function will receive both the previous mode and the new mode as `byte` values. To register the function, we can call the `onModeChange` method:

```ino
void modeChanged(byte prevMode, byte newMode) {
  // Do something (e.g. using another switch)
}

void setup() {
  myBlenderAnimation.onModeChange(modeChanged);
}
```

The [SwitchModeButton example](examples/SwitchModeButton) shows how to combine all mode methods to control an animation based on a single button. Make sure to also check out the other [examples](examples) to get started quickly.
