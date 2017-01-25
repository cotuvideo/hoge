#ifndef _SUPERDEV_H_
#define _SUPERDEV_H_

#include "CSuperDev.h"
#include "TypedefStruct.h"
#include "stm32f10x.h"
#include "CommonHead.h"

class CSuperDev
{
	public:
		CSuperDev() {}
		~CSuperDev(){}
		
		virtual void Initial(Sys_Config	*Sys) = 0;
		virtual void Update(__IO UINT Time) = 0;
		virtual void Destroy(void) = 0;

	private:

	protected:
		Sys_Config	*Sysval;
	
};

#endif
