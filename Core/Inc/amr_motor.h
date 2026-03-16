#ifndef __AMR_MOTOR_H__
#define __AMR_MOTOR_H__

#include "amr_app.h"

uint32_t PercentToDuty(float duty, uint32_t period);

void SetMotorFL(float speed_percent);
void SetMotorFR(float speed_percent);
void SetMotorRL(float speed_percent);
void SetMotorRR(float speed_percent);
void StopAllMotors(void);

#endif