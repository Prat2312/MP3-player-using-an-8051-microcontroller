#include "main.h"
#include "PORT.H"
#include "SPI.h"


uint8_t SPI_Master_Init(uint32_t clock_rate)
{
  uint8_t divider,return_val;
  return_val=init_okay;
  divider=(uint8_t)(((OSC_FREQ/OSC_PER_INST)*6)/clock_rate);
  if(divider<=2)
  {
     SPCON=0x70;
  }
  else if((divider>2)&&(divider<=4))
  {
     SPCON=0x71;
  }
  else if((divider>4)&&(divider<=8))
  {
     SPCON=0x72;
  }
  else if((divider>8)&&(divider<=16))
  {
     SPCON=0x73;
  } 
  else if((divider>16)&&(divider<=32))
  {
     SPCON=0xF0;
  }
  else if((divider>32)&&(divider<=64))
  {
     SPCON=0xF1;
  }
  else if((divider>64)&&(divider<=128))
  {
     SPCON=0xF2;
  }
  else  // if the SPI clock rate is too slow, a divider cannot be found
  {
    return_val=illegal_clockrate;
  }
  return return_val;
}



uint8_t SPI_Transfer(uint8_t data_input, uint8_t * data_output_p)
{
   uint8_t test, timeout;
   timeout=0;
   SPDAT=data_input;
   do
   {
      test=SPSTA;
	  timeout++;
   }while(((test&0xF0)==0)&&(timeout!=0));
   if(timeout!=0)
   {
     if((test&0x70)==0)  // no errors
     {
         *data_output_p=SPDAT;
         timeout=no_errors;
     }
     else
     {
         *data_output_p=0xff;
         timeout=SPI_ERROR;
     }
   }
   else
   {
     *data_output_p=0xff;
     timeout=spi_timeout_error;
   }
   return timeout;
}
 

