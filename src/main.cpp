// Inclusión de librerías necesarias
#include <WiFi.h>
#include <BluetoothSerial.h>

// Variables MEF
enum Estado { RECEPCION_TEXTO, IMPRESION };
Estado estadoActual = RECEPCION_TEXTO;

// ---------------------------------
// Configuración de WiFi y Bluetooth
// ---------------------------------
const char* ssid = "CATERPILAR";  // Nombre de la red WiFi
const char* password = "Carranza"; // Contraseña de la red WiFi

BluetoothSerial SerialBT;         // Objeto para la comunicación Bluetooth
WiFiServer servidor(80);          // Servidor en el puerto 80 (HTTP)

// --------------------------------------
// Variables para manejar texto y Braille
// --------------------------------------
String textoRecibido = "";  // Cadena de texto recibida del usuario

// Matriz que representa las letras y el espacio en Braille
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

// --------------------
// Variables de Control
// --------------------
int anguloServo[] = {102, 100, 98};// Patrones de ángulos
int matrizTexto[30][6];  // Vector para almacenar el texto a imprimir en Braille
int posicionTexto = 0;   // Posición actual en el texto
int letraActual = 0;
int cintaFlag = 0;



bool columnaMostrada = false;  // Flag para mostrar la columna
unsigned long tiempoAnterior = 0;
bool corteActivo = false;  // Para saber si ya se ha iniciado el corte
unsigned long tiempoParada = 0;  // Variable para almacenar el tiempo de parada
bool enviandoPulsoServo = false;
unsigned long tiempoAnteriorServo = 0;
unsigned long duracionPulsoServo = 0;
unsigned long inicioPulsoServo = 0;
bool textoImpreso = false;  // Flag para verificar si el texto ha sido impreso
int stepPin1State = LOW;
int stepPin2State = LOW;
int stepPin3State = LOW;
int frecuencia = 750;  // Frecuencia para los motores (Hz)
int frecuencia_1 = 300;  // Otra frecuencia para los motores (mas frecuencia, mas velocidad y viceversa)
int indiceAnguloServo = 0;
unsigned long intervaloCambioServo = 500; // Cambia de ángulo cada 500 ms
int angulo = 0;

// ----------------------
// Configuración de pines 
// ----------------------

// Servo
#define pinServoControl 12

// Motor 1
#define stepPin1 18
#define dirPin1 5
#define enable1 19

// Motor 2
#define stepPin2 33
#define dirPin2 32
#define enable2 25

// Motor tijera
#define stepPin3 27
#define dirPin3 26
#define enable3 14


// ------------------------
// Declaración de funciones 
// ------------------------

void espera();
void perforar();
void moverCinta();
void moverLeva();
void cortarCinta();
void conectarWiFi();
void recibirTexto();
void imprimirBraille();
void enviarTextoPorBluetooth();
void moverServo();


