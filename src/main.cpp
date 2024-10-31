#include <WiFi.h>
#include <BluetoothSerial.h>

const char* ssid = "CATERPILAR";
const char* password = "Carranza";

BluetoothSerial SerialBT;
WiFiServer servidor(80);
String textoRecibido = "";
bool textoEnviadoPorBT = false;

const int ledPin = 2;

unsigned long tiempoAnterior = 0;
const unsigned long intervaloChequeo = 1000;

unsigned long tiempoEncendidoLed = 0;
const unsigned long duracionEncendidoLed = 1000;
bool ledEncendido = false;

void conectarWiFi() {
    WiFi.begin(ssid, password);
    unsigned long tiempoInicio = millis();
    const unsigned long tiempoEspera = 5000;

    Serial.println("Conectando a WiFi...");

    while (WiFi.status() != WL_CONNECTED && millis() - tiempoInicio < tiempoEspera) {
        // Espera no bloqueante de conexión Wi-Fi
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Conectado a WiFi!");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
        servidor.begin();
    } else {
        Serial.println("No se pudo conectar a WiFi.");
    }
}

void recibirTexto() {
    WiFiClient cliente = servidor.available();
    
    if (cliente) {
        Serial.println("Cliente conectado");
        String peticion = cliente.readStringUntil('\r');
        cliente.flush();

        int indiceTexto = peticion.indexOf("/?texto=");
        if (indiceTexto != -1) {
            textoRecibido = peticion.substring(indiceTexto + 8, peticion.indexOf(" ", indiceTexto));
            textoRecibido.trim();
            textoRecibido.replace("%20", " ");

            cliente.println("HTTP/1.1 200 OK");
            cliente.println("Content-type:text/html");
            cliente.println();
            cliente.println("<html><h1>Texto recibido correctamente</h1>");
            cliente.println("<p>¿Está seguro de enviar el texto?</p>");
            cliente.println("<button onclick=\"fetch('/enviar')\">Enviar por Bluetooth</button>");
            cliente.println("<p>Espere mientras se procesa...</p>");
            cliente.println("</html>");
            Serial.println("Texto recibido: " + textoRecibido);
            textoEnviadoPorBT = false;
        } else {
            Serial.println("No se recibió el texto");
        }
        cliente.stop();
        Serial.println("Cliente desconectado");
    }
}

void encenderLed() {
    digitalWrite(ledPin, HIGH);
    tiempoEncendidoLed = millis();
    ledEncendido = true;
}

void enviarTextoBluetooth() {
    if (!textoRecibido.isEmpty() && !textoEnviadoPorBT) {
        Serial.println("Enviando texto por Bluetooth...");
        SerialBT.println("Texto recibido: " + textoRecibido);
        Serial.println("Texto enviado por Bluetooth.");
        
        textoEnviadoPorBT = true;
        textoRecibido = "";
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    conectarWiFi();

    SerialBT.begin("ESP32_Bluetooth");
    Serial.println("Bluetooth iniciado, esperando conexión...");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        recibirTexto();
    }

    if (!textoRecibido.isEmpty() && !textoEnviadoPorBT) {
        encenderLed();
        enviarTextoBluetooth();
    }

    if (ledEncendido && millis() - tiempoEncendidoLed >= duracionEncendidoLed) {
        digitalWrite(ledPin, LOW);
        ledEncendido = false;
    }

    if (millis() - tiempoAnterior >= intervaloChequeo) {
        tiempoAnterior = millis();
    }
}
