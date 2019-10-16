/* EXP2 - SPI and SD Card
 * Names: Tyler Andrews, Brennan Campbell, Tyler Tetens
 */


#include <stdio.h>

#include "SD_Card.h"
#include "SPI_Interface.h"
#include "PORT.h"

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
			return_value=spi_error;
		}
		if(return_value==no_errors)
		{
			// Send the rest of the command
			for(index=0;index<4;index++)
			{
				if(return_value==no_errors)
				{   
					send_value=(uint8_t)(argument>>(24-(index*8)));
					error_flag=SPI_Transfer(send_value,&rec_value);
					if( error_flag!=no_errors)
					{
						return_value=spi_error;
					}  
				}
			}
		}
		// Send checksum if CMD0 
		if(command==CMD0) 
		{
			send_value=0x95;
		}
		// Send checksum if CMD8
		else if(command==CMD8) 
		{
			send_value=0x87;
		}
		// Transmission bits
		else
		{
			send_value=0x01; 
		}
		if(return_value==no_errors)
		{
			error_flag=SPI_Transfer(send_value,&rec_value);
			if(error_flag!=no_errors)
			{
				return_value=spi_error;
			}
		}
	}
	//if cmd is over 63 bits then it is ilegal_command
	else
	{
		return_value=illegal_command;
	}
	return return_value;
}

uint8_t receive_response(uint8_t num_bytes, uint8_t *rec_array) 
{
    uint8_t return_value = no_errors;
	uint16_t timeout = 0;
	uint8_t SPI_value;
	uint8_t error_flag = no_errors;
	uint8_t index = 0;

	// Send 0xFF until R1 response is received
	do
	{
		error_flag = SPI_Transfer(0xFF,&SPI_value);
		timeout++;
	}while((SPI_value==0xFF)&&(timeout!=0)&&(error_flag==no_errors));
	//error checking
	if(error_flag!=no_errors)
	{  
		return_value=spi_error;
	}
	else if(timeout==0)
	{
	}
	// Check for either 0x00 or 0x01. Both are good values.
	else if((SPI_value&0xFE)!=0x00)
	{
		*rec_array=SPI_value;
		return_value=comm_error;
	}
	else
	{
		// first received value (R1 resp.)
		*rec_array=SPI_value; 
		if(num_bytes>1)
		{
			// Grab rest of response if number of bytes is greater than 1
			for(index=1;index<num_bytes;index++)
			{
				error_flag=SPI_Transfer(0xFF,&SPI_value);
				*(rec_array+index)=SPI_value;
				printf("Receiving.... %x\n", SPI_value);

			}
		}
	}
	error_flag = SPI_Transfer(0xFF,&SPI_value);
	return return_value;
}

