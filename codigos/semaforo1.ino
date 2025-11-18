/*
 * SEMÁFORO 1 (VIA PRINCIPAL) + MQTT UBIDOTS
 * - LDR no ESP1 detecta carro (sombra)
 * - Envia para Ubidots:
 *    ldr-carro-s1        (numérico)
 *    carro-esperando-s1  (0 ou 1)
 *    estado-texto        ("vermelho","verde","amarelo")
 */

#include <WiFi.h>
#include <PubSubClient.h>

// ---------------------- WIFI ----------------------
const char* WIFI_SSID = "Telefone Samel";
const char* WIFI_PASS = "Chiovat123";

// ---------------------- UBIDOTS MQTT ----------------------
const char* UBIDOTS_TOKEN     = "BBUS-QrgIjqLuDaWRvGkhk7hvefjQT6rlxn";
const char* MQTT_BROKER      = "industrial.api.ubidots.com";
const int   MQTT_PORT        = 1883;
const char* MQTT_CLIENT_NAME = "esp32-semaforo-1";

// DEVICE E VARIÁVEIS (labels IGUAIS no Ubidots)
const char* DEVICE_LABEL_S1        = "esp32_t17_g01";
const char* VAR_LDR_CARRO_S1       = "ldr-carro-s1";
const char* VAR_CARRO_ESPERANDO_S1 = "carro-esperando-s1";
const char* VAR_ESTADO_TEXTO_S1    = "estado-texto";

// ---------------------- PINOS ----------------------
const int ledVerde    = 27;
const int ledAmarelo  = 33;
const int ledVermelho = 12;

const int ldrCarroPin = 34;   // LDR do carro (entrada analógica)

// ---------------------- LDR / CARRO ----------------------
int  ldrBruto     = 0;
int  ldrFiltrado  = 0;
int  limiarCarro  = 1500;   // ABAIXO disso = carro (sombra)
bool carroDetectado = false;

// ---------------------- ESTADOS DO SEMÁFORO ----------------------
enum EstadoS1 {
  VERDE,
  AMARELO,
  VERMELHO
};

EstadoS1 estadoAtual = VERMELHO;
String   estadoTexto = "vermelho";

unsigned long tempoEstado = 0;

// tempos em ms
const long TEMPO_VERMELHO_MIN     = 2000; // mínimo em vermelho
const long TEMPO_VERDE_APOS_SAIDA = 3000; // verde extra depois que o carro sai
const long TEMPO_AMARELO          = 2000;

// ---------------------- TIMERS LEITURA / MQTT ----------------------
unsigned long ultimoPingLdr = 0;
unsigned long ultimoPublish = 0;

const long INTERVALO_LDR  = 250;   // 0,25 s
const long INTERVALO_MQTT = 2000;  // 2 s

// ---------------------- CLIENTES ----------------------
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// =====================================================
// WIFI
// =====================================================
void setup_wifi() {
  Serial.print("\nConectando ao WiFi ");
  Serial.print(WIFI_SSID);
  Serial.println(" ...");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// =====================================================
// MQTT
// =====================================================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT... ");
    if (client.connect(MQTT_CLIENT_NAME, UBIDOTS_TOKEN, "")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 4s...");
      delay(4000);
    }
  }
}

