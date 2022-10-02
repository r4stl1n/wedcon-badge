#pragma once


#define MODE_PROXIMITY_TIMEOUT      1000
#define MODE_PROXIMITY_RSSI_HIGH    -50
#define MODE_PROXIMITY_RSSI_LOW     -60

void Mode_Proximity_Init();
void Mode_Proximity_Loop();
void Mode_Proximity_Shutdown();
