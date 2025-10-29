/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
 **********************************************************************************************************************/
#include "awo_task.h"
#include <stdio.h>
#include "hal_data.h"
#include "../all/bsp_clocks.h"
#include "r_riic_pmic.h"
#include "r_riic_greenpack.h"
#define LOW_POWER_MODE    2   // 1:VBAT 2:AWO

#define CPG_CPG_PL6_DDIV_DIVPL6A_1_01 (0)
#define CPG_CPG_PL6_DDIV_DIVPL6A_1_02 (1)
#define CPG_CPG_PL6_DDIV_DIVPL6A_1_04 (2)
#define CPG_CPG_PL6_DDIV_DIVPL6A_1_08 (3)
#define CPG_CPG_PL6_DDIV_DIVPL6A_1_32 (4)

#define VBATT_BKR0                  (*(volatile uint32_t *)0x4005C080)

#define CPG_CLKON_AXI_ACPU_BUS      (*(volatile uint32_t *)0x410105B4)
#define CPG_CLKON_AXI_COM_BUS       (*(volatile uint32_t *)0x410105BC)
#define CPG_CLKON_PERI_COM          (*(volatile uint32_t *)0x410105C4)
#define CPG_CLKON_PERI_DDR          (*(volatile uint32_t *)0x410105D8)
#define CPG_CLKMON_AXI_ACPU_BUS     (*(volatile uint32_t *)0x41010734)
#define CPG_CLKMON_AXI_COM_BUS      (*(volatile uint32_t *)0x4101073C)
#define CPG_CLKMON_PERI_COM         (*(volatile uint32_t *)0x41010744)
#define CPG_CLKMON_PERI_DDR         (*(volatile uint32_t *)0x41010758)
#define CPG_RST_AXI_ACPU_BUS        (*(volatile uint32_t *)0x410108B4)
#define CPG_RST_AXI_COM_BUS         (*(volatile uint32_t *)0x410108BC)
#define CPG_RST_PERI_COM            (*(volatile uint32_t *)0x410108C4)
#define CPG_RST_PERI_DDR            (*(volatile uint32_t *)0x410108D8)
#define CPG_RSTMON_AXI_ACPU_BUS     (*(volatile uint32_t *)0x41010A34)
#define CPG_RSTMON_AXI_COM_BUS      (*(volatile uint32_t *)0x41010A3C)
#define CPG_RSTMON_PERI_COM         (*(volatile uint32_t *)0x41010A44)
#define CPG_RSTMON_PERI_DDR         (*(volatile uint32_t *)0x41010A58)

#define SYS_LP_CTL0                 (*(volatile uint32_t *)0x41020D00)
#define SYS_LP_CTL1                 (*(volatile uint32_t *)0x41020D04)
#define SYS_LP_CTL6                 (*(volatile uint32_t *)0x41020D18)
#define SYS_LP_CA55CK_CTL2          (*(volatile uint32_t *)0x41020D3C)
#define SYS_LP_CA55CK_CTL3          (*(volatile uint32_t *)0x41020D40)
#define SYS_LP_CM33FPUCTL0          (*(volatile uint32_t *)0x41020D54)
#define ISO_IOBUF_SE18_CTRL         (*(volatile uint32_t *)0x41020D6C)

#define OTP_CTRL                    (*(volatile uint32_t *)0x41860000)

#define TZC_GATE_KEEP_SRAM_A0       (*(volatile uint32_t *)0x41200008)
#define TZC_SPECUL_CTL_SRAM_A0      (*(volatile uint32_t *)0x4120000C)
#define TZC_RGN_ATTR_SRAM_A0        (*(volatile uint32_t *)0x41200110)
#define TZC_RGN_ID_ACC_SRAM_A0      (*(volatile uint32_t *)0x41200114)

#define TZC_GATE_KEEP_SRAM_A1       (*(volatile uint32_t *)0x41210008)
#define TZC_SPECUL_CTL_SRAM_A1      (*(volatile uint32_t *)0x4121000C)
#define TZC_RGN_ATTR_SRAM_A1        (*(volatile uint32_t *)0x41210110)
#define TZC_RGN_ID_ACC_SRAM_A1      (*(volatile uint32_t *)0x41210114)

#define TZC_GATE_KEEP_SRAM_A2       (*(volatile uint32_t *)0x41220008)
#define TZC_SPECUL_CTL_SRAM_A2      (*(volatile uint32_t *)0x4122000C)
#define TZC_RGN0_ATTR_SRAM_A2       (*(volatile uint32_t *)0x41220110)
#define TZC_RGN0_ID_ACC_SRAM_A2     (*(volatile uint32_t *)0x41220114)
#define TZC_RGN1_BASE_SRAM_A2       (*(volatile uint32_t *)0x41220120)
#define TZC_RGN1_TOP_SRAM_A2        (*(volatile uint32_t *)0x41220128)
#define TZC_RGN1_ATTR_SRAM_A2       (*(volatile uint32_t *)0x41220130)
#define TZC_RGN1_ID_ACC_SRAM_A2     (*(volatile uint32_t *)0x41220134)


#define CM33_BACKUP_ADDR            (void*)(0x60000)
#define CA55_BUCKUP_ADDR            (void*)(0xA0000)
#define CA55_BACKUP_SIZE            0x30000

#define GREEN_PAK_F4_SLP_EN 0x01
#define GREEN_PAK_F1_SLPBTN_INT_STS 0x20
#define GREEN_PAK_F1_PG_INT_STS 0x40

#define POWER_ON_UNKNOWN 0xFF
#define POWER_ON_G3S_RESUME 0x55
#define POWER_ON_NORMAL 0xAA

#define VBATT_AWO_VALUE 0xA5

#define USE_DEBUG_PRINT (0)

// resolve warning: implicit declaration of function
void stdio_close(void);
int stdio_read(uint8_t *pbyBuffer, uint32_t uiCount);
int stdio_write(const uint8_t * pbyBuffer, uint32_t uiCount);

#define printf(a)  stdio_write((const uint8_t*)a, sizeof(a))

/* AWO Thread entry function */
/* pvParameters contains TaskHandle_t */
static uint8_t string_buf[100];
uint32_t indicate_data __attribute__((section(".indicate")));

extern bsp_leds_t g_bsp_leds;

static void timer_isr (timer_callback_args_t * const p_unused)
{
    (void) p_unused;

    /* Holds level to set for pins */
    static bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;

    /* Update all board LEDs */
    for (uint32_t i = 0; i < g_bsp_leds.led_count; i++)
    {
        /* Get pin to toggle */
        uint32_t pin = g_bsp_leds.p_leds[i];

        /* Write to this pin */
        R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);
    }

    /* Toggle level for next write */
    if (BSP_IO_LEVEL_LOW == pin_level)
    {
        pin_level = BSP_IO_LEVEL_HIGH;
    }
    else
    {
        pin_level = BSP_IO_LEVEL_LOW;
    }
}

