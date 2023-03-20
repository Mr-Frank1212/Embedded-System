////////////////////////////////////////////////////////////////////////////////
/// DISPLAY.H
///
/// Display module
///
/// Dr J A Gow / Dr M A Oliver 2022
///
////////////////////////////////////////////////////////////////////////////////

#include "display.h"
#include "common.h"
#include <kernel.h>
#include <LiquidCrystal_I2C.h>


//
// This enum defines the states used by the state machine

typedef enum _DISPSTATE {

	DISPSTATE_REFSH,
	DISPSTATE_IDLE,
	DISPSTATE_UPDATING,
	DISPSTATE_VALIDATE,
	DISPSTATE_ERROR

} DISPSTATE;


// One module-wide instance of the display object, as declared in the
// library.

LiquidCrystal_I2C lcd(DISP_I2C_ADDR,16,2);

// Two module variables containing the demanded and
// actual RPM to display
static unsigned int ActualRPS = 0;
static unsigned int DemandRPS = 0;

// Another module variable contains the unvalidated
// entered RPM value.
static unsigned int EnteredRPS = 0;	// value entered

// The character sequence entered by the user. We keep this because
// it is needed in both task and message handler
static char numarr[5];

// Display state variable
DISPSTATE state = DISPSTATE_REFSH;

// Prototype of display task functions

void DISPTask(void * context);			// display task handler
void DISPUpdateRPS(void * context);		// message handler for actual RPM updates
void DISPUpdateDemandRPS(void * context);	// message handler for demand RPM updates
void DISPKeyPressed(void * context);		// keypad update pressed

////////////////////////////////////////////////////////////////////////////////
/// DISPInitialize
///
/// Initialize the display
///
/// @scope: EXPORTED
/// @context: TASK
/// @param: none
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPInitialize(void)
{
  // Preliminary setup
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Starting.."));

  Kernel::OS.MessageQueue.Subscribe(MSG_ID_NEW_ACTUAL_RPS,DISPUpdateRPS); //DISPUpdateRPS() mapped against MSG_ID_NEW_ACTUAL_RPS
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_KEY_PRESSED,DISPKeyPressed); //DISPKeyPressed() mapped against MSG_ID_KEY_PRESSED
  Kernel::OS.MessageQueue.Subscribe(MSG_ID_NEW_DEMAND_RPS,DISPUpdateDemandRPS); //DISPUpdateDemandRPS() mapped against MSG_ID_NEW_DEMAND_RPS

  Kernel::OS.TaskManager.RegisterTaskHandler(DISPTask,(void *)NULL); // Register the task for the display
}

