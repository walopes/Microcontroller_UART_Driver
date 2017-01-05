/*
 * drvUART.c
 *
 *  Created on: 4 de dez de 2016
 *      Author: willian
 */

// Code for TIVA EK-TM4C1294XL, using TivaWare
///////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "drvUART.h"

// Show all the functions of the driver
static driver driverUART;

// List of the functions of the driver(size DRV_END)
static ptrFuncDrv uartFunc[END_UART];


// The clock of the CPU to configure the UART
uint32_t g_ui32SysClock; // extern variable


//OBS.: Colocar o #define MAX_UART	8 em drvUART.h

// Queue/ Fila
xQueueHandle buffUART[MAX_UART];
// Semaphore/ Semaforo
xSemaphoreHandle semUART[MAX_UART];
// Mutex/ Fila
xSemaphoreHandle mutexTx[MAX_UART], mutexRx[MAX_UART];



// This function initializes all the modules of the UART
// Inicializa todos os modulos da UART
char initUART(void *parameters)
{
	driverUART.drv_int = (char) parameters;

	char i;
	for(i=0;i<MAX_UART;i++){

		// Create binary semaphore
		vSemaphoreCreateBinary(semUART[i]);
		if( semUART[i] == NULL )
		{
			/* There was insufficient FreeRTOS heap available for the semaphore to
			be created successfully. */
			return FAIL;
		}else{

			// Create mutex
			mutexTx[i] = xSemaphoreCreateMutex();
			if( mutexTx[i] == NULL )
			{
				/* There was insufficient FreeRTOS heap available for the semaphore to
				be created successfully. */
				vSemaphoreDelete( semUART[i]);
				return FAIL;
			}else
			{
				mutexRx[i] = xSemaphoreCreateMutex();
				if( mutexRx[i] == NULL )
				{
					/* There was insufficient FreeRTOS heap available for the semaphore to
					be created successfully. */
					vSemaphoreDelete( semUART[i]);
					vSemaphoreDelete(mutexTx[i]);
					return FAIL;
				}else
				{
					buffUART[i] = xQueueCreate(128, sizeof(char));
					if( buffUART[i] == NULL )
					{
						/* There was insufficient FreeRTOS heap available for the queue to
						be created successfully. */
						vSemaphoreDelete( semUART[i]);
						vSemaphoreDelete(mutexTx[i]);
						vSemaphoreDelete( mutexRx[i]);
						return FAIL;
					}
				}

			}

		}
	}
	return OK;

}


