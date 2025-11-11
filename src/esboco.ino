#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

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
#define LED_MANHA 12
#define LED_TARDE 13
#define LED_NOITE 14
#define LED_EXTRA 15
#define BTN_MANHA 16
#define BTN_TARDE 17
#define BTN_NOITE 18
#define BTN_EXTRA 19
#define BUZZER 21
#define LDR_PIN 34

// Debounce
unsigned long lastDebounceTime[4] = {0,0,0,0};
bool lastButtonRead[4] = {HIGH, HIGH, HIGH, HIGH};
bool buttonHandled[4] = {false,false,false,false};
const unsigned long debounceDelay = 80;

bool tampaAberta = false;

String horarioISO() {
  time_t now = time(NULL);
  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
  return String(buf);
}

// Publica JSON
void publishJSON(String topic, JsonDocument &doc) {
  char buffer[250];
  serializeJson(doc, buffer);
  client.publish(topic.c_str(), buffer);
  Serial.print("MQTT → "); Serial.println(topic);
  Serial.println(buffer);
}

// ---- Eventos ----
void publicarDose(String periodo) {
  StaticJsonDocument<200> doc;
  doc["dispositivo"] = device_id;
  doc["evento"] = "dose";
  doc["horario"] = periodo;
  doc["status"] = "tomada";
  doc["timestamp"] = horarioISO();
  publishJSON("pillbox/pillbox01/dose", doc);
}

void publicarTampa(String estado) {
  StaticJsonDocument<200> doc;
  doc["sensor"] = "tampa";
  doc["status"] = estado;
  doc["timestamp"] = horarioISO();
  publishJSON("pillbox/pillbox01/tampa", doc);
}

void publicarStatus(String estado) {
  StaticJsonDocument<200> doc;
  doc["dispositivo"] = device_id;
  doc["evento"] = "status";
  doc["status"] = estado;
  doc["timestamp"] = horarioISO();
  publishJSON("pillbox/pillbox01/status", doc);
}

void publicarStatusSensor(bool ok) {
  StaticJsonDocument<200> doc;
  doc["sensor"] = "ldr";
  doc["status"] = ok ? "online" : "offline";
  doc["timestamp"] = horarioISO();
  publishJSON("pillbox/pillbox01/sensor", doc);
}

// Dose local
void handleButtonPress(int idx) {
  const char* nomes[4] = {"manha","tarde","noite","extra"};
  int leds[4] = {LED_MANHA, LED_TARDE, LED_NOITE, LED_EXTRA};

  if (idx == 3) {
    tone(BUZZER, 1500); delay(300); noTone(BUZZER);
  }

  digitalWrite(leds[idx], HIGH);
  publicarDose(nomes[idx]);
  delay(200);
  digitalWrite(leds[idx], LOW);
}

// MQTT Callback
def callback(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
  Serial.print("MQTT CMD: "); Serial.println(t);

  if (t.endsWith("/buzzer")) {
    tone(BUZZER, 1500); delay(300); noTone(BUZZER);
    StaticJsonDocument<150> doc;
    doc["evento"] = "buzzer";
    doc["status"] = "ativado";
    doc["timestamp"] = horarioISO();
    publishJSON("pillbox/pillbox01/buzzer", doc);
  }

  if (t.endsWith("/manha")) handleButtonPress(0);
  if (t.endsWith("/tarde")) handleButtonPress(1);
  if (t.endsWith("/noite")) handleButtonPress(2);
  if (t.endsWith("/extra")) handleButtonPress(3);
  if (t.endsWith("/tampa_simular")) publicarTampa("simulada");

  if (t.endsWith("/status")) publicarStatus("online");

  if (t.endsWith("/status_sensor")) {
    bool ok = analogRead(LDR_PIN) >= 0;
    publicarStatusSensor(ok);
  }
}

// ---- Conexões ----
def conectarWiFi() {
  Serial.println(" Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(300); Serial.print("."); }
  Serial.println("\n✅ WiFi conectado");
}

def conectarMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println(" Conectando ao MQTT...");
  while (!client.connected()) {
    if (client.connect(device_id)) {
      Serial.println("✅ MQTT conectado");
      client.subscribe("pillbox/pillbox01/comando/#");
      publicarStatus("online");
    } else {
      Serial.println("❌ Falha, tentando...");
      delay(800);
    }
  }
}

def setup() {
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
  conectarMQTT();
  configTime(0,0,"pool.ntp.org");
}

def loop() {
  if (!client.connected()) conectarMQTT();
  client.loop();

  bool aberta = (analogRead(LDR_PIN) > 2500);
  if (aberta && !tampaAberta) { tampaAberta = true; publicarTampa("aberta"); }
  if (!aberta && tampaAberta) { tampaAberta = false; publicarTampa("fechada"); }

  int reads[4] = {
    digitalRead(BTN_MANHA),
    digitalRead(BTN_TARDE),
    digitalRead(BTN_NOITE),
    digitalRead(BTN_EXTRA)
  };

  for (int i=0;i<4;i++) {
    if (reads[i] == LOW && !buttonHandled[i]) { buttonHandled[i] = true; handleButtonPress(i); }
    if (reads[i] == HIGH) buttonHandled[i] = false;
  }
}