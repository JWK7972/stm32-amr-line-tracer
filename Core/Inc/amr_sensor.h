#ifndef __AMR_SENSOR_H__
#define __AMR_SENSOR_H__

#include "amr_app.h"

void DWT_Delay_Init(void);
void DWT_Delay_us(uint32_t us);

uint8_t ReadLineSensor(GPIO_TypeDef* port, uint16_t pin);
uint8_t IsLeftBlack(void);
uint8_t IsRightBlack(void);

float ReadUltrasonicCm(GPIO_TypeDef* trigPort, uint16_t trigPin,
                       GPIO_TypeDef* echoPort, uint16_t echoPin);

#endif