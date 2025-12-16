//lcd.h
#include <LPC214x.h>
#include "types.h"

// Initialize LCD (8-bit, 2-line, display ON, cursor OFF)
void Init_LCD(void);

// Write a single byte to LCD data pins
void WriteLCD(u8 byte);

// Send command to LCD (RS = 0)
void CmdLCD(u8 cmd);

// Send single character to LCD (RS = 1)
void CharLCD(u8 asciiVal);

// Display a string on LCD
void StrLCD(const char *str);

// Display unsigned 32-bit number
void U32LCD(u32 n);

// Display signed 32-bit number
void S32LCD(s32 num);

// Display floating-point number
void F32LCD(f32 fnum,u32 nDP);

// Build custom character in CGRAM
void BuildCGRAM(u8 *p, u8 nBytes);

