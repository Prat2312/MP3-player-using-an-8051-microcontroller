#ifndef _File_System_H
#define _File_System_H

#include "Main.H"
#include "Directory_Functions.h"
#include "read_sector.h"


#define Relative_Sectors (8192)
//#define Bytes_per_sec (512)

uint8_t read8(uint16_t offset, uint8_t * array_name);
uint16_t read16(uint16_t offset, uint8_t * array_name);
uint32_t read32(uint16_t offset, uint8_t * array_name);
uint8_t Mount_Drive(uint8_t xdata * array_read);
void BIOS_parameter_block();
uint32_t First_Sector(uint32_t Cluster_Num);
uint32_t Find_Next_Clus(uint32_t cluster_num, uint8_t*array);
uint8_t open_file(uint32_t Clus_num, uint8_t *array_name);
#endif
