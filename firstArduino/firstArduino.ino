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
uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U', 'F', 'T',
		'I', '1', 0x61 };

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

/* Testing Function */
void print_info(){
	Serial.print("\nHi\n");
	for (int i = 0; i < 236; ++i) {
		Serial.print(info[i], HEX);
	}
	Serial.flush();
	Serial.print("\nHi 2\n");
	Serial.println(g_infoSize);
}

void printSerialTXBufferToSerial(){
	if (flag_SerialTXBuffer == FULL) {
		for (int i = 0; i < AX25_FRAME_MAX_SIZE; ++i) {
			Serial.print(SerialTXBuffer[i], HEX);
		}
		Serial.flush();
		flag_SerialTXBuffer = EMPTY;
		Serial.print("\n\n");
	}
}



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

//	uint8 AddressReadyFlag = EMPTY;
//	uint8 ControlReadyFlag = EMPTY;
//  uint8 infoReadyFlag = EMPTY;
//	uint8 FCSReadyFlag = EMPTY;

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

	/* Sends next frame */
	if (flag_next_frame == FULL) {
		if (flag_SSP_to_Control == EMPTY) {
			AX25_getInfo(info);
			flag_SSP_to_Control = FULL;
		}
		flag_next_frame = EMPTY;
	}

	/* Calls the manager function */
	if ((flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY)
			|| (flag_Control_to_SSP == EMPTY
					&& flag_Deframing_to_Control == FULL)) {
		Serial.print("\nManagement\n");
		AX25_Manager(&control);
	}

	/* Builds Frame after receiving fields */
	if (flag_Control_to_Framing == FULL && flag_SerialTXBuffer == EMPTY) {
		Serial.print("\nBuild Frame\n");
		AX25_buildFrame(SerialTXBuffer, info, &frameSize, addr, control,
				g_infoSize);
	}

	/* Prints Serial TX buffer */
	printSerialTXBufferToSerial();
//	if (flag_SerialTXBuffer == FULL) {
//		for (int i = 0; i < frameSize; ++i) {
//			Serial.print(SerialTXBuffer[i], HEX);
//		}
//		Serial.flush();
//		flag_SerialTXBuffer = EMPTY;
//		Serial.print("\n\n");
//	}

	/* Gets frame from serial */
	if (Serial.available() && flag_SerialRXBuffer == EMPTY) {
		g_infoSize = 236;
		Serial.print("\n waiting for data \n");
		Serial.readBytes(SerialRXBuffer, 256);
		Serial.flush();
		delay(100);
		flag_SerialRXBuffer = FULL;
		Serial.print("\n Received Frame\n");
		Serial.flush();

		/* prints the frame received from serial on serial monitor */
//        for (int i = 0; i < 256; ++i) {
//			  Serial.print(SerialRXBuffer[i], HEX);
//		    }
//        Serial.print("\n\n");
	}

	/* Calls the de-framing function */
	if (flag_Deframing_to_Control == EMPTY && flag_SerialRXBuffer == FULL) {
		Serial.print("\nDeframe\n");
		Serial.flush();
		AX25_deFrame(SerialRXBuffer, frameSize, g_infoSize);
	}
}

