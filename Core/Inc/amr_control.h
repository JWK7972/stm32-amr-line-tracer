#ifndef __AMR_CONTROL_H__
#define __AMR_CONTROL_H__

#include "amr_app.h"

void DecideLineTrace(INT8U leftBlack, INT8U rightBlack,
                     INT16U leftCnt, INT16U rightCnt,
                     float *fl, float *fr, float *rl, float *rr);

#endif