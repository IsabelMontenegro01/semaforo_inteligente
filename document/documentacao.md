# Documentação — Semáforo Inteligente

## Sumário

<details>
  <summary><a href="#c1">1. Introdução</a></summary>
</details>

<details>
  <summary><a href="#c2">2. Objetivo</a></summary>
</details>

<details>
  <summary><a href="#c3">3. Desenvolvimento</a></summary>
  <ul>
    <li><a href="#c3.1">3.1. Montagem Física</a></li>
    <li><a href="#c3.2">3.2. Funcionamento do Sensor LDR</a></li>
    <li><a href="#c3.3">3.3. Lógica de Funcionamento do Semáforo</a></li>
    <li><a href="#c3.4">3.4. Comunicação entre os Dois Semáforos</a></li>
    <li><a href="#c3.5">3.5. Interface Online</a></li>
  </ul>
</details>

<details>
  <summary><a href="#c4">4. Resultados</a></summary>
</details>

<details>
  <summary><a href="#c5">5. Conclusão</a></summary>
</details>


# <a name="c1"></a>1. Introdução

<div align="center">
<sub>Figura 1 – Diagrama geral do sistema</sub>  
<img src="../assets/figura_sistema.png" width="100%">  
<sup>Fonte: Material produzido pelos autores (2025)</sup>
</div>

&nbsp; O crescimento das cidades requer soluções modernas capazes de otimizar o tráfego urbano. Nesse contexto, tecnologias inteligentes, incluindo sensores, microcontroladores e sistemas interconectados, tornam-se fundamentais para melhorar o fluxo de veículos e reduzir congestionamentos.

&nbsp; Este projeto propõe a implementação de um **semáforo inteligente**, capaz de detectar a luminosidade ambiente, identificar a presença de veículos e alternar automaticamente entre modos de operação. Além disso, dois semáforos se comunicam por meio de uma plataforma IoT, simulando um sistema avançado de infraestrutura urbana.


# <a name="c2"></a>2. Objetivo

&nbsp; O objetivo do projeto é desenvolver um sistema funcional composto por dois semáforos controlados por ESP32, conectados a sensores LDR e integrados a uma interface online que permita visualizar dados e ajustar comportamentos.

**Objetivos específicos:**

* Implementar leitura de luminosidade via LDR;
* Alterar o comportamento do semáforo com base na luz ambiente (modo noturno);
* Construir dois semáforos físicos com LEDs;
* Integrar ambos à internet por meio do ESP32;
* Enviar dados para uma plataforma IoT (ex.: Ubidots);
* Permitir o controle e monitoramento remoto do sistema;
* Simular comunicação e sincronização entre dois semáforos.


# <a name="c3"></a>3. Desenvolvimento

## <a name="c3.1"></a>3.1. Montagem Física

<div align="center">
<sub>Figura 2 – Montagem física do protótipo</sub>  
<img src="../assets/montagem_fisica.png" width="100%">  
<sup>Fonte: Material produzido pelos autores (2025)</sup>
</div>

### Componentes utilizados

* ESP32 (1 por semáforo)
* LDR
* LEDs vermelho, amarelo, verde
* Protoboard
* Resistores
* Jumpers

### Descrição da montagem

🟦 *Adicionar depois: como os LEDs foram organizados, conexões dos pinos, esquema de ligação do LDR, valores dos resistores, etc.*


## <a name="c3.2"></a>3.2. Funcionamento do Sensor LDR

&nbsp; O LDR (Light Dependent Resistor) é um sensor que varia sua resistência conforme a intensidade da luz. No projeto, ele é responsável por:

* Detectar a presença simulada de um veículo (alteração brusca de luminosidade)
* Indicar mudança para modo noturno quando a luz ambiente diminui

🟦 *Adicionar valores e thresholds utilizados no código*
🟦 *Adicionar fórmula ou lógica utilizada para conversão da leitura analógica*


## <a name="c3.3"></a>3.3. Lógica de Funcionamento do Semáforo

🟦 *Descrição do ciclo normal (verde → amarelo → vermelho)*
🟦 *Descrição do modo noturno (ex.: amarelo piscando)*
🟦 *Condições de entrada e saída do modo noturno*
🟦 *Como o LDR altera o comportamento em tempo real*


## <a name="c3.4"></a>3.4. Comunicação entre os Dois Semáforos

<div align="center">
<sub>Figura 3 – Comunicação entre semáforos via Ubidots</sub>  
<img src="../assets/comunicacao_ubidots.png" width="100%">  
<sup>Fonte: Material produzido pelos autores (2025)</sup>
</div>

&nbsp; Os dois semáforos comunicam-se por meio de uma plataforma IoT. Cada ESP32 envia e recebe dados da nuvem, permitindo sincronização e controle externo.

🟦 *Adicionar: variáveis utilizadas, endpoints, tópicos, lógica de sincronização*


## <a name="c3.5"></a>3.5. Interface Online

&nbsp; A interface online permite:

* Visualizar dados captados pelo LDR
* Ativar o modo noturno remotamente
* Ajustar parâmetros do semáforo

🟦 *Adicionar screenshots da interface*
🟦 *Adicionar link da dashboard*
🟦 *Descrever variáveis enviadas e recebidas pelo ESP32*


# <a name="c4"></a>4. Resultados

🟦 *Adicionar resultados obtidos com testes reais*
🟦 *Adicionar observações sobre estabilidade, precisão do LDR, funcionamento do modo noturno*
🟦 *Adicionar tabelas/gráficos se houver*


# <a name="c5"></a>5. Conclusão

&nbsp; O desenvolvimento do semáforo inteligente permitiu aplicar conceitos de eletrônica, programação de microcontroladores, IoT e sistemas distribuídos. A construção do protótipo demonstrou como sensores simples, aliados a plataformas de nuvem, podem criar soluções escaláveis para problemas reais do cotidiano urbano.

&nbsp; O projeto evidencia o potencial da automação e da conectividade em redes de dispositivos, reforçando a importância de soluções inteligentes para o gerenciamento de tráfego e melhoria da mobilidade nas cidades.