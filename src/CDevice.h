#ifndef _CDEVICE_H_
#define _CDEVICE_H_

#include "CSuperDev.h"
#include "CommonHead.h"
#include "TypedefStruct.h"

// Device Select..
//#define _ADCDEV_
//#define	_FNDDEV_
#define _RUNLEDDEV_



class CDevice
{
public:
	CDevice();
	~CDevice();

	void Initial(void);
	void Update();
	void Destroy(void);
	
	void DeviceSetting(void);
	void SetUpMisc(void);
	void SetUpTimer(void);

	void CreateDevice(void);

private:

//////////////////////////////////////////////////
	enum 
	{
#ifdef	_ADCDEV_
		SCHEDUL_ADC, 
#endif

#ifdef	_FNDDEV_
		SCHEDUL_FND,  
#endif

#ifdef	_RUNLEDDEV_
		SCHEDUL_RUNLED,
#endif
		SCHEDUL_MAX
	};
//////////////////////////////////////////////////
	
	CSuperDev	*pDev[SCHEDUL_MAX];

	// Valiable..
	Sys_Config	*Sysval;

};

#endif
