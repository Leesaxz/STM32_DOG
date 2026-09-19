#include <stdlib.h>
#include "cmsis_os2.h"
#include "main.h"
#include "FreeRTOS.h"
#include "usart.h"


void StartSensorTask(void *argument)
{
    /* USER CODE BEGIN StartActionTask */
    MPU6050_Init();

    /* Infinite loop */
    for(;;)
    {
        MPU6050_Data* SensorData =  pvPortMalloc(sizeof(MPU6050_Data));
        if(SensorData != NULL)
        {
            MPU6050_GetData(&SensorData->AX, &SensorData->AY, &SensorData->AZ,
    &SensorData->GX,&SensorData->GY, &SensorData->GZ);

            HAL_UART_Transmit_DMA(&huart2, (uint8_t *)SensorData, sizeof(MPU6050_Data));

            osMessageQueuePut(sensorQueueHandle, (&SensorData), 0,osWaitForever);
        }

        osDelay(200);
    }
}
