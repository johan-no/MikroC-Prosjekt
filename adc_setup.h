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

void adc_initial_startup(void)
{
	// Enable ADC and ADPS2/1/0
	// ADPS2/1/0 gives division factor 128 resulting in 125kHz
	ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// Set AVCC as ref with ADLAR 1 and ADC0 SE input
	ADMUX = 0b01100000;
}


#endif /* ADC_SETUP_H_ */