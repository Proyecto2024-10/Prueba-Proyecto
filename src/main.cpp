#include <Arduino.h>
#include <SPI.h>
const int DIR = 23;
const int STEP = 22;
const int  steps_per_rev = 200;

void setup()
{
  Serial.begin(115200);

  // Configura el pin del LED como salida
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Asegúrate de que el LED está apagado inicialmente

  // Conecta el ESP32 a la red WiFi
  WiFi.begin(ssid, password);

  // Espera hasta que el ESP32 esté conectado a la red WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  // Imprime la dirección IP del ESP32
  Serial.print("Conectado a WiFi. Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Inicia el servidor
  server.begin();
  Serial.println("Servidor iniciado");
}
void loop()
{
  digitalWrite(DIR, HIGH);
  
  for(int i = 0; i<steps_per_rev; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(2000);
    digitalWrite(STEP, LOW);
    delayMicroseconds(2000);
  }
  delay(1000); 
  
  digitalWrite(DIR, LOW);
  Serial.println("")
  for(int i = 0; i<steps_per_rev; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEP, LOW);
    delayMicroseconds(1000);
  }
  delay(1000);
}