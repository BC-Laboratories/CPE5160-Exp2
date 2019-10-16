#include <stdio.h>
#include "Main.h"
#include "PORT.H"
#include "UART.h"
#include "print_bytes.h"
#include "Timer0_hardware_delay_1ms.h"
#include "LCD_routines.h"
#include "LED_Outputs.h"
#include "Long_Serial_In.h"
#include "SD_Card.h"
#include "SPI_Interface.h"


int main()
{
	//*** This line is needed to make all 1,024 byes of XRAM avaiable ***//
	AUXR=0x0c;   // make all of XRAM available, ALE always on
	LEDS_ON(Red_LED);
	
	if(OSC_PER_INST==6)
	{
		CKCON0=0x01;  // set X2 clock mode
	}
	else if(OSC_PER_INST==12)
	{
		CKCON0=0x00;  // set standard clock mode
	}
	Timer0_DELAY_1ms(300); // Delay to see RED LED (visual indication program is executing)
	//*** Initialization Functions  ***//
	uart_init(9600);
	LCD_Init();
	SD_card_init();
	LEDS_OFF(Red_LED);
	//**** Super Loop ****//
	while(1)
	{
		uint8_t xdata rec_array[block_size] = {0}; //buffer used for block
		uint32_t input_value;
		//Example of how to use long serial input function to read a 32-bit input value
		printf("Input a value: ");
		input_value = long_serial_input();
		SD_select = 0;
		send_command(CMD17, input_value);
		read_block(block_size, &rec_array);
		SD_select = 1;
		print_memory(&rec_array,block_size);
		//Notice the 'l' modifier on %u which indicates a long (32-bit value)
		//If the value to print is a char (8-bit value), then use a 'b' modifier (%bu).
        printf("Value Entered = %lu\n\r", input_value);	
	}
	return 0;
}


