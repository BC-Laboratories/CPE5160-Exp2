#include "main.h"
#include "LCD_Routines.h"
#include "LCD_hardware_functions.h"
#include "Timer0_hardware_delay_1ms.h"



/***********************************************************************
DESC:    Initializes LCD
INPUT: Nothing
RETURNS: Nothing
CAUTION: Uses timer 0 in delay functions
************************************************************************/
void LCD_Init(void)
{
  LCD_Write(COMMAND,function_set_8bit|two_line|set_5x8);
  Timer0_DELAY_1ms(1);
  LCD_Write(COMMAND,display_control|display_on|cursor_off|blink_off);   // Display on, Cursor On and Not blinking
  Timer0_DELAY_1ms(1);
  LCD_Write(COMMAND,clear_display);   // Clear Display
  Timer0_DELAY_1ms(2);         // Must wait until this is complete
  LCD_Write(COMMAND,entry_mode|shift_right|cursor);   // DDRAM address increments with no scrolling
  Timer0_DELAY_1ms(1);
}



void LCD_print(uint8_t address, uint8_t num_bytes, uint8_t *array_name_p)
{
   uint8_t index,character;
   if(address!=no_addr_change)
   {
     LCD_Write(COMMAND,set_ddram_addr|address);
     Timer0_DELAY_1ms(1);
   }
   if(num_bytes==0)
   {
      index=0;
      character=*(array_name_p+index);
      while(character!=0)
      {
         LCD_Write(DDRAM,character);
         Timer0_DELAY_1ms(1);
         index++;
         character=*(array_name_p+index);
      }
    }
    else
    {
       for(index=0;index<num_bytes;index++)
       {
          character=*(array_name_p+index);
          LCD_Write(DDRAM,character);
          Timer0_DELAY_1ms(1);
       }
    }     
}	

