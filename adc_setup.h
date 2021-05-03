/*
 * adc_setup.h
 *
 *  Author: olemarh
 *	
 *	Initial configuration of ADC
 *	
 */ 


#ifndef ADC_SETUP_H_
#define ADC_SETUP_H_

// Single ended conversion is equal to:
// ADC = (V_in * 1024) / V_ref
// where V_ref = AV_CC = V_in, making it ADC = 1024


// ADIF = SET when ADC conversion complete
// ADSC = SET WHILE converting, CLEAR when done
// ADATE = Auto-ADC, disable?

// Pre-defining ADC channels for on-the-fly ADMUX change
// ADMUX bits 7-4 should only be set as 0b0110xxxx as they configure the ADC behaviour.

#define ADMUX_ADC0 0b01100000
#define ADMUX_ADC1 0b01100001
#define ADMUX_ADC2 0b01100010
// #define ADMUX_ADC3 0b01100011
// #define ADMUX_ADC4 0b01100100
// #define ADMUX_ADC5 0b01100101


void adc_initial_startup(void)
{
	// Enable ADC and ADPS2/1/0
	// ADPS2/1/0 gives division factor 128 resulting in 125kHz
	ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// Set AVCC as ref with ADLAR 1 and ADC0 SE input as default
	ADMUX = ADMUX_ADC0;
}

#endif /* ADC_SETUP_H_ */
