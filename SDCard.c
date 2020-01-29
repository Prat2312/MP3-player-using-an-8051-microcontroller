#include "AT89C51RC2.h"
#include "PORT.H"
#include "main.h"
#include "SDCard.h"
#include "print_bytes.h"
#include "SPI.h"
#include <stdio.h>
#include  "Outputs.h"

static uint8_t  idata SD_Card_Type;


uint8_t SEND_COMMAND(uint8_t cmnd, uint32_t argum)
{
 uint8_t SPI_send, return_val, SPI_return, error_flag;

 return_val=no_errors;
 if(cmnd<64)
 {
   SPI_send=cmnd | 0x40;
   error_flag=SPI_Transfer(SPI_send,&SPI_return);
   return_val|=error_flag;
   if((error_flag)==no_errors)
   {
     SPI_send=argum>>24;   // MSB
     error_flag=SPI_Transfer(SPI_send,&SPI_return);
     return_val|=error_flag;
   }
   if(return_val==no_errors)
   {
     SPI_send=argum>>16;  // BYTE2
     error_flag=SPI_Transfer(SPI_send,&SPI_return);
	 return_val|=error_flag;
   }
   if(return_val==no_errors)
   {
     SPI_send=argum>>8;   // BYTE1
     error_flag=SPI_Transfer(SPI_send,&SPI_return);
     return_val|=error_flag;
   }
   if(return_val==no_errors)
   {
     SPI_send=argum & 0x000000ff;  // LSB
     error_flag=SPI_Transfer(SPI_send,&SPI_return);
     return_val|=error_flag;
   }
   if(return_val==no_errors)
   {         
      if (cmnd == CMD0)
      {
         SPI_send=0x95;  // CRC7 and end bit for CMD0
      }
      else if (cmnd == CMD8)
      {
         SPI_send=0x87;   // CRC7 and end bit for CMD8
      }
      else
      {
         SPI_send=0x01;  // end bit only for other commands
      }
      error_flag=SPI_Transfer(SPI_send,&SPI_return);
      return_val|=error_flag;
    }
 }
 else
 {
   return_val=illegal_cmnd;
 }
 return return_val;  
}

uint8_t response(uint8_t num_bytes, uint8_t * valout_p)
{
   uint8_t index,return_val,error_flag, SPI_return;

   return_val=no_errors;
   do
   {
      error_flag=SPI_Transfer(0xFF,&SPI_return);
      index++;
   }while(((SPI_return&0x80)==0x80)&&(index!=0)&&(error_flag==no_errors));
   if(error_flag!=no_errors)
   {
      return_val=SPI_error;
   }
   else if(index==0)
   {
      return_val=sd_timeout_error;
   }
   else
   {
     *valout_p=SPI_return;
     if((SPI_return==0x00)||(SPI_return==0x01))
     {
       if(num_bytes>1)
       {
         for(index=1;index<num_bytes;index++)
         {
            error_flag=SPI_Transfer(0xFF,&SPI_return);
            *(valout_p+index)=SPI_return;
         }
       }
     }
     else
     {
        return_val=response_error;
     }
   }
   error_flag=SPI_Transfer(0xFF,&SPI_return);  // send 8 more clock cycles to complete read
   return return_val;
}



uint8_t read_block(uint16_t num_bytes, uint8_t * array_out_p)
{
   uint8_t error_flag,return_val,SPI_return;
   uint16_t index;
   index=0;
   return_val=no_errors;
   do
   {
      error_flag=SPI_Transfer(0xFF,&SPI_return);
      index++;
   }while(((SPI_return&0x80)==0x80)&&(index!=0)&&(error_flag==no_errors));
   if(error_flag!=no_errors)
   {
      return_val=SPI_error;
   }
   else if(index==0)
   {
      return_val=sd_timeout_error;
   }
   else
   {
     if (SPI_return==0x00)
     {
        index=0;
	    do
        { 
           error_flag=SPI_Transfer(0xFF,&SPI_return); 
           index++;
        }while((SPI_return==0xFF)&&(index!=0)&&(error_flag==no_errors));          // wait for data token response  
        if(error_flag!=no_errors)
        {
           return_val=SPI_error;
        }
        else if(index==0)
        {
          return_val=sd_timeout_error;
        }
        else if(SPI_return==0xfe)
        {
          for(index=0;index<num_bytes;index++)
          {
             error_flag=SPI_Transfer(0xFF,&SPI_return);
             *(array_out_p + index)=SPI_return;
          }
          error_flag=SPI_Transfer(0xFF,&SPI_return); // discard byte 1 of CRC16	
          error_flag=SPI_Transfer(0xFF,&SPI_return); // discard byte 2 of CRC16
        }
	    else
	    {
	      return_val=data_error;
	    }
     }
     else
     {
        return_val=response_error;
     }
   }
 error_flag=SPI_Transfer(0xFF,&SPI_return);// send 8 more clock cycles to complete read
return return_val;
}


