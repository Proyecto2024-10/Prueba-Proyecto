#include <WiFi.h>
#include <BluetoothSerial.h>

const char* ssid = "CATERPILAR";
const char* password = "Carranza";

BluetoothSerial SerialBT;
WiFiServer servidor(80);
String textoRecibido = "";
bool textoEnviadoPorBT = false;

enum Estado { RECEPCION_TEXTO, IMPRESION };
Estado estadoActual = RECEPCION_TEXTO;

int vectorBraille[27][6] = {
    {1, 0, 0, 0, 0, 0}, // "a"
    {1, 1, 0, 0, 0, 0}, // "b"
    {1, 0, 0, 1, 0, 0}, // "c"
    {1, 0, 0, 1, 1, 0}, // "d"
    {1, 0, 0, 0, 1, 0}, // "e"
    {1, 1, 0, 1, 0, 0}, // "f"
    {1, 1, 0, 1, 1, 0}, // "g"
    {1, 1, 0, 0, 1, 0}, // "h"
    {0, 1, 0, 1, 0, 0}, // "i"
    {0, 1, 0, 1, 1, 0}, // "j"
    {1, 0, 1, 0, 0, 0}, // "k"
    {1, 1, 1, 0, 0, 0}, // "l"
    {1, 0, 1, 1, 0, 0}, // "m"
    {1, 0, 1, 1, 1, 0}, // "n"
    {1, 0, 1, 0, 1, 0}, // "o"
    {1, 1, 1, 1, 0, 0}, // "p"
    {1, 1, 1, 1, 1, 0}, // "q"
    {1, 1, 1, 0, 1, 0}, // "r"
    {0, 1, 1, 1, 0, 0}, // "s"
    {0, 1, 1, 1, 1, 0}, // "t"
    {1, 0, 1, 0, 0, 1}, // "u"
    {1, 1, 1, 0, 0, 1}, // "v"
    {0, 1, 0, 1, 1, 1}, // "w"
    {1, 0, 1, 1, 0, 1}, // "x"
    {1, 0, 1, 1, 1, 1}, // "y"
    {1, 0, 1, 0, 1, 1}, // "z"
    {0, 0, 0, 0, 0, 0}  // " " (espacio)
};

int vectorTexto[15][6];
int posicionTexto = 0;
const int dirPinCinta = 32;
const int stepPinCinta = 33;
const int dirPinLeva = 16;
const int stepPinLeva = 4;
const int dirPinCorte = 2;
const int stepPinCorte = 15;
const int servoPin = 0;

void conectarWiFi() {
    WiFi.begin(ssid, password);
    unsigned long tiempoInicio = millis();
    const unsigned long tiempoEspera = 5000;

    while (WiFi.status() != WL_CONNECTED && millis() - tiempoInicio < tiempoEspera) {}
    
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
        String peticion = cliente.readStringUntil('\r');
        cliente.flush();

        int indiceTexto = peticion.indexOf("/?texto=");
        if (indiceTexto != -1) {
            textoRecibido = peticion.substring(indiceTexto + 8, peticion.indexOf(" ", indiceTexto));
            textoRecibido.trim();
            textoRecibido.replace("%20", " ");
            Serial.println("Texto recibido: " + textoRecibido);
            textoEnviadoPorBT = false;

            for (int i = 0; i < textoRecibido.length(); i++) {
                char letra = textoRecibido[i];
                if (letra >= 97 && letra <= 122) { //97 es "a" y 122 es "z"
                    for (int j = 0; j < 6; j++) {
                        vectorTexto[posicionTexto][j] = vectorBraille[letra - 97][j];
                    }
                    posicionTexto++;
                } else if (letra == 32) { //32 es " "
                    for (int j = 0; j < 6; j++) {
                        vectorTexto[posicionTexto][j] = vectorBraille[26][j];
                    }
                    posicionTexto++;
                }
            }
            estadoActual = IMPRESION;
        } else {
            Serial.println("No se recibió el texto o se ingresó un caracter inválido");
        }
        cliente.stop();
    }
}

void moverCinta() {
// CODIGO MOVIMIENTO CINTA
}

void perforar() {
// CODIGO PERFORACION
}

void moverServo(int posicion) {
    // CODIGO SERVO
}

void imprimirBraille() {
    static int letraActual; // Control de la letra actual
    static int columnaActual; // Control de la columna actual
    static int puntoActual; // Control del punto actual

    for (letraActual = 0; letraActual < posicionTexto; letraActual++) { // Recorre todas las letras
        for (columnaActual = 0; columnaActual < 2; columnaActual++) { // Dos columnas por letra
            for (puntoActual = 0; puntoActual < 3; puntoActual++) { // Tres puntos por columna
                if (vectorTexto[letraActual][columnaActual * 3 + puntoActual] == 1) {
                    moverServo(puntoActual); // Mover el servo a la posición del punto
                    perforar();
                }
            }
            moverCinta(); // Mover la cinta a la siguiente posición
        }
    }


    letraActual = 0; 
    posicionTexto = 0; 
    columnaActual = 0;
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
    conectarWiFi();

    SerialBT.begin("ESP32_Bluetooth");
    Serial.println("Bluetooth iniciado, esperando conexión...");

    pinMode(dirPinCinta, OUTPUT);
    pinMode(stepPinCinta, OUTPUT);
    pinMode(dirPinLeva, OUTPUT);
    pinMode(stepPinLeva, OUTPUT);
    pinMode(dirPinCorte, OUTPUT);
    pinMode(stepPinCorte, OUTPUT);
    pinMode(servoPin, OUTPUT);
}

void loop() {
    switch (estadoActual) {
        case RECEPCION_TEXTO:
            recibirTexto();
            break;
        case IMPRESION:
            imprimirBraille();
            enviarTextoBluetooth();
            break;
    }
}
