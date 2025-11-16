#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// --- Configurações de Rede (Q1) ---
// (Não precisa mexer, usa a rede Wi-Fi do Wokwi)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// --- Configurações do Broker MQTT (Q1) ---
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// --- Configurações do Projeto SkillUp ---
const char* ID = "559250_560027_560393"; // TODO: Validar RMs
const char* moduleID = "FocusTimer_01";  // ID deste dispositivo

// --- Pinos ---
#define BTN_PIN 4      // Botão de Foco (conectado ao D4)
#define LED_PIN 2      // LED onboard do ESP32

// --- Variáveis de Controle ---
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
bool isFocusSession = false; // Controla o estado (START/END)
String uniqueClientID;       // ID Unico (MAC Address) para conexao MQTT

// --- Função: Conectar ao WiFi ---
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Conectando ao Wi-Fi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Wi-Fi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
}

// --- Função: Conectar ao Broker MQTT ---
void reconnect_mqtt() {
    while (!client.connected()) {
        Serial.print("Conectando ao Broker MQTT...");
        
        // Tenta conectar usando o ID Unico (MAC)
        if (client.connect(uniqueClientID.c_str())) {
            Serial.println("Conectado ao Broker!");
        } else {
            Serial.print("Falha na conexão. Estado: ");
            Serial.print(client.state());
            Serial.println(" Tentando novamente em 5 segundos...");
            delay(5000);
        }
    }
}

// --- Função: Enviar Mensagem de Foco ---
void sendFocusMessage(String status) {
    StaticJsonDocument<256> doc;

    // Limpar o doc antes de montar
    doc.clear();

    // Monta o JSON definido no projeto SkillUp
    doc["ID_Grupo"] = ID;
    doc["Modulo"] = moduleID;
    doc["IP"] = WiFi.localIP().toString();
    doc["MAC"] = WiFi.macAddress();
    doc["Status"] = status; // "FOCUS_START" ou "FOCUS_END"

    char buffer[256];
    serializeJson(doc, buffer);

    // Publica no tópico
    String topico = String("FIAP/SkillUp/Focus/") + ID;
    client.publish(topico.c_str(), buffer);

    Serial.print("Mensagem enviada [");
    Serial.print(topico);
    Serial.print("]: ");
    Serial.println(buffer);

    // Pisca o LED para feedback
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
}

// --- SETUP: Executa uma vez ---
void setup() {
    Serial.begin(115200);
    
    // Configura os pinos
    pinMode(LED_PIN, OUTPUT);
    // Configura o botão: INPUT_PULLUP usa o resistor interno do ESP32.
    pinMode(BTN_PIN, INPUT_PULLUP);

    setup_wifi(); // Wi-Fi deve vir primeiro

    // Cria o ID Unico para o MQTT
    String mac = WiFi.macAddress();
    uniqueClientID = String(moduleID) + "-" + mac;
    
    Serial.print("ID de Cliente MQTT Unico: ");
    Serial.println(uniqueClientID);

    // Aponta o cliente para o servidor (broker)
    client.setServer(mqtt_server, mqtt_port);
}

// --- LOOP: Executa continuamente ---
void loop() {
    // Garante que o cliente MQTT esteja conectado
    if (!client.connected()) {
        reconnect_mqtt();
    }
    client.loop(); // Mantem a conexao MQTT ativa

    // --- Lógica do Botão ---
    // Leitura do botão (LOW = pressionado, por causa do PULLUP)
    if (digitalRead(BTN_PIN) == LOW) {
        
        Serial.println("Botão pressionado!");
        
        // Controla o estado da sessão
        if (isFocusSession) {
            // Se estava focado, termina a sessão
            sendFocusMessage("FOCUS_END");
            isFocusSession = false;
        } else {
            // Se não estava focado, inicia a sessão
            sendFocusMessage("FOCUS_START");
            isFocusSession = true;
        }

        // Delay para "Debounce"
        // Evita leitura múltipla de um único clique
        delay(1000); 
    }
}