#include "pms.h"
#include "chevents.h"
#include "string.h"
#include "chprintf.h"
#include "gui.h"

static const SerialConfig sd6cfg = {
    9600,
};

pms_data_t PMSData;
uint8_t buffer[40];

char printf_buf[255];

inline void __attribute__((always_inline)) swapEndianBig16(uint16_t *x) {
	uint8_t hi = (*x & 0xff00) >> 8;
	uint8_t lo = (*x & 0xff);
	*x = lo << 8 | hi;
}

THD_WORKING_AREA(waThreadPMS, 512);
THD_FUNCTION(ThreadPMS, arg)
{
    (void)arg;
    palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));
    palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));
    sdStart(&SD6, &sd6cfg);

    event_listener_t elPMSdata;
    eventflags_t flags;
    chEvtRegisterMask((event_source_t *)chnGetEventSource(&SD6), &elPMSdata, EVENT_MASK(1));

    while (1)
    {
        chEvtWaitOneTimeout(EVENT_MASK(1), MS2ST(10000));
        chSysLock();
        flags = chEvtGetAndClearFlags(&elPMSdata);
        chSysUnlock();

        if (flags & CHN_INPUT_AVAILABLE)
        {
            sdRead(&SD6, buffer, 40);
            if (buffer[0] == 0x42) {
                for(int k = 0; k < 20; k++){
                    swapEndianBig16(&buffer[2*k]);
                }
                memcpy(&PMSData, buffer, 40);
                chsnprintf(printf_buf, sizeof(printf_buf), "%u", PMSData.pm2_5_atm);
                gwinSetText(ghLabelPM25, printf_buf, TRUE);
                chsnprintf(printf_buf, sizeof(printf_buf), "%u", PMSData.pm10_0_atm);
                gwinSetText(ghLabelPM10, printf_buf, TRUE);
                chsnprintf(printf_buf, sizeof(printf_buf), "%u", PMSData.pm1_0_atm);
                gwinSetText(ghLabelPM1, printf_buf, TRUE);
                chsnprintf(printf_buf, sizeof(printf_buf), "%.1f", PMSData.temp/10.0);
                gwinSetText(ghLabelTemp, printf_buf, TRUE);
                chsnprintf(printf_buf, sizeof(printf_buf), "%.1f", PMSData.humid/10.0);
                gwinSetText(ghLabelHumid, printf_buf, TRUE);
                chsnprintf(printf_buf, sizeof(printf_buf), "%.4f", PMSData.hcho/1000.0);
                gwinSetText(ghLabelHCHO, printf_buf, TRUE);
                gwinPrintf(ghConsole1, "[%u] >0.3um: %d\n\r", chVTGetSystemTime() , PMSData.cnt_gr_0_3);
            }
        }
        chThdSleepMilliseconds(10);
    }
}

void PMSSanityCheck() {
  BUILD_BUG_ON(sizeof(pms_data_t) != 40);
}