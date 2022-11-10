#pragma once


#define MODE_PROXIMITY_TIMEOUT      3000
#define MODE_PROXIMITY_RSSI_HIGH    -55
#define MODE_PROXIMITY_RSSI_LOW     -58

void Mode_Proximity_Init();
void Mode_Proximity_Loop();
void Mode_Proximity_Shutdown();
