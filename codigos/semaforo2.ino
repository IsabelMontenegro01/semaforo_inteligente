/*
 * SEMÁFORO 2 — VIA SECUNDÁRIA
 * COM MODO NOTURNO + MQTT UBIDOTS
 *
 * LÓGICA MODO DIA:
 * - Começa VERMELHO.
 * - LDR carro detecta sombra = carro.
 * - Quando detecta carro E passou o tempo mínimo de vermelho:
 *       → VERDE
 * - Enquanto houver carro, fica VERDE.
 * - Quando o carro sai:
 *       espera TEMPO_VERDE_APOS_SAIDA → AMARELO → VERMELHO.
 *
 * LÓGICA MODO NOTURNO:
 * - LDR ambiente < limiar ⇒ pisca AMARELO.
 *
 * UBIDOTS (device: semaforo-2):
 *   - ldr-ambiente-s2
 *   - ldr-carro-s2
 *   - carro-esperando-s2
 *   - estado-texto
 *   - modo-noturno-manual (opcional)
 */

// ---------------------- BIBLIOTECAS ----------------------
#include <WiFi.h>
#include <PubSubClient.h>

// ---------------------- WIFI ----------------------
const char* WIFI_SSID = "Telefone Samel";
const char* WIFI_PASS = "Chiovat123";

// ---------------------- UBIDOTS MQTT ----------------------
const char* UBIDOTS_TOKEN = "BBUS-LpQc6IcwfcDwcKIMjlL8CKZuPuwz6T";
const char* MQTT_BROKER   = "industrial.api.ubidots.com";
const int   MQTT_PORT     = 1883;
const char* MQTT_CLIENT_NAME = "esp32-semaforo-2";

// ⚠ TEM QUE SER IGUAL AO API LABEL DO DEVICE NO UBIDOTS
const char* DEVICE_LABEL_S2 = "semaforo-2";

// Nomes das variáveis no Ubidots
const char* VAR_LDR_AMBIENTE_S2    = "ldr-ambiente-s2";
const char* VAR_LDR_CARRO_S2       = "ldr-carro-s2";
const char* VAR_CARRO_ESPERANDO_S2 = "carro-esperando-s2";
const char* VAR_ESTADO_TEXTO_S2    = "estado-texto";
const char* VAR_MODO_NOTURNO_CMD   = "modo-noturno-manual";

// ---------------------- PINOS ----------------------
const int ledVerde2    = 27;
const int ledAmarelo2  = 25;
const int ledVermelho2 = 26;

const int ldrAmbientePin = 34; // ADC1
const int ldrCarroPin    = 35; // ADC1

// ---------------------- VARIÁVEIS ----------------------
int  ldrAmbientePercent = 0;
int  ldrCarroBruto      = 0;
int  ldrCarroFiltrado   = 0;

int  limiarNoitePercent = 60;
int  limiarCarro        = 1500;

bool noite          = false;
bool carroDetectado = false;

bool modoForcadoDia   = false;
bool modoForcadoNoite = false;

// ---------------------- ESTADOS ----------------------
enum EstadoDiaS2 { S2_VERDE, S2_AMARELO, S2_VERMELHO };
enum EstadoNoturnoS2 { S2_NOTURNO_ON, S2_NOTURNO_OFF };

EstadoDiaS2 estadoDia = S2_VERMELHO;
EstadoNoturnoS2 estadoNoturno = S2_NOTURNO_OFF;

String estadoTexto = "vermelho";

unsigned long tempoEstado = 0;
unsigned long instanteUltimoCarro = 0;

// tempos
const long TEMPO_VERMELHO_MIN      = 2000;
const long TEMPO_VERDE_APOS_SAIDA  = 3000;
const long TEMPO_AMARELO           = 2000;
const long TEMPO_PISCA_NOTURNO     = 500;

// Timers
unsigned long ultimoPing = 0;
unsigned long ultimoPublish = 0;

const long INTERVALO_SENSOR = 250;
const long INTERVALO_MQTT   = 2000;

// MQTT
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// ---------------------- HELPERS ----------------------
void desligaTodos() {
  digitalWrite(ledVerde2, LOW);
  digitalWrite(ledAmarelo2, LOW);
  digitalWrite(ledVermelho2, LOW);
}

// ---------------------- WIFI ----------------------
void setup_wifi() {
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

// ---------------------- MQTT CALLBACK ----------------------
void callback(char* topic, byte* payload, unsigned int length) {
  char buf[length + 1];
  memcpy(buf, payload, length);
  buf[length] = '\0';
  String msg = String(buf);

  Serial.println("[MQTT] Recebido: " + String(topic) + " = " + msg);

  char topicModo[128];
  sprintf(topicModo, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL_S2, VAR_MODO_NOTURNO_CMD);

  if (strcmp(topic, topicModo) == 0) {
    if (msg == "1" || msg == "1.0") {
      modoForcadoNoite = true;
      modoForcadoDia = false;
    } else if (msg == "0" || msg == "0.0") {
      modoForcadoDia = true;
      modoForcadoNoite = false;
    } else {
      modoForcadoDia = false;
      modoForcadoNoite = false;
    }
  }
}

// ---------------------- MQTT RECONNECT ----------------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Tentando conexão...");
    if (client.connect(MQTT_CLIENT_NAME, UBIDOTS_TOKEN, "")) {

      Serial.println("Conectado!");

      char topicModo[128];
      sprintf(topicModo, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL_S2, VAR_MODO_NOTURNO_CMD);
      client.subscribe(topicModo);

    } else {
      Serial.println("Falhou, tentando de novo...");
      delay(3000);
    }
  }
}

