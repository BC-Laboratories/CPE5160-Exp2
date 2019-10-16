/* EXP2 - SPI and SD Card
 * Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 */

#ifndef _SD_CARD_H
#define _SD_CARD_H

#include "Main.h"

// error codes
#define illegal_command 0xE0
#define sd_init_error 0xE1
#define comm_error 0xE3
#define data_error 0xE4
#define send_error 0xE5
#define no_errors 0x00
#define spi_error 0x01
#define timeout_error 0x02

// SD card commands 
#define CMD0 0
#define CMD8 8
#define CMD16 16
#define CMD17 17
#define CMD55 55
#define CMD58 58
#define ACMD41 41

#define block_size 512

// SD card select
sbit SD_select = P1^4;

/* Desc: Sends a command to the SD Card
 * Pre: None
 * Post: Returns an error code or 0x00 if no_errors
 */
uint8_t send_command(uint8_t command, uint32_t argument);

/* Desc: Receives the R1 or R3 response from SD Card
 * Pre: None
 * Post: Will modify the rec_array buffer passed to it
 */
uint8_t receive_response(uint8_t num_bytes, uint8_t * rec_array);

/* Desc: Setups SD Card
 * Pre: None
 * Post: Calls SPI Master Init and returns an error code
 */
uint8_t SD_card_init(void);

/* Desc: Reads a block from SD Card
 * Pre: None
 * Post: Will modify the rec_array buffer passed to it
 */
uint8_t read_block(uint16_t num_bytes, uint8_t * rec_array);

#endif
