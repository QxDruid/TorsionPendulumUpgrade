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

#define APB1_frec 72000000 // Частота тактирования таймеров
#define f_m 2000000 // Частота тактирования линейки f_m, Гц
#define CCD_size 3694 // Количество пикселей в линейке
#define ICG_period 65000 // Период считывания линейки.
/* x_delay - Задержки в импульсах при запуске таймеров для вормирования 
 Timing requirements пзс линейки.
*/
#define SH_delay 12
#define ICG_delay 11
#define fm_delay 3

extern int8_t SH_period;
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

#endif
