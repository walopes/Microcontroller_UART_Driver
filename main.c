///																					///
///       							APS de Sistemas Embarcados	 			  		///
///										Embedded Systems Final Project				///
///   																				///
///  Bruno Follmann 																///
///  Willian Americano Lopes														///
///  																				///
///  Prof. Dr. Gustavo Weber Denardin												///
///  Driver design based on Almeida's model											///
/// 																				///
///////////////////////////////////////////////////////////////////////////////////////

// Standard includes.
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// MCU includes
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"

// Kernel includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "TarefaTeste.h"

static xTaskHandle tsk1; // Task1
static xTaskHandle tsk2; // Task2
//static xTaskHandle tsk2; // Task2

// Variable to get the
extern uint32_t g_ui32SysClock;

void CPUconfig(void)
{
	// Set the clocking to run directly from the crystal at 120MHz.
   g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

  // Enable the GPIO port that is used for the on-board LED.
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

  // Enable the GPIO pins for the LED (PN0).
  ROM_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

  // Enable Interrupts from the CPU
  ROM_IntMasterEnable();


}

void main(void)
{

	CPUconfig(); // Function to configure the CPU

	//BaseType_t xTaskCreate(    TaskFunction_t pvTaskCode,
  //                          const char * const pcName,
  //                          unsigned short usStackDepth,
  //                          void *pvParameters,
  //                          UBaseType_t uxPriority,
  //                          TaskHandle_t *pxCreatedTask
  //                        );


	initController(); // Initialize the driver controller

	// Install the tasks
	xTaskCreate(TestTask1,"Tarefa1",256,NULL,1,&tsk1);
	xTaskCreate(TestTask2,"Tarefa2",256,NULL,2,&tsk2);


	vTaskStartScheduler(); // Start the scheduler

	//while(1);

}
///////////////////////////////////////////////////////////////////////////////////////

