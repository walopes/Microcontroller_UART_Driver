/*
 * TarefaTeste.h
 *
 *  Created on: 7 de dez de 2016
 *      Author: willian
 */

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* MCU includes */
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "TarefaTeste.h"
#include "ctrDriver.h"	// Driver Controllers
#include "drvUART.h"

#define modulo_teste 0

// Task to Echo the UART (send and receive the same string)
void TestTask1(void *parameters)
{
	// To avoid warning
	// Evitar avisos do compilador
	(void) parameters;
	
	//initialSet Conf;
	//Conf.module = 0;

	// Install the driver of the UART -> DRV_UART
	initDriver(DRV_UART);

	uartConf Teste;
	//Teste.module = 0; // UART Mod 0
	Teste.module = modulo_teste; // UART Mod 0
	//Teste.baudrate = 115200; // 115200 of Baud Rate
	Teste.baudrate = 9600; // 9600 of Baud Rate
	Teste.databits = 8; // 8-n-1 data bits
	Teste.stopbits = 1; // 0 stop bits
	Teste.parity = 0; // none parity


	// Call the function to Configurate the UART
	// Chama a funcao para configurar a UART
	//UARTEnable(UART0_BASE);
	callDriver(DRV_UART,CONFIG_UART,&Teste);
	
	message M;
	M.module = modulo_teste;
	M.value = "Teste\r\n";
	M.size = strlen(M.value);
	// Call the function to Send via UART
	// Chama a funcao para enviar via UART
	callDriver(DRV_UART,SEND_STRING_UART,&M);

	getChar g;
	g.module = modulo_teste;
	g.character = 'A';

	callDriver(DRV_UART,SEND_CHAR_UART,&g);
	callDriver(DRV_UART,SEND_CHAR_UART,&g);

	while(1)
	{
		// Echo/ Eco
		callDriver(DRV_UART,RECEIVE_UART,&g);
		callDriver(DRV_UART,SEND_CHAR_UART,&g);

		vTaskDelay(100);
	}
	
}

// Task to receive via UART
void TestTask2(void *parameters)
{
	// Install the driver of the UART -> DRV_UART
	//initDriver(DRV_UART);

	vTaskDelay(5000);
	//UARTEnable(UART1_BASE);
	uartConf Teste;
	//Teste.module = 1; // UART Mod 0
	Teste.module = 1; // UART Mod 0
	//Teste.baudrate = 115200; // 115200 of Baud Rate
	Teste.baudrate = 115200; // 9600 of Baud Rate
	Teste.databits = 8; // 8-n-1 data bits
	Teste.stopbits = 1; // 0 stop bits
	Teste.parity = 0; // none parity


	// Call the function to Configurate the UART
	// Chama a funcao para configurar a UART
	callDriver(DRV_UART,CONFIG_UART,&Teste);


	int i=0;
	//getChar g;
	//g.module = 1;
	//g.character = 'G';


	message M;
	M.module = 1;
	M.value = "UART1\r\n";
	M.size = strlen(M.value);

	while(1)
	{
		//i++;
		callDriver(DRV_UART,SEND_STRING_UART,&M);
		vTaskDelay(1000);
	}

}


