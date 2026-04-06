#include "TCA9555.h"
#define TCA9555_ADDR_7BIT   0x20
#define TCA9555_ADDR        (TCA9555_ADDR_7BIT << 1)  // 0x40

#define REG_OUTPUT0         0x02
#define REG_OUTPUT1         0x03
#define REG_POL0            0x04
#define REG_POL1            0x05
#define REG_CONFIG0         0x06
#define REG_CONFIG1         0x07
#define REG_OUTPUT0         0x02
#define REG_OUTPUT1         0x03


static uint8_t tca_out0 = 0x00; // Port0 输出缓存
static uint8_t tca_out1 = 0x00; // Port1 输出缓存


extern I2C_HandleTypeDef hi2c2; // 根据实际使用的 I2C 句柄修改
static HAL_StatusTypeDef TCA9555_WriteReg(uint8_t reg, uint8_t val)
{
    return HAL_I2C_Mem_Write(&hi2c2, TCA9555_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 100);
}

HAL_StatusTypeDef TCA9555_Init_AllOutput_Low()
{
    HAL_StatusTypeDef st;

    // 可选：确认设备在线（能应答 ACK）
    st = HAL_I2C_IsDeviceReady(&hi2c2, TCA9555_ADDR, 3, 100);
    if (st != HAL_OK) return st;

    // 1) 关闭输入极性翻转（一般不需要）
    st = TCA9555_WriteReg(REG_POL0, 0x00);
    if (st != HAL_OK) return st;
    st = TCA9555_WriteReg(REG_POL1, 0x00);
    if (st != HAL_OK) return st;

    // 2) 先写输出寄存器为 0（默认全部低电平）
    st = TCA9555_WriteReg(REG_OUTPUT0, 0x00);
    if (st != HAL_OK) return st;
    st = TCA9555_WriteReg(REG_OUTPUT1, 0x00);
    if (st != HAL_OK) return st;

    // 3) 再把方向配置为输出（0=输出）
    st = TCA9555_WriteReg(REG_CONFIG0, 0x00);
    if (st != HAL_OK) return st;
    st = TCA9555_WriteReg(REG_CONFIG1, 0x00);
    if (st != HAL_OK) return st;

    return HAL_OK;
}

/**
 * @param pin: 0~7 => P0~P7, 8~15 => P10~P17
 * @param level: true=高电平, false=低电平
 */
HAL_StatusTypeDef TCA9555_SetPin(uint8_t pin, bool level)
{
    if (pin < 8)
    {
        uint8_t mask = (uint8_t)(1u << pin);
        if (level) tca_out0 |= mask;
        else       tca_out0 &= (uint8_t)~mask;
        return TCA9555_WriteReg(REG_OUTPUT0, tca_out0);
    }
    else if (pin < 16)
    {
        uint8_t bit  = (uint8_t)(pin - 8);
        uint8_t mask = (uint8_t)(1u << bit);
        if (level) tca_out1 |= mask;
        else       tca_out1 &= (uint8_t)~mask;
        return TCA9555_WriteReg(REG_OUTPUT1, tca_out1);
    }
    return HAL_ERROR;
}