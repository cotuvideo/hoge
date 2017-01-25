#include "stm32f10x.h"
#include "main.h"
#include "CommonHead.h"
#include "TypedefStruct.h"
//#include "..\source_lib\gpio.h"

#include "CDevice.h"

/// Main Routine... ///
int main()
{
	CDevice *pDev = new CDevice();
	pDev->Initial();
	while(TRUE)
	{
		pDev->Update();
	}
	pDev->Destroy();
	delete pDev;
	
	pDev=NULL;
	return 1;
}
