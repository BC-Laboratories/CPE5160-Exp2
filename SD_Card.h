/* EXP2 - SPI and SD Card
 * Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 */

#ifndef _SD_CARD_H
#define _SD_CARD_H

#include "Main.h"

// error codes
#define ILLEGAL_COMMAND 0xE0
#define SD_INIT_ERROR 0xE1
#define COMM_ERROR 0xE3
#define DATA_ERROR 0xE4
#define SEND_ERROR 0xE5

// SD card commands 
#define CMD0 0
#define CMD8 8
#define CMD16 16
#define CMD17 17
#define CMD55 55
#define CMD58 58
#define ACMD41 41

// function prototypes
uint8_t send_command(uint8_t command, uint32_t argument);
uint8_t receive_response(uint8_t num_bytes, uint8_t * byte_array);
uint8_t SD_card_init(void);
uint8_t read_block(uint16_t num_bytes, uint8_t * byte_array);

/*
#ifndef _SD_Card_H
#define _SD_Card_H

#include "Main.h"
//below is for uint8_t types
//#include <stdint.h>

/*
 *Desc: 
 *Pre:
 *Post:
 */
//uint8_t SD_Card_Init(void);

/*
 *
 */
//uint8_t send_command(uint8_t command, uint32_t argument);

/*
 *
 */
//uint8_t receive_response(uint8_t num_bytes,uint8_t * buffer);

#endif
