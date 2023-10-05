#include <Arduino.h>
#line 1 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MPU6050.h>

MPU6050 mpu;

const int threshold = 6000;                                                                     // limite da amplitude para ativação do alerta
const unsigned long intervalMPU = 100;                                                          // intervalo de leitura do MPU
unsigned long previousMillisMPU = 0;                                                            // variável de controle de tempo de leitura do MPU
int sda_pin = D5;                                                                               // porta SDA do MPU-6050
int scl_pin = D6;                                                                               // porta SCL do MPU-6050
int MPU_addr = 0x68;                                                                            // endereço I2C do sensor MPU-6050
float bateria = A0;                                                                             // porta da bateria
int buzzer = D3;                                                                                // porta do buzzer ativo
int botao = D2;                                                                                 // porta do botão
int led = 2;                                                                                    // porta do led built in
bool botaoState = false;                                                                        // estado inicial do botao
bool oldBotaoState = false;                                                                     // salva estado do botao
int amp;                                                                                        // variável com valores calculados dos eixos do acelerômetro
int angle;                                                                                      // variável com valores calculados dos eixos do giroscópio
float Raw_Amp;                                                                                  // dados brutos do acelerômetro
float Raw_Angle;                                                                                // dados brutos do giroscópio
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;                                                      // variáveis dos sensores do MPU-6050
float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;                                           // valores iniciais das variáveis dos sensores
boolean queda = false;                                                                          // armazena o valor quando uma 2queda ocorre
boolean gatilho1 = false;                                                                       // armazena valor quando o primeiro trigger (threshold mínimo) é ativado
boolean gatilho2 = false;                                                                       // armazena valor quando o segundo trigger (threshold máximo) é ativado
boolean gatilho3 = false;                                                                       // armazena valor quando o terceiro trigger (mudança de orientação) é ativado
byte gatilho1Count = 0;                                                                         // armazena contagem desde a ativação do primeiro trigger
byte gatilho2Count = 0;                                                                         // armazena contagem desde a ativação do segundo trigger
byte gatilho3Count = 0;                                                                         // armazena contagem desde a ativação do terceiro trigger
int mudancaAngulo = 0;                                                                          // armazena valor quando acontece uma mudança de angulo no sensor
int envioAlertaCount = 0;                                                                       // variável para controlar quantas vezes o alerta foi enviado
unsigned long now;                                                                              // armazena o tempo atual
unsigned long alarmStartTime = 0;                                                               // registra o início do alarme
unsigned long previousMillisBuzzer = 0;                                                         // armazena o tempo anterior de leitura do buzzer
unsigned long previousMillisSendEvent = 0;                                                      // armazena o tempo anterior de envio de evento
unsigned long previousMillisLed = 0;                                                            // armazena o tempo anterior de leitura do led
unsigned long previousMakePOSTTime = 0;                                                         // armazena o tempo anterior de envio da solicitação POST
unsigned long tempoDecorrido = millis() / 1000;                                                 // tempo de execução do código em segundos
unsigned long previousMillisBattery = 0;                                                        // armazena o tempo anterior de leitura da bateria
unsigned long tempoInicial;                                                                     // armazena tempo inicial para controle de tempo do dispositivo ligado
const unsigned long intervaloBateria = 10000;                                                   // intervalo de leitura tempo da bateria (1 segundo)
const unsigned long intervaloSendEvent = 50;                                                    // intervalo de leitura tempo de envio de evento (0,05 segundos)
const unsigned long tempoAlertaAtivo = 3000;                                                    // duração do alerta
const unsigned long intervaloBuzzer = 200;                                                      // intervalo de leitura tempo do buzzer (0,2 segundos)
const unsigned long intervaloLed = 100;                                                         // intervalo de leitura tempo do led (0,05 segundos)
const unsigned long intervalMakePOST = 200;                                                     // intervalo de leitura tempo do POST (0,1)
bool buzzerState = false;                                                                       // estado atual do buzzer
bool ledState = false;                                                                          // estado inicial do led builtin
bool estadoBotaoEmergencia = false;                                                             // armazena estado do botão de socorro para alertas enviados à página web
bool alarmeAtivo = false;                                                                       // controla o acionamento do alarme
float batteryVoltage;                                                                           // recebe voltagem da bateria (ref. 3.3v)
int batteryLevel;                                                                               // recebe nível de carga da bateria (0-100)
float batteryRaw;                                                                               // recebe dados brutos da bateria
bool stateAlert = false;                                                                        // variável de controle para enviar alertas através do botão pressionado
const char* host = "maker.ifttt.com";                                                           // URL IFTTT
const char* connectedJSHost;                                                                    // recebe IP do host NODEJS conectado
int erroServidorNODEQtdMsg = 0;                                                                 // quantidade de vezes que mensagem de erro de conexão ao NODE foi exibida
const char* nodeMCUIP;                                                                          // armazena IP do NodeMCU
const char* privateKey = "nskX8XdRbVPV5_4a-efXKH4x5SOohAouhmYGmrZ8AAw";                         // chave privada IFTTT
const char* ssid[] = { "AMIGO_JESSICA_2.4G", "Imembuy", "$wifi" };                                // SSIDs das redes WiFi
const char* pass[] = { "andromeda", "Imembuy#7762", "*#master#*" };                               // senhas das redes WiFi
const char* nodeJSHost[] = { "192.168.1.16", "192.168.10.15", "192.168.43.170", "192.168.1.13" }; // hosts NodeJS
int tempoLigado;                                                                                // armazena tempo em que o dispositivo se manteve ligado
const int dotDuration = 50;                                                                     // Tempo de duração de ponto
const int dashDuration = 150;                                                                   // Tempo de duração de traço
unsigned long previousMillis = 0;                                                               // Variável para controle de tempo
unsigned long interval = 0;                                                                     // Variável para controle de tempo
unsigned long buttonPressedTime = 0;                                                            // Controla tempo total que botão foi pressionado
int morseIndex = 0;                                                                             // Índice para percorrer a mensagem SOS em morse
const char* morseMessage = "... --- ...";                                                       // Define mensagem em morse para SOS
bool alertaEnviado = false;                                                                     // Controla quando o evento é enviado por SMS
double voltagemBateria = batteryVoltage;
// Controle de estados do botão
enum ButtonState
{
  BUTTON_IDLE,
  BUTTON_PRESSED,
  BUTTON_WAIT_RELEASE
};

