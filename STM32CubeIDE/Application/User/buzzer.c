/*
 * buzzer.c
 *
 *  Created on: Jun 24, 2025
 *      Author: Admin
 */
// buzzer.c
#include "buzzer.h"
#include "main.h"

typedef struct {
    uint16_t duration_ms;
} BuzzerCommand;

static osMessageQueueId_t buzzerQueueHandle;
static osThreadId_t buzzerTaskHandle;

static void StartBuzzerTask(void *argument)
{
    BuzzerCommand cmd;

    for (;;) {
        if (osMessageQueueGet(buzzerQueueHandle, &cmd, NULL, osWaitForever) == osOK) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // LOW = kêu
            HAL_Delay(cmd.duration_ms);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // HIGH = tắt
        }
    }
}

void Buzzer_Init(void)
{
    buzzerQueueHandle = osMessageQueueNew(8, sizeof(BuzzerCommand), NULL);

    const osThreadAttr_t buzzerTask_attributes = {
        .name = "buzzerTask",
        .priority = (osPriority_t) osPriorityLow,
        .stack_size = 128 * 4
    };
    buzzerTaskHandle = osThreadNew(StartBuzzerTask, NULL, &buzzerTask_attributes);
}

void playBeep(uint16_t duration_ms)
{
    BuzzerCommand cmd = { duration_ms };
    osMessageQueuePut(buzzerQueueHandle, &cmd, 0, 0);
}
