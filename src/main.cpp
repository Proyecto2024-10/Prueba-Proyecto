#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>

// Credenciales de la red WiFi
const char* ssid = "WIFI1";
const char* password = "del.sel1";

// Inicializa el servidor web en el puerto 80
WiFiServer server(80);

// Pin donde está conectado el LED
const int ledPin = 5; // Cambia el pin según tu configuración

void setup() {
  Serial.begin(115200);  // Inicia la comunicación serial

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

  Serial.print("Conectado a WiFi. Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Inicia el servidor
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  // Comprueba si hay clientes conectados
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Nuevo cliente conectado");

    String currentLine = "";
    bool isBlankLine = false;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        request += c;

        // Detecta el final de la solicitud HTTP
        if (c == '\n') {
          if (request.length() == 1) {
            // Procesa la solicitud y extrae el texto
            String text = getValueFromRequest(request, "text");

            if (text.equals("Hola")) {
              digitalWrite(ledPin, HIGH); // Enciende el LED
              Serial.println("Texto correcto: LED encendido");

              // Responde al cliente
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
              client.println("<!DOCTYPE HTML>");
              client.println("<html><h1>LED Encendido</h1></html>");
              client.println();
            } else {
              digitalWrite(ledPin, LOW); // Apaga el LED
              Serial.println("Error: Texto incorrecto");

              // Responde al cliente
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
              client.println("<!DOCTYPE HTML>");
              client.println("<html><h1>Error: Texto incorrecto</h1></html>");
              client.println();
            }
            break;
          }
          request = ""; // Reinicia la solicitud
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado");
  }
}

// Función para extraer el valor de un parámetro de la solicitud HTTP
String getValueFromRequest(String request, String parameter) {
  int start = request.indexOf(parameter + "=");
  if (start == -1) {
    return "";
  }
  start += parameter.length() + 1;
  int end = request.indexOf('&', start);
  if (end == -1) {
    end = request.indexOf(' ', start);
  }
  return request.substring(start, end);
}
