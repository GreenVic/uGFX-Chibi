#include "ow.h"
#include "gui.h"

static uint8_t TEMP, HR, CHECK_SUM, tmp, bit_counter = 0;
static icucnt_t widths[40];
static void icuwidthcb(ICUDriver *icup)
{
    icucnt_t width = icuGetWidthX(icup);
    if (width >= DHT_START_BIT_WIDTH)
    {
        /* starting bit resetting the bit counter */
        bit_counter = 0;
    }
    else
    {
        /* Recording current width. Just for fun  */
        widths[bit_counter] = width;
        if (width > DHT_LOW_BIT_WIDTH)
        {
            tmp |= (1 << (7 - (bit_counter % 8)));
        }
        else
        {
            tmp &= ~(1 << (7 - (bit_counter % 8)));
        }
        /* When bit_counter is 7, tmp contains the bit from 0 to 7 corresponding to
       The Humidity Rate integer part (Decimal part is 0 on DHT 11) */
        if (bit_counter == 7)
            HR = tmp;
        /* When bit_counter is 23, tmp contains the bit from 16 to 23 corresponding to
       The Temperature integer part (Decimal part is 0 on DHT 11) */
        if (bit_counter == 23)
            TEMP = tmp;
        /* When bit_counter is 39, tmp contains the bit from 32 to 39 corresponding to
       The Check sum value */
        if (bit_counter == 39)
            CHECK_SUM = tmp;
        bit_counter++;
    }
}

static ICUConfig icucfg = {
    ICU_INPUT_ACTIVE_HIGH,
    ICU_TIM_FREQ, /* 1MHz ICU clock frequency.   */
    icuwidthcb,
    NULL,
    NULL,
    ICU_CHANNEL_1,
    0};

THD_WORKING_AREA(waThreadOnewire, 512);
THD_FUNCTION(ThreadOnewire, arg)
{
    (void) arg;
    while (true)
    {
        /*
        * Making a request
        */
        palSetPadMode(GPIOA, 8, PAL_MODE_OUTPUT_PUSHPULL);
        palWritePad(GPIOA, 8, PAL_LOW);
        chThdSleepMicroseconds(MCU_REQUEST_WIDTH);
        palWritePad(GPIOA, 8, PAL_HIGH);

        /*
        * Initializes the ICU driver 1.
        * GPIOA8 is the ICU input.
        */
        palSetPadMode(GPIOA, 8, PAL_MODE_ALTERNATE(1));
        icuStart(&ICUD1, &icucfg);
        icuStartCapture(&ICUD1);
        icuEnableNotifications(&ICUD1);
        chThdSleepMilliseconds(700);
        icuStopCapture(&ICUD1);
        icuStop(&ICUD1);
        gwinPrintf(ghConsole1, "[%u] Temperature: %d C, Humidity: %d %% \n\r", chVTGetSystemTime() ,TEMP, HR);
        if (CHECK_SUM == (TEMP + HR))
        {
            gwinPrintf(ghConsole1, "Checksum OK!\n\r");
        }
        else
        {
            gwinPrintf(ghConsole1, "Checksum FAILED!\n\r");
        }
        chThdSleepMilliseconds(1000);
    }
}