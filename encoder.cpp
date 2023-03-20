///////////////////////////////////////////////////////////////////////////////
/// ENCODER.H
///
/// This module manages the rotary encoder. It is interrupt-driven rather
/// than polled and provides a good demonstration of how to use the pin change
/// interrupt to capture a real time signal
///
/// Dr J A Gow / Dr M A Oliver 2022
///
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <kernel.h>
#include "encoder.h"
#include "common.h"

/////////////////////////////
/// Exported functions
/////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// ENCInitialize
///
/// Initialize the rotary encoder. This function will enable the relevant
/// pin change interrupt, so the message queue should be initialized first
///
/// @scope: EXPORTED
/// @context: TASK
/// @param: none
/// @return: none
///
///////////////////////////////////////////////////////////////////////////////

void ENCInitialize(void)
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

	DDRC &= ~0b00000110; // both set to inputs

	// now set up the pin change ints on A1. We use pin change interrupt 0,
	// as we will use PCINT1 later for the motor speed encoder.

	PCMSK1 |= 0b00000010;	// PC9
	PCICR  |= 0b00000010;	// enable PCI1

  // Unfortunately the beam-breaker tacho shares the same PCI as the rotary
  // encoder. So both devices will eventually share the same ISR. The ISR will
  // have to do extra checking to work out which device raised the interrupt.
  // This is not ideal as the ISR needs to be as lean as possible.
}


///////////////////////////////////////////////////////////////////////////////
/// ISR(PCINT1_vect)
///
/// Interrupt Service Routine: Handles pin change 1 interrupt
///
/// @scope: INTERNAL
/// @context: INTERRUPT
/// @param: none
/// @return: none
///
///////////////////////////////////////////////////////////////////////////////

void ENCInterruptHandler(void)
{
	// This interrupt is set a change of PC9. 
	
	// TODO: If the state of PC1 is high we deal with this with the following:-
  //       If the state of PC2 is high, this means anticlockwise
  //       - so send the value -1 to the Encoder Message ID.
  //       If the state of PC2 is low, this means clockwise
  //       - so send the value +1 to the Encoder Message ID.
  
  if (PINC & 0b00000010){               // executes if PC1 is high
    if (PINC & 0b00000100){             //post -1 to MSG_ID_ENCODER if PORTC pin 2 is HIGH (anti-clockwise) and +1 to MSG_ID_ENCODER if PORTC pin 2 is LOW (CLockwise)
      Kernel::OS.MessageQueue.Post(MSG_ID_ENCODER, (void * )-1, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
    }
    else{
      Kernel::OS.MessageQueue.Post(MSG_ID_ENCODER, (void * )1, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
    }
  }
	// done. We really do need this to be as short as possible.
}
