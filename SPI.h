#ifndef _SPI_H
#define _SPI_H

#include "Main.H"

#define init_okay (0)
#define no_errors (0)
#define SPI_ERROR (0)
#define illegal_clockrate (0x0F)
#define spi_timeout_error (0x80)

uint8_t SPI_Master_Init(uint32_t clock_rate);
uint8_t SPI_Transfer(uint8_t data_input, uint8_t * data_output_p);

#endif