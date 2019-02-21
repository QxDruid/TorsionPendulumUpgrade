#include "init.h"

/* Конфигурация прерываний */
void nvic_init(void)
{
    // Структура инициализации прерывания TIM3
    NVIC_InitTypeDef NVIC_InitStructure;
    // Разрешаем прерывание
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    // Приоритет прерывания наивысший 
    // (так как у нас всего 1 прерывание не страшно)
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // СубПриоритет наивысший
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    // Вклчаем прерывание
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // Функция инициализации
    NVIC_Init(&NVIC_InitStructure);
}

/* Инициализация пинов контроллера */
void gpio_init(void)
{
    // Подключаем PORTA и PORTC к тактированию.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
    
    // Структура инициализации пина.
    GPIO_InitTypeDef GPIO_initStruct;
    
    /*
             Настройка LED на PIN13 PORTC.
--------------------------------------------------------------------------------
    */
    // Выбор пина для настройки.
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_13;
    // Mode: Push-Pull выход.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    // Максимальная скорость 2МГц.
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    // Функция инициализации.
    GPIO_Init(GPIOC, &GPIO_initStruct);
    // Устанавливаем Лог. 1 на Pin13 (гасим светодиод на плате). 
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    
    /*
             Настройка кнопки на PIN14 PORTC
--------------------------------------------------------------------------------
    */
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_14;
    // Mode: Вход с подтяжкой Pull-up.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_initStruct); 
    
    /*
             Настройка Выхода Шим для тактирования линейки
--------------------------------------------------------------------------------
    */
    // Выход подключен на Pin3 PORTA.
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_3;
    // Mode: альтернативный Push-Pull выход.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    // Максимальная скорость 2МГц.
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_initStruct);
    
    /*
             Пины для IGM и SH
--------------------------------------------------------------------------------
    */
    // Выходы подключены к Pin4 и Pin2 PORTA.
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_2;
    // Mode: Push-Pull выход.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    // Максимальная скорость 2МГц.
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_initStruct);
    
     /*
             Настройка пина TX для USART1
--------------------------------------------------------------------------------
    */
    // TX USART1 Находиться на Pin9 PORTA.
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_9;
    // Mode: альтернативный Push-Pull выход.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    // Максимальная скорость 50МГц.
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_initStruct);
    // Устанавливаем высокий уровень на линии TX.
    GPIO_SetBits(GPIOA, GPIO_Pin_9);
    
    /*
             Настройка пина RX для USART1
--------------------------------------------------------------------------------
    */
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_10;
    // Mode: Вход без подтяжки.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_initStruct);
}

/* Настройка USART */
void usart_init(void)
{
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
    /*
    Устанавливаем предделитель для ADC так как макс частота работы ADC 14МГц,
    а системнаое тактирование установлено на 72МГц. 72/6 = 12МГц.
    */
    RCC_ADCCLKConfig (RCC_PCLK2_Div6);
    // Подключаем ADC1 к тактированию
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    // Заполняем труктуру инициализации
    ADC_InitTypeDef ADC_initStruct;
    // Mode: ADC работают независимо.
    ADC_initStruct.ADC_Mode = ADC_Mode_Independent;
    // Align: Данные 12 битного ADC выравниваются вправо.
    ADC_initStruct.ADC_DataAlign = ADC_DataAlign_Right;
    // ContinuousMode: Однократное измерение.
    ADC_initStruct.ADC_ContinuousConvMode = DISABLE;
    // ExtTrigConv: Отключен внешний триггер преобразования.
    ADC_initStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    // NbrChannel: Количество каналов - 1.
    ADC_initStruct.ADC_NbrOfChannel= 1;
    // ScanCOnversion: Нет сканирования каналов.
    ADC_initStruct.ADC_ScanConvMode = DISABLE;
    //Выбираем канал регулярной группы (Pin A1) и устанавливаем время 
    //обработки одного преобразования
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
    ADC_Init(ADC1, &ADC_initStruct);
    
    // Включаем ADC.
    ADC_Cmd(ADC1, ENABLE);
    // сбрасываем колибровку.
    ADC_ResetCalibration(ADC1);
    // Ждем пока калибровка сброситься.
    while(ADC_GetResetCalibrationStatus(ADC1));
    // Запускаем новую калибровку.
    ADC_StartCalibration(ADC1);
    // Ждем пока закончится
    while(ADC_GetCalibrationStatus(ADC1));
    // Запускаем модуль ADC После калибровки
    ADC_Cmd(ADC1, ENABLE);
}

/* Настройка таймеров */
void timer_init()
{   
     /*
             Настройка ШИМ для CLK линейки
--------------------------------------------------------------------------------
    
    При частоте APB1 d 36МГц указанные ниже параметры 
    устанавливают частоту PWM в 1МГц и скважность 50%.
    
    */
    // Для генерации PWM включаем к тактированию таймер TIM2. 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // Структура инициализации Basic таймера.
    TIM_TimeBaseInitTypeDef timer;
    TIM_TimeBaseStructInit(&timer);
    // Делитель тактовой частоты: 1.
    timer.TIM_Prescaler = 1;
    // Период 36 тиков.
    timer.TIM_Period = 35;
    timer.TIM_ClockDivision = 0;
    // Таймер считает на увеличение.
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    // Инициализация таймера.
    TIM_TimeBaseInit(TIM2, &timer);
 
    // Структура инициализации PWM
    TIM_OCInitTypeDef timerPWM;
    TIM_OCStructInit(&timerPWM);
    // Тип шим с выравниванием по центру.
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    // Разрешен вывод PWM.
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    // Количество импульсов до установки Лог.0 (18/36 = 50% скаважность).
    timerPWM.TIM_Pulse = 17;
    // Полярность HIGH (чем выше Pulse тем больще скважность).
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_High;
    // Выход PWM устанавливаем на канале 4 таймера TIM2.
    TIM_OC4Init(TIM2, &timerPWM);
    // Включаем таймер TIM2
    TIM_Cmd(TIM2, ENABLE);  

    /*
             Настройка таймера для побитного чтения линейки для CLK линейки
--------------------------------------------------------------------------------    
        При частоте APB1 36МГц данные параметры устанавливают период
        через который происходит переполнение таймера в 10 мкс.
        36МГц/360тиков = 100КГц = 10мкс.
    */
    // Подключаем таймер TIM3 к тактированию.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // Таймер считает на увеличение.
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    // Делитель тактовой частоты: 1.
    timer.TIM_Prescaler = 1;
    // Период 36 тиков.
    timer.TIM_Period = 359;
    timer.TIM_ClockDivision = 0;
    // Инициализация TIM3.
    TIM_TimeBaseInit(TIM3, &timer);
    // Включаем TIM3.
    TIM_Cmd(TIM3, ENABLE);
    // Включаем прерывание по сбросу для TIM3.
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}