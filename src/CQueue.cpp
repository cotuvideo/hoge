#include "CQueue.h"

CQueue::CQueue():
m_FrontPos(0),
m_RearPos(0)
{

}

CQueue::~CQueue()
{
	delete[] Queue;
}

void CQueue::Initail(void)
{
	Queue = new __IO UCHAR[_QUEUEMAX_];
	for(int j=0; j<_QUEUEMAX_; j++)
		Queue[j] = 0;
}

UCHAR CQueue::SetPush(UCHAR _data)
{
   m_FrontPos++;
   if(m_FrontPos >= _QUEUEMAX_) // m_FrontPos %= _QUEUEMAX_;
        m_FrontPos = 0; 

   Queue[m_FrontPos] = _data;
   return true;
}

UINT CQueue::GetPop()
{
    if(m_FrontPos != m_RearPos)
    {
        m_RearPos++;
        if(m_RearPos >= _QUEUEMAX_)
            m_RearPos = 0;
        return (UINT) Queue[m_RearPos];
    }
    else
        return 0x8000;
}

UINT CQueue::GetRearPos()
{
    return m_RearPos;
}

UINT CQueue::GetFrontPos()
{
    return m_FrontPos;
}

UCHAR CQueue::GetEmpity()
{
	return (m_RearPos==m_FrontPos)? FALSE : TRUE;
}