char changeConfUART(void *parameters)
{

	uint32_t Module, BaudRate, Config, stop, datas, parity;

	uartConf *character = (uartConf*) parameters;

	// Baud Rate
		BaudRate = character->baudrate;


	// Config = { Stop bits | Data bits | Parity }
	if(character->databits==5) datas=UART_CONFIG_WLEN_5;
	else if(character->databits==6) datas=UART_CONFIG_WLEN_6;
	else if(character->databits==7) datas=UART_CONFIG_WLEN_7;
	else datas=UART_CONFIG_WLEN_8;
	//
	if(character->stopbits==1) stop=UART_CONFIG_STOP_ONE;
	else stop=UART_CONFIG_STOP_TWO;
	//
	if(character->parity==0) parity=UART_CONFIG_PAR_NONE;
	else if(character->parity==1) parity=UART_CONFIG_PAR_EVEN;
	else if(character->parity==2) parity=UART_CONFIG_PAR_ODD;
	else if(character->parity==3) parity=UART_CONFIG_PAR_ONE;
	else parity=UART_CONFIG_PAR_ZERO;
	//
	Config = ( datas | stop | parity);


	// Module that will be used be
	// Modulo a ser utilizado
	if(character->module == 0){
		// UART0
		Module = UART0_BASE;
		// Enable GPIOA peripheral
		// Habilita o periferico GPIOA
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		// Wait for the GPIOA to be ready
		// Espera que a GPIOA esteja pronta
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

		// Enable the UART_x peripheral
		// Habilita o periferico UARTx
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
		// Wait for the UARTx to be ready
		// Espera que a UARTx esteja pronta
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

		// Configures the alternate function of a GPIO pin
		// Configura funções alternativas dos pinos de GPIO
		GPIOPinConfigure(GPIO_PA0_U0RX);
		GPIOPinConfigure(GPIO_PA1_U0TX);
		// Configures pin(s) for use by the UART peripheral.
		// Configura os pinos para uso do periferico UART
		GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0 | GPIO_PIN_1);

		// UART Configurator
		ROM_UARTConfigSetExpClk(Module,g_ui32SysClock,BaudRate,Config);
		// Disable the FIFO for the UARTx
				UARTFIFODisable(UART0_BASE);
				// Define a priority to the UARTx
				IntPrioritySet(INT_UART0, 0xC0);
				// Enable the Interrupt of UARTx
				ROM_IntEnable(INT_UART0);
				// Enable TX and RX Interrupt for the UARTx
				ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX);
	}
	else if(character->module == 1){
		// UART1
		Module = UART1_BASE;
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));
		//
		GPIOPinConfigure(GPIO_PB0_U1RX);
		GPIOPinConfigure(GPIO_PB1_U1TX);
		GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
		ROM_UARTConfigSetExpClk(Module,g_ui32SysClock,BaudRate,Config);

		UARTFIFODisable(UART1_BASE);
				IntPrioritySet(INT_UART1, 0xC0);
				ROM_IntEnable(INT_UART1);
				ROM_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX);
	}
	else if(character->module == 2){
		// UART2
		Module = UART2_BASE;
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART2));
		//
		GPIOPinConfigure(GPIO_PA6_U2RX);
		GPIOPinConfigure(GPIO_PA7_U2TX);
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);
		ROM_UARTConfigSetExpClk(Module,g_ui32SysClock,BaudRate,Config);
		//
		UARTFIFODisable(UART2_BASE);
				IntPrioritySet(INT_UART2, 0xC0);
				IntEnable(INT_UART2);
				UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_RT);
	}
	else if(character->module == 3){
		// UART3
		Module = UART3_BASE;
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART3));
		//
		GPIOPinConfigure(GPIO_PA4_U3RX);
		GPIOPinConfigure(GPIO_PA5_U3TX);
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);
		ROM_UARTConfigSetExpClk(Module,g_ui32SysClock,BaudRate,Config);
		//UARTFIFODisable(UART3_BASE);
		IntPrioritySet(INT_UART3, 0xC0);
		IntEnable(INT_UART3);
		UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);
	}
	else if(character->module == 4){
		// UART4
		Module = UART4_BASE;
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART4));
		//
		GPIOPinConfigure(GPIO_PA2_U4RX);
		GPIOPinConfigure(GPIO_PA3_U4TX);
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3);
		ROM_UARTConfigSetExpClk(Module,g_ui32SysClock,BaudRate,Config);
		//
		UARTFIFODisable(UART4_BASE);
				IntPrioritySet(INT_UART4, 0xC0);
				IntEnable(INT_UART4);
				UARTIntEnable(UART4_BASE, UART_INT_RX | UART_INT_RT);
	}
	else if(character->module == 5){
		// UART5
		Module = UART5_BASE;
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART5));
		//
		GPIOPinConfigure(GPIO_PC6_U5RX);
		GPIOPinConfigure(GPIO_PC7_U5TX);
		GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
		ROM_UARTConfigSetExpClk(Module,g_ui32SysClock,BaudRate,Config);
		//
		UARTFIFODisable(UART5_BASE);
				IntPrioritySet(INT_UART5, 0xC0);
				IntEnable(INT_UART5);
				UARTIntEnable(UART5_BASE, UART_INT_RX | UART_INT_RT);
	}
	else if(character->module == 6){
		// UART6
		Module = UART6_BASE;
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP));
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART6));
		//
		GPIOPinConfigure(GPIO_PP0_U6RX);
		GPIOPinConfigure(GPIO_PP1_U6TX);
		GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);
		ROM_UARTConfigSetExpClk(Module,g_ui32SysClock,BaudRate,Config);
		//
		UARTFIFODisable(UART6_BASE);
				IntPrioritySet(INT_UART6, 0xC0);
				IntEnable(INT_UART6);
				UARTIntEnable(UART6_BASE, UART_INT_RX | UART_INT_RT);
	}
	else if(character->module == 7){
		//UART7
		Module = UART7_BASE;
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
		ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART7));
		//
		GPIOPinConfigure(GPIO_PC4_U7RX);
		GPIOPinConfigure(GPIO_PC5_U7TX);
		GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
		ROM_UARTConfigSetExpClk(Module,g_ui32SysClock,BaudRate,Config);
		//
		UARTFIFODisable(UART7_BASE);
				IntPrioritySet(INT_UART7, 0xC0);
				IntEnable(INT_UART7);
				UARTIntEnable(UART7_BASE, UART_INT_RX | UART_INT_RT);
	}




	return OK;
}


