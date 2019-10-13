#include "SPI_Interface.h"

#define CPOL 0
#define CPHA 0

#define no_errors 0x00
#define timeout_error 0x01
#define clock_rate_error 0x02
#define SPI_error 0x03

uint8_t SPI_Master_Init(uint32_t clock_freq)
{
	uint8_t divider;
	uint8_t return_value=no_errors;

	divider=(OSC_FREQ*6)/(OSC_PER_INST*clock_freq);
	if(divider<2) 
	{
		SPCON=0x70|(CPOL<<3)|(CPHA<<2); 
	}
	else if(divider<4) 
	{
		SPCON=0x71|(CPOL<<3)|(CPHA<<2); 
	}
	else 
	{
		return_value=clock_rate_error; 
	}
	return return_value;
}


uint8_t SPI_Transfer(uint8_t send_value, uint8_t * received_value)
{
	uint8_t timeout = 0;
	uint8_t status;
	uint8_t error_flag;

	//Init transfer by writing a value to SPDAT register
	SPDAT=send_value;
	do 
	{
		status=SPSTA;
		timeout++;
	}while(((status&0xF0)==0)&&(timeout!=0));
	if(timeout==0)
	{  
		error_flag=timeout_error;
		*received_value=0xFF;
	}
	else if((status&0x70)!=0)
	{
		error_flag=SPI_error;
		*received_value=0xFF;
	}
	else
	{
		error_flag=no_errors;
		*received_value=SPDAT;
	}
	return error_flag;
}
