#include <WiFi.h>

// Credenciales WiFi
const char* ssid = "CATERPILAR";
const char* password = "Carranza";

// Definir el pin del LED
const int ledPin = 2;

WiFiServer servidor(80);


void enviarRespuestaHTTP(WiFiClient cliente, String contenido) {
  cliente.println("HTTP/1.1 200 OK");
  cliente.println("Content-type:text/html");
  cliente.println();
  cliente.println("<!DOCTYPE HTML>");
  cliente.println("<html>" + contenido + "</html>");
  cliente.println();
}
void setup() {
  Serial.begin(115200);

  // Configurar el pin del LED como salida
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Conectar a la red WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }
  
  // Mostrar la IP
  Serial.println("Conectado a WiFi!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(2, HIGH);
  // Iniciar el servidor web
  servidor.begin();
}

void loop() {
  // Esperar a que un cliente se conecte
  WiFiClient cliente = servidor.available();
  if (cliente) {
    Serial.println("Nuevo cliente conectado");
    String peticion = "";
    while (cliente.connected()) {
      if (cliente.available()) {
        char c = cliente.read();
        peticion += c;

        // Verificar si se recibió la petición completa
        if (c == '\n') {
          Serial.println("Petición recibida: ");
          Serial.println(peticion);

          // Buscar si el texto "/?texto=" está en la petición
          int indiceTexto = peticion.indexOf("/?texto=");
          if (indiceTexto != -1) {
            // Extraer el texto de la petición
            String texto = peticion.substring(indiceTexto + 8, peticion.indexOf(" ", indiceTexto));
            texto.trim();  // Limpiar espacios en blanco

            // Si el texto es "ON", encender el LED
            if (texto.equalsIgnoreCase("ON")) {
              digitalWrite(ledPin, HIGH);
              enviarRespuestaHTTP(cliente, "<h1>LED ENCENDIDO</h1>");
            } else if (texto.equalsIgnoreCase("OFF")) {
              digitalWrite(ledPin, LOW);
              enviarRespuestaHTTP(cliente, "<h1>LED APAGADO</h1>");
            } else {
              enviarRespuestaHTTP(cliente, "<h1>Error: Texto no reconocido</h1>");
            }
          } else {
            enviarRespuestaHTTP(cliente, "<h1>Error: Texto no enviado correctamente</h1>");
          }

          break;
        }
      }
    }
    cliente.stop();  // Desconectar al cliente
    Serial.println("Cliente desconectado");
  }
}
