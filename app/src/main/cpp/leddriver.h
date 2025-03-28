#ifndef _LEDDRIVER_H_
#define _LEDDRIVER_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

int ledDriver_Init();
int ledDriver_EnableChannel(unsigned char channelNo, unsigned char enable);
int ledDriver_GetChannelStatus(unsigned char channelNo);
int ledDriver_Output(unsigned char enable);
int ledDriver_GetOutputStatus();
int ledDriver_SetIntensity(unsigned char intensity);
int ledDriver_GetIntensity();
int ledDriver_SetChannelTonTime(unsigned char channelNo, int tOnTime);
int ledDriver_GetChannelTonTime(unsigned char channelNo);
int ledDriver_SetRegVal(unsigned char regAddr, unsigned char value);
int ledDriver_GetRegVal(unsigned char regAddr);
int ledDriver_DeInit();

#ifdef __cplusplus
}; 
#endif

#endif
