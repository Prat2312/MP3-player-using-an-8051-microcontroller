#include <stdio.h>
#include "main.h"
#include "print_bytes.h"
#include "UART.h"
#include "SPI.h"
#include "Timer0_hardware_delay_1ms.h"
#include "Long_Serial_In.h"
#include "LCD_Routines.h"
#include "LCD_hardware_functions.h"

uint8_t Send_Command(uint8_t CMD_value, uint32_t argument)
{	uint8_t error_flag,send_value,return_value,no_error,SPI_error,rec_value,;
	// First check to see if CMD_valueis valid (i.e. <64).
	if(CMD_value<64)
	{
		// If it is valid, then start sending the command 
		return_value=no_errors;
		// If it is not valid exit the function with an illegal command error
		return_value=illegal_command;
		// The first byte to send is the CMD_valuewith the start and transmission bits
		send_value=0x40|CMD_value;
		// Send using the SPI_Transferfunction
		// A error flag can be used to verify the SPI transfer was succesful.
		// The received value is not used in this function.
		error_flag=SPI_Transfer(send_value,&rec_value);
	}// If an error has occurred, then update the return value
	if(error_flag!=no_errors)
	{
		return_value=SPI_error;
	}
	//   Send the argument next starting with the most significant byte.  Right 
	// shifting the argument by 24 will place the MSB in the lowest byte which is 
	// then assigned to send_value.
	if(return_value==no_errors)
	{
		send_value=(uint8_t)(argument>>24);// Send using the SPI_Transferfunction
		error_flag=SPI_Transfer(send_value,&rec_value);
		if(error_flag!=no_errors)
		{
			return_value=SPI_error;
		}
	}
	//   Repeat for each byte of the argument.
	if(return_value==no_errors)
	{
		send_value=(uint8_t)(argument>>16);
		error_flag=SPI_Transfer(send_value,&rec_value);
		if(error_flag!=no_errors)
		{
			return_value=SPI_error;
		}
	}
	send_value=(uint8_t)(argument>>8);
	send_value=(uint8_t)(argument);
	//   The programmer may wish to place this into a for loop instead 
	// The index of the for loop is used to determine how much to shift
	for(index=0;index<4;index++)
	{
		if( return_value==no_errors)
		{
			send_value=(uint8_t)(argument>>(24-(index*8)));
			error_flag=SPI_Transfer(send_value,&rec_value);
			if(error_flag!=no_errors)
			{
				return_value=SPI_error;
			}
		}
	}
	// The final byte to send is determined by the CMD_value.
	if(CMD_value==CMD0) 
	{
		send_value=0x95;
	}
	else if(CMD_value==CMD8) 
	{
		send_value=0x87;
	}
	else{send_value=0x01;  // end bit only, CRC7=0}
	//   Send using the SPI_Transferfunction
	if(return_value==no_errors)
	{
		error_flag=SPI_Transfer(send_value,&rec_value);
		if(error_flag!=no_errors)
		{
			return_value=SPI_error;
		}
	}//   Return the error flag.return return_value;}

uint8_t receive_response(uint8_t num_bytes,uint8_t * rec_array);
// First set the return value to no_errorsand initialize the timeout counter
return_value=no_errors;timeout=0;// Use a loop to repeatedly receive values until the R1 response is received,
// a timeout occurs, or an SPI error occurs.
do
{
error_flag=SPI_Transfer(0xFF,&SPI_value);timeout++;
}
while((SPI_value==0xFF)&&(timeout!=0)&&(error_flag==no_errors));
SD Card – Command & Response // Check for SPI error, timeout error or communication error
if(error_flag!=no_errors)
{
  return_value=SPI_error;
}
else if(timeout==0)
{
return_value=SD_timeout_error;
}
else if((SPI_value&0xFE)!=0x00) // 0x00 and 0x01 are good values
{
*rec_array=SPI_value;  // return the value to see the error
return_value=comm_error;
}
else
{
// Receive the rest of the bytes (if there are more to receive).
// Receive the rest of the bytes (if there are more to receive).
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
}// Send one SPI transfer after response is completely received
error_flag=SPI_Transfer(0xFF,&SPI_value);

if(error_status==no_errors)
{
Clear_SPI_bit(SD_select);
error_flag=Send_Command(CMDx,argument);
if(  error_flag==no_errors)
{
error_flag=Receive_Response(num_bytes,rec_array);
}
Set  _SPI_bit(SD_select);
if(  error_flag!=no_errors)
{
error_status=error_flag;
}
else if(rec_array[0]!=0x01)
{
error_status=response_error;
}	
