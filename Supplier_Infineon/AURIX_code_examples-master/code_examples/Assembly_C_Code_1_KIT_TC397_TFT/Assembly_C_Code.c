/**********************************************************************************************************************
 * \file Assembly_C_Code.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 *
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are solely in the form of
 * machine-executable object code generated by a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------------Includes-----------------------------------------------------*/
/*********************************************************************************************************************/
#include "Assembly_C_Code.h"
#include "Ifx_Types.h"
#include "Bsp.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/* Define LEDs port pins */
#define LED1        &MODULE_P13,0           /* LED port, pin definition */
#define LED2        &MODULE_P13,1           /* LED port, pin definition */

#define LED_ON      1U
#define LED_OFF     0U

#define DELAY_1S    1000                    /* Time to wait for delay in ms */

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
extern void set_LED1_State_Assembly(uint32 state);
void wait_ms(uint32 ms);

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Set state of LED2 using assembly code */
void set_LED2_State_Assembly(uint32 state)
{
    __asm(  "jne %0, #0, switch_on  \n\t"   /* Check the passed value, stored in D4                         */
            "mov d0, #2             \n\t"   /* If 0 (LED_OFF), switch off LED2: by writing 0x2              */
            "j common               \n\t"
            "switch_on:             \n\t"
            "mov d1, #0x2           \n\t"   /* Else 1 (LED_ON), switch on LED2: by writing 0x20000          */
            "sh d0, d1, #16         \n\t"   /* Shift of 16 bits                                             */
            "common:                \n\t"
            "movh.a a0, #0xF003     \n\t"   /* Load the Address of P13_OMR register "0xF003AD04" into A0    */
            "lea a0, [a0] + 0x7700  \n\t"
            "lea a0, [a0] + 0x3604  \n\t"
            "st.w [a0], d0          \n\t"   /* Store the D0 value to the target register                    */
            : "=d" (state)
            : "0" (state)
      );
}

/* Toggle LED1, LED2 using assembly code functions */
void run_Assembly_Code(void)
{
    /* Configure LED1, LED2 port pins */
    IfxPort_setPinMode(LED1, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED2, IfxPort_Mode_outputPushPullGeneral);

    /* Turn off LEDs (LEDs are low-level active)   */
    IfxPort_setPinHigh(LED1);
    IfxPort_setPinHigh(LED2);

    /* Switch ON LED1 using assembly code from Assembly_Code.src file */
    set_LED1_State_Assembly(LED_ON);

    /* Add a one second delay */
    wait_ms(DELAY_1S);

    /* Switch ON LED2 using the above assembly code function */
    set_LED2_State_Assembly(LED_ON);

    /* Add a one second delay */
    wait_ms(DELAY_1S);

    /* Switch OFF LED1 using assembly code from Assembly_Code.src file */
    set_LED1_State_Assembly(LED_OFF);

    /* Add a one second delay */
    wait_ms(DELAY_1S);

    /* Switch OFF LED2 using the above assembly code function */
    set_LED2_State_Assembly(LED_OFF);
}

/* Wait until a timeout in milliseconds */
void wait_ms(uint32 ms)
{
    sint32 fSys = IfxStm_getFrequency(&MODULE_STM0);
    Ifx_TickTime wait_ms = (fSys / (1000 / ms));

    wait(wait_ms);
}
