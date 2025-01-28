/*
  Sending live servo positions via web socket commands.

  This example requires an ESP32 and a running Blender instance with the Blender Servo Animation
  Add-on. We create a web socket server to receive live position values via Blender and move a
  single servo.
*/

#include <AsyncWebSocket.h>
#include <BlenderServoAnimation.h>
#include <ESP32Servo.h>
#include <WiFi.h>

#define SERVO_PIN 12
#define PORT 80
#define PATH "/"

// Change to your SSID and password
const char *ssid = "SSID";
const char *password = "PASSWORD";

// Create an asynchronous web socket server
AsyncWebServer server(PORT);
AsyncWebSocket ws(PATH);

// Servo object to send positions
Servo myServo;

// Callback function which is called whenever a servo needs to be moved
void move(byte servoID, int position) {
  // Ignore the servoID (there is only one servo) and write the current position
  myServo.writeMicroseconds(position);
}

// Animation object to control the animation
BlenderServoAnimation animation;

// Callback function writing live stream data to the animation
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                      void *arg, uint8_t *data, size_t len) {
  if (type != WS_EVT_DATA) {
    return;
  }

  for (size_t i = 0; i < len; i++) {
    animation.writeLiveStream(data[i]);
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  };

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Register the web socket callback function
  ws.onEvent(onWebSocketEvent);

  // Add the web socket handler and start the server
  server.addHandler(&ws);
  server.begin();

  // Attach the servo to the defined servo pin
  myServo.attach(SERVO_PIN);

  // Set the position callback
  animation.onPositionChange(move);

  // Trigger the animation live mode
  animation.live();
}

void loop() {
  // Update the animation state on each loop
  animation.run();
}
