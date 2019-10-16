/* EXP2 - SPI and SD Card
 * Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 */


#include <stdio.h>

// local includes
#include "SD_Card.h"
#include "SPI_Interface.h"
#include "PORT.h"
//#include "delay.h"


sbit nCS0 = P1^4;
#define NO_ERROR 0x00
#define SPI_ERROR 0x01
#define TIMEOUT_ERROR 0x02

uint8_t send_command(uint8_t command, uint32_t argument) {
    uint8_t rec_value, argument_LSB, argument_byte1, argument_byte2, argument_MSB, command_end, error_flag;
    
    // check if CMD value is valid (less than 64)
    if (command >= 64) {
        return ILLEGAL_COMMAND;
    }
    
    // append start and transmission bits to the 6 bit command
    command |= 0x40;
    
    // set command_end based on necessary CRC7 and end bit
    if (command == 0x40) {      // CMD0
        command_end = 0x95;
    }
    else if (command == 0x48) { // CMD8
        command_end = 0x87;
    }
    else {                      // all other commands require no checksum
        command_end = 0x01;
    }
    
    // split argument into 4 bytes
    argument_LSB = argument & 0xFF;
    argument_byte1 = (argument >> 8) & 0xFF;
    argument_byte2 = (argument >> 16) & 0xFF;
    argument_MSB = (argument >> 24) & 0xFF;
    
    // send 6 byte command while checking for errors
    error_flag = SPI_transfer(command, &rec_value);
    if (error_flag != NO_ERROR) {
        return SPI_ERROR;
    }
    
    error_flag = SPI_transfer(argument_MSB, &rec_value);
    if (error_flag != NO_ERROR) {
        return SPI_ERROR;
    }    
    
    error_flag = SPI_transfer(argument_byte2, &rec_value);
    if (error_flag != NO_ERROR) {
        return SPI_ERROR;
    }    
    
    error_flag = SPI_transfer(argument_byte1, &rec_value);
    if (error_flag != NO_ERROR) {
        return SPI_ERROR;
    }    
    
    error_flag = SPI_transfer(argument_LSB, &rec_value);
    if (error_flag != NO_ERROR) {
        return SPI_ERROR;
    }    
    
    error_flag = SPI_transfer(command_end, &rec_value);
    if (error_flag != NO_ERROR) {
        return SPI_ERROR;
    }
    
    return NO_ERROR;
}

uint8_t receive_response(uint8_t num_bytes, uint8_t *byte_array) {
    uint8_t SPI_val, count = 0, error_flag;

    // Keep transmitting until a response is valid received or timeout occurs
	do {
        error_flag = SPI_transfer(0xFF, &SPI_val);
        count++;
    } while (((SPI_val&0x80) == 0x80) && (error_flag == NO_ERROR) && (count != 0));
    
	// Error handling
    if (error_flag != NO_ERROR) {
        return SPI_ERROR;
    }
	else if (count == 0) {
		return TIMEOUT_ERROR;
    }
    else {
        *byte_array = SPI_val;
     
        // if valid R1 response (active or idle)
        if ((SPI_val == 0x00) || (SPI_val == 0x01)) {
            if (num_bytes > 1) {
                for (count = 1; count < num_bytes; ++count) {
                    error_flag = SPI_transfer(0xFF, &SPI_val);
                    *(byte_array + count) = SPI_val;
                }
            }
        }
        else {
            return COMM_ERROR;
        }
    }
    
    // end with sending one last 0xFF out of the SPI port
    error_flag = SPI_transfer(0xFF, &SPI_val);

    return NO_ERROR;
}

