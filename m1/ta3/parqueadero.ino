#include <ESP32Servo.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// Configuración del servo
Servo servoMotor;

// Pines del sensor ultrasonido
const int Trigger = 35;
const int Echo = 32;

// Pines del sistema
const int led = 18;

// Credenciales Wi-Fi
const char* ssid = "LIKENET-Bryan Lojano";               // Reemplaza con tu SSID
const char* password = "mateo2414";                      // Reemplaza con tu contraseña

// Configuración de ThingSpeak
unsigned long channelID = 2759134;                       // Reemplaza con tu Channel ID
const char* WriteApiKey = "FQL3145OWHWV4CM9";            // Reemplaza con tu API Key
WiFiClient client;

// Variables globales
int vehiculosDentro = 0;                                 // Total de vehículos en el parqueadero
const int capacidadMaxima = 10;                          // Capacidad máxima del parqueadero

void setup() {
  Serial.begin(115200);

  // Configurar pines
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(led, OUTPUT);

  // Iniciar el servo
  servoMotor.attach(5);
  servoMotor.write(0);

  // Conectar a Wi-Fi
  conectarWiFi();

  // Inicializar ThingSpeak
  ThingSpeak.begin(client);

  // Inicializar LED
  digitalWrite(led, LOW);
}

void loop() {
  int distancia = medirDistancia();

  // Detectar entrada o salida de vehículos
  if (distancia <= 10) { // Si detecta un vehículo cerca
    if (vehiculosDentro < capacidadMaxima) { // Ingreso permitido
      abrirPuerta();
      delay(3000); // Tiempo para que el vehículo pase
      cerrarPuerta();
      vehiculosDentro++;
      Serial.print("Vehículos dentro: ");
      Serial.println(vehiculosDentro);
    } else {
      Serial.println("Parqueadero lleno");
    }
  } else if (distancia > 50 && vehiculosDentro > 0) { // Vehículo saliendo
    vehiculosDentro--;
    Serial.print("Vehículos dentro: ");
    Serial.println(vehiculosDentro);
  }

  // Enviar datos a ThingSpeak
  enviarDatosThingSpeak();

  // Actualizar LED según disponibilidad
  actualizarLED();

  // Esperar antes de la próxima lectura (15 segundos)
  delay(15000); 
}

// Función para medir distancia con el sensor de ultrasonidos
int medirDistancia() {
  digitalWrite(Trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);

  long duracion = pulseIn(Echo, HIGH);
  int distancia = duracion / 58; // Conversión a cm
  return distancia;
}

// Función para abrir la puerta
void abrirPuerta() {
  for (int i = 0; i <= 90; i++) {
    servoMotor.write(i);
    delay(15);
  }
}

// Función para cerrar la puerta
void cerrarPuerta() {
  for (int i = 90; i >= 0; i--) {
    servoMotor.write(i);
    delay(15);
  }
}

// Función para actualizar ThingSpeak
void enviarDatosThingSpeak() {
  ThingSpeak.setField(1, vehiculosDentro); // Enviar al campo Field 1
  int respuesta = ThingSpeak.writeFields(channelID, WriteApiKey);
  
  if (respuesta == 200) {
    Serial.println("Datos enviados a ThingSpeak correctamente!");
  } else {
    Serial.print("Error enviando datos. Código: ");
    Serial.println(respuesta);
  }
}

// Función para actualizar el LED
void actualizarLED() {
  if (vehiculosDentro < capacidadMaxima) {
    digitalWrite(led, HIGH); // Hay disponibilidad
  } else {
    digitalWrite(led, LOW); // No hay disponibilidad
  }
}

// Función para conectar a Wi-Fi
void conectarWiFi() {
  Serial.print("Conectando a Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado al WiFi");
}
