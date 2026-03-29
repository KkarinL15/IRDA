/**
 * @file    rtos_tasks.c
 * @author  Deadline039
 * @brief   RTOS tasks.
 * @version 1.0
 * @date    2024-01-31
 */

#include "includes.h"

static TaskHandle_t start_task_handle;
void start_task(void *pvParameters);

static TaskHandle_t message_polling_task_handle;
void message_polling_task(void *pvParameters);

static TaskHandle_t message_send_task_handle;
void message_send_task(void *pvParameters);

static TaskHandle_t task3_handle;
void task3(void *pvParameters);

uint8_t txdata={0};     //发送数据缓冲区
/*****************************************************************************/

/**
 * @brief FreeRTOS start up.
 *
 */
void freertos_start(void) {
    xTaskCreate(start_task, "start_task", 128, NULL, 2, &start_task_handle);
    vTaskStartScheduler();
}

/**
 * @brief Start up task.
 *
 * @param pvParameters Start parameters.
 */
void start_task(void *pvParameters) {
    UNUSED(pvParameters);

    message_register_send_uart(MSG_TO_MASTER, &huart2, 20);
    message_register_polling_uart(MSG_TO_MASTER, &huart2, 100, 512);
    taskENTER_CRITICAL();

    xTaskCreate(message_send_task, "message_send_task", 128, NULL, 3, &message_send_task_handle);
    xTaskCreate(message_polling_task, "message_polling_task", 512, NULL, 2, &message_polling_task_handle);
    xTaskCreate(task3, "task3", 128, NULL, 2, &task3_handle);


    vTaskDelete(start_task_handle);
    taskEXIT_CRITICAL();
}

/**
 * @brief message_polling_task
 *
 * @param pvParameters Start parameters.
 */
void message_polling_task(void *pvParameters) {
    UNUSED(pvParameters);

    while (1) {
        LED0_TOGGLE();

        // uint8_t txdata;
        switch (Infrared_receive(&huart7, FROM_R2))
        {
        case 0x03:
            // txdata = 0x06;
            Infrared_send(&huart7, FROM_R1, RES_OK);
            break;
        case 0x02:
            // txdata = 0x07;
            Infrared_send(&huart7, FROM_R1, RES_OK);
            break;
        case 0x01:
            // txdata = 0x08;
            Infrared_send(&huart7, FROM_R1, RES_OK);
            break;
        case RES_OK:
            break;
        default:
            break;
        }
        vTaskDelay(500);
    }
}

/**
 * @brief message_send_task: Send messages.
 *
 * @param pvParameters Start parameters.
 */
void message_send_task(void *pvParameters) {
    UNUSED(pvParameters);

    while (1) {
        txdata = 0x00;
        LED1_TOGGLE();
        // Infrared_send(&huart7, FROM_R1, txdata);
        vTaskDelay(500);
    }
}

/**
 * @brief Task3: Scan the key and print which key pressed.
 *
 * @param pvParameters Start parameters.
 */
void task3(void *pvParameters) {
    UNUSED(pvParameters);

    key_press_t key = KEY_NO_PRESS;

    while (1) {
        key = key_scan(0);
        switch (key) {
            case WKUP_PRESS: {
                printf("Wake Up Pressed. \n");
            } break;

            case KEY0_PRESS: {
                printf("KEY0 Pressed. \n");
            } break;

            case KEY1_PRESS: {
                printf("KEY1 Pressed. \n");
            } break;

            case KEY2_PRESS: {
                printf("KEY2 Pressed. \n");
            } break;

            default: {
            } break;
        }

        vTaskDelay(10);
    }
}

#ifdef configASSERT
/**
 * @brief FreeRTOS assert failed function. 
 * 
 * @param pcFile File name
 * @param ulLine File line
 */
void vAssertCalled(const char *pcFile, unsigned int ulLine) {
    fprintf(stderr, "FreeRTOS assert failed. File: %s, line: %u. \n", pcFile,
            ulLine);
}
#endif /* configASSERT */

#if configCHECK_FOR_STACK_OVERFLOW
/**
 * @brief The application stack overflow hook is called when a stack overflow is detected for a task.
 *
 * @param xTask the task that just exceeded its stack boundaries.
 * @param pcTaskName A character string containing the name of the offending task.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    UNUSED(xTask);
    fprintf(stderr, "Stack overflow! Taskname: %s. \n", pcTaskName);
}
#endif /* configCHECK_FOR_STACK_OVERFLOW */

#if configUSE_MALLOC_FAILED_HOOK
/**
 * @brief This hook function is called when allocation failed.
 * 
 */
void vApplicationMallocFailedHook(void) {
    fprintf(stderr, "FreeRTOS malloc failed! \n");
}
#endif /* configUSE_MALLOC_FAILED_HOOK */