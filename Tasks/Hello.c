/*
 * Hello.c
 *
 *  Created on: 15 de nov de 2016
 *      Author: Fabricio
 */


/* Task dependencies */
#include "Hello.h"
#include "projdefs.h"

/* Include drivers */
#include "../Drivers/Gpio.h"

// Count how much hello messages was sent
volatile unsigned long long hello_sent;

/*
 * Task: Hello Word
 * \brief: Send "Hello World" in the UART
 */
void hello (void* parameters){
	hello_sent = 0;
	while(1){
		UARTSend("Hello World\n\r",13);
		hello_sent++;
		vTaskDelay(1000); // 1second
	}
}


