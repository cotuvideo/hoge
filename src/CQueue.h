#ifndef _CQUEUE_H_
#define _CQUEUE_H_

#include "CommonHead.h"
#include "stm32f10x.h"

class CQueue
{
public:
	CQueue();
	~CQueue();

	UCHAR	SetPush(UCHAR _data);
	UINT	GetPop();

	UINT	GetRearPos();
	UINT	GetFrontPos();
	void	Initail();

	UCHAR	GetEmpity();

private: 
	enum  { _QUEUEMAX_ = 512 };
	__IO UCHAR	*Queue;
	
	UINT	m_FrontPos;
	UINT	m_RearPos;
};

#endif
