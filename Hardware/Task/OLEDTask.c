#include "cmsis_os2.h"
#include "main.h"
#include "OLED.h"
#include "FreeRTOS.h"

void StartOLEDTask(void *argument)
{
    OLED_Init();

    for(;;)
    {
        MPU6050_Data *SensorData;
        if (osMessageQueueGet(sensorQueueHandle, &SensorData, 0, osWaitForever) == osOK)
        {
            if (SensorData != NULL)
            {
                OLED_ShowSignedNum(2,1,SensorData->AX,5);
                OLED_ShowSignedNum(3,1,SensorData->AY,5);
                OLED_ShowSignedNum(4,1,SensorData->AZ,5);
                OLED_ShowSignedNum(2,8,SensorData->GX,5);
                OLED_ShowSignedNum(3,8,SensorData->GY,5);
                OLED_ShowSignedNum(4,8,SensorData->GZ,5);
                vPortFree(SensorData);
            }
        }

        osDelay(100);
    }
}
