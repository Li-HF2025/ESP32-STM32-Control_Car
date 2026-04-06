#ifndef __SERIAL_H
#define __SERIAL_H
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define SERIAL_MSG_MAX_LEN 64U

typedef struct
{
	char ucData[SERIAL_MSG_MAX_LEN];
} Message_t;

void Serial_Init(void);
void Serial_SendString(const char* str);
void Serial_SendNum(uint32_t num);
void Serial_RX_Task(void* params);

#endif /* __SERIAL_H */