// envia 3 variáveis
void publicarDados() {
  Serial.println(">> ENTROU em publicarDados()");  // DEBUG

  // 1) LDR filtrado
  char payload1[64];
  sprintf(payload1, "{\"value\": %d}", ldrFiltrado);

  char topic1[128];
  sprintf(topic1, "/v1.6/devices/%s/%s", DEVICE_LABEL_S1, VAR_LDR_CARRO_S1);
  client.publish(topic1, payload1);

  // 2) Carro detectado (0/1)
  char payload2[64];
  sprintf(payload2, "{\"value\": %d}", carroDetectado ? 1 : 0);

  char topic2[128];
  sprintf(topic2, "/v1.6/devices/%s/%s", DEVICE_LABEL_S1, VAR_CARRO_ESPERANDO_S1);
  client.publish(topic2, payload2);

  // 3) Estado do semáforo (texto)
  char topic3[128];
  sprintf(topic3, "/v1.6/devices/%s/%s", DEVICE_LABEL_S1, VAR_ESTADO_TEXTO_S1);
  client.publish(topic3, estadoTexto.c_str());

  Serial.print("MQTT -> LDR=");
  Serial.print(ldrFiltrado);
  Serial.print(" | carro=");
  Serial.print(carroDetectado ? 1 : 0);
  Serial.print(" | estado=");
  Serial.println(estadoTexto);
}

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);

  pinMode(ledVerde,    OUTPUT);
  pinMode(ledAmarelo,  OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(ldrCarroPin, INPUT);

  // começa vermelho
  digitalWrite(ledVerde,    LOW);
  digitalWrite(ledAmarelo,  LOW);
  digitalWrite(ledVermelho, HIGH);

  estadoAtual = VERMELHO;
  estadoTexto = "vermelho";
  tempoEstado = millis();

  setup_wifi();
  client.setServer(MQTT_BROKER, MQTT_PORT);
}

// =====================================================
// LOOP
// =====================================================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long agora = millis();

  // -------- LEITURA LDR + FILTRO --------
  if (agora - ultimoPingLdr > INTERVALO_LDR) {
    ultimoPingLdr = agora;

    int leitura = analogRead(ldrCarroPin);  // 0..4095

    static bool init = false;
    if (!init) {
      ldrFiltrado = leitura;
      init = true;
    } else {
      ldrFiltrado = (ldrFiltrado * 3 + leitura) / 4; // média móvel
    }

    carroDetectado = (ldrFiltrado < limiarCarro);

    Serial.print("LDR bruto = ");
    Serial.print(leitura);
    Serial.print(" | filtrado = ");
    Serial.print(ldrFiltrado);
    Serial.print(" | carroDetectado = ");
    Serial.println(carroDetectado ? "SIM" : "NAO");
  }

  // -------- MÁQUINA DE ESTADOS --------
  switch (estadoAtual) {
    case VERMELHO: {
      digitalWrite(ledVerde,    LOW);
      digitalWrite(ledAmarelo,  LOW);
      digitalWrite(ledVermelho, HIGH);
      estadoTexto = "vermelho";

      if ((agora - tempoEstado > TEMPO_VERMELHO_MIN) && carroDetectado) {
        estadoAtual = VERDE;
        tempoEstado = agora;
        Serial.println("Carro detectado + tempo mínimo -> VERDE");
      }
      break;
    }

    case VERDE: {
      digitalWrite(ledVerde,    HIGH);
      digitalWrite(ledAmarelo,  LOW);
      digitalWrite(ledVermelho, LOW);
      estadoTexto = "verde";

      static bool esperandoFechar = false;

      if (carroDetectado) {
        esperandoFechar = false;
        tempoEstado = agora;
      } else {
        if (!esperandoFechar) {
          esperandoFechar = true;
          tempoEstado = agora;
          Serial.println("Carro saiu -> contando tempo extra de VERDE...");
        } else if (agora - tempoEstado > TEMPO_VERDE_APOS_SAIDA) {
          estadoAtual = AMARELO;
          tempoEstado = agora;
          esperandoFechar = false;
          Serial.println("Tempo extra de VERDE acabou -> AMARELO");
        }
      }
      break;
    }

    case AMARELO: {
      digitalWrite(ledVerde,    LOW);
      digitalWrite(ledAmarelo,  HIGH);
      digitalWrite(ledVermelho, LOW);
      estadoTexto = "amarelo";

      if (agora - tempoEstado > TEMPO_AMARELO) {
        estadoAtual = VERMELHO;
        tempoEstado = agora;
        Serial.println("AMARELO acabou -> VERMELHO");
      }
      break;
    }
  }

  // -------- PUBLICAÇÃO MQTT --------
  if (agora - ultimoPublish > INTERVALO_MQTT) {
    Serial.println("Agendando publish...");   // DEBUG
    ultimoPublish = agora;
    publicarDados();
  }
}
