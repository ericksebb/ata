#include <UbidotsEsp32Mqtt.h>
#include <DHT.h>

const char *UBIDOTS_TOKEN = "BBUS-H6fy5BpJq3cotefvnxctZAaTZXABuA";  // Put here your Ubidots TOKEN
const char *ssid = "Suda";      // Put here your Wi-Fi SSID
const char *password = "";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "nodemcu32s";   // Put here your Device label to which data  will be published

//  Pin definitions
const int dhtPin = 21;
const uint8_t mq135 = 39;
const int blink = 2;

//  Variable labels

const char *dhtVarT = "Temperature";
const char *dhtVarH = "Humidity";
const char *mq135Var = "Air Quality";

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

unsigned long timer;
const int analogPin = 34; 
float mq135Value = 2;
int ledStatus = 0;

Ubidots ubidots(UBIDOTS_TOKEN);
DHT dht(dhtPin, DHT11);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

float pollution () {
  float mq135Value = analogRead(mq135); 
  Serial.println(mq135Value);
  
  if ((dht.readTemperature() > 14 || dht.readHumidity() > 85) || (mq135Value > 4095 / 4)) {
    digitalWrite(blink, HIGH);
  } else {
    digitalWrite(blink, LOW);
  }
  return mq135Value / 4095.0 * 100;
}

void setup() {
  Serial.begin(115200);
  pinMode(blink, OUTPUT);
  digitalWrite(blink, LOW);
  analogSetWidth(12);
  dht.begin();
  ubidots.setDebug(true);
  ubidots.connectToWifi(ssid, password);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}


void loop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
  }
  if (labs(millis() - timer) > PUBLISH_FREQUENCY) {
    ubidots.add(mq135Var, pollution());
    ubidots.add(dhtVarT, dht.readTemperature());
    ubidots.add(dhtVarH, dht.readHumidity());
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
}