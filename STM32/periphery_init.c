#include "main.h"

/* Конфигурация прерываний */
void nvic_init(void)
{
	NVIC_InitTypeDef		NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* ICG (TIM3) IRQ */
	/* Обновление TIM3 (Конец импульса ICG)
    запускает TIM4 который управляет запуском ADC */
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    
	/* ADC-DMA IRQ */
	/* Окончание передачи DMA1 останавливает TIM4 и ADC1 */

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    	/* USART IRQ */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
}

/* Инициализация пинов контроллера */
void gpio_init(void)
{
    // Подключаем PORTA и PORTC к тактированию.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);  
    
    GPIO_InitTypeDef GPIO_initStruct;
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_initStruct);
}

/* Настройка USART */
void usart_init(void)
{
    /*
             Настройка пина TX для USART1
--------------------------------------------------------------------------------
    */
    GPIO_InitTypeDef GPIO_initStruct;
    // TX USART1 Находиться на Pin9 PORTA.
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_9;
    // Mode: альтернативный Push-Pull выход.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    // Максимальная скорость 50МГц.
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_initStruct);
    // Устанавливаем высокий уровень на линии TX.
    //GPIO_SetBits(GPIOA, GPIO_Pin_9);
    
    /*
             Настройка пина RX для USART1
--------------------------------------------------------------------------------
    */
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_10;
    // Mode: Вход без подтяжки.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_initStruct);

    
    /*
             Настройка параметров USART1
--------------------------------------------------------------------------------
    */
        // Включаем тактирование модуля USART1.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    // Структура инициализации.
    USART_InitTypeDef USART_initStruct = {0};
    // BaudRate: 115200.
    USART_initStruct.USART_BaudRate = 115200;
    // Длинна слова 8 бит.
    USART_initStruct.USART_WordLength = USART_WordLength_8b;
    // 1 стоп бит.
    USART_initStruct.USART_StopBits = USART_StopBits_1;
    // Нет бита четности.
    USART_initStruct.USART_Parity = USART_Parity_No;
    // Разрешена передача и прием.
    USART_initStruct.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    // Аппаратный контроль потока отключен.
    USART_initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    // Функция инициализации
    USART_Init(USART1, &USART_initStruct);
    // Включаем USART1.
    USART_Cmd(USART1, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void USARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    // Цикл побайтовой передачи посылки.
    while(ulCount--)
    {
        // Отправляем текуший байт и переносим указатель на следующий байт посылки
        USART_SendData(USART1, *pucBuffer++);// Last Version USART_SendData(USART1,(uint16_t) *pucBuffer++);
        // Ждем пока байт не отправится.
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }
    }
}

