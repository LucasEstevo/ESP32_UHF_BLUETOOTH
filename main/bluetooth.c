/*********************************************************************!   
 *  \fn     bluetooth.c                                              *
 *  \date   06/2022                                                  *
 *  \author L.Estevo                                                 *
 *********************************************************************/
#include "bluetooth.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "uart.h"

#define SPP_TAG "ESP_SPP_BT"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_BT"

int CS = 0;

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;
static const esp_spp_role_t role_master = ESP_SPP_ROLE_MASTER;
static uint32_t conn_handle;
static const char dev_name_set[5] = {"nome;"};
static char device_name[18];
static nvs_handle nvs_handler;
static bool is_dev_name_set = false;
esp_bd_addr_t peer_bd_addr = {0};


esp_err_t esp_bt_write(int len, uint8_t *data)
{
    return esp_spp_write(conn_handle, len, data);
}

bool is_dev_name(char *data)
{
    char aux[5];
    if (strlen(data) > 5)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            aux[i] = data[i];
        }
        aux[5] = '\0';
    }
    memcpy(device_name, &data[5], 16);
    if (0 == strcmp(dev_name_set, aux))
    {
        esp_bt_write(strlen(device_name), (uint8_t *)device_name);
        nvs_open("name", NVS_READWRITE, &nvs_handler);
        uart_write_debug("%s %s", SPP_TAG, device_name);
        // ESP_LOGI(SPP_TAG, "%s", device_name);
        nvs_set_str(nvs_handler, dev_name_set, device_name);
        nvs_commit(nvs_handler);
        nvs_close(nvs_handler);

        return true;
    }
    return false;
}

void is_dev_master(char *data)
{
    int bt_msg = 0;
    bt_msg = atoi(data);
    if (bt_msg != 0)
    {
        // esp_spp_stop_srv();
    }
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    esp_err_t ret;
    size_t read_bytes;
    switch (event)
    {
    case ESP_SPP_INIT_EVT:
        uart_write_debug("%s ESP_SPP_INIT_EVT", SPP_TAG);
        //    ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        ret = nvs_open("name", NVS_READWRITE, &nvs_handler);
        nvs_get_str(nvs_handler, "name", NULL, &read_bytes);
        ret = nvs_get_str(nvs_handler, dev_name_set, device_name, &read_bytes);
        device_name[18] = '\0';
        nvs_close(nvs_handler);
        if (ret == ESP_ERR_NVS_NOT_FOUND)
        {
            esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        }
        else
        {
            esp_bt_dev_set_device_name(device_name);
        }
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        uart_write_debug("%s ESP_SPP_DISCOVERY_COMP_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        if (param->disc_comp.status == ESP_SPP_SUCCESS)
        {
            esp_spp_connect(sec_mask, role_master, param->disc_comp.scn[0], peer_bd_addr);
            esp_bt_write(34, (uint8_t *)"mac encontrado, tentando conectar");
        }
        break;
    case ESP_SPP_OPEN_EVT:
        conn_handle = param->open.handle;
        uart_write_debug("%s ESP_SPP_OPEN_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        uart_write_debug("%s ESP_SPP_CLOSE_EVT", SPP_TAG);
        // ESP_LOGW(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        finalizar();
        CS = 0;
        break;
    case ESP_SPP_START_EVT:
        uart_write_debug("%s ESP_SPP_START_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        uart_write_debug("%s ESP_SPP_CL_INIT_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(SPP_TAG, "%s", (char *)param->data_ind.data);
        param->data_ind.data[param->data_ind.len] = 0;
        if (is_dev_name((char *)param->data_ind.data))
        {
            uart_write_debug("%s Dev name sest", SPP_TAG);
            // ESP_LOGI(SPP_TAG, "Dev name sest");
            is_dev_name_set = true;
        }
        else
        {
            // uart_write((char*)param->data_ind.data, param->data_ind.len);
        }
        is_dev_master((char *)param->data_ind.data);
        break;
    case ESP_SPP_CONG_EVT:
        uart_write_debug("%s ESP_SPP_CONG_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        uart_write_debug("%s ESP_SPP_WRITE_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT: // Dispositivo Conectado
        conn_handle = param->open.handle;
        uart_write_debug("%s ESP_SPP_SRV_OPEN_EVT", SPP_TAG);
        // ESP_LOGW(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        iniciar();
        CS = 1;
        break;
    case ESP_SPP_SRV_STOP_EVT:
        uart_write_debug("%s ESP_SPP_SRV_STOP_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        uart_write_debug("%s ESP_SPP_UNINIT_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_BT_GAP_DISC_RES_EVT:
        uart_write_debug("%s ESP_BT_GAP_DISC_RES_EVT", SPP_TAG);
        // ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_RES_EVT");
        esp_log_buffer_hex(SPP_TAG, param->disc_res.bda, ESP_BD_ADDR_LEN);
        char aux[19];
        sprintf(aux, "" MACSTR ";", MAC2STR(peer_bd_addr));
        // uart_write(aux, 19);
        if (memcmp(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN) == 0)
        {
            esp_spp_start_discovery(peer_bd_addr);
            esp_bt_gap_cancel_discovery();
        }
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT:
    {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
        {
            uart_write_debug("%s authentication success: %s", SPP_TAG, param->auth_cmpl.device_name);
            // ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        }
        else
        {
            uart_write_debug("%s authentication failed, status:%d", SPP_TAG, param->auth_cmpl.stat);
            // ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:
    {
        uart_write_debug("%s ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", SPP_TAG, param->pin_req.min_16_digit);
        // ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit)
        {
            uart_write_debug("%s Input pin code: 0000 0000 0000 0000", SPP_TAG);
            // ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        }
        else
        {
            uart_write_debug("%s Input pin code: 1234", SPP_TAG);
            // ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    default:
    {
        uart_write_debug("%s event: %d", SPP_TAG, event);
        // ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

void bt_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK)
    {
        uart_write_debug("%s %s initialize controller failed: %s\n", SPP_TAG, __func__, esp_err_to_name(ret));
        // ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK)
    {
        uart_write_debug("%s %s enable controller failed: %s\n", SPP_TAG, __func__, esp_err_to_name(ret));
        //ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK)
    {
        uart_write_debug("%s %s initialize bluedroid failed: %s\n", SPP_TAG, __func__, esp_err_to_name(ret));
        //ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK)
    {
        uart_write_debug("%s %s enable bluedroid failed: %s\n", SPP_TAG, __func__, esp_err_to_name(ret));
        //ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK)
    {
        uart_write_debug("%s %s gap register failed: %s\n", SPP_TAG, __func__, esp_err_to_name(ret));
        //ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK)
    {
        uart_write_debug("%s %s spp register failed: %s\n", SPP_TAG, __func__, esp_err_to_name(ret));
        //ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK)
    {
        uart_write_debug("%s %s spp init failed: %s\n", SPP_TAG, __func__, esp_err_to_name(ret));
        //ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use fixed pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
    esp_bt_pin_code_t pin_code = {'0', '0', '0', '0'};
    esp_bt_gap_set_pin(pin_type, 4, pin_code);
}

void bt_print(const char *format, ...)
{
    va_list args;
    char buffer[MAX_DATA_LEN];
    va_start(args, format);
    int length = vsprintf(&buffer[0], format, args);
    esp_bt_write(length, (uint8_t *)&buffer[0]);
    va_end(args);
}

int conection_status()
{
    return CS;
}