static void backup_sram(uint32_t *sys_ca55_cfg_rval_back, uint32_t *sys_ca55_cfg_rvah_back)
{
    memcpy(CM33_BACKUP_ADDR,CA55_BUCKUP_ADDR,CA55_BACKUP_SIZE);

    /* backup reset vector address */
    *sys_ca55_cfg_rval_back = R_SYSC->SYS_CA55_CFG_RVAL0;
    *sys_ca55_cfg_rvah_back = R_SYSC->SYS_CA55_CFG_RVAH0;
}

static void set_pmic()
{
#if (LOW_POWER_MODE == 2)
    {
        /* Init PMIC for AWO mode */
        printf("\r\nCM33:Init PMIC for AWO mode\r\n");
        uint8_t data1[2] = {0x35, 0x07}; // D0.9V_AWO(BUCK4)
        uint8_t data2[2] = {0x4B, 0x0C}; // D1.8V_AWO(LDO1)
        uint8_t data3[2] = {0x50, 0x0F}; // D3.3V_AWO(LDO2)
        uint8_t data4[2] = {0x80, 0x3E}; // PRST_N

        init_pmic();

        write_pmic(data1, 2);
        write_pmic(data2, 2);
        write_pmic(data3, 2);
        write_pmic(data4, 2);

        deinit_pmic();
    }
#endif
}

static void wait_ca55_sleep()
{
    SYS_LP_CTL0 = 0x00000001;

    while( R_SYSC->SYS_PD_ISO_CTRL != 0x000000000)
    {
        vTaskDelay(1);
    }

    while(0x00000000 != SYS_LP_CTL1)
    {
        vTaskDelay(1);
    }
    /*
     ---- ---- --00 0000 ---- ---- ---- ---- : PSTATE0_SET
     ---- ---- ---- ---- ---- ---- ---- ---0 : PREQ0_SET  */
    R_CPG->CPG_CORE0_PCHCTL = 0x00000000;
    while (0x00000000 != R_CPG->CPG_CORE0_PCHMON)
    {
        /* Do nothing */
        vTaskDelay(1);
    }
    /*
     ---- ---- -000 0000 ---- ---- ---- ---- : PSTATE0_SET
     ---- ---- ---- ---- ---- ---- ---- ---1 : PREQ_SET  */
    R_CPG->CPG_CLUSTER_PCHCTL = 0x00000001;
    while ((0x00000001 != R_CPG->CPG_CLUSTER_PCHMON) && (0x00000002 != R_CPG->CPG_CLUSTER_PCHMON))
    {
        /* Do nothing */
        vTaskDelay(1);
    }
    /*
     ---- ---- -000 0000 ---- ---- ---- ---- : PSTATE0_SET
     ---- ---- ---- ---- ---- ---- ---- ---0 : PREQ_SET  */
    R_CPG->CPG_CLUSTER_PCHCTL = 0x00000000;
    while (0x00000000 != R_CPG->CPG_CLUSTER_PCHMON)
    {
        /* Do nothing */
        vTaskDelay(1);
    }
}

static void q_channel_stop()
{
    SYS_LP_CA55CK_CTL2 = 0x00000006;
    while (0x00000006 != SYS_LP_CA55CK_CTL3)
    {
        /* Do nothing */
        vTaskDelay(1);
    }

    SYS_LP_CA55CK_CTL2 = 0x00000000;

    while(0x00000000 != SYS_LP_CA55CK_CTL3)
    {
        /* Do nothing */
        vTaskDelay(1);
    }
}

static void stop_ca55_clock()
{
    /*
     ---- ---- --1- ---- ---- ---- --0- ---- : CLK5_ON CA55_TSCLK
     ---- ---- ---1 ---- ---- ---- ---0 ---- : CLK4_ON CA55_AC
     ---- ---- ---- 1--- ---- ---- ---- 0--- : CLK3_ON CA55_GICCLK
     ---- ---- ---- -1-- ---- ---- ---- -0-- : CLK2_ON CA55_ATCLK
     ---- ---- ---- --1- ---- ---- ---- --0- : CLK1_ON CA55_PCLK
     ---- ---- ---- ---1 ---- ---- ---- ---0 : CLK0_ON CA55_SCLK/PERICLK  */
    R_CPG->CPG_CLKON_CA55     = 0x003F0000;
    while(0x00000000 != R_CPG->CPG_CLKMON_CA55)
    {
        vTaskDelay(1);
    }
}

static void assert_ca55_reset()
{
    R_CPG->CPG_RST_CA55 = 0x1FF50000;
    while (0x00001FFF != R_CPG->CPG_RSTMON_CA55)
    {
        vTaskDelay(1);
    }
}

static void stop_module()
{
    /*
     ---- ---- ---- ---1 ---- ---- ---- ---1 : BUS_ACPU_MSTOP_MXSRAM_A
     */
    R_CPG->CPG_BUS_ACPU_MSTOP     = 0x000F000F;

    /*
     ---- ---1 ---- ---- ---- ---1 ---- ---- : BUS_PERI_COM_MSTOP8_MXCOM
     ---- ---- 1--- ---- ---- ---- 1--- ---- : BUS_PERI_COM_MSTOP7_MHUSB2_1
     ---- ---- -1-- ---- ---- ---- -1-- ---- : BUS_PERI_COM_MSTOP6_MHUSB2_0_F
     ---- ---- --1- ---- ---- ---- --1- ---- : BUS_PERI_COM_MSTOP5_MHUSB2_0_H
     ---- ---- ---1 ---- ---- ---- ---1 ---- : BUS_PERI_COM_MSTOP4_MPUSBT
     ---- ---- ---- 1--- ---- ---- ---- 1--- : BUS_PERI_COM_MSTOP3_MPGIGE_1
     ---- ---- ---- -1-- ---- ---- ---- -1-- : BUS_PERI_COM_MSTOP2_MPGIGE_0
     ---- ---- ---- --1- ---- ---- ---- --1- : BUS_PERI_COM_MSTOP1_MXSDHI_1
     ---- ---- ---- ---1 ---- ---- ---- ---1 : BUS_PERI_COM_MSTOP0_MXSDHI_0 */
    R_CPG->CPG_BUS_PERI_COM_MSTOP = 0x0FFF0FFF;

    /*
     ---- ---- ---- --1- ---- ---- ---- --1- : BUS_PERI_DDR_MSTOP1_MXMEMC_REG
     ---- ---- ---- ---1 ---- ---- ---- ---1 : BUS_PERI_DDR_MSTOP0_MPPHY */
    R_CPG->CPG_BUS_PERI_DDR_MSTOP = 0x00030003;

    /*
     ---- ---- ---- -1-- ---- ---- ---- -1-- : BUS_TZCDDR_MSTOP2
     ---- ---- ---- --1- ---- ---- ---- --1- : BUS_TZCDDR_MSTOP1
     ---- ---- ---- ---1 ---- ---- ---- ---1 : BUS_TZCDDR_MSTOP0 */
    R_CPG->CPG_BUS_TZCDDR_MSTOP   = 0x000F000F;

    /*
     ---- ---- ---- ---1 ---- ---- ---- ---1 : MHU_MSTOP */
    R_CPG->CPG_MHU_MSTOP           = 0x00010001;

    /*
     ---- ---- ---- ---1 ---- ---- ---- ---1 : PWRDN_MSTOP */
    R_CPG->CPG_PWRDN_MSTOP           = 0x00010001;
}

