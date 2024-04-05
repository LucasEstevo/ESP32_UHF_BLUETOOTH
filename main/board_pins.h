/********************************************************************!   
 *  \fn     board_pins.h                                            *
 *  \date   06/2022                                                 *
 *  \author L.Estevo                                                *
 ********************************************************************/

#ifndef BOARD_PINS_H
#define BOARD_PINS_H

/********************************************************************
 *************************  UI - UART  ******************************
 ********************************************************************/
#define GPIO_TXD_DEBUG (GPIO_NUM_1)
#define GPIO_RXD_DEBUG (GPIO_NUM_3)

//Aqui devera ser definido qual chip está sendo utilizado "ESP_WROVER_E" ou "ESP_WROVER_B"
#define Microcontrolador ESP_WROVER_B //ESP_WROVER_E

#ifdef ESP_WROVER_B
    #define GPIO_TXD_DIGI (GPIO_NUM_10) 
    #define GPIO_RXD_DIGI (GPIO_NUM_9)  
#else
    #define GPIO_TXD_DIGI (GPIO_NUM_21)   
    #define GPIO_RXD_DIGI (GPIO_NUM_15)
#endif

#endif