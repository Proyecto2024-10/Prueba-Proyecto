

// Incluímos la librería para poder controlar el servo
#include <ESP32Servo.h>

 
Servo servo; 
 
int pos = 0;   
 
void setup() {
   // put your setup code here, to run once:
   servo.attach(23); 
}
 
void loop() {
   // put your main code here, to run repeatedly:
   for (pos = 0; pos <= 180; pos += 1){ 
      servo.write(pos);              
      delay(10);                       
   }

   for (pos = 180; pos >= 0; pos -= 1){ 
      servo.write(pos);              
      delay(10);                       
   }
}