/* Настройка ADC */
void adc_init(void)
{
    GPIO_InitTypeDef initStruct;
    initStruct.GPIO_Pin = GPIO_Pin_0;
    initStruct.GPIO_Mode = GPIO_Mode_AIN;
    initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &initStruct);
    /*
    Устанавливаем предделитель для ADC так как макс частота работы ADC 14МГц,
    а системнаое тактирование установлено на 72МГц. 72/6 = 12МГц.
    */
    RCC_ADCCLKConfig (RCC_PCLK2_Div6);
    // Подключаем ADC1 к тактированию
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // Подключаем DMA1
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    // Структура инициализации DMA
    
    DMA_InitTypeDef DMA_initStruct;
    DMA_DeInit(DMA1_Channel1);
    DMA_ClearITPendingBit(DMA1_IT_TC1);
    DMA_initStruct.DMA_BufferSize = 3694;
    DMA_initStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_initStruct.DMA_MemoryBaseAddr = (uint32_t)&CCD_Buffer;
    DMA_initStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_initStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_initStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_initStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_initStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_initStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_initStruct.DMA_Priority = DMA_Priority_High;
    DMA_initStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_initStruct);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1 , ENABLE ) ;
    
    // Заполняем труктуру инициализации
    ADC_InitTypeDef ADC_initStruct;
    ADC_initStruct.ADC_Mode = ADC_Mode_Independent; // ADC работают независимо.
    ADC_initStruct.ADC_DataAlign = ADC_DataAlign_Right; //Данные 12 битного ADC выравниваются вправо.
    ADC_initStruct.ADC_ContinuousConvMode = DISABLE; // Однократное измерение.
	ADC_initStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4; // внешний триггер по захвату на канале T4C4
    ADC_initStruct.ADC_NbrOfChannel= 1; // Количество каналов - 1
    ADC_initStruct.ADC_ScanConvMode = DISABLE; // Нет сканирования каналов.
    //Выбираем канал регулярной группы (Pin A1) и устанавливаем время 
    //обработки одного преобразования
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
    ADC_Init(ADC1, &ADC_initStruct); // Инициализация ADC
    
    ADC_DMACmd(ADC1, ENABLE); // ВКлючаем DMA
    ADC_Cmd(ADC1, ENABLE); // Включаем ADC.
    
    // Обязательная калибровка ADC.
    ADC_ResetCalibration(ADC1);  // сбрасываем колибровку.
    while(ADC_GetResetCalibrationStatus(ADC1)); // Ждем пока калибровка сброситься.
    ADC_StartCalibration(ADC1);// Запускаем новую калибровку.
    while(ADC_GetCalibrationStatus(ADC1));// Ждем пока закончится
    ADC_Cmd(ADC1, ENABLE); // Запускаем модуль ADC После калибровки
}

