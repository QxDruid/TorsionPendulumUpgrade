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
extern volatile uint8_t RX_data;


extern volatile uint8_t data_ready_flag;
extern volatile uint8_t CCD_read_flag;

void sysclock_init(void);
void nvic_init(void);
void gpio_init(void);
void usart_init(void);
void adc_init(void);
void timer_init(void);

//void DMA1_Channel1_IRQHandler(void);
//void TIM1_UP_IRQHandler (void);

#endif
