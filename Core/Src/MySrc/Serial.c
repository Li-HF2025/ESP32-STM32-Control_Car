#include "Serial.h"

uint8_t rx_byte=0; // 用于接收中断的单字节缓冲区
extern QueueHandle_t serial_rx_queueBit;
extern UART_HandleTypeDef huart1;
void Serial_Init(){
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}
void Serial_SendString(const char* str){
    Serial_SendMsg(0x01, (uint8_t*)str, strlen(str));
}

void Serial_SendNum(uint32_t num){
    char buffer[12]; // 足够存储-2147483648到2147483647
    snprintf(buffer, sizeof(buffer), "%ld", num);
    Serial_SendMsg(0x02, (uint8_t*)buffer, strlen(buffer));
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // 只处理 USART1
    if(huart->Instance == USART1)
    {
        // 【最重要】立即重新开启下一次接收中断，防止丢数据
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
        // 将收到的字节发送到队列（ISR 安全版本）
        if((serial_rx_queueBit != NULL) &&
           (xQueueSendFromISR(serial_rx_queueBit, &rx_byte, &xHigherPriorityTaskWoken) != pdPASS))
        {
            // 队列已满，数据丢失（可在这里添加丢包统计，不写也不影响运行）
        }
        // 【FreeRTOS 强制】上下文切换
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}