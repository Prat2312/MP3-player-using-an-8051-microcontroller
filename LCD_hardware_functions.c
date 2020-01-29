#include "main.h"
#include "LCD_hardware_functions.h"
#include "PORT.H"
#include "Outputs.h"


/***********************************************************************
DESC:    Writes data to the LCD with proper order for RS and E
INPUT: Uint8 for RS and Data to be written
       RS=0 instruction, RS=1 data
RETURNS: Nothing
CAUTION: None
************************************************************************/
void LCD_Write(uint8_t RegSelect, uint8_t LCD_Data)
{
  if(RegSelect==0)     // Set or Clear RS before E is Set
  {
	Clear_P3_bit(RS_bit);
  }
  else
  {
    Set_P3_bit(RS_bit);
  }
  Set_P3_bit(E_bit);  // Set Enable before data written
  Output_P0_bit(LCD_Data);    
  Clear_P3_bit(E_bit);   
  Output_P0_bit(0xFF);
  Set_P3_bit(RS_bit);
}



