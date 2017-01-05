/*
 * ctrDriver.h
 *
 *  Created on: 4 de dez de 2016
 *      Author: willian
 */
 
#ifndef UARTDRIVERAPS_CTRDRIVER_H_
	#define UARTDRIVERAPS_CTRDRIVER_H_
	
	#define MAX_DRIVERS	20 // Only 20 drivers can be installed
	
	#include "dd_types.h"
	#include "drvUART.h"

	typedef struct{
		char FuncIndex;
		char module;
	}driverConfIN;
	
	// Available functions for UART
	enum{
		DRV_UART, // Identificator for the UART
		DRV_END	// DRV_END is the last index (size)
	};

	
	void initController(void);
	char initDriver(char newDriver);
	char callDriver(char drv_id, char func_id, void *parameter);
	
	static ptrGetDrv drvGetFunc[DRV_END] = {
			getDriverUART, // Driver of the UART
	};
	
#endif //UARTDRIVERAPS_CTRDRIVER_H
