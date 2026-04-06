#include "Motor.h"
#define PWM_MAX 49

//PWM的输出引脚
#define PWM_L_GPIO_Port GPIOA
#define PWM_L1_Pin GPIO_PIN_0
#define PWM_L2_Pin GPIO_PIN_3

#define PWM_R_GPIO_Port GPIOB
#define PWM_R1_Pin GPIO_PIN_6
#define PWM_R2_Pin GPIO_PIN_7

// 电机方向控制引脚（通过 TCA9555 控制）

#define STBY1_Pin 0
#define M1_IN2_Pin 1 // P0_1
#define M2_IN1_Pin 2 // P0_2
#define M2_IN2_Pin 3 // P0_3
#define M3_IN1_Pin 4 // P0_4
#define STBY2_Pin 8
#define M3_IN2_Pin 9 // P0_11
#define M4_IN1_Pin 10 // P0_12
#define M4_IN2_Pin 11 // P0_13
#define M1_IN1_Pin 12 // P0_14

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

static inline uint16_t clamp_pwm(int v) {
    if (v < 0) v = -v;
    if (v > 1000) v = 1000;
    // 把 0..1000 映射到 0..PWM_MAX
    return (uint16_t)((v * PWM_MAX) / 1000);
}
// 根据速度设置电机方向和PWM占空比
static void motor_dir( uint8_t in1_pin,uint8_t in2_pin,int speed)
{
    if (speed > 0) {
        TCA9555_SetPin(in1_pin, true);
        TCA9555_SetPin(in2_pin, false);
    } else if (speed < 0) {
        TCA9555_SetPin(in1_pin, false);
        TCA9555_SetPin(in2_pin, true);
    } else {
        // 滑行停止（coast）
        TCA9555_SetPin(in1_pin, false);
        TCA9555_SetPin(in2_pin, false);
    }
}

void TB6612_Init(void)
{
    // 1) 初始化 TCA9555，设置所有引脚为输出并拉低
    TCA9555_Init_AllOutput_Low();

    // 2) 根据电路连接，设置初始状态（比如让所有电机都处于停止状态）
    TCA9555_SetPin(STBY1_Pin, false); // 先让驱动处于待机状态
    TCA9555_SetPin(STBY2_Pin, false);

    // 3) PWM 占空比清零
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); // M1
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0); // M2
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0); // M3
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 0); // M4

    // 4) 启动 PWM
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

    // 5) 使能驱动
    TCA9555_SetPin(STBY1_Pin, true);
    TCA9555_SetPin(STBY2_Pin, true);
}
void Motor_Set(int m1, int m2, int m3, int m4)
{
    // 方向
    motor_dir(M1_IN2_Pin, M2_IN1_Pin, -m1);
    motor_dir(M2_IN2_Pin, M3_IN1_Pin, -m2);
    motor_dir(M3_IN2_Pin, M4_IN1_Pin, -m3);
    motor_dir(M4_IN2_Pin, M1_IN1_Pin, -m4);

    // PWM
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, clamp_pwm(m1));
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, clamp_pwm(m2));
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, clamp_pwm(m3));
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, clamp_pwm(m4));
}

void Motor_Stop(void)
{
    Motor_Set(0, 0, 0, 0);
}