# Caixa de Remédios Inteligente (PillBox IoT)

## Descrição do Projeto
Este projeto apresenta uma **Caixa de Remédios Inteligente** (PillBox IoT) desenvolvida para auxiliar pacientes na adesão correta a tratamentos médicos.  
O sistema utiliza o **microcontrolador ESP32** para controlar LEDs, botões, sensor LDR e buzzer, com **comunicação MQTT** para enviar informações sobre o uso do medicamento para cuidadores ou sistemas remotos.  

A solução busca **promover autonomia e segurança** na administração de remédios, além de contribuir para o **ODS 3 da ONU (Saúde e Bem-Estar)**.  

---

## Funcionalidades
- Lembrar o usuário dos horários de medicação  
- Indicar com LEDs o horário do remédio (manhã, tarde, noite e extra)  
- Registrar a confirmação da dose por botão  
- Detectar a abertura da tampa por sensor LDR  
- Emitir alertas sonoros com buzzer  
- Enviar e receber mensagens via **MQTT (HiveMQ Public Broker)**  
- Funcionar localmente e com monitoramento remoto  

---

## Componentes Utilizados

| Componente | Função |
|-------------|--------|
| **ESP32** | Microcontrolador principal com Wi-Fi integrado |
| **4 LEDs (5mm)** | Indicadores visuais dos horários (manhã, tarde, noite, extra) |
| **4 Botões de pressão** | Confirmação da dose tomada |
| **LDR (Sensor de Luz)** | Detecta a abertura da tampa da caixa |
| **Buzzer ativo KY-012** | Emite alerta sonoro de lembrete/esquecimento |
| **Resistores 220–330Ω** | Proteção dos LEDs |
| **MQTT Broker (HiveMQ)** | Comunicação entre ESP32 e nuvem |

---

## Arquitetura e Comunicação

O sistema é composto por três camadas principais:

| Camada | Função |
|---------|--------|
| **Dispositivo IoT (ESP32)** | Leitura de sensores e controle de atuadores |
| **Broker MQTT (HiveMQ)** | Intermediação da comunicação entre dispositivos |
| **Aplicação/Dashboard** | Visualização e controle remoto |

### Fluxo de Operação
1. O ESP32 conecta-se à rede Wi-Fi.  
2. Estabelece conexão com o broker público **HiveMQ (broker.hivemq.com, porta 1883)**.  
3. Publica o status “online”.  
4. Aguarda interações via botões ou mensagens MQTT.  
5. Acende LEDs, aciona buzzer e registra eventos.  
6. Envia confirmações e alertas ao broker via tópicos MQTT.  

### Estrutura dos Tópicos MQTT

| Ação | Tópico | Descrição |
|------|---------|-----------|
| Confirmar dose manhã | `pillbox/pillbox01/comando/manha` | Registra dose confirmada |
| Confirmar dose tarde | `pillbox/pillbox01/comando/tarde` | Registra dose confirmada |
| Confirmar dose noite | `pillbox/pillbox01/comando/noite` | Registra dose confirmada |
| Dose extra | `pillbox/pillbox01/comando/extra` | Dose adicional |
| Tocar buzzer | `pillbox/pillbox01/comando/buzzer` | Ativa o alerta sonoro |
| Status | `pillbox/pillbox01/status` | Indica se o dispositivo está online |
| Retorno | `pillbox/pillbox01/dose/.../status` | Confirma o envio de mensagens |

---

## Especificações Técnicas

- **Microcontrolador:** ESP32 DevKit V1  
- **Protocolo de comunicação:** MQTT (QoS 1, retained messages e LWT configurado)  
- **Broker:** `broker.hivemq.com` (porta 1883)  
- **Linguagem de programação:** C++ (Arduino IDE)  
- **Simulação:** [Wokwi – Projeto Online](https://wokwi.com/projects/446834998824109057)  

---

## Resultados

- O sistema acende o LED correspondente ao horário do remédio.  
- O buzzer emite som quando o horário chega.  
- O botão registra a dose e envia a confirmação via MQTT.  
- O LDR detecta abertura da tampa e gera um evento adicional de confirmação.  
- Testes mostraram **resposta média de 120 ms** entre sensor e publicação MQTT e **80 ms** entre comando MQTT e ação do atuador.

 **Imagem do protótipo em funcionamento:**
 ![Protótipo](https://github.com/joaorinaldo210/caixa-remedio-cloud/blob/main/Captura%20de%20tela%202025-11-07%20163050.png)

 **Vídeo de demonstração (não listado no YouTube):**  
 [Link do vídeo no YouTube](COLE_AQUI_SEU_LINK)

---

## Estrutura do Repositório

```
📦 caixa-remedio-cloud
 ┣ 📜 diagrm.json
 ┣ 📜 libraries.txt
 ┣ 📜 sketch.ino
 ┣ 📜 wokwi-project.txt
 ┣ 📜 Captura de tela 2025-11-07
 ┣ 📜 README.md
 ┣ 📜 artigo_CaixaRemedios_Inteligente.pdf
 ┣ 📜 projeto.pdf
```

---

## Referências
- HiveMQ. *MQTT Essentials – Part 1: Introducing MQTT.* (2024).  
- Espressif Systems. *ESP32 Datasheet.* (2024).  
- IBM Developer. *Why MQTT is a good fit for IoT.* (2021).  
- WHO. *Adherence to long-term therapies: evidence for action.* (2003).  
- Wokwi. *Online Arduino and ESP32 Simulator.* (2025).  

---

## Autores
**Bruno Otávio Ramos** • **Gabriel Matheus Soares de Carvalho** • **João Rinaldo França Neris**  
Universidade Presbiteriana Mackenzie – Faculdade de Computação e Informática  
