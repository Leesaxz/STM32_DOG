#include "cmsis_os2.h"
#include "main.h"
#include "OLED.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

#define OLED_FACE_WIDTH             128
#define OLED_FACE_HEIGHT            64
#define OLED_IDLE_START_DELAY_MS    15000
#define OLED_IDLE_SWITCH_MS         15000
#define OLED_CONTENT_TIMEOUT_MS     15000
#define OLED_CONTENT_LINE_LENGTH    17
#define OLED_CONTENT_LINE_COUNT     3

static char oled_content_lines[OLED_CONTENT_LINE_COUNT][OLED_CONTENT_LINE_LENGTH];
static volatile uint8_t oled_content_type = 0;
static volatile uint8_t oled_content_version = 0;
static volatile uint32_t oled_content_tick = 0;

const uint8_t * const IdleFaces[] = {
    BMP5,   // 特殊脸
    BMP7,   // 花痴脸
    BMP9,   // 开心脸
    BMP10,  // 调皮脸
    BMP11,  // 迷糊脸
    BMP12   // 猫猫脸
};

#define IDLE_FACE_COUNT (sizeof(IdleFaces) / sizeof(IdleFaces[0]))

static void OLED_CopyLine(char *destination, const char *source)
{
    uint8_t length = (uint8_t)strlen(source);

    if (length >= OLED_CONTENT_LINE_LENGTH)
    {
        length = OLED_CONTENT_LINE_LENGTH - 1;
    }

    memcpy(destination, source, length);
    destination[length] = '\0';
}

void OLED_RequestContent(uint8_t contentType, const uint8_t *text, uint16_t length)
{
    char lines[OLED_CONTENT_LINE_COUNT][OLED_CONTENT_LINE_LENGTH] = {{0}};
    uint8_t line = 0;
    uint8_t column = 0;

    if (text == NULL || length == 0)
    {
        return;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        uint8_t value = text[i];

        if (value == '\r')
        {
            continue;
        }

        if (value == '\n' || value == '|')
        {
            if (line < OLED_CONTENT_LINE_COUNT - 1)
            {
                line++;
                column = 0;
            }
            continue;
        }

        if (value < 32 || value > 126)
        {
            value = '?';
        }

        if (line < OLED_CONTENT_LINE_COUNT &&
            column < OLED_CONTENT_LINE_LENGTH - 1)
        {
            lines[line][column++] = (char)value;
        }
    }

    taskENTER_CRITICAL();
    memcpy(oled_content_lines, lines, sizeof(oled_content_lines));
    oled_content_type = contentType;
    oled_content_tick = osKernelGetTickCount();
    oled_content_version++;
    taskEXIT_CRITICAL();
}

static const uint8_t *OLED_GetActionFace(uint8_t mode)
{
    switch (mode)
    {
        case DOG_STOP:    return BMP9;   // 开心脸
        case DOG_STAND:   return BMP1;   // 立正脸
        case DOG_SIT:     return BMP6;   // 睡觉脸
        case DOG_DOWN:    return BMP8;   // 酣睡脸
        case DOG_FORWARD: return BMP2;   // 前进脸
        case DOG_BACK:    return BMP2;   // 后退也使用前进脸
        case DOG_LEFT:    return BMP3;   // 左转脸
        case DOG_RIGHT:   return BMP4;   // 右转脸
        default:          return BMP1;
    }
}

static uint8_t OLED_GetRandomIdleFace(uint32_t now, uint8_t last_index, uint32_t *random_state)
{
    uint8_t index;

    *random_state = (*random_state * 1664525U) + 1013904223U + now;
    index = (uint8_t)((*random_state >> 16) % IDLE_FACE_COUNT);

    if (IDLE_FACE_COUNT > 1 && index == last_index)
    {
        index = (uint8_t)((index + 1) % IDLE_FACE_COUNT);
    }

    return index;
}

static void OLED_ShowCenteredText(uint8_t line, const char *text)
{
    uint8_t length = (uint8_t)strlen(text);
    uint8_t column = 1;

    if (length < 16)
    {
        column = (uint8_t)((16 - length) / 2 + 1);
    }

    OLED_ShowString(line, column, (char *)text);
}

