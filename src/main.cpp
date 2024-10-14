#include <Arduino.h>

// Pines de los motores paso a paso
#define stepPin1 33
#define dirPin1 32
#define stepPin2 4
#define dirPin2 16 
#define stepPin3 15
#define dirPin3 2

// Pines del servo
#define pinServo 0

// Variables para controlar los motores paso a paso
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
const long interval = 1;  // 1 milisegundo entre pasos (ajustar según velocidad)
int stepPin1State = LOW;
int stepPin2State = LOW;
int stepPin3State = LOW;

// Variables para el control del servo
unsigned long tiempoAnterior = 0;
unsigned long duracionPulso = 0;
unsigned long inicioPulso = 0;
bool enviandoPulso = false;

int angulos[] = {55, 45, 35, 45};  // Patrones de ángulos del servo
int indiceAngulo = 0;
unsigned long intervaloCambio = 500; // Cambia de ángulo cada 500 ms

void setup() {
  // Configurar pines de los motores paso a paso
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  digitalWrite(dirPin1, HIGH);  // Fijar dirección del Motor 1

  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  digitalWrite(dirPin2, HIGH);  // Fijar dirección del Motor 2

  pinMode(stepPin3, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  digitalWrite(dirPin3, HIGH);  // Fijar dirección del Motor 3

  // Configurar pin del servo
  pinMode(pinServo, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // Control del Motor 1
  if (currentMillis - previousMillis1 >= interval) {
    previousMillis1 = currentMillis;
    stepPin1State = !stepPin1State;
    digitalWrite(stepPin1, stepPin1State);
  }

  // Control del Motor 2
  if (currentMillis - previousMillis2 >= interval) {
    previousMillis2 = currentMillis;
    stepPin2State = !stepPin2State;
    digitalWrite(stepPin2, stepPin2State);
  }

  // Control del Motor 3
  if (currentMillis - previousMillis3 >= interval) {
    previousMillis3 = currentMillis;
    stepPin3State = !stepPin3State;
    digitalWrite(stepPin3, stepPin3State);
  }

  // Control del Servo
  unsigned long tiempoActual = millis();  

  // Si no se está enviando un pulso, iniciamos un nuevo pulso
  if (!enviandoPulso) {
    duracionPulso = map(angulos[indiceAngulo], 0, 180, 600, 2400); 
    inicioPulso = micros();  
    digitalWrite(pinServo, HIGH);
    enviandoPulso = true;
  }

  // Si se ha alcanzado la duración del pulso, apagamos el pin
  if (enviandoPulso && (micros() - inicioPulso >= duracionPulso)) {
    digitalWrite(pinServo, LOW);
    enviandoPulso = false;
  }

  // Cambiar ángulo cada 500 milisegundos
  if (tiempoActual - tiempoAnterior >= intervaloCambio) {
    tiempoAnterior = tiempoActual;
    indiceAngulo++;
    if (indiceAngulo >= 4) {
      indiceAngulo = 0; // Reiniciar el índice después del último ángulo
    }
  }
}
