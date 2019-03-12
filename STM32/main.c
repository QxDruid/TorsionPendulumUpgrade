#include "init.h"

// Буфер чения линейки
volatile uint16_t CCD_Buffer[3694]; 

int main()
{
    // Настройка такирование ядра и переферии
    sysclock_init();
    gpio_init();
    timer_init();
    
    while(1)
    {
    }
    return 0;
}