ButtonState buttonState = BUTTON_IDLE;

// Construção do objeto JSON para armazenar dados dos sensores
StaticJsonDocument < 300 > jsonDocument;

JsonObject object = jsonDocument.to<JsonObject>();
JsonObject data = object.createNestedObject("data");

JsonObject accel = data.createNestedObject("accel");
JsonObject temp = data.createNestedObject("temp");
JsonObject gyro = data.createNestedObject("gyro");
JsonObject battery = data.createNestedObject("battery");
JsonObject infoDev = data.createNestedObject("infoDev");

JsonObject accelX = accel.createNestedObject("accelX");
JsonObject accelY = accel.createNestedObject("accelY");
JsonObject accelZ = accel.createNestedObject("accelZ");

JsonObject gyroX = gyro.createNestedObject("gyroX");
JsonObject gyroY = gyro.createNestedObject("gyroY");
JsonObject gyroZ = gyro.createNestedObject("gyroZ");

JsonObject ampData = infoDev.createNestedObject("ampData");
JsonObject botaoEmergencia = infoDev.createNestedObject("botaoEmergencia");
JsonObject quedaDetectada = infoDev.createNestedObject("quedaDetectada");
JsonObject nodeIP = infoDev.createNestedObject("nodeIP");
JsonObject angleData = infoDev.createNestedObject("angleData");

JsonObject battLevel = battery.createNestedObject("battLevel");
JsonObject battVoltage = battery.createNestedObject("battVoltage");

