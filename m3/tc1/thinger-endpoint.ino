#include <WiFi.h>       //WiFi connection library
#include <ThingerESP32.h>    //THINGER.IO library
#include "DHT.h" 

#define user "ericksebb" 
#define device_Id "nodemcu32s"
#define device_credentials "nodemcu32s"
#define ssid "Suda"
#define password ""

#define DHTPIN 21
#define DHTTYPE DHT11
#define led 2

DHT dht(DHTPIN, DHTTYPE);
ThingerESP32 thing(user, device_Id, device_credentials);

unsigned long previousMillis = 0; // Stores the last time the endpoint was called
const long interval = 60000; // Interval at which to call the endpoint (1 minute)

void setup() {
    pinMode(led,OUTPUT);
    Serial.begin(115200);
    thing.add_wifi(ssid, password);
    thing["LED"] << digitalPin(led);//variable que es leida desde la plataforma
    thing["dht11"] >> [](pson &out){//varible compuesta para enviar datos a la plataforma
        out["humidity"] = dht.readHumidity();
        Serial.println(dht.readHumidity());
        out["celsius"] = dht.readTemperature();
        Serial.println(dht.readTemperature());
    };
}

void loop() {
    thing.handle();
    
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        
        pson data;
        data["temperature"] = dht.readTemperature();
        data["humidity"] = dht.readHumidity();
        Serial.println("Mail sent");
        thing.call_endpoint("mail_dht", data);
    }
}