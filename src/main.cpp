#include <WiFi.h>

// Credenciales WiFi
const char* ssid = "CATERPILAR";
const char* password = "Carranza";

// Definición de pines
#define stepPin1 33
#define dirPin1 32
#define stepPin2 4
#define dirPin2 16
#define stepPin3 15
#define dirPin3 2

// Variables de control
unsigned long previousMillis = 0;
const long movimientoDuracion = 3000;  // Duración de movimiento por motor
const long pasoIntervalo = 4;  // Intervalo de 4 ms entre pasos
int motorActual = 0;  // Motor actual a girar
bool motoresActivos = false;

WiFiServer servidor(80);

// Funciones
void conectarWiFi() {
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }
  Serial.println("Conectado a WiFi!");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());  // Mostrar la IP asignada
}

void moverMotor(int motor) {
  int stepPin, dirPin;

  // Seleccionar pines según el motor
  switch (motor) {
    case 1:
      stepPin = stepPin1;
      dirPin = dirPin1;
      break;
    case 2:
      stepPin = stepPin2;
      dirPin = dirPin2;
      break;
    case 3:
      stepPin = stepPin3;
      dirPin = dirPin3;
      break;
    default:
      return;  // No hacer nada si el motor no es válido
  }

  // Configurar dirección y girar el motor
  digitalWrite(dirPin, HIGH);  // Fijar dirección del motor
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(100);  // Ajustar tiempo para simular un paso
  digitalWrite(stepPin, LOW);
  delayMicroseconds(100);  // Ajustar tiempo para simular un paso
}

void procesarCliente(WiFiClient cliente) {
  String peticion = "";
  while (cliente.connected()) {
    if (cliente.available()) {
      char c = cliente.read();
      peticion += c;

      // Verificar si se recibió la petición completa
      if (c == '\n') {
        Serial.println("Petición recibida: ");
        Serial.println(peticion);

        // Verificar si el texto es "ON" o "OFF"
        if (peticion.indexOf("GET /?texto=ON") != -1) {
          motoresActivos = true;
          motorActual = 1;  // Iniciar con el motor 1
          previousMillis = millis();  // Reiniciar temporizador
        } else if (peticion.indexOf("GET /?texto=OFF") != -1) {
          motoresActivos = false;  // Apagar motores
          // Desactivar todos los pasos
          digitalWrite(stepPin1, LOW);
          digitalWrite(stepPin2, LOW);
          digitalWrite(stepPin3, LOW);
        }

        // Enviar respuesta HTTP
        cliente.println("HTTP/1.1 200 OK");
        cliente.println("Content-type:text/html");
        cliente.println();
        cliente.println("<!DOCTYPE HTML>");
        cliente.println("<html><h1>Control de Motores</h1></html>");
        break;
      }
    }
  }
  cliente.stop();  // Desconectar al cliente
}

void setup() {
  Serial.begin(115200);

  // Configurar pines
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  
  // Conectar a WiFi
  conectarWiFi();
  
  // Iniciar el servidor web
  servidor.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // Control de motores secuencialmente
  if (motoresActivos) {
    if (currentMillis - previousMillis >= movimientoDuracion) {
      previousMillis = currentMillis;

      motorActual++;
      if (motorActual > 3) {
        motorActual = 1;  // Reiniciar el ciclo
      }
    }
    moverMotor(motorActual);
    delay(pasoIntervalo);  // Esperar 4 ms entre pasos
  } else {
    // Desactivar todos los pasos si motoresActivos es false
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    digitalWrite(stepPin3, LOW);
  }

  // Verificar solicitudes HTTP
  WiFiClient cliente = servidor.available();
  if (cliente) {
    procesarCliente(cliente);
  }
}
