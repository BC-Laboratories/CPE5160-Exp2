/* EXP2 - SPI and SD Card
 * Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 */

#include "SD_Card.h"
#include "SPI_Interface.h"

#define clock_frequency 400000

// Below are the error flags
#define no_errors 0x00
#define illegal_command 0x05
#define SPI_error 0x02
#define SD_timeout_error 0x03
#define comm_error 0x09
#define failed_init_response 0x10

//COMMAND # constants
#define CMD0 0
#define CMD8 8
#define CMD55 55
#define CMD58 58
#define ACMD41 41

//SD_select - Variable used for setting CS low and high during SD card transfers
sbit SD_select = P1^4;
//sfr SPIF = SPSTA^7


uint8_t SD_Card_Init(void)
{
	uint8_t received_value; //used to hold value returned from SPI_Transfer
	uint8_t error_status; //used to hold error value
	uint8_t index = 0; //index for for loops
	uint8_t rec_array[5];

	error_status = SPI_Master_Init(clock_frequency); //#define later
	
	//send at least 74 clock cycles (rounded to 80, 10x 8 bytes)
	if(error_status == no_errors)
	{
		SD_select = 1;
		for( index = 0; index < 10; index++)
		{
			//sends each clock cycle
			SPI_Transfer(0x00, &received_value);
		}
	}

	//CMD0 Place SD Card into SPI Mode and IDLE state
	if(error_status == no_errors)
	{
		SD_select = 0;
		//Send CMD0 command
		send_command(CMD0, 0x000000);
		error_status = receive_response(1, rec_array);
	}

	//CMD8 if CMD0 response successful
	if(error_status == no_errors)
	{
		//Exit if card not in idle state
		if(rec_array[0] != 0x01)
		{
			error_status = failed_init_response; //FIXME
		}
		else
		{
			SD_select = 0;
			//States 3.3v is accepted, with 0xAA as the check byte
			send_command(CMD8, 0x0001AA);
			error_status = receive_response(3, rec_array);
		}
		
	}

	//CMD58 send
	if (error_status = no_errors)
	{


		if(rec_array[0] == illegal_command)
		{
			//v1 card: We won't accept it
			error_status = illegal_command;
		}
		else
		{
			SD_select = 0;
			send_command(CMD58, 0x00);
			error_status = receive_response;
		}
	}



	if(error_status == no_errors)
	{
		SD_select = 1;
		
		
		SD_select = 0;
		send_command(CMD55, 0x00);
		error_status = receive_response( 1, rec_array);

		for( index = 0; index < 1; index++)
		{
			SPI_Transfer(0x00, &received_value);
		}

	}
	
	if (error_status = no_errors)
	{

		SD_select = 0;
	}

	if(error_status == no_errors)
	{
		SD_select = 1;
		for(index = 0; index < 4; index++)
		{
			SPI_Transfer(0x00, &received_value);
		}

		SD_select = 0;
		do
		{
			send_command(ACMD41, 0x40000000);
			error_status = receive_response(1, rec_array);
		}
		while((error_status = no_errors) && (rec_array != 0x01));
	}
	

}

uint8_t send_command(uint8_t command, uint32_t argument)
{
	uint8_t error_flag;
	uint8_t return_value;
	uint8_t send_value;
	uint8_t rec_value;
	uint8_t index = 0;

	//check if command is 63 bits
	if(command<64)
	{
		return_value=no_errors;
	}
	else
	{
		return_value=illegal_command;
	}
	//Append start and transmission bits to first byte
	send_value=0x40|command;
	//Send the first part of command
	error_flag = SPI_Transfer(send_value, &rec_value);
	if(error_flag!=no_errors)
	{
		return_value=SPI_error;
	}
	if(return_value==no_errors)
	{
	    //send MSB first
		send_value=(uint8_t)(argument>>24);
		error_flag=SPI_Transfer(send_value,&rec_value);
		if(error_flag!=no_errors)
		{
			return_value=SPI_error;
		}
	}
	// TODO: is this correct
	// Send the rest of the 4 bytes
	for(index=0;index<4;index++)
	{
		if(return_value==no_errors)
		{   
			send_value=(uint8_t)(argument>>(24-(index*8)));
			error_flag=SPI_Transfer(send_value,&rec_value);
			if( error_flag!=no_errors)
			{
				return_value=SPI_error;
			}  
		}
	}
	// Send checksum if CMD0 or CMD 8
	if(command==CMD0) 
	{
		send_value=0x95;
	}
	else if(command==CMD8) 
	{
		send_value=0x87;
	}
	else
	{
		send_value=0x01;  // end bit only, CRC7=0
	}
	if(return_value==no_errors)
	{
		error_flag=SPI_Transfer(send_value,&rec_value);
		if(error_flag!=no_errors)
		{
			return_value=SPI_error;
		}
	}
	return return_value;
 }

uint8_t receive_response(uint8_t num_bytes, uint8_t * rec_array)
{
	uint8_t return_value=no_errors;
	uint8_t timeout=0;
	uint8_t SPI_value;
	uint8_t error_flag = no_errors;
	uint8_t index = 0;

	do
	{
		error_flag=SPI_Transfer(0xFF,&SPI_value);
		timeout++;
	}while((SPI_value==0xFF)&&(timeout!=0)&&(error_flag==no_errors));
	
	if(error_flag!=no_errors)
	{  
		return_value=SPI_error;
	}
	else if(timeout==0)
	{
		return_value=SD_timeout_error;
	}
	// 0x00 and 0x01 are good values
	else if((SPI_value&0xFE)!=0x00)
	{
		*rec_array=SPI_value;
		return_value=comm_error;
	}
	else
	{
		*rec_array=SPI_value; // first received value (R1 resp.)
		if(num_bytes>1)
		{
			for(index=1;index<num_bytes;index++)
			{
				error_flag=SPI_Transfer(0xFF,&SPI_value);
				*(rec_array+index)=SPI_value;
			}
		}
	}
	error_flag=SPI_Transfer(0xFF,&SPI_value);
	return return_value;
}
