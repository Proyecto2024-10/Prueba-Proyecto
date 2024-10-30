#include <WiFi.h>
#include <BluetoothSerial.h>

const char* ssid = "CATERPILAR";
const char* password = "Carranza";

BluetoothSerial SerialBT;
WiFiServer servidor(80);  // Servidor en el puerto 80
String textoRecibido = "";
bool textoEnviadoPorBT = false;

const int ledPin = 2;  // Pin donde está conectado el LED

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

            // Reemplazar %20 por espacios
            textoRecibido.replace("%20", " ");  // Reemplaza los %20 por espacios

            // Respuesta al cliente
            cliente.println("HTTP/1.1 200 OK");
            cliente.println("Content-type:text/html");
            cliente.println();
            cliente.println("<html><h1>Texto recibido correctamente</h1>");
            cliente.println("<p>¿Está seguro de enviar el texto?</p>");
            cliente.println("<button onclick=\"fetch('/enviar')\">Enviar por Bluetooth</button>");
            cliente.println("<p>Espere mientras se procesa...</p>");
            cliente.println("</html>");
            Serial.println("Texto recibido: " + textoRecibido);
            textoEnviadoPorBT = false; // Reset para enviar por Bluetooth
        } else {
            Serial.println("No se recibió el texto");
        }
        cliente.stop();  // Desconectar al cliente
        Serial.println("Cliente desconectado");
    }
}

void encenderLed() {
    digitalWrite(ledPin, HIGH);  // Encender el LED
    delay(1000);  // Mantener el LED encendido durante 1 segundo
    digitalWrite(ledPin, LOW);  // Apagar el LED
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
    pinMode(ledPin, OUTPUT);  // Configurar el pin del LED como salida
    conectarWiFi();

    SerialBT.begin("ESP32_Bluetooth");  // Iniciar Bluetooth
    Serial.println("Bluetooth iniciado, esperando conexión...");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        recibirTexto();  // Recibe el texto desde HTML si está conectado a Wi-Fi
    }

    if (!textoRecibido.isEmpty() && !textoEnviadoPorBT) {
        encenderLed();  // Ejecutar acción antes de enviar por Bluetooth
        enviarTextoBluetooth();  // Envía el texto por Bluetooth si hay uno para enviar
    }

    delay(1000);  // Reducir el intervalo de chequeo
}
