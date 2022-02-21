/*
 * main.c
 *
 *  Created on: Oct 8, 2021
 *      Author: Mohamed Ezzat
 */
#include "Arduino.h"
#include <stdlib.h>
#include <stdio.h>

#include "ax25.h"

#define rxDebug_IFRAME
//#define rxDebug_SFRAME

uint8 SerialTXBuffer[AX25_FRAME_MAX_SIZE];
uint8 SerialRXBuffer[AX25_FRAME_MAX_SIZE];
uint8 info[SSP_FRAME_MAX_SIZE];
uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U', 'F',
    'T', 'I', '1', 0x61 };



uint8 flag_SSP_to_Control = EMPTY;
uint8 flag_Control_to_Framing = EMPTY;
uint8 flag_Control_to_SSP = EMPTY;
uint8 flag_Deframing_to_Control = EMPTY;
uint8 flag_SerialTXBuffer = EMPTY;
uint8 flag_SerialRXBuffer = EMPTY;
uint8 flag_next_frame = EMPTY;

uint8 g_infoSize = 236; //temp set as 236


//extern uint8 flag_TX;
//extern uint8 flag_RX;
//extern uint8 flag_busy;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  /* if we connect as RX remove this part */

  if (flag_SSP_to_Control == EMPTY) {
     AX25_getInfo(info);
     flag_SSP_to_Control = FULL;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

      uint8 AddressReadyFlag = EMPTY;
  uint8 ControlReadyFlag = EMPTY;
//  uint8 infoReadyFlag = EMPTY;
  uint8 FCSReadyFlag = EMPTY;


  uint8 control;
  uint16 frameSize = 0;
    //  uint8 NR=0;
#ifdef rxDebug_SFRAME
  if (flag_SSP_to_Control == EMPTY) {
     AX25_getInfo(info);
     flag_SSP_to_Control = FULL;
  }
#endif
  delay(100);

  if(flag_next_frame == FULL){
	  if (flag_SSP_to_Control == EMPTY) {
	     AX25_getInfo(info);
	     flag_SSP_to_Control = FULL;
	  }
	  flag_next_frame = EMPTY;
  }
  if ((flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY) || (flag_Control_to_SSP == EMPTY && flag_Deframing_to_Control == FULL) ) {
     Serial.print("\nManagement\n");
	  AX25_Manager(&control);
  }

  if (flag_Control_to_Framing == FULL && flag_SerialTXBuffer == EMPTY) {
	  Serial.print("\nBuild Frame\n");

	  AX25_buildFrame(SerialTXBuffer, info, &frameSize, addr, control, g_infoSize);
  }

if (flag_SerialTXBuffer == FULL){
  for (int i = 0; i < frameSize; ++i) {
    Serial.print(SerialTXBuffer[i], HEX);
  }
  flag_SerialTXBuffer = EMPTY;
  Serial.print("\n\n");
}
  //printf("\n frame size in bytes is: %d\n", frameSize);

//  if (flag_Deframing_to_Control == EMPTY) {
//    AX25_deFrame(SerialTXBuffer, frameSize, g_infoSize);
//  }

#ifdef rxDebug_SFRAME
    /* for RX Testing, sending a S frame */

    //static uint8 rx_NR = 0;
    //uint8 rx_control = AX25_getControl(S, REJ, 0, rx_NR, 0);
    //rx_NR++;
    if (flag_SerialRXBuffer == EMPTY) {
    	if(Serial.available()){
    		 Serial.readBytes(SerialRXBuffer,256);
    		 flag_SerialRXBuffer = FULL;
    	}
      //AX25_buildFrame_TEST(SerialRXBuffer, info, addr, rx_control, 0);
      if (flag_Deframing_to_Control == EMPTY) {
        AX25_deFrame(SerialRXBuffer, frameSize, 0);
      }
      flag_SerialRXBuffer = EMPTY;
    }
#endif

#ifdef rxDebug_IFRAME
    /* for RX Testing, sending a I frame */

    /* to test if address is wrong */
    //    uint8 addr_RX[ADDR_LEN] = { 'B', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U', 'F','T', 'I', '1', 0x61 };

if (flag_SerialRXBuffer == EMPTY){
      if (Serial.available()) {
        g_infoSize= 236;
        Serial.print("\n waiting for data \n");
        Serial.readBytes(SerialRXBuffer,256);
        Serial.flush();
        delay(100);
        flag_SerialRXBuffer = FULL;
        Serial.print("\n Received Frame\n");
        Serial.flush();
//        for (int i = 0; i < 256; ++i) {
//			  Serial.print(SerialRXBuffer[i], HEX);
//		    }
//        Serial.print("\n\n");
      }
}

      if (flag_Deframing_to_Control == EMPTY && flag_SerialRXBuffer == FULL) {
    	  Serial.print("\nDeframe\n");
          Serial.flush();
        AX25_deFrame(SerialRXBuffer, frameSize, g_infoSize);
      }

#endif
  }


