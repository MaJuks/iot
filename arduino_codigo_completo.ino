#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ===== CONFIG Wi-Fi =====
const char* ssid = "FAG-ENG";
const char* password = "";

// ===== PINOS =====
#define RELAY_PIN 5
#define TRIG_PIN 19
#define ECHO_PIN 18

// ===== SERVIDOR =====
WebServer server(80);

// ===== VARIÁVEIS DE CONTROLE =====
unsigned long lastMovementTime = 0;
unsigned long systemStartTime = 0;
bool lampOn = false;
const int detectionDistance = 500;  // distância em cm para acionar
const unsigned long timeoutOff = 10000;  // 10 segundos sem movimento

// ===== ESTATÍSTICAS =====
struct Stats {
  int totalActivations = 0;
  unsigned long totalTimeOn = 0;  // em milissegundos
  unsigned long lastActivationTime = 0;
  unsigned long currentActivationStart = 0;
  long currentDistance = 0;
  bool manualMode = false;  // true quando controlado manualmente
} stats;

// ===== HISTÓRICO DE ATIVAÇÕES (últimas 20) =====
#define MAX_LOGS 20
struct LogEntry {
  unsigned long timestamp;
  unsigned long duration;  // em segundos
  bool isOn;
};
LogEntry activationLogs[MAX_LOGS];
int logCount = 0;
int logIndex = 0;

// ===== FUNÇÕES AUXILIARES =====

// Adiciona entrada no log
void addLog(bool isOn, unsigned long duration = 0) {
  activationLogs[logIndex].timestamp = millis();
  activationLogs[logIndex].duration = duration;
  activationLogs[logIndex].isOn = isOn;

  logIndex = (logIndex + 1) % MAX_LOGS;
  if (logCount < MAX_LOGS) logCount++;
}

// Liga a lâmpada
void turnLampOn(bool isManual = false) {
  if (!lampOn) {
    digitalWrite(RELAY_PIN, LOW);  // Relé ativa em LOW
    lampOn = true;
    stats.totalActivations++;
    stats.currentActivationStart = millis();
    stats.manualMode = isManual;
    addLog(true);
    Serial.println("💡 Lâmpada LIGADA!");
  }
}

// Desliga a lâmpada
void turnLampOff(bool isManual = false) {
  if (lampOn) {
    digitalWrite(RELAY_PIN, HIGH);  // Relé desativa em HIGH
    lampOn = false;

    // Calcula duração da ativação
    unsigned long duration = (millis() - stats.currentActivationStart);
    stats.totalTimeOn += duration;
    addLog(false, duration / 1000);

    stats.manualMode = false;
    Serial.println("💡 Lâmpada DESLIGADA!");
  }
}

// ===== FUNÇÃO DISTÂNCIA =====
long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}

// ===== FUNÇÕES WEB API =====

// Habilita CORS para todas as requisições
void enableCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// Endpoint OPTIONS para CORS preflight
void handleOptions() {
  enableCORS();
  server.send(204);
}

// GET /status - Retorna estatísticas do sistema
void handleStatus() {
  enableCORS();

  StaticJsonDocument<512> doc;

  doc["isOn"] = lampOn;
  doc["totalActivations"] = stats.totalActivations;
  doc["totalTimeOn"] = stats.totalTimeOn / 1000;  // converte para segundos
  doc["uptime"] = (millis() - systemStartTime) / 1000;  // segundos
  doc["currentDistance"] = stats.currentDistance;
  doc["manualMode"] = stats.manualMode;

  // Calcula tempo médio por ativação
  if (stats.totalActivations > 0) {
    doc["averageTimeOn"] = (stats.totalTimeOn / 1000) / stats.totalActivations;
  } else {
    doc["averageTimeOn"] = 0;
  }

  // Última ativação
  if (lampOn) {
    doc["currentSessionDuration"] = (millis() - stats.currentActivationStart) / 1000;
  } else {
    doc["currentSessionDuration"] = 0;
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// GET /logs - Retorna histórico de ativações
void handleLogs() {
  enableCORS();

  DynamicJsonDocument doc(4096);
  JsonArray logs = doc.createNestedArray("logs");

  // Retorna logs em ordem cronológica inversa (mais recente primeiro)
  for (int i = 0; i < logCount; i++) {
    int idx = (logIndex - 1 - i + MAX_LOGS) % MAX_LOGS;
    JsonObject log = logs.createNestedObject();

    log["id"] = i;
    log["timestamp"] = activationLogs[idx].timestamp;
    log["duration"] = activationLogs[idx].duration;
    log["type"] = activationLogs[idx].isOn ? "on" : "off";
  }

  doc["total"] = logCount;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// GET /distance - Retorna distância atual
void handleDistance() {
  enableCORS();

  StaticJsonDocument<128> doc;
  doc["distance"] = stats.currentDistance;
  doc["unit"] = "cm";
  doc["timestamp"] = millis();

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// POST /on - Liga a lâmpada manualmente
void handleOn() {
  enableCORS();
  turnLampOn(true);

  StaticJsonDocument<128> doc;
  doc["success"] = true;
  doc["message"] = "Lâmpada ligada manualmente";
  doc["isOn"] = lampOn;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// POST /off - Desliga a lâmpada manualmente
void handleOff() {
  enableCORS();
  turnLampOff(true);

  StaticJsonDocument<128> doc;
  doc["success"] = true;
  doc["message"] = "Lâmpada desligada manualmente";
  doc["isOn"] = lampOn;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Inicia desligado

  systemStartTime = millis();

  // Wi-Fi
  Serial.println("🌐 Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.print("📡 IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println("==== ENDPOINTS DISPONÍVEIS ====");
  Serial.println("GET  /status    - Estatísticas do sistema");
  Serial.println("GET  /logs      - Histórico de ativações");
  Serial.println("GET  /distance  - Distância atual do sensor");
  Serial.println("POST /on        - Liga a lâmpada manualmente");
  Serial.println("POST /off       - Desliga a lâmpada manualmente");
  Serial.println("================================");
  Serial.println();

  // WebServer endpoints
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/logs", HTTP_GET, handleLogs);
  server.on("/distance", HTTP_GET, handleDistance);
  server.on("/on", HTTP_POST, handleOn);
  server.on("/on", HTTP_OPTIONS, handleOptions);
  server.on("/off", HTTP_POST, handleOff);
  server.on("/off", HTTP_OPTIONS, handleOptions);

  server.begin();
  Serial.println("🚀 Servidor HTTP iniciado!");
}

// ===== LOOP =====
void loop() {
  server.handleClient();

  // Lê distância do sensor
  long distance = readDistance();
  stats.currentDistance = distance;

  // Log periódico no Serial (a cada 2 segundos)
  static unsigned long lastSerialLog = 0;
  if (millis() - lastSerialLog > 2000) {
    Serial.printf("📏 Distância: %ld cm | 💡 Luz: %s\n",
                  distance,
                  lampOn ? "LIGADA" : "DESLIGADA");
    lastSerialLog = millis();
  }

  // MODO AUTOMÁTICO: Só funciona se não estiver em modo manual
  if (!stats.manualMode) {
    // Detecta presença
    if (distance > 0 && distance <= detectionDistance) {
      lastMovementTime = millis();
      turnLampOn(false);
    }

    // Desliga após timeout sem movimento
    if (lampOn && (millis() - lastMovementTime > timeoutOff)) {
      turnLampOff(false);
    }
  }

  delay(300);  // Leitura a cada 0,3s
}
