#ifndef	_TYPEDEFSTRUCT_H_
#define _TYPEDEFSTRUCT_H_
#include "CommonHead.h"

#define BALANCE 4

typedef struct
{
	float EXT_V;
	float Battery_V;
	float Sys_V;
	float Temp;
	float Balance[BALANCE];
	float Current;
}ADC_Info;

typedef struct
{
	CHAR Room;		
	CHAR PCNum;
	CHAR Indicator;	
	
}FND_Info;

typedef struct
{
	UCHAR	SysCount;
	UCHAR	OldSysCount;
}
Time_Config;

// System Variable Config..
typedef struct
{
	ADC_Info		AdcInfo;
	FND_Info		FndInfo;
	Time_Config		Time;
	UINT i;
}Sys_Config;

#endif

