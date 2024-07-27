# Blender Servo Animation Arduino Library

This library helps to control servos based on an exported Blender animation. It is specifically designed to work with the [Blender Servo Animation Add-on](https://github.com/timhendriks93/blender-servo-animation).

[![Continuous Integration](https://github.com/timhendriks93/blender-servo-animation-arduino/actions/workflows/ci.yml/badge.svg)](https://github.com/timhendriks93/blender-servo-animation-arduino/actions/workflows/ci.yml)

## Installation

Please refer to the official [Arduino documentation](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries) to see how you can install this library.

## Quick Start

Take a look at the [StandardServoLib](examples/StandardServoLib) example to get started quickly. It represents the most simple setup and is based on the standard Servo Arduino library with which you might already be familiar.

## Usage

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

### Define the Position Change Callback

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

1. test

| Parameter | Type | Description |
|-----------|------|-------------|
| fps | byte | Frames per second as specified in Blender |
| frames | int | Total amount of frames as specified in Blender |

In general, the provided values should align with the Blender animation you would like to export or work on. Given that the Blender animation consists of 1000 frames and plays back at a rate of 30 frames per second, the animation object should be created as follows:

```ino
Animation myBlenderAnimation(30, 1000);
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
