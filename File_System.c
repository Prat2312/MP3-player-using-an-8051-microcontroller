#include "main.h"
#include "read_sector.h"
#include "File_System.h"
#include "Directory_Functions.h"
#include <stdio.h>




extern uint32_t idata  FirstDataSec_g, StartofFAT_g, FirstRootDirSec_g, RootDirSecs_g;
extern uint16_t idata  BytesPerSec_g;
extern uint8_t idata SecPerClus_g, FATtype_g, BytesPerSecShift_g,FATshift_g;

uint8_t xdata array_read[512];



///////////////////////////////////////////////////////////////////////////////

uint8_t Mount_Drive(uint8_t xdata * array_read)
{
	uint8_t error_flag;
	uint8_t first_byte;	
	uint8_t eight_bit_value;	
	uint32_t  BPB;	
	
	error_flag = Read_Sector(0, 512, array_read);						
	if(error_flag == no_errors)
	{
		eight_bit_value = read8(0,array_read);
		printf("\n eight_bit_valu = %bu \n",eight_bit_value);
		if(eight_bit_value!= 0xEB && eight_bit_value!= 0xE9)
		{
			BPB = read32(0x01C6, array_read);
			printf("\n Rel_Sec= %lu \n",BPB);
			error_flag = Read_Sector(BPB, 512, array_read);
			if(error_flag == no_errors)
			{	
				first_byte = read8(0,array_read);
				printf("\n first_byte = %bu \n",first_byte);
				if(first_byte != 0xEB && first_byte != 0xE9)
				{ 
				error_flag = not_found;					//change to appropriate error
				printf("\n BPB not_found \n");
				}											
				else
				{
					printf("\n First block is not BPB \n",BPB);
				BIOS_parameter_block();
				}
			}
			else
			{
				error_flag = read_error;
			}

		}
		else
		{
		printf("\n First block is BPB \n",BPB);
		BIOS_parameter_block();
		}

	}
	else 
	{ 
		error_flag = read_error;
	}
return error_flag;
}





void BIOS_parameter_block()
	{
		uint32_t FATSz,DataSec,CountofClusters ;
		uint32_t TotalSec;
 		uint16_t  Rsvd_Sec_Count ;
 		uint8_t Num_FAT ;
 		uint16_t  Root_Entry_Count;
		 uint16_t  Total_Sec_16;	
		 uint16_t FAT_Size_16 ;
 		uint32_t   Total_Sec_32 ;
 		uint32_t  FAT_Size_32;
		 uint32_t  Rootr_Clus ;


   		 BytesPerSec_g = read16(0x0B, array_read);
		 	 printf("\n BytesPerSec_g = %u \n",BytesPerSec_g );
		SecPerClus_g =  read8(0x0D, array_read);
			 printf("\n SecPerClus_g = %bu \n",SecPerClus_g );
		Rsvd_Sec_Count = read16(0x0E,array_read);
			 printf("\n Rsvd_Sec_Count = %u \n",Rsvd_Sec_Count );
		Num_FAT = read8(0x10, array_read);
			 printf("\n Num_FAT = %bu \n",Num_FAT );
		Root_Entry_Count = read16(0x11,array_read);
			 printf("\n Root_Entry_Count = %u \n",Root_Entry_Count );
		Total_Sec_16 = read16(0x13,array_read);
			 printf("\n Total_Sec_16 = %u \n",Total_Sec_16 );
		FAT_Size_16 = read16(0x16,array_read);
			 printf("\n FAT_Size_16 = %u \n",FAT_Size_16 );
		Total_Sec_32 = read32(0x20,array_read);
			 printf("\n Total_Sec_32 = %lu \n",Total_Sec_32 );
		FAT_Size_32 = read32(0x24,array_read);
			 printf("\n FAT_Size_32 = %lu \n",FAT_Size_32 );
		Rootr_Clus = read32(0x2C,array_read);
			 printf("\n Rootr_Clus = %lu \n",Rootr_Clus );
		
	 RootDirSecs_g = ((Root_Entry_Count*32) + ( BytesPerSec_g -1))/ BytesPerSec_g;
     printf("\n RootDirSecs_g = %lu \n",RootDirSecs_g );
	if(FAT_Size_16!=0)
	{
		FATSz = FAT_Size_16;
	}
	else
	{
		FATSz = FAT_Size_32;
	}
	  printf("\n FATSz = %lu \n",FATSz );
		if(Total_Sec_16!= 0)
	{
		TotalSec = Total_Sec_16;
	}
		else
	{
			TotalSec = Total_Sec_32;
	}
	 printf("\n TotalSec = %lu \n",TotalSec );

	DataSec = TotalSec - (Rsvd_Sec_Count + (Num_FAT*FATSz) +  RootDirSecs_g );
	 printf("\n DataSec = %lu \n",DataSec );
	CountofClusters = DataSec/SecPerClus_g;
	 printf("\n CountofClusters = %lu \n",CountofClusters );
	if(CountofClusters < 65525) 
	{
	 printf("\n CARD is FAT16 \n");
	 FATtype_g = FAT16;
	}
	else
	{
	printf("\n CARD is FAT32 \n");
	 FATtype_g = FAT32;
	}
	
	 StartofFAT_g = Rsvd_Sec_Count + Relative_Sectors; 
	
	 FirstDataSec_g = 	 StartofFAT_g + (Num_FAT*FATSz) +  RootDirSecs_g;
	
	if( FATtype_g == FAT16)
	{
	 FirstRootDirSec_g = Rsvd_Sec_Count + (Num_FAT*FATSz) + Relative_Sectors; //FAT16
	}
	else
	{
	 FirstRootDirSec_g = ((Rootr_Clus - 2)* SecPerClus_g ) +  FirstDataSec_g;	//FAT32
	}
}



