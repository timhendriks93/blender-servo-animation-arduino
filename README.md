# Blender Servo Animation Arduino Library

This library helps to control servos based on an exported Blender animation. It is specifically designed to work with the [Blender Servo Animation Add-on](https://github.com/timhendriks93/blender-servo-animation).

[![Continuous Integration](https://github.com/timhendriks93/blender-servo-animation-arduino/actions/workflows/ci.yml/badge.svg)](https://github.com/timhendriks93/blender-servo-animation-arduino/actions/workflows/ci.yml)

## First Steps

### Installation

Please refer to the official [Arduino documentation](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries) to see how you can install this library.

### Quick Start

Take a look at the [StandardServoLib](examples/StandardServoLib) example to get started quickly. It represents the most simple setup and is based on the standard Servo Arduino library with which you might already be familiar.

### Usage

To start using this library, add the following include statement to your script or sketch:

```ino
#include <BlenderServoAnimation.h>
```

## Defining an Animation

The animation object serves as a control instance to play back the servo movement. Just like in Blender, an animation can consist of multiple scenes and can be triggered to play, pause and stop.

Start simply by creating a new animation instance at the outer scope of your sketch (outside of `setup` and `loop`):

```ino
BlenderServoAnimation animation;
```

### Register a Position Change Callback

To specify what should happen when a servo needs to be moved to a new position, we have to define a callback function. It receives 2 arguments - the servo ID as `byte` and the new position as `int`:

```ino
void move(byte servoID, int position) {
  // Custom logic to move the servo
}
```

This allows the implementation of any logic to handle the actual servo control. For example, you can make use of the standard Servo library for simple setups or add logic to control servos via PWM control boards such as the PCA9685.

After defining the callback function, we need to register it via the `onPositionChange` method of the animation instance:

```ino
animation.onPositionChange(move);
```

### Adding Scenes and Animation Data

Before we can play back and control an animation, we first have to add at least one scene to our animation. There are 2 ways to do this depending on the type of animation data you want to provide:

```ino
animation.addScene(data, size, fps, frames);
animation.addScene(stream, fps, frames);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| data | const byte[] | Exported position data |
| stream | Stream | Stream instance to read positions from |
| size | int | Size of the position data |
| fps | byte | Frames per second as specified in Blender |
| frames | int | Total amount of frames as specified in Blender |

> Note: make sure the `fps` and `frames` values align exactly with the respective settings of the animation in Blender.

### Updating the Animation State

The animation needs to be triggered regularly in order to update its state and check if any servos have to be moved. We therefore need to call the `run` method during each `loop`:

```ino
void loop() {
  animation.run();
}
```

## Controlling an Animation

### Animation Modes

At first, an animation will be in the default mode. In this mode, the animation is simply not doing anything and waits until the mode has changed.

| Constant | Method | Description |
|----------|--------|-------------|
| MODE_DEFAULT | n/a | Not playing / waiting |
| MODE_PLAY | play() | Start or resume playing all scenes once |
| MODE_PLAY_SINGLE | playSingle(index) | Start or resume playing a single scene once |
| MODE_PLAY_RANDOM | playRandom() | Start or resume randomly playing scenes |
| MODE_PAUSE | pause() | Pausing the scene at the current frame |
| MODE_STOP | stop() | Slowly moving the servos to their starting position |
| MODE_LOOP | loop() | Start or resume playing all scenes in a loop |
| MODE_LIVE | live()/live(stream) | Reading serial commands to move the servos in real-time |

The modes can be changed or triggered by calling the above methods on the animation object:

```ino
animation.play();
animation.playSingle(index);
animation.playRandom();
animation.pause();
animation.loop();
animation.stop();
animation.live(stream);
```

> Note: the default mode can not be triggered as it is only handled internally.

When calling the `stop` method, the threshold values of the animation's servos are considered to control how fast or smooth they are moving towards their neutral position. Keep in mind that the servos will not have a threshold value by default which results in the stop mode to immediately trigger the neutral position of the servos. A slower and safer movement can be achieved by setting the threshold values as low as possible with the actual animation still able to run properly.

### Live Mode

To use the `live` method, we have to pass a stream instance which will be used for reading serial commands. For example, we can pass `Serial` if we want to use the standard USB connection of an Arduino compatible board:

```ino
void setup() {
  Serial.begin(115200);
  animation.live(Serial);
}
```

### React to Mode Changes

To get the current animation mode, we can simply call the `getMode` method. This will return a `byte` representing one of the mode constants mentioned in the table above. We can then compare the return value to those constants to act according to the current mode:

```ino
byte currentMode = animation.getMode();

switch (currentMode) {
case BlenderServoAnimation::MODE_DEFAULT:
  // Do something
  break;
case BlenderServoAnimation::MODE_PLAY:
  // Do something else
  break;
...
}
```

On top of manually checking the animation mode, we can also register a callback function which is triggered as soon as the animation mode has changed. The function will receive both the previous mode and the new mode as `byte` values. To register the function, we can call the `onModeChange` method:

```ino
void modeChanged(byte prevMode, byte newMode) {
  // Do something (e.g. using a switch statement)
}

void setup() {
  animation.onModeChange(modeChanged);
}
```

### React to Scene Changes

To execute logic whenever there is a transition into a different scene, another callback function can be registered. The function will receive the index of both the previous and the new scene as `byte` values. To register the function, we can call the `onSceneChange` method:

```ino
void sceneChanged(byte prevSceneIndex, byte newSceneIndex) {
  // Do something (e.g. opening another animation file on an SD card)
}

void setup() {
  animation.onSceneChange(sceneChanged);
}
```

## Examples

Make sure to also check out the other [examples](examples) to get started more quickly.
