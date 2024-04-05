/*********************************************************************!   
 *  \fn     uart.c                                                   *
 *  \date   06/2022                                                  *
 *  \author L.Estevo                                                 *
 *********************************************************************/
#include "uart.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "esp_system.h"
#include "board_pins.h"
#include "esp_log.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bluetooth.h"

#define BYTES_PER_LINE 26
char hex_buffer[3 * BYTES_PER_LINE + 1];

#define SPP_TAG "ESP_SPP_BT"
static const char *TAG_UART = "<ESP_UART>";

xTaskHandle xTaskDataHandle;
void vTaskData(void *pvParameters);

static const int TX_BUF_SIZE_DEBUG = 256;
static const int RX_BUF_SIZE_DEBUG = 256;
static const int RX_CARD_BUF_SIZE = 26;
static const int RX_BUF_SIZE = 78;

static const uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB,
};

static const uart_config_t uart_config_debug = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB,
};

void sendData(int data)
{
    static const char *TAG = "TX_UART";

    char read_on[30] = "\xaa\x02\x10\x00\x02\x01\x01\x71\xad";
    char read_off[30] = "\xaa\x02\xff\x00\x00\xa4\x0f";

    if (data == 1)
    {
        const int txBytes = uart_write_bytes(UART_NUM_1, read_on, 9);
        uart_write_debug("%s Escrito %d bytes: Leitura Iniciada ", TAG_UART, txBytes);
        // ESP_LOGI(TAG_UART, " Escrito %d bytes: Leitura Iniciada ", txBytes);
    }
    if (data == 2)
    {
        const int txBytes = uart_write_bytes(UART_NUM_1, read_off, 7);
        uart_write_debug("%s Escrito %d bytes: Leitura Finalizada ", TAG_UART, txBytes);
        // ESP_LOGI(TAG_UART, " Escrito %d bytes: Leitura Finalizada ", txBytes);
    }

    return;
}

void buffer_hex(const void *buffer, uint16_t buff_len)
{
    if (buff_len == 0)
    {
        return;
    }
    char temp_buffer[BYTES_PER_LINE + 3]; // for not-byte-accessible memory

    const char *ptr_line;
    int bytes_cur_line;

    if (buff_len > BYTES_PER_LINE)
    {
        bytes_cur_line = BYTES_PER_LINE;
    }
    else
    {
        bytes_cur_line = buff_len;
    }
    if (!esp_ptr_byte_accessible(buffer))
    {
        // use memcpy to get around alignment issue
        memcpy(temp_buffer, buffer, (bytes_cur_line + 3) / 4 * 4);
        ptr_line = temp_buffer;
    }
    else
    {
        ptr_line = buffer;
    }

    for (int i = 0; i < bytes_cur_line; i++)
    {
        sprintf(hex_buffer + 3 * i, "%02x ", ptr_line[i]);
    }
    buffer += bytes_cur_line;
    buff_len -= bytes_cur_line;
}

void uart_write_debug(char *data, ...)
{
    va_list args;
    char buffer[TX_BUF_SIZE_DEBUG];
    memset(buffer, 0, TX_BUF_SIZE_DEBUG);

    va_start(args, data);
    vsprintf(buffer, data, args);
    strcat(buffer, "\r\n");

    const int txBytes = uart_write_bytes(UART_NUM_0, buffer, strlen(buffer));

    va_end(args);
}

#include "esp_timer.h"
#define SEC_TO_USEC(sec) (sec * (1000 * 1000))
#define MSEC_TO_USEC(msec) (msec * 1000)
#define MIN_TO_SEC(min) (min * 60)

#define LOG_TIMER_TIMEOUT SEC_TO_USEC(2)
static void send_ftp_files_cb(void *args);
static esp_timer_handle_t send_ftp_files_handler;

