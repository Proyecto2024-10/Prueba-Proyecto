#include <WiFi.h>
#include <Servo.h>

enum Estado {
  CONECTAR_WIFI,
  ESPERAR_CLIENTE,
  PROCESAR_PETICION,
  MOVER_SERVO,
  PERFORAR_BRAILLE
};

Estado estadoActual = CONECTAR_WIFI;

const char* nombreWifi = "WIFI1";
const char* contrasenaWifi = "del.sel1";

const int pinServo = 18;
const int pinMotor = 19;

Servo servo;

const int braille[26][6] = {
    {1, 0, 0, 0, 0, 0}, // a
    {1, 1, 0, 0, 0, 0}, // b
    {1, 0, 0, 1, 0, 0}, // c
    {1, 0, 0, 1, 1, 0}, // d
    {1, 0, 0, 0, 1, 0}, // e
    {1, 1, 0, 1, 0, 0}, // f
    {1, 1, 0, 1, 1, 0}, // g
    {1, 1, 0, 0, 1, 0}, // h
    {0, 1, 0, 1, 0, 0}, // i
    {0, 1, 0, 1, 1, 0}, // j
    {1, 0, 1, 0, 0, 0}, // k
    {1, 1, 1, 0, 0, 0}, // l
    {1, 0, 1, 1, 0, 0}, // m
    {1, 0, 1, 1, 1, 0}, // n
    {1, 0, 1, 0, 1, 0}, // o
    {1, 1, 1, 1, 0, 0}, // p
    {1, 1, 1, 1, 1, 0}, // q
    {1, 1, 1, 0, 1, 0}, // r
    {0, 1, 1, 1, 0, 0}, // s
    {0, 1, 1, 1, 1, 0}, // t
    {1, 0, 1, 0, 0, 1}, // u
    {1, 1, 1, 0, 0, 1}, // v
    {0, 1, 0, 1, 1, 1}, // w
    {1, 0, 1, 1, 0, 1}, // x
    {1, 0, 1, 1, 1, 1}, // y
    {1, 0, 1, 0, 1, 1}, // z
};

WiFiServer servidor(80);
WiFiClient clienteActual;
String peticion = "";
String textoBraille = "";
int letraActual = 0;

unsigned long tiempoServo = 0;
unsigned long tiempoMotor = 0;
unsigned long tiempoPerforacion = 0;
const unsigned long intervalo = 500; // Intervalo en milisegundos

void enviarPaginaHTML(WiFiClient cliente) {
  cliente.println("HTTP/1.1 200 OK");
  cliente.println("Content-type:text/html");
  cliente.println();
  cliente.println("<!DOCTYPE HTML>");
  cliente.println("<html>");
  cliente.println("<head>");
  cliente.println("<style>");
  cliente.println("body { font-family: Arial, sans-serif; background-color: #f4f4f4; padding: 20px; }");
  cliente.println("h1 { color: #333; }");
  cliente.println("form { background-color: white; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }");
  cliente.println("input[type='text'] { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 4px; }");
  cliente.println("input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 15px; border: none; border-radius: 4px; cursor: pointer; }");
  cliente.println("input[type='submit']:hover { background-color: #45a049; }");
  cliente.println("</style>");
  cliente.println("</head>");
  cliente.println("<body>");
  cliente.println("<h1>Convertir Texto a Braille</h1>");
  cliente.println("<p>Ingresa un texto para convertirlo a braille y perforarlo en la cinta:</p>");
  cliente.println("<form action=\"/\" method=\"GET\">");
  cliente.println("<input type=\"text\" name=\"texto\" placeholder=\"Ingresa el texto...\" required>");
  cliente.println("<input type=\"submit\" value=\"Enviar\">");
  cliente.println("</form>");
  cliente.println("</body>");
  cliente.println("</html>");
  cliente.println();
}

void conectarWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando a WiFi...");
    WiFi.begin(nombreWifi, contrasenaWifi);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Conectado a WiFi. IP: " + WiFi.localIP().toString());
      servidor.begin();
      estadoActual = ESPERAR_CLIENTE;
    }
  }
}

void moverServoA(float posicion_mm) {
    int posicionServo = map(posicion_mm, 0, 9, 0, 180);
    if (millis() - tiempoServo >= intervalo) {
        servo.write(posicionServo);
        tiempoServo = millis();
    }
}

void perforarPunto() {
    if (millis() - tiempoMotor >= intervalo) {
        static bool motorActivo = false;
        motorActivo = !motorActivo;
        digitalWrite(pinMotor, motorActivo ? HIGH : LOW);
        tiempoMotor = millis();
    }
}

void escribirLetraBraille(char letra) {
    int indice = letra - 'a';
    for (int i = 0; i < 6; i++) {
        if (braille[indice][i] == 1) {
            if (millis() - tiempoPerforacion >= intervalo) {
                perforarPunto();
                tiempoPerforacion = millis();
            }
        }
        moverServoA(2 * (i + 1));
    }
}

void esperarCliente() {
    clienteActual = servidor.available();
    if (clienteActual) {
        Serial.println("Cliente conectado");
        estadoActual = PROCESAR_PETICION;
    }
}

void procesarPeticion() {
    if (clienteActual.connected()) {
        while (clienteActual.available()) {
            char c = clienteActual.read();
            peticion += c;
            if (c == '\n') {
                int indiceTexto = peticion.indexOf("/?texto=");
                if (indiceTexto != -1) {
                    textoBraille = peticion.substring(indiceTexto + 8, peticion.indexOf(" ", indiceTexto + 8));
                    textoBraille.trim();
                    enviarPaginaHTML(clienteActual);
                    estadoActual = MOVER_SERVO;
                    letraActual = 0;
                }
                clienteActual.stop();
                break;
            }
        }
    }
}

void moverBraille() {
    if (letraActual < textoBraille.length()) {
        escribirLetraBraille(textoBraille[letraActual]);
        letraActual++;
        moverServoA(9);
    } else {
        estadoActual = ESPERAR_CLIENTE;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(pinMotor, OUTPUT);
    digitalWrite(pinMotor, LOW);
    servo.attach(pinServo);
    moverServoA(0);
    conectarWiFi();
}

void loop() {
    switch (estadoActual) {
        case CONECTAR_WIFI:
            conectarWiFi();
            break;

        case ESPERAR_CLIENTE:
            esperarCliente();
            break;

        case PROCESAR_PETICION:
            procesarPeticion();
            break;

        case MOVER_SERVO:
            moverBraille();
            break;
    }
}
