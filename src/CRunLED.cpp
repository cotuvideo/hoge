#include "CRunLED.h"


CRunLED::CRunLED():
m_Cnt(0),
m_Toggle(0)
{
}

CRunLED::~CRunLED()
{
}

//////// Override ///////
void CRunLED::Initial(Sys_Config	*Sys)
{
	Sysval = Sys;
	SetUpGPIO();
}

void CRunLED::Update(__IO UINT Time)
{	

//	m_Cnt = Time;
	if(Time < RUN_LED/4)
	{
if(m_Cnt < sizeof(GPIO_InitTypeDef))
{
		if(m_Toggle)
		{
m_Cnt++;
			GPIO_SetBits(GPIOD, GPIO_Pin_2);
			m_Toggle = 0;
			//Send->Push("LED ON :  [%3d] \n\r", Time);
				Send->Push("%s. Data is :%4x \n\r", __FUNCTION__, Sysval->i);
		}
}
	}
	else
	{
		if(!m_Toggle)
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_2);
			m_Toggle = 1;
			//Send->Push("LED OFF : [%3d] \n\r", Time);
		}
	}
}

void CRunLED::Destroy(void)
{

}

///////////////////////////////
void CRunLED::SetUpGPIO(void)
{
	// Run LED Port : PD2 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef			   GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);		
}