////////////////////////////////////////////////////////////////////////////////
/// DISPTask
///
/// Main task for the display module
///
/// @scope: INTERNAL
/// @context: TASK
/// @param: none (context is null)
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPTask(void * context)
{
	static Kernel::OSTimer *errtimer;	// timeout for error.
	switch(state) {

		case DISPSTATE_REFSH:		
        
        //Displays the current "ActualRPS" value on the first line
        char act[3];
        sprintf(act,"%3.3d",ActualRPS);
        lcd.setCursor(0,0);
        lcd.print(F("Actual RPS:"));
        lcd.setCursor(12,0);
        lcd.print(act);
        
        //Displays the current "DemandRPS" value on the second line
        char dem[3];
        sprintf(dem,"%3.3d",DemandRPS);
        lcd.setCursor(0,1);
        lcd.print(F("Demand RPS:"));
        lcd.setCursor(12,1);
        lcd.print(dem);
        
			break;

		case DISPSTATE_IDLE:
		case DISPSTATE_UPDATING:	
		  // do nothing. We only update when messages arrive asking us to.
			break;

		case DISPSTATE_VALIDATE:
		    //Checks if EnteredRPS is valid
		    //EnteredRPS is valid if it is within RPS_MIN and RPS_MAX and is not equal to zero
			  if(EnteredRPS > RPS_MAX || EnteredRPS < RPS_MIN && (EnteredRPS != 0)){
			    errtimer=new Kernel::OSTimer(2000);                 // starts the errtimer of 2sec if the above two conditions are met
				  errtimer->Set(2000);
          lcd.setCursor(2,1);
          lcd.print(F("INVALID RPS"));					              // displays an error message, showing the invalidity of the EnteredRPS
			    state=DISPSTATE_ERROR;                              // change state to DISPSTATE_ERROR
			    }
        else {Kernel::OS.MessageQueue.Post(MSG_ID_NEW_RPS_KEYPAD, (void *)EnteredRPS, Kernel::MQ_OWNER_CALLER, Kernel::MQ_CONTEXT_TASK);
          lcd.clear();
          state=DISPSTATE_REFSH;
          }
			break;

		case DISPSTATE_ERROR:		
		  if(errtimer->isExpired()) {                           //checks is errTimer is expired
				delete errtimer;                                    
        char tem[5];
        lcd.clear();                                        //clears display
        sprintf(tem,"%3.3d",EnteredRPS);                    //saves the enteredRPS in %3.3d format into 'tem' variable
        lcd.setCursor(0,0);                                 //Resets the cursor
        lcd.print(F("RE-SET:"));                            //prints "RE-SET"
        lcd.setCursor(9,0);                                 //sets cursor at position to display the EnteredRPS value
        lcd.print(tem);                                     
        lcd.setCursor(9,0);
        lcd.blink();                                        //place the cursor at the first number of the EnteredRPS
       
				state=DISPSTATE_UPDATING;                           // Return to updating state in hope that a valid Demand RPM may be entered
      }
		  break;

		// a catch-all, we should never get here.
		default: 					
		  state=DISPSTATE_IDLE;
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
/// DISPUpdateRPM
///
/// Responds to messages coming in with an updated actual RPM. We
/// check if there is any change, and if so, display it. We can only
/// do this if in DISPSTATE_IDLE
///
/// @context: TASK
/// @scope: INTERNAL
/// @param: void * context - RPM value cast to unsigned int
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPUpdateRPS(void * context)
{
	unsigned int newrps=(unsigned int)context;

	// NOTE: The display is slow. We may thus want to
	// control the update rate, and not update every time
	// a change is made.
  
	if(state==DISPSTATE_IDLE || state==DISPSTATE_REFSH) {
		// we only update the display if we need to.
		if(newrps!=ActualRPS) {
			ActualRPS=newrps;
			char tempstr[6];
			sprintf(tempstr,"%3.3d",ActualRPS);
			lcd.setCursor(12,1);
			lcd.print(tempstr);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
/// DISPUpdateDemandRPM
///
/// Responds to messages coming in to change the displayed demand RPM. We
/// check if there is any change, and if so, display it. We can only
/// do this if in DISPSTATE_IDLE
///
/// @context: TASK
/// @scope: INTERNAL
/// @param: void * context - RPM value cast to unsigned int
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPUpdateDemandRPS(void * context)
{
	unsigned int newrps=(unsigned int)context;
  if(state==DISPSTATE_IDLE || state==DISPSTATE_REFSH) {     //This update function is ran only when the program is in the DISPSTATE_IDLE or DISPSTATE_REFSH state                                                           
    if(newrps!=DemandRPS) {                                 // checks if new input is same with old DemandRPS value
      DemandRPS=newrps;                                     // update the DemandRPS value to the new input
      char tempstrr[6];                                       
      sprintf(tempstrr,"%3.3d",DemandRPS);                  //saves the DemandRPS in %3.3d format into 'tempstrr' variable
      lcd.setCursor(12,0);
      lcd.print(tempstrr);                                  //Displays the updated DemandRPS
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
/// DISPKeyPressed
///
/// Someone has pressed a button. We need to read the user's value, and
/// deal with it according to state.
///
/// @context:  TASK
/// @scope: INTERNAL
/// @param: void * context: encoded value of key pressed as unsigned char
/// @return: none
///
////////////////////////////////////////////////////////////////////////////////

void DISPKeyPressed(void * context)
{
	unsigned char keyval=(unsigned char)context;
	static unsigned int curpos=9;
  unsigned char old;
	switch(state) {

		case DISPSTATE_IDLE:
		case DISPSTATE_REFSH:		// if the key is anything but a numeral, ignore it
		   if(state==DISPSTATE_IDLE || state==DISPSTATE_REFSH){
		    if(keyval<0x0a) {
				  // This is the first press. Set up the display:
				  curpos=9;
				  sprintf(numarr,"%3.3d",DemandRPS);
    			numarr[0]=0x30+keyval;
    			lcd.clear();
    			lcd.setCursor(0,0);
    			lcd.print(F("New RPS:"));
    			lcd.setCursor(curpos,0);
    			lcd.print(numarr);
    			lcd.setCursor(++curpos,0);				
    			lcd.blink();
          
    			state=DISPSTATE_UPDATING;}}
    	  break;
    		
		case DISPSTATE_UPDATING:
        if(keyval<0x0a){                    //executes if the keypad press is a number (less than 10)
          if(curpos <12){                   //stops input from keypad if the values inputted are already 3
            old=13;                         //resets to a dp value   
            if(old != keyval){              //executes a new key is pressed
              old=keyval;                   //save the current keypress in variable old
              numarr[curpos-9]=0x30+old;    //save the keypress in ascii into the next index of the array
              lcd.print(old);               //displays the key pressed 
              lcd.setCursor(++curpos,0);    // sets cursor in the next position awaiting input
              lcd.blink();                            
              }}}
       
        else if(keyval==0x0a && curpos!=9){ // executes if the key pressed is an (*) and the cursor isn't at the beginning of the input 
              --curpos;            // takes the cursor back by a step
              lcd.setCursor(curpos,0); 
              lcd.blink();
              state=DISPSTATE_UPDATING;     //remains in state for figures of RPM to be set
              break;}

        if(keyval==0x0b){                   //checks if a (#) was pressed
              lcd.noCursor();
              lcd.noBlink();
              curpos=9;                     // resets cursor position variable back to first digit
              sscanf(numarr,"%d",&EnteredRPS); //saves the new RPS value into EnteredRPS, to be validated in the next state
              state=DISPSTATE_VALIDATE;       // change state to DISPSTATE_VALIDATE for validation of figure
              break;}
               
              
      // TODO: Add the code to deal with Enter (#),
      // Backspace (*) and subsequent 0-9 keypresses.
        
		  

		// in all other states, we take no action if a key is pressed.

		default:					break;
	
  }
}
