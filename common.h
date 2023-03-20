///////////////////////////////////////////////////////////////////////////////
/// COMMON.H
///
/// This file contains common definitions (NOT code) that may be needed
/// across modules
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _COMMON_H_
#define _COMMON_H_

// We have 26 possible individual message IDs. It is up to the user to define them

#define MSG_ID_CHANGE_LED	0
#define MSG_ID_CHANGE_7SEG	1
#define MSG_ID_KEY_PRESSED  2
#define MSG_ID_KEY_RELEASED  3  
#define MSG_ID_NEW_ACTUAL_RPS  4
#define MSG_ID_NEW_DEMAND_RPS  5
#define MSG_ID_NEW_RPS_KEYPAD  6
#define RPS_MIN 20
#define RPS_MAX 300
#define MSG_ID_ENCODER 9


#endif