void setup() {
    // Configurar pines para Motor 1
     pinMode(stepPin1, OUTPUT);
    pinMode(dirPin1, OUTPUT);
    pinMode(enable1,OUTPUT);
    digitalWrite(dirPin1, HIGH);  // Fijar dirección del Motor 1
    // Configurar pines para Motor 2
    pinMode(stepPin2, OUTPUT);
    pinMode(dirPin2, OUTPUT);
    pinMode(enable2,OUTPUT);
    digitalWrite(dirPin2, HIGH);  // Fijar dirección del Motor 2

    // Configurar pines para Motor 3
   pinMode(stepPin3, OUTPUT);
    pinMode(dirPin3, OUTPUT);
    pinMode(enable3,OUTPUT);
    digitalWrite(dirPin3, HIGH);  // Fijar dirección del Motor 3
    ledcSetup(0,frecuencia_1,8);
    ledcSetup(1,frecuencia_1,8);
    ledcSetup(2,frecuencia,8);
    ledcAttachPin(stepPin1,0);
    ledcAttachPin(stepPin2,1);
    ledcAttachPin(stepPin3,2);

     
    Serial.begin(115200);
    conectarWiFi();
    SerialBT.begin("ESP32_Bluetooth");
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
// Función para conectar WiFi
void conectarWiFi() {
    Serial.println("Intentando conectar a WiFi...");
    WiFi.begin(ssid, password);
    unsigned long tiempoInicio = millis();
    const unsigned long tiempoEspera = 5000;

    while (WiFi.status() != WL_CONNECTED && millis() - tiempoInicio < tiempoEspera) {

    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConectado a WiFi!");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
        servidor.begin();
    } else {
        Serial.println("\nNo se pudo conectar a WiFi.");
    }
}


// Función para recibir texto desde la página
void recibirTexto() {
    WiFiClient cliente = servidor.available();  // Verifica si hay un cliente conectado
    if (cliente) {  // Si hay un cliente
        String peticion = cliente.readStringUntil('\r');  // Lee la solicitud del cliente
        cliente.flush();  // Vacía el buffer del cliente
        int indiceTexto = peticion.indexOf("/?texto=");  // Busca la parte de la solicitud que contiene el texto
        if (indiceTexto != -1) {
            textoRecibido = peticion.substring(indiceTexto + 8, peticion.indexOf(" ", indiceTexto));
            textoRecibido.trim();  // Elimina espacios innecesarios al principio y al final
            textoRecibido.replace("%20", " ");  // Reemplaza el código de espacio "%20" por un espacio real
            textoRecibido.toLowerCase();  // Convierte todo el texto a minúsculas

            // Convierte cada carácter del texto recibido a su representación en Braille
            for (int i = 0; i < textoRecibido.length(); i++) {
                char letra = textoRecibido[i];  // Obtiene el carácter actual del texto
                if (letra >= 'a' && letra <= 'z') {  // Si el carácter está entre 'a' y 'z'
                    for (int j = 0; j < 6; j++) {
                        matrizTexto[posicionTexto][j] = matrizBraille[letra - 'a'][j];  // Asigna el valor Braille correspondiente
                    }

                    posicionTexto++;  // Incrementa la posición del texto en el vector
                } else if (letra == ' ') {  // Si el carácter es un espacio
                    for (int j = 0; j < 6; j++) {
                        matrizTexto[posicionTexto][j] = matrizBraille[26][j];  // Asigna la representación Braille del espacio
                    }

                    posicionTexto++;  // Incrementa la posición para el próximo carácter
                }
            }
            estadoActual = IMPRESION;  // Cambia el estado para empezar a imprimir el Braille
        }
        cliente.stop();  // Cierra la conexión con el cliente
    }
}


// Función para imprimir el texto en Braille
void imprimirBraille() {
    perforar(); 
    if (textoImpreso) {
        enviarTextoPorBluetooth(); // Envía el texto recibido por Bluetooth
        posicionTexto = 0; // Reinicia la posición del texto
        letraActual = 0; // Reinicia el índice de letra
        textoImpreso = false; // Resetea la marca para la próxima letra
        tiempoAnterior = 0; // Resetea el tiempo para los LEDs
        estadoActual = RECEPCION_TEXTO; // Vuelve al estado de recepción de texto
        cintaFlag = 0;
        
    }
}

// Función para perforar en la cinta
void perforar() {
    tiempoAnterior = 0;
    const unsigned long intervalo = 3000;  // Intervalo de 1 segundo
    unsigned long tiempoActual = millis(); // Obtiene el tiempo actual
        if (letraActual >= posicionTexto) { // Marca que se ha mostrado todo el texto
        textoImpreso = true; 
        moverCinta();
        void espera();
       // cortarCinta(); 
        return;
    }
    cintaFlag = 1;
    moverCinta();
    void espera();
    // Control de la visualización de columnas
    if (tiempoActual - tiempoAnterior >= intervalo) {
        if (!columnaMostrada) { // Si estamos en la primera columna
            // Muestra primera columna 
            // Imprime en el serial la letra y su representación en Braille
           // Serial.print("Imprimiendo letra: ");
           // Serial.print(textoRecibido[letraActual]);
          //  Serial.print(" - Braille: ");
           // Serial.print(matrizTexto[letraActual][0]);
            angulo = 1;
            void espera();
            moverLeva();
            moverServo();
          //  Serial.print(matrizTexto[letraActual][1]);
            angulo = 2;
            void espera();
            moverLeva();
            moverServo();
           // Serial.print(matrizTexto[letraActual][2]);       
            angulo = 3;
            void espera();
            moverLeva();
            moverServo();
            Serial.print(" ");

            columnaMostrada = true; // Cambiamos a la segunda columna
            tiempoAnterior = tiempoActual; // Actualiza el tiempo
        } 
        else { // Segunda columna
            // Mueve la cinta hasta la segunda columna
            cintaFlag = 2;
            void espera();
            moverLeva();
            moverCinta();
          //  Serial.print(matrizTexto[letraActual][3]);
            angulo = 1;
            void espera();
            moverLeva();
            moverServo();
           // Serial.print(matrizTexto[letraActual][4]);
            angulo = 2;
            void espera();
            moverLeva();
            moverServo();
           // Serial.print(matrizTexto[letraActual][5]);
            angulo = 3;
            void espera();
            moverLeva();
            moverServo();
           // Serial.println(" ");
            tiempoAnterior = tiempoActual;
            letraActual++; // Mueve a la siguiente letra
            columnaMostrada = false; // Vuelve a la primera columna
        }
    }

}

// Función para mover la cinta
void moverCinta() {
    digitalWrite(enable1, LOW);
    digitalWrite(enable2, LOW);
    const unsigned long tiempoMovCinta1 = 300;  // Tiempo para cintaFlag == 1 
    const unsigned long tiempoMovCinta2 = 150;  // Tiempo para cintaFlag == 2 

    unsigned long tiempoInicioCinta = 0;

    if (cintaFlag == 1) {  // Si cintaFlag es 1, mover durante menos tiempo
    tiempoInicioCinta = millis();
        while (millis() - tiempoInicioCinta < tiempoMovCinta1) {
            // Mientras no se haya alcanzado el tiempo de movimiento, mantener el motor encendido
            ledcWrite(0, 127);  // Motor en funcionamiento
            digitalWrite(dirPin1, HIGH);  // Dirección hacia adelante
            Serial.print("motor cinta1");
        }

        // Detener el motor después de que pase el tiempo especificado
        ledcWrite(0, 0);  // Detiene el motor
        digitalWrite(enable1, HIGH);  // Detiene la dirección
        Serial.print("detener motor cinta1");
    } else if (cintaFlag == 2) {  // Si cintaFlag es 2, mover durante más tiempo
        tiempoInicioCinta = millis();
        while (millis() - tiempoInicioCinta < tiempoMovCinta2) {
            // Mientras no se haya alcanzado el tiempo de movimiento, mantener el motor encendido
            ledcWrite(0, 127);  // Motor en funcionamiento
            digitalWrite(dirPin1, HIGH);  // Dirección hacia adelante
            Serial.print("motor cinta2");
        }

        // Detener el motor después de que pase el tiempo especificado
        ledcWrite(0, 0);  // Detiene el motor
        digitalWrite(enable1, HIGH);  // Detiene la dirección
        Serial.print("detener motor cinta2");
    }
}


void moverLeva() {
    unsigned long tiempoInicioLeva = 0;
    unsigned long tiempoMovLeva = 300; // Tiempo de movimiento para la leva
    tiempoInicioLeva = millis();  // Inicia el temporizador cuando la leva comienza a moverse
digitalWrite(enable2, LOW);
    while (millis() - tiempoInicioLeva < tiempoMovLeva){
        ledcWrite(1, 127);  // Motor en funcionamiento
        digitalWrite(dirPin2, HIGH);  // Dirección hacia adelante
        Serial.println("mover leva");
    }
    
    ledcWrite(1, 0);  // Detiene el motor
    digitalWrite(enable2, HIGH);  // Detiene la dirección
    Serial.println("Leva detenida");

}

void moverServo() {
  unsigned long tiempoActualServo = millis();  

  // Comprobamos qué ángulo activar
  if (angulo == 1) {
    duracionPulsoServo = map(anguloServo[0], 0, 180, 600, 2400); 
    Serial.println("Ángulo 1");
  } 
  else if (angulo == 2) {
    duracionPulsoServo = map(anguloServo[1], 0, 180, 600, 2400); 
    Serial.println("Ángulo 2");
  } 
  else if (angulo == 3) {
    duracionPulsoServo = map(anguloServo[2], 0, 180, 600, 2400); 
    Serial.println("Ángulo 3");
  }

  // Si se ha detectado un cambio de ángulo (y se ha asignado una duración de pulso)
  if (duracionPulsoServo > 0 && !enviandoPulsoServo) {
    inicioPulsoServo = micros();  
    digitalWrite(pinServoControl, HIGH);  // Inicia el pulso
    Serial.println("servo");
    enviandoPulsoServo = true;
  }

  // Verifica si el pulso ha pasado el tiempo de duración
  if (enviandoPulsoServo && (micros() - inicioPulsoServo >= duracionPulsoServo)) {
    digitalWrite(pinServoControl, LOW);  // Apaga el pulso después de la duración
    enviandoPulsoServo = false;  // Finaliza el envío de pulso
    Serial.println("Pulso apagado");
  }
}

// Función para cortar la cinta
void cortarCinta() {
    digitalWrite(enable3, LOW);
     unsigned long tiempoCorte1 = 0;  // Variable global para el tiempo de corte
     unsigned long tiempoMovCorte1 = 1000;
    tiempoCorte1 = millis();
    unsigned long tiempoCorte2 = 0;  // Variable global para el tiempo de corte
    unsigned long tiempoMovCorte2 = 1000;
    bool direccion = 0;  // Dirección de los motores
    while (millis() - tiempoCorte1 < tiempoMovCorte1) {  
    digitalWrite(dirPin3, direccion); 
    ledcWrite(2, 127);  // Enciende el motor con una velocidad de 127 (ajustable)
    }
    ledcWrite(2, 0);
    delay(50);
    direccion = !direccion;
    tiempoCorte2 = millis();
    while (millis() - tiempoCorte2 < tiempoMovCorte2) {  
      // Cambia la dirección cada vez;
    digitalWrite(dirPin3, direccion);
    }
    digitalWrite(enable3, HIGH);

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
void espera(){
unsigned long flagespera1 = 0;  // Variable global para el tiempo de corte
unsigned long flagespera2 = 2000;
flagespera1 = millis();
while (millis() - flagespera1 < flagespera2) {  

}
}