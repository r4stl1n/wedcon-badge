#pragma once


#define MODE_PROXIMITY_TIMEOUT      3000
//#define MODE_PROXIMITY_RSSI_HIGH    -50
//#define MODE_PROXIMITY_RSSI_LOW     -60
#define MODE_PROXIMITY_RSSI_HIGH    -80
#define MODE_PROXIMITY_RSSI_LOW     -85

void Mode_Proximity_Init();
void Mode_Proximity_Loop();
void Mode_Proximity_Restore();
void Mode_Proximity_Shutdown();
