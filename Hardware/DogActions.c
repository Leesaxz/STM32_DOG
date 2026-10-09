#include "DogActions.h"
#include "cmsis_os2.h"

// ============================================================
// 全局变量
// ============================================================
uint8_t Dog_Mode = DOG_STOP;
volatile uint8_t Dog_DisplayMode = DOG_STAND;
volatile uint32_t Dog_DisplayModeTick = 0;
#define  Dog_Continuous     0
#define  Dog_Speed          200
#define  Dog_Repeat         3

static uint16_t repeat_cnt = 0;

// ============================================================
// 内部辅助
// ============================================================
static inline void L1(uint16_t v) { __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, v); }
static inline void L2(uint16_t v) { __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, v); }
static inline void L3(uint16_t v) { __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, v); }
static inline void L4(uint16_t v) { __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, v); }

static void Dog_SetDisplayMode(uint8_t mode)
{
    Dog_DisplayMode = mode;
    Dog_DisplayModeTick = osKernelGetTickCount();
}

static void AllLegs(uint16_t left, uint16_t right)
{
    L1(left);
    L2(right);
    L3(left);
    L4(right);
}

// ============================================================
// 初始化
// ============================================================
void Dog_Init(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);


    Dog_Stand();
    osDelay(500);
    Dog_Mode = DOG_STAND;
    Dog_SetDisplayMode(DOG_STAND);
}

// ============================================================
// 基础动作（内部使用，不修改 Dog_Mode）
// ============================================================

void Dog_Stand(void)
{
    L1(L_MID);
    L2(R_MID);
    osDelay(60);
    L3(L_MID);
    L4(R_MID);
}

void Dog_Sit(void)
{
    L1(L_MID);
    L2(R_MID);
    osDelay(60);
    L3(L_FRONT);
    L4(R_FRONT);
}

void Dog_Down(void)
{
    AllLegs(L_FRONT, R_FRONT);
}

// ============================================================
// 前进
// ============================================================
void Dog_Forward(void)
{
    Dog_Mode = DOG_FORWARD;
    repeat_cnt = 0;

    while(Dog_Mode == DOG_FORWARD)
    {
        L2(R_FRONT);
        L3(L_FRONT);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_FORWARD) break;

        L1(L_BACK);
        L4(R_BACK);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_FORWARD) break;

        L2(R_MID);
        L3(L_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_FORWARD) break;
        L1(L_MID);
        L4(R_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_FORWARD) break;

        L1(L_FRONT);
        L4(R_FRONT);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_FORWARD) break;

        L2(R_BACK);
        L3(L_BACK);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_FORWARD) break;

        L1(L_MID);
        L4(R_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_FORWARD) break;
        L2(R_MID);
        L3(L_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_FORWARD) break;

        repeat_cnt++;
        if(!Dog_Continuous && repeat_cnt >= Dog_Repeat)
        {
            Dog_Stand();
            Dog_Mode = DOG_STAND;
            Dog_SetDisplayMode(DOG_STAND);
            break;
        }
        uint8_t new_mode;
        if (osMessageQueueGet(action0QueueHandle, &new_mode, 0, 0) == osOK)
        {
            Dog_Mode = new_mode;
            Dog_SetDisplayMode(new_mode);
            return;
        }
    }
}

