///////////////////////////////////////////////////////////////////////////////
/// SSEGDRIVER.CPP
///
/// This is the driver for the seven segment display. It is a shell - you
/// should populate with your own code
///
///////////////////////////////////////////////////////////////////////////////

#include "ssegdriver.h"
#include "common.h"
#include "kernel.h"

// prototype the message handler function here.

void SSEGControlMessageHandler(void * context);

byte seven_seg[] = {
  0b11000000, //Displays "0"
  0b11001111, //Displays "1"
  0b10100100, //Displays "2"
  0b10110000, //Displays "3"
  0b10011001, //Displays "4"
  0b10010010, //Displays "5"
  0b10000010, //Displays "6"
  0b11111000, //Displays "7"
  0b10000000, //Displays "8"
  0b10011000, //Displays "9"
  0b10000011, //Displays "b"
  0b10000110, //Displays "E"
  0b01111111, //blank with dp
  0b01111111, //blank with dp
  0b01111111, //blank with dp
  0b01111111, //blank with dp
};

// array mapping values to segments - you may need something like that here


///////////////////////////////////////////////////////////////////////////////
/// SSEGInitializeDriver
///
/// This is called once at system startup. It initializes the seven segment
/// display driver.
/// This module is the only place where hardware related to the seven segment
/// display is directly accessed, so here is where we set the IO parameters.
///
///////////////////////////////////////////////////////////////////////////////

void SSEGInitializeDriver(void)
{
  // This is the code that configures the pins on the ATMega328 as
  // outputs WITHOUT CHANGING OTHER PINS (note the use of bitwise-OR)

  // DATA (SER on HC595) is on PORTD bit 4
  // CLK  (SRCLK on HC595) is on PORTB bit 0
  // EN   (ECLK on HC595) is on PORTD bit 7

  DDRD |= 0b10010000;	// set to o/p
  DDRB |= 0b00000001; // set to o/p

  // set EN high, data low, clock low (as initial)

  PORTD |= 0b10000000;
  PORTB &= ~0b00000001;
  PORTD &= ~0b00010000;

  // This line registers the message handler with the OS in order
  // that it can receive messages posted to it under the MSG_ID_CHANGE_7SEG
  // ID

  Kernel::OS.MessageQueue.Subscribe(MSG_ID_CHANGE_7SEG, SSEGControlMessageHandler);
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

void SSEGControlMessageHandler(void * context)
{
  unsigned int value = (unsigned int)context;
  byte mask;                           ///Declare byte(8-bit) variable called "mask"
  mask = 0b10000000;                   //Most significant bit of "mask" set to 'High'
  PORTD &= ~0b10000000;                //EN set 'LOW',awaiting information from data line
  int i;
  unsigned int index; 
  
 if (value & 0b00010000){                      // ()resets the "num" for the second count and turns the decimal point "ON". (when the 4th bit comes on)
    index = value-16;
    seven_seg[index] &= ~0b10000000;}    
    
 else if (value > 11){                 //Turns "ON" the dp during blanks (12 to 15)
    seven_seg[index] &= ~0b10000000;}
 
  
 else { index = value;                   //Turns "OFF" dp for anyother counts
    seven_seg[index] |= 0b10000000;
  }
  
  for (i = 0; i < 8; ++i) {            //"For" loop to check each bit 
    bool b = seven_seg[index] & mask;

    if ( b == LOW ) {
      PORTD &= ~0b00010000;            //data line "LOW" if 'b' is "lOW"
    }         
    else {
      PORTD |= 0b00010000;             //data line "HIGH" if 'b' is "HIGH"

    }         
    PORTB |= 0b00000001;               //clock line "HIGH"
    mask = mask >> 1;                  //Right-shift the mask by a bit
    PORTB &= ~0b0000001;               //clock line "LOW"
    
  }
  PORTD |= 0b10000000;                 //latch line "HIGH"
}
/// your code goes here, as well as above if you ned to declare statics
/// or module globals.

// Remember that the logic sense of the outputs of the port expander
// are INVERTED: a logic '0' will light the segment
