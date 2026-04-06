#ifndef __TCA9555_H
#define __TCA9555_H
#include "stm32f1xx_hal.h"
#include <stdbool.h>
HAL_StatusTypeDef TCA9555_Init_AllOutput_Low();
HAL_StatusTypeDef TCA9555_SetPin(uint8_t pin, bool level);
#endif /* __TCA9555_H */