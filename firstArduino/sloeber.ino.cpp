#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2022-03-04 15:19:54

#include "Arduino.h"
#include "AX25_CRC.h"
#include <stdio.h>
#include <stdlib.h>
#include "ax25.h"
#include "AX25_CRC.h"
#include "Arduino.h"
#include <stdlib.h>
#include <stdio.h>
#include "ax25.h"

uint16 computeCRC( uint8* data_p, uint16 *length) ;
void AX25_putCRC(uint8 *buffer, uint16 *OpArrSize) ;
uint8 AX25_getControl(frameType frameType, frameSecondaryType secondaryType, 		uint8 NS, uint8 NR, uint8 pollFinal) ;
void fillBuffer(uint8 *buffer, uint8 size) ;
void incrementStateVar(uint8 *stateVar) ;
void AX25_Manager(uint8 *a_control) ;
void AX25_buildFrame(uint8 *buffer, uint8 *a_info_ptr, uint16 *frameSize, 		uint8 *ADDR, uint8 control, uint8 infoSize) ;
void AX25_deFrame(uint8 *buffer, uint16 frameSize, uint8 infoSize) ;
void serialFlush() ;
void print_info() ;
void printSerialTXBufferToSerial() ;
void readFrameFromSerial() ;
void setup() ;
void loop() ;

#include "firstArduino.ino"

#include "AX25_CRC.ino"
#include "ax25.ino"

#endif
