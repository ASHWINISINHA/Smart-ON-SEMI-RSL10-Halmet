//-----------------------------------------------------------------------------
// Copyright (c) 2018 Semiconductor Components Industries LLC
// (d/b/a "ON Semiconductor").  All rights reserved.
// This software and/or documentation is licensed by ON Semiconductor under
// limited terms and conditions.  The terms and conditions pertaining to the
// software and/or documentation are available at
// http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf ("ON Semiconductor Standard
// Terms and Conditions of Sale, Section 8 Software") and if applicable the
// software license agreement.  Do not use this software and/or documentation
// unless you have carefully read and you agree to the limited terms and
// conditions.  By using this software and/or documentation, you agree to the
// limited terms and conditions.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <BDK.h>
#include "BSP_Components.h"

#include "main.h"

int main(void)
{
    int32_t status = 0;

    BDK_Initialize();
    LED_Initialize(PIN_GIO_SPARE);
    LED_Initialize(LED0);
    LED_Initialize(LED1);
    LED_Initialize(LED2);

    printf("\r\nAPP: NOA1305_ALS example.\r\n");

    status = NOA1305_ALS_Initialize();
    ASSERT_DEBUG(status == 0);

    status = NOA1305_ALS_StartContinuous(APP_ALS_PERIODIC_INTERVAL_MS,
            &ALS_ReadCallback);
    ASSERT_DEBUG(status == 0);

    status = NOA1305_ALS_EnableInterrupt(APP_ALS_THRESHOLD_LEVEL_LUX);
    ASSERT_DEBUG(status == 0);

    uint32_t tt = HAL_Time();

    printf("APP: Entering main loop.\r\n");
    while (1)
    {
        /* Execute any events that occurred and refresh Watchdog. */
        BDK_Schedule();

        /* Latest measured light level can be read at any time in continuous
         * mode.
         */
        if (HAL_TIME_ELAPSED_SINCE(tt) > APP_ALS_LOOP_INTERVAL_MS)
        {
            tt = HAL_Time();

            uint32_t lux;
            if (NOA1305_ALS_ReadLux(&lux) == NOA1305_OK)
            {
                printf("ALS: Loop LUX = %lu\r\n", lux);
                if (lux<20){
                	printf("low light");
                	LED_On(LED_GREEN);
                	LED_On(LED_RED);
                	LED_On(LED_BLUE);
                	LED_On(PIN_GIO_SPARE);
                } else{
                	LED_Of(LED_GREEN);
                	LED_Of(LED_RED);
                	LED_Of(LED_BLUE);
                	LED_Of(PIN_GIO_SPARE);
                }
            }
            else
            {
                printf("ALS: Loop error.\r\n");

            }
        }

        SYS_WAIT_FOR_INTERRUPT;
    }

    return 0;
}


void ALS_ReadCallback(uint32_t lux, bool is_interrupt)
{
    if (is_interrupt == true)
    {
        // Print this line if callback was called in response to lux value
        // passing above / below threshold value.
        printf("ALS: LUX value passed threshold.\r\n");
    }
    printf("ALS: LUX = %lu\r\n\n", lux);
}
