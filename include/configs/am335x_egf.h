/*
 * am335x_evm.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CONFIG_AM335X_EGF_H
#define __CONFIG_AM335X_EGF_H

#include <configs/ti_am335x_common.h>

#undef CONFIG_SPL_OS_BOOT

#define CONFIG_MACH_TYPE		MACH_TYPE_AM335X_EGF

/* Clock Defines */
#define V_OSCK				24000000  /* Clock output from T2 */
#define V_SCLK				(V_OSCK)

/* Custom script for NOR */
#define CONFIG_SYS_LDSCRIPT		"board/ti/am335x_egf/u-boot.lds"

#define CONFIG_ENV_IS_NOWHERE

/* Always 128 KiB env size */
#define CONFIG_ENV_SIZE			(128 << 10)

#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG

#ifndef CONFIG_SPL_BUILD

/*	"usbroot=/dev/sda2 rw\0" \
	"usbrootfstype=ext4 rootwait\0" \

	"root=${usbroot} " \
	"rootfstype=${usbrootfstype}\0" \

 */

#define CONFIG_EXTRA_ENV_SETTINGS \
	"loadaddr=0x80200000\0" \
	"fdtaddr=0x81100000\0" \
	"boot_fdt=try\0" \
	"bootpart=0:2\0" \
	"bootfile=zImage\0" \
	"fdtfile=undefined\0" \
	"console=ttyO0,115200n8\0" \
	"optargs=\0" \
	"mmcdev=0\0" \
	"mmcroot=/dev/mmcblk0p2 ro\0" \
	"mmcrootfstype=ext4 rootwait fixrtc \0" \
	"rootpath=/export/rootfs\0" \
	"mmcargs=setenv bootargs console=${console} " \
		"${optargs} " \
		"root=${mmcroot} " \
		"rootfstype=${mmcrootfstype}\0" \
	"usbargs=setenv bootargs console=${console} " \
		"${optargs} \0" \
	"findfdt=setenv fdtfile am335x-egf.dtb; \0" \
	BOOTCMD_COMMON \
	BOOTCMD_MMC \
	BOOTCMD_NAND \
	BOOTCMD_USB
#endif

/* NS16550 Configuration */
#define CONFIG_SYS_NS16550_COM1		0x44e09000	/* Base EVM has UART0 */
#define CONFIG_SYS_NS16550_COM2		0x48022000	/* UART1 */
#define CONFIG_SYS_NS16550_COM3		0x48024000	/* UART2 */
#define CONFIG_SYS_NS16550_COM4		0x481a6000	/* UART3 */
#define CONFIG_SYS_NS16550_COM5		0x481a8000	/* UART4 */
#define CONFIG_SYS_NS16550_COM6		0x481aa000	/* UART5 */
#define CONFIG_BAUDRATE			115200

/* I2C Configuration */
#define CONFIG_CMD_EEPROM
#define CONFIG_ENV_EEPROM_IS_ON_I2C
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50	/* Main EEPROM */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	2
#define CONFIG_SYS_I2C_MULTI_EEPROMS

/* PMIC support */
#define CONFIG_POWER_TPS65217
#define CONFIG_POWER_TPS65910

#define CONFIG_SPL_FAT_SUPPORT

/* SPL */
#ifndef CONFIG_NOR_BOOT
#define CONFIG_SPL_POWER_SUPPORT
/*
#define CONFIG_SPL_YMODEM_SUPPORT
*/
/* CPSW support */
/* #define CONFIG_SPL_ETH_SUPPORT */

/* USB gadget RNDIS */
/* #define CONFIG_SPL_MUSB_NEW_SUPPORT */

/* General network SPL, both CPSW and USB gadget RNDIS */
/* #define CONFIG_SPL_NET_SUPPORT */
#define CONFIG_SPL_ENV_SUPPORT
#define CONFIG_SPL_NET_VCI_STRING	"AM335x U-Boot SPL"

/* SPI flash. */
#define CONFIG_SPL_SPI_SUPPORT
#define CONFIG_SPL_SPI_FLASH_SUPPORT
#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SPL_SPI_BUS		0
#define CONFIG_SPL_SPI_CS		0
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x20000
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPL_LDSCRIPT		"$(CPUDIR)/am33xx/u-boot-spl.lds"
#endif

/* Enhance our eMMC support / experience. */
#define CONFIG_CMD_GPT
#define CONFIG_EFI_PARTITION
#define CONFIG_PARTITION_UUIDS
#define CONFIG_CMD_PART

/* Command Reset EEPROM with WID */
#define CONFIG_CMD_INIT_EEPROM

/* NAND support */
#ifdef CONFIG_NAND
#define MTDIDS_DEFAULT		      "nand0=nand.0"
#define MTDPARTS_DEFAULT		   "mtdparts=nand.0:" \
					  "128k(NAND.SPL)," \
					  "128k(NAND.SPL.backup1)," \
					  "128k(NAND.SPL.backup2)," \
					  "128k(NAND.SPL.backup3)," \
					  "256k(NAND.u-boot-spl-os)," \
					  "1m(NAND.u-boot)," \
					  "128k(NAND.u-boot-env)," \
					  "128k(NAND.u-boot-env.backup1)," \
					  "8m(NAND.kernel)," \
					  "110m(NAND.file-system)," \
					  "90m(NAND.home)," \
					  "30m(NAND.app)," \
					  "5m(NAND.app_conf)," \
					  "-(NAND.app_conf_factory)" \


