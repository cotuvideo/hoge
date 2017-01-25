#ifndef _CFND_H_
#define _CFND_H_

#include "CSuperDev.h"
#include "TypedefStruct.h"
#include "stm32f10x.h"
#include "CommonHead.h"

class CFND : public CSuperDev
{
public:
	CFND();
	~CFND();

	// Override...
	virtual void Initial(Sys_Config	*Sys);
	virtual void Update(__IO UINT Time);
	virtual void Destroy(void);

	void SetUpGPIO(void);
	void SetFND(void);

	FND_Info 	OldFndInfo;

private:
	enum {DOT = 0x80, SHIFT_BIT = 32 };
	UCHAR FND_Info[10];


};


#endif
