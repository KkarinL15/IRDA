/**
 * @file    infrared.c
 * @author  
 * @brief   infrared functions.
 * @note    适用26R1R2通信，注意串口初始化，波特率19200
 * @version 1.2
 * @date    2026-03-28
 */

#include "infrared.h"
#include "./usart_ex/usart_ex.h"
#include "./core_delay/core_delay.h"

unsigned char device_addr       = 0XA1;//器件地址
unsigned char END               = 0xFE;//结束标志
unsigned char Infrared_emission = 0XF1;//红外发射状态
unsigned char modified_addr     = 0XF2;//设备地址
unsigned char modified_baud     = 0XF3;//修改波特率
uint8_t rec_buf[1] = {0};        //接收反馈信息
uint8_t rxdata_buf[100]={0};     //接收数据缓冲区
uint8_t rxdata[3]={0};           //接收的数据

/**
 * @brief 串口红外发送消息
 *
 * @param huart    串口句柄
 * @param id       指令发送方
 * @param data     要发送的数据
 */
void Infrared_send(UART_HandleTypeDef *huart,com_id_t id,uint8_t data)
{     
    uint8_t send_data[5] = {0};
    send_data[0] = device_addr;
    send_data[1] = Infrared_emission;

    switch (id)
    {
    case FROM_R1:
        send_data[2] = FROM_R1;
        break;
    case FROM_R2:
        send_data[2] = FROM_R2;
        break;
    default:
        break;
    }

    send_data[3] = data;
    send_data[4] = END;

    HAL_UART_Transmit(huart, (uint8_t *)send_data, 5, HAL_MAX_DELAY);
    uart_dmarx_read(huart, rec_buf, sizeof(rec_buf));

    rec_buf[0] = 0;
}

/**
 * @brief 串口红外接收消息
 *
 * @param huart       串口句柄
 * @param id          指令发送方
 * @return 接收的有效数据
 */
uint8_t Infrared_receive(UART_HandleTypeDef *huart,com_id_t id){

    static int j;
    int res = 0;
    uart_dmarx_read(huart, rxdata_buf, sizeof(rxdata_buf));

    //提取有效数据
    for (int i = 0; i < sizeof(rxdata_buf); i++)
    {
        if (rxdata_buf[i] == SEND_OK)
        {
            continue;
        }else{
            rxdata[j] = rxdata_buf[i];
            j++;
            if (j > 2)
            {
                j = 0;
            }
            if (rxdata[0] == id && rxdata[2] == END)
            {
                res = rxdata[1];
                break;
            }
        }
    }
    return res;
}