void send_event(const char*event);

WiFiClient client;

#line 119 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void setup();
#line 144 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void loop();
#line 341 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void conectaWifi();
#line 384 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void mpu_read();
#line 448 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void buzzer_on();
#line 472 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void buzzer_off();
#line 479 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void led_on();
#line 485 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void led_off();
#line 491 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void populateJSON();
#line 516 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void makePOST();
#line 552 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void enviaAlerta();
#line 559 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void batteryLevelRead(int rawLevel);
#line 579 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void ledHostConnect();
#line 602 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void conectaNode();
#line 629 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void morseAlert();
#line 663 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void mpuRead();
#line 119 "J:\\Meu Drive\\#IFFAR\\2023_1-2_TCC_I-II\\3_Desenvolvimento TCC\\TCC2\\nodemcu-client-arduino\\nodemcu-client-arduino.ino"
void setup()
{
  previousMillis = millis();
  interval = dotDuration;
  Serial.begin(115200);
  Wire.begin(sda_pin, scl_pin);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // Registrador PWR_MGMT_1
  Wire.write(0);    // inicializa MPU-6050
  Wire.endTransmission(true);
  tempoInicial = millis(); // inicializa variável com o tempo de execução do dispositivo
  mpu.initialize();
  mpu.setDLPFMode(2);
  mpu.setFullScaleAccelRange(2);

  // realiza conexões com rede wifi e com o servidor nodejs
  conectaWifi();
  conectaNode();

  // Configuração do led e buzzer e liga led built-in
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(bateria, INPUT);
}

