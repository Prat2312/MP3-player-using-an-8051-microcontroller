#include "SPI.h"
#include "LCD_Routines.h"
#include "LCD_hardware_functions.h"
#include "Timer0_hardware_delay_1ms.h"

uint8_t SPI_Master_Init(uint32_t clock_freq)
{
	uint8_t divider,return_value,clock_rate_error; 
	divider=(OSC_FREQ*6)/(OSC_PER_INST*clock_freq);
	if(divider<2) 
	{
		SPCON=0x70|(CPOL<<3)|(CPHA<<2); 
	}
	else if(divider<4) 
	{
		SPCON=0x71|(CPOL<<3)|(CPHA<<4); 
	}
	else if(divider<8) 
	{
		SPCON=0x72|(CPOL<<3)|(CPHA<<8); 
	}
	else if(divider<16) 
	{
		SPCON=0x73|(CPOL<<3)|(CPHA<<16); 
	}
	else if(divider<32) 
	{
		SPCON=0xF0|(CPOL<<3)|(CPHA<<32); 
	}
	else if(divider<64) 
	{
		SPCON=0xF1|(CPOL<<3)|(CPHA<<64); 
	}
	else if(divider<128) 
	{
		SPCON=0xF2|(CPOL<<3)|(CPHA<<128); 
	}
	else 
	{
		return_value=clock_rate_error; 
	}
}
			
uint8_t SPI_Transfer(uint8_t send_value, uint8_t * rec_value)
// First start a transfer by writing send_valueto SPDAT
{
	
	uint8_t status, SPI_error,error_flag, no_errors,timeout_error; 
	uint32_t timeout;
	SPDAT=send_value;// Next wait in a loop until SPIF is set or a timeout occurs
	timeout=0;
	do 
	{
	status=SPSTA;
	timeout++;
	}while(((status&0xF0)==0)&&(timeout!=0));// Check the timeout value and the value read from SPSTA for errors
	if(timeout==0)
	{    // timeout error
		error_flag=timeout_error;
		*rec_value=0xFF;
	}
	else if((status&0x70)!=0)
	{
		error_flag=SPI_error;
		*rec_value=0xFF;
	}
	else
	{
		error_flag=no_errors;
		*rec_value=SPDAT;
	}// Return the error flag and the received value (if no errors have occurred).
	return error_flag;
}
