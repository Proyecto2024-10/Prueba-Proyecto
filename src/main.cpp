#include <WiFi.h>
#include <BluetoothSerial.h>

const char* ssid = "CATERPILAR";
const char* password = "Carranza";

BluetoothSerial SerialBT;
WiFiServer servidor(80);  // Servidor en el puerto 80
String textoRecibido = "";
bool textoEnviadoPorBT = false;

void conectarWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi!");
    servidor.begin();
}

void recibirTexto() {
    WiFiClient cliente = servidor.available();
    
    if (cliente) {
        Serial.println("Cliente conectado");
        String peticion = cliente.readStringUntil('\r');  // Lee la petición completa
        cliente.flush();

        int indiceTexto = peticion.indexOf("/?texto=");
        if (indiceTexto != -1) {
            textoRecibido = peticion.substring(indiceTexto + 8, peticion.indexOf(" ", indiceTexto));
            textoRecibido.trim();  // Quitar espacios en blanco

            // Respuesta al cliente
            cliente.println("HTTP/1.1 200 OK");
            cliente.println("Content-type:text/html");
            cliente.println();
            cliente.println("<html><h1>Texto recibido correctamente</h1></html>");
            Serial.println("Texto recibido: " + textoRecibido);
            textoEnviadoPorBT = false; // Reset para enviar por Bluetooth
        } else {
            Serial.println("No se recibió el texto");
        }
        cliente.stop();  // Desconectar al cliente
        Serial.println("Cliente desconectado");
    }
}

void enviarTextoBluetooth() {
    if (!textoRecibido.isEmpty() && !textoEnviadoPorBT) {
        Serial.println("Enviando texto por Bluetooth...");
        SerialBT.println("Texto recibido: " + textoRecibido);
        Serial.println("Texto enviado por Bluetooth.");
        
        textoEnviadoPorBT = true;  // Marcar como enviado
        
        // Reiniciar textoRecibido para permitir nuevos envíos
        textoRecibido = "";  // Limpiar el texto recibido
    }
}

void setup() {
    Serial.begin(115200);
    conectarWiFi();
    SerialBT.begin("ESP32_Bluetooth");  // Iniciar Bluetooth una vez en setup
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        recibirTexto();  // Recibe el texto desde HTML si está conectado a Wi-Fi
    }

    enviarTextoBluetooth();  // Envía el texto por Bluetooth si hay uno para enviar

    delay(1000);  // Reducir el intervalo de chequeo
}
