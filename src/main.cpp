#include <WiFi.h>
#include <Servo.h>

enum Estado {
  CONECTAR_WIFI,
  ESPERAR_CLIENTE,
  PROCESAR_PETICION,
  MOVER_SERVO,
  PERFORAR_BRAILLE
};

Estado estado = CONECTAR_WIFI;

const char* wifiNombre = "WIFI1";
const char* wifiClave = "del.sel1";

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
    {1, 0, 1, 0, 1, 1}  // z
};

WiFiServer server(80);
WiFiClient client;
String request = "";
String texto = "";
int letra = 0;

void enviarPagina(WiFiClient cliente) {
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
  Serial.println("Conectando a WiFi...");
  WiFi.begin(wifiNombre, wifiClave);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado a WiFi. IP: " + WiFi.localIP().toString());
    server.begin();
    estado = ESPERAR_CLIENTE;
  } else {
    Serial.println("Conexión fallida, reintentando...");
  }
}

void moverServo(float mm) {
    int angulo = map(mm, 0, 9, 0, 180);
    servo.write(angulo);
    delay(500);
}

void perforar() {
    digitalWrite(pinMotor, HIGH);
    delay(500);
    digitalWrite(pinMotor, LOW);
    delay(500);
}

void escribirBraille(char letra) {
  int idx = letra - 'a';
  for (int i = 0; i < 6; i++) {
    if (braille[idx][i] == 1) {
      perforar();
    }
    moverServo(2 * (i + 1));
  }
}

void esperarCliente() {
  client = server.available();
  if (client) {
    Serial.println("Cliente conectado");
    estado = PROCESAR_PETICION;
  }
}

void procesarPeticion() {
  if (client.connected()) {
    while (client.available()) {
      char c = client.read();
      request += c;

      if (c == '\n') {
        Serial.println("Petición recibida:");
        Serial.println(request);

        int idx = request.indexOf("/?texto=");
        if (idx != -1) {
          texto = request.substring(idx + 8, request.indexOf(" ", idx + 8));
          texto.trim();
          enviarPagina(client);
          estado = MOVER_SERVO;
          letra = 0;
        }
        client.stop();
        break;
      }
    }
  }
}

void moverBraille() {
  if (letra < texto.length()) {
    escribirBraille(texto[letra]);
    letra++;
    moverServo(9);
  } else {
    estado = ESPERAR_CLIENTE;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pinMotor, OUTPUT);
  digitalWrite(pinMotor, LOW);
  servo.attach(pinServo);
  moverServo(0);
  conectarWiFi();
}

void loop() {
  switch (estado) {
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
