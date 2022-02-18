#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2022-02-18 19:06:04

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
uint8 AX25_getControl(frameType frameType, frameSecondaryType secondaryType, uint8 NS, uint8 NR, uint8 pollFinal);
void AX25_getInfo(uint8 * info);
void fillBuffer(uint8 *buffer, uint8 size);
void AX25_Manager(uint8* a_control);
void AX25_buildFrame(uint8 *buffer, uint8 *info, uint16 *frameSize, uint8 *ADDR,     uint8 control, uint8 infoSize) ;
uint8 AX25_deFrame(uint8 *buffer, uint16 frameSize, uint8 infoSize) ;
void AX25_buildFrame_TEST(uint8 *buffer, uint8 *info, uint8 *ADDR,     uint8 control, uint8 infoSize) ;
void setup() ;
void loop() ;

#include "firstArduino.ino"

#include "AX25_CRC.ino"
#include "ax25.ino"

#endif
