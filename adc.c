#include <LPC214x.h>
#include "types.h"
#include "adc.h"
#include "adc_defines.h"
#include "pin_block.h"
#include "pin_block_defines.h"
#include "delay.h"

void Init_adc(void)
{
    // Power up ADC and set clock to 3 MHz
    AD0CR = (1 << PDN_BIT) | (CLKDIV << CLKDIV_BITS);
    
    // Configure ADC input pin (AD0.1 on P0.28)
    CfgPortPinFunc(0, 28, PIN_FUNC_2);
}

void Read_adc(u32 *adcDval, u32 ChNo, u32 *eAr)
{
    // Clear old channel select bits
    AD0CR &= 0xFFFFFF00;

    // Select channel and start conversion
    AD0CR |= (1 << ChNo) | (1 << ADC_START_BIT);

    // Small delay to stabilize conversion
    delayUS(5);

    // Wait for conversion complete
    while(((AD0GDR >> DONE_BIT) & 1) == 0);

    // Fetch result (10-bit ADC)
    *adcDval = (AD0GDR >> RESULT_BITS) & 0x3FF;
	
	  // Stop conversion
    AD0CR &= ~(1 << ADC_START_BIT);

    // Convert ADC value to voltage
    //*eAr = (*adcDval * (3.3 / 1023));
		*eAr = ((*adcDval) * 3300) / 1023;
}
