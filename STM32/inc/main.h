#ifndef INITH
#define INITH

/*

        Pin description
================================================
    F_m     PA8     Tim1_C1
    ICG     PA6     Tim3_C1
    SH      PA1     Tim2_C2
    OS      PA0     ADC Tim4_C4
    TX      PA9
    RX      PA10
================================================
*/


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

#define TX_BUFFER_SIZE 64
#define POCKET_SIZE 100

extern int8_t SH_period;
extern volatile uint16_t CCD_Buffer[3694];
extern volatile uint8_t RX_data;
extern volatile uint8_t data_ready_flag;
extern volatile uint8_t CCD_read_flag;
extern volatile uint8_t CCD_get_flag; 

void sysclock_init(void);
void nvic_init(void);
void gpio_init(void);
void adc_init(void);
void timer_init(void);

void send_a_transaction(void);

#endif
