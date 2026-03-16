#include "amr_sensor.h"

void DWT_Delay_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void DWT_Delay_us(uint32_t us)
{
    uint32_t cycles = us * (SystemCoreClock / 1000000U);
    uint32_t start = DWT->CYCCNT;

    while ((DWT->CYCCNT - start) < cycles) { }
}

uint8_t ReadLineSensor(GPIO_TypeDef* port, uint16_t pin)
{
    const int N = 10;
    int high_count = 0;
    int i;

    for (i = 0; i < N; i++)
    {
        if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET)
            high_count++;
    }

    return (high_count > (N / 2)) ? 1 : 0;
}

uint8_t IsLeftBlack(void)
{
    return ReadLineSensor(LS_LEFT_GPIO_Port, LS_LEFT_Pin);
}

uint8_t IsRightBlack(void)
{
    return ReadLineSensor(LS_RIGHT_GPIO_Port, LS_RIGHT_Pin);
}

float ReadUltrasonicCm(GPIO_TypeDef* trigPort, uint16_t trigPin,
                       GPIO_TypeDef* echoPort, uint16_t echoPin)
{
    uint32_t start_tick;
    uint32_t pulse_start;
    uint32_t pulse_end;
    uint32_t timeout_us = 30000;
    float distance_cm;

    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_RESET);
    DWT_Delay_us(2);
    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_SET);
    DWT_Delay_us(10);
    HAL_GPIO_WritePin(trigPort, trigPin, GPIO_PIN_RESET);

    start_tick = DWT->CYCCNT;
    while (HAL_GPIO_ReadPin(echoPort, echoPin) == GPIO_PIN_RESET)
    {
        if (((DWT->CYCCNT - start_tick) / (SystemCoreClock / 1000000U)) > timeout_us)
            return 0.0f;
    }

    pulse_start = DWT->CYCCNT;
    while (HAL_GPIO_ReadPin(echoPort, echoPin) == GPIO_PIN_SET)
    {
        if (((DWT->CYCCNT - pulse_start) / (SystemCoreClock / 1000000U)) > timeout_us)
            return 0.0f;
    }

    pulse_end = DWT->CYCCNT;

    distance_cm =
        ((pulse_end - pulse_start) / (float)(SystemCoreClock / 1000000U)) * 0.0343f / 2.0f;

    return distance_cm;
}