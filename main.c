/*
 * prosjekt_mikrokon_adc.c
 *
 * Created: 29.04.2021 15:19:43
 * Author : olemarh
 */ 

#define F_CPU 16e6

#include <avr/io.h>
#include <util/delay.h>

#include "timer_setup.h"
#include "adc_setup.h"
#include "usart_setup.h"


int main(void)
{
	
	timer_initial_startup();
	adc_initial_startup();
	usart_initial_startup();
	
	uint8_t ADC0_ADCH;
	uint8_t ADC1_ADCH;
	uint8_t ADC2_ADCH;
// 	uint8_t ADC3_ADCH;
// 	uint8_t ADC4_ADCH;
// 	uint8_t ADC5_ADCH;
		
    while (1)
    {
		// Start ADSC at every loop iteration
		ADCSRA |= (1 << ADSC);
		
		// Debug
		//usart_transmit(ADCH);
		
		// If-statements to sort and store ADCH into appropriate variables
		// If ADMUX is set to ADC0 and conversion is complete
		if (ADMUX == ADMUX_ADC0 && ADIF == 1 && ADSC == 0)
			{
				// Store current ADCH into ADC0_ADCH variable
				ADC0_ADCH = ADCH;
				
				// Set ADMUX to next-in-line for conversion
				// Next conversion begins at start of loop
				ADMUX = ADMUX_ADC1;
			}
			
		else if (ADMUX == ADMUX_ADC1 && ADIF == 1 && ADSC == 0)
			{
				ADC1_ADCH = ADCH;
				ADMUX = ADMUX_ADC2;
			}
			
		else if (ADMUX == ADMUX_ADC2 && ADIF == 1 && ADSC == 0)
			{
				ADC2_ADCH = ADCH;
				ADMUX = ADMUX_ADC0;
			}
			
			
		// DDRx = SET for output
		// PORTx = CLEAR for 0, SET for 1
			
		// If-statements for sorting ADC values to 2-channel digital output
		if (ADC0_ADCH < 256)
			{
				// Set 0 0 - 25%
			}
		else if ((255 < ADC0_ADCH) && (ADC0_ADCH < 512))
			{
				// Set 0 1 - 50%
			}
		else if ((511 < ADC0_ADCH) && (ADC0_ADCH < 768))
			{
				// Set 1 0 - 75%
			}
		else if (767 < ADC0_ADCH)
			{
				// Set 1 1 - 100%
			}
	}	
 }
