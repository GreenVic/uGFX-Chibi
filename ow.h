
#ifndef PROJ_ONEWIRE_H_
#define PROJ_ONEWIRE_H_

#include "ch.h"
#include "hal.h"

/*===========================================================================*/
/* DHT11 related defines                                                     */
/*===========================================================================*/
/*
 * Width are in useconds
 */
#define MCU_REQUEST_WIDTH 18000
#define DHT_ERROR_WIDTH 200
#define DHT_START_BIT_WIDTH 80
#define DHT_LOW_BIT_WIDTH 28
#define DHT_HIGH_BIT_WIDTH 70
/*===========================================================================*/
/* ICU related code                                                          */
/*===========================================================================*/
#define ICU_TIM_FREQ 1000000

#ifdef __cplusplus
extern "C" {
#endif
extern THD_WORKING_AREA(waThreadOnewire, 512);
THD_FUNCTION(ThreadOnewire, arg);
#ifdef __cplusplus
}
#endif

#endif