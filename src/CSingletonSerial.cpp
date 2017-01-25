#include "CSingletonSerial.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


CSingletonSerial*	CSingletonSerial::handle = NULL;

void CSingletonSerial::Push(UCHAR	*String, UCHAR _ch)
{
	int i = 0;
    while(1)
    {
        if(String[i] == 0) break;
        m_Queue[_ch]->SetPush(String[i]);
        i++;
    }  
}

void CSingletonSerial::Push(UCHAR	*String, UINT Lenght, UCHAR _ch)
{
	int i=0;
    for(i=0; i<Lenght;  i++)
        m_Queue[_ch]->SetPush(String[i]);
}

void CSingletonSerial::Push(UCHAR Mode, UCHAR Sub, UCHAR *String, UCHAR _ch)
{
	m_Queue[_ch]->SetPush(STX);
    m_Queue[_ch]->SetPush(Mode);
    m_Queue[_ch]->SetPush(Sub);
    Push(String, _ch);
    m_Queue[_ch]->SetPush(ETX);
}

void CSingletonSerial::Push(const char *format, ... )
{
    char buffer[256] = {0};
    va_list args;
    va_start (args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    
    Push((UCHAR*) buffer);
}

void CSingletonSerial::Push(BYTE Char, UCHAR _ch)
{
	m_Queue[_ch]->SetPush(Char); 
}

UCHAR CSingletonSerial::Pop(UCHAR _ch)
{
    return m_Queue[_ch]->GetPop();
}

void CSingletonSerial::SendQueueEnable(UCHAR _ch)
{
	if(m_Queue[_ch]->GetEmpity())	// GetFlag is Queue Empity Status
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}
///////////////////////////////////////////////////////////////////////////////////////
///// USART Receive....
void CSingletonSerial::ParserBufferInit()
{
    m_ParserCnt = 0;
    m_ParserStart = 0;
    memset(m_ParserData, 0, PARSERBUF);
}
void CSingletonSerial::GetRecive()
{
	UCHAR c = 0;
	if(!m_Queue[RX_QUEUE]->GetEmpity()) return;

	while(m_Queue[RX_QUEUE]->GetFrontPos() != m_Queue[RX_QUEUE]->GetRearPos())
	{
		c = m_Queue[RX_QUEUE]->GetPop();

		if(c <= 0 ) c=0;

		if(m_ParserCnt == 0)
		{
			if(c== STX)
				m_ParserStart = TRUE;
		}

		if(m_ParserStart)
		{
			m_ParserData[m_ParserCnt++] = c;

			if(m_ParserCnt == PARSERBUF)
				m_ParserCnt =0;

			switch(c)
			{
			case ETX:
				m_ParserData[m_ParserCnt] = 0;
				m_ParserCnt = 0;
				m_ParserStart = 0;

				ParserDecoder((char *) m_ParserData);
				break;
			}
		}
	}
}

void CSingletonSerial::ParserDecoder(char *data)
{
	int search = 0; 
	switch(data[MAIN_CMD])
	{
	case 'I':
	case 'i':
		Push("%s. %s\n\r", __FUNCTION__, data);
		search = SUB_CMD + 1;
		memset(m_BParser, 0, 5);
		for(int i=0; i<4; i++)
			m_BParser[i] = data[search+i];

		Sysval->i = strtol(m_BParser, NULL, 16);
		
		break;
	case 'P':
	case 'p':

		break;
	case 'F':
	case 'f':
		Sysval->FndInfo.Indicator = 0;	// Battery Check..
		search = SUB_CMD + 1;
		memset(m_BParser, 0, 5);

		m_BParser[search] = data[search];
		Sysval->FndInfo.Room = m_BParser[search] - 0x30;//strtol(m_BParser, NULL, 10);		// 10진수
		memset(m_BParser, 0, 5);
		++search;
		for(int i=0; i<2; i++)
		m_BParser[i] = data[search+i];
		Sysval->FndInfo.PCNum = strtol(m_BParser, NULL, 10);	// 10진수
			
		// Debugging 방법론..
		Send->Push("Room : %d / PC : %d \n\r", Sysval->FndInfo.Room,
											   Sysval->FndInfo.PCNum);							
		
		break;
	
	}
	memset(m_ParserData, 0, PARSERBUF);
}

 //strtol(m_BParser, NULL, 16);

///// Device Setting...
void CSingletonSerial::CreateSerial(UINT Buadrate1, UINT Buadrate2)
{
	// RCC Config..
	//////////////////////////////////////////////////////////////////////////////
	/* Enable USART1, GPIOA, GPIOx and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOx
                         | RCC_APB2Periph_AFIO, ENABLE);
	/* Enable USART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// NVIC Config..
	//////////////////////////////////////////////////////////////////////////////
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// GPIO Config..
	//////////////////////////////////////////////////////////////////////////////
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
  
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOx, &GPIO_InitStructure);

	// UART Config..
	///////////////////////////////////////////////////////////////////////////////
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = Buadrate1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	/* Configure USART2 */
	USART_InitStructure.USART_BaudRate = Buadrate2;
	USART_Init(USART2, &USART_InitStructure);
  
	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	/* Enable USART2 Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);
	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);
}


//// Receive Interrupt.. 
#define RXQUEUE	1
extern "C"{
	void USART2_IRQHandler(void)
	{
		UINT c=0;
		// Rx Interrupt...
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		{
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
			// Queue Push
			Send->GetQueueHandle(RXQUEUE)->SetPush((BYTE) USART_ReceiveData(USART2));

			// Receive Code..
			//Send->GetQueueHandle()->SetPush( (BYTE) USART_ReceiveData(USART2)); //Echo Test..
		}
		
		// Tx Interrupt...
		if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)	
		{
			USART_ClearITPendingBit(USART2, USART_IT_TXE);
			c = Send->GetQueueHandle()->GetPop();

			if(c == 0x8000)		//USART_SendData(USART2, '@');
				USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			else
				USART_SendData(USART2, c);
		}		
	}
}

extern "C"{
	void USART1_IRQHandler(void)
	{
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		}
	}
}