/* NAND: device related configs */

#define CONFIG_NAND_UBI_ROOTFS_VOLNAME 		"0438-evb-rootfs"
#define CONFIG_SYS_NAND_PAGE_SIZE		2048
#define CONFIG_SYS_NAND_SUBPAGE_SIZE	512
#define CONFIG_SYS_NAND_OOBSIZE			64
#define CONFIG_SYS_NAND_BLOCK_SIZE		(64*2048)
#define CONFIG_SPL_NAND_DEVICE_WIDTH		8
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_PAGE_COUNT		(CONFIG_SYS_NAND_BLOCK_SIZE / \
						 CONFIG_SYS_NAND_PAGE_SIZE)
/* NAND: driver related configs */
#define CONFIG_NAND_OMAP_GPMC
#define CONFIG_NAND_OMAP_ELM
#define CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_BASE			0x8000000
#define CONFIG_SYS_MAX_NAND_DEVICE		1
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_NAND_BAD_BLOCK_POS		NAND_LARGE_BADBLOCK_POS
#define CONFIG_SYS_NAND_ECCPOS		      { 2, 3, 4, 5, 6, 7, 8, 9, \
					       10, 11, 12, 13, 14, 15, 16, 17, \
					       18, 19, 20, 21, 22, 23, 24, 25, \
					       26, 27, 28, 29, 30, 31, 32, 33, \
					       34, 35, 36, 37, 38, 39, 40, 41, \
					       42, 43, 44, 45, 46, 47, 48, 49, \
					       50, 51, 52, 53, 54, 55, 56, 57, }
#define CONFIG_SYS_NAND_ECCSIZE			512
#define CONFIG_SYS_NAND_ECCBYTES		14
#define CONFIG_NAND_OMAP_ECCSCHEME		OMAP_ECC_BCH8_CODE_HW
#define CONFIG_SPL_NAND_AM33XX_BCH
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_BASE
#define CONFIG_SPL_NAND_DRIVERS
#define CONFIG_SPL_NAND_ECC
#define CONFIG_SYS_NAND_U_BOOT_START		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_NAND_U_BOOT_OFFS		0x000C0000
#else
#define NANDARGS ""
#endif /* !CONFIG_NAND */


/*
 * USB configuration.  We enable MUSB support, both for host and for
 * gadget.  We set USB0 as peripheral and USB1 as host, based on the
 * board schematic and physical port wired to each.  Then for host we
 * add mass storage support and for gadget we add both RNDIS ethernet
 * and DFU.
 */
#define CONFIG_USB_MUSB_DSPS
#define CONFIG_ARCH_MISC_INIT
#define CONFIG_MUSB_GADGET
#define CONFIG_MUSB_PIO_ONLY
#define CONFIG_MUSB_DISABLE_BULK_COMBINE_SPLIT
#define CONFIG_USB_GADGET
#define CONFIG_USBDOWNLOAD_GADGET
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_GADGET_VBUS_DRAW	2
#define CONFIG_MUSB_HOST
#define CONFIG_AM335X_USB0
#define CONFIG_AM335X_USB0_MODE		MUSB_HOST
#define CONFIG_AM335X_USB1
#define CONFIG_AM335X_USB1_MODE MUSB_PERIPHERAL

#ifdef CONFIG_MUSB_HOST
#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#endif

#ifdef CONFIG_MUSB_GADGET
#define CONFIG_USB_ETHER
#define CONFIG_USB_ETH_RNDIS
#define CONFIG_USBNET_HOST_ADDR	"de:ad:be:af:00:00"

/* USB TI's IDs */
#define CONFIG_G_DNL_VENDOR_NUM 0x0403
#define CONFIG_G_DNL_PRODUCT_NUM 0xBD00
#define CONFIG_G_DNL_MANUFACTURER "Texas Instruments"
#endif /* CONFIG_MUSB_GADGET */



/* SPI flash. */
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SF_DEFAULT_SPEED		24000000

/* Network. */
#define CONFIG_PHY_GIGE
#define CONFIG_PHYLIB
#define CONFIG_PHY_SMSC


#ifdef CONFIG_MMC
#define BOOT_TARGETS_MMC "mmc0"
#else
#define BOOT_TARGETS_MMC ""
#endif

#ifdef CONFIG_MUSB_HOST
#define BOOT_TARGETS_USB "usb"
#else
#define BOOT_TARGETS_USB ""
#endif

#ifdef CONFIG_NAND
#define BOOT_TARGETS_NAND "nand"
#else
#define BOOT_TARGETS_NAND ""
#endif

#endif	/* ! __CONFIG_AM335X_EGF_H */
