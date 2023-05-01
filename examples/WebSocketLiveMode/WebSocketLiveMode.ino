/*
  Sending live servo positions via web socket commands.

  This example requires an ESP32 and a running Blender instance with the Blender
  Servo Animation Add-on. However, this example could be slightly adjusted to
  work with other WiFi-capable micro controllers as well. We create a web socket
  server to receive live position values via Blender and move a single servo
  which is controlled via the ESP32Servo library.
*/

#include <AsyncWebSocket.h>
#include <BlenderServoAnimation.h>
#include <ESP32Servo.h>
#include <WiFi.h>

// Change to your SSID and password
const char *ssid = "SSID";
const char *password = "PASSWORD";

// Create an asynchronous web socket server
AsyncWebServer server(80);
AsyncWebSocket ws("/");

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

// Animation object to manage the servos
// We skip providing fps or frames as we just want to use the live mode
BlenderServoAnimation::Animation animation;

// Servo object to manage the positions
BlenderServoAnimation::Servo myBlenderServo(0, move);

// Live mode stream instance acting as a middleware between web socket and
// animation object
BlenderServoAnimation::LiveStream liveStream;

// Handler function writing data to the live stream instance when receiving data
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type != WS_EVT_DATA) {
    return;
  }

  for (size_t i = 0; i < len; i++) {
    liveStream.write(data[i]);
  }
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  ws.onEvent(onWebSocketEvent);

  server.addHandler(&ws);
  server.begin();

  // Attach the servo to pin 12
  myServo.attach(12);

  // Add the Blender servo object to the animation
  animation.addServo(myBlenderServo);

  // Trigger the animation live mode
  animation.live(liveStream);
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