char sendUART(void *parameters)
{

	message *character = (message*) parameters;
	/*char a;
	if(character->module == 0) a=0;
	else if(character->module == 1) a=1;
	else if(character->module == 2) a=2;
	else if(character->module == 3) a=3;
	else if(character->module == 4) a=4;
	else if(character->module == 5) a=5;
	else if(character->module == 6) a=6;
	else if(character->module == 7) a=7;*/

	if(mutexTx[character->module] != NULL)
	{
		if(xSemaphoreTake(mutexTx[character->module],portMAX_DELAY) == pdTRUE) /// If a semaphore is ready
		{
			int i;
			for(i=0;i<character->size;i++){

				if(character->module == 0){	// Module 0
					ROM_UARTCharPutNonBlocking(UART0_BASE, character->value[i]);
				}
				else if(character->module == 1){
					ROM_UARTCharPutNonBlocking(UART1_BASE, character->value[i]);
				}else if(character->module == 2){
					ROM_UARTCharPutNonBlocking(UART2_BASE, character->value[i]);
				}else if(character->module == 3){
					ROM_UARTCharPutNonBlocking(UART3_BASE, character->value[i]);
				}else if(character->module == 4){
					ROM_UARTCharPutNonBlocking(UART4_BASE, character->value[i]);
				}else if(character->module == 5){
					ROM_UARTCharPutNonBlocking(UART5_BASE, character->value[i]);
				}else if(character->module == 6){
					ROM_UARTCharPutNonBlocking(UART6_BASE, character->value[i]);
				}else if(character->module == 7){
					ROM_UARTCharPutNonBlocking(UART7_BASE, character->value[i]);
				}

				// Wait indefinitely for a UART interrupt
				xSemaphoreTake(semUART[character->module], portMAX_DELAY);
			}

			xSemaphoreGive(mutexTx[character->module]);
			return OK;
		}
	}

	return FAIL;
}

char sendCharUART(void *parameter)
{
	getChar *g = parameter;

	if(mutexTx[g->module] != NULL)
	{
		if(xSemaphoreTake(mutexTx[g->module],portMAX_DELAY) == pdTRUE) /// If a semaphore is ready
		{
			if(g->module == 0) ROM_UARTCharPutNonBlocking(UART0_BASE, g->character);
			else if(g->module == 1) ROM_UARTCharPutNonBlocking(UART1_BASE, g->character);
			else if(g->module == 2) ROM_UARTCharPutNonBlocking(UART2_BASE, g->character);
			else if(g->module == 3) ROM_UARTCharPutNonBlocking(UART3_BASE, g->character);
			else if(g->module == 4) ROM_UARTCharPutNonBlocking(UART4_BASE, g->character);
			else if(g->module == 5) ROM_UARTCharPutNonBlocking(UART5_BASE, g->character);
			else if(g->module == 6) ROM_UARTCharPutNonBlocking(UART6_BASE, g->character);
			else if(g->module == 7) ROM_UARTCharPutNonBlocking(UART7_BASE, g->character);


			xSemaphoreTake(semUART[g->module], portMAX_DELAY);
		}
		xSemaphoreGive(mutexTx[g->module]);
		return OK;
	}

	return FAIL;

}

char receiveUART(void *parameters)
{
	portBASE_TYPE s;
	getChar *g = (getChar*) parameters;
	char p;

	if(mutexRx[g->module] != NULL)
	{
			if((xSemaphoreTake(mutexRx[g->module],portMAX_DELAY) == pdTRUE)) /// If a semaphore is ready
			{
				//s = xQueueReceive(buffUART[g->module],(void*)g->character,portMAX_DELAY);
				s = xQueueReceive(buffUART[g->module],&p,portMAX_DELAY);
				g->character = p;
				xSemaphoreGive(mutexRx[g->module]);
			}
			return OK;
	}

	return FAIL;
}


