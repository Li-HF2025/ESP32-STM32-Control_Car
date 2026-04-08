#include "LineTrack.h"

#define GPIO_Port1 GPIOA
#define X1_Pin GPIO_PIN_7
#define X2_Pin GPIO_PIN_6
#define X3_Pin GPIO_PIN_5
#define X4_Pin GPIO_PIN_4
#define GPIO_Port2 GPIOB
#define X5_Pin GPIO_PIN_12
#define X6_Pin GPIO_PIN_13
#define X7_Pin GPIO_PIN_14
#define X8_Pin GPIO_PIN_15

uint8_t LineTrack_Read()
{
    uint8_t result = 0;
    result |= (HAL_GPIO_ReadPin(GPIO_Port1, X1_Pin) == GPIO_PIN_SET) ? 0x01 : 0x00;
    result |= (HAL_GPIO_ReadPin(GPIO_Port1, X2_Pin) == GPIO_PIN_SET) ? 0x02 : 0x00;
    result |= (HAL_GPIO_ReadPin(GPIO_Port1, X3_Pin) == GPIO_PIN_SET) ? 0x04 : 0x00;
    result |= (HAL_GPIO_ReadPin(GPIO_Port1, X4_Pin) == GPIO_PIN_SET) ? 0x08 : 0x00;
    result |= (HAL_GPIO_ReadPin(GPIO_Port2, X5_Pin) == GPIO_PIN_SET) ? 0x10 : 0x00;
    result |= (HAL_GPIO_ReadPin(GPIO_Port2, X6_Pin) == GPIO_PIN_SET) ? 0x20 : 0x00;
    result |= (HAL_GPIO_ReadPin(GPIO_Port2, X7_Pin) == GPIO_PIN_SET) ? 0x40 : 0x00;
    result |= (HAL_GPIO_ReadPin(GPIO_Port2, X8_Pin) == GPIO_PIN_SET) ? 0x80 : 0x00;
    return result;
}