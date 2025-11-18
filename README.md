# Inteli – Instituto de Tecnologia e Liderança

<p align="center">
<a href="https://www.inteli.edu.br/">
<img src="assets/inteli.png" alt="Inteli - Instituto de Tecnologia e Liderança" width="40%">
</a>
</p>

# Semáforo Inteligente

## Grupo do desconforto

## 👨‍🎓 Integrantes

- <a href="https://www.linkedin.com/in/christian-gandra/">Christian Gandra</a>
- <a href="https://www.linkedin.com/in/giovanna-neves-rodrigues/">Giovanna Neves</a>
- <a href="https://www.linkedin.com/in/isabel-montenegro01/">Isabel Montenegro</a>
- <a href="https://www.linkedin.com/in/paulo-henrique0601/">Paulo Henrique</a>
- <a href="https://www.linkedin.com/in/samuel-vono/">Samuel Vono</a>
- <a href="https://www.linkedin.com/in/tobias-viana/">Tobias Viana</a>
- <a href="https://www.linkedin.com/in/vitor-lopes-91763b34a/">Vitor Lopes</a> 

## 👩‍🏫 Professora

- <a href="https://www.linkedin.com/in/crishna-irion-7b5aa311/">Crishna Irion</a>


## 📜 Sobre o Projeto

&nbsp; O projeto consiste na criação de um **semáforo inteligente**, capaz de:

- Detectar veículos com um sensor de luminosidade (LDR)  
- Entrar automaticamente em modo noturno quando o ambiente estiver escuro  
- Comunicação entre dois semáforos usando ESP32  
- Exibir dados de luminosidade e permitir ajustes via interface online (Ubidots)

&nbsp; O trabalho simula uma solução de **gestão urbana inteligente**, explorando automação, IoT e comunicação em rede.


## 🎥 Demonstração

👉 **Link do vídeo de demonstração:** *[Adicionar link aqui]*
 
## 📁 Estrutura de Pastas

- **assets/**  
  Imagens, vídeos, diagramas e demais arquivos gráficos do projeto.

- **codigos/**  
  Arquivos de firmware/programação dos ESP32.

- **document/**  
  Documentação formal do projeto.  

- **README.md**  
  Documento de apresentação geral do projeto.


## 🚦 Funcionalidades

- Detecção de luminosidade por LDR  
- Alteração automática para modo noturno  
- Dois semáforos funcionando simultaneamente  
- Comunicação via plataforma na nuvem (Ubidots)  
- Dashboard com dados do sensor e controles remotos  

### Funcionalidades que serão detalhadas no `documentacao.md`

- Lógica exata de mudança de estado  
- Condições específicas para modo noturno  
- Comunicação exata entre os dois ESP32  
- Como os semáforos sincronizam seus tempos  
- Layout final da interface online  


## 🛠️ Tecnologias Utilizadas

* **ESP32**
* **MQTT (Ubidots)**
* **C/C++ (Arduino Framework)**
* **Protoboard e componentes eletrônicos**
* **Sensores LDR**
* **LEDs RGB**
* **Dashboard Ubidots para visualização e controle remoto**


## 📌 Como Executar

1. Instale o **Arduino IDE** ou **PlatformIO**
2. Instale as bibliotecas:

   ```
   WiFi.h
   PubSubClient.h
   ```
3. Edite o arquivo do ESP32 e coloque:

   * Nome da sua rede Wi-Fi
   * Senha do Wi-Fi
   * Token da sua conta Ubidots
4. Faça o upload dos códigos:

   * `/codigos/semaforo1.ino`
   * `/codigos/semaforo2.ino`
5. Acesse a dashboard da Ubidots para visualizar os dados.

## 📊 Variáveis da Dashboard (Ubidots)

### Semáforo 1

* `ldr-carro-s1`
* `carro-esperando-s1`
* `estado-texto`

### Semáforo 2

* `ldr-ambiente-s2`
* `ldr-carro-s2`
* `carro-esperando-s2`
* `modo-noturno-manual`
* `estado-texto`

**Semáforo funcionando:**  
[Vídeo do semáforo funcionando](./assets/semaforo_inteligente.mp4)

## 📄 Licença

Uso acadêmico – Inteli (2025).


