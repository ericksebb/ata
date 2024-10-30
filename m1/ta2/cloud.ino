#include <DHTesp.h>
#include "thingProperties.h"

#define ldr 36
#define dhtPin 22

#define motor 19
#define buzzer 23

DHTesp dht;

/* 
CloudTemperatureSensor temp;      
CloudRelativeHumidity humidity;
bool led;
*/

void setup() {
  Serial.begin(115200);
  delay(1500);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  dht.setup(dhtPin, DHTesp::DHT11);  
  
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(dhtPin, INPUT);
  pinMode(motor, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  ArduinoCloud.update();

  temp = dht.getTemperature();
  humidity = dht.getHumidity();

  int light = analogRead(ldr);
  int newLight = map(light, 0, 4095, 0, 100);

  if(temp >= 30){
    digitalWrite(motor, HIGH);
  } else digitalWrite(motor, LOW);

  if(newLight >= 75){
    led = true;
    digitalWrite(buzzer, HIGH);
  } else {
    led = false;
    digitalWrite(buzzer, LOW);
  }
  Serial.println(light);
  Serial.println(newLight);
  delay(1000);
}
