#include <WiFi.h>
#include <BluetoothSerial.h>

// Credenciales WiFi
const char* ssid = "CATERPILAR";
const char* password = "Carranza";

// Configuración de Bluetooth
BluetoothSerial SerialBT;

// Variable para almacenar el texto recibido
String textoRecibido = "";

// Declaración de la función para enviar texto por Bluetooth
void enviarTextoBluetooth(String texto);

void enviarRespuestaHTTP(WiFiClient cliente, String contenido) {
    cliente.println("HTTP/1.1 200 OK");
    cliente.println("Content-type:text/html");
    cliente.println();
    cliente.println("<!DOCTYPE HTML>");
    cliente.println("<html>" + contenido + "</html>");
    cliente.println();
}

void conectarWiFi() {
    Serial.println("Conectando a WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando...");
    }

    Serial.println("Conectado a WiFi!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Iniciar el servidor web
    WiFiServer servidor(80);
    servidor.begin();
    
    // Esperar a que se conecte un cliente
    WiFiClient cliente = servidor.available();
    if (cliente) {
        Serial.println("Nuevo cliente conectado");
        String peticion = "";
        while (cliente.connected()) {
            if (cliente.available()) {
                char c = cliente.read();
                peticion += c;

                if (c == '\n') {
                    Serial.println("Petición recibida: ");
                    Serial.println(peticion);

                    int indiceTexto = peticion.indexOf("/?texto=");
                    if (indiceTexto != -1) {
                        String texto = peticion.substring(indiceTexto + 8, peticion.indexOf(" ", indiceTexto));
                        texto.trim();  
                        textoRecibido = texto;  // Guardar el texto recibido

                        // Enviar respuesta al cliente
                        enviarRespuestaHTTP(cliente, "<h1>Texto recibido correctamente</h1>");

                        // Enviar texto por Bluetooth
                        enviarTextoBluetooth(textoRecibido);
                    } else {
                        enviarRespuestaHTTP(cliente, "<h1>Error: Texto no enviado correctamente</h1>");
                    }
                    break;
                }
            }
        }
        cliente.stop();  
        Serial.println("Cliente desconectado");
    }

    // Desconectar Wi-Fi
    WiFi.disconnect();
    Serial.println("Desconectado de WiFi");
}

void enviarTextoBluetooth(String texto) {
    SerialBT.begin("ESP32_Bluetooth");
    Serial.println("Esperando conexión Bluetooth...");

    // Esperar a que un cliente se conecte
    while (!SerialBT.hasClient()) {
        delay(100);  // Esperar conexión
    }

    // Enviar texto por Bluetooth
    SerialBT.print("Texto recibido: ");
    SerialBT.println(texto);
    Serial.println("Texto enviado por Bluetooth.");

    // Desconectar Bluetooth después de enviar
    SerialBT.end();
}

void setup() {
    Serial.begin(115200);  // Iniciar monitor serie
    conectarWiFi();  // Conectar a Wi-Fi
}

void loop() {
    delay(100);  // Reduce la carga en el bucle
}