static void stop_clock_assert_reset()
{
    CPG_CLKON_AXI_ACPU_BUS = 0x007F0000;
    CPG_CLKON_AXI_COM_BUS = 0x03030000;
    CPG_CLKON_PERI_COM = 0x03030000;
    CPG_CLKON_PERI_DDR = 0x00010000;

    /*
    ---- ---- ---- ---1 ---- ---- ---- ---1 : CLK0_ON BUS_TZCDDR_PCLK  */
    R_CPG->CPG_CLKON_AXI_TZCDDR = 0x00010000;

    /*
     ---- ---- --1- ---- ---- ---- --0- ---- : CLK9_ON CM33_FPU_TSCLK
     ---- ---- ---1 ---- ---- ---- ---0 ---- : CLK8_ON CM33_FPU_CLKIN
     ---- ---- ---- --0- ---- ---- ---- --0- : CLK1_ON CM33_TSCLK
     ---- ---- ---- ---0 ---- ---- ---- ---0 : CLK0_ON CM33_CLKIN */
    R_CPG->CPG_CLKON_CM33 = 0x03000000;

    /*
     1--- ---- ---- ---- 1--- ---- ---- ---- : bit15 SDHI2
     -1-- ---- ---- ---- -1-- ---- ---- ---- : bit14 SDHI1
     --1- ---- ---- ---- --1- ---- ---- ---- : bit13 SDHI0
     ---1 ---- ---- ---- ---1 ---- ---- ---- : bit12 GBE1
     ---- 1--- ---- ---- ---- 1--- ---- ---- : bit11 GBE0
     ---- -1-- ---- ---- ---- -1-- ---- ---- : bit10 USB_APB
     ---- --1- ---- ---- ---- --1- ---- ---- : bit9 USB0
     ---- ---1 ---- ---- ---- ---1 ---- ---- : bit8 USB1
     ---- ---- ---- ---- ---- ---- ---- ---- : bit7 -----
     ---- ---- -0-- ---- ---- ---- -0-- ---- : bit6 WDT(Ch2)
     ---- ---- --0- ---- ---- ---- --0- ---- : bit5 WDT(Ch0)
     ---- ---- ---0 ---- ---- ---- ---0 ---- : bit4 IM33_FPU
     ---- ---- ---- 0--- ---- ---- ---- 0--- : bit3 IA55
     ---- ---- ---- -1-- ---- ---- ---- -1-- : bit2 GIC
     ---- ---- ---- --1- ---- ---- ---- --1- : bit1 MHU
     ---- ---- ---- ---1 ---- ---- ---- ---1 : bit0 SRAM_ACPU  */
    R_CPG->CPG_PWRDN_IP1 = 0xFF07FF07;
    /*
    ---- ---- ---1 ---- ---- ---- ---1 ---- : bit4 RSIPG
    ---- ---- ---- 1--- ---- ---- ---- 1--- : bit3 OTFDE_DDR
    ---- ---- ---- -1-- ---- ---- ---- -1-- : bit2 OTFDE_DDR
    ---- ---- ---- --1- ---- ---- ---- --1- : bit1 AXI_TZCDDR
    ---- ---- ---- ---1 ---- ---- ---- ---1 : bit0 DDR   */
    R_CPG->CPG_PWRDN_IP2 = 0x001F001F;

    /*
    ---- ---- ---- ---1 ---- ---- ---- ---1 : PWR DN_CLKON  */
    R_CPG->CPG_PWRDN_CLKON = 0x00010001;

    /*
    ---- ---- ---- ---1 ---- ---- ---- ---1 : PWR DN_CLKON  */
    R_CPG->CPG_PWRDN_RST = 0x00010001;

    /*
     ---- ---- -1-- ---- ---- ---- -1-- ---- : UNIT2_RSTB CM33_FPU_MISCRESETN
     ---- ---- --1- ---- ---- ---- --1- ---- : UNIT1_RSTB CM33_FPU_NSYSRESET
     ---- ---- ---1 ---- ---- ---- ---1 ---- : UNIT0_RSTB CM33_FPU_NPORESET
     ---- ---- ---- --0- ---- ---- ---- --0- : UNIT2_RSTB CM33_MISCRESETN
     ---- ---- ---- --0- ---- ---- ---- --0- : UNIT1_RSTB CM33_NSYSRESET
     ---- ---- ---- ---0 ---- ---- ---- ---0 : UNIT0_RSTB CM33_NPORESET */
    R_CPG->CPG_RST_CM33 = 0x07000000;

    CPG_RST_AXI_ACPU_BUS = 0x00010000;
    CPG_RST_AXI_COM_BUS = 0x00030000;
    CPG_RST_PERI_COM = 0x00030000;
    CPG_RST_PERI_DDR = 0x00010000;

    /*
      ---- ---- ---- 1--- ---- ---- ---- 1--- : bit3 BUS_TZCDDR_ARESET2N
      ---- ---- ---- -1-- ---- ---- ---- -1-- : bit2 BUS_TZCDDR_ARESET1N
      ---- ---- ---- --1- ---- ---- ---- --1- : bit1 BUS_TZCDDR_ARESET0N
      ---- ---- ---- ---1 ---- ---- ---- ---1 : bit0 BUS_TZCDDR_PRESETN  */
    R_CPG->CPG_RST_AXI_TZCDDR = 0x000F0000;

    R_SYSC->SYS_USB_PWRRDY      = 0x00000001;
    R_SYSC->SYS_PCIE_RST_RSM_B      = 0x00000001;
    R_CPG->CPG_RST_PCI = 0x00000001;
    while(0x0000007F != R_CPG->CPG_RSTMON_PCI)
    {
        vTaskDelay(1);
    }
}

