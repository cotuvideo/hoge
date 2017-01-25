#ifndef _CSINGLETONSERIAL_H_
#define _CSINGLETONSERIAL_H_
#include "CommonHead.h"
#include "CQueue.h"
#include "stdarg.h"
#include "stm32f10x.h"

#include "TypedefStruct.h"


/// UART ReDefine
#define  GPIOx                    GPIOA
#define  RCC_APB2Periph_GPIOx     RCC_APB2Periph_GPIOA
#define  GPIO_TxPin               GPIO_Pin_2
#define  GPIO_RxPin               GPIO_Pin_3

class CSingletonSerial
{
private:
	enum {TX_QUEUE=0, RX_QUEUE, MAXCH_QUEUE} QUEUE;

	static CSingletonSerial		*handle;
	CQueue	*m_Queue[MAXCH_QUEUE];
	Sys_Config *Sysval;

	CSingletonSerial(){}
	~CSingletonSerial(){}

public:
	///////////////////////////////////////////////
	static bool Create(Sys_Config	*Sys)
	{
		if(handle == NULL)
		{
			handle = new CSingletonSerial;

			handle->Sysval = Sys;
			handle->ParserBufferInit();
			for(int i=0; i<MAXCH_QUEUE; i++)
			{
				handle->m_Queue[i] = new CQueue;
				handle->m_Queue[i]->Initail();
			}
			return TRUE;
		}
		return FALSE;
	}
	static bool Release()
	{
		if(handle != NULL)
		{
			for(int i=0; i<MAXCH_QUEUE; i++)
			{
				delete handle->m_Queue[i];
				handle->m_Queue[i] = NULL;
			}
			delete handle;
			handle = NULL;
			return TRUE;
		}
		return FALSE;
	}
	static CSingletonSerial* GetHandle()
	{
		return handle;
	}
	///////////// Queueu ฐüทร //////////////
	void Push(UCHAR	*String,UCHAR _ch = TX_QUEUE);
	void Push(UCHAR	*String, UINT Lenght, UCHAR _ch = TX_QUEUE);
	void Push(UCHAR Mode, UCHAR Sub, UCHAR *String, UCHAR _ch = TX_QUEUE);
	void Push(const char *format, ...);
	void Push(BYTE Char, UCHAR _ch = TX_QUEUE);
	UCHAR Pop(UCHAR _ch = TX_QUEUE);
	////////////////////////////////////////

	///////////// Create Serial Port Setting ////////
	void CreateSerial (UINT Buadrate1 = 115200, UINT Buadrate2 = 115200);
	void SendQueueEnable(UCHAR _ch = TX_QUEUE);	
	inline CQueue* GetQueueHandle(UCHAR _ch = TX_QUEUE) {	return m_Queue[_ch];	}
	/////////////////////////////////////////////////

	//////////// Receiver Rutine ////////////
	void ParserBufferInit();
	void GetRecive();
	void ParserDecoder(char *data);
	
private:
	UINT m_ParserCnt;
	UINT m_ParserStart;
	enum { MAIN_CMD = 1, SUB_CMD = 2, PARSERBUF= 50 };
	BYTE m_ParserData[PARSERBUF];
	char m_BParser[5];
	////////////////////////////////////////
};

#define Send CSingletonSerial::GetHandle()
#endif