uint8_t SD_card_init(void) {
    uint8_t idata receive_array[8], error_flag, timeout = 1, return_value, i, error_message;
	SPI_Master_Init(400000);
    timeout = 1;
    
    printf("Initializing SD card...\n");
    
    // 74+ clock pulses on SCK with nCS high
    nCS0 = 1;
    for(i = 0; i < 10; ++i) {
        error_flag = SPI_transfer(0xFF, &return_value);
    }
    
    /************
    *
    *  command 0
    *
    *************/
    
    // Check for error
    if (error_flag != NO_ERROR) {
        printf("SCK init error\n");
        return SD_INIT_ERROR;
    }
    
    // send CMD0 to SD card
    printf("CMD0 sent...\n");
    nCS0 = 0;
    error_flag = send_command(CMD0, 0);
    
    // check for error
    if (error_flag != NO_ERROR) {
        error_message = SEND_ERROR;
    }
    
    // receive response from SD card
    error_flag = receive_response(1, receive_array);
    nCS0 = 1;
    printf("CMD0 R1 Response expected\n");
    
    if (error_flag != NO_ERROR) {
        if (error_message == SEND_ERROR) {
            printf("CMD0 send error\n");
        }
        else {
            printf("CMD0 receive error\n");
        }
        
        return SD_INIT_ERROR;
    }
    else if (receive_array[0] != 0x01) {
        printf("CMD0 response incorrect");
        printf("Response received: 0x");
        printf("%2.2Bx", receive_array[0]);
        printf("...\n");
        
        return SD_INIT_ERROR;
    }
    
    printf("Response received: 0x");
    printf("%2.2Bx", receive_array[0]);
    printf("...\n");
    
    /************
    *
    *  command 8
    *
    *************/
    
    nCS0 = 0;
    
    // Send CMD8 to SD card
    printf("CMD8 sent...\n");
    error_flag = send_command(CMD8, 0x000001AA);
    
    // Check for error
    if (error_flag != NO_ERROR) {
        error_message = SEND_ERROR;
    }
    
    // Receive response from SD card
    error_flag = receive_response(6, receive_array);
    nCS0 = 1;
    
    // Check for error
    if (error_flag != NO_ERROR) {
        if(error_message == SEND_ERROR) {
            printf("CMD8 send error\n");
        }
        else {
            printf("CMD8 receive error\n");
        }
        
        return SD_INIT_ERROR;
    }
    
    // Print results
    printf("CMD8 R7 Response expected\n");
    
    if (receive_array[0] == 0x05) {
        printf("Version 1 SD Card is Not Supported\n");
        return SD_INIT_ERROR;
    }
    else {
        printf("Version 2 Card Detected\n");
        printf("Response received: 0x");
        
        for(i = 0;i < 5; i++){
            printf("%2.2Bx", receive_array[i]);
        }
        
        printf("...\n");
    }
    
    // Check for correct voltage
    if ((receive_array[3]&0x0F) != 0x01) {
        printf("CMD8 incorrect voltage error\n");
        return SD_INIT_ERROR;
    }
    
    // Check for matching check byte
    if (receive_array[4] != 0xAA) {
        printf("CMD8 check byte mismatch\n");
        return SD_INIT_ERROR;
    }
    
    /************
    *
    *  CMD58
    *
    *************/
        
    nCS0 = 0;
    
    // Send CMD58 to SD card
    printf("CMD58 sent...\n");
    error_flag = send_command(CMD58, 0);
    
    // Check for error
    if (error_flag != NO_ERROR) {
        error_message = SEND_ERROR;
    }
    
    // Receive response from SD card
    error_flag = receive_response(5, receive_array);
    nCS0 = 1;
    
    // Check for error
    if (error_flag != NO_ERROR) {
        if (error_message == SEND_ERROR) {
            printf("CMD58 send error\n");
        }
        else {
            printf("CMD58 receive error\n");
        }
        
        return SD_INIT_ERROR;
    }
    
    // Print results
    printf("CMD58 R3 Response expected\n");
    
    if ((receive_array[2]&0xFC) != 0xFC) {
        printf("CMD58 incorrect voltage error\n");
        return SD_INIT_ERROR;
    }
    else {
        printf("Response received: 0x");
        for(i = 0; i < 5; i++){
            printf("%2.2Bx", receive_array[i]);
        }
        printf("...\n");
    }
    
    /************
    *
    *  ACMD41
    *
    *************/
    
    // Sending command CMD55 and ACMD41 until the R1 response is 0 or a timeout occurs
    printf("ACMD41 sending...\n");
    while(receive_array[0] != 0) {
        // Send CMD55 to SD card
        nCS0 = 0;
        error_flag = send_command(CMD55, 0);
    
        // Check for error
        if (error_flag != NO_ERROR) {
            error_message = SEND_ERROR;
        }
    
        // Receive response from SD card
        error_flag = receive_response(1, receive_array);
    
        // Check for error
        if (error_flag != NO_ERROR) {
            if (error_message == SEND_ERROR) {
                printf("CMD55 send error\n");
            }
            else {
                printf("CMD55 receive error\n");
            }
        
            return SD_INIT_ERROR;
        }
        
        // Send ACMD41 to SD card
        error_flag = send_command(ACMD41, 0x40000000);
    
        // Check for error
        if (error_flag != NO_ERROR) {
            printf("ACMD41 send error\n");
            return SD_INIT_ERROR;
        }
    
        // Receive response from SD card
        error_flag = receive_response(1, receive_array);
    
        // Check for error
        if (error_flag != NO_ERROR) {
            printf("ACMD41 receive error\n");
            return SD_INIT_ERROR;
        }
        
        // Incriment timeout and check to see if it has reloaded
        timeout++;
        if (timeout == 0) {
            printf("ACMD41 timeout error\n");
            return SD_INIT_ERROR;
        }
        nCS0 = 1;
    }
    
    // Print results
    printf("ACMD41 R1 Response Expected\n");
    printf("Response received: 0x");
    printf("%2.2Bx", receive_array[0]);
    printf("...\n");

    /************
    *
    *  CMD58 Again
    *
    *************/
        
    nCS0 = 0;
    
    // Send CMD58 to SD card
    printf("CMD58 sent...\n");
    error_flag = send_command(CMD58, 0);
    
    // Check for error
    if (error_flag != NO_ERROR) {
        error_message = SEND_ERROR;
    }
    
    // Receive response from SD card
    error_flag = receive_response(5, receive_array);
    nCS0 = 1;
    
    // Check for error
    if (error_flag != NO_ERROR) {
        if (error_message == SEND_ERROR) {
            printf("CMD58-2 send error\n");
        }
        else {
            printf("CMD58-2 receive error\n");
        }
        
        return SD_INIT_ERROR;
    }
    
    // Print results
    printf("R3 Response expected\n");
    
    if ((receive_array[1]&0x80) != 0x80) {
        printf("CMD58 card not in active state error\n");
        return SD_INIT_ERROR;
    }
    else if ((receive_array[1]&0xC0) == 0xC0) {
        printf("High capacity card accepted\n");
        printf("Response received: 0x");
        for(i = 0; i < 5; i++){
            printf("%2.2Bx", receive_array[i]);
        }
        printf("...\n");
    }
    else {
        printf("High capacity card not detected error");
        return SD_INIT_ERROR;
    }
        
    printf("Initialization of SD card complete...\n\n");
    
    return NO_ERROR;
}

