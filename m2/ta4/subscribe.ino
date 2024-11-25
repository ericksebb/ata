// ESP32 Side B

#include <WiFi.h>
#include <PubSubClient.h>

const int lightPin = 2;
const int buzzerPin = 4;

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* mqtt_server = "your_MQTT_BROKER_IP";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(lightPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(lightPin, LOW);
  digitalWrite(buzzerPin, LOW);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  if (message == "Turn on lights") {
    digitalWrite(lightPin, HIGH);
  } else if (message == "Turn off lights") {
    digitalWrite(lightPin, LOW);
  } else if (message == "Movement detected") {
    digitalWrite(buzzerPin, HIGH);
    delay(1000); // Beep for 1 second
    digitalWrite(buzzerPin, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32ClientB")) {
      Serial.println("connected");
      client.subscribe("output/dustsensors");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}