#pragma once

#include <3ds/types.h>
#include <3ds/synchronization.h>

typedef struct TaskRunner {
    LightEvent readyEvent;
    LightEvent parametersSetEvent;
    void (*task)(void *argdata);
    u8 argStorage[0x40];
    bool shouldTerminate;
} TaskRunner;

extern TaskRunner g_taskRunner;

void TaskRunner_Init(void);
void TaskRunner_RunTask(void (*task)(void *argdata), void *argdata, size_t argsize);
void TaskRunner_Terminate(void);

/// Thread function
void TaskRunner_HandleTasks(void *p);
void TaskRunner_WaitReady(void);
