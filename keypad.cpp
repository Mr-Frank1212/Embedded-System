///////////////////////////////////////////////////////////////////////////////
/// KEYPAD.CPP
///
/// Keyboard module
///
/// Dr J A Gow / Dr M A Oliver 2022
///
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "keypad.h"
#include "kernel.h"
#include "iic.h"

#define KEY_ADDR_IIC	0x40

//
// This enum defines the states used by the state machine

typedef enum _KEYSTATE {

	KEY_IDLE,
	KEY_PRESSDETECTED,
	KEY_PRESSED,
	KEY_RELEASEDETECTED

} KEYSTATE;

// The single task timer used in this module

static Kernel::OSTimer KeyTimer(10);
//
// Forward definition of keypad task handler

void KEYTaskHandler(void * context);

//
// Exported functions

///////////////////////////////////////////////////////////////////////////////
/// KEYInitializeKeypad
///
/// This is called once at system startup. It initializes the keypad driver.
/// This module is the only place where hardware related to the keypad is
/// directly accessed, so here is where we set the IO parameters. We then
/// register the message handler so it picks up the messages posted from
/// the control application
///
///////////////////////////////////////////////////////////////////////////////

void KEYInitializeKeypad(void)
{
	unsigned char iicreg[2]; // space to put our required I2C data in.

	// Configure the port expander. We want GPIA0,1 and 2 as outputs
	// We also need GPIA3-7 as inputs. We can then usefully construct
	// these into a byte we only need to read once.

	iicreg[0]=0x00;		// IODIRA
	iicreg[1]=0xf8;		// bottom 3 pins output
	IICWrite(KEY_ADDR_IIC,iicreg,2);

	// Now, to start with we want only the LSB low (remember
	// keypad has reverse logic because unfortunately the hardware designer
	// slipped up and pulled EVERYTHING high. If you design hardware, be
	// sympathetic to your firmware designers!

	iicreg[0]=0x12;		// GPIOA
	iicreg[1]=0x06;		// bottommost bit zero
	IICWrite(KEY_ADDR_IIC,iicreg,2);

	// Now, because the hardware designer pulled everything
	// high and used inverse logic, we set the relevant bits in
	// the IPOLA register to put it back to rights

	iicreg[0]=0x02;
	iicreg[1]=0b01111000;
	IICWrite(KEY_ADDR_IIC,iicreg,2);

	// Register the task handler. We do not need to pass any context
	// as in this module, our timer is declared with the scope limited
	// to this module
  Kernel::OS.TaskManager.RegisterTaskHandler(KEYTaskHandler,(void *)NULL);
}

//////////////////////////////////////////////////////////////////////////////
/// KEYTaskHandler
///
/// This is our main task handler for the keypad. It does this in a polled
/// mode. It simply uses a delay and a state machine to debounce, with a map
/// to translate keys to values
///
/// The 'context' parameter is unused in this function
///
//////////////////////////////////////////////////////////////////////////////

void KEYTaskHandler(void * context)
{
	unsigned char matrix;					        
	unsigned char iicreg[2];				      // IIC data buffer
	static unsigned char lastpressed;		  // needs to be remembered across calls to KEYTaskHandler
	static KEYSTATE keystate = KEY_IDLE;	// needs to hold state across calls to KEYTaskHandler
  static unsigned int numberToDisplay;
  static unsigned int lastValueDisplayed;
	
  
  // The first thing we need to do is read back the port value
	// We can't use IIC repeat starts here because we don't know if anything
	// else will want to use I2C in the meantime, between calls to this task
  // Following this operation, the value of Port A will be stored in the variable 'matrix'
	iicreg[0]=0x12;						        // GPIOA register address
	IICWrite(KEY_ADDR_IIC,iicreg,1);	// write the address.
	IICRead(KEY_ADDR_IIC,&matrix,1);	// read the value
    
  switch (lastpressed) {            // A switch case system to represent the different keypad buttons and expected output                       
                        case 0b01000110:numberToDisplay = 3;break;
                        case 0b00100110:numberToDisplay = 6;break;
                        case 0b00010110:numberToDisplay = 9;break;
                        case 0b01000011:numberToDisplay = 1;break;
                        case 0b01000101:numberToDisplay = 2;break;
                        case 0b00100011:numberToDisplay = 4;break;
                        case 0b00100101:numberToDisplay = 5;break;
                        case 0b00010011:numberToDisplay = 7;break;
                        case 0b00010101:numberToDisplay = 8;break;
                        case 0b00001101:numberToDisplay = 0x00;break;
                        case 0b00001011:numberToDisplay = 0x0a;break;       
                        case 0b00001110:numberToDisplay = 0x0b;break;}
    
	
	switch(keystate) {                                       // check the state machine
		case KEY_IDLE:
      if (matrix & 0b01111000){                            // check if a keypad has been pressed
        KeyTimer=new Kernel::OSTimer(10);                  // starts a 10ms debounce timer
        KeyTimer.Set(10);
        lastpressed = matrix;                              // save the current matrix of key pressed
        keystate = KEY_PRESSDETECTED;}                     // change state to KEY_PRESSDETECTED 
    
      else {unsigned int col = (matrix<<1)|0b00000001;     //if no keypress was detected, move to next column
        if ((col & 0b00000111)==0b00000111){               
          col = 0b00000110;}                               //reset to check 'column 1' after checking 'column 3'
      
			  iicreg[1]=col;                                     // Then write the column to the I2C
			IICWrite(KEY_ADDR_IIC,iicreg,2);}
      break;
		
	  case KEY_PRESSDETECTED:
         
      if(KeyTimer.isExpired()) {                          // confirm the keypress after debounce timer expores
         if (lastpressed == matrix){
         Kernel::OS.MessageQueue.Post(MSG_ID_CHANGE_7SEG, (void *)numberToDisplay, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK); // post keypress to MSG_ID_CHANGE_7SEG
         Kernel::OS.MessageQueue.Post(MSG_ID_KEY_PRESSED, (void *)numberToDisplay, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK); // post keypress to MSG_ID_KEY_PRESSED
         lastValueDisplayed=numberToDisplay;              // save the last value of the keypress
         keystate = KEY_PRESSED;}                         // Change state to KEY_PRESSED
            
         else{ keystate = KEY_IDLE;                       //Return to state KEY_IDLE if the keypress wasn't debounced
         }
      }   
      break;

		case KEY_PRESSED:	                                    // check for a key release.  
        if (lastpressed != matrix){                       //checks prvious keypress with current keypress state
           unsigned int dp = 0x0c;                        //post a decimal point value to the MSG_ID_CHANGE_7SEG and MSG_ID_KEY_RELEASED  
           Kernel::OS.MessageQueue.Post(MSG_ID_KEY_RELEASED, (void *)lastValueDisplayed, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK); //numberToDisplay
           Kernel::OS.MessageQueue.Post(MSG_ID_CHANGE_7SEG, (void *)dp, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
           keystate = KEY_IDLE;}                          //change state to KEY_IDLE to await next press
    
        else{ keystate = KEY_PRESSED;}                    //remain in KEY_PRESSED if keypress hasn't been released
                
        break;
      

		default:			
      // catch-all. We should never get here
		  // but this gives us belt and braces.
			keystate=KEY_IDLE;
			break;
	}
}
