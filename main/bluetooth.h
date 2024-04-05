/*********************************************************************!   
 *  \fn     bluetooth.h                                              *
 *  \date   06/2022                                                  *
 *  \author L.Estevo                                                 *
 *********************************************************************/
#ifndef COMPONENTS_BLUETOOTH_INCLUDE_BLUETOOTH_H_
#define COMPONENTS_BLUETOOTH_INCLUDE_BLUETOOTH_H_

#include "esp_log.h"
#include "esp_err.h"

bool is_dev_name(char * data);

void is_dev_master(char * data);

esp_err_t esp_bt_write(int len, uint8_t * data);

void bt_init(void);

#define MAX_DATA_LEN 128

void bt_print(const char *format, ...);

int conection_status();

#endif /* COMPONENTS_BLUETOOTH_INCLUDE_BLUETOOTH_H_ */
