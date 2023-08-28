#pragma once

/* Scheduler Related */
#define configUSE_PREEMPTION     1
#define configUSE_TICKLESS_IDLE  0
#define configUSE_IDLE_HOOK      0
#define configUSE_TICK_HOOK      0
#define configTICK_RATE_HZ       ((TickType_t)1000)
#define configMAX_PRIORITIES     32
#define configMINIMAL_STACK_SIZE (configSTACK_DEPTH_TYPE)256
#define configUSE_16_BIT_TICKS   0

#define configIDLE_SHOULD_YIELD 1

/* Synchronization Related */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               8
#define configUSE_QUEUE_SETS                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5

/* System */
#define configSTACK_DEPTH_TYPE           uint32_t
#define configMESSAGE_BUFFER_LENGTH_TYPE size_t

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION  1
#define configSUPPORT_DYNAMIC_ALLOCATION 0
// #define configTOTAL_HEAP_SIZE            (128 * 1024)
#define configAPPLICATION_ALLOCATED_HEAP 0

/* Hook function related definitions. */
#define configCHECK_FOR_STACK_OVERFLOW     2
#define configUSE_MALLOC_FAILED_HOOK       1
#define configUSE_DAEMON_TASK_STARTUP_HOOK 0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS        0
#define configUSE_TRACE_FACILITY             1
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

/* Software timer related definitions. */
#define configUSE_TIMERS             1
#define configTIMER_TASK_PRIORITY    (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH     10
#define configTIMER_TASK_STACK_DEPTH 1024

/* SMP port only */
#define configNUMBER_OF_CORES         2
#define configTICK_CORE               0
#define configRUN_MULTIPLE_PRIORITIES 0

/* RP2040 specific */
#define configSUPPORT_PICO_SYNC_INTEROP 1
#define configSUPPORT_PICO_TIME_INTEROP 1

#include <assert.h>
/* Define to trap errors during development. */
#define configASSERT(x) assert(x)

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_xTaskGetCurrentTaskHandle   1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_xTaskGetIdleTaskHandle      1
#define INCLUDE_eTaskGetState               1
#define INCLUDE_xTimerPendFunctionCall      1
#define INCLUDE_xTaskAbortDelay             1
#define INCLUDE_xTaskGetHandle              1
#define INCLUDE_xTaskResumeFromISR          1
#define INCLUDE_xQueueGetMutexHolder        1

/* A header file that defines trace macro can be included here. */

/* SMP Related config. */
#define configUSE_MINIMAL_IDLE_HOOK 0
#define portSUPPORT_SMP             1