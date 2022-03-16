/*
 *
 *
 *  Created on: Oct 8, 2021
 *      Author: Mohamed Ezzat
 */
#include "Arduino.h"
#include <stdlib.h>
#include <stdio.h>

#include "ax25.h"

/* keep this line when in Arduino is in RX mode, otherwise, comment it out. */
//#define RX_M

/* keep this line when Debugging, otherwise, comment it out. */
//#define DEBUG

uint8 SerialTXBuffer[AX25_FRAME_MAX_SIZE];
uint8 SerialRXBuffer[AX25_FRAME_MAX_SIZE];
uint8 info[SSP_FRAME_MAX_SIZE]; /* this is in Control to Framing part */
uint8 SSP_to_Control_Buffer[SIZE_SSP_to_Control_Buffer];
uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U', 'F', 'T',
		'I', '1', 0x61 };

uint8 flag_SSP_to_Control = EMPTY;
uint8 flag_Control_to_Framing = EMPTY;
uint8 flag_Control_to_SSP = EMPTY;
uint8 flag_Deframing_to_Control = EMPTY;
uint8 flag_SerialTXBuffer = EMPTY;
uint8 flag_SerialRXBuffer = EMPTY;
uint8 flag_next_frame = EMPTY;

uint8 g_infoSize = 236;  //temp set as 236

void serialFlush() {
	while (Serial.available() > 0) {
		char t = Serial.read();
	}
#ifdef DEBUG
	while (Serial.available() > 0) {
		char t = Serial.read();
	}
#endif
}

/* Testing Function */
void print_info() {
	Serial.print("\nHi\n");
	for (int i = 0; i < SSP_FRAME_MAX_SIZE; ++i) {
		Serial.print(info[i], HEX);
	}
	Serial.flush();
	Serial.print("\nHi 2\n");
	Serial.println(g_infoSize);
}

void printSerialTXBufferToSerial() {
	if (flag_SerialTXBuffer == FULL) {
		for (int i = 0; i < AX25_FRAME_MAX_SIZE; ++i) {
			Serial.write(SerialTXBuffer[i]);

			//	Serial.print(SerialTXBuffer[i], HEX);
			Serial.flush();
		}
		flag_SerialTXBuffer = EMPTY;
		//	Serial.print("\n\n");
	}
}

void readFrameFromSerial() {
	uint8 flag_flagAndDestMatchSerialRXBuffer = SET; /* init value as set */
	uint8 flagAndDestAddress[8] = { 0x7e, 'O', 'N', '4', 'U', 'L', 'G', 0x60 };

	if (Serial.available() && flag_SerialRXBuffer == EMPTY) {
		g_infoSize = SSP_FRAME_MAX_SIZE;

#ifdef DEBUG
		Serial.print("\n waiting for data \n");
#endif

		Serial.readBytes(SerialRXBuffer, 8);
		for (uint8 i = 0; i < 8; i++) {
			if (SerialRXBuffer[i] != flagAndDestAddress[i]) {
				flag_flagAndDestMatchSerialRXBuffer = CLEAR;
			}
		}

#ifdef DEBUG
		Serial.println(SerialRXBuffer[0]);
#endif
		if (flag_flagAndDestMatchSerialRXBuffer == SET) {
			for (uint8 i = 1; i < 32; i++) {
				Serial.readBytes(SerialRXBuffer + (8 * i), 8);
			}

			//Serial.readBytes(SerialRXBuffer, AX25_FRAME_MAX_SIZE);

			//Serial.flush();
			delay(100);
			flag_SerialRXBuffer = FULL;

#ifdef DEBUG
		Serial.print("\n Received Frame\n");
#endif
			Serial.flush();

			/* prints the frame received from serial on serial monitor */

			for (int i = 0; i < 30; ++i) {

#ifdef DEBUG
			Serial.print(SerialRXBuffer[i], HEX);
			Serial.flush();
#endif

			}

#ifdef DEBUG
		Serial.print("\n\n");
#endif

			for (int i = 230; i < 256; ++i) {

#ifdef DEBUG
					Serial.print(SerialRXBuffer[i], HEX);
					Serial.flush();
#endif
			}

#ifdef DEBUG
		Serial.print("\n\n");
#endif

		}
	}
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);

#ifdef DEBUG_1
	Serial1.begin(9600);
#endif

	/* if we connect as RX remove this part */
#ifndef RX_M
	if (flag_SSP_to_Control == EMPTY) {
		fillBuffer(SSP_to_Control_Buffer, SIZE_SSP_to_Control_Buffer);
		flag_SSP_to_Control = FULL;
	}
#endif
}

void loop() {
	// put your main code here, to run repeatedly:

	uint8 control;
	uint16 frameSize = 0;
	/* Sends next frame */
	if (flag_next_frame == FULL) {
		if (flag_SSP_to_Control == EMPTY) {
			fillBuffer(SSP_to_Control_Buffer, SIZE_SSP_to_Control_Buffer);
			flag_SSP_to_Control = FULL;
		}
		flag_next_frame = EMPTY;
	}

	/* Calls the manager function */
	if ((flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY)
			|| (flag_Control_to_SSP == EMPTY
					&& flag_Deframing_to_Control == FULL)) {

#ifdef DEBUG
		Serial.print("\nManagement\n");
#endif
		AX25_Manager(&control);
	}

	/* Builds Frame after receiving fields */
	if (flag_Control_to_Framing == FULL && flag_SerialTXBuffer == EMPTY) {

#ifdef DEBUG
		Serial.print("\nBuild Frame\n");
#endif
		AX25_buildFrame(SerialTXBuffer, info, &frameSize, addr, control,
				g_infoSize);
	}

	/* Prints Serial TX buffer */
	printSerialTXBufferToSerial();

	/* Gets frame from serial */
	readFrameFromSerial();
	//serialFlush();

	/* Calls the de-framing function */
	if (flag_Deframing_to_Control == EMPTY && flag_SerialRXBuffer == FULL) {

#ifdef DEBUG
		Serial.print("\nDeframe\n");
#endif
		AX25_deFrame(SerialRXBuffer, frameSize, g_infoSize);
	}
}

