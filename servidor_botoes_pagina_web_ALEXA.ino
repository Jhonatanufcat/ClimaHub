#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

// Definição dos pinos
#define BUTTON_PIN_0 D5   // Pino do botão power
#define BUTTON_PIN D0     // Pino do botão velocidade
#define LED_PIN_1 D3      // Pino do primeiro LED
#define LED_PIN_2 D4      // Pino do segundo LED
#define LED_PIN_3 D8      // Pino do terceiro LED

// Variáveis de estado
bool button0State = false;
bool buttonState = false;
bool ledState1 = false;
bool ledState2 = false;
bool ledState3 = false;

// Variável para armazenar a temperatura recebida
float temperatura;
float umidade;
String temperaturaStr = "0,0";
String umidadeStr = "0,0";

// Porta do servidor socket
const uint16_t port = 12345;
WiFiServer socket(port);

// Objeto para o servidor web
ESP8266WebServer server(80);

// Definições WiFi para o ponto de acesso (AP)
const char* ap_ssid = "CLIMAHUB";
const char* ap_password = "12345678";

// Definições WiFi para a rede do cliente
const char* client_ssid = "AUTOMACAO";
const char* client_password = "computacao";

// Sinric Pro credentials
const char* APP_KEY = "0939ee59-bb44-4205-bcd4-131c466ab33a";
const char* APP_SECRET = "714fd5fc-5d4f-4df1-9ec2-9663e15437da-1687dc8f-bc97-4d7a-a00d-c64c251b7058";
const char* SWITCH_ID = "66742514888aa7f7a23a8c1e";

SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];

// Protótipos das funções de tratamento
void handleRoot();
void handleToggleSequence();
void handleTogglePower();
void handleWiFi();
void handleConnect();
void handleStatus();
void sendCommandToClient(const char* command);
bool connectToWiFi(const char* ssid, const char* password);

// Callback function for Sinric Pro
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s power turned %s\r\n", deviceId.c_str(), state ? "ON" : "OFF");

if (deviceId == SWITCH_ID) {
    if (state) {
      digitalWrite(LED_PIN_2, HIGH);
      ledState2 = true;
      sendCommandToClient("acionar");
    } else {
      digitalWrite(LED_PIN_1, LOW);
      digitalWrite(LED_PIN_2, LOW);
      digitalWrite(LED_PIN_3, LOW);
      ledState1 = false;
      ledState2 = false;
      ledState3 = false;
      sendCommandToClient("parar");
    }
  }
  return true;
}
// Configuração inicial
void setup() {
  Serial.begin(115200);

  // Configurar como ponto de acesso
  WiFi.softAP(ap_ssid, ap_password);

  // Obter o endereço IP do ponto de acesso
  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP Address (AP): ");
  Serial.println(IP);

  // Conectar à rede WiFi do cliente
  if (connectToWiFi(client_ssid, client_password)) {
    Serial.println("Connected to WiFi network");
  } else {
    Serial.println("Failed to connect to WiFi network");
  }

  // Iniciar o servidor socket
  socket.begin();
  Serial.println("Servidor socket iniciado!");

  pinMode(BUTTON_PIN_0, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);

  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);

  // Iniciar o servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/toggleSequence", HTTP_GET, handleToggleSequence);
  server.on("/togglePower", HTTP_GET, handleTogglePower);
  server.on("/wifi", HTTP_GET, handleWiFi);
  server.on("/connect", HTTP_POST, handleConnect);
  server.on("/status", HTTP_GET, handleStatus);
  server.begin();
  Serial.println("Servidor web iniciado!");

  // Sinric Pro setup
  mySwitch.onPowerState(onPowerState);
  SinricPro.onConnected([]() {
    Serial.println("Connected to SinricPro");
  });
  SinricPro.onDisconnected([]() {
    Serial.println("Disconnected from SinricPro");
  });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// Loop principal
