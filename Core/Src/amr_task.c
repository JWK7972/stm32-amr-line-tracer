#include "amr_task.h"
#include "amr_sensor.h"
#include "amr_motor.h"
#include "amr_control.h"

OS_EVENT    *DataSem  = 0;
OS_EVENT    *UartSem  = 0;
OS_FLAG_GRP *AmrFlags = 0;

OS_STK StartTaskStk[TASK_STK_SIZE];
OS_STK SensorTaskStk[TASK_STK_SIZE];
OS_STK ControlTaskStk[TASK_STK_SIZE];
OS_STK DriveTaskStk[TASK_STK_SIZE];
OS_STK UartTaskStk[TASK_STK_SIZE];

volatile AMR_State gAmr =
{
    0.0f, 0.0f, 0.0f,
    0, 0,
    1.0f, 0,
    0, 0,
    0.0f, 0.0f, 0.0f, 0.0f,
    0, 0, 0, 0
};

void StartTask(void *data)
{
    INT8U err;

    (void)data;

    SysTick_Configuration();
    OSStatInit();

    DataSem  = OSSemCreate(1);
    UartSem  = OSSemCreate(1);
    AmrFlags = OSFlagCreate(0x00, &err);

    OSTaskCreate(SensorTask,  (void *)0, (void *)&SensorTaskStk[TASK_STK_SIZE - 1], SENSOR_TASK_PRIO);
    OSTaskCreate(ControlTask, (void *)0, (void *)&ControlTaskStk[TASK_STK_SIZE - 1], CONTROL_TASK_PRIO);
    OSTaskCreate(DriveTask,   (void *)0, (void *)&DriveTaskStk[TASK_STK_SIZE - 1], DRIVE_TASK_PRIO);
    OSTaskCreate(UartTask,    (void *)0, (void *)&UartTaskStk[TASK_STK_SIZE - 1], UART_TASK_PRIO);

    for (;;)
    {
        OSCtxSwCtr = 0;
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

void SensorTask(void *data)
{
    INT8U err;

    (void)data;

    for (;;)
    {
        float front;
        float left;
        float right;
        INT8U leftBlack;
        INT8U rightBlack;

        leftBlack  = IsLeftBlack();
        rightBlack = IsRightBlack();

        front = ReadUltrasonicCm(ULTRA_TRIG_GPIO_Port, ULTRA_TRIG_Pin,
                                 ULTRA_ECHO_GPIO_Port, ULTRA_ECHO_Pin);

        left  = ReadUltrasonicCm(ULTRA_LEFT_TRIG_GPIO_Port, ULTRA_LEFT_TRIG_Pin,
                                 ULTRA_LEFT_ECHO_GPIO_Port, ULTRA_LEFT_ECHO_Pin);

        right = ReadUltrasonicCm(ULTRA_RIGHT_TRIG_GPIO_Port, ULTRA_RIGHT_TRIG_Pin,
                                 ULTRA_RIGHT_ECHO_GPIO_Port, ULTRA_RIGHT_ECHO_Pin);

        OSSemPend(DataSem, 0, &err);
        gAmr.frontCm   = front;
        gAmr.leftCm    = left;
        gAmr.rightCm   = right;
        gAmr.lineLeft  = leftBlack;
        gAmr.lineRight = rightBlack;
        OSSemPost(DataSem);

        OSTimeDlyHMSM(0, 0, 0, SENSOR_PERIOD_MS);
    }
}

void ControlTask(void *data)
{
    INT8U err = 0;

    (void)data;

    for (;;)
    {
        float front;
        float left;
        float right;
        INT8U leftBlack;
        INT8U rightBlack;
        INT8U mode;
        float speedScale;
        INT8U buzzerEnable;
        INT32U buzzerPeriod;
        float fl = 0.0f;
        float fr = 0.0f;
        float rl = 0.0f;
        float rr = 0.0f;
        INT16U leftCnt;
        INT16U rightCnt;

        OSSemPend(DataSem, 0, &err);
        front      = gAmr.frontCm;
        left       = gAmr.leftCm;
        right      = gAmr.rightCm;
        leftBlack  = gAmr.lineLeft;
        rightBlack = gAmr.lineRight;
        mode       = gAmr.obstacleMode;
        leftCnt    = gAmr.leftBlackCnt;
        rightCnt   = gAmr.rightBlackCnt;
        OSSemPost(DataSem);

        if (front <= 0.0f || front > FRONT_WARN_CM)
        {
            mode = 0;
        }
        else
        {
            if (mode == 2)
            {
                if (front > FRONT_RELEASE_CM)
                {
                    if (front <= FRONT_STOP_CM) mode = 2;
                    else if (front <= FRONT_SLOW_CM) mode = 1;
                    else mode = 0;
                }
                else
                {
                    mode = 2;
                }
            }
            else
            {
                if (front <= FRONT_STOP_CM) mode = 2;
                else if (front <= FRONT_SLOW_CM) mode = 1;
                else mode = 0;
            }
        }

        if (front > 0.0f && front <= FRONT_STOP_CM &&
            left  > 0.0f && left  <= SIDE_WARN_CM &&
            right > 0.0f && right <= SIDE_WARN_CM)
        {
            mode = 2;
        }

        if (mode == 0) speedScale = 1.0f;
        else if (mode == 1) speedScale = 0.7f;
        else speedScale = 0.0f;

        buzzerEnable = 0;
        buzzerPeriod = 0;

        if ((left > 0.0f && left <= SIDE_WARN_CM) ||
            (right > 0.0f && right <= SIDE_WARN_CM))
        {
            buzzerEnable = 1;
            buzzerPeriod = 300;
        }

        if (front > 0.0f && front <= FRONT_WARN_CM)
        {
            INT32U p;

            if (front <= FRONT_STOP_CM) p = 80;
            else if (front <= FRONT_SLOW_CM) p = 150;
            else p = 300;

            buzzerEnable = 1;
            if (buzzerPeriod == 0 || p < buzzerPeriod)
                buzzerPeriod = p;
        }

        if (leftBlack) leftCnt++;
        else leftCnt = 0;

        if (rightBlack) rightCnt++;
        else rightCnt = 0;

        if (mode == 2)
        {
            fl = 0.0f;
            fr = 0.0f;
            rl = 0.0f;
            rr = 0.0f;
        }
        else
        {
            DecideLineTrace(leftBlack, rightBlack, leftCnt, rightCnt, &fl, &fr, &rl, &rr);
        }

        OSSemPend(DataSem, 0, &err);
        gAmr.obstacleMode   = mode;
        gAmr.speedScale     = speedScale;
        gAmr.buzzerEnable   = buzzerEnable;
        gAmr.buzzerPeriodMs = buzzerPeriod;
        gAmr.leftBlackCnt   = leftCnt;
        gAmr.rightBlackCnt  = rightCnt;
        gAmr.cmdFL          = fl;
        gAmr.cmdFR          = fr;
        gAmr.cmdRL          = rl;
        gAmr.cmdRR          = rr;
        OSSemPost(DataSem);

        if (mode == 2)
            OSFlagPost(AmrFlags, AMR_FLAG_STOP, OS_FLAG_SET, &err);
        else
            OSFlagPost(AmrFlags, AMR_FLAG_STOP, OS_FLAG_CLR, &err);

        if (mode == 1)
            OSFlagPost(AmrFlags, AMR_FLAG_SLOW, OS_FLAG_SET, &err);
        else
            OSFlagPost(AmrFlags, AMR_FLAG_SLOW, OS_FLAG_CLR, &err);

        if (buzzerEnable)
            OSFlagPost(AmrFlags, AMR_FLAG_BUZZER, OS_FLAG_SET, &err);
        else
            OSFlagPost(AmrFlags, AMR_FLAG_BUZZER, OS_FLAG_CLR, &err);

        OSTimeDlyHMSM(0, 0, 0, CONTROL_PERIOD_MS);
    }
}

void DriveTask(void *data)
{
    INT8U err;

    (void)data;

    for (;;)
    {
        float fl;
        float fr;
        float rl;
        float rr;
        float scale;
        INT8U buzzerEnable;
        INT32U buzzerPeriod;
        INT32U now;

        OSSemPend(DataSem, 0, &err);
        fl = gAmr.cmdFL;
        fr = gAmr.cmdFR;
        rl = gAmr.cmdRL;
        rr = gAmr.cmdRR;
        scale = gAmr.speedScale;
        buzzerEnable = gAmr.buzzerEnable;
        buzzerPeriod = gAmr.buzzerPeriodMs;
        now = HAL_GetTick();
        OSSemPost(DataSem);

        if (scale <= 0.0f)
        {
            StopAllMotors();
        }
        else
        {
            SetMotorFL(fl * scale);
            SetMotorFR(fr * scale);
            SetMotorRL(rl * scale);
            SetMotorRR(rr * scale);
        }

        OSSemPend(DataSem, 0, &err);
        if (buzzerEnable && buzzerPeriod > 0)
        {
            if ((now - gAmr.buzzerLastTick) >= (buzzerPeriod / 2))
            {
                gAmr.buzzerLastTick = now;
                gAmr.buzzerState = (gAmr.buzzerState == 0) ? 1 : 0;
                HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin,
                                  gAmr.buzzerState ? GPIO_PIN_SET : GPIO_PIN_RESET);
            }
        }
        else
        {
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
            gAmr.buzzerState = 0;
        }
        OSSemPost(DataSem);

        OSTimeDlyHMSM(0, 0, 0, DRIVE_PERIOD_MS);
    }
}

void UartTask(void *data)
{
    INT8U err;
    char msg[160];

    (void)data;

    for (;;)
    {
        float front;
        float left;
        float right;
        float scale;
        INT8U leftBlack;
        INT8U rightBlack;
        INT8U mode;
        INT16U leftCnt;
        INT16U rightCnt;
        int len;

        OSSemPend(DataSem, 0, &err);
        front      = gAmr.frontCm;
        left       = gAmr.leftCm;
        right      = gAmr.rightCm;
        leftBlack  = gAmr.lineLeft;
        rightBlack = gAmr.lineRight;
        mode       = gAmr.obstacleMode;
        scale      = gAmr.speedScale;
        leftCnt    = gAmr.leftBlackCnt;
        rightCnt   = gAmr.rightBlackCnt;
        OSSemPost(DataSem);

        len = snprintf(msg, sizeof(msg),
                       "ULTRA F=%.1f L=%.1f R=%.1f | LINE L=%u R=%u Lc=%u Rc=%u | MODE=%u SCALE=%.2f\r\n",
                       front, left, right, leftBlack, rightBlack, leftCnt, rightCnt, mode, scale);

        OSSemPend(UartSem, 0, &err);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 50);
        OSSemPost(UartSem);

        OSTimeDlyHMSM(0, 0, 0, UART_PERIOD_MS);
    }
}