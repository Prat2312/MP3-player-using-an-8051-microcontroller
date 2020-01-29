#ifndef _SDCard_H
#define _SDCard_H

#include "Main.H"

#define no_errors (0)
#define sd_timeout_error (0x81)
#define illegal_cmnd (0x82)
#define response_error (0x83)
#define data_error (0x84)
#define voltage_error (0x85)
#define card_inactive (0x86)
#define SPI_error (0x87)
#define unknown (0xFF)
#define Ver2 (0x02)
#define Standard_Capacity (9)  
#define High_Capacity (0)      
#define CMD0 (0)
#define CMD8 (8)
#define CMD16 (16)
#define CMD17 (17)
#define CMD55 (55)
#define CMD58 (58)
#define ACMD41 (41)



uint8_t SEND_COMMAND(uint8_t cmnd, uint32_t argum);
uint8_t response(uint8_t num, uint8_t * valout_p);
uint8_t read_block(uint16_t num, uint8_t * array_out_p);
uint8_t SD_card_init(void);
uint8_t Return_SD_Card_Type(void);
void print_error(uint8_t error);


#endif