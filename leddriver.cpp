///////////////////////////////////////////////////////////////////////////////
/// LEDDRIVER.CPP
///
/// This module is responsible for the control of the LED. It contains a
/// handler that runs in response to a posted message to either turn on or
/// turn off the LED. Note the 'creative' use of the context variable - although
/// declared as a pointer it is not used as a pointer at all. Very common in
/// embedded design and device driver design, but generally makes computer
/// scientists have a fit.
///
///////////////////////////////////////////////////////////////////////////////

#include "leddriver.h"
#include "common.h"
#include "kernel.h"


// prototype the message handler function here.

void LEDControlMessageHandler(void * context);


///////////////////////////////////////////////////////////////////////////////
/// LEDInitializeDriver
///
/// This is called once at system startup. It initializes the LED driver.
/// This module is the only place where hardware related to the LED is
/// directly accessed, so here is where we set the IO parameters. We then
/// register the message handler so it picks up the messages posted from
/// the control application
///
///////////////////////////////////////////////////////////////////////////////

void LEDInitializeDriver(void)
{
	DDRB |= 0b00100000;
	Kernel::OS.MessageQueue.Subscribe(MSG_ID_CHANGE_LED, LEDControlMessageHandler);
}

///////////////////////////////////////////////////////////////////////////////
/// LEDControlMessageHandler
///
/// This function is called in response to a posted message
/// The 'context' parameter is not used as a pointer, but is cast to
/// an integer, which is used as a boolean. Zero means LED off, nonzero
/// means LED on.
///
///////////////////////////////////////////////////////////////////////////////

void LEDControlMessageHandler(void * context)
{
	if((int)context) {
		PORTB |= 0b00100000;
	} else {
		PORTB &= ~0b00100000;
	}
}

