//lcd.c
#include <LPC214x.h>
#include "types.h"
#include "delay.h"
#include "lcd_defines.h"
#include "lcd.h"

void WriteLCD(u8 byte)
{
	//select write operation
	//remove this for hardware usage
	// For hardware: comment the below line (RW tied to GND)
	IOCLR0 = 1 << LCD_RW; //R = 1/W = 0
	
	//write byte to lcd data pins
	IOPIN0 = ((IOPIN0 & ~(0xFF << LCD_DATA))) | ((byte & 0xFF) << LCD_DATA);
	
	//provide high to low pulse
	IOSET0 = 1 << LCD_EN; 			//EN = 1
	delayUS(40);  								// inside WriteLCD instead of 2 µs write 10
	IOCLR0 = 1 << LCD_EN; 			//EN = 0
	delayMS(2); 								// after each command instead of 2 ms write 5
}

void Init_LCD(void)
{
    // Configure LCD data and control pins as outputt
		// For hardware: remove (1 << LCD_RW) if RW is tied to GND
    IODIR0 |= (0xFF << LCD_DATA) | (1 << LCD_RS) | (1 << LCD_EN)|(1 << LCD_RW); 

		//(1 << LCD_RW) remove this for hardware usage 
    // Power-on delay
    delayMS(20); // power-on delay instead of 20 ms wite 30

    // Function set commands (as per datasheet)
    CmdLCD(MODE_8BIT_1LINE);

    delayMS(6);

    CmdLCD(MODE_8BIT_1LINE);

		delayMS(1);

		CmdLCD(MODE_8BIT_1LINE);

		delayMS(1);

    CmdLCD(MODE_8BIT_2LINE);

		// Display ON, Cursor OFF
    CmdLCD(DSP_ON_CUR_OFF);

    // Clear display
    CmdLCD(CLEAR_LCD);

    // Entry mode set - cursor moves right
    CmdLCD(SHIFT_CUR_RIGHT);

		//Enabling the display with blinking cursor
		CmdLCD(0x0f);
}

void CmdLCD(u8 cmd)
{
	//set pin for cmd / inst reg
	IOCLR0 = 1 << LCD_RS; //CMD = 0
	
	//write cmd byte to cmd reg
	WriteLCD(cmd);
}

void CharLCD(u8 asciiVal)
{
    IOSET0 = 1 << LCD_RS;   // RS = 1 ? data mode
    WriteLCD(asciiVal);     // send character
    IOCLR0 = 1 << LCD_RS;   // RS = 0 ? back to command mode
}

	
void StrLCD(const char *str)
{
	while(*str)
	{
		CharLCD((u8)*str++);  // Cast char to u8
		delayMS(1);
	}
}

void U32LCD(u32 n)
{
	s32 i=0;
	u8 a[10];
	
	if(n==0)
	{
		CharLCD('0');
	}
	else
	{
	  while(n>0)
    {
	  a[i++]=(n%10)+48;
      n /= 10;			
    }
    for(--i; i>=0; i--)
      CharLCD(a[i]);		
	}
}

void S32LCD(s32 num)
{
	if(num<0)
	{
		CharLCD('-');
		num=-num;
	}
	U32LCD(num);
}

void F32LCD(f32 fnum,u32 nDP)
{
	u32 num,i;
	if(fnum<0.0)
	{
		CharLCD('-');
		fnum=-fnum;
	}
	num=fnum;
	U32LCD(num);
	CharLCD('.');
	for(i=0;i<nDP;i++)
	{
		fnum=(fnum-num)*10;
		num=fnum;
		CharLCD(num+48);
	}
}

void BuildCGRAM(u8 *p,u8 nBytes)
{
	u32 i;
	//point to cgram start
	CmdLCD(GOTO_CGRAM_START);
	//select data reg
	IOSET0=1<<LCD_RS;
	
	for(i=0;i<nBytes;i++)
	{
		//write to cgram vi data reg
		WriteLCD(p[i]);
	}
	//point back to ddram start/display
	CmdLCD(GOTO_LINE1_POS0);
}






