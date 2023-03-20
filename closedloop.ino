///////////////////////////////////////////////////////////////
//  
//  NAME: EBUBE FRANKLYN EMELIE
//  DEPT: ELECTRONICS ENGINEERING
//  EMBEDDED SYSTEMS ENGT-5154
//  CLOSED LOOP CONTROL
//  ID NO: 2709792
//  DATE: 13/05/2022
//
///////////////////////////////////////////////////////////////////////////////
/// LedDemo
///
/// This is the main module showing an encapsulated LED driver.
///
/// The module containing the LED driver itself is in led.h/led.cpp
/// The module containing the controller that passes messages to the LED to
/// change its state is in control.h/control.cpp
///
///////////////////////////////////////////////////////////////////////////////

#include "kernel.h"
#include "leddriver.h"
#include "ssegdriver.h"
#include "display.h"
#include "keypad.h"
#include "control.h"
#include "iic.h"
#include "encoder.h"
#include "pinchange.h"
#include "pwm.h"
#include "revcount.h"

//////////////////////////////////////////////////////////////////////////////
/// UserInit
///
/// This function is called once by the kernel on startup
///
/// We use it to initialize our modules
///
/////////////////////////////////////////////////////////////////////////////

void UserInit()
{
	// Order may be important - always check your code.
	IICInitialize();
	LEDInitializeDriver();
	SSEGInitializeDriver(); 
  DISPInitialize();
  REVInitialize();
  PWMInitialize();
  PINInitialize();
  KEYInitializeKeypad();
  ENCInitialize();
  CONTROLInitialize();
}
