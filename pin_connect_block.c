#include <LPC214x.h>
#include "types.h"
#include "delay.h"
#include "pin_block.h"

// 00 → GPIO (default)

// 01 → Alternate Function 1

// 10 → Alternate Function 2

// 11 → Alternate Function 3

//.......................................................................................

// 📌 Example for P0.0          📌 Example for P0.1             📌 Example for P0.2

// 00 → GPIO (default)              00 → GPIO                       00 → GPIO

// 01 → TXD0 (UART0 Transmit)       01 → RXD0 (UART0 Receive)       01 → SCL0 (I²C0 Clock)

// 10 → PWM1                        10 → PWM3                       10 → CAP0.0 (Timer0 Capture Input)

// 11 → Reserved                    11 → EINT0                      11 → Reserved

// So here 01 = UART0.              Here too, 01 = UART0            Here 01 is I²C, not UART.

//............So................................................................................

// 00 = GPIO always

// 01 / 10 / 11 → depend on pin (from datasheet’s Pin Function Table)


void CfgPortPinFunc(u32 portNo, u32 pinNo, u32 pinFuncNo)
{
    if (portNo == 0)
    {
        if (pinNo <= 15)   // P0.0 - P0.15 -> PINSEL0
        {
            PINSEL0 = (PINSEL0 & ~(3 << (pinNo * 2))) | (pinFuncNo << (pinNo * 2));
        }
        else if (pinNo >= 16 && pinNo <= 31) // P0.16 - P0.31 -> PINSEL1
        {
            PINSEL1 = (PINSEL1 & ~(3 << ((pinNo - 16) * 2))) | (pinFuncNo << ((pinNo - 16) * 2));
        }
    }
    else if (portNo == 1)
    {
        if (pinNo >= 16 && pinNo <= 31)   // P1.16 - P1.31 -> PINSEL2
        {
            PINSEL2 = (PINSEL2 & ~(3 << ((pinNo - 16) * 2))) | (pinFuncNo << ((pinNo - 16) * 2));
        }
    }
}

