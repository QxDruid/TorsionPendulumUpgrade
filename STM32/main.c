#include "init.h"

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
