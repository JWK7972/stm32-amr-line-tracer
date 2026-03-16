#ifndef __AMR_APP_H__
#define __AMR_APP_H__

#include <includes.h>
#include "main.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define TASK_STK_SIZE           512

#define START_TASK_PRIO         10
#define SENSOR_TASK_PRIO        11
#define CONTROL_TASK_PRIO       12
#define DRIVE_TASK_PRIO         13
#define UART_TASK_PRIO          14

#define AMR_FLAG_STOP           0x01
#define AMR_FLAG_SLOW           0x02
#define AMR_FLAG_BUZZER         0x04

#define FRONT_STOP_CM           5.0f
#define FRONT_SLOW_CM           20.0f
#define FRONT_WARN_CM           30.0f
#define FRONT_RELEASE_CM        12.0f
#define SIDE_WARN_CM            15.0f

#define SENSOR_PERIOD_MS        20
#define CONTROL_PERIOD_MS       10
#define DRIVE_PERIOD_MS         10
#define UART_PERIOD_MS          100

#define FWD_FL                  85.0f
#define FWD_FR                  85.0f
#define FWD_RL                 -15.0f
#define FWD_RR                 -15.0f

#define FL_FAST                 (FWD_FL)
#define FL_SLOW                 (FWD_FL * 0.2f)
#define FR_FAST                 (FWD_FR)
#define FR_SLOW                 (FWD_FR * 0.2f)
#define RL_FAST                 (FWD_RL)
#define RL_SLOW                 (FWD_RL * 0.2f)
#define RR_FAST                 (FWD_RR)
#define RR_SLOW                 (FWD_RR * 0.2f)

#define SHARP_TURN_COUNT        20

#define LS_LEFT_Pin             GPIO_PIN_12
#define LS_LEFT_GPIO_Port       GPIOB
#define LS_RIGHT_Pin            GPIO_PIN_10
#define LS_RIGHT_GPIO_Port      GPIOC

#define ULTRA_TRIG_Pin          GPIO_PIN_8
#define ULTRA_TRIG_GPIO_Port    GPIOB
#define ULTRA_ECHO_Pin          GPIO_PIN_9
#define ULTRA_ECHO_GPIO_Port    GPIOB

#define ULTRA_LEFT_ECHO_Pin        GPIO_PIN_13
#define ULTRA_LEFT_ECHO_GPIO_Port  GPIOB
#define ULTRA_LEFT_TRIG_Pin        GPIO_PIN_14
#define ULTRA_LEFT_TRIG_GPIO_Port  GPIOB

#define ULTRA_RIGHT_ECHO_Pin       GPIO_PIN_15
#define ULTRA_RIGHT_ECHO_GPIO_Port GPIOA
#define ULTRA_RIGHT_TRIG_Pin       GPIO_PIN_12
#define ULTRA_RIGHT_TRIG_GPIO_Port GPIOC

#define BUZZER_Pin              GPIO_PIN_15
#define BUZZER_GPIO_Port        GPIOB

typedef struct
{
    float frontCm;
    float leftCm;
    float rightCm;

    INT8U lineLeft;
    INT8U lineRight;

    float speedScale;
    INT8U obstacleMode;

    INT16U leftBlackCnt;
    INT16U rightBlackCnt;

    float cmdFL;
    float cmdFR;
    float cmdRL;
    float cmdRR;

    INT8U  buzzerEnable;
    INT32U buzzerPeriodMs;
    INT32U buzzerLastTick;
    INT8U  buzzerState;
} AMR_State;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

extern OS_EVENT    *DataSem;
extern OS_EVENT    *UartSem;
extern OS_FLAG_GRP *AmrFlags;

extern OS_STK StartTaskStk[TASK_STK_SIZE];
extern OS_STK SensorTaskStk[TASK_STK_SIZE];
extern OS_STK ControlTaskStk[TASK_STK_SIZE];
extern OS_STK DriveTaskStk[TASK_STK_SIZE];
extern OS_STK UartTaskStk[TASK_STK_SIZE];

extern volatile AMR_State gAmr;

void SystemClock_Config(void);
void Error_Handler(void);

#endif