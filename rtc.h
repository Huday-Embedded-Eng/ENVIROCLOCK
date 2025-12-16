#ifndef RTC_H
#define RTC_H

#include "types.h"

#define MAX_ALARMS 5

/* Structures */
typedef struct {
    u8 hours;
    u8 minutes;
    u8 seconds;
} RTC_Time;

typedef struct {
    u8 date;
    u8 month;
    u16 year;
} RTC_Date;

typedef struct {
    u8 hours;
    u8 minutes;
    u8 seconds;
} RTC_Alarm;

/* Function Prototypes */
void Init_Rtc(void);

/* Time functions */
void SetRTCTimeInfo(RTC_Time *t);
void GetRTCTimeInfo(RTC_Time *t);
void DisplayRTCTime(RTC_Time *t);

/* Date functions */
void SetRTCDateInfo(RTC_Date *d);
void GetRTCDateInfo(RTC_Date *d);
void DisplayRTCDate(RTC_Date *d);

/* Day functions */
void SetRTCDay(u32 dayofweek);
void GetRTCDay(u32 *dayofweek);
void DisplayRTCDay(u32 dayofweek);

/* Alarm functions */
void SetRTCAlarm(RTC_Alarm *a);
void RTC_EnableAlarmInterrupt(void);
void RTC_Interrupt_Handler(void) __irq;

extern volatile u32 alarm_flag;
extern volatile u8 eint0_flag;
extern void HandleMenu(void);
extern volatile u8 alarm_index;  // defined in main.c

void RTC_EnableExtInterrupt(void);
void Eint0_isr(void) __irq;

extern RTC_Alarm alarms[MAX_ALARMS];  // <-- declare, not define

#endif
