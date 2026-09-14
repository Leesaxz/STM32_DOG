#include "cmsis_os2.h"
#include "main.h"
#include "FreeRTOS.h"

void StartCommandTask(void *argument)
{
    /* USER CODE BEGIN StartCommandTask */
    uint8_t Dog_Data;
    static uint8_t last_data = 0xFF;
    /* Infinite loop */
    for(;;)
    {
        Dog_Data = (HAL_GPIO_ReadPin(Data_L_GPIO_Port, Data_L_Pin)      ? 0x01 : 0)
                 | (HAL_GPIO_ReadPin(Data_M_GPIO_Port, Data_M_Pin)      ? 0x02 : 0)
                 | (HAL_GPIO_ReadPin(Data_H_GPIO_Port, Data_H_Pin)      ? 0x04 : 0);    
        if (Dog_Data != last_data)
        {
            last_data = Dog_Data;
            osMessageQueuePut(action0QueueHandle, &Dog_Data, 0, 0);
        }
        osDelay(10);
    }
    /* USER CODE END StartCommandTask */
}
