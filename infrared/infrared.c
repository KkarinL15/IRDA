/**
 * @file    infrared.c
 * @author  
 * @brief   infrared functions.
 * @note    使用串口初始化
 * @version 1.0
 * @date    2026-03-12
 */

#include "infrared.h"
#include "./usart_ex/usart_ex.h"
#include "./core_delay/core_delay.h"


unsigned char device_addr       = 0XA1;//器件地址
unsigned char Infrared_emission = 0XF1;//红外发射状态
unsigned char modified_addr     = 0XF2;//设备地址
unsigned char modified_baud     = 0XF3;//修改波特率
uint8_t rec_buf[1] = {0};   //接收反馈信息

/**
 * @brief 串口红外发送消息
 *
 * @param huart     串口句柄
 * @param data    要发送的数据
 * @param len       数据长度
 */
void Infrared_send(UART_HandleTypeDef *huart,uint8_t* data)
{     
    uint8_t send_data[5] = {0};
    send_data[0] = device_addr;
    send_data[1] = Infrared_emission;
    send_data[2] = data[0];
    send_data[3] = data[1];
    send_data[4] = data[2];
    HAL_UART_Transmit(huart, (uint8_t *)send_data, 5, HAL_MAX_DELAY);
    uart_dmarx_read(huart, rec_buf, sizeof(rec_buf));

    //检测是否发送成功
    while (rec_buf[0] != SEND_OK)
    {
        HAL_UART_Transmit(huart, (uint8_t *)send_data, 5, HAL_MAX_DELAY);
        uart_dmarx_read(huart, rec_buf, sizeof(rec_buf));
    } 
    rec_buf[0] = 0;
} 

/**
 * @brief 串口红外接收消息
 *
 * @param huart       串口句柄
 * @param data_buf    接收到的所有数据
 * @param len         数据长度
 * @param rx_data     有效数据
 */
void Infrared_receive(UART_HandleTypeDef *huart,uint8_t* data_buf,uint8_t len,uint8_t* rx_data){

    int j = 0;
    uart_dmarx_read(huart, data_buf, len);

    for (int i = 0; i < len; i++)
    {
        if (data_buf[i] == 0xF1)
        {
            continue;
        }else{
            rx_data[j] = data_buf[i];
            j++;
        }
        
    }

    // if (rx_data[1] == 0)
    // {
    //     /* code */
    // }
    

    // if (data_buf[0] == 0xF1)
} 