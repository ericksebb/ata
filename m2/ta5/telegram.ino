#include <ESP32Servo.h>
#include <WiFi.h>
#include <ThingerESP32.h>

Servo servoMotor;

const int trigger = 32;
const int echo = 35; 
const int infrarred = 23;
const int follower = 22;
const int led = 18;
const int buzzer = 19;
const int servo = 5;
const int mq02 = 34;

const int maxVehicles = 5;
int currentVehicles = 0;
bool doorOpened = false;

const char* ssid = "LIKENET-Bryan Lojano";
const char* password = "mateo2414";

const char* user = "ericksebb";
const char* device_id = "nodemcu32s";
const char* device_credentials = "nodemcu32s";

bool ledStatus;

WiFiClient client;
ThingerESP32 thing(user, device_id, device_credentials);

void setup()
{
  Serial.begin(115200);

  thing.add_wifi(ssid, password);

  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(infrarred, INPUT);
  pinMode(follower, INPUT);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  servoMotor.attach(servo);
  closeDoor();

  thing["vehiclesInside"] >> [](pson &out)
  {
    out = manageDoor();
  };
  thing["pollution"] >> [](pson &out)
  {
    out = measurePollution();
  };
  thing["ventilation"] >> [](pson &out)
  {
    out = ledStatus;
  };
}

int manageDoor() {

  int followerStatus = digitalRead(follower);
  if (followerStatus == HIGH)
  {
    openDoor();
    delay(4000);
    closeDoor();
    if (currentVehicles < maxVehicles)
    {
      currentVehicles++;
    }
    previousDistance = currentDistance;
    return currentVehicles;
  }

  int infraredStatus = digitalRead(infrarred);
  if (infraredStatus == HIGH)
  {
    openDoor();
    delay(4000);
    closeDoor();
    if (currentVehicles > 0)
    {
      currentVehicles--;
    }
  }
  return currentVehicles;
}



float measurePollution() {
  int mq02Value = analogRead(mq02);
  float mq02Percentage = (mq02Value / 4095.0) * 100;

  if (mq02Percentage > 30) {
    ledOn(1);
    for (int i = 0; i < 3; i++) {
      for (int i = 0; i < 4; i++){
        digitalWrite(buzzer, HIGH);
        delay(90);
        digitalWrite(buzzer, LOW);
        delay(90);
      }
      delay(1000);
    }
  }
    else {ledOn(0);
  }
  delay(1000);
  return mq02Percentage;
}

bool ledOn(int status) {
  if (status == 1)
  {
    digitalWrite(led, HIGH);
    ledStatus = true;
    return true;
  }
  else if (status == 0)
  {
    digitalWrite(led, LOW);
    ledStatus = false;
    return false;
  }
}

void openDoor()
{
  Serial.println("Abriendo puerta...");
  for (int degrees = 0; degrees <= 90; degrees++)
  {
    servoMotor.write(degrees);
    delay(15);
  }
  doorOpened = true;
  Serial.println("Puerta abierta");
}

void closeDoor()
{
  Serial.println("Cerrando puerta...");
  for (int degrees = 90; degrees >= 0; degrees--)
  {
    servoMotor.write(degrees);
    delay(15);
  };
    doorOpened = false;
    Serial.println("Puerta cerrada");
}

void loop() {
  thing.handle();
}