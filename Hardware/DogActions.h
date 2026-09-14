#ifndef __DOGACTIONS_H
#define __DOGACTIONS_H

#include "main.h"
#include "tim.h"

// ============================================================
// 角度定义（只有45°变化范围）
// ============================================================
#define ANGLE_45    50    // 前伸位置
#define ANGLE_90    75    // 站立位置
#define ANGLE_135   100   // 后蹬位置

// 左腿：角度直接映射
#define L_FRONT     ANGLE_45    // 前伸
#define L_MID       ANGLE_90    // 站立
#define L_BACK      ANGLE_135   // 后蹬

// 右腿：角度反向映射
#define R_FRONT     ANGLE_135   // 前伸（反向）
#define R_MID       ANGLE_90    // 站立
#define R_BACK      ANGLE_45    // 后蹬（反向）

// ============================================================
// 动作模式
// ============================================================
#define DOG_STOP    0
#define DOG_STAND   1
#define DOG_SIT     2
#define DOG_DOWN    3
#define DOG_FORWARD 4
#define DOG_BACK    5
#define DOG_LEFT    6
#define DOG_RIGHT   7

// ============================================================
// 外部控制变量
// ============================================================
extern uint8_t Dog_Mode;         // 当前动作模式
extern uint8_t Dog_Continuous;   // 连续模式 (1=连续, 0=单次)
extern uint16_t Dog_Speed;       // 速度延迟(ms)
extern uint16_t Dog_Repeat;      // 重复次数

// ============================================================
// API函数
// ============================================================
void Dog_Init(void);
void Dog_Stand(void);
void Dog_Sit(void);
void Dog_Down(void);
void Dog_Forward(void);
void Dog_Backward(void);
void Dog_TurnLeft(void);
void Dog_TurnRight(void);
void Dog_Update(void);    // 放在主循环/任务中调用

#endif