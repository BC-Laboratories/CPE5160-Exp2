#ifndef _print_bytes_H
#define _print_bytes_H

#include "main.h"
#include "UART.h"

// ASCII characters
#define space (0x20)
#define CR (0x0D)
#define LF (0x0A)
#define BS (0x08)
#define DEL (0x7F)


// ------ Public function prototypes -------------------------------

void print_16bytes(uint8_t * array_in_p);

void print_memory(uint8_t * array_in_p, uint16_t number_of_bytes);


#endif