// ---------------------- PUBLICAÇÃO UBIDOTS ----------------------
void publicarUbidots() {
  // Ambiente
  char payload1[64];
  sprintf(payload1, "{\"value\": %d}", ldrAmbientePercent);
  char topic1[128];
  sprintf(topic1, "/v1.6/devices/%s/%s", DEVICE_LABEL_S2, VAR_LDR_AMBIENTE_S2);
  client.publish(topic1, payload1);

  // Carro
  char payload2[64];
  sprintf(payload2, "{\"value\": %d}", ldrCarroFiltrado);
  char topic2[128];
  sprintf(topic2, "/v1.6/devices/%s/%s", DEVICE_LABEL_S2, VAR_LDR_CARRO_S2);
  client.publish(topic2, payload2);

  // Detecção (0 ou 1)
  char payload3[64];
  sprintf(payload3, "{\"value\": %d}", carroDetectado ? 1 : 0);
  char topic3[128];
  sprintf(topic3, "/v1.6/devices/%s/%s", DEVICE_LABEL_S2, VAR_CARRO_ESPERANDO_S2);
  client.publish(topic3, payload3);

  // Estado do semáforo
  char topic4[128];
  sprintf(topic4, "/v1.6/devices/%s/%s", DEVICE_LABEL_S2, VAR_ESTADO_TEXTO_S2);
  client.publish(topic4, estadoTexto.c_str());

  Serial.println("[MQTT] Publicado!");
}

// ---------------------- SETUP ----------------------
void setup() {
  Serial.begin(115200);

  pinMode(ledVerde2, OUTPUT);
  pinMode(ledAmarelo2, OUTPUT);
  pinMode(ledVermelho2, OUTPUT);

  pinMode(ldrAmbientePin, INPUT);
  pinMode(ldrCarroPin, INPUT);

  desligaTodos();
  digitalWrite(ledVermelho2, HIGH);

  estadoDia = S2_VERMELHO;
  estadoTexto = "vermelho";
  tempoEstado = millis();
  instanteUltimoCarro = millis();

  setup_wifi();
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
}

// ---------------------- LOOP ----------------------
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long agora = millis();

  // ========== LEITURA DOS SENSORES ==========
  if (agora - ultimoPing > INTERVALO_SENSOR) {
    ultimoPing = agora;

    // Ambiente
    int amb = analogRead(ldrAmbientePin);
    ldrAmbientePercent = map(amb, 0, 4095, 0, 100);

    // Carro filtrado
    int leitura = analogRead(ldrCarroPin);
    static bool init = false;
    if (!init) {
      ldrCarroFiltrado = leitura;
      init = true;
    } else {
      ldrCarroFiltrado = (ldrCarroFiltrado * 3 + leitura) / 4;
    }

    carroDetectado = (ldrCarroFiltrado < limiarCarro);

    Serial.printf("[S2] Amb=%d%% | Carro=%d (filt=%d) | det=%d\n",
      ldrAmbientePercent, leitura, ldrCarroFiltrado, carroDetectado);
  }

  // ========== MODO NOITE ==========
  bool noiteAuto = (ldrAmbientePercent < limiarNoitePercent);
  if (modoForcadoDia) noite = false;
  else if (modoForcadoNoite) noite = true;
  else noite = noiteAuto;

  if (noite) {
    desligaTodos();
    if ((agora - tempoEstado) > TEMPO_PISCA_NOTURNO) {
      tempoEstado = agora;
      digitalWrite(ledAmarelo2, !digitalRead(ledAmarelo2));
      estadoTexto = digitalRead(ledAmarelo2) ? "noturno-on" : "noturno-off";
    }
  }

  // ========== MODO DIA ==========
  else {
    switch (estadoDia) {
      case S2_VERMELHO:
        desligaTodos();
        digitalWrite(ledVerde2, LOW);
        digitalWrite(ledAmarelo2, LOW);
        digitalWrite(ledVermelho2, HIGH);
        estadoTexto = "vermelho";

        if ((agora - tempoEstado > TEMPO_VERMELHO_MIN) && carroDetectado) {
          estadoDia = S2_VERDE;
          tempoEstado = agora;
          instanteUltimoCarro = agora;
        }
        break;

      case S2_VERDE:
        desligaTodos();
        digitalWrite(ledVerde2, HIGH);
        estadoTexto = "verde";

        if (carroDetectado) {
          instanteUltimoCarro = agora;
        } else {
          if (agora - instanteUltimoCarro > TEMPO_VERDE_APOS_SAIDA) {
            estadoDia = S2_AMARELO;
            tempoEstado = agora;
          }
        }
        break;

      case S2_AMARELO:
        desligaTodos();
        digitalWrite(ledAmarelo2, HIGH);
        estadoTexto = "amarelo";

        if (agora - tempoEstado > TEMPO_AMARELO) {
          estadoDia = S2_VERMELHO;
          tempoEstado = agora;
        }
        break;
    }
  }

  // ========== PUBLICAÇÃO UBIDOTS ==========
  if (agora - ultimoPublish > INTERVALO_MQTT) {
    ultimoPublish = agora;
    publicarUbidots();
  }
}