void loop()
{
  // Serial.println("Início do loop");
  unsigned long mpuNow = millis();
  if (mpuNow - previousMillisMPU >= intervalMPU)
  {
    // Serial.println("Leitura do MPU");
    previousMillisMPU = mpuNow;
    mpuRead();
  }

  // Calcula tempo decorrido desde a incialização do dispositivo
  unsigned long tempoAtual = millis();
  unsigned long tempoDecorrido = (tempoAtual - tempoInicial) / 1000;
  tempoLigado = tempoDecorrido;

  // Leitura do botão
  botaoState = digitalRead(botao);

  // Recebe dados brutos da bateria
  batteryRaw = analogRead(bateria);

  unsigned long batteryNow = millis(); // controle de leitura do nível da bateria
  if (batteryNow - previousMillisBattery >= intervaloBateria)
  {
    // Serial.println("Leitura da bateria");
    previousMillisBattery = batteryNow;
    batteryLevelRead(batteryRaw);
  }

  // Calibra eixos dos sensores
  // Serial.println("Calibração dos sensores");
  mpu_read();
  ax = (AcX + 14000) / 16384.00;
  ay = (AcY - 77) / 16384.00;
  az = (AcZ - 1947) / 16384.00;
  gx = (GyX + 270) / 131.07;
  gy = (GyY - 351) / 131.07;
  gz = (GyZ + 136) / 131.07;

  // Calcula vetor de amplitude dos 6 eixos
  Raw_Amp = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
  Raw_Angle = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);

  // Multiplica por 10 para resultados de 0 a 1
  amp = Raw_Amp * 10;
  angle = Raw_Angle * 10;

  /* // Se amplitude alcançar limiar mínimo (0.4g)
  if (amp <= 2 && gatilho2 == false)
  {
    gatilho1 = true;
    Serial.println("GATILHO 1 ATIVADO");
  }
  if (gatilho1 == true)
  {
    gatilho1Count++;

    // Se amplitude alcança limiar máximo (3g)
    if (amp >= 12)
    {
      gatilho2 = true;
      Serial.println("GATILHO 2 ATIVADO");
      gatilho1 = false;
      gatilho1Count = 0;
    }
  }
  if (gatilho2 == true)
  {
    gatilho2Count++;
    mudancaAngulo = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
    Serial.println(mudancaAngulo);

    // Se orientação muda entre 80-100 graus
    if (mudancaAngulo >= 30 && mudancaAngulo <= 400)
    {
      gatilho3 = true;
      gatilho2 = false;
      gatilho2Count = 0;
      Serial.println(mudancaAngulo);
      Serial.println("GATILHO 3 ATIVADO");
    }
  }
  if (gatilho3 == true)
  {
    gatilho3Count++;
    if (gatilho3Count >= 10)
    {
      mudancaAngulo = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
      Serial.println(mudancaAngulo);

      // Se mudanças de orientação se mantém entre 0-10 graus
      if ((mudancaAngulo >= 0) && (mudancaAngulo <= 10))
      {
        queda = true;
        gatilho3 = false;
        gatilho3Count = 0;
        Serial.println(mudancaAngulo);
      }
      else

      // Usuário recupera orientação normal
      {
        gatilho3 = false;
        gatilho3Count = 0;
        Serial.println("GATILHO 3 DESATIVADO");
      }
    }
  }

  // Em caso de detecção de queda
  if (queda == true && !alarmeAtivo)
  {
    alarmeAtivo = true;
    Serial.println("QUEDA DETECTADA");
    //send_event("queda_detectada");
    enviaAlerta();
  }

  if (alarmeAtivo)
  {
    for (int i = 0; i < 10; i++)
    {
      led_alert();
      buzzer_on();
    }
  }

  // Permite 0.5 segundos para mudança de orientação
  if (gatilho2Count >= 6)
  {
    gatilho2 = false;
    gatilho2Count = 0;
    Serial.println("GATILHO 2 DESATIVADO");
  }

  // Permite 0.5 segundos para amplitude quebrar limiar máximo
  if (gatilho1Count >= 6)
  {
    gatilho1 = false;
    gatilho1Count = 0;
    Serial.println("GATILHO 1 DESATIVADO");
  }
 */

  // Serial.println("Leitura do botão");

  //  Leitura do botão
  switch (buttonState)
  {
    case BUTTON_IDLE:
      if (botaoState == HIGH)
      {
        buttonState = BUTTON_PRESSED;
        buttonPressedTime = millis();
      }
      break;

    case BUTTON_PRESSED:
      if (botaoState == LOW)
      {
        buttonState = BUTTON_IDLE;
      }
      else if (millis() - buttonPressedTime >= 3000)
      {
        buttonState = BUTTON_WAIT_RELEASE;
        enviaAlerta();
        for (int i = 0; i < 20; i++)
        {
          Serial.println(i);
          buzzer_on();
          delay(200);
          buzzer_off();
          delay(50);
        }
        estadoBotaoEmergencia = false;
      }
      break;

    case BUTTON_WAIT_RELEASE:
      if (botaoState == LOW)
      {
        estadoBotaoEmergencia = true;
        //morseAlert();
        //buzzer_off();
        buttonState = BUTTON_IDLE;
      }
      break;
  }

  // Serial.println("populateJSON");
  populateJSON();
  // Serial.println("makePOST");
  makePOST();
}

// Realiza conexão com rede WiFi
void conectaWifi()
{
  led_on();
  int arraySize = sizeof(ssid) / sizeof(ssid[0]);
  Serial.println("Conectando ao WiFi...");
  for (int i = 0; i < arraySize; i++)
  {
    Serial.printf("\nTentando conectar-se a %s\n", ssid[i]);
    WiFi.begin(ssid[i], pass[i]);
    int tentativa = 0;
    while (WiFi.status() != WL_CONNECTED && tentativa < 10)
    {
      delay(1000);
      Serial.print(".");
      tentativa++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      IPAddress localIP = WiFi.localIP();
      String ipString = localIP.toString();
      nodeMCUIP = ipString.c_str();
      Serial.printf("\nConectado a %s\n", ssid[i]);
      Serial.printf("Endereço IP: %s\n", nodeMCUIP);
      break;
    }
    else if (WiFi.status() != WL_CONNECTED)
    {
      unsigned long lastMsgTime = 0;
      const unsigned long msgInterval = 5000;
      unsigned long currentMillis = millis();

      if (currentMillis - lastMsgTime >= msgInterval)
      {
        lastMsgTime = currentMillis;
        Serial.printf("\nFalha na conexão. Não foi possível conectar ao WiFi");
        led_off();
      }
    }
  }
}

