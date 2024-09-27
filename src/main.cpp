#include <WiFi.h>
#include <WebServer.h>

// Pines de los motores
#define pinStep1 33
#define pinDir1 32
#define pinStep2 4
#define pinDir2 16
#define pinStep3 15
#define pinDir3 2

// Intervalos de tiempo y estado de los pasos para cada motor
unsigned long tiempoAnterior1 = 0;
unsigned long tiempoAnterior2 = 0;
unsigned long tiempoAnterior3 = 0;

const long intervalo = 1;  // 1 milisegundo entre pasos (ajustar según velocidad)
int estadoPaso1 = LOW;
int estadoPaso2 = LOW;
int estadoPaso3 = LOW;

// Configuración de red WiFi
const char* ssid = "WIFI1";
const char* password = "del.sel1";

// Inicializar el servidor web
WebServer server(80);

// Definición de estados de los motores
enum EstadoMotor {
    MOTOR_APAGADO,
    MOTOR_ACTIVO
};

// Variables para el estado de cada motor
EstadoMotor estadoMotor1 = MOTOR_APAGADO;
EstadoMotor estadoMotor2 = MOTOR_APAGADO;
EstadoMotor estadoMotor3 = MOTOR_APAGADO;

// Función para cambiar el estado de un motor
void cambiarEstadoMotor(int motor, EstadoMotor nuevoEstado) {
    switch(motor) {
        case 1:
            estadoMotor1 = nuevoEstado;
            break;
        case 2:
            estadoMotor2 = nuevoEstado;
            break;
        case 3:
            estadoMotor3 = nuevoEstado;
            break;
    }
}

// Función que procesa el texto recibido y cambia el estado en consecuencia
void procesarTexto(String texto) {
    texto.trim();  // Eliminar espacios en blanco

    if (texto == "1") {
        cambiarEstadoMotor(1, MOTOR_ACTIVO);  // Activa el motor 1
        server.send(200, "text/plain", "Motor 1 Activado");
    } else if (texto == "2") {
        cambiarEstadoMotor(2, MOTOR_ACTIVO);  // Activa el motor 2
        server.send(200, "text/plain", "Motor 2 Activado");
    } else if (texto == "3") {
        cambiarEstadoMotor(3, MOTOR_ACTIVO);  // Activa el motor 3
        server.send(200, "text/plain", "Motor 3 Activado");
    } else if (texto == "frenar1") {
        cambiarEstadoMotor(1, MOTOR_APAGADO);  // Apaga el motor 1
        server.send(200, "text/plain", "Motor 1 Frenado");
    } else if (texto == "frenar2") {
        cambiarEstadoMotor(2, MOTOR_APAGADO);  // Apaga el motor 2
        server.send(200, "text/plain", "Motor 2 Frenado");
    } else if (texto == "frenar3") {
        cambiarEstadoMotor(3, MOTOR_APAGADO);  // Apaga el motor 3
        server.send(200, "text/plain", "Motor 3 Frenado");
    } else {
        server.send(400, "text/plain", "Comando no válido");
    }
}

// Ruta de control: recibe el texto enviado desde la página web
void handleControl() {
    String texto = server.arg("texto");
    procesarTexto(texto);  // Procesar el texto ingresado
}

void setup() {
    Serial.begin(115200);

    // Configurar pines de los motores
    pinMode(pinStep1, OUTPUT);
    pinMode(pinDir1, OUTPUT);
    digitalWrite(pinDir1, LOW);  // Inicialmente apagados

    pinMode(pinStep2, OUTPUT);
    pinMode(pinDir2, OUTPUT);
    digitalWrite(pinDir2, LOW);  // Inicialmente apagados

    pinMode(pinStep3, OUTPUT);
    pinMode(pinDir3, OUTPUT);
    digitalWrite(pinDir3, LOW);  // Inicialmente apagados

    // Conectar a la red WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Configurar rutas del servidor
    server.on("/control", handleControl);  // Ruta para procesar el texto

    // Iniciar servidor
    server.begin();
    Serial.println("Servidor web iniciado");
}

void loop() {
    server.handleClient();  // Procesar solicitudes del servidor web

    unsigned long tiempoActual = millis();

    // Control del motor 1 con la MEF
    if (estadoMotor1 == MOTOR_ACTIVO) {
        if (tiempoActual - tiempoAnterior1 >= intervalo) {
            tiempoAnterior1 = tiempoActual;
            estadoPaso1 = !estadoPaso1;
            digitalWrite(pinStep1, estadoPaso1);
        }
    }

    // Control del motor 2 con la MEF
    if (estadoMotor2 == MOTOR_ACTIVO) {
        if (tiempoActual - tiempoAnterior2 >= intervalo) {
            tiempoAnterior2 = tiempoActual;
            estadoPaso2 = !estadoPaso2;
            digitalWrite(pinStep2, estadoPaso2);
        }
    }

    // Control del motor 3 con la MEF
    if (estadoMotor3 == MOTOR_ACTIVO) {
        if (tiempoActual - tiempoAnterior3 >= intervalo) {
            tiempoAnterior3 = tiempoActual;
            estadoPaso3 = !estadoPaso3;
            digitalWrite(pinStep3, estadoPaso3);
        }
    }
}
