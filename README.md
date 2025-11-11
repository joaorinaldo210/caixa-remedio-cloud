# Projeto IoT / Artigo_CaixaRemedios_Inteligente

Desenvolvido por: **João Rinaldo**, **Bruno Otávio Ramos** e **Gabriel Matheus Soares de Carvalho**  
Faculdade Presbiteriana Mackenzie  
Curso: **Análise e Desenvolvimento de Sistemas**  
Atividade: **AC4 — Aplicando conhecimento 4 (IoT)**  
Ano: **2025**

Repositório oficial: https://github.com/joaorinaldo210/caixa-remedio-cloud  
Simulação Wokwi: https://wokwi.com/projects/446834998824109057

---

## i) Descrição do funcionamento e uso

O projeto **Caixa Remédio Nuvem** é um sistema IoT desenvolvido para auxiliar no controle de medicação.  
O dispositivo, baseado no **ESP32**, possui **LEDs indicativos**, **botões de confirmação de dose**, **sensor LDR para detecção de tampa aberta** e **buzzer de alerta**.  
Ele se conecta via **Wi-Fi** a um **broker MQTT (HiveMQ)**, enviando e recebendo dados em tempo real.

### Funcionamento básico
- Cada botão representa um período do dia (manhã, tarde, noite, extra).  
- Ao pressionar um botão, o LED correspondente acende e o ESP32 publica uma mensagem MQTT com o registro da dose tomada.  
- O sensor LDR detecta se a tampa foi aberta e publica o status no broker.  
- O buzzer emite um som de alerta quando o comando MQTT `buzzer` é recebido ou quando a tampa é aberta.  
- Todos os eventos são enviados em formato JSON com data e hora sincronizadas via NTP.

📸 **Exemplo visual do projeto (simulação Wokwi):**  
![Foto do Projeto](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/assets/foto_projeto.jpg)


---

## ii) Software desenvolvido e documentação de código

O software foi desenvolvido em **C++ (Arduino)** para o microcontrolador **ESP32 DevKit V4**.  
O código principal está localizado em `src/esboco.ino`, totalmente comentado e modularizado.

### Estrutura e principais seções
- **Conexão Wi-Fi:** autenticação e verificação de rede.  
- **Conexão MQTT:** publicação e assinatura de tópicos via broker HiveMQ.  
- **Callback MQTT:** tratamento de comandos recebidos (buzzer, botões e tampa).  
- **Publicação JSON:** envio de mensagens estruturadas utilizando `ArduinoJson`.  
- **Controle de hardware:** LEDs, botões e buzzer com lógica de debounce.  
- **Sincronização de horário:** via NTP (`pool.ntp.org`).

### Arquivos principais do diretório `src/`
| Arquivo | Descrição |
|---|---|
| `esboco.ino` | Código-fonte principal do ESP32 |
| `diagrama.json` | Circuito completo da simulação no Wokwi |
| `bibliotecas.txt` | Lista de bibliotecas utilizadas |
| `config.h` | Configurações opcionais (SSID, broker, pinos) |

**Trecho representativo do código:**

```cpp
void callback(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
  if (t.endsWith("/comando/buzzer")) {
    tone(BUZZER, 1500); delay(300); noTone(BUZZER);
  }
  if (t.endsWith("/comando/manha")) handleButtonPress(0);
}
```

**Bibliotecas utilizadas**

- WiFi.h  
- PubSubClient.h  
- ArduinoJson.h  
- time.h

---

## iii) Descrição do hardware utilizado

### Componentes principais

### 🔹 Componentes do Projeto

**ESP32:**
![ESP32](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/assets/esp32.png)

**Sensor LDR:**
![Sensor LDR](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/assets/sensor_ldr.png)

**Buzzer:**
![Buzzer](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/assets/buzzer.png)

**LEDs:**
![LEDs](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/assets/leds.png)

**Botões:**

![Botões](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/assets/botoes.png)


| Componente | Função | Quantidade |
|---|---:|---:|
| ESP32 DevKit V4 | Microcontrolador principal com Wi-Fi | 1 |
| LEDs (vermelho, verde, azul, extra) | Indicação de períodos do dia | 4 |
| Botões (push-button) | Confirmação de dose | 4 |
| Buzzer | Alerta sonoro | 1 |
| Sensor LDR | Detecção de tampa aberta | 1 |
| Resistores de 220 kΩ | Proteção e limitação de corrente | 3 |

