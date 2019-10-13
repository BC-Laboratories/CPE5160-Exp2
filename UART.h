#ifndef _UART_H
#define _UART_H

#include "main.h"

//------- Public Constant definitions --------------------------------
// UART settings
#define UART_MODE0 (0x00)
#define UART_MODE1 (0x40)
#define UART_MODE2 (0x80)
#define UART_MODE3 (0xC0)
#define ENABLE_485_MODE (1<<5)
#define RECEIVE_ENABLE (1<<4)
#define TRANSMIT_FLAG_SET (1<<1)
#define TRANSMIT_FLAG_CLEAR (0)
#define RECEIVE_FLAG_SET (1<<0)
#define RECEIVE_FLAG_CLEAR (0)



#define SMOD1 (1)
#define SPD (1)
/*** These defines can be used for compile time baud rate values ****/
//#define BAUD_RATE (9600)
//#define BAUD_RATE_RELOAD ((uint8_t) (256-((1+(5*SPD))*(1+(1*SMOD1))*OSC_FREQ)/(32UL*OSC_PER_INST*BAUD_RATE)))


// ASCII characters
#define space (0x20)
#define CR (0x0D)
#define LF (0x0A)
#define BS (0x08)
#define DEL (0x7F)


// ------ Public function prototypes -------------------------------
void uart_init(uint16_t BAUD_RATE);
uint8_t UART_Transmit(uint8_t send_value);
uint8_t UART_Receive(void);



#endif