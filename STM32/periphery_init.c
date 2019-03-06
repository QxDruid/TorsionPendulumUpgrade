#include "init.h"

// Частота тактирования таймеров
#define APB1_frec 72000000
// Частота тактирования линейки f_m, Гц
#define f_m 2000000 
// Количество пикселей в линейке
#define CCD_size 3694
//Задержки в импульсах при запуске таймеров для вормирования 
// Timing requirements пзс линейки.
#define SH_delay 12
#define ICG_delay 11
#define fm_delay 3
// Период считывания линейки.
#define ICG_period 50000

// определяет время интеграции t_int.
// Минимальное время равно 10мкс что соответствует SH_period = 20.
// Максимальное время определяется периодом ICG.
int8_t SH_period = 20;

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
    // Включаем тактирование модуля USART1.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    /*
             Настройка параметров USART1
--------------------------------------------------------------------------------
    */
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
    // Структура инициализации GPIO_PIN.
    GPIO_InitTypeDef GPIO_initStruct;
     /*
             Настройка Выхода Шим для F_m (TIM1_C1)
--------------------------------------------------------------------------------
    */
    // Выход подключен на Pin8 PORTA.
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_8;
    // Mode: альтернативный Push-Pull выход.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    // Максимальная скорость 2МГц.
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_initStruct);
    
      
     /*
             Настройка Таймера TIM1 для F_m
--------------------------------------------------------------------------------
    
    При частоте APB1 36МГц (тактирование таймера 36*2) указанные ниже параметры 
    устанавливают частоту PWM в 2 МГц и скважность 50%.
    */
    // Для генерации PWM включаем к тактированию таймер TIM1. 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    // Для Advanced таймеров отдельно подлючается выход ШИМ
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    // Структура инициализации Basic таймера.
    TIM_TimeBaseInitTypeDef timer;
    TIM_TimeBaseStructInit(&timer);
    // Делитель тактовой частоты: 1.
    timer.TIM_Prescaler = 2-1;
    // Период 36 тиков.
    timer.TIM_Period = APB1_frec/f_m - 1;
    timer.TIM_ClockDivision = 0;
    // Таймер считает на увеличение.
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    // Инициализация таймера.
    TIM_TimeBaseInit(TIM1, &timer);
 
    // Структура инициализации PWM
    TIM_OCInitTypeDef timerPWM;
    TIM_OCStructInit(&timerPWM);
    // Тип шим с выравниванием по центру.
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    // Разрешен вывод PWM.
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    // Количество импульсов до установки Лог.0 (18/36 = 50% скаважность).
    timerPWM.TIM_Pulse = APB1_frec/(2*f_m);
    // Полярность Low (чем ниже Pulse тем больще скважность).
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_Low;
    // Выход PWM устанавливаем на канале 1 (ОС1) таймера TIM2.
    TIM_OC1Init(TIM1, &timerPWM);
    // Включаем пресет таймера TIM1
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);

// Включаем TIM1
	TIM_Cmd(TIM1, ENABLE); 
    

/*
             Пин  c ШИМ для SH (TIM2_C2)
--------------------------------------------------------------------------------
    */
    // Выход подключен к Pin1 PORTA.
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_1;
    // Mode: Push-Pull выход.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    // Максимальная скорость 2МГц.
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
    // Подключаем таймер TIM2 к тактированию.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // Таймер считает на увеличение.
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    // Делитель тактовой частоты:
    timer.TIM_Prescaler = APB1_frec/f_m-1;
    // Период таймера задает t_int (минимум 20 тиков = 10мкс)
    timer.TIM_Period = SH_period - 1;
    timer.TIM_ClockDivision = 0;
    // Инициализация TIM2.
    TIM_TimeBaseInit(TIM2, &timer);
    
    // Тип шим с выравниванием по центру.
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    // Разрешен вывод PWM.
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    // Длительность SH = 2 мкс -> pulse = 2 µs * CCD_fm, MHz.
    timerPWM.TIM_Pulse = (2 * f_m) / 1000000;
    // Полярность Low.
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_Low;
    // Выход PWM устанавливаем на канале 2 таймера TIM2.
    TIM_OC2Init(TIM2, &timerPWM);
    // Включаем Пресет таймеа  
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);


    /*
             Пин  c ШИМ для ICG (TIM3_C1)
--------------------------------------------------------------------------------
    */
    // Выход подключен к Pin6 PORTA.
    GPIO_initStruct.GPIO_Pin = GPIO_Pin_6;
    // Mode: Push-Pull выход.
    GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    // Максимальная скорость 2МГц.
    GPIO_initStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_initStruct);
/*
             Настройка таймера TIM3 для ICG
--------------------------------------------------------------------------------    
 */
    // Подключаем таймер TIM3 к тактированию.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // Таймер считает на увеличение.
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    // Делитель тактовой частоты:
    timer.TIM_Prescaler = APB1_frec/f_m-1;
    // Период равен периоду считывания линейки
    timer.TIM_Period = ICG_period-1;
    timer.TIM_ClockDivision = 0;
    // Инициализация TIM3.
    TIM_TimeBaseInit(TIM3, &timer);
    
    // Тип шим с выравниванием по центру.
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    // Разрешен вывод PWM.
    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
    // Длительность импульса 5мкс.
    timerPWM.TIM_Pulse = (5 * f_m) / 1000000;
    // Полярность HIGH (чем выше Pulse тем больще скважность).
    timerPWM.TIM_OCPolarity = TIM_OCPolarity_High;
    // Выход PWM устанавливаем на канале 1 таймера TIM3.
    TIM_OC1Init(TIM3, &timerPWM);
    // Включаем пресет таймера TIM3
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE); 
    
    /* 	Запуск таймеров */
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

// Установка задержек между импульсами таймеров для соответсвия 
// Timing requiremets линейки TCD1304AP
    TIM2->CNT = SH_period - SH_delay;// + (SH_period % 2);
	TIM3->CNT = ICG_period - ICG_delay;
	TIM1->CNT = fm_delay;
}
