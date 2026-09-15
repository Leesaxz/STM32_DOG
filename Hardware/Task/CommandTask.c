#include "cmsis_os2.h"
#include "main.h"
#include "FreeRTOS.h"

void StartCommandTask(void *argument)
{
    /* USER CODE BEGIN StartCommandTask */
    /* Infinite loop */
    for(;;)
    {
        osDelay(10);
    }
    /* USER CODE END StartCommandTask */
}
