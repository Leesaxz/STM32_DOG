#ifndef STM32_DOG_MYIIC_H
#define STM32_DOG_MYIIC_H

#include <stdint.h>

typedef struct
{
    void (*W_SDA)(uint8_t);
    void (*W_SCL)(uint8_t);
    uint8_t (*R_SDA)(void);

}MyIIC_Handle;

void MyIIC_Start(MyIIC_Handle *hi2c);
void MyIIC_Stop(MyIIC_Handle *hi2c);
void MyIIC_SendByte(MyIIC_Handle *hi2c,uint8_t Byte);
uint8_t MyIIC_ReceiveByte(MyIIC_Handle *hi2c);


#endif //STM32_DOG_MYIIC_H
