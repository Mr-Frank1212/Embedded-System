///////////////////////////////////////////////////////////////////////////////
/// REVCOUNT.CPP
///
/// Rev counter module
///
/// Dr J A Gow 2022
///
//////////////////////////////////////////////////////////////////////////////

#include <kernel.h>
#include "revcount.h"
#include "control.h"

//
// Module variables used by interrupt context.

unsigned long currpscount=0;
unsigned long rpscount=0;

///////////////////////////////////////////////////////////////////////////////
/// REVInitialize
///
/// This is called once at system startup. The timer is configured and pin
/// change interrupts are set up
///
///
///////////////////////////////////////////////////////////////////////////////

void REVInitialize(void)
{
	// this section sets up Timer1 to give an interrupt every
	// 0.262144 seconds. We use this to count revs.
	// We have a 16MHz clock. We use the prescaler to divide by 64, then
	// the timer will overflow every 65536 cycles. With a 16MHz clock, that is
	// 0.262144 seconds.
	//
	// We then set up in CTC mode, so we can alter the resolution and get
	// the loop running faster. We know we have enough time, so we can
	// increase the sample rate a little - even by a factor of 2.

	TCCR1A=0;
	TCCR1B=0b00001011; 		// prescaler /64, set WGM12 to change resolution
	OCR1A = 0x7fff;			// half the value
	TIMSK1 = 0b00000010;	// int on capture/compare A only (clock/0xffff)

	// this next section sets up the beam-breaker so we can generate
	// an interrupt every time the beam is broken

	DDRC &= ~0b00001000;	// beam-breaker on pin PC3
	PCMSK1 |= 0b00001000;	// PCINT11 enabled
	PCICR  |= 0b00000010;	// PCIE1 enabled
}

///////////////////////////////////////////////////////////////////////////////
/// REVGetRevsPerSec
///
/// Get a calculated value of revs per second. This is not synchronized to the
/// interrupt, and is slow. It can be used for interface and display, but
/// will not be a quick solution for real-time rps control in a feedback loop.
///
/// @context: TASK
/// @scope: EXPORTED
/// @param: NONE
/// @return: unsigned long rpscount
///
///////////////////////////////////////////////////////////////////////////////

double REVGetRevsPerSec(void)
{
	return ((double)currpscount)/0.39;
}

///////////////////////////////////////////////////////////////////////////////
/// REVDisableOvfInterrupt
///
/// Disables the overflow interrupt. We use this to ensure that access to
/// input variables for the PI are atomic with respect to the PI controller
/// (which runs in interrupt context)
///
/// @context: TASK
/// @scope: EXPORTED
/// @param: NONE
/// @return: NONE
///
///////////////////////////////////////////////////////////////////////////////

void REVDisableOvfInterrupt(void)
{
	TIMSK1 &= ~0b00000010;	// int on overflow only (clock/0xffff)
}

///////////////////////////////////////////////////////////////////////////////
/// REVEnableOvfInterrupt
///
/// Enables the overflow interrupt. We use this to ensure that access to
/// input variables for the PI are atomic with respect to the PI controller
/// (which runs in interrupt context)
///
/// @context: TASK
/// @scope: EXPORTED
/// @param: NONE
/// @return: NONE
///
///////////////////////////////////////////////////////////////////////////////

void REVEnableOvfInterrupt(void)
{
	TIMSK1 |= 0b00000010;	// int on overflow only (clock/0xffff)
}

///////////////////////////////////////////////////////////////////////////////
/// ISR - Timer 1 overflow.
///
/// This ISR is triggered every time the timer 1 overflow wraps around
/// THIS IS OUR SAMPLE EVENT INTERRUPT.
///
/// @context: INTERRUPT
/// @scope: INTERNAL
///
///////////////////////////////////////////////////////////////////////////////

ISR(TIMER1_COMPA_vect)
{
	OCR1A = 0x7fff;			// half the value

	// if this is called, we need to count the number of pin-change
	// interrupts we have, and save this in the currpm global.
	currpscount=rpscount;
	rpscount=0;

	CTRLPILoop(REVGetRevsPerSec());
}

///////////////////////////////////////////////////////////////////////////////
/// REVInterruptHandler
///
/// This ISR is triggered when the beam is brkoen
///
/// @context: INTERRUPT
/// @scope: INTERNAL
///
///////////////////////////////////////////////////////////////////////////////

void REVInterruptHandler(void)
{
	// we know we have a valid interrupt from the beam break.
	rpscount++;
}
