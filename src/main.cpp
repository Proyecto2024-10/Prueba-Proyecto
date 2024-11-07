#include <WiFi.h>
#include <BluetoothSerial.h>

// Configuración de WiFi y Bluetooth
const char* ssid = "CATERPILAR";
const char* password = "Carranza";
BluetoothSerial SerialBT;
WiFiServer servidor(80);

// Variables para manejar texto
String textoRecibido = "";
const int matrizBraille[27][6] = {
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
int matrizTexto[30][6]; // Vector para el texto a imprimir
int posicionTexto = 0; // Posición actual en el texto
const unsigned long intervalo = 1000; // Intervalo de 1 segundo
bool columnaMostrada = false;
//Declaración Funciones
void perforar();
void conectarWiFi();
void recibirTexto();
void imprimirBraille();
void enviarTextoPorBluetooth();

// Pines de LEDs
const int ledFila1 = 12; // Fila 1
const int ledFila2 = 26; // Fila 2
const int ledFila3 = 27; // Fila 3
const int ledColumna  = 13; // LED para indicar columna

// Variables de estado
enum Estado { RECEPCION_TEXTO, IMPRESION };
Estado estadoActual = RECEPCION_TEXTO;


unsigned long tiempoAnterior = 0; // Tiempo anterior para controlar el intervalo
int letraActual = 0; // Índice de la letra que se está mostrando
bool ledsMostrados = false; // Marca si los LEDs de la letra actual ya han sido mostrados

void setup() {
    Serial.begin(115200);
    conectarWiFi();
    SerialBT.begin("ESP32_Bluetooth");
    pinMode(ledFila1, OUTPUT);
    pinMode(ledFila2, OUTPUT);
    pinMode(ledFila3, OUTPUT);
    pinMode(ledColumna, OUTPUT);
}

void loop() {
    switch (estadoActual) {
        case RECEPCION_TEXTO:
            recibirTexto();
            break;
        case IMPRESION:
            imprimirBraille();
            break;
    }
}
// Función para conectar WiFi
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

// Función para recibir texto desde la página
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

            for (int i = 0; i < textoRecibido.length(); i++) {
                char letra = textoRecibido[i];
                if (letra >= 'a' && letra <= 'z') {
                    for (int j = 0; j < 6; j++) {
                        matrizTexto[posicionTexto][j] = matrizBraille[letra - 'a'][j];
                    }

                    posicionTexto++;
                } else if (letra == ' ') {
                    for (int j = 0; j < 6; j++) {
                        matrizTexto[posicionTexto][j] = matrizBraille[26][j];
                    }

                    posicionTexto++;
                }
            }
            estadoActual = IMPRESION;
        }
        cliente.stop();
    }
}


// Función para imprimir el texto en Braille
void imprimirBraille() {
    perforar(); // Llama a Perforar para la letra actual
    if (ledsMostrados) {
        enviarTextoPorBluetooth(); // Envía el texto recibido por Bluetooth
        posicionTexto = 0; // Reinicia la posición del texto
        letraActual = 0; // Reinicia el índice de letra
        ledsMostrados = false; // Resetea la marca para la próxima letra
        tiempoAnterior = 0; // Resetea el tiempo para los LEDs
        estadoActual = RECEPCION_TEXTO; // Vuelve al estado de recepción de texto
    }
}

// Modificamos la función perforar para controlar correctamente el incremento
void perforar() {
    unsigned long tiempoActual = millis(); // Obtiene el tiempo actual

    // Verifica si todas las letras han sido procesadas
    if (letraActual >= posicionTexto) {
        ledsMostrados = true; // Marca que se han mostrado todos los LEDs
        return;
    }

    // Control de la visualización de columnas
    if (tiempoActual - tiempoAnterior >= intervalo) {
        // Primero apaga el LED de columna para los primeros 3 bits
        digitalWrite(ledColumna, LOW); // Asegúrate de que esté apagado

        if (!columnaMostrada) { // Si estamos en la primera columna
            // Muestra primera columna (primeros 3 bits)
            digitalWrite(ledFila1, matrizTexto[letraActual][0]);
            digitalWrite(ledFila2, matrizTexto[letraActual][1]);
            digitalWrite(ledFila3, matrizTexto[letraActual][2]);

            // Imprime en el serial la letra y su representación en Braille
            Serial.print("Imprimiendo letra: ");
            Serial.print(textoRecibido[letraActual]);
            Serial.print(" - Braille: ");
            Serial.print(matrizTexto[letraActual][0]);
            Serial.print(matrizTexto[letraActual][1]);
            Serial.print(matrizTexto[letraActual][2]);
            Serial.print(" ");

            columnaMostrada = true; // Cambiamos a la segunda columna
            tiempoAnterior = tiempoActual; // Actualiza el tiempo
        } 
        else { // Si estamos en la segunda columna
            // Enciende el LED de columna y muestra la segunda columna (últimos 3 bits)
            digitalWrite(ledColumna, HIGH); // Prende el LED de columna
            digitalWrite(ledFila1, matrizTexto[letraActual][3]);
            digitalWrite(ledFila2, matrizTexto[letraActual][4]);
            digitalWrite(ledFila3, matrizTexto[letraActual][5]);

            // Completa la impresión en el serial de la representación en Braille
            Serial.print(matrizTexto[letraActual][3]);
            Serial.print(matrizTexto[letraActual][4]);
            Serial.println(matrizTexto[letraActual][5]);

            // Actualizamos para pasar a la siguiente letra
            letraActual++;              // Avanzamos a la siguiente letra
            columnaMostrada = false;    // Reinicia el estado de columna
            tiempoAnterior = tiempoActual; // Actualiza el tiempo para el próximo ciclo
        }
    }
}


// Función para enviar el texto por Bluetooth
void enviarTextoPorBluetooth() {
    if (!textoRecibido.isEmpty()) {
        Serial.println("Enviando texto por Bluetooth...");
        SerialBT.println("Texto recibido: " + textoRecibido);
        Serial.println("Texto enviado por Bluetooth.");
        
        textoRecibido = "";  // Limpiamos el texto después de enviarlo
    }
}

