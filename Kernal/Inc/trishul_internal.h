#ifndef TRISHUL_INTERNAL_H
#define TRISHUL_INTERNAL_H

#include "trishul_types.h"

Trishul_Tcb *Trishul_SchedulerSelectNext(void);
void Trishul_SchedulerMarkRunning(Trishul_Tcb *task);
bool Trishul_SchedulerHasReadyTask(void);
void Trishul_TaskExitError(void);

#endif
