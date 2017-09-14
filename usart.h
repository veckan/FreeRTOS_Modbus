#ifndef USART_H
#define	USART_H

#include <stm32f4xx.h>					// common stuff
#include <stm32f4xx_rcc.h>			// reset and clocking
#include <stm32f4xx_gpio.h>			// gpio control
#include <stm32f4xx_usart.h>		// USART

#define BUFSIZE					16
#define BAUDRATE				115200

void USART1_Init(void);					// init USART1 peripheral
void USART2_Init(void);					// init USART2 peripheral

#endif
