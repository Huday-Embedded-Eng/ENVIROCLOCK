#include <LPC214x.h>
#include "types.h"
#include "rtc.h"
#include "rtc_defines.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "kpm_defines.h"
#include "delay.h"
#include "pin_block_defines.h"
#include "pin_block.h"


#define RTC_VIC_CHNO 13
#define EINT0_VIC_CHNO 14
#define EINT0_LED 16			//p1.16


const char *week[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

volatile u32 alarm_flag = 0;
volatile u8 eint0_flag = 0;

void Init_Rtc(void)
{
	PCONP |= (1 << 9);
	
	CCR = 0x00;
	PREINT = PREINT_VAL;
	PREFRAC = PREFRAC_VAL;
	
	CCR = (1 << 4) | (1 << 0);
}

void SetRTCTimeInfo(RTC_Time *t)
{
	HOUR = t->hours;
	MIN =  t->minutes;
	SEC =  t->seconds;
}

void GetRTCTimeInfo(RTC_Time *t)
{
	 t->hours = HOUR;
	 t->minutes = MIN;
	 t->seconds = SEC;
}

void DisplayRTCTime(RTC_Time *t)
{
    CmdLCD(GOTO_LINE1_POS0);  // Line 1 start 

    // Display hours
    CharLCD((t->hours / 10) + '0');
    CharLCD((t->hours % 10) + '0');
    CharLCD(':');

    // Display minutes
    CharLCD((t->minutes / 10) + '0');
    CharLCD((t->minutes % 10) + '0');
    CharLCD(':');

    // Display seconds
    CharLCD((t->seconds / 10) + '0');
    CharLCD((t->seconds % 10) + '0');
}

void SetRTCDateInfo(RTC_Date *d)
{
	DOM = d->date;
	MONTH =  d->month;
	YEAR =  d->year;
}

void GetRTCDateInfo(RTC_Date *d)
{
	 d->date = DOM;
	 d->month = MONTH;
	 d->year = YEAR;
}

void DisplayRTCDate(RTC_Date *d)
{
    CmdLCD(GOTO_LINE2_POS0); 
	
    // Display hours
    CharLCD((d->date / 10) + '0');
    CharLCD((d->date % 10) + '0');
    CharLCD('/');

    // Display minutes
    CharLCD((d->month / 10) + '0');
    CharLCD((d->month % 10) + '0');
    CharLCD('/');

    // Display seconds
    U32LCD(d->year);
 
}

void SetRTCDay(u32 dayofweek)
{
	DOW = dayofweek & 0x7;
}
void GetRTCDay(u32 *dayofweek)
{
	*dayofweek = DOW & 0x7;
}

void DisplayRTCDay(u32 dayofweek)
{
    CmdLCD(GOTO_LINE2_POS0 + 11);
    if (dayofweek <= 6)
        StrLCD(week[dayofweek]);
    else
        StrLCD("---");
}

/* Alarm setup */
void SetRTCAlarm(RTC_Alarm *a)
{
    ALSEC  = a->seconds;
    ALMIN  = a->minutes;
    ALHOUR = a->hours;
}

void RTC_EnableAlarmInterrupt(void)
{
	AMR = 0xF8;
	CIIR = 0x00;
	ILR = 0x03;
	
	VICIntSelect = 0;
	VICIntEnable = (1 << RTC_VIC_CHNO);
	VICVectAddr0 = (u32 )RTC_Interrupt_Handler;
  VICVectCntl0 = (1 << 5) | RTC_VIC_CHNO;
}

void RTC_Interrupt_Handler(void) __irq
{
	  if (ILR & 0x02)
    {
        alarm_flag = 1;
        ILR = 0x02; 
    }
    VICVectAddr = 0;
}

/* ------------------- EXTERNAL INTERRUPT ------------------- */

void RTC_EnableExtInterrupt(void)
{
		CfgPortPinFunc(0,1,EINT0_0_1);

    VICIntSelect = 0;
	  VICIntEnable  = (1 << EINT0_VIC_CHNO);
		VICVectCntl1  = (1 << 5) | EINT0_VIC_CHNO;
		VICVectAddr1 = (u32 )Eint0_isr;
	
	  EXTMODE  =  (1 << 0);   // Edge sensitive
    //EXTPOLAR = 0;   // Falling edge trigger
		IODIR1 |= (1 << EINT0_LED);
}

void Eint0_isr(void) __irq
{
		eint0_flag = 1;
    EXTINT = (1 << 0);
    VICVectAddr = 0;
}

/* ------------------- MENU HANDLING ------------------- */

/*void ModifyTime()
{
    u8 h, m, s, Vkey;
    u32 num;

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Enter Hour:");
    ReadNum(&num, &Vkey);
    h = num;

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Enter Min:");
    ReadNum(&num, &Vkey);
    m = num;

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Enter Sec:");
    ReadNum(&num, &Vkey);
    s = num;

    HOUR = h;
    MIN = m;
    SEC = s;

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Time Updated!");
    delayMS(1000);
}*/

void ModifyTime()
{
    u8 option, Vkey;
    u32 num;
    
    u8 h = HOUR;
    u8 m = MIN;
    u8 s = SEC;

		while(1)
		{
				CmdLCD(CLEAR_LCD);
				delayMS(2);
				StrLCD("1:H 2:M 3:S");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("=:Exit  Sel:");

				option = KeyScan();
				while(ColScan() == 0);
				delayMS(200);
				
				// Exit to main menu
				if(option == '=')
				{
						CmdLCD(CLEAR_LCD);
						delayMS(2);
						StrLCD("Exit Time Menu");
						delayMS(800);
						return;
				}

				switch(option)
				{
						case '1':   //hours
								CmdLCD(CLEAR_LCD);
								delayMS(2);
								StrLCD("Enter Hour(0-23)");
								ReadNum(&num, &Vkey);
								if(num <= 23) h = num;
								break;

						case '2':   //mins
								CmdLCD(CLEAR_LCD);
								delayMS(2);
								StrLCD("Enter Min(0-59)");
								ReadNum(&num, &Vkey);
								if(num <= 59) m = num;
								break;

						case '3':   //secs
								CmdLCD(CLEAR_LCD);
								delayMS(2);
								StrLCD("Enter Sec(0-59)");
								ReadNum(&num, &Vkey);
								if(num <= 59) s = num;
								break;

						default:
								CmdLCD(CLEAR_LCD);
								delayMS(2);
								StrLCD("Invalid Key!");
								delayMS(800);
								continue;
				}

				// Update RTC time
				HOUR = h;
				MIN  = m;
				SEC  = s;

				CmdLCD(CLEAR_LCD);
				delayMS(2);
				StrLCD("Time Updated!");
				delayMS(800);
		}
}


/*void ModifyDate()
{
    u8 d, x, Vkey;
    u16 y;
    u32 num, dayOfWeek;

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Enter Date:");
    ReadNum(&num, &Vkey);
    d = num;

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Enter Month:");
    ReadNum(&num, &Vkey);
    x = num;

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Enter Year:");
    ReadNum(&num, &Vkey);
    y = num;
		
    CmdLCD(CLEAR_LCD);
    delayMS(2);
    StrLCD("Day(1=Sun..7=Sat):");
    ReadNum(&num, &Vkey);
    dayOfWeek = num;
	
	  if(d > 31) d = 31;
    if(x > 12) x = 12;
		if (dayOfWeek < 1 || dayOfWeek > 7) dayOfWeek = 1;

    DOM = d;
    MONTH = x;
    YEAR = y;
		SetRTCDay(dayOfWeek);

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Date Updated!");
    delayMS(1000);
}*/

void ModifyDate()
{
    u8 d = DOM;
		u8 x = MONTH;
    u16 y = YEAR;
    u32 num, dayOfWeek;
		u8 option, Vkey;

		while(1)
		{
				CmdLCD(CLEAR_LCD);
				delayMS(2);
				StrLCD("1-Date 2-Month");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("3-Year 4-Day =Ex");

				option = KeyScan();
				while(ColScan() == 0);
				delayMS(200);
				
				// Exit to main menu
				if(option == '=')
				{
						CmdLCD(CLEAR_LCD);
						delayMS(2);
						StrLCD("Exit Date Menu");
						delayMS(800);
						return;
				}

				switch(option)
				{
						case '1':   
								CmdLCD(CLEAR_LCD);
								delayMS(2);
								StrLCD("Enter date:");
								ReadNum(&num, &Vkey);
								if(num >=1 && num <= 31)
									d = num;
								break;

						case '2':   
								CmdLCD(CLEAR_LCD);
								delayMS(2);
						StrLCD("Enter month:");
								ReadNum(&num, &Vkey);
								if(num >=1 && num <= 12) 
									x = num;
								break;

						case '3': 
								CmdLCD(CLEAR_LCD);
								delayMS(2);
								StrLCD("Enter year:");
								ReadNum(&num, &Vkey);
								if(num <= 2039) 
									y = num;
								break;
								
						case '4': 
                CmdLCD(CLEAR_LCD);
								delayMS(2);
                StrLCD("1=Sun..7=Sat:");
                ReadNum(&num, &Vkey);
                if(num >=1 && num <= 7)
                    dayOfWeek = num;
                break;

						default:
								CmdLCD(CLEAR_LCD);
								delayMS(2);
								StrLCD("Invalid Key!");
								delayMS(800);
								continue;
				}

				// Update RTC time
				DOM = d;
				MONTH = x;
				YEAR = y;
				SetRTCDay(dayOfWeek);

				CmdLCD(CLEAR_LCD);
				delayMS(2);
				StrLCD("Updated!");
				delayMS(800);
		}
}



// Initial manual alarms
RTC_Alarm alarms[MAX_ALARMS] = {
    {23, 6, 10},
		{23, 7, 0},
		{23, 8, 0},
		{23, 9, 0}
};

void SetAlarm(void)
{
    u8 choice, key;
    u32 temp; // for ReadNum()

    CmdLCD(CLEAR_LCD);
		delayMS(2);
    StrLCD("Set Alarm (1-4)");
    delayMS(500);

    choice = KeyScan(); // Get user input
    choice -= '0';      // Convert ASCII to number
	
	  // Wait for key release to avoid repeated inputs
    while(ColScan() == 0);  // wait until all keys released
    delayMS(200);            // debounce delay

    if (choice >= 1 && choice <= MAX_ALARMS)
    {
        CmdLCD(CLEAR_LCD);
				delayMS(2);
        StrLCD("Enter Hour:");
        ReadNum(&temp, &key);
        alarms[choice - 1].hours = (u8)temp;

        CmdLCD(CLEAR_LCD);
				delayMS(2);
        StrLCD("Enter Min:");
        ReadNum(&temp, &key);
        alarms[choice - 1].minutes  = (u8)temp;

        CmdLCD(CLEAR_LCD);
				delayMS(2);
        StrLCD("Enter Sec:");
        ReadNum(&temp, &key);
        alarms[choice - 1].seconds  = (u8)temp;
			
			  // **Write immediately to RTC hardware**
				alarm_index = choice - 1;          // set the current alarm index
				SetRTCAlarm(&alarms[alarm_index]); // apply to hardware

        CmdLCD(CLEAR_LCD);
				delayMS(2);
        StrLCD("Alarm ");
        CharLCD(choice + '0');
				delayMS(2);
        StrLCD(" Set OK!");
        delayMS(1000);
    }
    else
    {
        CmdLCD(CLEAR_LCD);
				delayMS(2);
        StrLCD("Invalid Choice!");
        delayMS(1000);
    }

}


void HandleMenu(void)
{
    u8 keyV = 0;

    CmdLCD(CLEAR_LCD);
    delayMS(2);
		StrLCD("Select Modify:");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("1.TM 2.AL 3.DA");

    // Wait for a key press
    while(keyV == 0)
    {
        keyV = KeyScan();
    }

    // Debounce delay
    delayMS(200);

    // Wait for key release
    while(ColScan() == 0);

    CmdLCD(CLEAR_LCD);
    delayMS(2);

    if(keyV == '1')
    {
        StrLCD("Modify Time");
        delayMS(1000);
        CmdLCD(CLEAR_LCD);
        delayMS(2);
        ModifyTime();
    }
    else if(keyV == '2')
    {
        StrLCD("Set Alarm");
        delayMS(1000);
        CmdLCD(CLEAR_LCD);
        delayMS(2);
        SetAlarm();
    }
		else if(keyV == '3')
    {
        StrLCD("Modify Date");
        delayMS(1000);
        CmdLCD(CLEAR_LCD);
        delayMS(2);
        ModifyDate();
    }
    else
    {
        StrLCD("Invalid Key!");
        delayMS(1000);
    }
}







	
	