// ============================================================
// 后退（前进反向）
// ============================================================
void Dog_Backward(void)
{
    Dog_Mode = DOG_BACK;
    repeat_cnt = 0;

    while(Dog_Mode == DOG_BACK)
    {
        // 右前+左后 后蹬（向后推力）
        L2(R_BACK);
        L3(L_BACK);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_BACK) break;

        // 左前+右后 前伸
        L1(L_FRONT);
        L4(R_FRONT);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_BACK) break;

        L2(R_MID);
        L3(L_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_BACK) break;

        L1(L_MID);
        L4(R_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_BACK) break;

        // 左前+右后 后蹬
        L1(L_BACK);
        L4(R_BACK);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_BACK) break;

        // 右前+左后 前伸
        L2(R_FRONT);
        L3(L_FRONT);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_BACK) break;

        L1(L_MID);
        L4(R_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_BACK) break;

        L2(R_MID);
        L3(L_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_BACK) break;

        repeat_cnt++;
        if(!Dog_Continuous && repeat_cnt >= Dog_Repeat)
        {
            Dog_Stand();
            Dog_Mode = DOG_STAND;
            Dog_SetDisplayMode(DOG_STAND);
            break;
        }
        uint8_t new_mode;
        if (osMessageQueueGet(action0QueueHandle, &new_mode, 0, 0) == osOK)
        {
            Dog_Mode = new_mode;
            Dog_SetDisplayMode(new_mode);
            return;
        }
    }
}

// ============================================================
// 左转
// ============================================================
void Dog_TurnLeft(void)
{
    Dog_Mode = DOG_LEFT;
    repeat_cnt = 0;

    while(Dog_Mode == DOG_LEFT)
    {
        // 左腿前伸，右腿后蹬 -> 向右推，车头左转
        L1(L_FRONT);
        L4(R_BACK);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_LEFT) break;

        L2(R_FRONT);
        L3(L_BACK);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_LEFT) break;

        AllLegs(L_MID, R_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_LEFT) break;

        repeat_cnt++;
        if(!Dog_Continuous && repeat_cnt >= Dog_Repeat)
        {
            Dog_Stand();
            Dog_Mode = DOG_STAND;
            Dog_SetDisplayMode(DOG_STAND);
            break;
        }
        uint8_t new_mode;
        if (osMessageQueueGet(action0QueueHandle, &new_mode, 0, 0) == osOK)
        {
            Dog_Mode = new_mode;
            Dog_SetDisplayMode(new_mode);
            return;
        }
    }
}

// ============================================================
// 右转
// ============================================================
void Dog_TurnRight(void)
{
    Dog_Mode = DOG_RIGHT;
    repeat_cnt = 0;

    while(Dog_Mode == DOG_RIGHT)
    {
        // 右腿前伸，左腿后蹬 -> 向左推，车头右转
        L2(R_FRONT);
        L3(L_BACK);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_RIGHT) break;

        L1(L_FRONT);
        L4(R_BACK);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_RIGHT) break;

        AllLegs(L_MID, R_MID);
        osDelay(Dog_Speed);
        if(Dog_Mode != DOG_RIGHT) break;

        repeat_cnt++;
        if(!Dog_Continuous && repeat_cnt >= Dog_Repeat)
        {
            Dog_Stand();
            Dog_Mode = DOG_STAND;
            Dog_SetDisplayMode(DOG_STAND);
            break;
        }
        uint8_t new_mode;
        if (osMessageQueueGet(action0QueueHandle, &new_mode, 0, 0) == osOK)
        {
            Dog_Mode = new_mode;
            Dog_SetDisplayMode(new_mode);
            return;
        }
    }
}

// ============================================================
// 动作更新（放在主循环或FreeRTOS任务中调用）
// ============================================================
void Dog_Update(void)
{
    uint8_t new_mode;
    if (osMessageQueueGet(action0QueueHandle, &new_mode, 0, 0) == osOK)
    {
        Dog_Mode = new_mode;
        Dog_SetDisplayMode(new_mode);
    }

    switch(Dog_Mode)
    {
        case DOG_STOP:
            Dog_Stand();
            Dog_Mode = DOG_STAND;
            break;
        case DOG_STAND:
            Dog_Stand();
            break;
        case DOG_SIT:
            Dog_Sit();
            break;
        case DOG_DOWN:
            Dog_Down();
            break;
        case DOG_FORWARD:
            Dog_Forward();
            break;
        case DOG_BACK:
            Dog_Backward();
            break;
        case DOG_LEFT:
            Dog_TurnLeft();
            break;
        case DOG_RIGHT:
            Dog_TurnRight();
            break;
        default:
            break;
    }
}
