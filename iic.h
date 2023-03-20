///////////////////////////////////////////////////////////////////////////////
/// IIC.H
///
/// IIC peripheral driver for ATMega328p
///
/// Dr J A Gow / Dr M A Oliver 2022
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _IIC_H_
#define _IIC_H_

//
// Exported functions

///////////////////////////////////////////////////////////////////////////////
/// IICInitialize
///
/// Initialize the IIC subsystem
///
/// @scope: EXPORTED
/// @context: TASK
/// @param: NONE
/// @return: NONE
///
///////////////////////////////////////////////////////////////////////////////

void IICInitialize(void);

///////////////////////////////////////////////////////////////////////////////
/// IICWrite
///
/// Write a string of data bytes to the IIC address
///
/// @scope: INTERNAL
/// @context: TASK
/// @param: addr - unsigned char. Address. Top 7 bits used
/// @param: dbyte - pointer to unsigned char. Data to send
/// @param: nToSend - number of bytes to send.
///
///////////////////////////////////////////////////////////////////////////////

int IICWrite(unsigned char addr,unsigned char * dbytes, unsigned int nToSend);

///////////////////////////////////////////////////////////////////////////////
/// IICRead
///
/// Read multiple bytes of data from the IIC address
///
/// @scope: INTERNAL
/// @context: TASK
/// @param: addr - unsigned char. Address. Top 7 bits used
/// @param: dbytes - unsigned char * Pointer to buffer big enough to receive
///                  data
/// @param: nToRecv - number of bytes to receive
///
///////////////////////////////////////////////////////////////////////////////

int IICRead(unsigned char addr,unsigned char * dbytes, unsigned int nToRecv);

#endif
