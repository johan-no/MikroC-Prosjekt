/*
 * mikrokon_project_2021.c
 *
 * Created: 04.05.2021 14:56:13
 * Author : Grand Wizard Ølë
 */ 

#define F_CPU 16e6

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>


#include "SETUP_adc.h"
#include "SETUP_usart.h"



int main(void)
{
	
	// 1024x prescaler (set CS12 & CS10) and CTC mode with OCR1A top (mode 4 - set WGM12)
	// This gives 16e6/1024 = 15625 ticks pr. second
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	
	// Enable interrupts
	sei();
	
	// PB0 (LED) og PB1/OC1A (Timer) som output
	DDRB = (1 << PORTB0) | (1 << PORTB1);
	
	// Intern resistor
	PORTD = (1 << PORTD2);
	
	// Enable INT0
	EIMSK = (1 << INT0);
	
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
		
		_delay_ms(500);
		
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
		
		ADC_Lightsensor();
    }
}

// INT0 funksjon
ISR(INT0_vect){
	
	//Debouncing
	_delay_ms(500);
	
	//Toggle LED
	PORTB ^= (1 << PORTB0);
	
	//Enable timer og set timer til 500ms
	if (bit_is_set(PORTB,PB0))
	{
		TCCR1A = (1 << COM1A0);
		OCR1A = ((F_CPU / 1024) * 500) / 1000;
	}
	
	//Disable timer
	if (bit_is_clear(PORTB,PB0))
	{
		TCCR1A = 0;
	}
}


// ---------------------------- //
// Funksjonen ADC_Lightsensor() //
// ---------------------------- //

void ADC_Lightsensor (void) {

// ---------------------------- //
// -------- Variabler --------- //
// ---------------------------- //
	// Grenseverdi for Dag/Natt
	// 500 var mørkt rom når solen gikk ned
	const uint16_t ADC_Threshold_Value = 500;
	uint16_t ADC_Current_Value;

	// State-maskin
	uint8_t Light_Current_State;
	// Starter ved å anta at det er dag, siden da gjør den ingen ting
	uint8_t Light_Previous_State = 1;

	// LED nattlys, for å se panelet
	// Varmer ekstra om natten
	// Setter pins til output
	#define Night_Light PD4
	#define Night_Varmth PD5
	DDRD |= (1 << Night_Light) | (1 << Night_Varmth);	// Data Direction Register D: writing a one to the bit enables output
	
	
// ---------------------------- //
// ------ Funksjons Kode ------ //
// ---------------------------- //
	// ADC: Leser fra ADC
	ADCSRA |= (1 << ADSC); // Start ADC conversion
	loop_until_bit_is_clear(ADCSRA, ADSC); // Wait until done
	ADC_Current_Value = ADC; // Read ADC in


	// Tester først ADC_Current_Value mot ADC_Threshold_Value
	if (ADC_Current_Value > ADC_Threshold_Value) {
		// Dag
		Light_Current_State = 1;
	}
	else {
		// Natt
		Light_Current_State = 0;
	}
	// Ser om ADC_Current_Value er endret siden sist
	// Light_Previous_State starter opp som dag
	if (Light_Current_State != Light_Previous_State) {
		// Dag
		if (Light_Current_State == 1) {
			// Slår av nattlys og varme
			PORTB = (0 << Night_Light) | (0 << Night_Varmth);
		}
		// Natt
		else {
			// Slår på nattlys og varme
			PORTB = (1 << Night_Light) | (1 << Night_Varmth);
		}
		// Setter previous state
		Light_Previous_State = Light_Current_State;
	}
	} // Slutten på Funksjonen: ADC_Lightsensor