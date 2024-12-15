// ESP32 Side A

#include <WiFi.h>
#include <PubSubClient.h>

const int ldrPin = 39;
const int trigger = 23;
const int echo = 21;
const int magnetic = 34;

const char* ssid = "Suda";
const char* password = "";
const char* mqtt_server = "broker.emqx.io";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32dust-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("output/dustsensors", "Dust's ESP32 connected");
      client.subscribe("output/dustsensors");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(magnetic, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  int value1 = analogRead(ldrPin);
  float percentage = ( value1 / 4095.0 ) * 100;         
  Serial.print("Light percentage: ");
  Serial.print(percentage);
  Serial.print(" || ");
  Serial.println(value1);

  static bool lightsOn = false;

  if (percentage > 50 && lightsOn) {
    snprintf(msg, MSG_BUFFER_SIZE, "Turn off lights");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("output/dustsensors", msg);
    lightsOn = false;
  } else if (percentage <= 50 && !lightsOn) {
    snprintf(msg, MSG_BUFFER_SIZE, "Turn on lights");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("output/dustsensors", msg);
    lightsOn = true;
  }

  static float prevDistance = 0;
  long duration;
  float distance;

  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (abs(distance - prevDistance) > 5) {
    snprintf(msg, MSG_BUFFER_SIZE, "Movement detected");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("output/dustsensors", msg);
  }

  prevDistance = distance;

  int magneticValue = digitalRead(magnetic);
  bool magneticState = false;

  if (magneticValue == HIGH && !magneticState) {
    snprintf(msg, MSG_BUFFER_SIZE, "Door opened");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("output/dustsensors", msg);
    magneticState = true;
  } else if (magneticValue == LOW && magneticState) {
    snprintf(msg, MSG_BUFFER_SIZE, "Door closed");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("output/dustsensors", msg);
    magneticState = false;
  }

  delay(5000);
}