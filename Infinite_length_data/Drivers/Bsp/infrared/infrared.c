/**
 * @file    infrared.c
 * @author  
 * @brief   infrared functions.
 * @note    可直接使用CSP的串口初始化
 * @version 1.0
 * @date    2026-03-12
 */

#include "infrared.h"
#include "./core/core_delay.h"


unsigned char device_addr       = 0XA1;//器件地址
unsigned char Infrared_emission = 0XF1;//红外发射状态
unsigned char modified_addr     = 0XF2;//设备地址
unsigned char modified_baud     = 0XF3;//修改波特率
uint8_t end_data[3] = {0xEE, 0xEF, 0xF0};//结束标志
extern uint8_t rx_buf[20];
extern uint8_t rxdata[20];
/**
 * @brief 串口红外发送消息
 *
 * @param huart     串口句柄
 * @param txdata    要发送的数据
 * @param len       数据长度
 */
void Infrared_send(UART_HandleTypeDef *huart,uint8_t* data, uint8_t len)
{     
    uint8_t send_data[5] = {0};

    send_data[0] = device_addr;
    send_data[1] = Infrared_emission;   
    for (int i = 1; i <= len; i++){
        if (i%3 == 0){
            send_data[2] = data[i - 3];
            send_data[3] = data[i - 2];
            send_data[4] = data[i - 1];
            //确定是否发送成功
            while (rx_buf[0] != SEND_OK){
                HAL_UART_Transmit(huart, (uint8_t *)send_data, 5, HAL_MAX_DELAY);
                uart_dmarx_read(huart, rx_buf, sizeof(rx_buf));
            }
            rx_buf[0] = 0;
        }
        else
        {
            continue;
        }
        delay_ms(100);
    }
    //标志发送结束
    send_data[2] = end_data[0];
    send_data[3] = end_data[1];
    send_data[4] = end_data[2];
    while (rx_buf[0] != SEND_OK){
        HAL_UART_Transmit(huart, (uint8_t *)send_data, 5, HAL_MAX_DELAY);
        uart_dmarx_read(huart, rx_buf, sizeof(rx_buf));
    }
    rx_buf[0] = 0;
} 

/**
 * @brief 串口红外接收消息
 *
 * @param huart     串口句柄
 * @param data    接收到的数据
 * @param len       数据长度
 */
void Infrared_receive(UART_HandleTypeDef *huart,uint8_t* data,uint8_t len){
    int j = 0;
    uart_dmarx_read(huart, data, len);
    
    for (int i = 0; i < len; i++)
    {
        if (data[i] == 0xEE || data[i] == 0xEF || data[i] == 0xF0)
        {
            break;
        }
        else
        {
            rxdata[j] = data[i];
            j++;
        }
    }
} 