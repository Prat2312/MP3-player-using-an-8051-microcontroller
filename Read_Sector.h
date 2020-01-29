#ifndef _read_sector_H
#define _read_sector_H

#include "Main.H"
#include "AT89C51RC2.h"
#include "PORT.H"
#include "SDCard.h"
#include "print_bytes.h"
#include "SPI.h"
#include <stdio.h>



#define Disk_Error (0xF0)
#define No_Disk_Error (0)
#define not_found (0xFF)
#define read_error (0xFF)

uint8_t Read_Sector(uint32_t sector_number, uint16_t sector_size, uint8_t * array_for_data);

#endif
