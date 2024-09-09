#include <AccelStepper.h>

//Pines puestos de ejemplo (no son los que se van a utilizar)
const int dirPin1 = 15;  // Motor 1 DIR
const int stepPin1 = 76; // Motor 1 STEP
const int dirPin2 = 34;  // Motor 2 DIR
const int stepPin2 = 23; // Motor 2 STEP
const int dirPin3 = 17;  // Motor 3 DIR
const int stepPin3 = 18; // Motor 3 STEP

AccelStepper motor1(1, stepPin1, dirPin1);
AccelStepper motor2(1, stepPin2, dirPin2);
AccelStepper motor3(1, stepPin3, dirPin3);

void setup() {

    motor1.setMaxSpeed(700);      
    motor1.setAcceleration(300);  
    motor1.moveTo(600);   
    motor2.setMaxSpeed(700);     
    motor2.setAcceleration(300); 
    motor2.moveTo(600);         

    motor3.setMaxSpeed(700);     
    motor3.setAcceleration(300);
    motor3.moveTo(600);           
}

void loop() {
   
    motor1.run();
    motor2.run();
    motor3.run();
}
