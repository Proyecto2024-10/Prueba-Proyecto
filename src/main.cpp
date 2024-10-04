#include <Arduino.h>

#define pinServo 0

unsigned long tiempoAnterior = 0;
unsigned long duracionPulso = 0;
unsigned long inicioPulso = 0;
bool enviandoPulso = false;

int angulos[] = {55, 45, 35, 45};  // Patrones de ángulos
int indiceAngulo = 0;
unsigned long intervaloCambio = 500; // Cambia de ángulo cada 500 ms

void setup() {
  pinMode(pinServo, OUTPUT);
}

void loop() {
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
