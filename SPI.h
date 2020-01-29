#ifndef _SPI_H
#define _SPI_H
#include "Main.H"

#define CPOL (0)
#define CPHA (0)
uint8_t SPI_Master_Init(uint32_t clock_freq);
uint8_t SPI_Transfer(uint8_t send_value, uint8_t * rec_value);

#endif
