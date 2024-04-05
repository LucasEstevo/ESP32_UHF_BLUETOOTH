/*********************************************************************!   
 *  \fn     uart.h                                                   *
 *  \date   06/2022                                                  *
 *  \author L.Estevo                                                 *
 *********************************************************************/
#ifndef UART_H_
#define UART_H_
#include "string.h"

void uart0_init();
void uart1_init();
void sendData(int data);
void iniciar();
void finalizar();
void uart_write_debug(char *data, ...);
void init_timers();

#endif