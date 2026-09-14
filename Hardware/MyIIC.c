#include "stm32f1xx_hal.h"
#include "MyIIC.h"

void MyIIC_Start(MyIIC_Handle *hi2c)
{
    hi2c->W_SCL(1);
    hi2c->W_SDA(1);
    hi2c->W_SDA(0);
    hi2c->W_SCL(0);
}


void MyIIC_Stop(MyIIC_Handle *hi2c)
{
    hi2c->W_SDA(0);
    hi2c->W_SCL(1);
    hi2c->W_SDA(1);
}

void MyIIC_SendByte(MyIIC_Handle *hi2c,uint8_t Byte)
{
    for(uint8_t i=0;i<8;i++)
    {
        hi2c->W_SDA(!!(Byte & (0x80 >> i)));
        hi2c->W_SCL(1);
        hi2c->W_SCL(0);
    }
}

uint8_t MyIIC_ReceiveByte(MyIIC_Handle *hi2c)
{
    uint8_t i,Byte = 0x00;
    hi2c->W_SDA(1);
    for(i=0;i<8;i++)
    {
        hi2c->W_SCL(1);
        if (hi2c->R_SDA() ==1)
        {
            Byte = Byte | (0x80 >> i);
        }
        hi2c->W_SCL(0);
    }
    return Byte;
}