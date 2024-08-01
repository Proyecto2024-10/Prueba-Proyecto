#include <WiFi.h>


const char* ssid = "WIFI1";     //Nombre de tu wifi 
const char* password = "del.sel1"; //Contraseña de tu wifi

WiFiServer server(80);  

const int ledPin = 2;  

void sendHttpResponse(WiFiClient client, String content) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>" + content + "</html>");
  client.println();
}

void setup() {
  Serial.begin(115200); 

 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  

 
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

    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        if (c == '\n') {
          Serial.println("Petición completa recibida:");
          Serial.println(request);

 
          int commandIndex = request.indexOf("/?command=");
          if (commandIndex != -1) {
            String command = request.substring(commandIndex + 10, request.indexOf(" ", commandIndex + 10));
            command.trim(); 

            
            if (command.equalsIgnoreCase("on")) {
              digitalWrite(ledPin, HIGH); 
              Serial.println("LED Encendido");
              sendHttpResponse(client, "<h1>LED Encendido</h1>");
            } else if (command.equalsIgnoreCase("off")) {
              digitalWrite(ledPin, LOW);
              Serial.println("LED Apagado");
              sendHttpResponse(client, "<h1>LED Apagado</h1>");
            } else {
              Serial.println("Comando desconocido");
              sendHttpResponse(client, "<h1>Error: Comando desconocido</h1>");
            }
          } else {
            Serial.println("Comando no encontrado en la solicitud");
            sendHttpResponse(client, "<h1>Error: Comando no encontrado</h1>");
          }
          break;  
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
