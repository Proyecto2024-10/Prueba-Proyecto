#include <WiFi.h>
#include <BluetoothSerial.h>

const char* ssid = "CATERPILAR";        // Nombre de la red Wi-Fi
const char* password = "Carranza";      // Contraseña de la red Wi-Fi

BluetoothSerial SerialBT;                // Objeto para Bluetooth
WiFiServer servidor(80);                 // Servidor en el puerto 80
String textoRecibido = "";               // Variable para almacenar el texto recibido
bool textoEnviadoPorBT = false;          // Bandera para controlar el envío por Bluetooth

// Definición de estados
enum Estado { RECEPCION_TEXTO, IMPRESION };
Estado estadoActual = RECEPCION_TEXTO;

// Vector Braille para letras a-z y espacio
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

// Vector para almacenar la representación Braille del texto
int vectorTexto[100][6]; // Se asume un máximo de 100 caracteres
int posicionTexto = 0;   // Variable para controlar la posición en el vectorTexto

// Variables de estado de impresión
unsigned long tiempoImpresion = 0; // Tiempo de impresión
const unsigned long intervaloPerforacion = 1000; // Tiempo entre perforaciones

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

            // Convertir texto a Braille
            for (int i = 0; i < textoRecibido.length(); i++) {
                char letra = textoRecibido[i];
                if (letra >= 'a' && letra <= 'z') {
                    // Guardar la representación Braille de la letra
                    for (int j = 0; j < 6; j++) {
                        vectorTexto[posicionTexto][j] = vectorBraille[letra - 'a'][j];
                    }
                    posicionTexto++;
                } else if (letra == ' ') {
                    // Guardar la representación Braille del espacio
                    for (int j = 0; j < 6; j++) {
                        vectorTexto[posicionTexto][j] = vectorBraille[26][j]; // Espacio
                    }
                    posicionTexto++;
                }
            }
            estadoActual = IMPRESION; // Cambiar el estado a impresión
        } else {
            Serial.println("No se recibió el texto");
        }
        cliente.stop();
        Serial.println("Cliente desconectado");
    }
}

void imprimirBraille() {
    if (posicionTexto > 0) {
        unsigned long tiempoActual = millis();
        if (tiempoActual - tiempoImpresion >= intervaloPerforacion) {
            // Imprimir el siguiente carácter en Braille
            Serial.print("Imprimiendo: ");
            for (int j = 0; j < 6; j++) {
                Serial.print(vectorTexto[0][j]); // Imprime la representación Braille
            }
            Serial.println();

            // Mover los elementos del vector hacia la izquierda
            for (int i = 1; i < posicionTexto; i++) {
                for (int j = 0; j < 6; j++) {
                    vectorTexto[i - 1][j] = vectorTexto[i][j];
                }
            }
            posicionTexto--; // Disminuir la posición del texto
            tiempoImpresion = tiempoActual; // Actualizar el tiempo de impresión
        }
    }
}

void enviarTextoBluetooth() {
    if (!textoRecibido.isEmpty() && !textoEnviadoPorBT) {
        Serial.println("Enviando texto por Bluetooth...");
        SerialBT.println("Texto recibido: " + textoRecibido);
        Serial.println("Texto enviado por Bluetooth.");
        
        textoEnviadoPorBT = true;
        textoRecibido = ""; // Limpiar el texto recibido después de enviarlo
    }
}

void setup() {
    Serial.begin(115200);
    conectarWiFi();

    SerialBT.begin("ESP32_Bluetooth");
    Serial.println("Bluetooth iniciado, esperando conexión...");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        recibirTexto();
    }

    if (estadoActual == IMPRESION) {
        imprimirBraille();
        if (posicionTexto == 0) {
            enviarTextoBluetooth(); // Enviar texto por Bluetooth una vez que se complete la impresión
            estadoActual = RECEPCION_TEXTO; // Volver al estado de recepción de texto
        }
    }
}