// Realiza leituras dos sensores
void mpu_read()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // Iniciando com registrador 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // Requisita um total de 14 registradores
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H)   & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H)  & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H)  & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H)  & 0x48 (GYRO_ZOUT_L)
}

// Envia eventos para o IFTTT
void send_event(const char*event)
{
  Serial.print("Conectando em ");
  Serial.println(host);

  // Utiliza WiFiClient para criar conexões TCP
  const int httpPort = 80;
  if (!client.connect(host, httpPort))
  {
    Serial.println("Conexão falhou");
    return;
  }

  // Cria identificador uniforme de recurso (URI) para requisição
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;
  Serial.print("Requisitando URL: ");
  Serial.println(url);

  // Envia requisição ao servidor
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  // Aguarda resposta do servidor
  while (client.connected())
  {
    if (client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    else
    {
      // Aguarda até receber dados
      if (now - previousMillisSendEvent >= intervaloSendEvent)
      {
        previousMillisSendEvent = now;
        delay(1);
      }
    };
  }
  Serial.println();
  Serial.println("Fechando conexão");
  client.stop();
}

// Liga buzzer
void buzzer_on()
{
  // obtém tempo atual
  unsigned long currentMillis = millis();

  // verifica se passou tempo suficiente desde a última mudança no buzzer
  if (currentMillis - previousMillisBuzzer >= intervaloBuzzer)
  {
    // atualiza o tempo anterior
    previousMillisBuzzer = currentMillis;

    if (buzzerState)
    {
      digitalWrite(buzzer, LOW);
    }
    else
    {
      digitalWrite(buzzer, HIGH);
    }
    buzzerState = !buzzerState;
  }
}

// Desliga buzzer
void buzzer_off()
{
  digitalWrite(buzzer, LOW);
}

// Liga led

void led_on()
{
  digitalWrite(led, LOW);
}

// Desliga led
void led_off()
{
  digitalWrite(led, HIGH);
}

// Armazena dados do sensor em um objeto JSON
void populateJSON()
{
  object["nodeID"] = "NodeMCUESP8266";

  data["temp"] = Tmp / 340.00 + 36.53;

  accel["accelX"] = ax;
  accel["accelY"] = ay;
  accel["accelZ"] = az;

  gyro["gyroX"] = gx;
  gyro["gyroY"] = gy;
  gyro["gyroZ"] = gz;

  infoDev["ampData"] = amp;
  infoDev["botaoEmergencia"] = estadoBotaoEmergencia;
  infoDev["quedaDetectada"] = queda;
  infoDev["nodeIP"] = tempoLigado;
  infoDev["angleData"] = angle;

  battery["battLevel"] = batteryLevel;
  battery["battVoltage"] = voltagemBateria;
}

// Envia dados do sensor para o servidor NodeJS
void makePOST()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMakePOSTTime >= intervalMakePOST)
  {
    previousMakePOSTTime = currentMillis;
    // Serial.println("Início envio dos dados");
    led_on();
    client.println("POST /accel HTTP/1.1");
    client.print("Host: ");
    client.println(connectedJSHost);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(measureJson(jsonDocument));
    client.println();
    serializeJson(jsonDocument, client);
    // Serial.println("Dados enviados");
    led_off();

    // Recebe e imprime resposta do servidor
    while (client.available())
    {
      char c = client.read();
      Serial.print(c);
    }
    /* if (!client.connected())
    {
      Serial.println("A conexão com o servidor NODEjs foi perdida");
      Serial.printf("\nO servidor desconectou após %s segundos. Essa mensagem foi exibida %s vezes.\n", tempoDecorrido, erroServidorNODEQtdMsg);
      //client.stop();
    } */
  }
}

