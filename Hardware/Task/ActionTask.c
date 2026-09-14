#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "main.h"
#include "DogActions.h"

void StartActionTask(void *argument)
{
    /* USER CODE BEGIN StartActionTask */
    Dog_Init();
    /* Infinite loop */
    for(;;)
    {
        Dog_Update();
        osDelay(1);
    }
}