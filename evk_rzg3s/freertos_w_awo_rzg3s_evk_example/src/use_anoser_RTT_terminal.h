/***********************************************************************************************************************
 * File Name    : use_anoser_RTT_terminal.h
 * Description  : Contains a macro that overwrites the destination terminal of RTTViewer
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright 2023 Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics Corporation and/or its affiliates and may only
 * be used with products of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.
 * Renesas products are sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for
 * the selection and use of Renesas products and Renesas assumes no liability.  No license, express or implied, to any
 * intellectual property right is granted by Renesas.  This software is protected under all applicable laws, including
 * copyright laws. Renesas reserves the right to change or discontinue this software and/or this documentation.
 * THE SOFTWARE AND DOCUMENTATION IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND
 * TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY,
 * INCLUDING WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE
 * SOFTWARE OR DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR
 * DOCUMENTATION (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER,
 * INCLUDING, WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY
 * LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 ***********************************************************************************************************************/

#ifndef USE_ANOTHER_TERMINAL_H
#define USE_ANOTHER_TERMINAL_H

#ifdef TERMINAL_NUMBER
#undef TERMINAL_NUMBER
#endif
#define TERMINAL_NUMBER 1   /* If need change TERMINAL_NUMBER, override TERMINAL_NUMBER with #undef and #define */

#ifdef APP_PRINT
#undef APP_PRINT
#endif
#ifdef APP_ERR_PRINT
#undef APP_ERR_PRINT
#endif
#ifdef APP_ERR_TRAP
#undef APP_ERR_TRAP
#endif

#define APP_PRINT(fn_, ...) \
    do { \
    	vTaskSuspendAll(); \
    	SEGGER_RTT_SetTerminal(TERMINAL_NUMBER); \
    	SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__); \
        SEGGER_RTT_SetTerminal(0); \
    	xTaskResumeAll(); \
    } while(0)

#define APP_ERR_PRINT(fn_, ...) \
    do { \
        if(LVL_ERR) {\
            vTaskSuspendAll(); \
            SEGGER_RTT_SetTerminal(TERMINAL_NUMBER); \
            SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",__FUNCTION__,(fn_),##__VA_ARGS__); \
            SEGGER_RTT_SetTerminal(0); \
            xTaskResumeAll(); \
        } \
    } while(0)

#define APP_ERR_TRAP(err) \
    do { \
    	if(err) {\
            vTaskSuspendAll(); \
            SEGGER_RTT_SetTerminal(TERMINAL_NUMBER); \
    		SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", err);\
    		__asm("BKPT #0\n"); \
            SEGGER_RTT_SetTerminal(0); \
            xTaskResumeAll(); \
        } /* trap upon the error  */ \
    } while(0)

#endif /* USE_ANOTHER_TERMINAL_H */