// Chama função de envio de alertas ao IFTTT e atualiza variável que envia mensagem para página web
void enviaAlerta()
{
  // send_event("queda_detectada");
  Serial.println("Função enviaAlerta ativada");
}

// Lê e retorna valores da leitura do nível da bateria
void batteryLevelRead(int rawLevel)
{
  batteryVoltage = (((rawLevel / 1023.0) * 3.3) * 2); // Converte leitura analógica para voltagem (assumindo 3.3V como referência)

  float minVoltage = 3.0;
  float maxVoltage = 4.2;

  float nivelCarga = ((batteryVoltage - minVoltage) / (maxVoltage - minVoltage)) * 100;
  batteryLevel = nivelCarga;

  Serial.print("Nível bruto da bateria: ");
  Serial.print(rawLevel);
  Serial.print(". Voltagem da bateria (após mapeamento): ");
  Serial.print(batteryVoltage);
  Serial.print("V. Nível da bateria (calculado): ");
  Serial.print(batteryLevel);
  Serial.println("%");
}

// Pisca led como forma de alertar perda de conexão com nodejs
void ledHostConnect()
{
  unsigned long previousMillisError = 0;
  unsigned long currentMillis = millis();
  const unsigned long intervaloError = 200;

  if (currentMillis - previousMillisError >= intervaloError)
  {
    previousMillisLed = currentMillis;

    if (ledState)
    {
      digitalWrite(led, LOW);
    }
    else
    {
      digitalWrite(led, HIGH);
    }
    ledState = !ledState;
  }
}

// Realiza conexão com servidor NodeJS
void conectaNode()
{
  int arraySize = sizeof(nodeJSHost) / sizeof(nodeJSHost[0]);

  while (!client.connected())
  {
    for (int i = 0; i < arraySize; i++)
    {
      Serial.printf("\nTentando conexão com servidor %s\n", nodeJSHost[i]);
      led_on();
      if (client.connect(nodeJSHost[i], 3000))
      {
        Serial.println("Conectado ao servidor NODEjs");
        connectedJSHost = nodeJSHost[i];
        Serial.print("Endereço IP: ");
        Serial.println(connectedJSHost);
        break;
      }
      else
      {
        led_off();
        Serial.printf("\nNão foi possível conectar ao servidor %s\n", nodeJSHost[i]);
      }
    }
  }
}

void morseAlert()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    char currentChar = morseMessage[morseIndex];

    if (currentChar == '.')
    {
      digitalWrite(buzzer, HIGH);
      interval = dotDuration;
    }
    else if (currentChar == '-')
    {
      digitalWrite(buzzer, HIGH);
      interval = dashDuration;
    }
    else if (currentChar == ' ')
    {
      // Se for um espaço em branco, não desligue o buzzer
      interval = dotDuration;
    }

    morseIndex++;

    if (morseMessage[morseIndex] == '\0')
    {
      morseIndex = 0;
      // Não use delay aqui, pois isso irá bloquear o loop
    }
  }
}

void mpuRead()
{
  int16_t accelX, accelY, accelZ;
  mpu.getAcceleration(&accelX, &accelY, &accelZ);
  int accelerationMagnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
  if (accelerationMagnitude > threshold)
  {
    Serial.print("Threshold ultrapassado. Queda detectada! Magnitude: ");
    Serial.println(accelerationMagnitude);
    queda = true;
    Serial.print("Status queda: ");
    Serial.println(queda);
    for (int i = 0; i < 20; i++)
    {
      buzzer_on();
      delay(300);
      buzzer_off();
      delay(100);
    }
  }
  else
  {
    queda = false;
    buzzer_off();
  }
  /*   Serial.print("Magnitude: ");
    Serial.println(accelerationMagnitude);
    size_t freeHeapSize = ESP.getFreeHeap();
    Serial.print("Memória livre: ");
    Serial.print(freeHeapSize);
    Serial.println(" bytes"); */
}
