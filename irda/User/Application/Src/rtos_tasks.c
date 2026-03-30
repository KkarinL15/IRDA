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

static TaskHandle_t Interaction_task_handle;
void Interaction_task(void *pvParameters);

static TaskHandle_t task3_handle;
void task3(void *pvParameters);

static QueueHandle_t irda_queue_handle;
static QueueHandle_t interaction_queue_handle;

uint8_t txbuf={0};     //发送数据缓冲区
uint8_t txdata={0};     //发送数据
uint8_t rxbuf;         //接收有效数据
uint8_t rxqueue;        //接收队列数据
uint8_t WAIT      = 0xFF;//可以应答标志
uint8_t CONTINUE  = 0xFE;//继续发送标志
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

    irda_queue_handle = xQueueCreate(IRDA_QUEUE_LENGTH, sizeof(IRDA_QUEUE_LENGTH));
    interaction_queue_handle = xQueueCreate(INTERACTION_QUEUE_LENGTH, sizeof(INTERACTION_QUEUE_LENGTH));

    xTaskCreate(task3, "task3", 128, NULL, 3, &task3_handle);
    xTaskCreate(message_polling_task, "message_polling_task", 512, NULL, 3, &message_polling_task_handle);
    xTaskCreate(Interaction_task, "Interaction_task", 128, NULL, 3, &Interaction_task_handle);

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

        rxbuf = Infrared_receive(&huart7, FROM_R2);

        if(rxbuf == 0){
            xQueueOverwrite(irda_queue_handle, &CONTINUE);
        }else if(rxbuf == RES_OK){
            xQueueOverwrite(irda_queue_handle, &rxbuf);
        }else{
            xQueueOverwrite(irda_queue_handle, &WAIT);
        }

        vTaskDelay(500);
    }
}

/**
 * @brief Interaction_task: Send messages.
 *
 * @param pvParameters Start parameters.
 */
void Interaction_task(void *pvParameters) {
    UNUSED(pvParameters);

    while (1) {
        LED1_TOGGLE();

        xQueueReceive(irda_queue_handle, &rxqueue, portMAX_DELAY);
        xQueuePeek(interaction_queue_handle, &txbuf, portMAX_DELAY);

        if (rxqueue == RES_OK){
            continue;
        }else if(rxqueue == WAIT){
            Infrared_send(&huart7, FROM_R1, RES_OK);
        }else if(rxqueue == CONTINUE){
            Infrared_send(&huart7, FROM_R1, txbuf);
        }

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

    while (1) {
        int i = 2;
        xQueueSend(interaction_queue_handle, &i, portMAX_DELAY);
        vTaskDelay(100);
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