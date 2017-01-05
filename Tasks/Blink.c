/*
 * Blink.c
 *
 *  Created on: 15 de nov de 2016
 *      Author: Fabricio
 */



#include "Blink.h"
#include "projdefs.h"
#include "task.h"

#include "../Drivers/Gpio.h"


/**
 * \brief: Task blink some board LED
 */
void blink (void* parameters){
	while(1){
		//
		// Turn on the LED.
		//
		//GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
		Gpio_ToggleLed(LED1);

		//
		// Delay for a bit.
		//
		vTaskDelay(1000); // 1second
	}
}

