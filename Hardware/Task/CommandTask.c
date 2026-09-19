#include "cmsis_os2.h"
#include "main.h"
#include "usart.h"
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



/* 蓝牙数据帧：帧格式 AA 55 类型 长度 数据 校验（sum=类型+长度+数据 */
#define ble_size 16
static uint8_t ble_state = 0;
static uint8_t ble_type,ble_len, ble_buf[ble_size], ble_cnt, ble_sum;

static void BLE_FeedByte(uint8_t Byte)
{
    switch (ble_state){

    case 0:   if (Byte == 0xAA) ble_state = 1; break;

    case 1:   ble_state = (Byte == 0x55) ? 2 : 0; break;

    case 2:   ble_type = Byte; ble_sum = Byte; ble_state = 3; break;

    case 3:   ble_len = Byte; ble_sum += Byte; ble_cnt = 0;
        ble_state = (ble_len <= 16) ? 4 : 0;   /* 长度防越界 */
        break;

    case 4:   ble_buf[ble_cnt++] = Byte; ble_sum += Byte;
        if (ble_cnt >= ble_len) ble_state = 5;
        break;

    case 5:   if (Byte == (uint8_t)ble_sum && ble_type == 0x01){
        uint8_t ble_cmd = ble_buf[0] + '0';        /* 动作命令直接入队 */
        osMessageQueuePut(action0QueueHandle, &ble_cmd, 0, 0);
    }
        ble_state = 0; break;
    default:  break;
    }
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        for (uint16_t i = 0; i < Size; i++) BLE_FeedByte(DMABLE_RXBUFF[i]);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2,DMABLE_RXBUFF,BLE_RXBUFF_SIZE);
    }

    else if (huart->Instance == USART3)
    {
        uint8_t voice_cmd = DMAVoice_RXBUFF[0];
        if (Size == 1 && voice_cmd >= '0' && voice_cmd <= '7')
        {
            osMessageQueuePut(action0QueueHandle, &voice_cmd, 0, 0);
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, DMAVoice_RXBUFF, Voice_RXBUFF_SIZE);
    }


}