// Interrupt Handler of the UART 0
void UART0_IntHandler(void)
{
	uint32_t ui32Status;
	signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
	signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
	char data;

	// Get the interrrupt status of UART Module 0
	// Obtem o status da interrupcao da UART Modulo 0
	ui32Status = UARTIntStatus(UART0_BASE, true);

	// Clears UART interrupt sources
	// Limpa as fontes de interrupcao da UART
	UARTIntClear(UART0_BASE, ui32Status);

	// If there's data to be read
	// Se ha dados para serem lidos
	if ((ui32Status & UART_INT_RX) )
	{
		// Loop while there are characters in the receive FIFO.
		// Faz um laco enquanto existem caracteres na FIFO
		while(UARTCharsAvail(UART0_BASE))
		{
			data = (char)UARTCharGetNonBlocking(UART0_BASE);
			// Sent the data to the queue
			// Envia o dado para a fila
			xQueueSendToBackFromISR(buffUART[0], &data, &pxHigherPriorityTaskWokenRX);
		}
	}

	// If there's data to be sent
	// Se ha dados para serem lidos
	if ((ui32Status & UART_INT_TX) )
	{
		// Release the semaphore
		// Libera o semaforo
		xSemaphoreGiveFromISR(semUART[0], &pxHigherPriorityTaskWokenTX);
	}

	if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE))
	{
		portYIELD();
	}
}

// Interrupt Handler of the UART 1
void UART1_IntHandler(void)
{
	uint32_t ui32Status;
	signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
	signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
	char data;
	//
	ui32Status = UARTIntStatus(UART1_BASE, true);
	UARTIntClear(UART1_BASE, ui32Status);
	//
	if ((ui32Status & UART_INT_RX) ) // If there's data to be read
	{
		while(UARTCharsAvail(UART1_BASE))
		{
			data = (char)UARTCharGetNonBlocking(UART1_BASE);
			xQueueSendToBackFromISR(buffUART[1], &data, &pxHigherPriorityTaskWokenRX);
		}
	}
	//
	if ((ui32Status & UART_INT_TX) ) // If there's data to be sent
		xSemaphoreGiveFromISR(semUART[1], &pxHigherPriorityTaskWokenTX);
	//
	if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE))
	{
		portYIELD();
	}
}

// Interrupt Handler of the UART 2
void UART2_IntHandler(void)
{
	uint32_t ui32Status;
		signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
		signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
		char data;
		//
		ui32Status = UARTIntStatus(UART2_BASE, true);
		UARTIntClear(UART2_BASE, ui32Status);
		//
		if ((ui32Status & UART_INT_RX) ) // If there's data to be read
		{
			while(UARTCharsAvail(UART2_BASE))
			{
				data = (char)UARTCharGetNonBlocking(UART2_BASE);
				xQueueSendToBackFromISR(buffUART[2], &data, &pxHigherPriorityTaskWokenRX);
			}
		}
		//
		if ((ui32Status & UART_INT_TX) ) // If there's data to be sent
			xSemaphoreGiveFromISR(semUART[2], &pxHigherPriorityTaskWokenTX);
		//
		if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE))
		{
			portYIELD();
		}
}

// Interrupt Handler of the UART 0
void UART3_IntHandler(void)
{
	uint32_t ui32Status;
		signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
		signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
		char data;
		//
		ui32Status = UARTIntStatus(UART3_BASE, true);
		UARTIntClear(UART3_BASE, ui32Status);
		//
		if ((ui32Status & UART_INT_RX) ) // If there's data to be read
		{
			while(UARTCharsAvail(UART3_BASE))
			{
				data = (char)UARTCharGetNonBlocking(UART3_BASE);
				xQueueSendToBackFromISR(buffUART[3], &data, &pxHigherPriorityTaskWokenRX);
			}
		}
		//
		if ((ui32Status & UART_INT_TX) ) // If there's data to be sent
			xSemaphoreGiveFromISR(semUART[3], &pxHigherPriorityTaskWokenTX);
		//
		if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE))
		{
			portYIELD();
		}
}

