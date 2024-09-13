#include <Servo.h>  // Librería para controlar el servo

// Pines
const int pinServo = 18;  // Pin donde está conectado el servo
const int pinMotor = 19;  // Pin donde está conectado el motor NEMA

// Servo
Servo servo;

// Mapeo de letras a braille (6 puntos por letra, 1 = perforado, 0 = no perforado)
// Cada fila representa una letra del alfabeto (a, b, c, etc.)
const int mapaBraille[26][6] = {
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
    // Continúa con el resto de letras según el alfabeto braille
};

// Variables generales
const int numPuntos = 6;  // Número de puntos en cada bloque braille (2 columnas x 3 filas)
const int pasoHorizontal = 2;  // Paso en mm entre cada punto horizontalmente en la cinta

// Función para mover el servo a una posición en mm
// El servo se mueve entre 0 mm y 9 mm, lo mapeamos de 0 a 180 grados
void moverServoA(float posicion_mm) {
    int posicionServo = map(posicion_mm, 0, 9, 0, 180);  // Mapea mm a grados del servo
    servo.write(posicionServo);  // Mueve el servo a la posición calculada
    delay(500);  // Espera para que el servo termine de moverse
}

// Función para accionar el motor NEMA y perforar un punto
// Esta función simula la bajada y subida del electrodo para perforar
void perforarPunto() {
    digitalWrite(pinMotor, HIGH);  // Activa el motor (baja el electrodo para perforar)
    delay(500);  // Espera para que se realice la perforación
    digitalWrite(pinMotor, LOW);   // Desactiva el motor (sube el electrodo)
    delay(500);  // Espera antes del siguiente movimiento
}

// Función para escribir una letra en braille
// Esta función toma una letra y perfora los puntos correspondientes en la cinta
void escribirLetraBraille(char letra) {
    // Convertimos la letra a su índice en el alfabeto (a = 0, b = 1, ..., z = 25)
    int indice = letra - 'a';
    
    // Recorre los 6 puntos del bloque de la letra en braille (2 columnas y 3 filas)
    for (int i = 0; i < numPuntos; i++) {
        // Si el valor en la posición es 1, perfora el punto
        if (mapaBraille[indice][i] == 1) {
            perforarPunto();  // Llama a la función para perforar
        }
        // Mueve el servo al siguiente punto horizontal
        moverServoA(pasoHorizontal * (i + 1));  // Se mueve 2 mm por cada punto
    }
}

// Función principal que se ejecuta una sola vez al inicio
void setup() {
    // Configuramos el pin del motor como salida
    pinMode(pinMotor, OUTPUT);
    digitalWrite(pinMotor, LOW);  // Asegura que el motor esté apagado al inicio

    // Iniciamos el servo y lo configuramos en el pin correspondiente
    servo.attach(pinServo);
    moverServoA(0);  // Inicializamos el servo en la posición 0 mm

    // Escribimos una palabra en braille
    String palabra = "hola";  // Palabra que queremos escribir en braille
    for (int i = 0; i < palabra.length(); i++) {
        escribirLetraBraille(palabra[i]);  // Escribimos cada letra de la palabra
        moverServoA(9);  // Mueve el servo al siguiente bloque de letras (9 mm de separación)
        delay(1000);  // Pausa de 1 segundo entre letras
    }
}

// Bucle principal, no se necesita en este caso porque la escritura ocurre en `setup`
void loop() {
    // El código en `setup` se ejecuta una sola vez
}
