#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>

const char* ssid = "WIFI1";
const char* password = "del.sel1";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.print("Conectado a WiFi. Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Nuevo cliente conectado");

    String currentLine = "";
    bool isBlankLine = false;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (isBlankLine) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<h1>Hola desde ESP32</h1>");
            client.println("</html>");
            client.println();

            break;
          } else {
            isBlankLine = true;
          }
        } else if (c != '\r') {
          isBlankLine = false;
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado");
  }
}