static void sleep_enable_on_greenpack()
{
#if (LOW_POWER_MODE == 1)
        // VBAT
        {
            uint8_t data1[2] = {0xB0, 0xFF};
            uint8_t data2[2] = {0xF4, 0x31};

            init_greenpack();

            write_greenpack(data1, 2);
            write_greenpack(data2, 2);

            deinit_greenpack();
        }
#endif

#if (LOW_POWER_MODE == 2)
        // AWO
        {
            uint8_t data2[2] = {0xF4, 0x31};
            uint8_t data3[2] = {0xAD, 0xFF};
            uint8_t greenpak_f4[2]={0};
            fsp_err_t err_f4;

            init_greenpack();
            write_greenpack(data3, 2);
            do
            {
                vTaskDelay(1);
                err_f4 = read_greenpack(0xF4, greenpak_f4, 2);
            } while(err_f4 !=FSP_SUCCESS);
            data2[1] = greenpak_f4[0] | 0x01;
            write_greenpack(data2, 2);
        }
#endif
}

static void sleep_enable_off_greenpack()
{
#if (LOW_POWER_MODE == 2)
    uint8_t data2[2] = {0xF4, 0x30};
    uint8_t data3[2] = {0xAD, 0xF8};
    uint8_t greenpak_f4[2]={0};
    uint8_t greenpak_f2[2]={0};
    fsp_err_t err_f4;
    fsp_err_t err_f2;
    do
    {
        vTaskDelay(1);
        err_f4 = read_greenpack(0xF4, greenpak_f4, 2);
    } while(err_f4 !=FSP_SUCCESS);
    data2[1] = greenpak_f4[0] & 0xFE;
    write_greenpack(data2, 2);
    do
    {
        vTaskDelay(1);
        err_f2 = read_greenpack(0xF2, greenpak_f2, 2);
    } while ((err_f2!=FSP_SUCCESS)||((greenpak_f2[0]&0x08)==0));
    write_greenpack(data3, 2);
#endif
}

static void wait_pd_isovcc_stable()
{
    /* Wait till PD ISOVCC */
    vTaskDelay(5);          /* 10000 * 500ns = 5ms */



    /*
    ---- ---- ---1 ---- ---- ---- ---1 ---- : bit4 RSIPG
    ---- ---- ---- 1--- ---- ---- ---- 1--- : bit3 OTFDE_DDR
    ---- ---- ---- -1-- ---- ---- ---- -1-- : bit2 OTFDE_DDR
    ---- ---- ---- --1- ---- ---- ---- --1- : bit1 AXI_TZCDDR
    ---- ---- ---- ---1 ---- ---- ---- ---0 : bit0 DDR   */
    R_CPG->CPG_PWRDN_IP2 = 0x001F001E;

    /*
    ---- ---- ---- 0--- ---- ---- ---- 0--- : CLK3_ON DDR_AXI1_ACLK
    ---- ---- ---- -0-- ---- ---- ---- -0-- : CLK2_ON DDR_AXI0_ACLK
    ---- ---- ---- --0- ---- ---- ---- --0- : CLK1_ON DDR_REG_ACLK
    ---- ---- ---- ---1 ---- ---- ---- ---1 : CLK0_ON DDR_PCLK  */
    R_CPG->CPG_CLKON_DDR = 0x00010001;

    /*
    ---- ---- ---- ---1 ---- ---- ---- ---0 : PWR DN_CLKON  */
    R_CPG->CPG_PWRDN_CLKON = 0x00010000;

    /*
    ---- ---- ---- ---- ---- ---- ---- 0--- : CLK3_MON DDR_AXI1_ACLK
    ---- ---- ---- ---- ---- ---- ---- -0-- : CLK2_MON DDR_AXI0_ACLK
    ---- ---- ---- ---- ---- ---- ---- --0- : CLK1_MON DDR_REG_ACLK
    ---- ---- ---- ---- ---- ---- ---- ---1 : CLK0_MON DDR_PCLK  */
    while( 0x00000001 != R_CPG->CPG_CLKMON_DDR )
    {
        break;
    }

    /*
    ---- ---- ---- 0--- ---- ---- ---- 0--- : CLK3_ON DDR_AXI1_ACLK
    ---- ---- ---- -0-- ---- ---- ---- -0-- : CLK2_ON DDR_AXI0_ACLK
    ---- ---- ---- --0- ---- ---- ---- --0- : CLK1_ON DDR_REG_ACLK
    ---- ---- ---- ---1 ---- ---- ---- ---0 : CLK0_ON DDR_PCLK  */
    R_CPG->CPG_CLKON_DDR = 0x00010000;
}

static void pd_isovcc_power_supply()
{
    /* SYS_PD_ISO_CTRL
     ---- ---- ---- ---- ---- ---- ---- ---0 : PD_ISOVCC_ISOEN */
    R_SYSC->SYS_PD_ISO_CTRL = 0x00000000;

    if (0x00000000 == R_SYSC->SYS_PD_ISO_CTRL)
    {
        /* sim_milestone: 0x0310040A */
    }
    else
    {
        /* sim_milestone: 0x03100C0A */
    }
}

static void restore_sram_repair_information()
{
    /*
    ---- ---- ---- ---- ---- ---- ---0 ---- : ACCL
    ---- ---- ---- ---- ---- ---- ---- ---1 : PWR  */
    OTP_CTRL = 0x00000001;

    /*
    ---- ---- ---- ---- ---- ---- ---- -110 : BISR_DOM_EN  */
    R_CPG->CPG_RET_FUNC2 = 0x00000006;
    do {
        /*
        ---- ---- ---- ---1 ---- ---- ---- ---0 : BISR_RSTB  */
        R_CPG->CPG_RET_FUNC1 = 0x00010000;
        vTaskDelay(1);

    } while(0x000000000 !=  R_CPG->CPG_RET_FUNC1);

    /*
    ---- ---- ---- ---1 ---- ---- ---- ---1 : BISR_RSTB  */
    R_CPG->CPG_RET_FUNC1 = 0x00010001;

    /*
    ---- ---- ---- ---0 ---- ---- ---- ---1 : BISR_REPAIR_END  */
    while(0x00000001 != R_CPG->CPG_RET_FUNC3)
    {
        /* Do nothing */
        vTaskDelay(1);
    }

    /*
    ---- ---- ---- ---- ---- ---- ---1 ---- : ACCL
    ---- ---- ---- ---- ---- ---- ---- ---0 : PWR  */
    OTP_CTRL = 0x00000010;
}

