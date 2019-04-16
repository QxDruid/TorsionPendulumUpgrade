#include "main.h"

void sysclock_init(void)
{
    ErrorStatus HSEStartUpStatus;
 
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* Системный RESET RCC (делать не обязательно, но полезно на этапе отладки) */
    RCC_DeInit();
 
    /* Включаем HSE (внешний кварц) */
    RCC_HSEConfig( RCC_HSE_ON);
 
    /* Ждем пока HSE будет готов */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
 
    /* Если с HSE все в порядке */
    if (HSEStartUpStatus == SUCCESS)
    {
/* 
    Следующие две команды касаются исключительно работы с FLASH.
    Если вы не собираетесь использовать в своей программе функций работы с Flash,
    FLASH_PrefetchBufferCmd( ) та FLASH_SetLatency( ) можно закомментировать 
 
        Включаем Prefetch Buffer 
        FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);
 
        FLASH Latency.
    Рекомендовано устанавливать:
        FLASH_Latency_0 - 0 < SYSCLK≤ 24 MHz
        FLASH_Latency_1 - 24 MHz < SYSCLK ≤ 48 MHz
        FLASH_Latency_2 - 48 MHz < SYSCLK ≤ 72 MHz 
        FLASH_SetLatency( FLASH_Latency_2);
 */
        /* HCLK = SYSCLK */ /* Смотри на схеме AHB Prescaler. Частота не делится (RCC_SYSCLK_Div1) */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);
 
        /* PCLK2 = HCLK */ /* Смотри на схеме APB2 Prescaler. Частота не делится (RCC_HCLK_Div1)  */
        RCC_PCLK2Config( RCC_HCLK_Div1);
 
        /* PCLK1 = HCLK/2 */ /* Смотри на схеме APB1 Prescaler. Частота делится на 2 (RCC_HCLK_Div2)
        потому что на выходе APB1 должно быть не более 36МГц (смотри схему) */
        RCC_PCLK1Config( RCC_HCLK_Div2);
        /* СЛЕДУЕТ ПОМНИТЬ ЧТО ЕСЛИ ДЕЛИТЕЛЬ APB |= 1 ТО ДЛЯ ИАКТИРОВАНИЯ ТАЙМЕРОВ
        ЧАСТОТА УМНОЖАЕТСЯ НА 2 (Т.Е. ЧАСТОТА TIM2.3.4 = 72Мгц) */
 
        /* PLLCLK = 8MHz * 9 = 72 MHz
        Указываем PLL от куда брать частоту (RCC_PLLSource_HSE_Div1) и на сколько ее умножать (RCC_PLLMul_9) 
        PLL может брать частоту с кварца как есть (RCC_PLLSource_HSE_Div1) или поделенную на 2 (RCC_PLLSource_HSE_Div2). Смотри схему */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
 
        /* Включаем PLL */
        RCC_PLLCmd( ENABLE);
 
        /* Ждем пока PLL будет готов */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }
 
        /* Переключаем системное тактирование на PLL */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);
 
        /* Ждем пока переключиться */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* Проблемы с HSE. Тут можно написать свой код, если надо что-то делать когда микроконтроллер не смог перейти на работу с внешним кварцом */
 
        /* Пока тут заглушка - вечный цикл*/
        while (1)
        {
        }
    }
}