// Interrupt Handler of the UART 0
void UART4_IntHandler(void)
{
	uint32_t ui32Status;
		signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
		signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
		char data;
		//
		ui32Status = UARTIntStatus(UART4_BASE, true);
		UARTIntClear(UART4_BASE, ui32Status);
		//
		if ((ui32Status & UART_INT_RX) ) // If there's data to be read
		{
			while(UARTCharsAvail(UART4_BASE))
			{
				data = (char)UARTCharGetNonBlocking(UART4_BASE);
				xQueueSendToBackFromISR(buffUART[4], &data, &pxHigherPriorityTaskWokenRX);
			}
		}
		//
		if ((ui32Status & UART_INT_TX) ) // If there's data to be sent
			xSemaphoreGiveFromISR(semUART[4], &pxHigherPriorityTaskWokenTX);
		//
		if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE))
		{
			portYIELD();
		}
}

// Interrupt Handler of the UART 0
void UART5_IntHandler(void)
{
	uint32_t ui32Status;
		signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
		signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
		char data;
		//
		ui32Status = UARTIntStatus(UART5_BASE, true);
		UARTIntClear(UART5_BASE, ui32Status);
		//
		if ((ui32Status & UART_INT_RX) ) // If there's data to be read
		{
			while(UARTCharsAvail(UART5_BASE))
			{
				data = (char)UARTCharGetNonBlocking(UART5_BASE);
				xQueueSendToBackFromISR(buffUART[5], &data, &pxHigherPriorityTaskWokenRX);
			}
		}
		//
		if ((ui32Status & UART_INT_TX) ) // If there's data to be sent
			xSemaphoreGiveFromISR(semUART[5], &pxHigherPriorityTaskWokenTX);
		//
		if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE))
		{
			portYIELD();
		}
}

// Interrupt Handler of the UART 0
void UART6_IntHandler(void)
{
	uint32_t ui32Status;
		signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
		signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
		char data;
		//
		ui32Status = UARTIntStatus(UART6_BASE, true);
		UARTIntClear(UART6_BASE, ui32Status);
		//
		if ((ui32Status & UART_INT_RX) ) // If there's data to be read
		{
			while(UARTCharsAvail(UART6_BASE))
			{
				data = (char)UARTCharGetNonBlocking(UART6_BASE);
				xQueueSendToBackFromISR(buffUART[6], &data, &pxHigherPriorityTaskWokenRX);
			}
		}
		//
		if ((ui32Status & UART_INT_TX) ) // If there's data to be sent
			xSemaphoreGiveFromISR(semUART[6], &pxHigherPriorityTaskWokenTX);
		//
		if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE))
		{
			portYIELD();
		}
}

// Interrupt Handler of the UART 0
void UART7_IntHandler(void)
{
	uint32_t ui32Status;
		signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
		signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
		char data;
		//
		ui32Status = UARTIntStatus(UART7_BASE, true);
		UARTIntClear(UART7_BASE, ui32Status);
		//
		if ((ui32Status & UART_INT_RX) ) // If there's data to be read
		{
			while(UARTCharsAvail(UART7_BASE))
			{
				data = (char)UARTCharGetNonBlocking(UART7_BASE);
				xQueueSendToBackFromISR(buffUART[7], &data, &pxHigherPriorityTaskWokenRX);
			}
		}
		//
		if ((ui32Status & UART_INT_TX) ) // If there's data to be sent
			xSemaphoreGiveFromISR(semUART[7], &pxHigherPriorityTaskWokenTX);
		//
		if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE))
		{
			portYIELD();
		}
}


driver* getDriverUART(void)
{
	//init
	driverUART.initFunc = initUART;

	//functions
	uartFunc[SEND_CHAR_UART] =  sendCharUART;
	uartFunc[CONFIG_UART] =  changeConfUART;
	uartFunc[SEND_STRING_UART] =  sendUART;
	uartFunc[RECEIVE_UART] =  receiveUART;

	driverUART.funcoes = &uartFunc[0];
	return &driverUART;
}

