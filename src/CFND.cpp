#include "CFND.h"
#include <stdio.h>
#include <string.h>
#include "CSingletonSerial.h"


CFND::CFND()
{
	FND_Info[0] = 0x3f;	
	FND_Info[1] = 0x06; 
	FND_Info[2] = 0x5b;
	FND_Info[3] = 0x4f;
	FND_Info[4] = 0x66;
	FND_Info[5] = 0x6d;
	FND_Info[6] = 0x7d;
	FND_Info[7] = 0x27;
	FND_Info[8] = 0x7f;
	FND_Info[9] = 0x6f;
}

CFND::~CFND()
{
}

void CFND::Initial(Sys_Config *Sys)
{
	Sysval = Sys;
	memset(&OldFndInfo, 0, sizeof(FND_Info));

	SetUpGPIO();
}

void CFND::Update(__IO UINT Time)
{

	if(Time != 700) return;

	if(memcmp(&OldFndInfo, &Sysval->FndInfo, sizeof(FND_Info)))
	{
		SetFND();
		memcpy(&OldFndInfo, &Sysval->FndInfo, sizeof(FND_Info));
	}
}

void CFND::Destroy()
{

}
void CFND::SetUpGPIO(void)
{
	// PortC.9 == FND_CLK
	// PortC.10 == FND_Latch
	// PortA.8 == FND_DATA

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef			   GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void CFND::SetFND()
{
	UINT ShiftData = 0;

	// Setting Shift FND Data
	ShiftData = (Sysval->FndInfo.Indicator) & 0x000000ff;
	ShiftData <<= 8;
	ShiftData |= ((FND_Info[Sysval->FndInfo.PCNum %10]) & 0x000000ff);
	ShiftData <<= 8;
	ShiftData |= ((FND_Info[Sysval->FndInfo.PCNum /10]) & 0x000000ff);
	ShiftData <<= 8;
	ShiftData |= ((FND_Info[Sysval->FndInfo.Room] | DOT) & 0x000000ff);
	
	Send->Push("Sizefo : %d - %8x\n\r", sizeof(UINT), ShiftData);

	// Set Latch Bit
	GPIO_SetBits(GPIOA, GPIO_Pin_8);	// Data
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);	// Clk
	GPIO_ResetBits(GPIOC, GPIO_Pin_10);	// Latch
	for(int i=0; i<SHIFT_BIT; i++)
	{
		// ShiftData
		if(ShiftData & 0x80000000)		GPIO_SetBits(GPIOA, GPIO_Pin_8);
		else							GPIO_ResetBits(GPIOA, GPIO_Pin_8);

		// Clk
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		ShiftData <<= 1;
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	}
	GPIO_SetBits(GPIOC, GPIO_Pin_10);	// Latch
	GPIO_SetBits(GPIOC, GPIO_Pin_9);	// Clk
	GPIO_SetBits(GPIOA, GPIO_Pin_8);	// Data
	
}

