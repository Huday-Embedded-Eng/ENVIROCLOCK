#ifndef __ADC_H__
#define __ADC_H__

#include <LPC214x.h>
#include "types.h"
#include "adc_defines.h"

// Function prototypes
void Init_adc(void);
void Read_adc(u32 *adcDval, u32 ChNo, u32 *eAr);

#endif

