# Projeto IoT / Artigo_CaixaRemedios_Inteligente
Desenvolvido por: João Rinaldo, Bruno Otávio Ramos, Gabriel Matheus Soares de Carvalho
Faculdade Presbiteriana Mackenzie
Curso: Análise e Desenvolvimento de Sistemas
Atividade: AC4 — Aplicando conhecimento 4 (IoT)
Ano: 2025

## 1. Descrição do funcionamento e uso
A Caixa Remédio Nuvem é um protótipo IoT baseado em ESP32 que registra confirmações de dose, detecta abertura de tampa e comunica eventos via MQTT para o broker HiveMQ. O sistema possui quatro botões (manhã, tarde, noite, extra), quatro LEDs, um buzzer e um sensor LDR.

Para reproduzir a simulação no Wokwi:
1. Abra: https://wokwi.com/projects/446834998824109057
2. Clique em "Start Simulation".
3. Abra o Serial Monitor para visualizar logs de conexão e eventos.
4. No HiveMQ WebSocket Client (https://www.hivemq.com/demos/websocket-client/), conecte-se com host `broker.hivemq.com`, port `8000`, sem SSL, e assine `pillbox/#`.

## 2. Software desenvolvido e documentação de código
O código-fonte principal está em `src/esboco.ino`. O firmware implementa:
- Conexão Wi-Fi (WiFi.h)
- Cliente MQTT (PubSubClient)
- Formato JSON para mensagens (ArduinoJson)
- NTP para timestamps (configTime)
- Leitura de botões com debounce e publicação de confirmações
- Detecção de tampa via LDR e publicação de status

O arquivo `src/bibliotecas.txt` lista as dependências necessárias.

## 3. Descrição do hardware utilizado
Tabela de componentes e mapeamento de pinos:

| Componente | Função | Pino ESP32 |
|---|---:|---:|
| ESP32 DevKitC V4 | Controlador principal | - |
| LED Manhã | Indica confirmação manhã | GPIO 12 |
| LED Tarde | Indica confirmação tarde | GPIO 13 |
| LED Noite | Indica confirmação noite | GPIO 14 |
| LED Extra | Indica confirmação extra | GPIO 15 |
| Botão Manhã | Confirmação manhã (INPUT_PULLUP) | GPIO 16 |
| Botão Tarde | Confirmação tarde (INPUT_PULLUP) | GPIO 17 |
| Botão Noite | Confirmação noite (INPUT_PULLUP) | GPIO 18 |
| Botão Extra | Confirmação extra (INPUT_PULLUP) | GPIO 19 |
| Buzzer ativo | Alerta sonoro | GPIO 21 |
| Sensor LDR | Detecta abertura da tampa (ADC) | GPIO 34 |

O arquivo `src/diagrama.json` contém o circuito Wokwi correspondente.

## 4. Documentação das interfaces, protocolos e módulos de comunicação
- Wi-Fi: biblioteca `WiFi.h`, rede `Wokwi-GUEST` (simulação)
- MQTT: biblioteca `PubSubClient`, broker `broker.hivemq.com`, porta 1883 (MQTT/TCP), tópicos usados:
  - `pillbox/pillbox01/status`
  - `pillbox/pillbox01/dose`
  - `pillbox/pillbox01/tampa`
  - `pillbox/pillbox01/buzzer`
  - `pillbox/pillbox01/sensor`
  - `pillbox/pillbox01/comando/#` (assinatura)
- Mensagens em JSON, timestamps em ISO 8601 (UTC)
- NTP: `pool.ntp.org` para sincronização de horário
- Fluxo: dispositivo publica eventos (dose/tampa/status) e assina comando para atuar remotamente

## Estrutura de pastas
- `src/` : código-fonte e diagrama Wokwi
- `docs/`: documentos acadêmicos e especificações
- `assets/`: imagens e mídias (opcional)
- `testes/`: logs de teste e evidências
- `wokwi/`: export do projeto Wokwi e instruções

## Licença
Coloque aqui o tipo de licença desejada (ex.: MIT).

## Contato
João Rinaldo — https://github.com/joaorinaldo210/caixa-remedio-cloud
