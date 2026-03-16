#ifndef __AMR_TASK_H__
#define __AMR_TASK_H__

#include "amr_app.h"

void StartTask(void *data);
void SensorTask(void *data);
void ControlTask(void *data);
void DriveTask(void *data);
void UartTask(void *data);

#endif