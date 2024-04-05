<h1 align="center">💻 ESP BT Clássico - UART (Antena UHF) | ESP-IDF 💻</h1>
<p align="center">
<a>
    <img alt="GitHub last commit" src="https://img.shields.io/github/repo-size/LucasEstevo/Simple_CRC">
</a>
<a>
    <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/LucasEstevo/Simple_CRC">
</a>
<a>
    <img alt="GitHub last commit" src="https://img.shields.io/github/license/LucasEstevo/Simple_CRC">
</a>
<a>
    <img alt="GitHub last commit" src="https://img.shields.io/github/stars/LucasEstevo?style=social">
</a>
<a>
    <img alt="" src="https://img.shields.io/badge/feito%20por-Lucas Estevo-%237519C1">
<a>
</p>
<br/>

<h4 align="center"> 
	🚧 Projeto Concluído 🐱‍👤 🚧
</h4>

---

## ✨Pré-requisitos
Antes de começar, você vai precisar ter instalado em sua máquina as seguintes ferramentas:
[VSCode](https://code.visualstudio.com/), [Git](https://git-scm.com), [Python3](https://www.python.org/downloads/) e [CMake](https://cmake.org/).
Após a instalação de todos estes softwares abra o VSCode e instale a extensão **Espressif IDF**, agora o VSCode estará pronto para executar o código.
<br/>

---

## 📝 Sobre o Projeto
Este projeto faz a ponte entre dois dispositivos através da comunicação UART <-> Bluetooth clássico. </br>
Toda informação recebida pela uart será transmitida para o bluetooth.</br>
Para conexão via bluetooth é necessário um "pin code" para realizar o pareamento e ele está pré definido como "0000"

<br/>

---

## 🐱‍💻 Sobre o Código 

<br/>

```C
#define BYTES_PER_LINE 26 // Quantidade de Bytes que váriavel hex_buffer irá armazenar

static const int TX_BUF_SIZE_DEBUG = 256; //Quantidade máxima de Bits para escrita na UART Debug
static const int RX_BUF_SIZE_DEBUG = 256; //Quantidade máxima de Bits para leitura na UART Debug
static const int RX_CARD_BUF_SIZE = 26; //Configura o Tamanho de bytes do Cartão, fornecidos pela antena
static const int RX_BUF_SIZE = 78; //Tamanho do Buffer da UART de Comunicação com o outro equipamento
```

<br/>

``` C
//brief: Inicia o processo de coleta de dados via UART1
void iniciar()

//brief: Finaliza o processo de coleta de dados via UART1  
void finalizar()
```

<br/>

``` C
/*! \brief: Coleta os dados recebidos na UART1 e faz a conversão para Hex Scapce
 *  \param: buffer = Mensagem Advinda da UART1 & buff_len = Tamanho da Mensagem
 */ 
void buffer_hex(const void *buffer, uint16_t buff_len)
```
<br/>

``` C
/*! \brief: Faz o controle de inicio de finalização das leitura pela Antena
 *  \param: int data = 1 (Inicia Leitura) | 2 (Finaliza Leitura) 
 */ 
void sendData(int data)
```

<br/>

``` C
//brief: Responsável por iniciar a UART0 onde ocorre os eventos Debug
void uart0_init();

//brief: Responsável por iniciar a UART1 onde ocorre a comunicação com a Antena
void uart1_init();
```

``` C
/*! \brief: Faz a impressão de mensagens na UART0 - Usado para monitorar o funcionamento
 */
void uart_write_debug(char *data, ...)
```

``` C
/*! \brief: Fica fazendo a varredura da UART1 e sempre que identificar um código de cartão, vai fazer o tratamento e enviar somente o código do cartão por bluetooth.
 */
void vTaskData(void *pvParameters)
```


---

## ⚙ Configurações
CPU frequency - 160 MHz;

Flash size - 2 MB;

Timeout for RTC watchdog(ms) - 9000;

Parition Table - Single factory app, no OTA;

Bluetooth controller
* BR/EDR Only ;
* Core 0 (PRO CPU);
* SPP;
* Pin code 0000;     
                     
UART0
* baudrate 115200;
* data bits 8;
* parity disable;
* stop bits 1;
     
UART1
* baudrate 115200;
* data bits 8;
* parity disable;
* stop bits 1;

> 📜 Os pinos GPIO da UART 0 e 1 estão definidas em board_pins.h 📜

<br/>

---
## Documentação de Apoio - Espressif
[UART](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html#_CPPv418uart_driver_delete11uart_port_t)

[Bluetooth Classic](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_spp.html)

[Conversão Hex Space](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/log.html)

[Datasheet ESP32-Wrover B](https://br.mouser.com/datasheet/2/891/esp32_wrover_b_datasheet_en-1384674.pdf)

[Datasheet ESP32-Wrover E](https://br.mouser.com/datasheet/2/891/esp32_wrover_e_esp32_wrover_ie_datasheet_en-1855913.pdf)

</br>

---
## 🦸🏻‍♂️ Autor

 <img style="border-radius: 50%;" src="https://avatars.githubusercontent.com/u/86720109?s=400&v=4" width="100px;" alt=""/>

 Lucas Estevo 🚀
 
	
[![Linkedin Badge](https://img.shields.io/badge/-Lucas-blue?style=flat-square&logo=Linkedin&logoColor=white&link=https://www.linkedin.com/in/lucasestevo/)](https://www.linkedin.com/in/lucasestevo/)
![Gmail Badge](https://img.shields.io/badge/-lucasestevo2015@gmail.com-c14438?style=flat-square&logo=Gmail&logoColor=white)

---

## 📝 Licença

Este projeto esta sobe a licença [MIT](./LICENSE).

Feito por Lucas Estevo 👋🏽 [Entre em contato!](https://www.linkedin.com/in/lucasestevo/)

---

##  Versões do README

[Português 🇧🇷](./README-pt_BR.md)  |  [Inglês 🇺🇸](./README-en.md) 
