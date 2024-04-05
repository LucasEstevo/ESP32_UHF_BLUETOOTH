/*********************************************************************!   
 *  \fn     main.c                                                   *
 *  \date   06/2022                                                  *
 *  \author L.Estevo                                                 *
 *********************************************************************/
#include <stdbool.h>
#include "uart.h"
#include "bluetooth.h"
#include "esp_err.h"

static const char *TAG_ESP = "<MAIN>";

void app_main(void)
{
    uart0_init();
    //uart1_init(); //Apenas para Debug
    
    bt_init();
}