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

// LiveStream instance acting as a middleware between web socket and animation
// instance
BlenderServoAnimation::LiveStream liveStream;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

// Animation object to represent the original Blender animation
BlenderServoAnimation::Animation animation;

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

  // Set the position callback
  animation.onPositionChange(move);

  // Add a new scene with the LiveStream - we can omit fps and frames
  animation.addScene(liveStream);

  // Trigger the animation play mode
  animation.play();
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
