#include <WiFi.h>

// Configuración de las credenciales WiFi
const char* ssid = "WIFI1";      // Cambia esto a tu SSID
const char* password = "del.sel1"; // Cambia esto a tu contraseña

WiFiServer server(80);  // Inicializa el servidor en el puerto 80

const int ledPin = 2;  // Pin donde está conectado el LED (cambiar si es necesario)

// Función para enviar la respuesta HTTP
void sendHttpResponse(WiFiClient client, String content) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>" + content + "</html>");
  client.println();
}

void setup() {
  Serial.begin(115200);  // Inicia la comunicación serial

  // Configura el pin del LED como salida
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Asegura que el LED esté apagado inicialmente

  // Conecta a la red WiFi
  WiFi.begin(ssid, password);

  // Espera a que se conecte a la red WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.print("Conectado a WiFi. Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.begin();  // Inicia el servidor
  Serial.println("Servidor iniciado");
}

void loop() {
  WiFiClient client = server.available();  // Espera un cliente

  if (client) {
    Serial.println("Nuevo cliente conectado");

    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        // Detectar el final de la solicitud HTTP
        if (c == '\n') {
          Serial.println("Petición completa recibida:");
          Serial.println(request);

          // Extraer el comando de la solicitud HTTP
          int commandIndex = request.indexOf("/?command=");
          if (commandIndex != -1) {
            String command = request.substring(commandIndex + 10, request.indexOf(" ", commandIndex + 10));
            command.trim();  // Remueve espacios en blanco al inicio y fin

            // Analiza el comando recibido
            if (command.equalsIgnoreCase("on")) {
              digitalWrite(ledPin, HIGH); // Enciende el LED
              Serial.println("LED Encendido");
              sendHttpResponse(client, "<h1>LED Encendido</h1>");
            } else if (command.equalsIgnoreCase("off")) {
              digitalWrite(ledPin, LOW); // Apaga el LED
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
          break;  // Salir del bucle de lectura
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
