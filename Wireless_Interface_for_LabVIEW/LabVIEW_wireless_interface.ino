//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";

uint8_t SerialTXBuffer[256];
uint8_t SerialRXBuffer[256];
uint8_t frameIndexCounter = 0;
uint8_t flag_FrameIsFull = 0;

void readFrameFromSerial() {
	for (uint8_t i = 0; i < 32; i++) {
		Serial.readBytes(SerialTXBuffer + (8 * i), 8);
	}

	// for (uint8_t i = 0; i < 256; i++) {
	//   Serial.print(SerialTXBuffer[i]);
	// }
	// Serial.flush();
	//delay(100);
}

void writeToNrf() {
	radio.stopListening(); //switch to TX mode
	for (int i = 0; i < 256; ++i) {
		radio.write(&SerialTXBuffer[i], sizeof(uint8_t));
		//   delayMicroseconds(10);
	}
	radio.startListening(); // swtich back to RX mode
}

void readFrameFromNrf() {

	//Read the data if available in buffer
	unsigned char text[1] = { 0 };
	for (int j = 0; j < 256; j++) {
		while (!(radio.available()))
			;
		radio.read(&text, sizeof(text));
		SerialRXBuffer[j] = text[0];
	}
}

void writeToSerial() {
	for (int i = 0; i < 256; ++i) {
		Serial.write(SerialRXBuffer[i]);
//	delay(1);
	Serial.flush();
	}
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	radio.begin();
	radio.openWritingPipe(address);
	radio.openReadingPipe(0, address);
	radio.startListening(); //default to RX mode
}

void loop() {
	// put your main code here, to run repeatedly:

	//read from serial (LabVIEW) and then write to nRF
	if (Serial.available()) {
		readFrameFromSerial();
		writeToNrf();
	}

	//read from nRF then write to Serial (LabVIEW)
	if (radio.available()) {
		readFrameFromNrf();
		writeToSerial();
	}
}