void loop() {
  // Verificar o estado do botão
  bool newState0 = digitalRead(BUTTON_PIN_0);
  if (newState0 != button0State) {
    delay(50); // debounce
    if (newState0 == HIGH) {
      button0Pressed();
    }
    button0State = newState0;
  }

  // Verificar o estado do botão
  bool newState = digitalRead(BUTTON_PIN);
  if (newState != buttonState) {
    delay(50); // debounce
    if (newState == HIGH) {
      buttonPressed();
    }
    buttonState = newState;
  }

  WiFiClient client = socket.available();
  if (client) {
    Serial.println("Cliente conectado!");
    while (client.connected()) {
      if (client.available()) {
        temperatura = client.parseFloat();
        umidade = client.parseFloat();
        temperaturaStr = String(temperatura, 1);
        umidadeStr = String(umidade, 1);
        if (temperatura != 0.0 && umidade != 0.0) {
          Serial.print("Temperatura recebida: ");
          Serial.println(temperatura);
          Serial.print("Umidade recebida: ");
          Serial.println(umidade);
          client.stop();
          Serial.println("Cliente desconectado");
        } else {
          Serial.println("Erro ao ler dados do cliente.");
        }
      }
    }
  }

  // Lidar com as solicitações do servidor web
  server.handleClient();
  SinricPro.handle();
}

// Função para conectar-se a uma rede WiFi
bool connectToWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  for (int i = 0; i < 10; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" connected");
      return true;
    }
    delay(1000);
    Serial.print(".");
  }
  Serial.println(" failed");
  return false;
}

// Função chamada quando o botão é pressionado
void buttonPressed() {
  Serial.print("botão VELOCIDADE pressionado");
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);

  if (!ledState1 && !ledState2 && !ledState3) {
  } else if (ledState1 && !ledState2 && !ledState3) {
    digitalWrite(LED_PIN_2, HIGH);
    ledState2 = true;
    ledState1 = false;
  } else if (!ledState1 && ledState2 && !ledState3) {
    digitalWrite(LED_PIN_3, HIGH);
    ledState3 = true;
    ledState2 = false;
  } else if (!ledState1 && !ledState2 && ledState3) {
    digitalWrite(LED_PIN_1, HIGH);
    ledState3 = false;
    ledState1 = true;
  }
}

// Função chamada quando o botão 0 é pressionado
void button0Pressed() {
  Serial.print("botão POWER pressionado");
  if (!ledState1 && !ledState2 && !ledState3) {
    digitalWrite(LED_PIN_2, HIGH);
    ledState2 = true;
    sendCommandToClient("acionar");
  } else {
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(LED_PIN_3, LOW);
    ledState1 = false;
    ledState2 = false;
    ledState3 = false;
    enviarComandoReiniciar();
    //sendCommandToClient("parar");
  }
}

void enviarComandoReiniciar() {
  WiFiClient client;
  IPAddress clientIP(192, 168, 4, 2);
  if (client.connect(clientIP, port)) {
    Serial.println("Conectado ao servidor!");
  
 // if (client && client.connected()) {
    client.println("RESTART");
    Serial.println("Comando de reinicialização enviado.");
  } else {
    Serial.println("Nenhum cliente conectado ou conexão perdida.");
  }
}
// Função para enviar comandos ao cliente via socket
void sendCommandToClient(const char* command) {
  WiFiClient client;
  IPAddress clientIP(192, 168, 4, 2);
  if (client.connect(clientIP, port)) {
    Serial.println("Conectado ao servidor!");
    client.println(command);
    Serial.print("Comando enviado ao cliente: ");
    Serial.println(command);
    Serial.println("Desconectado ao servidor!");
  } else {
    Serial.println("Falha ao conectar ao cliente!");
  }
}

