#include "amr_motor.h"

uint32_t PercentToDuty(float duty, uint32_t period)
{
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 100.0f) duty = 100.0f;

    return (uint32_t)((duty * (period + 1U)) / 100.0f);
}

void SetMotorFL(float speed_percent)
{
    int8_t dir = (speed_percent >= 0.0f) ? +1 : -1;
    float duty = fabsf(speed_percent);

    if (duty > 100.0f) duty = 100.0f;
    if (duty > 0.0f && duty < 25.0f) duty = 25.0f;

    if (dir > 0)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    }

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PercentToDuty(duty, htim1.Init.Period));
}

void SetMotorFR(float speed_percent)
{
    int8_t dir = (speed_percent >= 0.0f) ? +1 : -1;
    float duty = fabsf(speed_percent);

    if (duty > 100.0f) duty = 100.0f;
    if (duty > 0.0f && duty < 25.0f) duty = 25.0f;

    if (dir > 0)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    }

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, PercentToDuty(duty, htim4.Init.Period));
}

void SetMotorRL(float speed_percent)
{
    float duty = fabsf(speed_percent);
    uint32_t pwm;

    if (duty > 100.0f) duty = 100.0f;
    pwm = PercentToDuty(duty, htim3.Init.Period);

    if (speed_percent > 0.0f)
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwm);
    }
    else if (speed_percent < 0.0f)
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    }
}

void SetMotorRR(float speed_percent)
{
    float duty = fabsf(speed_percent);
    uint32_t pwm;

    if (duty > 100.0f) duty = 100.0f;
    pwm = PercentToDuty(duty, htim2.Init.Period);

    if (speed_percent > 0.0f)
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pwm);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    }
    else if (speed_percent < 0.0f)
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwm);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    }
}

void StopAllMotors(void)
{
    SetMotorFL(0.0f);
    SetMotorFR(0.0f);
    SetMotorRL(0.0f);
    SetMotorRR(0.0f);
}