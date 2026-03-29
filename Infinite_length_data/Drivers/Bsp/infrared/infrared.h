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

#include <CSP_Config.h>

/*****************************************************************************
 * @defgroup INFRARED Public Marco. 
 * @{
 */

#define SEND_OK         0xF1

/**
 * @}
 */

/* clang-format off */

void Infrared_send(UART_HandleTypeDef *huart,uint8_t* data, uint8_t len);
void Infrared_receive(UART_HandleTypeDef *huart,uint8_t* data,uint8_t len);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INFRARED_H */