#ifndef _LCD_hardware_H
#define _LCD_hardware_H

#include "Main.H"

/***** Public Constants ****/
#define COMMAND (0)
#define DDRAM (1)

/**** Public Functions ****/

void LCD_Write(uint8_t RegSelect, uint8_t LCD_Data);



#endif