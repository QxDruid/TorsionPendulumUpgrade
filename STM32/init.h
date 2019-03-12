#ifndef INITH
#define INITH

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dma.h"
#include "stdio.h"

extern volatile uint16_t CCD_Buffer[3694]; 

void sysclock_init(void);
void nvic_init(void);
void gpio_init(void);
void usart_init(void);
void adc_init(void);
void timer_init(void);

#endif
