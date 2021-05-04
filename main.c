/*
 * prosjekt_mikrokon_adc.c
 *
 * Created: 29.04.2021 15:19:43
 * Author : olemarh
 */ 


#define F_CPU 16e6

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

//#include "timer_setup.h"
#include "adc_setup.h"
#include "usart_setup.h"


int main(void)
{
	
//	timer_initial_startup();
	adc_initial_startup();
	usart_initial_startup();
	
	uint16_t ADC0_ADCH;
	uint16_t ADC1_ADCH;
	uint16_t ADC2_ADCH;
// 	uint16_t ADC3_ADCH;
// 	uint16_t ADC4_ADCH;
// 	uint16_t ADC5_ADCH;

	
	#define ADC0_DIG1 PORTD6
	#define ADC0_DIG2 PORTD7
	
	DDRD = (1 << ADC0_DIG1) | (1 << ADC0_DIG2);
	
	ADMUX = ADMUX_ADC0;
	
	FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &uart_output;
	
    while (1)
    {
		// Start ADSC at every loop iteration
		ADCSRA |= (1 << ADSC);
		
		// Debug
		//usart_transmit();

		// If-statements to sort and store ADCH into appropriate variables
		// If ADMUX is set to ADC0 and conversion is complete:
		if (ADMUX == ADMUX_ADC0) //&& ADIF == 1 && ADSC == 0)
		{
			// Store current ADCH into ADC0_ADCH variable
			ADC0_ADCH = ADCH;
			
			// Set ADMUX to next-in-line for conversion
			// Next conversion begins at start of loop
			//ADMUX = ADMUX_ADC1;
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
			
		// If-statements for sorting ADC0 values to 2-channel digital output
		if (ADC0_ADCH < 64)
		{
			// DIG1	DIG2
			//  0	 0	- 20% Power
			PORTD = (0 << ADC0_DIG1) | (0 << ADC0_DIG2);
		}
		else if ((63 < ADC0_ADCH) && (ADC0_ADCH < 128))
		{
			// DIG1	DIG2
			//  0	 1 - 40% Power
			PORTD = (0 << ADC0_DIG1) | (1 << ADC0_DIG2);
			
		}
		else if ((127 < ADC0_ADCH) && (ADC0_ADCH < 192))
		{
			// DIG1	DIG2
			//  1	 0 - 60% Power
			PORTD = (1 << ADC0_DIG1) | (0 << ADC0_DIG2);
			
		}
		else if (191 < ADC0_ADCH)
		{
			// DIG1	DIG2
			//  1	 1 - 80% Power
			PORTD = (1 << ADC0_DIG1) | (1 << ADC0_DIG2);
			
		}
		
		// Tempsensor ADC1 handling
		// Only used to relay information via USART
		// 3 brackets as more do not add functionality; this is purely for demonstration
		if (ADC1_ADCH < 86)
		{
			//it cold
			//puts("\r\n");
		}
		else if ((85 < ADC1_ADCH) && (ADC1_ADCH < 171))
		{
			//it aight
		}
		else if (170 < ADC1_ADCH)
		{
			//it hot
		}
		
		
		
	}	
 }
