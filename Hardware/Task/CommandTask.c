#include "cmsis_os2.h"
#include "main.h"
#include "usart.h"
#include "FreeRTOS.h"
#include <string.h>

static volatile uint16_t Bluetooth_Size;
static volatile uint16_t Voice_Size;

#define BLE_FRAME_ACTION       0x01
#define BLE_FRAME_WEATHER      0x03
#define BLE_FRAME_TIME         0x04
#define BLE_FRAME_MCU_REQUEST  0x81

#define BLE_DATA_MAX           160
#define BLE_RXBUFF_SIZE       192

#define BLE_REQUEST_WEATHER    0x08
#define BLE_REQUEST_TIME       0x09

static uint8_t DMABLE_RXBUFF[BLE_RXBUFF_SIZE];
static uint8_t ble_state = 0;
static uint8_t ble_type, ble_len, ble_buf[BLE_DATA_MAX], ble_cnt, ble_sum;


/* ASR串口缓冲区 */
#define Voice_RXBUFF_SIZE 32
static uint8_t DMAVoice_RXBUFF[Voice_RXBUFF_SIZE];


/* 蓝牙数据帧：AA 55 类型 长度 数据 校验（sum=类型+长度+数据） */
static uint8_t BLE_CalculateChecksum(uint8_t type, const uint8_t *data, uint8_t length)
{
    uint8_t checksum = type + length;

    for (uint8_t i = 0; i < length; i++)
    {
        checksum += data[i];
    }

    return checksum;
}

static void BLE_SendAppRequest(uint8_t request)
{
    uint8_t frame[6];

    frame[0] = 0xAA;
    frame[1] = 0x55;
    frame[2] = BLE_FRAME_MCU_REQUEST;
    frame[3] = 1;
    frame[4] = request;
    frame[5] = BLE_CalculateChecksum(BLE_FRAME_MCU_REQUEST, &frame[4], 1);

    if (osMutexAcquire(BleTxMutexHandle, osWaitForever) == osOK)
    {
        if (HAL_UART_Transmit_DMA(&huart2, frame, sizeof(frame)) == HAL_OK)
        {
            (void)osSemaphoreAcquire(TxBleSemHandle, pdMS_TO_TICKS(500));
        }
        osMutexRelease(BleTxMutexHandle);
    }
}

static void BLE_FeedByte(uint8_t Byte)
{
    switch (ble_state){

    case 0:   if (Byte == 0xAA) ble_state = 1; break;

    case 1:   ble_state = (Byte == 0x55) ? 2 : 0; break;

    case 2:   ble_type = Byte; ble_sum = Byte; ble_state = 3; break;

    case 3:   ble_len = Byte; ble_sum += Byte; ble_cnt = 0;
        ble_state = (ble_len > 0 && ble_len <= BLE_DATA_MAX) ? 4 : 0;   /* 长度防越界 */
        break;

    case 4:   ble_buf[ble_cnt++] = Byte; ble_sum += Byte;
        if (ble_cnt >= ble_len) ble_state = 5;
        break;

    case 5:
        if (Byte == (uint8_t)ble_sum)
        {
            if (ble_type == BLE_FRAME_ACTION && ble_len == 1 && ble_buf[0] <= 7)
            {
                uint8_t ble_cmd = ble_buf[0] + '0';
                (void)osMessageQueuePut(action0QueueHandle, &ble_cmd, 0, 0);
            }
            else if (ble_type == BLE_FRAME_WEATHER)
            {
                OLED_RequestContent(OLED_CONTENT_WEATHER, ble_buf, ble_len);
            }
            else if (ble_type == BLE_FRAME_TIME)
            {
                OLED_RequestContent(OLED_CONTENT_TIME, ble_buf, ble_len);
            }
        }
        ble_state = 0; break;
    default:  break;
    }
}



void StartBluetoothCommandTask(void *argument)
{
    /* USER CODE BEGIN StartCommandTask */

    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, DMABLE_RXBUFF, BLE_RXBUFF_SIZE);
    /* Infinite loop */
    for(;;)
    {
        if (osSemaphoreAcquire(RxBleSemHandle, osWaitForever) == osOK)
        {
            for (uint16_t i = 0; i < Bluetooth_Size; i++)
            {
                BLE_FeedByte(DMABLE_RXBUFF[i]);
            }
            HAL_UARTEx_ReceiveToIdle_DMA(&huart2, DMABLE_RXBUFF, BLE_RXBUFF_SIZE);
        }
        osDelay(10);
    }
    /* USER CODE END StartCommandTask */
}

void StartVoiceCommandTask(void *argument)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, DMAVoice_RXBUFF, Voice_RXBUFF_SIZE);
    for(;;)
    {
        if ((osSemaphoreAcquire(RxVoiceSemHandle, osWaitForever) == osOK))
        {
            for (uint16_t i = 0; i < Voice_Size; i++)
            {
                uint8_t voice_cmd = DMAVoice_RXBUFF[i];

                if (voice_cmd >= '0' && voice_cmd <= '7')
                {
                    (void)osMessageQueuePut(action0QueueHandle, &voice_cmd, 0, 0);
                }
                else if (voice_cmd == '8')
                {
                    BLE_SendAppRequest(BLE_REQUEST_WEATHER);
                }
                else if (voice_cmd == '9')
                {
                    BLE_SendAppRequest(BLE_REQUEST_TIME);
                }
            }
            HAL_UARTEx_ReceiveToIdle_DMA(&huart3, DMAVoice_RXBUFF, Voice_RXBUFF_SIZE);
        }

    }
}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)//蓝牙
    {
        Bluetooth_Size = Size;
        osSemaphoreRelease(RxBleSemHandle);
    }

    else if (huart->Instance == USART3)//ASR
    {
        Voice_Size = Size;
        osSemaphoreRelease(RxVoiceSemHandle);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        // 蓝牙 DMA 发送完毕，唤醒等待发送的任务
        osSemaphoreRelease(TxBleSemHandle);
    }
    else if (huart->Instance == USART3)
    {
        // Voice 文本 DMA 发送完毕
        osSemaphoreRelease(TxVoiceSemHandle);
    }
}

/* STM32F1 专用硬件错误回调（标准读 SR+DR 清标志位流程） */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    // STM32F1 清除 ORE/NE/FE/PE 硬件错误标志的标准方式：先读 SR，再读 DR
    __IO uint32_t tmpreg = huart->Instance->SR;
    tmpreg = huart->Instance->DR;
    (void)tmpreg;

    if (huart->Instance == USART2)
    {
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, DMABLE_RXBUFF, BLE_RXBUFF_SIZE);
    }
    else if (huart->Instance == USART3)
    {
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, DMAVoice_RXBUFF, Voice_RXBUFF_SIZE);
    }
}
