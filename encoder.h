///////////////////////////////////////////////////////////////////////////////
/// ENCODER.H
///
/// This module manages the rotary encoder. It is interrupt-driven rather
/// than polled and provides a good demonstration of how to use the pin change
/// interrupt to capture a real time signal
///
/// Dr J A Gow 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef ENCODER_H_
#define ENCODER_H_

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

void ENCInitialize(void);
void ENCInterruptHandler(void);


#endif
