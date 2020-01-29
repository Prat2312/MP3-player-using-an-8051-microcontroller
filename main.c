#include <stdio.h>
#include "AT89C51RC2.h"
#include "main.h"
#include "PORT.H"
#include "SPI.h"
#include "SDCard.h"
#include "File_System.h"
#include "Read_Sector.h" 
#include "UART.h"      
#include "Play_Song.h"
#include "Outputs.h"
#include "LED_Outputs.h"


void main()
{
  uint8_t xdata array_read[512];

 if(OSC_PER_INST==6)
   {
       CKCON0=0x01;  // set X2 clock mode
   }
   else if(OSC_PER_INST==12)
   {
       CKCON0=0x00;  // set standard clock mode
   }
 
        uart_init();
		SPI_Master_Init(400000);  //400kHz
		SD_card_init();
		SPI_Master_Init(25000000);	//25MHz
		Mount_Drive(array_read);
        Play_Song(145600);  // play MAIDWI.mp3 start sector # 145600
}
                
