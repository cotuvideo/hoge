#ifndef _CADC_H_
#define _CADC_H_
#include "CSuperDev.h"
#include "TypedefStruct.h"
#include "stm32f10x.h"

#define ADC1_DR	((uint32_t) 0x4001244C)

typedef struct
{
	uint8_t	channel;
	uint8_t Rank;
}ADC_Mapping;

class CADC : public CSuperDev
{
	public:
		CADC();
		~CADC();
		// Member function...
		void SetupGPIO(void);
		void SetupADC(void);
		void SetupDMA(void);
		void SetUpMisc(void);
		
		// Override...
		virtual void Initial(Sys_Config	*Sys);
		virtual void Update(__IO UINT Time);
		virtual void Destroy(void);

		// ADC Value... Read
		ADC_Info* GetADCInfo(void);

	private:
		enum {EXT_ST=0, BAT_ST, SYS_ST, TEMP_ST, BAL_0, BAL_1, BAL_2, BAL_3, CURRENT, ADCMAX};
		__IO	uint16_t	ADCReadValue[ADCMAX];
		ADC_Info	m_Info_ADC;
};


#endif
