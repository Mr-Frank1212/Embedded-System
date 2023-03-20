///////////////////////////////////////////////////////////////////////////////
/// PWM.CPP
///
/// PWM module
///
/// Dr J A Gow 2022
///
//////////////////////////////////////////////////////////////////////////////

#include <kernel.h>
#include "pwm.h"

///////////////////////////////////////////////////////////////////////////////
/// PWMInitialize
///
/// This is called once at system startup. This function sets the PWM function
/// of Timer0. The core libraries use Timer0 to produce a 1ms timer tick - it
/// is easy to extend this to generate 8-bit PWM with a 1ms period and a period
/// register range of 0-0xff
///
///////////////////////////////////////////////////////////////////////////////

void PWMInitialize(void)
{
	DDRD|=0b01000000;		// PD6 to output
	TCCR0A |= 0b10000011;	// Clear on match, fast pwm mode
	OCR0A = 0;				// set duty to zero on init
}

///////////////////////////////////////////////////////////////////////////////
/// PWMSetDuty
///
/// Set the duty cycle. The argument is an unsigned char, yielding duty
/// between 0 (0x00) and 1 (0xff)
///
/// @scope: EXPORTED
/// @context: ANY
/// @param: unsigned char duty - value of duty between 0x00 and 0xff
/// @return: none
///
///////////////////////////////////////////////////////////////////////////////

void PWMSetDuty(unsigned char duty)
{
	OCR0A=duty;
}
