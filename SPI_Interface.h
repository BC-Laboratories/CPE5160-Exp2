#ifndef _SPI_Interface_H
#define _SPI_Interface_H

#include "Main.h"

uint8_t SPI_Master_Init(uint32_t clock_freq);

uint8_t SPI_Transfer(uint8_t send_value, uint8_t * received_value);

#endif