uint8_t SD_card_init(void)
{
   uint8_t i,error_status,error_flag,valsout[8],SPI_return;
   uint32_t argument;
   uint16_t timeout;
   error_status=no_errors;
   SD_Card_Type=unknown;
   Set_P1_bit(nCS0_bit);  //nCS0=1;
   printf("SD Card Initialization ... \n\r");
   for(i=0;i<10;i++)
   {
       error_flag=SPI_Transfer(0xFF,&SPI_return);
   }
   Clear_P2_bit(Green_LED);  //GREENLED=0;
   printf("CMD0 sent ... ");   
   Clear_P1_bit(nCS0_bit); //nCS0=0;  // SD card CS low
   error_flag=SEND_COMMAND(CMD0,0);
   if(error_flag==no_errors)
   {
     error_flag=response(1,valsout);
	 Set_P1_bit(nCS0_bit);  //nCS0=1;
     Set_P2_bit(Green_LED); //GREENLED=1;
     printf("Response = %2.2bX\n\r",valsout[0]);
   }
   else
   {
     Set_P1_bit(nCS0_bit);  //nCS0=1;
	 Set_P2_bit(Green_LED); //GREENLED=1;
	 Clear_P2_bit(Red_LED); //REDLED=0;  // indicates error has occured.
   }
   if(error_flag!=no_errors)
   {
     error_status=error_flag;
   }
   if(error_status==no_errors)
   {
     Clear_P2_bit(Green_LED);  //GREENLED=0;
     printf("CMD8 sent ... ");
     Clear_P1_bit(nCS0_bit); //nCS0=0;  // SD card CS low
     error_flag=SEND_COMMAND(CMD8,0x000001AA);
     Clear_P2_bit(Yellow_LED);  //Trigger for Logic Analyzer Capture
	 if(error_flag==no_errors)
     {
        error_flag=response(5,valsout);
		Set_P1_bit(nCS0_bit);  //nCS0=1;
		Set_P2_bit(Green_LED); //GREENLED=1;
	    printf("Response = ");
        for(i=0;i<5;i++)
		{   
		        printf("%2.2bX ",valsout[i]);
		}
        putchar(0x0D);
        putchar(0x0A);
		if(valsout[4]!=0xAA)
		{
		   error_flag=response_error;
		}
		if(valsout[3]!=0x01)
		{
		   error_flag=voltage_error;
		}
     }
     else
     {
        Set_P1_bit(nCS0_bit);  //nCS0=1;
        Set_P2_bit(Green_LED); //GREENLED=1;
	    Clear_P2_bit(Red_LED); //REDLED=0;  // indicates error has occured.
     }
	 if(error_flag!=no_errors)
	 {
        if(error_flag==illegal_cmnd)
        {
           error_status=no_errors;
		   SD_Card_Type=Standard_Capacity;
		   printf("Version 1 SD Card detected.\n\r");
		   printf("Standard Capacity Card detected.\n\r");
        }
		else
		{
		   error_status=error_flag;
		}
	 }
	 else
	 {
		SD_Card_Type=Ver2;
		printf("Version 2 SD Card detected.\n\r");
	 }
   }
   if(error_status==no_errors)
   {
     Clear_P2_bit(Green_LED);  //GREENLED=0;
     printf("CMD58 sent ... ");
     Clear_P1_bit(nCS0_bit); //nCS0=0;  // SD card CS low
     error_flag=SEND_COMMAND(CMD58,0);
	 if(error_flag==no_errors)
     {
        error_flag=response(5,valsout);
		Set_P1_bit(nCS0_bit);  //nCS0=1;
		Set_P2_bit(Green_LED); //GREENLED=1;
	    printf("Response = ");
        for(i=0;i<5;i++)
		{   
		     printf("%2.2bX ",valsout[i]);
		}
        putchar(0x0D);
        putchar(0x0A);
		if((valsout[2]&0xFC)!=0xFC)
		{
		   error_flag=voltage_error;
		}
     }
     else
     {
        Set_P1_bit(nCS0_bit);  //nCS0=1;
        Set_P2_bit(Green_LED); //GREENLED=1;
	    Clear_P2_bit(Red_LED); //REDLED=0;  // indicates error has occured.
     }
	 if(error_flag!=no_errors)
	 {		
	    error_status=error_flag;
	 }
   }
   if(error_status==no_errors)
   {
     if(SD_Card_Type==Ver2)
	 {
	    argument=0x40000000;
	 }
	 else
	 {
	    argument=0;
	 }
	 timeout=0;
     
     printf("ACMD41 sent ... ");
     //Clear_P1_bit(nCS0_bit); //nCS0=0;  // SD card CS low
	 do
	 {
	    Clear_P1_bit(nCS0_bit); //nCS0=0;  // SD card CS low
	    error_flag=SEND_COMMAND(CMD55,0);
		if(error_flag==no_errors)  error_flag=response(1,valsout);
	    if((valsout[0]==0x01)||(valsout[0]==0x00))
		   error_flag=SEND_COMMAND(ACMD41,argument);
	    if(error_flag==no_errors) response(1,valsout);
        Set_P1_bit(nCS0_bit);  //nCS0=1;
		timeout++;
		if(timeout==0) error_flag=sd_timeout_error;
	   }while(((valsout[0]&0x01)==0x01)&&(error_flag==no_errors));
	 if(error_flag==no_errors)
     {
		//Set_P1_bit(nCS0_bit);  //nCS0=1;
        Set_P2_bit(Yellow_LED); //GREENLED=1;
	    printf("Response = %2.2bX\n\r",valsout[0]);
     }
     else
     {
        Set_P1_bit(nCS0_bit);  //nCS0=1;
        Set_P2_bit(Green_LED); //GREENLED=1;
	    Clear_P2_bit(Red_LED); //REDLED=0;  // indicates error has occured.
     }
	 if(error_flag!=no_errors)
	 {		
	    error_status=error_flag;
	 }
   }
   if((error_status==no_errors)&&(SD_Card_Type==Ver2))
   {
     Clear_P2_bit(Green_LED);  //GREENLED=0;
     printf("CMD58 sent ... ");
     Clear_P1_bit(nCS0_bit); //nCS0=0;  // SD card CS low
     error_flag=SEND_COMMAND(CMD58,0);
	 if(error_flag==no_errors)
     {
        error_flag=response(5,valsout);
		Set_P1_bit(nCS0_bit);  //nCS0=1;
        Set_P2_bit(Green_LED); //GREENLED=1;
	    printf("Response = ");
        for(i=0;i<5;i++)
		{   
           printf("%2.2bX ",valsout[i]);
		}
        putchar(0x0D);
        putchar(0X0A);
		if((valsout[1]&0x80)!=0x80)
		{
		   error_flag=card_inactive;
		}
		else
		{
		   if((valsout[1]&0xC0)==0xC0)
		   {
		      SD_Card_Type=High_Capacity;
		      printf("High Capacity Card Detected\n\r");
		   }
		   else
		   {
		      SD_Card_Type=Standard_Capacity;
			  printf("Standard Capacity Card Detected\n\r");
			  Clear_P2_bit(Green_LED);  //GREENLED=0;
              printf("CMD16 sent ... ");
              Clear_P1_bit(nCS0_bit); //nCS0=0;  // SD card CS low
              error_flag=SEND_COMMAND(CMD16,512);
	          if(error_flag==no_errors)
              {
                  error_flag=response(1,valsout);
		          Set_P1_bit(nCS0_bit);  //nCS0=1;
                  Set_P2_bit(Green_LED); //GREENLED=1;
	              printf("Response = %2.2bX \n\r",valsout[0]);
                  printf("Block size set to 512 bytes\n\r");
		      }
			}
		 }
     }
     else
     {
        Set_P1_bit(nCS0_bit);  //nCS0=1;
        Set_P2_bit(Green_LED); //GREENLED=1;
	    Clear_P2_bit(Red_LED); //REDLED=0;  // indicates error has occured.
     }
	 if(error_flag!=no_errors)
	 {		
	    error_status=error_flag;
		print_error(error_status);
	 }
   }
 if(error_status!=no_errors)
 {		
	print_error(error_status);
 }
return error_status;
}



uint8_t Return_SD_Card_Type(void)
{
   return SD_Card_Type;
}




void print_error(uint8_t error)
{
   if(error==spi_timeout_error) printf("Timeout Error");
   else if(error==sd_timeout_error) printf("SD Timeout Error");
   else if(error==illegal_cmnd) printf("Illegal Command\n\r");
   else if(error==response_error) printf("Response Error");
   else if(error==data_error) printf("Data Token Error");
   else if(error==voltage_error) printf("Incompatible Voltage");
   else if(error==card_inactive) printf("Card is Inactive");
   else if(error==SPI_error) printf("SPI or Timeout Error");
   else printf("Unknown Error");
}

