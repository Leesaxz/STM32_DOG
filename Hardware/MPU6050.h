#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f1xx_hal.h"
#include "MPU6050.h"
#include "MyIIC.h"

static MyIIC_Handle MPU6050_IIC_Handle;

#define MPU6050_W_SCL(x)        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, (GPIO_PinState)(x))
#define MPU6050_W_SDA(x)        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, (GPIO_PinState)(x))
#define MPU6050_R_SDA()         HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)

static inline void MPU6050_SetSCL(uint8_t level) { MPU6050_W_SCL(level); }
static inline void MPU6050_SetSDA(uint8_t level) { MPU6050_W_SDA(level); }
static inline uint8_t MPU6050_GetSDA(void)       { return MPU6050_R_SDA(); }

#define MPU6050_ADDRESS     0xD0

void MPU6050_Init(void);
void MPU6050_Start(void);
void MPU6050_Stop(void);
void MPU6050_SendByte(uint8_t Byte);
uint8_t MPU6050_ReceiveByte(void);
void MPU6050_SendAck(uint8_t AckBit);
uint8_t MPU6050_ReceiveAck(void);
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
void MPU6050_GetData(int16_t *Accx, int16_t *Accy, int16_t *Accz,
                     int16_t *Gyrox, int16_t *Gyroy, int16_t *Gyroz);

#endif

