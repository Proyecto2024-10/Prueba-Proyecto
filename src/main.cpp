#include <Arduino.h>
 
  #define STEP 33
  #define DIR 32

  void setup()
  {
    pinMode(STEP, OUTPUT);
    pinMode(DIR, OUTPUT);
  }

  void loop() {
    digitalWrite (DIR, HIGH);

      digitalWrite(STEP, HIGH);
      delayMicroseconds(500);
      digitalWrite(STEP, LOW);
      delayMicroseconds(500);

  }