static void negate_ip_power_down()
{
    /*
     1--- ---- ---- ---- 0--- ---- ---- ---- : bit15 SDHI2
     -1-- ---- ---- ---- -0-- ---- ---- ---- : bit14 SDHI1
     --1- ---- ---- ---- --0- ---- ---- ---- : bit13 SDHI0
     ---1 ---- ---- ---- ---0 ---- ---- ---- : bit12 GBE1
     ---- 1--- ---- ---- ---- 0--- ---- ---- : bit11 GBE0
     ---- -1-- ---- ---- ---- -0-- ---- ---- : bit10 USB_APB
     ---- --1- ---- ---- ---- --0- ---- ---- : bit9 USB0
     ---- ---1 ---- ---- ---- ---0 ---- ---- : bit8 USB1
     ---- ---- ---- ---- ---- ---- ---- ---- : bit7 -----
     ---- ---- -0-- ---- ---- ---- -0-- ---- : bit6 WDT(Ch2)
     ---- ---- --0- ---- ---- ---- --0- ---- : bit5 WDT(Ch0)
     ---- ---- ---0 ---- ---- ---- ---0 ---- : bit4 IM33_FPU
     ---- ---- ---- 0--- ---- ---- ---- 0--- : bit3 IA55
     ---- ---- ---- -0-- ---- ---- ---- -0-- : bit2 GIC
     ---- ---- ---- --1- ---- ---- ---- --0- : bit1 MHU
     ---- ---- ---- ---1 ---- ---- ---- ---0 : bit0 SRAM_ACPU  */
    R_CPG->CPG_PWRDN_IP1 = 0xFF030000;

    /*
    ---- ---- ---1 ---- ---- ---- ---0 ---- : bit4 RSIPG
    ---- ---- ---- 1--- ---- ---- ---- 0--- : bit3 OTFDE_DDR
    ---- ---- ---- -1-- ---- ---- ---- -0-- : bit2 OTFDE_DDR
    ---- ---- ---- --1- ---- ---- ---- --0- : bit1 AXI_TZCDDR
    ---- ---- ---- ---1 ---- ---- ---- ---0 : bit0 DDR   */
    R_CPG->CPG_PWRDN_IP2 = 0x001F0000;
}

static void start_system_bus_clock()
{
    /*
    ---- ---- ---- ---1 ---- ---- ---- ---0 : PWR DN_CLKON  */
    R_CPG->CPG_PWRDN_CLKON = 0x00010000;

    /*
    ---- ---- ---- 1--- ---- ---- ---- 1--- : CLK3_ON USB_PCLK
    ---- ---- ---- -1-- ---- ---- ---- -1-- : CLK2_ON USB_U2P_EXR_CPUCLK
    ---- ---- ---- --1- ---- ---- ---- --1- : CLK1_ON USB_U2H1_HCLK
    ---- ---- ---- ---1 ---- ---- ---- ---1 : CLK0_ON USB_U2H0_HCLK  */
    R_CPG->CPG_CLKON_USB = 0x000F000F;

    /*
    ---- ---- ---- ---- ---- ---- ---- 1--- : CLK3_ON USB_PCLK
    ---- ---- ---- ---- ---- ---- ---- -1-- : CLK2_ON USB_U2P_EXR_CPUCLK
    ---- ---- ---- ---- ---- ---- ---- --1- : CLK1_ON USB_U2H1_HCLK
    ---- ---- ---- ---- ---- ---- ---- ---1 : CLK0_ON USB_U2H0_HCLK  */
    while (0x0000000F != R_CPG->CPG_CLKMON_USB)
    {
        ;
    }

    /*
    ---- ---- ---- 1--- ---- ---- ---- 0--- : CLK3_ON USB_PCLK
    ---- ---- ---- -1-- ---- ---- ---- -0-- : CLK2_ON USB_U2P_EXR_CPUCLK
    ---- ---- ---- --1- ---- ---- ---- --0- : CLK1_ON USB_U2H1_HCLK
    ---- ---- ---- ---1 ---- ---- ---- ---0 : CLK0_ON USB_U2H0_HCLK  */
    R_CPG->CPG_CLKON_USB = 0x000F0000;

    /*
    ---- ---- ---- ---- ---- ---- ---- ---0 : PWRRDY_N  */
    R_SYSC->SYS_USB_PWRRDY = 0x00000000;

    /*
    ---- ---- ---- 0--- ---- ---- ---- 1--- : UNIT3_RSTB USB_PRESETN
    ---- ---- ---- -0-- ---- ---- ---- -1-- : UNIT2_RSTB USB_U2P_EXL_SYSRST
    ---- ---- ---- --0- ---- ---- ---- --1- : UNIT1_RSTB USB_U2H1_HRESETN
    ---- ---- ---- ---0 ---- ---- ---- ---1 : UNIT0_RSTB USB_U2H0_HRESETN  */
    R_CPG->CPG_RST_USB = 0x0000000F;

    /*
    ---- ---- ---- ---- ---- ---- ---- 1--- : UNIT3_RSTB USB_PRESETN
    ---- ---- ---- ---- ---- ---- ---- -1-- : UNIT2_RSTB USB_U2P_EXL_SYSRST
    ---- ---- ---- ---- ---- ---- ---- --1- : UNIT1_RSTB USB_U2H1_HRESETN
    ---- ---- ---- ---- ---- ---- ---- ---1 : UNIT0_RSTB USB_U2H0_HRESETN  */
    while (0x0000000F != R_CPG->CPG_RSTMON_USB)
    {
        break;
        ;
    }

    /*
    ---- ---- ---- 1111 ---- ---- ---- 1111 : CLK0_ON BUS_TZCDDR_PCLK  */
    R_CPG->CPG_CLKON_AXI_TZCDDR = 0x000F000F;

    CPG_CLKON_AXI_ACPU_BUS = 0x007F007F;
    CPG_CLKON_AXI_COM_BUS = 0x03030303;
    CPG_CLKON_PERI_COM = 0x03030303;
    CPG_CLKON_PERI_DDR = 0x00010001;
    CPG_CLKON_PERI_DDR = 0x00010001;

    /*
     1--- ---- ---- ---- 0--- ---- ---- ---- : bit15 SDHI2
     -1-- ---- ---- ---- -0-- ---- ---- ---- : bit14 SDHI1
     --1- ---- ---- ---- --0- ---- ---- ---- : bit13 SDHI0
     ---1 ---- ---- ---- ---0 ---- ---- ---- : bit12 GBE1
     ---- 1--- ---- ---- ---- 0--- ---- ---- : bit11 GBE0
     ---- -1-- ---- ---- ---- -0-- ---- ---- : bit10 USB_APB
     ---- --1- ---- ---- ---- --0- ---- ---- : bit9 USB0
     ---- ---1 ---- ---- ---- ---0 ---- ---- : bit8 USB1
     ---- ---- ---- ---- ---- ---- ---- ---- : bit7 -----
     ---- ---- -0-- ---- ---- ---- -0-- ---- : bit6 WDT(Ch2)
     ---- ---- --0- ---- ---- ---- --0- ---- : bit5 WDT(Ch0)
     ---- ---- ---0 ---- ---- ---- ---0 ---- : bit4 IM33_FPU
     ---- ---- ---- 1--- ---- ---- ---- 0--- : bit3 IA55
     ---- ---- ---- -1-- ---- ---- ---- -0-- : bit2 GIC
     ---- ---- ---- --1- ---- ---- ---- --0- : bit1 MHU
     ---- ---- ---- ---1 ---- ---- ---- ---0 : bit0 SRAM_ACPU  */
    R_CPG->CPG_PWRDN_IP1 = 0xFF070000;

    /*
    ---- ---- ---1 ---- ---- ---- ---0 ---- : bit4 RSIPG
    ---- ---- ---- 0--- ---- ---- ---- 0--- : bit3 OTFDE_DDR
    ---- ---- ---- -0-- ---- ---- ---- -0-- : bit2 OTFDE_DDR
    ---- ---- ---- --0- ---- ---- ---- --0- : bit1 AXI_TZCDDR
    ---- ---- ---- ---0 ---- ---- ---- ---0 : bit0 DDR   */
    R_CPG->CPG_PWRDN_IP2 = 0x00100000;

    /*
    ---- ---- ---- ---1 ---- ---- ---- ---0 : PWR DN_CLKON  */
    R_CPG->CPG_PWRDN_CLKON = 0x00010000;

    while(0x00000003 != R_CPG->CPG_CLKMON_CM33)
    {
        /* Do nothing */
        break;
    }
    while(0x00000007 != R_CPG->CPG_CLKMON_SRAM_ACPU)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000001 != R_CPG->CPG_CLKMON_GIC600)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_CLKMON_MHU)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000001 != R_CPG->CPG_CLKMON_DDR)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_CLKMON_SDHI)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_CLKMON_USB)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_CLKMON_ETH)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x0000007F != CPG_CLKMON_AXI_ACPU_BUS)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000303 != CPG_CLKMON_AXI_COM_BUS)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000303 != CPG_CLKMON_PERI_COM)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_CLKMON_PCI)
    {
        /* Do nothing */
        break;
        ;
    }
}

