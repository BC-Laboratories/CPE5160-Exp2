#include "main.h"
#include "LCD_hardware_functions.h"
#include "PORT.H"


/****** Private Functions  ******/

void Output_LCD_data(uint8_t Port_Data);
void Clear_LCD_bit(uint8_t Bit_Data);
void Set_LCD_bit(uint8_t Bit_Data);


/***********************************************************************
DESC:    Writes data to the LCD with proper order for RS and E
INPUT: Uint8 for RS and Data to be written
       RS=0 instruction, RS=1 data
RETURNS: Nothing
CAUTION: None
************************************************************************/
void LCD_Write(uint8_t RegSelect, uint8_t LCD_Data)
{
  if(RegSelect==0)     // Set or Clear RS before E is Set
  {
	Clear_LCD_bit(RS_bit);
  }
  else
  {
    Set_LCD_bit(RS_bit);
  }
  Set_LCD_bit(E_bit);  // Set Enable before data written
  Output_LCD_data(LCD_Data);         
  Clear_LCD_bit(E_bit);   
  Output_LCD_data(0xFF);
  Set_LCD_bit(RS_bit);
}


/*********************************************************************
*    Port Output Functions
********************************************************************/

/***********************************************************************
DESC:  Writes the value passed as a parameter to the LCD data port
INPUT: uint8_t value to be written to a port
RETURNS: Nothing
CAUTION: 
************************************************************************/
void Output_LCD_data(uint8_t Port_Data)
{
  P0=Port_Data;
}

/***********************************************************************
DESC:  Clears the specified bits on a specified port for LCD signals
INPUT: 8-bit pattern with '1' bits for the bits to be cleared
RETURNS: Nothing
CAUTION: 
************************************************************************/


void Clear_LCD_bit(uint8_t Bit_Data)
{
  P3&=(~Bit_Data);
}

/***********************************************************************
DESC:  Set the specified bits on a specified port for LCD signals
INPUT: 8-bit pattern with '1' bits for the bits to be set
RETURNS: Nothing
CAUTION: 
************************************************************************/


void Set_LCD_bit(uint8_t Bit_Data)
{
  P3|=Bit_Data;
}


