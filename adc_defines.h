#ifndef ADC_DEFINES_H
#define ADC_DEFINES_H

#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define ADCCLK 3000000                     // ADC clock 3 MHz
#define CLKDIV ((PCLK / ADCCLK) - 1)       // Clock Divider for ADC

// ADCR SFR bits
#define PDN_BIT 21
#define ADC_START_BIT 24
#define CLKDIV_BITS 8

// ADDR SFR bits
#define DONE_BIT 31
#define RESULT_BITS 6

#endif

