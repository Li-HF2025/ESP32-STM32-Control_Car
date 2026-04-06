#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f1xx_hal.h"
#include "TCA9555.h"
void TB6612_Init(void);
void Motor_Set(int m1_speed, int m2_speed, int m3_speed, int m4_speed);
void Motor_Stop(void);
#endif /* __MOTOR_H */