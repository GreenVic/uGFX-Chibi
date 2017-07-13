#ifndef PROJ_PMS_H_
#define PROJ_PMS_H_

#include "ch.h"
#include "hal.h"

#define PMS_SERIAL_PORT SD6
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

typedef struct {
    uint8_t header[2];
    uint16_t length;
    uint16_t pm1_0_cf;
    uint16_t pm2_5_cf;
    uint16_t pm10_0_cf;
    uint16_t pm1_0_atm;
    uint16_t pm2_5_atm;
    uint16_t pm10_0_atm;
    uint16_t cnt_gr_0_3;
    uint16_t cnt_gr_0_5;
    uint16_t cnt_gr_1_0;
    uint16_t cnt_gr_2_5;
    uint16_t cnt_gr_5_0;
    uint16_t cnt_gr_10_0;
    uint16_t hcho;
    uint16_t temp;
    uint16_t humid;
    uint16_t resv;
    uint8_t version;
    uint8_t err_code;
    uint16_t checksum;
} __attribute__((__packed__)) pms_data_t;

extern pms_data_t PMSData;

#ifdef __cplusplus
extern "C" {
#endif
extern THD_WORKING_AREA(waThreadPMS, 512);
THD_FUNCTION(ThreadPMS, arg);
#ifdef __cplusplus
}
#endif

#endif