static void negate_system_bus_reset()
{
    CPG_RST_AXI_ACPU_BUS = 0x00010001;
    CPG_RST_AXI_COM_BUS = 0x00030003;
    CPG_RST_PERI_COM = 0x00030003;
    CPG_RST_PERI_DDR = 0x00010001;

    /*
     ---- ---- ---- 1--- ---- ---- ---- 1--- : bit3 BUS_TZCDDR_ARESET2N
     ---- ---- ---- -1-- ---- ---- ---- -1-- : bit2 BUS_TZCDDR_ARESET1N
     ---- ---- ---- --1- ---- ---- ---- --1- : bit1 BUS_TZCDDR_ARESET0N
     ---- ---- ---- ---1 ---- ---- ---- ---1 : bit0 BUS_TZCDDR_PRESETN  */
    R_CPG->CPG_RST_AXI_TZCDDR = 0x000F000F;

    /*
     ---- ---- ---- ---1 ---- ---- ---- ---0 : PWRDN_RST  */
    R_CPG->CPG_PWRDN_RST = 0x001F0000;      //@@@@@@

    while(0x00000000 != R_CPG->CPG_RSTMON_SRAM_ACPU)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_RSTMON_GIC600)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000001 != R_CPG->CPG_RSTMON_MHU)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x000001FF != R_CPG->CPG_RSTMON_DDR)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000007 != R_CPG->CPG_RSTMON_SDHI)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x0000000F != R_CPG->CPG_RSTMON_USB)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000003 != R_CPG->CPG_RSTMON_ETH)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != CPG_RSTMON_AXI_ACPU_BUS)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != CPG_RSTMON_AXI_COM_BUS)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != CPG_RSTMON_PERI_COM)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != CPG_RSTMON_PERI_DDR)
    {
        /* Do nothing */
        break;
        ;
    }
}

static void start_module()
{
    /*
     ---- ---- ---- 1111 ---- ---- ---- ---0 : MSTOP0_ON BUS_ACPU_MSTOP_MXSRAM_A */
    R_CPG->CPG_BUS_ACPU_MSTOP = 0x000F0000;
    R_CPG->CPG_BUS_PERI_COM_MSTOP = 0x0FFF0000;
    R_CPG->CPG_BUS_PERI_DDR_MSTOP = 0x00030000;
    R_CPG->CPG_BUS_TZCDDR_MSTOP = 0x000F0000;
    R_CPG->CPG_MHU_MSTOP = 0x00010000;
    R_CPG->CPG_PWRDN_MSTOP = 0x00010000;

    R_CPG->CPG_BUS_REG0_MSTOP = 0x00110000;
    R_CPG->CPG_BUS_MCPU3_MSTOP = 0x07FF0000;
    R_CPG->CPG_BUS_MCPU2_MSTOP = 0xFFFF0000;
    R_CPG->CPG_BUS_MCPU1_MSTOP = 0xFFFF0000;

    while(0x00000000 != R_CPG->CPG_BUS_ACPU_MSTOP)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_BUS_PERI_COM_MSTOP)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_BUS_PERI_DDR_MSTOP)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_BUS_TZCDDR_MSTOP)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_MHU_MSTOP)
    {
        /* Do nothing */
        break;
        ;
    }
    while(0x00000000 != R_CPG->CPG_PWRDN_MSTOP)
    {
        /* Do nothing */
        break;
        ;
    }
}

