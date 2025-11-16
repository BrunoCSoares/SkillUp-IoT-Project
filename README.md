# SkillUp - Medidor de Foco (IoT) - Global Solution 2025

> 🌟 PROJETO GLOBAL SOLUTION 2025 - SkillUp

Este repositório contém a implementação da solução de IoT para o projeto **SkillUp**. O projeto foi baseado no template Q1 e foi adaptado para atender aos requisitos da nossa solução.

## Estrutura do projeto

Raiz do repositório:

```
q1/                 # Projeto SkillUp (MQTT + Node-RED)
  ├─ src/
  │   └─ main.cpp   # Código do Medidor de Foco
  └─ diagram.json   # Diagrama Wokwi com botão
q2/                 # Projeto ThingSpeak original (não utilizado)
  └─ src/
      └─ main.cpp
platformio.ini      # Configuração PlatformIO
README.md           # Esta documentação
```

---

## Pasta `q1` — Medidor de Foco (MQTT + Node-RED)

### Descrição

O projeto em `q1` implementa um "Medidor de Foco" com as seguintes características:

- **Conectividade:** conecta um ESP32 à rede Wi-Fi (Wokwi-GUEST na simulação) e a um broker MQTT público (HiveMQ).
- **Client ID único:** o ID do cliente MQTT é gerado a partir do MAC address para evitar colisões.
- **Entrada:** um **botão** simulado no Wokwi (pino GPIO4 / D4) alterna o estado entre `FOCUS_START` e `FOCUS_END` a cada pressão.
- **Envio:** é enviada uma mensagem JSON ao broker somente quando o botão é pressionado.
- **Feedback local:** o LED onboard pisca a cada envio.

### Identificação enviada

O dispositivo envia, entre outros, os seguintes campos:

- `ID_Grupo`: RMs dos integrantes (ex.: `559250_560027_560393`)
- `Modulo`: identificador do módulo (ex.: `FocusTimer_01`)
- `IP`: endereço IP local
- `MAC`: endereço MAC
- `Status`: `FOCUS_START` ou `FOCUS_END`

### Broker utilizado

Para a simulação no Wokwi usamos o broker público HiveMQ, pois o broker da FIAP bloqueia conexões externas na simulação.

- **Endereço:** `broker.hivemq.com`
- **Porta:** `1883`
- **Autenticação:** nenhuma (conexão anônima)

### Formato do JSON enviado

```json
{
  "ID_Grupo": "559250_560027_560393",
  "Modulo": "FocusTimer_01",
  "IP": "xxx.xxx.xxx.xxx",
  "MAC": "XX:XX:XX:XX:XX:XX",
  "Status": "FOCUS_START"
}
```

### Código de configuração (em `q1/src/main.cpp`)

Exemplo de variáveis configuradas no código:

```cpp
const char* ID = "559250_560027_560393"; // RMs
const char* moduleID = "FocusTimer_01";  // Nome do ESP32
```

## Pasta `q2` — Integração com ThingSpeak (não utilizada)

A pasta `q2` contém o projeto original do template que demonstra integração com ThingSpeak. Não foi utilizada na solução final.

## Configuração do ambiente

Pré-requisitos:

- Visual Studio Code
- Extensão PlatformIO IDE
- Git
- Node.js (para rodar o Node-RED localmente)
- Conta Wokwi (para simulação)

Instalação rápida (exemplo):

```powershell
git clone https://github.com/BrunoCSoares/SkillUp-IoT-Project
cd SkillUp-IoT-Project
```

Configuração do Node-RED (opcional, local):

```powershell
npm install -g --unsafe-perm node-red
node-red
```

No Node-RED instale o dashboard via Manage palette: procure por `@flowfuse/node-red-dashboard`.

Observação: o JSON com o modelo (fluxos) do Node-RED pronto está em `2TDS-GS2025\\q1\\node-red\\flows.json`. Importe esse arquivo no Node-RED (Menu > Import > Clipboard/File) para carregar o fluxo predefinido.

## Dependências

O projeto usa (gerenciadas pelo PlatformIO):

- `ArduinoJson` (JSON)
- `PubSubClient` (MQTT)

> Observação: as bibliotecas de DHT e do potenciômetro foram removidas desta versão do projeto.

## Diagrama Wokwi

O arquivo `q1/diagram.json` foi modificado para incluir um `wokwi-pushbutton` conectado ao pino GPIO4 (D4) e ao GND.

## Monitor Serial

O monitor serial (115200 baud) exibe mensagens como:

- "Conectando ao Wi-Fi..."
- "Wi-Fi conectado!"
- Exemplo de saída:

```
IP: 10.13.37.2
MAC Address: 24:0A:C4:00:01:10
ID de Cliente MQTT Único: FocusTimer_01-24:0A:C4:00:01:10
Conectando ao Broker MQTT...
Conectado ao Broker!
```

Quando o botão é pressionado, o serial mostra a mensagem enviada com o JSON correspondente (`FOCUS_START` / `FOCUS_END`).

## Depuração com MQTT Explorer

Para inspecionar tópicos MQTT recomendamos o MQTT Explorer:

- Site: https://mqtt-explorer.com/
- Host: `broker.hivemq.com`
- Porta: `1883` (sem usuário/senha)
- Procure pelo tópico: `FIAP/SkillUp/Focus/`

---

Se quiser, posso também:

- Rodar uma verificação rápida do Markdown localmente.
- Abrir o `q1/src/main.cpp` e aplicar pequenas melhorias de comentários.

