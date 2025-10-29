/***********************************************************************************************************************
 * File Name    : r_riic_greenpack.c
 * Description  : Contains data structures and functions used in r_riic_eeprom.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "r_riic_greenpack.h"


/*******************************************************************************************************************//**
 * @addtogroup r_iic_master_ep
 * @{
 **********************************************************************************************************************/

/*
 * Global Variables
 */

/* Reading IIC call back event through i2c_master callback */
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;

/*
 * private function declarations
 */
static fsp_err_t validate_i2c_event(void);
#if (0)  /* resolve warning:declared 'static' but never defined */
static fsp_err_t ackpoll(void);
#endif

/*******************************************************************************************************************//**
 *  @brief       initialize IIC master module
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS                  Upon successful open and start of timer
 *  @retval      Any Other Error code apart from FSP_SUCCESS is  Unsuccessful open or start
 **********************************************************************************************************************/
fsp_err_t init_greenpack(void)
{
    fsp_err_t err     = FSP_SUCCESS;

    /* opening IIC master module */
    err = R_RIIC_MASTER_Open (&g_i2c_greenpack_ctrl, &g_i2c_greenpack_cfg);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    return err;
}


/*******************************************************************************************************************//**
 *  @brief       DeInitialize IIC master module
 *  @param[IN]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_greenpack(void)
{
    fsp_err_t err     = FSP_SUCCESS;

    /* close open modules */
    err =  R_RIIC_MASTER_Close (&g_i2c_greenpack_ctrl);

    if (FSP_SUCCESS != err)
    {

    }
}
/*******************************************************************************************************************//**
 *  @brief       GreenPack Control
 *  @param[IN]
 *  @retval      FSP_SUCCESS
 *  @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful write or read
 **********************************************************************************************************************/
fsp_err_t write_greenpack(uint8_t *data, uint32_t size)
{
    fsp_err_t err   = FSP_SUCCESS;

    /* reset i2c_event */
    i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Write data to GreenPack  */
    err = R_RIIC_MASTER_Write (&g_i2c_greenpack_ctrl, data, size, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    else
    {
        err = validate_i2c_event();
        if (FSP_ERR_TRANSFER_ABORTED == err)
        {
            return err;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief       GreenPack Control
 *  @param[IN]
 *  @retval      FSP_SUCCESS
 *  @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful write or read
 **********************************************************************************************************************/
fsp_err_t read_greenpack(uint8_t adr, uint8_t *data, uint32_t size)
{
    fsp_err_t err   = FSP_SUCCESS;

    /* reset i2c_event */
    i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Write data to GreenPack  */
    err = R_RIIC_MASTER_Write (&g_i2c_greenpack_ctrl, &adr, 1, true);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    else
    {
        err = validate_i2c_event();
        if (FSP_ERR_TRANSFER_ABORTED == err)
        {
            return err;
        }
    }

    /* reset i2c_event */
    i2c_event = (i2c_master_event_t)RESET_VALUE;

    /* Read data to GreenPack  */
    err = R_RIIC_MASTER_Read (&g_i2c_greenpack_ctrl, data, size, false);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    else
    {
        err = validate_i2c_event();
        if (FSP_ERR_TRANSFER_ABORTED == err)
        {
            return err;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief      User callback function
 *  @param[in]  p_args
 *  @retval None
 **********************************************************************************************************************/
void i2c_greenpack_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* capture callback event for validating the i2c transfer event*/
        i2c_event = p_args->event;
    }
}

/*******************************************************************************************************************//**
 *  @brief       Validate i2c receive/transmit  based on required write read operation
 *
 *  @param[in]   None
 *
 *  @retval      FSP_SUCCESS                       successful event receiving returns FSP_SUCCESS
 *               FSP_ERR_TRANSFER_ABORTED          Either on timeout elapsed or received callback event is
 *                                                 I2C_MASTER_EVENT_ABORTED
 **********************************************************************************************************************/
static fsp_err_t validate_i2c_event(void)
{
    uint32_t volatile local_time_out = 0xFFFFFFFF;

    do
    {
        /* This is to avoid infinite loop */
        --local_time_out;

        if(TIMEOUT_VALUE == local_time_out)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }

    }while(i2c_event == RESET_VALUE);

    if(i2c_event != I2C_MASTER_EVENT_ABORTED)
    {
        i2c_event = (i2c_master_event_t)RESET_VALUE;  // Make sure this is always Reset before return
        return FSP_SUCCESS;
    }

    i2c_event = (i2c_master_event_t)RESET_VALUE; // Make sure this is always Reset before return
    return FSP_ERR_TRANSFER_ABORTED;
}


/*******************************************************************************************************************//**
 * @} (end addtogroup r_iic_master_ep)
 **********************************************************************************************************************/