static void restore_sram(volatile uint32_t *dummy_read)
{
    TZC_RGN_ATTR_SRAM_A0   = 0xC000000Fu;  /* Secure read/write permit. Enable all filter unit */
    *dummy_read = TZC_RGN_ATTR_SRAM_A0;
    TZC_RGN_ID_ACC_SRAM_A0 = 0x000F000Fu;  /* NSAIDW/R<x>=0 to 3, read/write enable */
    *dummy_read = TZC_RGN_ID_ACC_SRAM_A0;
    TZC_GATE_KEEP_SRAM_A0  = 0x0000000Fu;  /* Open filter from #0 to #3 */
    *dummy_read = TZC_GATE_KEEP_SRAM_A0;
    TZC_SPECUL_CTL_SRAM_A0 = 0x00000003u;  /* Disable read/write access speculation */
    *dummy_read = TZC_SPECUL_CTL_SRAM_A0;

    TZC_RGN_ATTR_SRAM_A1   = 0xC000000Fu;  /* Secure read/write permit. Enable all filter unit */
    *dummy_read = TZC_RGN_ATTR_SRAM_A1;
    TZC_RGN_ID_ACC_SRAM_A1 = 0x000F000Fu;  /* NSAIDW/R<x>=0 to 3, read/write enable */
    *dummy_read = TZC_RGN_ID_ACC_SRAM_A1;
    TZC_GATE_KEEP_SRAM_A1  = 0x0000000Fu;  /* Open filter from #0 to #3 */
    *dummy_read = TZC_GATE_KEEP_SRAM_A1;
    TZC_SPECUL_CTL_SRAM_A1 = 0x00000003u;  /* Disable read/write access speculation */
    *dummy_read = TZC_SPECUL_CTL_SRAM_A1;

    TZC_RGN0_ATTR_SRAM_A2   = 0xC000000Fu;  /* Secure read/write permit. Enable all filter unit */
    *dummy_read = TZC_RGN0_ATTR_SRAM_A2;
    TZC_RGN0_ID_ACC_SRAM_A2 = 0x000F000Fu;  /* NSAIDW/R<x>=0 to 3, read/write enable */
    *dummy_read = TZC_RGN0_ID_ACC_SRAM_A2;

    TZC_RGN1_ATTR_SRAM_A2   = 0x0000000Fu;  /* Secure read/write not permit. Enable all filter unit */
    *dummy_read = TZC_RGN1_ATTR_SRAM_A2;
    TZC_RGN1_ID_ACC_SRAM_A2 = 0x000F000Fu;  /* NSAIDW/R<x>=0 to 3, read/write enable */
    *dummy_read = TZC_RGN1_ID_ACC_SRAM_A2;

    TZC_RGN1_BASE_SRAM_A2  = 0x00121000u;  /* Region1 Base address Low */
    *dummy_read = TZC_RGN1_BASE_SRAM_A2;

    TZC_RGN1_TOP_SRAM_A2   = 0x00121000u;  /* Region1 Top address Low */
    *dummy_read = TZC_RGN1_TOP_SRAM_A2;

    TZC_GATE_KEEP_SRAM_A2  = 0x0000000Fu;  /* Open filter from #0 to #3 */
    *dummy_read = TZC_GATE_KEEP_SRAM_A2;
    TZC_SPECUL_CTL_SRAM_A2 = 0x00000003u;  /* Disable read/write access speculation */
    *dummy_read = TZC_SPECUL_CTL_SRAM_A2;

    memcpy(CA55_BUCKUP_ADDR,CM33_BACKUP_ADDR,CA55_BACKUP_SIZE);
}

static void negate_ca55_reset(uint32_t sys_ca55_cfg_rval_back, uint32_t sys_ca55_cfg_rvah_back)
{
    VBATT_BKR0 = 0xF0;
    /*
     ---- ---- --1- ---- ---- ---- --1- ---- : CLK5_ON CA55_TSCLK
     ---- ---- ---1 ---- ---- ---- ---1 ---- : CLK4_ON CA55_AC
     ---- ---- ---- 1--- ---- ---- ---- 1--- : CLK3_ON CA55_GICCLK
     ---- ---- ---- -1-- ---- ---- ---- -1-- : CLK2_ON CA55_ATCLK
     ---- ---- ---- --1- ---- ---- ---- --1- : CLK1_ON CA55_PCLK
     ---- ---- ---- ---1 ---- ---- ---- ---1 : CLK0_ON CA55_SCLK/PERICLK  */
    R_CPG->CPG_CLKON_CA55 = 0x003F003F;

    /*
     ---- ---- ---- ---- ---- ---- --1- ---- : CLK5_ON CA55_TSCLK
     ---- ---- ---- ---- ---- ---- ---1 ---- : CLK4_ON CA55_AC
     ---- ---- ---- ---- ---- ---- ---- 1--- : CLK3_ON CA55_GICCLK
     ---- ---- ---- ---- ---- ---- ---- -1-- : CLK2_ON CA55_ATCLK
     ---- ---- ---- ---- ---- ---- ---- --1- : CLK1_ON CA55_PCLK
     ---- ---- ---- ---- ---- ---- ---- ---1 : CLK0_ON CA55_SCLK/PERICLK  */
    while(0x0000003F != R_CPG->CPG_CLKMON_CA55)
    {
        /* Do nothing */
        break;
        ;
    }

    /* Set CA55 resetVector Address */
    R_SYSC->SYS_CA55_CFG_RVAL0 = sys_ca55_cfg_rval_back;
    R_SYSC->SYS_CA55_CFG_RVAH0 = sys_ca55_cfg_rvah_back;


    /*
     ---1 ---- ---- ---- ---1 ---- ---- ---- : UNIT12_RSTB CA55_RST12
     ---- 1--- ---- ---- ---- 1--- ---- ---- : UNIT11_RSTB CA55_RST11
     ---- -1-- ---- ---- ---- -1-- ---- ---- : UNIT10_RSTB CA55_RST10
     ---- --0- ---- ---- ---- --0- ---- ---- : UNIT9_RSTB CA55_RST9
     ---- ---0 ---- ---- ---- ---0 ---- ---- : UNIT8_RSTB CA55_RST8
     ---- ---- 0--- ---- ---- ---- 0--- ---- : UNIT7_RSTB CA55_RST7
     ---- ---- -0-- ---- ---- ---- -0-- ---- : UNIT6_RSTB CA55_RST6
     ---- ---- --0- ---- ---- ---- --0- ---- : UNIT5_RSTB CA55_RST5
     ---- ---- ---0 ---- ---- ---- ---0 ---- : UNIT4_RSTB CA55_RST4
     ---- ---- ---- -0-- ---- ---- ---- -0-- : UNIT2_RSTB CA55_RST3_0
     ---- ---- ---- ---0 ---- ---- ---- ---0 : UNIT0_RSTB CA55_RST1_0  */
    R_CPG->CPG_RST_CA55 = 0x1C001C00;
    while(0x000003FF != R_CPG->CPG_RSTMON_CA55)
    {
        /* Do nothing */
        break;
        ;
    }

    /*
     ---1 ---- ---- ---- ---1 ---- ---- ---- : UNIT12_RSTB CA55_RST12
     ---- 1--- ---- ---- ---- 1--- ---- ---- : UNIT11_RSTB CA55_RST11
     ---- -1-- ---- ---- ---- -1-- ---- ---- : UNIT10_RSTB CA55_RST10
     ---- --1- ---- ---- ---- --1- ---- ---- : UNIT9_RSTB CA55_RST9
     ---- ---1 ---- ---- ---- ---1 ---- ---- : UNIT8_RSTB CA55_RST8
     ---- ---- 1--- ---- ---- ---- 1--- ---- : UNIT7_RSTB CA55_RST7
     ---- ---- -1-- ---- ---- ---- -1-- ---- : UNIT6_RSTB CA55_RST6
     ---- ---- --1- ---- ---- ---- --1- ---- : UNIT5_RSTB CA55_RST5
     ---- ---- ---1 ---- ---- ---- ---1 ---- : UNIT4_RSTB CA55_RST4
     ---- ---- ---- -1-- ---- ---- ---- -1-- : UNIT2_RSTB CA55_RST3_0
     ---- ---- ---- ---1 ---- ---- ---- ---1 : UNIT0_RSTB CA55_RST1_0  */
    R_CPG->CPG_RST_CA55 = 0x1FF51FF5;
    while((0x00000000 != R_CPG->CPG_RSTMON_CA55) && (0x0000000A != R_CPG->CPG_RSTMON_CA55))
    {
        /* Do nothing */
        break;
        ;
    }
    SYS_LP_CA55CK_CTL2 = 0x00000F06;
    while(0x00000F06 != SYS_LP_CA55CK_CTL3)
    {
        /* Do nothing */
        break;
        ;
    }

    /*
     ---- ---- -100 1000 ---- ---- ---- ---- : PSTATE0_SET
     ---- ---- ---- ---- ---- ---- ---- ---1 : PREQ_SET  */
    R_CPG->CPG_CLUSTER_PCHCTL = 0x00480001;
    while ((0x00000001 != R_CPG->CPG_CLUSTER_PCHMON) && (0x00000002 != R_CPG->CPG_CLUSTER_PCHMON))
    {
        /* Do nothing */
        break;
        ;
    }

    R_CPG->CPG_CLUSTER_PCHCTL = R_CPG->CPG_CLUSTER_PCHCTL & 0xFFFFFFFE;

    /*
     ---- ---- --00 1000 ---- ---- ---- ---- : PSTATE0_SET
     ---- ---- ---- ---- ---- ---- ---- ---1 : PREQ0_SET  */

    R_CPG->CPG_CORE0_PCHCTL = 0x00080001;

    while ((0x00000001 != R_CPG->CPG_CORE0_PCHMON) && (0x00000002 != R_CPG->CPG_CORE0_PCHMON))
    {
        /* Do nothing */
        break;
        ;
    }
    /* Enable CA55 Interrupt */

    R_CPG->CPG_CLKON_GIC600 = 0x00010001;
    R_CPG->CPG_RST_GIC600 = 0x00030003;

    SYS_LP_CTL6 = SYS_LP_CTL6 & 0xFFFFFEFF;

    R_CPG->CPG_CORE0_PCHCTL = R_CPG->CPG_CORE0_PCHCTL & 0xFFFFFFFE;
    SYS_LP_CTL0 = SYS_LP_CTL0 & 0xFFFFFFFE;
    R_CPG->CPG_CLKON_SCIF = 0x003F0009;
}