uint8_t read_block(uint16_t num_bytes, uint8_t * byte_array) {
    uint8_t SPI_val, error_flag = NO_ERROR;
    uint16_t idata count = 0;
    
    // keep transmitting until a error_flag is received or timeout occurs
	do {
        error_flag = SPI_transfer(0xFF, &SPI_val);
        count++;
    } while (((SPI_val & 0x80) == 0x80) && (error_flag == NO_ERROR) && (count != 0));
    
    // error handling
    if (error_flag != NO_ERROR) {
        error_flag = SPI_ERROR;
    }
	else if (count == 0) {
		error_flag = TIMEOUT_ERROR;
    }
    else {
        if (SPI_val == 0x00) {
            count = 0;
            
            // wait for data token
            do {
                error_flag = SPI_transfer(0xFF, &SPI_val);
                count++;
            } while ((SPI_val == 0xFF) && (error_flag == NO_ERROR) && (count != 0));
            
            if (error_flag != NO_ERROR) {
                error_flag = SPI_ERROR;
            }
            else if (count == 0) {
                error_flag = TIMEOUT_ERROR;
            }
            // if data token
            else if (SPI_val == 0xFE) {
                for (count = 0; count < num_bytes; ++count) {
                    error_flag = SPI_transfer(0xFF, &SPI_val);
                    *(byte_array + count) = SPI_val;
                }
                
                // discard CRC
                error_flag = SPI_transfer(0xFF, &SPI_val);
                error_flag = SPI_transfer(0xFF, &SPI_val);
            }
            else {
                error_flag = DATA_ERROR;
            }
        }
        else {
            error_flag = COMM_ERROR;
        }
    }
    
    // return to standby state
    error_flag = SPI_transfer(0xFF, &SPI_val);
    
    return error_flag;
}

