#include <Arduino.h>

//Motor 
#define stepPin1 18
#define dirPin1 5
#define enable1 19

//Motor 
#define stepPin2 33 
#define dirPin2 32 
#define enable2 25

//Motor Tijera
#define stepPin3 27
#define dirPin3 26
#define enable3 14

unsigned long tiempo = 0;
bool direccion=0;
int frecuencia = 500; //Hz 
int frecuencia_1 = 700; 

int stepPin1State = LOW;
int stepPin2State = LOW;
int stepPin3State = LOW;


void setup() {
  // Configurar pines para Motor 1
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(enable1,OUTPUT);
  digitalWrite(dirPin1, HIGH);  // Fijar dirección del Motor 1
  // Configurar pines para Motor 2
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(enable2,OUTPUT);
  digitalWrite(dirPin2, HIGH);  // Fijar dirección del Motor 2

  // Configurar pines para Motor 3
  pinMode(stepPin3, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  pinMode(enable3,OUTPUT);
  digitalWrite(dirPin3, HIGH);  // Fijar dirección del Motor 3
  ledcSetup(0,frecuencia_1,8);
  ledcSetup(1,frecuencia_1,8);
  ledcSetup(2,frecuencia,8);
  ledcAttachPin(stepPin1,0);
  ledcAttachPin(stepPin2,1);
  ledcAttachPin(stepPin3,2);
  tiempo=millis();
}

void loop() {
    digitalWrite(enable1,LOW);
    digitalWrite(enable2,LOW);
    digitalWrite(enable3,LOW);
    ledcWrite(0,127); //50% Duty Cycle
    ledcWrite(1,127);
    digitalWrite(dirPin1,HIGH);
    digitalWrite(dirPin2,HIGH);
    if(tiempo+250>=millis()){
        digitalWrite(dirPin3,direccion);
        ledcWrite(2,127);
    }
    else{
        delay(50);
        direccion=!direccion;
        tiempo=millis();
    }
}