#ifndef _Play_Song_H
#define _Play_Song_H

#include "Main.H"

#define ON (1)
#define OFF (0)
//------- Public Constant definitions --------------------------------


typedef enum {Data_Idle_1, Data_send_1, Load_Buffer_2, Find_Cluster_2, Data_Idle_2, Data_send_2, Load_Buffer_1, Find_Cluster_1}state;
// ------ Public function prototypes -------------------------------
void Play_Song(uint32_t Start_Cluster);

#endif