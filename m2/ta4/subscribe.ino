// ESP32 Side B

#include <WiFi.h>
#include <PubSubClient.h>

const int lightPin = 2;
const int buzzerPin = 4;

const char* ssid = "Pinguinos_de_Madagascar";
const char* password = "Paligienco.2023";
const char* mqtt_server = "broker.emqx.io";
const int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(lightPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(lightPin, LOW);
  digitalWrite(buzzerPin, LOW);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, port);
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
    delay(1000); 
    digitalWrite(buzzerPin, LOW);
  } else if (message == "Door opened" || message == "Door closed") {
    for(int i = 0; i < 5; i++) {
      digitalWrite(buzzerPin, HIGH);
      delay(150);
      digitalWrite(buzzerPin, LOW);
      delay(150);
    }
    
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