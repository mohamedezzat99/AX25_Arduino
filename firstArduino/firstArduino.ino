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

/* used to initiate serial 1 for Arduino MEGA */
//#define DEBUG_1




//Include nRF Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";






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
	for (int i = 0; i < SSP_FRAME_MAX_SIZE; ++i) {
		Serial.print(info[i], HEX);
	}
	Serial.flush();
	Serial.println(g_infoSize);
}

void printSerialTXBufferToSerial() {
	if (flag_SerialTXBuffer == FULL) {

		//Send message to receiver
			  //const char text[] = "Hello World";
		radio.stopListening();
			// delay(1000);




	//for (uint8 i = 0; i < 31; i++) {
		//radio.write(&SerialTXBuffer[i], sizeof(uint8));
		//radio.write(SerialRXBuffer + (8 * i), 8);
	//	delay(1);
	//}

		for (int i = 0; i < AX25_FRAME_MAX_SIZE; ++i) {
			radio.write(&SerialTXBuffer[i], sizeof(uint8));
			//delay(1);
			delayMicroseconds(10);
			//Serial.write(SerialTXBuffer[i]);

			//	Serial.print(SerialTXBuffer[i], HEX);
			//Serial.flush();
	}
		flag_SerialTXBuffer = EMPTY;
		radio.startListening();
		//	Serial.print("\n\n");
	}
}

void readFrameFromSerial() {
	uint8 flag_flagAndDestMatchSerialRXBuffer = SET; /* init value as set */
	uint8 flagAndDestAddress[8] = { 0x7e, 'O', 'N', '4', 'U', 'L', 'G', 0x60 };

#ifdef SerialWire

	if (Serial.available() && flag_SerialRXBuffer == EMPTY) {
		g_infoSize = SSP_FRAME_MAX_SIZE;

#ifdef DEBUG
		Serial1.print("\n waiting for data \n");
#endif

		Serial.readBytes(SerialRXBuffer, 8);
		for (uint8 i = 0; i < 8; i++) {
			if (SerialRXBuffer[i] != flagAndDestAddress[i]) {
				flag_flagAndDestMatchSerialRXBuffer = CLEAR;
			}
		}

//#ifdef DEBUG
//		Serial1.println(SerialRXBuffer[0]);
//#endif
		if (flag_flagAndDestMatchSerialRXBuffer == SET) {
			for (uint8 i = 1; i < 32; i++) {
				Serial.readBytes(SerialRXBuffer + (8 * i), 8);
			}

			//Serial.readBytes(SerialRXBuffer, AX25_FRAME_MAX_SIZE);

			//Serial.flush();
			delay(100);
			flag_SerialRXBuffer = FULL;

#ifdef DEBUG
			Serial1.print("\n Received Frame\n");
#endif
			Serial.flush();

			/* prints the frame received from serial on serial monitor */

			for (int i = 0; i < 30; ++i) {

#ifdef DEBUG
				Serial1.print(SerialRXBuffer[i], HEX);
				Serial.flush();
#endif

			}

#ifdef DEBUG
			Serial1.print("\n\n");
#endif

			for (int i = 230; i < 256; ++i) {

#ifdef DEBUG
				Serial1.print(SerialRXBuffer[i], HEX);
				Serial.flush();
#endif
			}

#ifdef DEBUG
			Serial1.print("\n\n");
#endif

		}
	}
#endif


#ifndef SerialWire
	//todo:write here code to read from nrf and take care about flags
#endif
}

void setup() {
	// put your setup code here, to run once:

#ifndef RX_M

	radio.begin();

	  //set the address
	  radio.openWritingPipe(address);

	  radio.openReadingPipe(0, address);
	  //Set module as transmitter
	  radio.stopListening();


#endif


#ifdef RX_M


	  while (!Serial);
	     Serial.begin(9600);

	   radio.begin();

	   //set the address
	   radio.openReadingPipe(0, address);

	   //Set module as receiver
	   radio.startListening();

#endif







	Serial.begin(9600);


//	Serial1.begin(9600);

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
		Serial1.print("\nManagement\n");
#endif
		AX25_Manager(&control);
	}

	/* Builds Frame after receiving fields */
	if (flag_Control_to_Framing == FULL && flag_SerialTXBuffer == EMPTY) {

#ifdef DEBUG
		Serial1.print("\nBuild Frame\n");
#endif
		AX25_buildFrame(SerialTXBuffer, info, &frameSize, addr, control,
				g_infoSize);
	}

	/* Prints Serial TX buffer (and now sends frame to nRF) */
	printSerialTXBufferToSerial();



	/* Gets frame from serial */
	readFrameFromSerial();

	//Read the data if available in buffer
//	  if (radio.available())
//	  {
//	    char text[32] = {0};
//	    radio.read(&text, sizeof(text));
//	    Serial.println(text);
//	  }


	//serialFlush();

	/* Calls the de-framing function */
	if (flag_Deframing_to_Control == EMPTY && flag_SerialRXBuffer == FULL) {

#ifdef DEBUG
		Serial1.print("\nDeframe\n");
#endif
		AX25_deFrame(SerialRXBuffer, frameSize, g_infoSize);
	}
}
