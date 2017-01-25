#include "CDevice.h"
#include "CommonHead.h"

#include "CADC.h"
#include "CFND.h"
#include "CRunLED.h"

#include "CSingletonSerial.h"
#include <string.h>
#include <stdlib.h>

__IO UINT m_Tick;
__IO UINT msTicks;

CDevice::CDevice()
{
	Sysval = new Sys_Config;

	memset(Sysval, 0, sizeof(Sys_Config));

}
CDevice::~CDevice()
{
	Destroy();
}

#define VER	1.0
void CDevice::Initial(void)
{
	// Initial Setting...
	DeviceSetting();

	// Create Device Object...
	Send->Push("flyhigh %2.2f\n\r", VER);
	for(int i=0; i<SCHEDUL_MAX; i++)
	{
		pDev[i]->Initial(Sysval);
	}
}

void CDevice::Update(void)
{
	for(int i=0; i<SCHEDUL_MAX; i++)
	{
		pDev[i]->Update(m_Tick);
	}
	// USART... Trans & Reciver
	Send->SendQueueEnable();		// Queue Transfer Enable..
	Send->GetRecive();

}

void CDevice::Destroy(void)
{
	for(int i=0; i<SCHEDUL_MAX; i++)
	{
		pDev[i]->Destroy();
		pDev[i] = NULL;
	}
	
	delete Sysval;
	CSingletonSerial::Release();
}

////////////////////////////////////////////////////////
// Timer2 Setting...
void CDevice::DeviceSetting(void)
{
	m_Tick = 0;
	msTicks = 0;

	SystemInit();		// System Clock Initial... 

	// Tick
	SysTick_Config(SystemCoreClock / 1000);
	CSingletonSerial::Create(Sysval);		// Create Singleton Class
	Send->CreateSerial();
	
	// TIM2 Setting...
	SetUpMisc();
	SetUpTimer();

	// Create Device Setting...
	CreateDevice();
}

void CDevice::CreateDevice(void)
{
#ifdef	_ADCDEV_
	pDev[SCHEDUL_ADC] = new CADC();
#endif

#ifdef	_FNDDEV_
	pDev[SCHEDUL_FND] = new CFND();
#endif

#ifdef	_RUNLEDDEV_
	pDev[SCHEDUL_RUNLED] = new CRunLED();
#endif

}

void CDevice::SetUpMisc(void)
{
	NVIC_InitTypeDef		NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void CDevice::SetUpTimer(void)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	
	// 1msec Setting..
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 1000-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
				
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	
	TIM_Cmd(TIM2, ENABLE);
}


/////////////////////////////////////////////////////////
// Interrupt Service Routine....

#define SEC_1	1000
extern "C" {
	
	/// Timer Interrupt --- 1mesec Tick..
	void TIM2_IRQHandler(void)
	{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
			m_Tick++;
			if(m_Tick >= SEC_1)		m_Tick = 0;		//m_Tick %= SEC_1;
		}
	}
}

extern "C" {
	void SysTick_Handler(void) 
	{
		msTicks++;
	}
}
