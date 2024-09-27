#include <Arduino.h>

#define stepPin1 33
#define dirPin1 32
#define stepPin2 4
#define dirPin2 16 
#define stepPin3 15
#define dirPin3 2

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
const long interval = 1;  // 1 milisegundo entre pasos (ajustar según velocidad)

int stepPin1State = LOW;
int stepPin2State = LOW;
int stepPin3State = LOW;

void setup() {
  // Configurar pines para Motor 1
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  digitalWrite(dirPin1, HIGH);  // Fijar dirección del Motor 1

  // Configurar pines para Motor 2
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  digitalWrite(dirPin2, HIGH);  // Fijar dirección del Motor 2

  // Configurar pines para Motor 3
  pinMode(stepPin3, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  digitalWrite(dirPin3, HIGH);  // Fijar dirección del Motor 3
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
}
