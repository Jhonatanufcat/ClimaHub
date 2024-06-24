#include <AccelStepper.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

// Definições WiFi
const char* ssid = "CLIMAHUB";
const char* password = "12345678";

//DEFINIÇÃO DE IP FIXO PARA O NODEMCU
IPAddress ip(192,168,4,2); //IP na mesma classe que o EspServer
IPAddress gateway(192,168,4,1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255,255,255,0); //MASCARA DE REDE

// IP e porta do servidor esp2
const char* serverIP = "192.168.4.1"; // IP do esp2 na rede WiFi
const uint16_t port = 12345; // Porta do servidor socket no esp2


#define motorPin1 D6
#define motorPin2 D7
#define motorPin3 D3
#define motorPin4 D4
#define endstopPin D5
#define FULL_STEP 4
void TempInicial();

AccelStepper stepper(FULL_STEP, motorPin1, motorPin3, motorPin2, motorPin4);
Adafruit_BME280 bme;


unsigned long previousMillis = 0; // Armazena a última vez que a temperatura foi enviada
const long interval = 240000; // Intervalo de 1 minuto (em milissegundos)
bool motorRunning = false;
String request;
void moveStepper();
String motor;

WiFiServer server(port); // Inicializa o servidor na porta especificada

void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1); // D1 (SCL), D2 (SDA) - definir os pinos corretos
  pinMode(endstopPin, INPUT_PULLUP);
   
  stepper.setMaxSpeed(250);
  stepper.setAcceleration(150);

  while (digitalRead(endstopPin) == HIGH) {
    stepper.setSpeed(250);
    stepper.runSpeed();
  }
  stepper.stop();
  stepper.setSpeed(250);
  stepper.runToNewPosition(stepper.currentPosition() - 100);
  Serial.println("posição correta!");

  // Conectar-se ao WiFi
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.print("Conectado a rede sem fio "); //ESCREVE O TEXTO NA SERIAL
  Serial.println(ssid); //ESCREVE O NOME DA REDE NA SERIAL

  // Iniciar comunicação serial para debug
  Serial.print("IP CLIMAHUB_1: ");
  Serial.println(WiFi.localIP());

   // Iniciar o servidor socket
  server.begin();
  Serial.println("Servidor socket iniciado!");


  if (!bme.begin(0x76)) {
    Serial.println(F("Não foi possível encontrar o sensor BMP280. Verifique as conexões!"));
    while (1);
  }
  TempInicial();
  

  
}


void loop() {
  unsigned long currentMillis = millis();
   // Verifica se é hora de enviar os dados
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    TempInicial();
 }

  WiFiClient client = server.available(); // Verifica se há um cliente conectado
  if (client) {
    Serial.println("Cliente conectado");
    client.connected();
      String request = client.readStringUntil('\n');
      request.trim(); // Remove espaços em branco e quebras de linha
      Serial.print("Comando recebido: ");
      Serial.println(request);

      if (request.equals("RESTART")) {
        Serial.println("Reiniciando ESP8266...");
        ESP.restart();
      } else if (request.equals("acionar")) {
        motorRunning = true;
        Serial.println("Motor acionado!");
        client.println("Motor acionado!");
      } else if (request.equals("parar")) {
        motorRunning = false;
        //ESP.restart();
        Serial.println("Motor parado!");
        client.println("Motor parado!");
      } else {
        Serial.println("Comando inválido!");
        client.println("Comando inválido!");
      }
    

    client.stop();
    Serial.println("Cliente desconectado");
  }

  if (motorRunning) {
    moveStepper();
  } else {
    stepper.stop();
    //ESP.restart();
  }
}
void moveStepper() {
  // Girar o motor no sentido anti-horário por meia volta
  stepper.setSpeed(400);
  stepper.runSpeed(); // Mantém o motor em movimento contínuo
  stepper.runToNewPosition(stepper.currentPosition() - 1070);
  
  // Girar o motor no sentido horário por uma volta e meia
  stepper.setSpeed(400);
  stepper.runToNewPosition(stepper.currentPosition() + 1070);
  
  
  delay(100); // Adicione um pequeno atraso para evitar que o loop execute muito rapidamente
}

void TempInicial() {
if (WiFi.status() == WL_CONNECTED) {
      // Ler temperatura do sensor BME280
      float temperatura = bme.readTemperature();
      float umidade = bme.readHumidity();

    // Conectar ao servidor esp2 via socket
      WiFiClient client;
      if (client.connect(serverIP, port)) {
        Serial.println("Conectado ao servidor!");

// Enviar dados de temperatura ao servidor
// Enviar temperatura
  Serial.print("Enviando Temperatura: ");
  Serial.println(temperatura);
  client.print(temperatura);
  client.print('\n'); // Enviar nova linha para indicar o fim da mensagem
  
  delay(100); // Aguardar um breve intervalo antes de enviar a próxima informação
  
  // Enviar umidade
  Serial.print("Enviando Umidade: ");
  Serial.println(umidade);
  client.print(umidade);
  client.print('\n'); // Enviar nova linha para indicar o fim da mensagem

        client.stop();
        Serial.println("Conexão fechada");
      } else {
        Serial.println("Falha ao conectar ao servidor!");
      }
    } else {
      Serial.println("WiFi desconectado. Tentando reconectar...");
      WiFi.begin(ssid, password);
    }

}

