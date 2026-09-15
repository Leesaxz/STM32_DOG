#include "stm32f1xx_hal.h"
#include "MPU6050.h"
#include "MPU6050_Reg.h"

static MyIIC_Handle MPU6050_IIC_Handle;

void MPU6050_Init(void)
{
    MPU6050_IIC_Handle.W_SCL = MPU6050_SetSCL;
    MPU6050_IIC_Handle.W_SDA = MPU6050_SetSDA;
    MPU6050_IIC_Handle.R_SDA = MPU6050_GetSDA;
    MPU6050_W_SCL(1);
    MPU6050_W_SDA(1);

    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

void MPU6050_Start(void)
{
    MyIIC_Start(&MPU6050_IIC_Handle);
}

void MPU6050_Stop(void)
{
    MyIIC_Stop(&MPU6050_IIC_Handle);
}

void MPU6050_SendByte(uint8_t Byte)
{
    MyIIC_SendByte(&MPU6050_IIC_Handle,Byte);
}

uint8_t MPU6050_ReceiveByte(void)
{
    return  (MyIIC_ReceiveByte(&MPU6050_IIC_Handle));
}

void MPU6050_SendAck(uint8_t AckBit)
{
    MPU6050_W_SDA(AckBit);
    MPU6050_W_SCL(1);
    MPU6050_W_SCL(0);
}


uint8_t MPU6050_ReceiveAck(void)
{
    uint8_t AckBit;
    MPU6050_W_SDA(1);
    MPU6050_W_SCL(1);
    AckBit = MPU6050_R_SDA();
    MPU6050_W_SCL(0);
    return AckBit;
}

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    MPU6050_Start();
    MPU6050_SendByte(MPU6050_ADDRESS);
    MPU6050_ReceiveAck();
    MPU6050_SendByte(RegAddress);
    MPU6050_ReceiveAck();
    MPU6050_SendByte(Data);
    MPU6050_ReceiveAck();
    MPU6050_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;

    MPU6050_Start();
    MPU6050_SendByte(MPU6050_ADDRESS);
    MPU6050_ReceiveAck();
    MPU6050_SendByte(RegAddress);
    MPU6050_ReceiveAck();


    MPU6050_Start();
    MPU6050_SendByte(MPU6050_ADDRESS | 0X01);
    MPU6050_ReceiveAck();
    Data = MPU6050_ReceiveByte();
    MPU6050_SendAck(1);
    MPU6050_Stop();

    return Data;
}

void MPU6050_GetData(int16_t *Accx, int16_t *Accy, int16_t *Accz,
                     int16_t *Gyrox, int16_t *Gyroy, int16_t *Gyroz)
{
    uint8_t Data_H,Data_L;

    Data_H = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    Data_L = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
    *Accx = (Data_H << 8) | Data_L;

    Data_H = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    Data_L = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
    *Accy = (Data_H << 8) | Data_L;

    Data_H = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
    Data_L = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
    *Accz = (Data_H << 8) | Data_L;

    Data_H = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    Data_L = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
    *Gyrox = (Data_H << 8) | Data_L;

    Data_H = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    Data_L = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
    *Gyroy = (Data_H << 8) | Data_L;

    Data_H = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    Data_L = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
    *Gyroz = (Data_H << 8) | Data_L;

}