#include <stdio.h>
#include "main.h"
#include "print_bytes.h"
#include "UART.h"





/***********************************************************************
DESC:    Prints 8 bytes in Hexadecimal and then ASCII
INPUT: Pointer to an array of eight bytes
RETURNS: nothing
CAUTION: 
************************************************************************/
void print_16bytes(uint8_t * array_in_p)
{
   uint8_t * input_p;
   uint8_t index, dat;
   input_p = array_in_p;
   printf("%p ",input_p);
   for (index=0;index<16;index++)
   {
      dat=*(input_p+index);
      printf("%2.2bX ",dat);
   }
   for (index=0;index<16;index++)
   { 
      dat=*(input_p+index);
      if (dat<32 || dat>126) dat=46;
      UART_Transmit(dat);
   }
   UART_Transmit(CR);
   UART_Transmit(LF);
}


/***********************************************************************
DESC: Prints an array from memory in Hexadecimal and then ASCII
INPUT: Pointer to an array, number of bytes to print
RETURNS: nothing
CAUTION: 
************************************************************************/
void print_memory(uint8_t * array_in_p, uint16_t number_of_bytes)
{
   uint8_t * input_p;
   uint16_t i;
   input_p = array_in_p;
   i = (uint16_t) input_p;
   i &= 0x000f;
   input_p = input_p - i;
   printf("Addr.   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n\r");
   do
   {
      print_16bytes(input_p);
      input_p+=16;
   }while(input_p<(array_in_p+number_of_bytes));
}
		