/* Настройка таймеров */
void timer_init()
{   
    
    GPIO_InitTypeDef GPIO_initStruct; // Структура инициализации GPIO_PIN.
     /*
             Настройка Выхода Шим для F_m (TIM1_C1)
--------------------------------------------------------------------------------
    */
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_8; // Выход подключен на Pin8 PORTA.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP; // альтернативный Push-Pull выход.
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz; // Максимальная скорость 2МГц.
    GPIO_Init(GPIOA, &GPIO_initStruct);
    
      
     /*
             Настройка Таймера TIM1 для F_m
--------------------------------------------------------------------------------
    
    При частоте APB1 36МГц (тактирование таймера 36*2 = 72 МГц)
    указанные ниже параметры устанавливают частоту PWM
    в 2 МГц и скважность 50%.
    */
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // Для генерации PWM включаем к тактированию таймер TIM1. 
    TIM_CtrlPWMOutputs(TIM1, ENABLE); // Для Advanced таймеров отдельно подлючается выход ШИМ
    
    // Структура инициализации Basic таймера.
    TIM_TimeBaseInitTypeDef timer;
    TIM_TimeBaseStructInit(&timer);
    
    timer.TIM_Prescaler = 1-1; // Делитель тактовой частоты: 0.
    timer.TIM_Period = APB1_frec/f_m - 1; // Расчет периода для частоты 2 Мгц
    timer.TIM_ClockDivision = 0; // Деление тактовой частоты 0.
    timer.TIM_CounterMode = TIM_CounterMode_Up; // Таймер считает на увеличение.
    TIM_TimeBaseInit(TIM1, &timer); // Инициализация таймера.
 
    // Структура инициализации PWM
    TIM_OCInitTypeDef timerPWM;
    TIM_OCStructInit(&timerPWM);
    
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1; // Тип шим с выравниванием по центру.   
    timerPWM.TIM_OutputState = TIM_OutputState_Enable; // Разрешен вывод PWM.   
    timerPWM.TIM_Pulse = APB1_frec/(2*f_m); // 50% скаважность.
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_Low; // чем ниже Pulse тем больще скважность.
    TIM_OC1Init(TIM1, &timerPWM);  // Выход PWM устанавливаем на канале 1 (ОС1) таймера TIM2.
    
    // Включаем пресет таймера TIM1
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);

    // Включаем TIM1 (Тактирование линейки)
	TIM_Cmd(TIM1, ENABLE); 
    

     /*
             Пин  c ШИМ для SH (TIM2_C2)
--------------------------------------------------------------------------------
    */
    
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_initStruct);
    
    /*
             Настройка таймера TIM2 для SH
--------------------------------------------------------------------------------    
    Для синхронизации таймер должен тактироваться с частотой F_m
    соответственно Preskaler = APB1_frec/F_m-1;
    APB1_Frec = 72000000;

    Минимальное t_int = 10мкс;
    t_int = period/F_m(МГц)
    соответсвенно период таймера будет равен 10; 
    */
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // Подключаем таймер TIM2 к тактированию.
    
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    timer.TIM_Prescaler = APB1_frec/f_m-1;
    timer.TIM_Period = SH_period - 1;  // Период таймера задает t_int (минимум 20 тиков = 10мкс)
    timer.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM2, &timer);
    
    // Инициализация ШИМ
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    
    timerPWM.TIM_Pulse = (2 * f_m) / 1000000; // Длительность SH = 2 мкс -> pulse = 2 µs * CCD_fm, MHz.
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC2Init(TIM2, &timerPWM);
    // Включаем пресет таймера
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);


    /*
             Пин  c ШИМ для ICG (TIM3_C1)
--------------------------------------------------------------------------------
    */
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_initStruct);
/*
             Настройка таймера TIM3 для ICG
--------------------------------------------------------------------------------    
 */
    // Подключаем таймер TIM3 к тактированию.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    timer.TIM_CounterMode = TIM_CounterMode_Up;
    timer.TIM_Prescaler = APB1_frec/f_m-1;
    timer.TIM_Period = ICG_period-1;
    timer.TIM_ClockDivision = 0;
    // Инициализация TIM3.
    TIM_TimeBaseInit(TIM3, &timer);
    
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    timerPWM.TIM_Pulse = (5 * f_m) / 1000000;     // Длительность импульса 5мкс.
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_High; // Полярность High
    TIM_OC1Init(TIM3, &timerPWM);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    // Включаем пресет таймера TIM3
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE); 
    
    /*
    Настройка таймера TIM4 для запуска ADC
--------------------------------------------------------------------------------    
 */
 	GPIO_initStruct.GPIO_Pin = GPIO_Pin_9;
  	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_initStruct);
 
    // Подключаем таймер TIM4 к тактированию.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    timer.TIM_CounterMode = TIM_CounterMode_Up;
    timer.TIM_Prescaler = 1-1;
    timer.TIM_Period = 4*APB1_frec/f_m-1;  // Период равен 4 тактам f_m
    timer.TIM_ClockDivision = 0;
    // Инициализация TIM4.
    TIM_TimeBaseInit(TIM4, &timer);
    
    // Инициализация ШИМ
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
	timerPWM.TIM_OutputState = TIM_OutputState_Enable;
	timerPWM.TIM_Pulse = 2*APB1_frec /f_m-1;
	timerPWM.TIM_OCPolarity = TIM_OCPolarity_High;

    //TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_OC4Init(TIM4, &timerPWM);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
    //TIM4 Включается по прерыванию ICG (TIM3);
    TIM_Cmd(TIM4, DISABLE);
    //Сообщение в прерывании для отладки
    
    
    /*
            ЗАПУСК И СИНХРОНИЗАЦИЯ ТАЙМЕРОВ
 -------------------------------------------------------------------------------
    */
    
    // Запуск таймерв SH и ICG.
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

// Установка задержек между импульсами таймеров для соответсвия 
// Timing requiremets линейки TCD1304AP
// Устанавливаем таймеры на x_delay тиков перед переключением
// Т.К SH полярность Low а ICG - High. SH переключится в 0 а ICG в 1
// (На миксросхеме 74HC04 произойдет инверсия и все станет как в datasheet)
// Разница в x_delay между ICG и SH в 1 тик что дает задержку в 500нс.
// fm_delay подбирается так, чтоб уровень ICG Падал при высоком уровне f_m.
    TIM2->CNT = SH_period - SH_delay;  
	TIM3->CNT = ICG_period - ICG_delay;
	TIM1->CNT = fm_delay;
}
