/* EXP2 - SPI and SD Card
 * Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 */

#ifndef _SPI_Interface_H
#define _SPI_Interface_H

#include "Main.h"

#define no_errors 0x00
#define timeout_error 0x02
#define clock_rate_error 0x03
#define SPI_error 0x01
#define baud_error 0x09

#define spi_clock_freq 400000
#define CPOL 0
#define CPHA 0

/* Desc: Sets up the SPI
 * Pre: Must be passed a maximum clock frequency
 * Post: Returns an error code or 0x00 if no errors
 */
uint8_t SPI_Master_Init(uint32_t clock_freq);

/* Desc: Transfers a byte over SPI
 * Pre: SPI must be init
 * Post: Returns an error code or 0x00 if no errors
 */
uint8_t SPI_Transfer(uint8_t send_value, uint8_t * received_value);

#endif
