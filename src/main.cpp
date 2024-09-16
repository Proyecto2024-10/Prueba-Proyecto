#include <WiFi.h>
#include <AccelStepper.h>


const char* nombreWifi = "WIFI1";     // Nombre de mi red WiFi
const char* contrasenaWifi = "del.sel1"; // Contraseña de mi red WiFi

// Pines de ejemplo 111
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

void enviarRespuestaHTTP(WiFiClient cliente, String contenido) {
  cliente.println("HTTP/1.1 200 OK");
  cliente.println("Content-type:text/html");
  cliente.println();
  cliente.println("<!DOCTYPE HTML>");
  cliente.println("<html>" + contenido + "</html>");
  cliente.println();
}

void setup() {
  Serial.begin(115200);

  motor1.setMaxSpeed(700);
  motor1.setAcceleration(300);
  motor2.setMaxSpeed(700);
  motor2.setAcceleration(300);
  motor3.setMaxSpeed(700);
  motor3.setAcceleration(300);

  // Conectar a la red WiFi
  WiFi.begin(nombreWifi, contrasenaWifi);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.print("Conectado a WiFi. IP: ");
  Serial.println(WiFi.localIP());

  servidor.begin(); 
  Serial.println("Servidor iniciado");
}

void loop() {
  WiFiClient cliente = servidor.available(); 

  if (cliente) {
    Serial.println("Nuevo cliente conectado");
    String peticion = "";
    while (cliente.connected()) {
      if (cliente.available()) {
        char c = cliente.read();
        peticion += c;

        if (c == '\n') {
          Serial.println("Petición completa recibida:");
          Serial.println(peticion);

          int indiceComando = peticion.indexOf("/?comando=");
          if (indiceComando != -1) {
            String comando = peticion.substring(indiceComando + 10, peticion.indexOf(" ", indiceComando + 10));
            comando.trim(); 

            // Comando para mover el motor 1
            if (comando.equalsIgnoreCase("motor1")) {
              motor1.moveTo(600);
              enviarRespuestaHTTP(cliente, "<h1>Motor 1 movido</h1>");
            
            // Comando para mover el motor 2
            } else if (comando.equalsIgnoreCase("motor2")) {
              motor2.moveTo(600);
              enviarRespuestaHTTP(cliente, "<h1>Motor 2 movido</h1>");
            
            // Comando para mover el motor 3
            } else if (comando.equalsIgnoreCase("motor3")) {
              motor3.moveTo(600);
              enviarRespuestaHTTP(cliente, "<h1>Motor 3 movido</h1>");
            
            } else {
              
              enviarRespuestaHTTP(cliente, "<h1>Error: Comando desconocido</h1>");
            }
          } else {
            enviarRespuestaHTTP(cliente, "<h1>Error: Comando no encontrado</h1>");
          }
          break;  // Salir del loop
        }
      }
    }
    cliente.stop();  // Desconectar al cliente
    Serial.println("Cliente desconectado");
  }

  motor1.run();
  motor2.run();
  motor3.run();
}
