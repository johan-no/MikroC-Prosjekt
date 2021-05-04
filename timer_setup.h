/*
 * timer_setup.h
 *
 *  Author: olemarh
 *
 *	Initial configuration of timer
 *
 */


#ifndef TIMER_SETUP_H_
#define TIMER_SETUP_H_

void timer_initial_startup(void)
{
	
	// PB1/OC1A Output pin
	DDRB = (1 << PB1);
	
	// Set OC1A (bit 6) to toggle on Compare Match
	TCCR1A = (1 << COM1A0);
	
	// 1024x prescaler (set CS12 & CS10) and CTC mode with OCR1A top (mode 4 - set WGM12)
	// This gives 16e6/1024 = 15625 ticks pr. second
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	
	// Top value to give 500ms period
	// Timer = ticks pr. seconds * desired ms / 1000
	// Note that desired ms is for half the cycle, ie. 500ms = 1000ms cycle
	OCR1A = ((F_CPU / 1024) * 500) / 1000;
}



#endif /* TIMER_SETUP_H_ */