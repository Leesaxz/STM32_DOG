#include <string.h>
#include "cmsis_os2.h"
#include "main.h"
#include "FreeRTOS.h"
#include "usart.h"

#define SENSOR_REPORT_TYPE       0x82
#define SENSOR_REPORT_FRAME_SIZE 17

static uint8_t SensorTxFrame[SENSOR_REPORT_FRAME_SIZE];

void StartSensorTask(void *argument)
{
    MPU6050_Init();

    for(;;)
    {
        MPU6050_Data sensorData;
        uint8_t checksum = SENSOR_REPORT_TYPE + (uint8_t)sizeof(sensorData);

        MPU6050_GetData(&sensorData.AX, &sensorData.AY, &sensorData.AZ,
                        &sensorData.GX, &sensorData.GY, &sensorData.GZ);

        SensorTxFrame[0] = 0xAA;
        SensorTxFrame[1] = 0x55;
        SensorTxFrame[2] = SENSOR_REPORT_TYPE;
        SensorTxFrame[3] = (uint8_t)sizeof(sensorData);
        memcpy(&SensorTxFrame[4], &sensorData, sizeof(sensorData));

        for (uint8_t i = 0; i < sizeof(sensorData); i++)
        {
            checksum += SensorTxFrame[4 + i];
        }
        SensorTxFrame[SENSOR_REPORT_FRAME_SIZE - 1] = checksum;

        if (osMutexAcquire(BleTxMutexHandle, osWaitForever) == osOK)
        {
            if (HAL_UART_Transmit_DMA(&huart2, SensorTxFrame, SENSOR_REPORT_FRAME_SIZE) == HAL_OK)
            {
                (void)osSemaphoreAcquire(TxBleSemHandle, pdMS_TO_TICKS(500));
            }
            osMutexRelease(BleTxMutexHandle);
        }

        osDelay(200);
    }
}
