/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "main.h"



volatile uint8_t data_ready_flag = 0; // Флаг окончания чтения линейки
volatile uint8_t CCD_read_flag = 0; // Флаг начала чтения линейки
__IO uint16_t CCD_Buffer[3694]; // Буфер чения линейки
uint8_t  USB_Tx_Buffer[TX_BUFFER_SIZE]; // буфер передачи
char flag = 0; // Флаг передачи данных
volatile uint8_t RX_data = '\0';


/* SH_period определяет время интеграции t_int.
 Минимальное время равно 10мкс что соответствует SH_period = 20.
 Максимальное время определяется периодом ICG.*/
int8_t SH_period = 20;



void LedInit(void);
void delay(uint32_t len);

int main(void)
{
  sysclock_init();
  gpio_init();
  timer_init();
  adc_init();   
  nvic_init();
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  
    
  while (1)
  {
      
         if(RX_data != '\0')
        {
                        
            //sprintf(Tx_Buffer, "data is - %c\n", RX_data);
            //USB_Send_Str(Tx_Buffer, 12);
            //block_transfer(Tx_Buffer, 12);
            
            if(RX_data == 'g')
            {    
                CCD_read_flag = 1;  
            }

            RX_data = '\0';
        }

        if(data_ready_flag == 1)
        {
            data_ready_flag = 0;
            
           // USB_Send_Str("begin\n", 6);
            USB_Send("begin\n", 6);

            uint16_t i = 32;
            while(i < (CCD_size-15))
            {
                
                delay(5000);
                sprintf(USB_Tx_Buffer, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", CCD_Buffer[i] , CCD_Buffer[i+1], CCD_Buffer[i+2], 
                            CCD_Buffer[i+3], CCD_Buffer[i+4] , CCD_Buffer[i+5], CCD_Buffer[i+6], CCD_Buffer[i+7],
                            CCD_Buffer[i+8] , CCD_Buffer[i+9], CCD_Buffer[i+10], CCD_Buffer[i+11]);
                
                USB_Send(USB_Tx_Buffer, 60);
                i += 12;                
            }
            
             delay(5000);
            USB_Send("end\n", 4);
            // USB_Send_Str("end\n", 4);  

            
         }
   }
}

void delay(uint32_t len)
{
    for(uint32_t i = 0; i < len; i++);

}


#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
