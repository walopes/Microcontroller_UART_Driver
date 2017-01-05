/*
 * ctrDriver.c
 *
 *  Created on: 4 de dez de 2016
 *      Author: willian
 */
 
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "ctrDriver.h"
#include "dd_types.h"
#include "drvUART.h"

static driver* driversLoaded[MAX_DRIVERS];

// How many drivers are loaded in the systems
// Quantidade maxima de drivers carregados pelo sistema
static char qtdDrvLoaded;

// Initialize the driver controller
// Inicializa o controlador dos drivers
void initController(void)
{
	qtdDrvLoaded = 0;
}

// Install the desired driver
// Instala o driver desejado
char initDriver(char newDriver)
//char initDriver()
{
	//newDriver is the index of the driver
	char resp = FAIL;
	
	if(qtdDrvLoaded <  MAX_DRIVERS) // Se e possivel instalar mais drivers
	{
		// Get the desired structure
		driversLoaded[qtdDrvLoaded] = drvGetFunc[newDriver]();
		// should test if driver was loaded correctly
		resp = (char)driversLoaded[qtdDrvLoaded]->initFunc(newDriver);
		qtdDrvLoaded++;
	}
	
	return resp;

}

// Get the function of the specified driver
// Retorna a funcao de um driver especifico
char callDriver(char drv_id, char func_id, void *parameters)
{
	char i=0;
	
	while(i<qtdDrvLoaded){
		if(drv_id == driversLoaded[i]->drv_int)	return driversLoaded[i]->funcoes[func_id](parameters);
		i++;
	}
	
	return DRIVER_NOT_FOUND;
	
}

