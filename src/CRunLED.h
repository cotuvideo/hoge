#ifndef _CRUNLED_H_
#define _CRUNLED_H_

#include "CSuperDev.h"
#include "CommonHead.h"
#include "TypedefStruct.h"
#include "stm32f10x.h"
// Single Tone Class
#include "CSingletonSerial.h"

class CRunLED : public CSuperDev
{
public:
	CRunLED();
	~CRunLED();

	// Override..
	virtual void Initial(Sys_Config	*Sys);
	virtual void Update(__IO UINT Time);
	virtual void Destroy(void);

	// Member Function..
	void SetUpGPIO(void);

private:
	enum {RUN_LED = 500};	// 0.5sec
	UINT	m_Cnt;
	UCHAR   m_Toggle;
};

#endif