/* AWO Task entry function */
/* pvParameters contains TaskHandle_t */
void awo_task_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    /* If this board has no LEDs then trap here */
    if (0 == leds.led_count)
    {
        while (1)
        {
            vTaskSuspend(NULL);     // There are no LEDs on this board
        }
    }

    uint32_t sys_ca55_cfg_rval_back;
    uint32_t sys_ca55_cfg_rvah_back;
    uint32_t counter = 0;

    g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);
    g_timer0.p_api->callbackSet(g_timer0.p_ctrl, timer_isr, NULL, NULL);
    g_timer0.p_api->start(g_timer0.p_ctrl);

    R_GPIO->PMC_35 &= 0xF7;
    R_GPIO->PM_35  = (uint16_t)((R_GPIO->PM_35 & 0xFF3F) | 0x0080);
    R_GPIO->P_35   &= 0x00;

    while(1)
    {
        volatile uint32_t dummy_read;       /* Add volatile for optimization */
        R_CPG->CPG_CLKON_VBAT = 0x00010001;
        R_CPG->CPG_RST_VBAT = 0x00010001;
        R_CPG->CPG_BUS_MCPU3_MSTOP = 0x01000000;
        counter++;

        while( VBATT_AWO_VALUE != VBATT_BKR0)
        {
            vTaskDelay(1);
            if(R_CPG->CPG_CLKON_VBAT==0)
            {
                R_CPG->CPG_CLKON_VBAT = 0x00010001;
                R_CPG->CPG_RST_VBAT = 0x00010001;
                R_CPG->CPG_BUS_MCPU3_MSTOP = 0x01000000;
            }

        }

        /* ALLON to AWO/VBATT */

        /****************************
         * Store SRAM
         ****************************/
        backup_sram(&sys_ca55_cfg_rval_back, &sys_ca55_cfg_rvah_back);

        /****************************
         * Set PMIC
         ****************************/
        set_pmic();

        /****************************
         * Wait CA55 Sleep
         ****************************/
        wait_ca55_sleep();

        /****************************
         * Q-Channel Control
         ****************************/
        q_channel_stop();

        /****************************
         * Stop clock
         ****************************/
        stop_ca55_clock();

        /****************************
         * Assert reset
         ****************************/
        assert_ca55_reset();

        /****************************
         * Stop Module
         ****************************/
        stop_module();


        /****************************
         * Clock and Reset Control
         ****************************/
        stop_clock_assert_reset();

        /* SYS_PD_ISO_CTRL
         ---- ---- ---- ---- ---- ---- ---- ---1 : PD_ISOVCC_ISOEN */
        R_SYSC->SYS_PD_ISO_CTRL = 0x00000001;
        dummy_read = R_SYSC->SYS_PD_ISO_CTRL;

        sleep_enable_on_greenpack();

        /* Wait till PD ISOVCC */
        vTaskDelay(1);
        printf("CM33:AWO Mode\r\n");


        /*******************************************************
         *
         * AWO/VBATT to ALLON
         *
         *******************************************************/
        /****************************
         * Wait PD_ISOVCC stable
         ****************************/
#if (LOW_POWER_MODE == 2)
        printf("Hit any key to go to ALLON mode.\r\n");
        stdio_read(string_buf,sizeof(string_buf));
        printf("\r\n");
        {
            printf("CM33:Set GreenPAK to ALLON\r\n");
            stdio_close();

            sleep_enable_off_greenpack();
        }
#endif
        /****************************
         * Wait PD_ISOVCC stable
         ****************************/
        wait_pd_isovcc_stable();

        /****************************
         * PD_ISOVCC Power supply
         ****************************/
        pd_isovcc_power_supply();

        /****************************
         * Restore SRAM repair information
         ****************************/
        restore_sram_repair_information();

        /****************************
         * Reset off of System Bus in PD_ISOVCC
         ****************************/
        negate_ip_power_down();

        /****************************
         * Clock on of System Bus in PD_ISOVCC
         ****************************/
        start_system_bus_clock();
        negate_system_bus_reset();

        /****************************
         * Deassert MSTOP
         ****************************/
        start_module();

        /****************************
         * Resotre SRAM
         ****************************/
        restore_sram(&dummy_read);

        /****************************
         * Reset off of CA55
         ****************************/
        negate_ca55_reset(sys_ca55_cfg_rval_back, sys_ca55_cfg_rvah_back);

        (void)dummy_read;

        vTaskDelay(1);
    }

    while (1)
    {
        stdio_close();


        printf("a\r\n");

        vTaskDelay(1);
    }
}
