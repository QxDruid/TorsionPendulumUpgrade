#include "init.h"

volatile uint8_t data_ready_flag = 0;
volatile uint8_t CCD_read_flag = 0;

// Буфер чения линейки
__IO uint16_t CCD_Buffer[3694]; 
volatile uint8_t RX_data = '\0';
char USART_buffer[50] = {'\0'};
char USART_data[5] = {'\0'};
    
int main()
{
    // Настройка такирование ядра и переферии
    sysclock_init();
    gpio_init();
    timer_init();
    adc_init();
    usart_init();    
    nvic_init();
    
    USARTSend("0\n", sizeof("0\n"));
    USARTSend("CCD Ready\n", sizeof("CCD Ready\n"));
    
    
    while(1)
    {
        if(RX_data != '\0')
        {
                        
            sprintf(USART_buffer, "data is - %c\r\n", RX_data);
            USARTSend(USART_buffer, sizeof(USART_buffer)); 
            
            if(RX_data == 'g')
            {    
                CCD_read_flag = 1;
                GPIOC->ODR ^= GPIO_Pin_13;   
            }
            if(RX_data == 't')
            {    
                for(int i = 0; i < 3694; i++)
                {
            // Записываем значение int в строку вывода как массив char
                sprintf(USART_data, "%d\n", CCD_Buffer[i]);
                // Отправляем данные
                USARTSend(USART_data, sizeof(USART_data));
                }  
            }
            RX_data = '\0';
        }

        if(data_ready_flag == 1)
        {
            for(int i = 0; i < 3694; i++)
            {
            // Записываем значение int в строку вывода как массив char
                sprintf(USART_data, "%d.", CCD_Buffer[i]);
                // Отправляем данные
                USARTSend(USART_data, sizeof(USART_data));
            }
            data_ready_flag = 0;
        }
            
    }
    return 0;
    
}    
    
