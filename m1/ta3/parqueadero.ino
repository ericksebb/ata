#include <ESP32Servo.h>
#include <WiFi.h>
#include <ThingSpeak.h>


Servo servoMotor;
const int TRIGGER_PIN = 32;    
const int ECHO_PIN = 35;       
const int IR_SENSOR_PIN = 21;  
const int LED_PIN = 18;
const int SERVO_PIN = 5;


const int CAPACIDAD_MAXIMA = 5;
int vehiculosDentro = 0;
bool puertaAbierta = false;


const char* ssid = "LIKENET-Bryan Lojano";
const char* password = "mateo2414";
unsigned long channelID = 2759134;
const char* writeApiKey = "FQL3145OWHWV4CM9";
WiFiClient client;

void setup() {
  Serial.begin(115200);
  
 
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  servoMotor.attach(SERVO_PIN);
  cerrarPuerta(); 
  
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado al WiFi");
  
  ThingSpeak.begin(client);
}

void loop() {
  int distancia = medirDistancia();
  bool salidaDetectada = digitalRead(IR_SENSOR_PIN) == LOW; 
  
  Serial.print("Distancia: ");
  Serial.println(distancia);
  Serial.print("Salida detectada (IR): ");
  Serial.println(salidaDetectada);

  // sensor ultrasónico
  if (distancia <= 5) { 
    if (vehiculosDentro < CAPACIDAD_MAXIMA && !puertaAbierta) {
      abrirPuerta();
      delay(3000); 
      cerrarPuerta();
      vehiculosDentro++;
      Serial.println("Vehículo entrando. Total: " + String(vehiculosDentro));
    } else if (vehiculosDentro >= CAPACIDAD_MAXIMA) {
      Serial.println("Parqueadero lleno - No se puede abrir la puerta");
    }
  }

  // sensor infrarrojo
  if (salidaDetectada) { 
    if (vehiculosDentro > 0 && !puertaAbierta) {
      abrirPuerta();
      delay(3000); 
      cerrarPuerta();
      vehiculosDentro--;
      Serial.println("Vehículo saliendo. Total: " + String(vehiculosDentro));
    } else if (vehiculosDentro == 0) {
      Serial.println("No hay vehículos en el parqueadero para salir");
    }
  }

  
  digitalWrite(LED_PIN, vehiculosDentro < CAPACIDAD_MAXIMA);
  enviarDatosThingSpeak();
  
  delay(1000); 
}

int medirDistancia() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  return pulseIn(ECHO_PIN, HIGH) / 58;
}

void abrirPuerta() {
  Serial.println("Abriendo puerta...");
  for (int angulo = 0; angulo <= 90; angulo++) {
    servoMotor.write(angulo);
    delay(15); 
  }
  puertaAbierta = true;
  Serial.println("Puerta abierta");
}

void cerrarPuerta() {
  Serial.println("Cerrando puerta...");
  for (int angulo = 90; angulo >= 0; angulo--) {
    servoMotor.write(angulo);
    delay(15); 
  puertaAbierta = false;
  Serial.println("Puerta cerrada");
}

void enviarDatosThingSpeak() {
  ThingSpeak.setField(1, vehiculosDentro);
  int respuesta = ThingSpeak.writeFields(channelID, writeApiKey);
  
  if (respuesta == 200) {
    Serial.println("Datos enviados correctamente a ThingSpeak");
  } else {
    Serial.println("Error al enviar datos: " + String(respuesta));
  }
}
