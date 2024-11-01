#include <WiFi.h>

// Credenciales WiFi
const char* ssid = "CATERPILAR";
const char* password = "Carranza";

// Definición de pines
#define stepPin1 33
#define dirPin1 32
#define stepPin2 4
#define dirPin2 16
#define stepPin3 15
#define dirPin3 2

// Variables de control
unsigned long previousMillis = 0;
const long movimientoDuracion = 3000;  // Duración de movimiento por motor
const long pasoIntervalo = 4;  // Intervalo de 4 ms entre pasos
int motorActual = 0;  // Motor actual a girar
bool motoresActivos = false;

WiFiServer servidor(80);
<<<<<<< Updated upstream
=======
String textoRecibido = "";
bool textoEnviadoPorBT = false;

unsigned long tiempoAnterior = 0;
const unsigned long intervaloChequeo = 1000;

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
int posicionTexto = 0;

// Variables de estado de impresión
unsigned long tiempoImpresion = 0;
const unsigned long intervaloPerforacion = 1000; // Tiempo entre perforaciones
>>>>>>> Stashed changes

// Funciones
void conectarWiFi() {
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando...");
  }
  Serial.println("Conectado a WiFi!");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());  // Mostrar la IP asignada
}

<<<<<<< Updated upstream
void moverMotor(int motor) {
  int stepPin, dirPin;

  // Seleccionar pines según el motor
  switch (motor) {
    case 1:
      stepPin = stepPin1;
      dirPin = dirPin1;
      break;
    case 2:
      stepPin = stepPin2;
      dirPin = dirPin2;
      break;
    case 3:
      stepPin = stepPin3;
      dirPin = dirPin3;
      break;
    default:
      return;  // No hacer nada si el motor no es válido
  }

  // Configurar dirección y girar el motor
  digitalWrite(dirPin, HIGH);  // Fijar dirección del motor
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(100);  // Ajustar tiempo para simular un paso
  digitalWrite(stepPin, LOW);
  delayMicroseconds(100);  // Ajustar tiempo para simular un paso
}

void procesarCliente(WiFiClient cliente) {
  String peticion = "";
  while (cliente.connected()) {
    if (cliente.available()) {
      char c = cliente.read();
      peticion += c;

      // Verificar si se recibió la petición completa
      if (c == '\n') {
        Serial.println("Petición recibida: ");
        Serial.println(peticion);

        // Verificar si el texto es "ON" o "OFF"
        if (peticion.indexOf("GET /?texto=ON") != -1) {
          motoresActivos = true;
          motorActual = 1;  // Iniciar con el motor 1
          previousMillis = millis();  // Reiniciar temporizador
        } else if (peticion.indexOf("GET /?texto=OFF") != -1) {
          motoresActivos = false;  // Apagar motores
          // Desactivar todos los pasos
          digitalWrite(stepPin1, LOW);
          digitalWrite(stepPin2, LOW);
          digitalWrite(stepPin3, LOW);
=======
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
>>>>>>> Stashed changes
        }

<<<<<<< Updated upstream
        // Enviar respuesta HTTP
        cliente.println("HTTP/1.1 200 OK");
        cliente.println("Content-type:text/html");
        cliente.println();
        cliente.println("<!DOCTYPE HTML>");
        cliente.println("<html><h1>Control de Motores</h1></html>");
        break;
      }
=======
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
>>>>>>> Stashed changes
    }
  }
  cliente.stop();  // Desconectar al cliente
}

void setup() {
<<<<<<< Updated upstream
  Serial.begin(115200);
=======
    Serial.begin(115200);
    conectarWiFi();
>>>>>>> Stashed changes

  // Configurar pines
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  
  // Conectar a WiFi
  conectarWiFi();
  
  // Iniciar el servidor web
  servidor.begin();
}

void loop() {
  unsigned long currentMillis = millis();

<<<<<<< Updated upstream
  // Control de motores secuencialmente
  if (motoresActivos) {
    if (currentMillis - previousMillis >= movimientoDuracion) {
      previousMillis = currentMillis;

      motorActual++;
      if (motorActual > 3) {
        motorActual = 1;  // Reiniciar el ciclo
      }
=======
    if (estadoActual == IMPRESION) {
        imprimirBraille();
        if (posicionTexto == 0) {
            enviarTextoBluetooth(); // Enviar texto por Bluetooth una vez que se complete la impresión
            estadoActual = RECEPCION_TEXTO; // Volver al estado de recepción de texto
        }
>>>>>>> Stashed changes
    }
    moverMotor(motorActual);
    delay(pasoIntervalo);  // Esperar 4 ms entre pasos
  } else {
    // Desactivar todos los pasos si motoresActivos es false
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    digitalWrite(stepPin3, LOW);
  }

  // Verificar solicitudes HTTP
  WiFiClient cliente = servidor.available();
  if (cliente) {
    procesarCliente(cliente);
  }
}