uint8_t SD_card_init(void) 
{
    uint8_t rec_array[8];
	uint8_t error_flag;
 	uint8_t timeout = 1;
	uint8_t return_value;
 	uint8_t index; 
	uint8_t error_message;
	
	SPI_Master_Init(spi_clock_freq);
    
	timeout = 1;
    
    printf("Starting Init of SD card...\n");
    
    // Sending 80 clock cycles with SD_select high
    SD_select = 1;
    for(index = 0; index < 10; ++index)
    {
        error_flag = SPI_Transfer(0xFF, &return_value);
    }
    
	// Starting CMD0
    // Check for error
    if (error_flag != no_errors) 
	{
        printf("Error Sending Clock\n");
        return sd_init_error;
    }
    
    // send CMD0 to SD card
    printf("CMD0 has been sent...\n");
    SD_select = 0;
    error_flag = send_command(CMD0, 0);
    
    // check for error
    if (error_flag != no_errors)
    {
        error_message = send_error;
    }
    
    // receive response from SD card
    error_flag = receive_response(1, rec_array);
    SD_select = 1;
    printf("CMD0 R1 Response expected\n");
    
    if (error_flag != no_errors)
    {
        if (error_message == send_error) 
		{
            printf("CMD0 send error\n");
        }
        else 
		{
            printf("CMD0 receive error\n");
        }
        
        return sd_init_error;
    }
    else if (rec_array[0] != 0x01)
    {
        printf("CMD0 response incorrect");
        printf("Response received: 0x");
        printf("%2.2Bx", rec_array[0]);
        printf("...\n");
        return sd_init_error;
    }
    
    printf("Response received: 0x");
    printf("%2.2Bx", rec_array[0]);
    printf("...\n");

    // Starting CMD8
    SD_select = 0;
    
    // Send CMD8 to SD card
    printf("CMD8 has been sent...\n");
    error_flag = send_command(CMD8, 0x000001AA);
    
    // Check for error
    if (error_flag != no_errors)
    {
        error_message = send_error;
    }
    
    // Receive response from SD card
    error_flag = receive_response(6, rec_array);
    SD_select = 1;
    
    // Check for error
    if (error_flag != no_errors) 
	{
        if(error_message == send_error) 
		{
            printf("CMD8 send error\n");
        }
        else
	    {
            printf("CMD8 receive error\n");
        }
        
        return sd_init_error;
    }
    
    // Print results
    printf("CMD8 R7 Response expected\n");
    
    if (rec_array[0] == 0x05)
	{
        printf("Version 1 SD Card is Not Supported\n");
        return sd_init_error;
    }
    else 
	{
        printf("Version 2 Card Detected\n");
        printf("Response received: 0x");
        
        for(index = 0;index < 5; index++)
		{
            printf("%2.2Bx", rec_array[index]);
        }
        
        printf("...\n");
    }
    
    // Check for correct voltage
    if ((rec_array[3]&0x0F) != 0x01)
	{
        printf("CMD8 incorrect voltage error\n");
        return sd_init_error;
    }
    
    // Check for matching check byte
    if (rec_array[4] != 0xAA)
    {
        printf("CMD8 byte check failed\n");
        return sd_init_error;
    }
    
    // Starting CMD58
    SD_select = 0;
    
    // Send CMD58 to SD card
    printf("CMD58 has been sent...\n");
    error_flag = send_command(CMD58, 0);
    
    // Check for error
    if (error_flag != no_errors)
    {
        error_message = send_error;
    }
    
    // Receive response from SD card
    error_flag = receive_response(5, rec_array);
    SD_select = 1;
    
    // Check for error
    if (error_flag != no_errors)
    {
        if (error_message == send_error)
	    {
            printf("CMD58 send error\n");
        }
        else 
		{
            printf("CMD58 receive error\n");
        }
        
        return sd_init_error;
    }
    
    // Print results
    printf("CMD58 R3 Response expected\n");
    
    if ((rec_array[2]&0xFC) != 0xFC)
    {
        printf("CMD58 incorrect voltage error\n");
        return sd_init_error;
    }
    else {
        printf("Response received: 0x");
        for(index = 0; index < 5; index++)
		{
            printf("%2.2Bx", rec_array[index]);
        }
        printf("...\n");
    }

    // Start ACMD41

    // Sending command CMD55 and ACMD41 until the R1 response is 0 or a timeout occurs
    printf("ACMD41 sending started...\n");
    while(rec_array[0] != 0)
    {
        // Send CMD55 to SD card
        SD_select = 0;
        error_flag = send_command(CMD55, 0);
    
        // Check for error
        if (error_flag != no_errors)
	    {
            error_message = send_error;
        }
    
        // Receive response from SD card
        error_flag = receive_response(1, rec_array);
    
        // Check for error
        if (error_flag != no_errors)
	    {
            if (error_message == send_error) 
			{
                printf("CMD55 send error\n");
            }
            else
		    {
                printf("CMD55 receive error\n");
            }
        
            return sd_init_error;
        }
        
        // Send ACMD41 to SD card
        error_flag = send_command(ACMD41, 0x40000000);
    
        // Check for error
        if (error_flag != no_errors)
	    {
            printf("ACMD41 send error\n");
            return sd_init_error;
        }
    
        // Receive response from SD card
        error_flag = receive_response(1, rec_array);
    
        // Check for error
        if (error_flag != no_errors)
	    {
            printf("ACMD41 receive error\n");
            return sd_init_error;
        }
        
        // Incriment timeout and check to see if it has reloaded
        timeout++;
        if (timeout == 0) 
		{
            printf("ACMD41 timeout error\n");
            return sd_init_error;
        }
        SD_select = 1;
    }
    
    // Print results
    printf("ACMD41 R1 Response Expected\n");
    printf("Response received: 0x");
    printf("%2.2Bx", rec_array[0]);
    printf("...\n");

    // Start CMD58 for second time
    SD_select = 0;
    
    // Send CMD58 to SD card
    printf("CMD58 sent...\n");
    error_flag = send_command(CMD58, 0);
    
    // Check for error
    if (error_flag != no_errors)
    {
        error_message = send_error;
    }
    
    // Receive response from SD card
    error_flag = receive_response(5, rec_array);
    SD_select = 1;
    
    // Check for error
    if (error_flag != no_errors)
    {
        if (error_message == send_error)
	    {
            printf("CMD58 send error\n");
        }
        else
	    {
            printf("CMD58 receive error\n");
        }
        
        return sd_init_error;
    }
    
    // Print results
    printf("R3 Response expected\n");
    
    if ((rec_array[1]&0x80) != 0x80)
    {
        printf("CMD58 card not in active state error\n");
        return sd_init_error;
    }
    else if ((rec_array[1]&0xC0) == 0xC0)
    {
        printf("High capacity card accepted\n");
        printf("Response received: 0x");
        for(index = 0; index < 5; index++)
		{
            printf("%2.2Bx", rec_array[index]);
        }
        printf("...\n");
    }
    else
    {
        printf("High capacity detection error");
        return sd_init_error;
    }
        
    printf("SD Card Initialized...\n\n");
    
    return no_errors;
}

