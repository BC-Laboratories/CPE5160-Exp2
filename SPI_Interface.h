/* EXP2 - SPI and SD Card
 * Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 */

#ifndef _SPI_Interface_H
#define _SPI_Interface_H

#include "Main.h"

/*
 * Sets up the SPI peripheral
 * Parameters: clock_freq - clock frequency
 * Returns: A clock rate error (0x02) flag if something went wrong
 * Warning: Will modify the SPCON register
 */
uint8_t SPI_Master_Init(uint32_t clock_freq);

/*
 * Sends a single byte over SPI
 * Parameters: send_value - value to be sent over SPI
 *             received_value - response back over SPI
 * Returns: A SPI (0x03) or timeout (0x01) error flag
 * Warning: Will modify the SPDAT and SPSTA register.
 *          Will modify the received_value buffer sent to it
 */
uint8_t SPI_Transfer(uint8_t send_value, uint8_t * received_value);

#endif