**Plataforma:** Wokwi (simulação online de hardware ESP32)

### Pinagem resumida
| Função | GPIO |
|---|---:|
| LED Manhã | 12 |
| LED Tarde | 13 |
| LED Noite | 14 |
| LED Extra | 15 |
| Botão Manhã | 16 |
| Botão Tarde | 17 |
| Botão Noite | 18 |
| Botão Extra | 19 |
| Buzzer | 21 |
| Sensor LDR | 34 |

### Diagrama do circuito (extraído do Wokwi)
![Diagrama do Circuito](docs/hardware.png)

---

## iv) Documentação das interfaces, protocolos e módulos de comunicação

O sistema utiliza o **protocolo MQTT (Message Queuing Telemetry Transport)** para comunicação entre o ESP32 e a nuvem.  
As mensagens seguem o formato **JSON** e são trocadas via **broker público HiveMQ**.

### Parâmetros de conexão
- **Broker:** broker.hivemq.com  
- **Porta:** 1883  
- **Client ID:** pillbox01  
- **QoS:** 0 (entrega simples)  
- **Tópico base:** pillbox/pillbox01/

### Tópicos MQTT utilizados
| Direção | Tópico | Descrição |
|---|---|---|
| → Publicação | pillbox/pillbox01/status | Status online/offline do dispositivo |
| → Publicação | pillbox/pillbox01/dose | Registro de dose confirmada |
| → Publicação | pillbox/pillbox01/tampa | Estado da tampa (aberta/fechada) |
| ← Assinatura | pillbox/pillbox01/comando/# | Comandos recebidos (buzzer, botões, etc.) |

**Exemplo de payload MQTT:**

```json
{
  "dispositivo": "pillbox01",
  "evento": "dose",
  "horario": "manha",
  "status": "tomada",
  "timestamp": "2025-11-11T20:14:53Z"
}
```
Resultados e medições

Durante os testes, foram realizadas medições de tempo de resposta entre os sensores, atuadores e a comunicação MQTT.

**Tabela 1 – Latências agregadas (Sensor→MQTT e Comando→Atuador) do protótipo (ms).**  
![Tabela de Latências](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/assets/tabela_latencias.png)

**Gráfico de desempenho das medições:**  
![Gráfico de Medição](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/testes/graficomedicao.png)

Esses testes demonstraram um desempenho consistente, com tempos médios adequados para aplicações IoT em ambiente doméstico.

---

## vi) Vídeo de apresentação

Apresentação completa do projeto e funcionamento do protocolo MQTT (PENDENTE):  
 [Assista ao vídeo no YouTube](COLE_AQUI_O_LINK_DO_VÍDEO)

---

## vii) Testes e comunicação MQTT

Durante a simulação e execução do firmware, foi verificada a comunicação bidirecional com o broker HiveMQ.

**Logs e capturas de teste:**  
![Teste MQTT](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/testes/testecomunicacaoMQTT.png)  

---

## Estrutura do Repositório

- `README.md` → documentação principal do projeto  
- `src/` → código-fonte, diagrama e bibliotecas  
- `docs/` → relatórios, PDFs e imagens de documentação  
- `assets/` → imagens e recursos visuais do projeto  
- `testes/` → logs e resultados de simulação  
- `wokwi/` → arquivos da simulação online

---

## Conclusão

O projeto **Caixa Remédio Nuvem** demonstra a aplicação prática da Internet das Coisas (IoT) na área da saúde, utilizando sensores, atuadores e comunicação MQTT.  
O sistema foi desenvolvido e testado integralmente no ambiente **Wokwi**, com integração em tempo real ao **HiveMQ**, comprovando sua viabilidade técnica e funcional.  
A documentação, código e resultados atendem plenamente aos critérios propostos na atividade AC4.

---

## Referência

- Documentação do Wokwi — https://docs.wokwi.com/  
- Broker Público HiveMQ — https://www.hivemq.com/demos/websocket-client/  
- Biblioteca PubSubClient — https://pubsubclient.knolleary.net/  
- ArduinoJson — https://arduinojson.org/