void uart0_init()
{

    uart_driver_install(UART_NUM_0, RX_BUF_SIZE_DEBUG * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config_debug);
    uart_set_pin(UART_NUM_0, GPIO_TXD_DEBUG, GPIO_RXD_DEBUG, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_write_debug("%s UART_NUM_0 Iniciada", TAG_UART);
    // ESP_LOGI(TAG_UART, " UART_NUM_0 Iniciada");
    init_timers();
    esp_timer_start_periodic(send_ftp_files_handler, LOG_TIMER_TIMEOUT);

    return;
}

void uart1_init()
{
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, GPIO_TXD_DIGI, GPIO_RXD_DIGI, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_write_debug("%s UART_NUM_1 Iniciada", TAG_UART);
    // ESP_LOGI(TAG_UART, "UART_NUM_1 Iniciada");

    return;
}

static esp_timer_create_args_t send_ftp_files_create_args =
    {
        .callback = &send_ftp_files_cb,
        .name = "sendftpfiles",
        .arg = NULL,
};

void init_timers()
{
    esp_timer_create(&send_ftp_files_create_args, &send_ftp_files_handler);
    return;
}

static char card[30];
void vTaskData(void *pvParameters)
{
    uint8_t *data = (uint8_t *)malloc(RX_CARD_BUF_SIZE + 1);
    char envio[80], verify[25]; //, card[30];

    while (1)
    {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_CARD_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        vTaskDelay(pdMS_TO_TICKS(10));
        // uart_write_debug("%d %d bytes", TAG_UART,rxBytes);
        // ESP_LOGI(TAG_UART, "%d bytes", rxBytes);
        card[0] = '\0';
        if (rxBytes > 0)
        {
            data[rxBytes] = '\0';

            buffer_hex(data, rxBytes);
            sprintf(envio, "%s", hex_buffer);
            //uart_write_debug("\n%s\n",envio); //Apenas Teste

            memcpy(&verify, &envio, 20);
            verify[20] = '\0';

            if (strcmp(verify, "aa 12 00 00 13 00 0c") == 0)
            {
                int x = 1, y = 24, z = 2;
                /* x =>
                 * y => Posição inicial do número coletado da tag
                 * z =>
                 */

                memcpy(&card[0], &envio[22], 1);
                card[1] = '\0';

                for (size_t i = 0; i < 7; i++)
                {
                    memcpy(&card[x], &envio[y], 2);
                    uart_write_debug("\n%s\n",card); //Apenas Teste
                    //card[z] = '\0';
                    x = x + 2;
                    y = y + 3;
                    z = z + 2;
                }
                
                
                /*int conection = conection_status();
                if (conection == 1)
                {
                    bt_print("%s", card);
                }*/
            }
            else
            {
                /*A antena por padrão quando recebe comando de iniciar vai enviar uma resposta de 8bytes.
                 *Porém se nesse período que você enviou o comando para iniciar e antes da antena responder ela já detectar alguma tag
                 * ela irá enviar a resposta de 8bytes acrescida da tag e isso resulta em uma distorção total de todas as próximas leituras
                 * de Tag recebidas na UART. Afim de evitar o trabalho de retirar a tag do campo da antena e voltar ela depois, eu coloquei
                 * o comando uart_flush para limpar o buffer da UART por completo assim ele irá excluir a mensagem defeituosa e as demais
                 * já seguirão no formato adequado para uso.
                 *  Toda mensagem deve ter o seguinte inicio:
                 * aa 12 00 00 13 00 0c 12 bytes
                 * Caso a mensagem saia deste formato o software do esp não irá conseguir tratá-la para enviar por bluetooth
                 */
                uart_write_debug("%d Limpando UART_NUM_1 - Motivo: Recebimento de Mensagem Defeituosa", TAG_UART);
                // ESP_LOGE(TAG_UART, "Limpando UART_NUM_1 - Motivo: Recebimento de Mensagem Defeituosa");
                uart_flush(UART_NUM_1);
            }
        }
    }
}

int conection = 0;
static void send_ftp_files_cb(void *args)
{
    conection = conection_status();
    if ((conection == 1) && (strlen(card) != 0))
    {
        bt_print("%s", card);
        //uart_write_debug("\n%s",card); //Apenas Teste
    }
}

void iniciar()
{
    uart1_init();
    sendData(1);
    xTaskCreate(vTaskData, "vTaskData", configMINIMAL_STACK_SIZE + 4096, NULL, 5, &xTaskDataHandle);
    return;
}

void finalizar()
{
    sendData(2);
    vTaskDelay(pdMS_TO_TICKS(1000));
    vTaskSuspend(xTaskDataHandle);
    uart_driver_delete(UART_NUM_1);
    return;
}