uint8_t read_block(uint16_t num_bytes, uint8_t *rec_array) 
{
    uint8_t SPI_val;
    uint8_t error_flag = no_errors;
    uint16_t count = 0;
    
	SD_select = 0;
    // keep transmitting until a error_flag is received or timeout occurs
	do {
        error_flag = SPI_Transfer(0xFF, &SPI_val);
        count++;
    } while (((SPI_val & 0x80) == 0x80) && (error_flag == no_errors) && (count != 0));
    
    // error handling
    if (error_flag != no_errors) {
        error_flag = spi_error;
		printf("SPI ERROR!");
    }
	else if (count == 0) {
		error_flag = timeout_error;
		printf("Timeout ERROR1!");
    }
    else {
        if (SPI_val == 0x00) {
            count = 0;
            
            // wait for data token
            do {
                error_flag = SPI_Transfer(0xFF, &SPI_val);
                count++;
            } while ((SPI_val == 0xFF) && (error_flag == no_errors) && (count != 0));
            
            if (error_flag != no_errors) {
                error_flag = spi_error;
				printf("SPI ERROR!");
            }
            else if (count == 0) {
                error_flag = timeout_error;
				printf("Timeout ERROR2!");
            }
            // if data token
            else if (SPI_val == 0xFE) {
                for (count = 0; count < num_bytes; ++count) {
                    error_flag = SPI_Transfer(0xFF, &SPI_val);
                    *(rec_array + count) = SPI_val;
                }
                // discard CRC
                error_flag = SPI_Transfer(0xFF, &SPI_val);
                error_flag = SPI_Transfer(0xFF, &SPI_val);
            }
            else {
                error_flag = data_error;
				printf("DATA ERROR!");
            }
        }
        else {
            error_flag = comm_error;
			printf("COMM ERROR!");
        }
    }
    
    // Send extra byte
    error_flag = SPI_Transfer(0xFF, &SPI_val);
	SD_select = 1;
    return error_flag;
}


