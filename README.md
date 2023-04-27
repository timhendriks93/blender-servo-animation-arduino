# Blender Servo Animation Arduino Library

This library helps to control servos based on an exported Blender animation. It is specifically designed to work with the [Blender Servo Animation Add-on](https://github.com/timhendriks93/blender-servo-animation).

[![Continuous Integration](https://github.com/timhendriks93/blender-servo-animation-arduino/actions/workflows/ci.yml/badge.svg)](https://github.com/timhendriks93/blender-servo-animation-arduino/actions/workflows/ci.yml)

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

// Blender show object
BlenderServoAnimation::Show();
```

When not using the standard servo library, you can use the namespace and therefore skip the namespace prefix:

```ino
using namespace BlenderServoAnimation;

// Blender servo object
Servo(...);

// Blender animation object
Animation(...);

// Blender show object
Show();
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
| threshold | byte | Max allowed position diff (default=0 / no threshold handling) |

> Note: the threshold is also used to define the speed for moving a servo to its neutral position when stopping an animation.

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
Animation myBlenderAnimation(30, 1000);

Servo myBlenderServos[] = {
  Servo(0, BoneA, move),
  Servo(1, BoneB, move),
  Servo(2, BoneC, move),
}

void setup() {
  myBlenderAnimation.addServos(myBlenderServos, 3);
}
```

> Note: the `addServos` function expects the amount of servos in the array to be passed via the second argument.

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

When calling the `stop` method, the threshold values of the animation's servos are considered to control how fast or smooth they are moving towards their neutral position. Keep in mind that the servos will not have a threshold value by default which results in the stop mode to immediately trigger the neutral position of the servos. A slower and safer movement can be achieved by setting the threshold values as low as possible with the actual animation still able to run properly.

To use the `live` method, we have to pass a stream instance which will be used for reading serial commands. For example, we can pass `Serial` if we want to use the standard USB connection of an Arduino compatible board:

```ino
void setup() {
  Serial.begin(115200);
  myBlenderAnimation.live(Serial);
}
```

This library also comes with a `LiveStream` class which allows for a more generic way to listen to live commands. For example, it can be used as part of the web socket based live mode for which you can find a dedicated example [here](examples/WebSocketLiveMode).

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
  // Do something (e.g. using a switch statement)
}

void setup() {
  myBlenderAnimation.onModeChange(modeChanged);
}
```

The [SwitchModeButton example](examples/SwitchModeButton) shows how to combine all mode methods to control an animation based on a single button.

## Defining a Show

A show object allows you to combine multiple animations and control their play back in an easy way. You can also think of a show as a playlist of animations. Since the show object does not expect any arguments, the initialization is very simple:

```ino
Show myBlenderShow;
```

### Registering Animations

After defining some animations as shown above, we have to register them to the show object by calling the `addAnimation` method:

```ino
myBlenderShow.addAnimation(myBlenderAnimation);
```

This is usually done inside the `setup` function after the animation and servo objects have been defined globally (outside of any function like `setup` or `loop`).

Alternatively, we can also create an array of animations and call the `addAnimations` method instead:

```ino
Show myBlenderShow;

Animation animations[3] = {
  {FPS, FRAMES_A},
  {FPS, FRAMES_B},
  {FPS, FRAMES_C},
};

void setup() {
  myBlenderShow.addAnimations(animations, 3);
}
```

> Note: the `addAnimations` function expects the amount of servos in the array to be passed via the second argument.

### Updating the Show State

Just like single animations, we have to regularly trigger the show instance in order to update its state and internally handle the servo movement of the current animation. We therefore need to call the `run` method during each `loop`:

```ino
void loop() {
  myBlenderShow.run();
}
```

### Show Modes

The show modes are similar to the previously mentioned animation modes. In addition to those, there are various playback modes to handle a multitude of animations.

Just like with animation modes, a show will be in the default mode at first. The following table is focusing on the differences and additions of the show modes compared to the animation modes:

| Constant | Method | Description |
|----------|--------|-------------|
| MODE_PLAY | play() | Start or resume playing the show once |
| MODE_PLAY_SINGLE | playSingle(index) | Start or resume playing a single animation once |
| MODE_PLAY_RANDOM | playRandom() | Start or resume randomly playing animations of the show |

The modes can be changed or triggered by calling the respective methods on the show object:

```ino
myBlenderShow.play();
myBlenderShow.playSingle(index);
myBlenderShow.playRandom();
myBlenderShow.pause();
myBlenderShow.loop();
myBlenderShow.stop();
myBlenderShow.live(stream);
```

> Note: the default mode can not be triggered as it is only handled internally.

To get the current show mode, we can again call a `getMode` method. This will return a `byte` representing one of the mode constants mentioned in the table above. We can then compare the return value to those constants to act according to the current mode:

```ino
byte currentMode = myBlenderShow.getMode();

switch (currentMode) {
case Show::MODE_DEFAULT:
  // Do something
  break;
case Show::MODE_PLAY:
  // Do something else
  break;
...
}
```

> Note: The actual byte values of the show modes differ from the animation modes. For example, `Show::MODE_PAUSE != Animation::MODE_PAUSE`.

As with animations, we can also register an `onModeChange` callback function:

```ino
void modeChanged(byte prevMode, byte newMode) {
  // Do something (e.g. using a switch statement)
}

void setup() {
  myBlenderShow.onModeChange(modeChanged);
}
```

There is also a specific [Show example](examples/Show) to illustrate a simple setup based on 2 different animations.

Make sure to also check out the other [examples](examples) to get started more quickly.
