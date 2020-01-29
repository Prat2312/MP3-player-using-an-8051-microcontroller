#include <stdio.h>
#include "Main.h"
#include "AT89C51RC2.h"
#include "PORT.H"
#include "SPI.h"
#include "SDCard.h"
#include "File_System.h"
#include "Directory_Functions.h"
#include "Read_Sector.h"       
#include "Play_Song.h"
#include "Outputs.h"
#include "LED_Outputs.h"


extern uint8_t xdata buf1[512];
extern uint8_t xdata buf2[512];

state state_g;


void Play_Song(uint32_t Start_Cluster)
{
uint16_t index1, index2;
uint8_t temp8, play = 1;
uint32_t sector, sector_offset;

printf("Starting Cluster = %1u\n\r",Start_Cluster);

sector = First_Sector(Start_Cluster);
printf("Starting Sector = %1u\n\r", sector);

sector_offset = 0;

 //buf1=1;
// buf2=0;

index1 = 0;
Read_Sector(sector+sector_offset, 512, buf1);
sector_offset++;
AMBERLED=0;

index2 = 0;
Read_Sector(sector+sector_offset, 512, buf2);
sector_offset++;
AMBERLED=1;

state_g = Data_Idle_1;
GREENLED = 0;
REDLED = 1;
while((Start_Cluster != 0x0FFFFFFF)&&(SW4 ==1))
{
if((state_g == Load_Buffer_1)&&(sector_offset >= 64))
{
state_g = Find_Cluster_1;
}
else if((state_g == Load_Buffer_2)&&(sector_offset >= 64))
{
state_g = Find_Cluster_2;
}
switch(state_g)
{
	case Data_Idle_1:
	{
		if (DATA_REQ == ON)
		{ 
			state_g = Data_send_1;
		}
		break;
	}
	case Data_send_1:
	{
		if(SW3 == 0)
		{
			play=0;
			GREENLED = 1;
			YELLOWLED = 0;
		}
		else if(SW2 == 0)
		{
			play = 1;
			GREENLED = 0;
			YELLOWLED = 1;
		}

if (DATA_REQ==OFF)// If data request is inactive

{
   if(index2>511)
     state_g = Load_Buffer_2;
   else
     state_g = Data_Idle_1;
}
else // If data id ACTIVE
{
   if(play==1)
{
	Set_P3_bit(BIT_EN_bit); 
   SPI_Transfer(buf1[index1], &temp8);
	Clear_P3_bit(BIT_EN_bit); 
   index1++;
}
if(index1>511)
{ 
    state_g = Data_send_2;
    if(index2>511)
         state_g = Load_Buffer_2;
}
}
break;
}

case Load_Buffer_2:
{
  index2=0;
  Read_Sector(sector+sector_offset, 512, buf2);
  sector_offset++;
  state_g = Data_Idle_1;
  break;

}

case Find_Cluster_2:

{
  Start_Cluster = Find_Next_Clus(Start_Cluster,buf2);
  sector= First_Sector(Start_Cluster);
  sector_offset = 0;
  state_g = Data_Idle_1;
  
  if(Start_Cluster == 0x0FFFFFFF)
  { 

    state_g = Data_Idle_1;
    LEDS_ON(REDLED);
   }

   break;

}

case Data_Idle_2:

{  

  if(DATA_REQ==ON)
{ 
  state_g = Data_send_2;
}

break;
}

case Data_send_2:

{

  if(SW3==0)

    play=0;

  else if(SW2==0)  //303

    play=1;

if (DATA_REQ==OFF) // If data request is INACTIVE

{
   if (index1>511)
 state_g = Load_Buffer_1;
   else
      state_g = Data_Idle_2;
}

else // If data request is ACTIVE
{
   if((play==1))

   { 
   Set_P3_bit(BIT_EN_bit);  
   SPI_Transfer(buf2[index2], &temp8);
    Clear_P3_bit(BIT_EN_bit); 
     index2++;

}
if(index2>511)
{
   state_g = Data_send_1;
   if(index1>511)

      state_g= Load_Buffer_1;

}

}

break;
}


case Load_Buffer_1:

{

  index1=0;
  Read_Sector(sector+sector_offset, 512, buf1);
  sector_offset++;
  state_g = Data_Idle_2;
  break;
}

case Find_Cluster_1:

{

   Start_Cluster = Find_Next_Clus(Start_Cluster, buf1);
   sector=First_Sector(Start_Cluster);
   sector_offset = 0;
   state_g = Data_Idle_2;


if(Start_Cluster == 0x0FFFFFFF)
{ 
  state_g = Data_Idle_2;
}

break;
}
}
}

LEDS_OFF(GREENLED);
LEDS_OFF(YELLOWLED);
LEDS_ON(REDLED);
}