// Manipulador para a raiz ("/")
void handleRoot() {
  String html = "<html><head><title>UMIDIFICADOR CONTROL</title>";
  html += "<style>h1 { text-align: center; }</style>";
  html += "<script>function toggleSequence() {";
  html += "var xhr = new XMLHttpRequest();";
  html += "xhr.onreadystatechange = function() {";
  html += "if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "var response = JSON.parse(xhr.responseText);";
  html += "document.getElementById('led1').innerHTML = response.led1;";
  html += "document.getElementById('led2').innerHTML = response.led2;";
  html += "document.getElementById('led3').innerHTML = response.led3;";
  html += "}};";
  html += "xhr.open('GET', '/toggleSequence', true);";
  html += "xhr.send();";
  html += "} function updateStatus() {";
  html += "var xhr = new XMLHttpRequest();";
  html += "xhr.onreadystatechange = function() {";
  html += "if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "var response = JSON.parse(xhr.responseText);";
  html += "document.getElementById('led1').innerHTML = response.led1;";
  html += "document.getElementById('led2').innerHTML = response.led2;";
  html += "document.getElementById('led3').innerHTML = response.led3;";
  html += "}};";
  html += "xhr.open('GET', '/status', true);";
  html += "xhr.send();";
  html += "} setInterval(updateStatus, 1000);";
  html += "function togglePower() {";
  html += "var xhr = new XMLHttpRequest();";
  html += "xhr.open('GET', '/togglePower', true);";
  html += "xhr.send();";
  html += "}</script></head><body>";
  html += "<h1>CLIMAHUB CONTROL</h1>";
  html += "<h2>Motor Control :</h2>";
  html += "<p>Low: <span id='led1'>OFF</span></p>";
  html += "<p>Average: <span id='led2'>OFF</span></p>";
  html += "<p>High: <span id='led3'>OFF</span></p>";
  html += "<button onclick='togglePower()'>Power</button>";
  html += "<button onclick='toggleSequence()'>Motor sequence</button>";
  html += "<h2>Weather data: </h2>";
  html += "<p>Temperature: " + temperaturaStr + " C</p>";
  html += "<p>Air Humidity: " + umidadeStr + " %</p>";
  html += "<h2>Connected to WIFI: </h2>";
  html += "<p>" + String(client_ssid) + "</p>";
  html += "<h2>Connect to WiFi:</h2>";
  html += "<form action='/connect' method='post'>";
  html += "SSID: <input type='text' name='ssid'><br>";
  html += "Senha: <input type='password' name='password'><br>";
  html += "<button type='submit'>Conectar</button>";
  html += "</form>";
  html += "<form action='/wifi' method='get'>";
  html += "<button type='submit'>Ver Redes Wi-Fi</button>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Manipulador para alternar a sequência de LEDs
void handleToggleSequence() {
  buttonPressed();
  String status = "{\"led1\": \"" + String(ledState1 ? "ONN" : "OFF") + "\", ";
  status += "\"led2\": \"" + String(ledState2 ? "ONN" : "OFF") + "\", ";
  status += "\"led3\": \"" + String(ledState3 ? "ONN" : "OFF") + "\"}";
  server.send(200, "application/json", status);
}

// Manipulador para alternar o botão Power
void handleTogglePower() {
  button0Pressed();
  String status = "{\"power\": \"" + String(button0State ? "ONN" : "OFF") + "\"}";
  server.send(200, "application/json", status);
}

// Manipulador para listar redes Wi-Fi disponíveis
void handleWiFi() {
  String ssidList = "";
  int numNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numNetworks; i++) {
    ssidList += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + "</option>";
  }
  String html = "<html><head><title>Available Wi-Fi Networks</title></head><body>";
  html += "<h2>Available Wi-Fi Networks:</h2>";
  html += "<form action='/connect' method='post'>";
  html += "SSID: <select name='ssid'>" + ssidList + "</select><br>";
  html += "Senha: <input type='password' name='password'><br>";
  html += "<button type='submit'>Conectar</button>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

// Manipulador para se conectar a uma rede Wi-Fi selecionada
void handleConnect() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  if (connectToWiFi(ssid.c_str(), password.c_str())) {
    server.sendHeader("Location", "/");
    server.send(303);
  } else {
    server.send(200, "text/plain", "Erro ao conectar-se à rede Wi-Fi.");
  }
}

// Manipulador para obter o status dos LEDs
void handleStatus() {
  String status = "{\"led1\": \"" + String(ledState1 ? "ONN" : "OFF") + "\", ";
  status += "\"led2\": \"" + String(ledState2 ? "ONN" : "OFF") + "\", ";
  status += "\"led3\": \"" + String(ledState3 ? "ONN" : "OFF") + "\"}";
  server.send(200, "application/json", status);
}
