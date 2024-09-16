#include <WiFi.h>
#include <AccelStepper.h>

// Estados 
enum Estado {
  CONECTAR_WIFI,
  ESPERAR_CLIENTE,
  PROCESAR_PETICION,
  MOVER_MOTORES,
  ESPERAR_MOTORES
};

Estado estadoActual = CONECTAR_WIFI;  

const char* nombreWifi = "WIFI1";     // Nombre de mi red WiFi
const char* contrasenaWifi = "del.sel1"; // Contraseña de mi red WiFi

// Pines para los motores (ejemplo)
const int DirMotor1 = 15;
const int StepMotor1 = 16;
const int DirMotor2 = 17;
const int StepMotor2 = 18;
const int DirMotor3 = 19;
const int StepMotor3 = 20;


AccelStepper motor1(1, StepMotor1, DirMotor1);
AccelStepper motor2(1, StepMotor2, DirMotor2);
AccelStepper motor3(1, StepMotor3, DirMotor3);

WiFiServer servidor(80); 
WiFiClient clienteActual;  
String peticion = "";  


void enviarRespuestaHTTP(WiFiClient cliente, String contenido) {
  cliente.println("HTTP/1.1 200 OK");
  cliente.println("Content-type:text/html");
  cliente.println();
  cliente.println("<!DOCTYPE HTML>");
  cliente.println("<html>" + contenido + "</html>");
  cliente.println();
}

// Función para conectar a WiFi
void conectarWiFi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(nombreWifi, contrasenaWifi);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado a WiFi. IP: " + WiFi.localIP().toString());
    servidor.begin(); 
    estadoActual = ESPERAR_CLIENTE;  
  } else {
    Serial.println("Intentando conectar...");
  }
}

// Función para esperar clientes
void esperarCliente() {
  clienteActual = servidor.available();  
  if (clienteActual) {
    Serial.println("Nuevo cliente conectado");
    estadoActual = PROCESAR_PETICION;  
    peticion = "";  
  }
}

// Función para procesar la petición HTTP
void procesarPeticion() {
  if (clienteActual.connected()) {
    while (clienteActual.available()) {
      char c = clienteActual.read();
      peticion += c;

      if (c == '\n') {
        Serial.println("Petición completa recibida:");
        Serial.println(peticion);

        int indiceComando = peticion.indexOf("/?comando=");
        if (indiceComando != -1) {
          String comando = peticion.substring(indiceComando + 10, peticion.indexOf(" ", indiceComando + 10));
          comando.trim(); 

          // Mover el motor 1
          if (comando.equalsIgnoreCase("motor1")) {
            motor1.moveTo(600);
            enviarRespuestaHTTP(clienteActual, "<h1>Motor 1 movido</h1>");
          } 
          // Mover el motor 2
          else if (comando.equalsIgnoreCase("motor2")) {
            motor2.moveTo(600);
            enviarRespuestaHTTP(clienteActual, "<h1>Motor 2 movido</h1>");
          } 
          // Mover el motor 3
          else if (comando.equalsIgnoreCase("motor3")) {
            motor3.moveTo(600);
            enviarRespuestaHTTP(clienteActual, "<h1>Motor 3 movido</h1>");
          } 
          else {
            enviarRespuestaHTTP(clienteActual, "<h1>Error: Comando desconocido</h1>");
          }
        } else {
          enviarRespuestaHTTP(clienteActual, "<h1>Error: Comando no encontrado</h1>");
        }
        clienteActual.stop(); 
        estadoActual = MOVER_MOTORES;  
        break;
      }
    }
  } else {
    clienteActual.stop();
    estadoActual = ESPERAR_CLIENTE; 
  }
}

// Función para mover los motores
void moverMotores() {

  motor1.run();
  motor2.run();
  motor3.run();


  estadoActual = ESPERAR_MOTORES;
}

// Función para esperar que los motores terminen de moverse
void esperarMotores() {

  if (!motor1.isRunning() && !motor2.isRunning() && !motor3.isRunning()) {
    Serial.println("Movimiento de motores completado");
    estadoActual = ESPERAR_CLIENTE;  
  }
}

void setup() {
  Serial.begin(115200);

  // Configuración de los motores
  motor1.setMaxSpeed(700);
  motor1.setAcceleration(300);
  motor2.setMaxSpeed(700);
  motor2.setAcceleration(300);
  motor3.setMaxSpeed(700);
  motor3.setAcceleration(300);
}

void loop() {
  // MEF
  Serial.println(estadoActual);
  switch (estadoActual) {
    case CONECTAR_WIFI:
      conectarWiFi();  // Conectar a WiFi
      break;

    case ESPERAR_CLIENTE:
      esperarCliente();  // Esperar un cliente
      break;

    case PROCESAR_PETICION:
      procesarPeticion();  // Procesar la petición del cliente
      break;

    case MOVER_MOTORES:
      moverMotores();  // Mover los motores según el comando recibido
      break;

    case ESPERAR_MOTORES:
      esperarMotores();  // Esperar hasta que los motores terminen de moverse
      break;
  }
}