static void OLED_RenderContent(
        uint8_t contentType,
        char lines[OLED_CONTENT_LINE_COUNT][OLED_CONTENT_LINE_LENGTH])
{
    OLED_Clear();

    if (contentType == OLED_CONTENT_TIME)
    {
        char date[OLED_CONTENT_LINE_LENGTH] = {0};
        char clock[OLED_CONTENT_LINE_LENGTH] = {0};
        char *separator = strchr(lines[0], ' ');

        if (separator != NULL)
        {
            *separator = '\0';
            OLED_CopyLine(date, lines[0]);
            OLED_CopyLine(clock, separator + 1);
        }
        else
        {
            OLED_CopyLine(date, lines[0]);
        }

        if (clock[0] != '\0')
        {
            OLED_ShowString_Size(1, 1, clock, 2);
        }
        OLED_ShowCenteredText(3, date);
        OLED_ShowCenteredText(4, "PHONE TIME");
        return;
    }

    OLED_ShowCenteredText(1, lines[0]);
    OLED_ShowCenteredText(2, lines[1]);
    OLED_ShowCenteredText(3, lines[2]);
}

void StartOLEDTask(void *argument)
{
    uint8_t last_action_mode = 0xFF;
    uint8_t last_idle_face = 0xFF;
    uint8_t showing_idle_face = 0;
    uint8_t content_visible = 0;
    uint8_t rendered_content_version = 0;
    uint32_t last_face_tick = osKernelGetTickCount();
    uint32_t random_state = osKernelGetTickCount() ^ 0xA5A5A5A5U;

    OLED_Init();

    for(;;)
    {
        uint8_t action_mode = Dog_DisplayMode;
        uint32_t now = osKernelGetTickCount();
        uint8_t content_type;
        uint8_t content_version;
        uint32_t content_tick;
        uint8_t content_active;

        taskENTER_CRITICAL();
        content_type = oled_content_type;
        content_version = oled_content_version;
        content_tick = oled_content_tick;
        taskEXIT_CRITICAL();

        content_active = content_version != 0 &&
                         (now - content_tick) < OLED_CONTENT_TIMEOUT_MS &&
                         (int32_t)(content_tick - Dog_DisplayModeTick) >= 0;

        if (content_active)
        {
            if (!content_visible || content_version != rendered_content_version)
            {
                char lines[OLED_CONTENT_LINE_COUNT][OLED_CONTENT_LINE_LENGTH];

                taskENTER_CRITICAL();
                memcpy(lines, oled_content_lines, sizeof(lines));
                taskEXIT_CRITICAL();

                OLED_RenderContent(content_type, lines);
                rendered_content_version = content_version;
                content_visible = 1;
                showing_idle_face = 0;
                last_action_mode = 0xFF;
            }

            osDelay(100);
            continue;
        }

        if (content_visible)
        {
            content_visible = 0;
            showing_idle_face = 0;
            last_action_mode = 0xFF;
        }

        if ((now - Dog_DisplayModeTick) >= OLED_IDLE_START_DELAY_MS)
        {
            if (!showing_idle_face || (now - last_face_tick) >= OLED_IDLE_SWITCH_MS)
            {
                last_idle_face = OLED_GetRandomIdleFace(now, last_idle_face, &random_state);
                OLED_ShowBMP(0, 0, OLED_FACE_WIDTH, OLED_FACE_HEIGHT,IdleFaces[last_idle_face], 1);
                last_face_tick = now;
                showing_idle_face = 1;
            }
        }
        else if (!showing_idle_face || action_mode != last_action_mode)
        {
            OLED_ShowBMP(0, 0, OLED_FACE_WIDTH, OLED_FACE_HEIGHT,
                         OLED_GetActionFace(action_mode), 1);
            last_action_mode = action_mode;
            last_face_tick = now;
            showing_idle_face = 0;
        }

        osDelay(100);
    }
}
