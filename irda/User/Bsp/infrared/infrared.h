/**
 * @file    infrared.h
 * @author  
 * @brief   Infrared function.
 * @version 1.0
 * @date    2026-03-12
 */

#ifndef __INFRARED_H
#define __INFRARED_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cubemx.h>

/*****************************************************************************
 * @defgroup INFRARED Public Marco. 
 * @{
 */

#define SEND_OK         0xF1  //反馈消息
#define RES_OK          0xFD  //应答信号
#define RES             0xFF  //可以应答标志
#define CON_SEND        0xFE  //继续发送标志
/**
 * @brief 指令发送方
 */
typedef enum {
    FROM_R1 = 0xE1,
    FROM_R2,
} com_id_t;

/**
 * @}
 */

/* clang-format off */

void Infrared_send(UART_HandleTypeDef *huart,com_id_t id,uint8_t data);
uint8_t Infrared_receive(UART_HandleTypeDef *huart,com_id_t id);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INFRARED_H */