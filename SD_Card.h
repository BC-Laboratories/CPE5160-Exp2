#ifndef _SD_Card_H
#define _SD_Card_H

#include "Main.h"

uint8_t send_command(uint8_t command, uint32_t argument);

uint8_t receive_response(uint8_t num_bytes,uint8_t * buffer);



#endif
