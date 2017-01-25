#include "CADC.h"
#include "stm32f10x.h"
#include "CSingletonSerial.h"

CADC::CADC()
{
}
CADC::~CADC()
{
}

void CADC::Initial(Sys_Config	*Sys)
{
	// Setting.. Register...
	//http://blog.naver.com/PostView.nhn?blogId=gauya&logNo=220232257331
	//http://www.embedds.com/multichannel-adc-using-dma-on-stm32/
	Sysval = Sys;

	SetupGPIO();
	SetupADC();
	SetupDMA();
//	SetUpMisc();

	// ADC Start..
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void CADC::SetupGPIO()
{
	GPIO_InitTypeDef		GPIO_InitStructure;
	//1.Port A Setting..
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;

	//2.PORT C Setting..
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void CADC::SetupADC()
{
	
	ADC_Mapping		m_ADC_Map[ADCMAX] = {{ADC_Channel_0, EXT_ST}, {ADC_Channel_1,BAT_ST}, {ADC_Channel_6,SYS_ST}, {ADC_Channel_7,TEMP_ST},
										 {ADC_Channel_10,BAL_0}, {ADC_Channel_11,BAL_1}, {ADC_Channel_12,BAL_2}, {ADC_Channel_13,BAL_3}, {ADC_Channel_15,CURRENT}};

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	// ADC1

	ADC_InitTypeDef	ADC_InitStructure;

	ADC_InitStructure.ADC_Mode					= ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode			= ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode	= ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv		= ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign				= ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel			= 9;

	ADC_Init(ADC1, &ADC_InitStructure);

	//// Configure each Channel...
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_0, EXT_ST,  ADC_SampleTime_41Cycles5);	// EXT_ST - PA0
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, BAT_ST,  ADC_SampleTime_41Cycles5);	// BAT_ST - PA1
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_6, SYS_ST,  ADC_SampleTime_41Cycles5);	// SYS_ST - PA6
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_7, TEMP_ST,  ADC_SampleTime_41Cycles5);	// TEMP_ST - PA7
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_16, CURRENT,  ADC_SampleTime_41Cycles5);	// Current - PC5
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_10, BAL_0,  ADC_SampleTime_41Cycles5);	// BAL_0 - PC0
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_11, BAL_1,  ADC_SampleTime_41Cycles5);	// BAL_1 - PC1
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_12, BAL_2,  ADC_SampleTime_41Cycles5);	// BAL_2 - PC2
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_13, BAL_3,  ADC_SampleTime_41Cycles5);	// BAL_3 - PC3

	for(int i=0; i<ADCMAX; i++)
		ADC_RegularChannelConfig(ADC1, m_ADC_Map[i].channel, m_ADC_Map[i].Rank + 1, ADC_SampleTime_41Cycles5);
	
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

void CADC::SetupDMA(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitTypeDef	DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);

	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //DMA_Mode_Circular;	//DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

	DMA_InitStructure.DMA_BufferSize = ADCMAX;	// size..
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) ADC1_DR;//&ADC1->DR; //(uint32_t)	ADC1_DR;	
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)	&ADCReadValue;
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void CADC::SetUpMisc(void)
{
	// Enable DMA1 Channel IRQ Channel
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

ADC_Info* CADC::GetADCInfo(void)
{
	return &m_Info_ADC;
}

void CADC::Update(__IO UINT Time)
{
	if(Time != 900) return;
	
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	// Buffer Read.. Ratio Operation..
	Sysval->AdcInfo.EXT_V =		ADCReadValue[EXT_ST];
	Sysval->AdcInfo.Battery_V = ADCReadValue[BAT_ST];
	Sysval->AdcInfo.Sys_V =		ADCReadValue[SYS_ST];
	Sysval->AdcInfo.Temp =		ADCReadValue[TEMP_ST];

	//Send->Push("EXTV : %3d, Battery : %3d\n\r", ADCReadValue[EXT_ST],  ADCReadValue[BAT_ST]);
	for(int i=0; i<9; i++)
		Send->Push("%d.[%6d]\n\r", i, ADCReadValue[i]);
	Send->Push("\n\r");

	// Balance Voltage..
	for(int i=BAL_0, j=0; i<CURRENT; i++, j++)
		Sysval->AdcInfo.Balance[j] = ADCReadValue[i];

	// Current Sensing..
	Sysval->AdcInfo.Current = ADCReadValue[CURRENT];
	// RestartADC..
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void CADC::Destroy(void)
{
}

/// External.. Interrupt...
// http://blog.naver.com/PostView.nhn?blogId=gauya&logNo=220232257331
extern "C" {
	void DMA1_Channel1_IRQHandler(void)
	{
		if(DMA_GetITStatus(DMA1_IT_TC1))
		{
			DMA_ClearITPendingBit(DMA1_IT_GL1 | DMA1_IT_TC1);
		}
	}
}

