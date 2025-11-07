# 💊 Caixa Remédio Nuvem

Projeto desenvolvido na plataforma [Wokwi](https://wokwi.com) utilizando **ESP32**, com integração ao **HiveMQ Cloud (MQTT)** para monitoramento e controle remoto de uma caixa de remédios inteligente.

---

## 📡 Conexão com o HiveMQ

- **Servidor MQTT:** `broker.hivemq.com`  
- **Porta:** `1883`  
- **Device ID:** `pillbox01`  
- **Tópicos usados:**
  - `pillbox/pillbox01/comando/#` – comandos recebidos (buzzer e botões)  
  - `pillbox/pillbox01/dose/<periodo>/status` – confirmações de dose enviadas  
  - `pillbox/pillbox01/status` – status de conexão (“online”)

---

## ⚙️ Código-fonte principal (`main.ino`)

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

// Projeto: Caixa Remédio Nuvem
// Repositório GitHub: https://github.com/joaorinaldo210/caixa-remedio-cloud
// Desenvolvido por: João Rinaldo França Neris, Bruno Otávio Ramos, Gabriel Matheus Soares de Carvalho

// --- WiFi ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// --- MQTT ---
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* device_id = "pillbox01";

WiFiClient espClient;
PubSubClient client(espClient);

// --- PINOS ---
// LEDs
#define LED_MANHA 12
#define LED_TARDE 13
#define LED_NOITE 14
#define LED_EXTRA 15

// Botões
#define BTN_MANHA 16
#define BTN_TARDE 17
#define BTN_NOITE 18
#define BTN_EXTRA 19

// Buzzer e LDR
#define BUZZER 21
#define LDR_PIN 34

// --- Debounce ---
unsigned long lastDebounceTime[4] = {0,0,0,0};
const unsigned long debounceDelay = 80;
bool lastButtonRead[4] = {HIGH, HIGH, HIGH, HIGH};
bool buttonHandled[4] = {false,false,false,false};

// ------------------------ MQTT CALLBACK ------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
  Serial.print("MQTT recebido: "); Serial.println(t);

  // ✅ Comando MQTT para buzzer
  if (t.endsWith("/comando/buzzer")) {
    tone(BUZZER, 1500);
    delay(300);
    noTone(BUZZER);
  }

  // ✅ Comandos MQTT para simular botões
  if (t.endsWith("/comando/manha")) { handleButtonPress(0); }
  if (t.endsWith("/comando/tarde")) { handleButtonPress(1); }
  if (t.endsWith("/comando/noite")) { handleButtonPress(2); }
  if (t.endsWith("/comando/extra")) { handleButtonPress(3); }
}

// ------------------------ WIFI ------------------------
void conectarWiFi() {
  Serial.print("Conectando WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println(" ✅ Conectado");
}

// ------------------------ MQTT ------------------------
void conectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");
    if (client.connect(device_id)) {
      Serial.println(" ✅ Conectado");
      client.subscribe("pillbox/pillbox01/comando/#");
      client.publish(("pillbox/" + String(device_id) + "/status").c_str(), "online", true);
    } else {
      Serial.print(" falhou, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ------------------------ FUNÇÕES ------------------------
String horarioISO() {
  time_t now = time(NULL);
  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
  return String(buf);
}

void publicarConfirmacao(const char* periodo) {
  StaticJsonDocument<128> doc;
  doc["ok"] = true;
  doc["ts"] = horarioISO();
  char buffer[128];
  serializeJson(doc, buffer);
  String topic = "pillbox/" + String(device_id) + "/dose/" + periodo + "/status";
  client.publish(topic.c_str(), buffer);
  Serial.print("Publicado: "); Serial.println(topic);
}

void handleButtonPress(int idx) {
  const char* nomes[4] = {"manha","tarde","noite","extra"};
  int leds[4] = {LED_MANHA, LED_TARDE, LED_NOITE, LED_EXTRA};

  digitalWrite(leds[idx], HIGH);
  publicarConfirmacao(nomes[idx]);
  Serial.print("Botao "); Serial.print(nomes[idx]); Serial.println(" pressionado");
  delay(250);
  digitalWrite(leds[idx], LOW);
}

// ------------------------ SETUP ------------------------
void setup() {
  Serial.begin(115200);

  pinMode(LED_MANHA, OUTPUT);
  pinMode(LED_TARDE, OUTPUT);
  pinMode(LED_NOITE, OUTPUT);
  pinMode(LED_EXTRA, OUTPUT);

  pinMode(BTN_MANHA, INPUT_PULLUP);
  pinMode(BTN_TARDE, INPUT_PULLUP);
  pinMode(BTN_NOITE, INPUT_PULLUP);
  pinMode(BTN_EXTRA, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  conectarWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  configTime(0,0,"pool.ntp.org");

  Serial.println("✅ Sistema iniciado");
}

// ------------------------ LOOP ------------------------
void loop() {
  if (!client.connected()) conectarMQTT();
  client.loop();

  // ✅ Tampa aberta
  int ldr = analogRead(LDR_PIN);
  if (ldr > 2500) { 
    Serial.println("⚠️ Tampa aberta detectada!");
    tone(BUZZER, 1500);
    delay(500);
    noTone(BUZZER);
    del

