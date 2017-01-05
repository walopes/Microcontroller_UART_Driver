/*
 * UARTDriver.h
 *
 *  Created on: 4 de dez de 2016
 *      Author: Willian
 */

//////////////////////////////////////////////////////////////////////////


#ifndef UARTDRIVERAPS_UARTDRIVER_H_
	#define UARTDRIVERAPS_UARTDRIVER_H_

	#include "dd_types.h"
	#include <stdio.h>
	#include <stdint.h>

	#define MAX_UART 8
	
	typedef struct{
		uint32_t module;	// The module of the UART which will be used (0-7)
		uint32_t baudrate; // Baud Rate of the UART
		uint32_t databits;
		uint32_t stopbits;
		uint32_t parity;
	}uartConf;
	
	typedef struct{
		uint32_t module;
		char *value;
		int size;
	}message;

	typedef struct{
		uint32_t module;
		char character;
	}getChar;

	
	// Available functions for UART
	enum{
		CONFIG_UART,
		SEND_STRING_UART,
		SEND_CHAR_UART,
		RECEIVE_UART,
		END_UART	// END_UART is the last index (size)
	};

	driver* getDriverUART(void); // Function to get access to the UART driver -> Pointer Function
	

	// Configure/ Set the UART
	char changeConfUART(void *parameters);

	char sendChar(void *parameter);


	// Send bytes via UART
	char sendUART(void *parameter);
	// Receive bytes via UART
	char receiveUART(void *parameter);
		

#endif /* UARTDRIVERAPS_UARTDRIVER_H_ */
