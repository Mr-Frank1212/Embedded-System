///////////////////////////////////////////////////////////////////////////////
/// LEDDRIVER.H
///
/// This module is responsible for the control of the LED. It contains a
/// handler that runs in response to a posted message to either turn on or
/// turn off the LED. Note the 'creative' use of the context variable - although
/// declared as a pointer it is not used as a pointer at all. Very common in
/// embedded design and device driver design, but generally makes computer
/// scientists have a fit.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _LEDDRIVER_H_
#define _LEDDRIVER_H_


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

void LEDInitializeDriver(void);


#endif