uint32_t First_Sector(uint32_t Cluster_Num)
{
	uint32_t First_Sector_of_Cluster;
	First_Sector_of_Cluster = ((Cluster_Num - 2)* SecPerClus_g) +  FirstDataSec_g;  //cluster 0 is a problem??
	return First_Sector_of_Cluster;
}



uint32_t Find_Next_Clus(uint32_t cluster_num, uint8_t*array)
{
	uint16_t Offset;
	uint32_t return_clus,error_flag,Sector;
	uint8_t FATtype_g;

	
if( FATtype_g == FAT16)
	FATtype_g = FAT16;
else
	FATtype_g = FAT32;

Sector = ((cluster_num * FATtype_g)/ BytesPerSec_g) +  StartofFAT_g;
error_flag = Read_Sector(Sector,  BytesPerSec_g, array);
Offset = ((cluster_num * FATtype_g)% BytesPerSec_g);

if(FATtype_g == FAT32)
{
return_clus = read32(Offset, array);
	return_clus &= 0x0FFFFFFF;
	}
else if(FATtype_g == FAT16)
{
return_clus = (uint32_t)read16(Offset, array);
}
return return_clus;
}

/////////////////////////////////////////////////////////////


uint8_t read8(uint16_t offset, uint8_t * array_name)
{
uint8_t return_val;
uint8_t temp;
return_val = 0;
offset &= 0x1FF; // limit offset to 0 to 511
temp = *(array_name+offset);
return_val = temp;
//printf("\n 8 bits = %bu \n", return_val);
return return_val;
}

///////////////////////////////////////////////////////////////

uint16_t read16(uint16_t offset, uint8_t * array_name)
{
uint16_t return_val;
uint8_t temp, i;
return_val = 0 ;
offset &= 0x1FF; // limit offset to 0 to 511
for(i=0;i<2;i++)
{
temp=*(array_name+offset+(1-i));
return_val = return_val<<8;
return_val |= temp;
}
//printf("\n 16 bits = %u \n", return_val);
return return_val;
}

///////////////////////////////////////////////////////////////////////

uint32_t read32(uint16_t offset, uint8_t * array_name)
{
uint32_t return_val;
uint8_t temp, i;
return_val = 0;
offset &= 0x1FF; // limit offset to 0 to 511
for(i=0;i<4;i++)
{
temp=*(array_name+offset+(3-i));
return_val = return_val<<8;
return_val |= temp;
}
//printf("\n 32 = %lu \n", return_val);
return return_val;
}

//////////////////////////////////////////////////////////

uint8_t open_file(uint32_t Clus_num, uint8_t *array_name)
{
	uint8_t key_pressed, error_flag; 
	uint32_t Sector, First_sector, Offset, Current_Clus;

	Current_Clus = Clus_num;

	First_sector = First_Sector(Current_Clus);
	Offset = 0;
	Sector = First_sector;
	do
	{
		Sector = Sector + Offset;
		if(Offset == 0)
		{
			Sector = ((Current_Clus - 2)*SecPerClus_g) + FirstDataSec_g;
		}
		error_flag == Read_Sector(Sector, 512, array_name);
		if(error_flag == no_errors)
		{
			print_memory(array_name, 512);
			Offset++;
			if(Offset == SecPerClus_g)
			{
				Offset = 0;
				Current_Clus = Find_Next_Clus(Current_Clus, array_name);
			}
			printf("Press X to exit, others to continue \n");
			key_pressed = UART_Receive();
		}
		else
		{
			key_pressed = 0x78;
		}
		//printf("\n %bu is pressed \n", key_pressed);
	}while(key_pressed != 120 && key_pressed != 88);
	//printf("\n exit the loop \n", key_pressed);
	return error_flag;
}