/*
#include "SD_Card.h"
#include "SPI_Interface.h"
#include <stdio.h>

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


uint8_t  SD_Card_Init(void)
{
	uint8_t received_value; //used to hold value returned from SPI_Transfer
	uint8_t error_status; //used to hold error value
	uint8_t index = 0; //index for for loops
	uint8_t rec_array[5];

	printf("Starting SPI_Master_init...\n");
	error_status = SPI_Master_Init(clock_frequency);
	printf("sd_card_init error_status: %x\n", error_status);
	printf("Ending SPI_Master_init...\n");
	//send at least 74 clock cycles (rounded to 80, 10x 8 bytes)
	if(error_status == no_errors)
	{
		SD_select = 1;
		for(index = 0; index < 10; index++)
		{
			//sends each clock cycle
			error_status = SPI_Transfer(0x00, &received_value);
			//printf("%u", error_status);
		}
		printf("After sending 80 clock cyles:: %x\n", received_value);
		printf("Initial clock cyles sent to SD\n");
	}
	else
	{
		printf("Error Occured.\n");
	}

	//CMD0 Place SD Card into SPI Mode and IDLE state
	if(error_status == no_errors)
	{
		SD_select = 0;
		//Send CMD0 command
		error_status = send_command(CMD0, 0x00);
		printf("error_status before in init: %x\n", error_status);
		error_status = receive_response(1, &rec_array);
		printf("CMD0 Sent \n");
		printf("error_status after in init: %x\n", error_status);
		SD_select = 1;
	}
	//printf("%u\n", error_status);
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
		printf("CMD8 Sent \n");
		//printf("%u\n",error_status);
	}
/*
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
	uint8_t error_flag = no_errors;
	uint8_t return_value = no_errors;
	uint8_t send_value;
	uint8_t rec_value;
	uint8_t index = 0;

	//check if command is 63 bits
	if(command<64)
	{
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
	}
	else
	{
		return_value=illegal_command;
	}
	return return_value;
}

uint8_t receive_response(uint8_t num_bytes, uint8_t * rec_array)
{
	uint8_t return_value = no_errors;
	uint16_t timeout = 0;
	uint8_t SPI_value;
	uint8_t error_flag = no_errors;
	uint8_t index = 0;

	do
	{
		error_flag = SPI_Transfer(0xFF,&SPI_value);
		timeout++;
	}while((SPI_value==0xFF)&&(timeout!=0)&&(error_flag==no_errors));
	//printf("receive response first transfer:: %u\n", error_flag);
	printf("SPI_value after do..while loop ended %x\n", SPI_value);
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
				printf("Receiving.... %x\n", SPI_value);

			}
		}
	}
	error_flag = SPI_Transfer(0xFF,&SPI_value);
	printf(" Return_value after response_receive() %x\n", return_value);
	return return_value;
}
*/
