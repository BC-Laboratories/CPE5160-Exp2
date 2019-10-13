#include "main.h"
#include "PORT.H"



/*********************************************************************
*    LED Output Functions
********************************************************************/


/***********************************************************************
DESC:  Clears the specified bits on Port 2 to switch on LEDS.
INPUT: 8-bit pattern with '1' bits for the LEDS to be switched on
RETURNS: Nothing
CAUTION: LEDS must be connected to Port 2 and be active low
************************************************************************/

void LEDS_ON(uint8_t LED_bits)
{
  P2&=(~LED_bits);
} 



/***********************************************************************
DESC:  Set the specified bits on Port 2 to switch off LEDS
INPUT: 8-bit pattern with '1' bits for the bits to be switched off
RETURNS: Nothing
CAUTION: LEDS must be connected to Port 2 and be active low
************************************************************************/


void LEDS_OFF(uint8_t LED_bits)
{
  P2|=LED_bits;
}


