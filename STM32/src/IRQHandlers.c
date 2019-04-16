#include "main.h"

volatile int count = 0;
void TIM3_IRQHandler (void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{	
		
        /* Clear TIM3 update interrupt */
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	
	/* Restart TIM4 as this gets the ADC running again */
        if(CCD_read_flag == 1)
        {
            CCD_read_flag = 0;
            TIM4->CNT = 0;
            TIM_Cmd(TIM4, ENABLE);
        }
    }
}


void DMA1_Channel1_IRQHandler(void)
{

	/* Test for DMA Stream Transfer Complete interrupt */
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA1_IT_TC1);
    
		/* Stop TIM4 and thus the ADC */
		TIM_Cmd(TIM4, DISABLE);

		data_ready_flag = 1;
	}
}


