///////////////////////////////////////////////////////////////////////////////
/// CONTROL.H
///
/// Control module. This passes information to the LED driver (and in theory
/// could be to anywhere else) via the message queue
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _CONTROL_H_
#define _CONTROL_H_

//
// coefficients of PI. These can be arbitrary for the speed test

#define PI_A1	0.04
#define PI_A0	0.01

///////////////////////////////////////////////////////////////////////////////
/// CONTROLInitialize
///
/// This is called once at system startup. It initializes the control
/// module (by registering a task to run repetitively) and then exits. This
/// function needs to be seen outside the module, so it is prototyped in the
/// header file.
///
///////////////////////////////////////////////////////////////////////////////

void CONTROLInitialize(void);

/////////////////////////////////////////////////////////////////////////////
/// CTRLPILoop
///
/// A simple PI implementation using floating point arithmetic. This is to
/// demonstrate the slow speed of operation when using software floating
/// point
///
/// Note that this is called in interrupt context - be careful to ensure
/// atomicity of the input (rpm)
///
/// This function will update the PWM from within
///
/// @context: INTERRUPT
/// @scope: EXPORTED
/// @param: double actualrpsin - the measured RPS
/// @return: none
///
/////////////////////////////////////////////////////////////////////////////

void CTRLPILoop(double actualrpsin);

#endif
