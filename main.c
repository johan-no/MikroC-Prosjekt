/* Project solution for IELET2111 Mikrokontrollersystemer, Spring 2021 
 *
 * Project participants:
 *  - Johan Olav Nordahl
 *  - Ole Marcus Hovlid
 *  - Shinthujan Poothappilai
 *	
 */ 

	  // ------------------------- //
	 // ------- Preamble -------- //
	// ------------------------- //
   
#define F_CPU 16e6   

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "SETUP_adc.h"
#include "SETUP_usart.h"

	  // -------------------------- //
	 // ------- Main-loop -------- //
	// -------------------------- //
	
int main(void)
{
	
	// Variables used for ADC handling
	uint16_t ADC0_ADCH;
	uint16_t ADC1_ADCH;
	uint16_t ADC2_ADCH;
// 	uint16_t ADC3_ADCH;
// 	uint16_t ADC4_ADCH;
// 	uint16_t ADC5_ADCH;
	
	// Defined ports used for ADC0
	#define ADC0_DIG1 PORTD6
	#define ADC0_DIG2 PORTD7
	
	// LED night-light to see panel
	// Extra heat during night (PD5)
	// Defining for easier handling
	#define Night_Light PORTD4
	#define Night_Warmth PORTD5
	
	// Set ADC0 as output
	// Set both LED and extra warmth as outputs
	DDRD = (1 << ADC0_DIG1) | (1 << ADC0_DIG2) | (1 << Night_Light) | (1 << Night_Warmth);

	// Timer configuration
	// 1024x prescaler (set CS12 & CS10) and CTC mode with OCR1A top (mode 4 - set WGM12)
	// This gives 16e6/1024 = 15625 ticks pr. second
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	
	// PB0 (LED) og PB1/OC1A (Timer) as output
	DDRB = (1 << PORTB0) | (1 << PORTB1);
	
	// Enable interrupts
	sei();
	
	// Enable INT0
	EIMSK = (1 << INT0);
	
	// Internal resistor
	PORTD = (1 << PORTD2);

	// Initialization of ADC and USART configurations
	// Found in respective SETUP_*.h files
	adc_initial_startup();
	usart_initial_startup();
	
	// Initial ADMUX adress to allow ADC-loop to run
	ADMUX = ADMUX_ADC0;
	
	// Config. used for sending character strings via USART
	FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &uart_output;
	
	while (1) 
    {
		// Start ADSC at every loop iteration
		ADCSRA |= (1 << ADSC);
		
		_delay_ms(500);
		
		  // ------------------------------------------------------------------------- //
		 // ---- If-statements to sort and store ADCH into appropriate variables ---- //
		// ------------------------------------------------------------------------- //
	   
		// If ADMUX is set to ADC0:
		if (ADMUX == ADMUX_ADC0)
		{
			// Store current ADCH into ADC0_ADCH variable
			ADC0_ADCH = ADCH;
			
			// Set ADMUX to next-in-line for conversion
			ADMUX = ADMUX_ADC1;
		}
		
		else if (ADMUX == ADMUX_ADC1)
		{
			ADC1_ADCH = ADCH;
			ADMUX = ADMUX_ADC2;
		}
		
		else if (ADMUX == ADMUX_ADC2)
		{
			ADC2_ADCH = ADCH;
			ADMUX = ADMUX_ADC0;
		}
		
		  // --------------------------------------------------------------------------- //
		 // ---- If-statements for sorting ADC0 values to 2-channel digital output ---- //
		// --------------------------------------------------------------------------- //
	   
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
		
		  // ------------------------------------------------------------------- //
		 // ---- If-statements for handling ADC1 values and related USART ----- //
		// ------------------------------------------------------------------- //
	   
		// Only (currently) used to relay information via USART
		// 3 brackets as more do not add functionality; this is purely for demonstration
		if (ADC1_ADCH < 86)
		{
			puts("Cold\r\n");
		}
		else if ((85 < ADC1_ADCH) && (ADC1_ADCH < 171))
		{
			puts("Comfy\r\n");
		}
		else if (170 < ADC1_ADCH)
		{
			puts("Hot\r\n");
		}
		
		  // --------------------------------------------------------------------- //
		 // ---- Initialize ADC2: light sensor + nightmode function handling ---- //
		// --------------------------------------------------------------------- //
		ADC2_Lightsensor(ADC2_ADCH);
    }
}

  // ---------------------------- //
 // ---- INTERRUPT + TIMER ----- //
// ---------------------------- //

ISR(INT0_vect){
	
	//Debouncing
	_delay_ms(500);
	
	//Toggle LED
	PORTB ^= (1 << PORTB0);
	
	//Enable timer and set timer to 500ms
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
 // - Func. ADC2_Lightsensor() - //
// ---------------------------- //

void ADC2_Lightsensor (uint16_t ADC_Current_Value)
{
  // ---------------------------- //
 // -------- Variables --------- //
// ---------------------------- //
	// Limits used for deciding day/night
	// 500 used as this is equivalent to dark room when sun sets
	const uint16_t ADC_Threshold_Value = 125;

	// State-machine
	uint8_t Light_Current_State;
	// Initial state is assuming it is day, as this is least intrusive
	uint8_t Light_Previous_State = 1;
	
  // ---------------------------- //
 // ------ Function code  ------ //
// ---------------------------- //

	// Initial check to compare current ADC value to threshold
	if (ADC_Current_Value > ADC_Threshold_Value)
	{
		// Day
		Light_Current_State = 1;
	}
	
	else
	{
		// Night
		Light_Current_State = 0;
	}
	
	// Checks if ADC_Current_Value has changed since last check
	// Light_Previous_State initially starts as day
	if (Light_Current_State != Light_Previous_State)
	{
		// Day
		if (Light_Current_State == 1)
		{
			// Turn off night-light and extra warmth
			PORTD = (0 << Night_Light) | (0 << Night_Warmth);
		}
		
		// Night
		else
		{
			// Turn on night-light and extra warmth
			PORTD = (1 << Night_Light) | (1 << Night_Warmth);
		}
		
		// Set previous state
		Light_Previous_State = Light_Current_State;
	}
} // End of func: ADC2_Lightsensor
