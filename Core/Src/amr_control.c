#include "amr_control.h"

void DecideLineTrace(INT8U leftBlack, INT8U rightBlack,
                     INT16U leftCnt, INT16U rightCnt,
                     float *fl, float *fr, float *rl, float *rr)
{
    if (leftBlack == 0 && rightBlack == 0)
    {
        *fl = FWD_FL;
        *fr = FWD_FR;
        *rl = FWD_RL;
        *rr = FWD_RR;
        return;
    }

    if (leftBlack == 1 && rightBlack == 0)
    {
        if (leftCnt < SHARP_TURN_COUNT)
        {
            *fl = FL_FAST;
            *fr = FR_SLOW;
            *rl = RL_FAST;
            *rr = RR_SLOW;
        }
        else
        {
            *fl = FL_FAST;
            *fr = 0.0f;
            *rl = RL_FAST;
            *rr = 0.0f;
        }
        return;
    }

    if (leftBlack == 0 && rightBlack == 1)
    {
        if (rightCnt < SHARP_TURN_COUNT)
        {
            *fl = FL_SLOW;
            *fr = FR_FAST;
            *rl = RL_SLOW;
            *rr = RR_FAST;
        }
        else
        {
            *fl = 0.0f;
            *fr = FR_FAST;
            *rl = 0.0f;
            *rr = RR_FAST;
        }
        return;
    }

    if (leftBlack == 1 && rightBlack == 1)
    {
        if (leftCnt > rightCnt)
        {
            *fl = FL_FAST;
            *fr = 0.0f;
            *rl = RL_FAST;
            *rr = 0.0f;
        }
        else if (rightCnt > leftCnt)
        {
            *fl = 0.0f;
            *fr = FR_FAST;
            *rl = 0.0f;
            *rr = RR_FAST;
        }
        else
        {
            *fl = 0.0f;
            *fr = 0.0f;
            *rl = 0.0f;
            *rr = 0.0f;
        }
        return;
    }

    *fl = 0.0f;
    *fr = 0.0f;
    *rl = 0.0f;
    *rr = 0.0f;
}