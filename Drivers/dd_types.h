/*
 * dd_types.h
 *
 *  Created on: 4 de dez de 2016
 *      Author: willian
 */

#ifndef UARTDRIVERAPS_DD_TYPES_H_
#define UARTDRIVERAPS_DD_TYPES_H_

#define OK	1
#define FAIL NULL
#define DRIVER_NOT_FOUND NULL

typedef char(*ptrFuncDrv)(void *parameters);


typedef struct {
    char drv_int;
    ptrFuncDrv *funcoes;
    ptrFuncDrv initFunc;
} driver;


typedef driver* (*ptrGetDrv)(void);

#endif /* UARTDRIVERAPS_DD_TYPES_H_ */
