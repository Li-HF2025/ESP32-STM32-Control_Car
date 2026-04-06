#include "Motor.h"
#define PWM_MAX 49

//PWM的输出引脚
#define PWM_L_GPIO_Port GPIOA
#define PWM_L1_Pin GPIO_PIN_0
#define PWM_L2_Pin GPIO_PIN_3

#define PWM_R_GPIO_Port GPIOB
#define PWM_R1_Pin GPIO_PIN_6
#define PWM_R2_Pin GPIO_PIN_7

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

static inline uint16_t clamp_pwm(int v) {
    if (v < 0) v = -v;
    if (v > 1000) v = 1000;
    // 把 0..1000 映射到 0..PWM_MAX
    return (uint16_t)((v * PWM_MAX) / 1000);
}
// 根据速度设置电机方向和PWM占空比
static void motor_dir(GPIO_TypeDef* in1_port, uint16_t in1_pin,
                      GPIO_TypeDef* in2_port, uint16_t in2_pin,
                      int speed)
{
    if (speed > 0) {
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
    } else if (speed < 0) {
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_SET);
    } else {
        // 滑行停止（coast）
        HAL_GPIO_WritePin(in1_port, in1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port, in2_pin, GPIO_PIN_RESET);
    }
}

void TB6612_Init(void)
{
    // // 1) 先禁用驱动（避免上电乱转）
    // HAL_GPIO_WritePin(STBY1_GPIO_Port, STBY1_Pin, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(STBY2_GPIO_Port, STBY2_Pin, GPIO_PIN_RESET);

    // // 2) 方向脚先置为停止
    // motor_dir(M1_IN1_Port, M1_IN1_Pin, M1_IN2_Port, M1_IN2_Pin, 0);
    // motor_dir(M2_IN1_Port, M2_IN1_Pin, M2_IN2_Port, M2_IN2_Pin, 0);
    // motor_dir(M3_IN1_Port, M3_IN1_Pin, M3_IN2_Port, M3_IN2_Pin, 0);
    // motor_dir(M4_IN1_Port, M4_IN1_Pin, M4_IN2_Port, M4_IN2_Pin, 0);

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
    // HAL_GPIO_WritePin(STBY1_GPIO_Port, STBY1_Pin, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(STBY2_GPIO_Port, STBY2_Pin, GPIO_PIN_SET);
}
void Motor_Set(int m1, int m2, int m3, int m4)
{
    // 方向
    // motor_dir(M1_IN1_Port, M1_IN1_Pin, M1_IN2_Port, M1_IN2_Pin, m1);
    // motor_dir(M2_IN1_Port, M2_IN1_Pin, M2_IN2_Port, M2_IN2_Pin, m2);
    // motor_dir(M3_IN1_Port, M3_IN1_Pin, M3_IN2_Port, M3_IN2_Pin, m3);
    // motor_dir(M4_IN1_Port, M4_IN1_Pin, M4_IN2_Port, M4_IN2_Pin, m4);

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