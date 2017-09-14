#include <usart.h>

void USART1_Init(void)
{
	GPIO_InitTypeDef		GPIO_InitStruct;
	USART_InitTypeDef		USART_InitStruct;
	
	// enable peripheral clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	// map port B pins for alternate function
	GPIO_InitStruct.GPIO_Pin		=	GPIO_Pin_6 | GPIO_Pin_7;	// Pins 6(TX) and 7(RX) will be used for USART1
	GPIO_InitStruct.GPIO_Mode		= GPIO_Mode_AF;		//GPIO pins defined as alternate
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;	//I/O pins speed (signal rise time)
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;		// push-pull output
	GPIO_InitStruct.GPIO_PuPd		=	GPIO_PuPd_UP;			//activates pullup resistors
	GPIO_Init(GPIOB, &GPIO_InitStruct);		//set chosen pins
	
	// set alternate function to USART1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //pins will automatically be assigned to TX/RX
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
	
	// use USART_InitStruct to config USART1 peripheral
	USART_InitStruct.USART_BaudRate		= BAUDRATE;
	USART_InitStruct.USART_WordLength	= USART_WordLength_8b; // 8 data bits
	USART_InitStruct.USART_StopBits		= USART_StopBits_1;
	USART_InitStruct.USART_Parity			= USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode				= USART_Mode_Tx | USART_Mode_Rx; //enable both character transmit and receive
	USART_Init(USART1, &USART_InitStruct);		// set USART1 peripheral
	
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART1, USART_IT_TC, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	
	USART_Cmd(USART1, ENABLE);
}

void USART2_Init(void)
{
	GPIO_InitTypeDef		GPIO_InitStruct;
	USART_InitTypeDef		USART_InitStruct;
	
	// enable peripheral clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	// map port B pins for alternate function
	GPIO_InitStruct.GPIO_Pin		=	GPIO_Pin_2 | GPIO_Pin_3;	// Pins 2(TX) and 3(RX) will be used for USART2
	GPIO_InitStruct.GPIO_Mode		= GPIO_Mode_AF;		//GPIO pins defined as alternate
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;	//I/O pins speed (signal rise time)
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;		// push-pull output
	GPIO_InitStruct.GPIO_PuPd		=	GPIO_PuPd_UP;			//activates pullup resistors
	GPIO_Init(GPIOA, &GPIO_InitStruct);		//set chosen pins
	
	// set alternate function to USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //pins will automatically be assigned to TX/RX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	// use USART_InitStruct to config USART1 peripheral
	USART_InitStruct.USART_BaudRate		= BAUDRATE;
	USART_InitStruct.USART_WordLength	= USART_WordLength_8b; // 8 data bits
	USART_InitStruct.USART_StopBits		= USART_StopBits_1;
	USART_InitStruct.USART_Parity			= USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode				= USART_Mode_Tx | USART_Mode_Rx; //enable both character transmit and receive
	USART_Init(USART2, &USART_InitStruct);		// set USART1 peripheral
	
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	
	USART_Cmd(USART2, ENABLE);
}