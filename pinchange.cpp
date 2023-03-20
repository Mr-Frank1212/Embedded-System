///////////////////////////////////////////////////////////////////////////////
/// PINCHANGE.H
///
/// Module servicing the pin change interrupt. This is a single interrupt
/// that services multiple modules - so it is handled here by the ISR and
/// dispatched then to the modules that need it.
///
/// Dr J A Gow 2022
///
//////////////////////////////////////////////////////////////////////////////

#include <kernel.h>
#include "pinchange.h"
#include "encoder.h"
#include "revcount.h"

static unsigned char lastPinC=0;

///////////////////////////////////////////////////////////////////////////////
/// PINInitialize
///
/// This is called once at system startup. This function enables the pin change
/// functionality and enables the interrupt associated with it
///
///////////////////////////////////////////////////////////////////////////////

void PINInitialize(void)
{
	// INC ENC A (on schematic) is attached to A1 -> PORTC bit 1
	// INC ENC B (on schematic) is attached to A2 -> PORTC bit 2
	//
	// According to ATMega328P datasheet, PC1 -> PCINT9
	//                                    PC2 -> PCINT10
	//
	// We need to enable both as inputs, but we only need to enable
	// the pin change interrupt on one of the two, as the
	// encoder works with relative phase.
	//
	// The beam breaker is on PC3


	// For timing only
	DDRD |= 0b00000100;	// Port D bit 2 is broken out to a test point.


	DDRC &= ~0b00001110; // PC1, PC2 and PC3 set to inputs

	// now set up the pin change ints on A1. We use pin change interrupt 0,
	// as this is used by both encoder and beam-breaker tacho

	PCMSK1 |= 0b00001010;	// Interrupts on PC3 (tacho) and PC1 (encoder)
}

///////////////////////////////////////////////////////////////////////////////
/// ISR - Pin change interrupt
///
/// This ISR is triggered when the beam is brkoen
///
/// @context: INTERRUPT
/// @scope: INTERNAL
///
///////////////////////////////////////////////////////////////////////////////

ISR(PCINT1_vect)
{
	// Ok. The problem here is that the rotary encoder is on the same
	// interrupt as the beam-breaker. We need to discriminate. So we
	// look for a positive edge change over the previous value of the pin.
	PORTD |= 0b00000100;	//PD2 on

	if(PINC&0b00001000 && !(lastPinC&0b00001000)) {
		REVInterruptHandler();
	}

	if(PINC&0b00000010 && !(lastPinC&0b00000010)) {
		ENCInterruptHandler();
	}

	lastPinC=PINC&0b00001010;
	PORTD &= ~0b00000100;	//PD2 off

}
