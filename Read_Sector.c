#include "main.h"
#include "read_sector.h"
#include "Directory_Functions.h"
#include "PORT.h"



uint8_t Read_Sector(uint32_t sector_number, uint16_t sector_size, uint8_t * array_for_data)
{
	uint8_t SDtype,error_flag = No_Disk_Error;
	SDtype = Return_SD_Card_Type(); // HC=0, no change to sector number
// SC=9, multiplies sector number by 512 to convert to byte addr.
	nCS0=0;
	error_flag = SEND_COMMAND(17,(sector_number<<SDtype));
	if(error_flag == no_errors)
		error_flag = read_block(sector_size,array_for_data);
	//print_memory(array_for_data, 512);
	nCS0=1;
	if(error_flag != no_errors)
	{
		error_flag=Disk_Error;
	}
	return error_flag;
}
