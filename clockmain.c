#include <LPC214x.h>
#include "types.h"
#include "delay.h"
#include "rtc.h"
#include "lcd.h"
#include "adc.h"
#include "kpm.h"
#include "adc_defines.h"
#include "lcd_defines.h"
#include "rtc_defines.h"
#include "kpm_defines.h"

#define BUZZ (1 << 7) // LED on P0.7
#define AL_SW (1 << 5)// (P0.5)

u8 cgramLUT[8]= 
{
	0x04,0x0E,0x0E,0x0E,0x1F,0x00,0x04,0x00
};

RTC_Time CurrentTime = {23,5,0};
RTC_Date CurrentDate = {30,10,2025};


volatile u8 alarm_index = 0;
u8 alarm_active = 0;

u32 dayOfWeek = 1;
u32 adcDval;
u32 eAr, temperature;

void ScheduleNextAlarm(void)
{
    if(alarm_index < MAX_ALARMS)
    {
        SetRTCAlarm(&alarms[alarm_index]);
        RTC_EnableAlarmInterrupt();
    }
}

void DisplayClockData(void)
{
    // Update time/date/day
    GetRTCTimeInfo(&CurrentTime);
    GetRTCDateInfo(&CurrentDate);
    GetRTCDay(&dayOfWeek);

    // Read temperature
    Read_adc(&adcDval,1,&eAr);
    temperature = eAr / 10;

    // Display time/date/day
    DisplayRTCTime(&CurrentTime);
    DisplayRTCDay(dayOfWeek);
    DisplayRTCDate(&CurrentDate);

    // Display temperature
    CmdLCD(GOTO_LINE1_POS0 + 9);
    CharLCD('T');
		CharLCD(':');
    U32LCD(temperature);
    CharLCD(0xDF); // Degree symbol
    CharLCD('C');
}

int main(void)
{
	  u8 lastSecond = 255; // force first update
	
    IODIR0 |= BUZZ;
    IODIR0 &= ~AL_SW;

    Init_LCD();
    delayMS(20);
    Init_adc();
    Init_Rtc();
		Init_KPM();

    SetRTCTimeInfo(&CurrentTime);
    SetRTCDateInfo(&CurrentDate);
    SetRTCDay(dayOfWeek);

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("DIGITAL CLOCK");
    BuildCGRAM(cgramLUT,8);
    delayMS(50);
    CmdLCD(CLEAR_LCD);
		
    ScheduleNextAlarm();
		RTC_EnableExtInterrupt();

    while(1)
    {
        // Update display only once per second
        GetRTCTimeInfo(&CurrentTime);
        if(CurrentTime.seconds != lastSecond)
        {
            lastSecond = CurrentTime.seconds;
            DisplayClockData();
        }
				
				if (alarm_flag)
					{
						alarm_flag = 0;
						alarm_active = 1;
						delayMS(2);
						CmdLCD(GOTO_LINE2_POS0 + 15);
						CharLCD(0);
					}
					
				if (alarm_active)
					{
							IOSET0 = BUZZ;
							delayMS(100);

							// Check if user pressed switch
							if ((IOPIN0 & AL_SW) == 0)
							{
									alarm_active = 0;
									IOCLR0 = BUZZ;
									CmdLCD(CLEAR_LCD);
									delayMS(2);
									StrLCD("Alarm Stopped");
									delayMS(1000);

									CmdLCD(CLEAR_LCD);
									DisplayClockData();
								
									alarm_index++;
									ScheduleNextAlarm();
							}
					
				}
					
				//external interrupt
				if (eint0_flag)
				{
						eint0_flag = 0;
						HandleMenu(); // handle menu on external interrupt
						CmdLCD(CLEAR_LCD);
						DisplayClockData(); // refresh time after returning from menu
				}
    }
}
