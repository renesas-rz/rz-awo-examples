# RZ/G3S Cortex-M33 AWO Example

This is the Cortex-M33 AWO - (Always in use (ON) power) Example designed to work with the RZ/G3S Multi-OS environment. This example demonstrates how the Cortex®-M33 core controls and manages system suspend (AWO mode) and resume (ALLON mode) operations in coordination with the Cortex®-A55 core running Linux. It utilizes the RZ/G Flexible Software Package (FSP) for FreeRTOS and integrates with the Linux BSP Plus support to provide a complete low-power suspend-to-RAM operation example.

## Overview of sample program behavior
- CM33 waits for CA55 Linux to enter Suspend-to-RAM.
- Upon Linux suspension, CM33 transitions PMIC and system to AWO mode.
- In AWO mode, CA55 is fully powered down, only CM33 remains active.
- On user input, CM33 restores PMIC and system to ALLON mode, and CA55 resumes Linux operation.
- The sequence demonstrates complete suspend/resume coordination in a Multi-OS power-management environment.

## Table of Contents
- [RZ/G3S Cortex-M33 AWO Example](#rzg3s-cortex-m33-awo-example)
    - [Overview of Sample Program Behavior](#overview-of-sample-program-behavior)
    - [Table of Contents](#table-of-contents)
    - [Getting Started](#getting-started)
        - [Sample Projects](#sample-projects)
        - [Development Environment](#development-environment)
    - [Demo freertos_w_awo_rzg3s_evk_example](#demo-freertos_w_awo_rzg3s_evk_example)
        - [Hardware Setup](#hardware-setup)
        - [CM33 Sample Project Setup](#cm33-sample-project-setup)
        - [CM33 Sample Program Invocation](#cm33-sample-program-invocation)

## Getting Started
### Sample Projects
|No.|Program File|Descriptions|
|----|----|----|
|1|freertos_w_awo_rzg3s_evk_example| CM33 FreeRTOS project demonstrating suspend (AWO) and resume (ALLON) flow with Linux on CA55


### Development Environment
|Hardware|Version|
|----|----|
|[RZ/G3S-EVKIT](https://www.renesas.com/ja/design-resources/boards-kits/rz-g3s-evkit )|
|[Segger J-link](https://www.segger.com/products/debug-probes/j-link/)| J-Link BASE v11.0 (J-Link DLL v7.96j) |


|Software|Version|
|----|----|
|e<sup>2</sup> studio|2025-07 (25.7.0)|
|GNU ARM Embedded 13.3-Rel1|13.3.1.arm-13-24|
|RZ/G FSP|3.1.0|
|Tera Term|-|


For more information to set up your development environment, refer to the following document.
- [Getting Started with Flexible Software Package](https://www.renesas.com/en/document/apn/rzg2l-getting-started-with-flexible-software-package)


## Demo freertos_w_awo_rzg3s_evk_example

The **freertos_w_awo_rzg3s_evk_example** outputs the message tranfer progress between CPU cores to the console.

### Hardware setup
1. Connect USB Type Micro-AB cable to SER3_UART Connector.
2. Insert micro SD card containing kernel image, device tree, rootfs support for multi-os communication into SDIO connector on the board.
3. Connect USB-PD Power Charger to USB_C_PWR_IN.<br>
<img src=./git_images/connection-board.png width="720">

### CM33 Sample Project Setup
1. Launch **e² studio 2025-07**.  
2. Import the project:  
   **File → Import → Existing Projects into Workspace** and select `freertos_w_awo_rzg3s_evk_example`.  
3. Build the project (**Project → Build All**).  
   Output artifacts are generated in `Debug` or `Release` directories:
   - `freertos_w_awo_rzg3s_evk_example.elf`  
   - `freertos_w_awo_rzg3s_evk_example.srec`
4. Deploy the generated `.srec` file using Flash Writer via QSPI or eMMC as follow:
- For QSPI Boot (1.8 V)
Program the freertos_w_awo_rzg3s_evk_example.srec file as shown:
```
> xls2
===== Qspi writing of RZ/G2 Board Command =============
Load Program to Spiflash
===== Please Input Program Top Address ============
  Please Input : H'80200000
===== Please Input Qspi Save Address ===
  Please Input : H'200000
please send ! ('.' & CR stop load)
```

After the transfer finishes, you should see messages confirming SPI erase and write completion.

Return the board to normal boot mode and continue setup per the Linux Start-up Guide.

- For eMMC Boot (1.8 V)
Program the freertos_w_awo_rzg3s_evk_example.srec file as shown:
```
> EM_W
EM_W Start --------------
Please select, eMMC Partition Area.
0:User Partition Area
1:Boot Partition 1
2:Boot Partition 2
Select area(0-2)>1
-- Boot Partition 1 Program ----------------------------
Please Input Start Address in sector :1000
Please Input Program Start Address :80200000
please send ! ('.' & CR stop load)
```

After the write completes, continue setup per the eMMC boot instructions in the Linux Start-up Guide.

### CM33 sample program invocation  
1. Boot Linux on CA55.

2. Log in as root.
```
smarc-rzg3s login: root
```

3. Invoke the commands below on Linux console to move Linux to Suspend to RAM (S2R):
```
root@smarc-rzg3s:~# echo deep > /sys/power/mem_sleep
root@smarc-rzg3s:~# echo mem > /sys/power/state
```

4. When Linux successfully moves to S2R, you should see the following display on Linux console:
```
[1082.105386] PM: suspend entry (deep)
[1082.109183] Filesystems sync: 0.000 seconds
[1082.122622] Freezing user space processes ... (elapsed 0.001 seconds) done.
[1082.131266] OOM killer disabled.
[1082.134496] Freezing remaining freezable tasks ... (elapsed 0.001 seconds) done.
[1082.143134] printk: Suspending console(s) (use no_console_suspendto debug)
CM33:Init PMIC for AWO mode
CM33:AWO Mode
Hit any key to go to ALLON mode.
```
Note: The board is now in AWO mode, with CM33 running while CA55 is suspended.

5. When typing any key on Linux Console, RZ/G3S starts to move to ALLON, and Linux should be resumed as shown below:
```
Hit any key to go to ALLON mode.
a
CM33:Set GreenPAKto ALLON
NOTICE: BL2: v2.7(release):2.7.0/g3s_1.0.0_rc4
NOTICE: BL2: Built : 11:52:53, Feb 292024
NOTICE: BL2: Booting BL31
[60.710450] ehci-platform 11e30100.usb: port 1 resume error -110
[60.836140] usbusb2-port1: device 2-1 not suspended yet
[60.880506] Disabling non-boot CPUs ...
[60.899495] Microchip KSZ9131 Gigabit PHY 11c30000.ethernet-ffffffff:07: attached PHY driver 
[Microchip KSZ9131 Gigabit PHY] (mii_bus:phy_addr=11c30000.ethernet-ffffffff:07, irq=137)
[60.920867] Microchip KSZ9131 Gigabit PHY 11c40000.ethernet-ffffffff:07: attached PHY driver 
[Microchip KSZ9131 Gigabit PHY] (mii_bus:phy_addr=11c40000.ethernet-ffffffff:07, irq=138)
[61.017924] usbusb3: root hub lost power or was reset
[61.018018] usbusb1: root hub lost power or was reset
[61.105918] usbusb4: root hub lost power or was reset
[61.106020] usbusb2: root hub lost power or was reset
[61.309991] OOM killer enabled.
[61.313131] Restarting tasks ...
[61.313647] usb2-1: USB disconnect, device number 2
[61.334271] done.
[61.352163] PM: suspend exit
[61.619922] usb2-1: new high-speed USB device number 3 using ehci-platform
[61.788714] hub 2-1:1.0: USB hub found
[61.798400] hub 2-1:1.0: 4 ports detected
root@smarc-rzg3s:~#
```
