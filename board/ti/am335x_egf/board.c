/*
 * board.c
 *
 * Board functions for elettronica GF AM335X based SOM module
 *
 * Copyright (C) 2013, Elettronica GF s.r.l - http://www.elettronicagf.it
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <spl.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/omap.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mem.h>
#include <asm/io.h>
#include <asm/emif.h>
#include <asm/gpio.h>
#include <i2c.h>
#include <miiphy.h>
#include <cpsw.h>
#include <power/tps65217.h>
#include <power/tps65910.h>
#include "board.h"
#include "gf_eeprom.h" 

DECLARE_GLOBAL_DATA_PTR;

/* GPIO that controls power to DDR on EVM-SK */
#define GPIO_DDR_VTT_EN		7

/* Supported DDR3 RAM Models */
#define MICRON_MT41K128M16JT	1	// 256MB
#define MICRON_MT41K256M16HA	2	// 512MB

/* SW REVISIONS*/
#define REV_WID0400_AA0101 "WID0400_AA01.01"
#define REV_WID0400_AB0101 "WID0400_AB01.01"
#define REV_WID0400_AC0101 "WID0400_AC01.01"
#define REV_WID0400_AD0101 "WID0400_AD01.01"

struct egf_som {
	int ram_model;
};

static struct egf_som __attribute__((section (".data"))) the_som;
static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

int gf_strcmp(const char * cs, const char * ct) {
	register signed char __res;

	while (1) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}

	return __res;
}

int load_revision(void)
{
	char * egf_sw_id_code;
	int ret;
	
	ret = gf_load_som_revision(&egf_sw_id_code,0);
	if (ret)
	{
		printf("System Hang.\n");
		while(1);
 	}

	if(!gf_strcmp(egf_sw_id_code,REV_WID0400_AA0101))
	{
		/* SW Revision is WID0400_AA01.00 */
		printf("GF Software ID Code: WID0400_AA01.01\n");
		the_som.ram_model = MICRON_MT41K128M16JT;
	}
	else if(!gf_strcmp(egf_sw_id_code,REV_WID0400_AB0101))
	{
		/* SW Revision is WID0400_AB01.00 */
		printf("GF Software ID Code: WID0400_AB01.01\n");
		the_som.ram_model = MICRON_MT41K256M16HA;
	}
	else if(!gf_strcmp(egf_sw_id_code,REV_WID0400_AC0101))
	{
		/* SW Revision is WID0400_AC01.00 */
		printf("GF Software ID Code: WID0400_AC01.01\n");
		the_som.ram_model = MICRON_MT41K128M16JT;
	}
	else if(!gf_strcmp(egf_sw_id_code,REV_WID0400_AD0101))
	{
		/* SW Revision is WID0400_AD01.00 */
		printf("GF Software ID Code: WID0400_AD01.01\n");
		the_som.ram_model = MICRON_MT41K128M16JT;
	}
	else {
		printf("Unrecognized EGF SW ID Code: %s\n",egf_sw_id_code);
		printf("System Hang.\n");
		while(1);
	}
	return 0;
}

#if defined(CONFIG_SPL_BUILD) || defined(CONFIG_NOR_BOOT)

static const struct ddr_data ddr3_mt41k128m16jt_data = {
	.datardsratio0 = MT41K128M16JT125E_RD_DQS,
	.datawdsratio0 = MT41K128M16JT125E_WR_DQS,
	.datafwsratio0 = MT41K128M16JT125E_PHY_FIFO_WE,
	.datawrsratio0 = MT41K128M16JT125E_PHY_WR_DATA,
};

static const struct ddr_data ddr3_mt41k256m16ha_data = {
	.datardsratio0 = MT41K256M16HA125E_RD_DQS,
	.datawdsratio0 = MT41K256M16HA125E_WR_DQS,
	.datafwsratio0 = MT41K256M16HA125E_PHY_FIFO_WE,
	.datawrsratio0 = MT41K256M16HA125E_PHY_WR_DATA,
};

static const struct cmd_control ddr3_mt41k128m16jt_cmd_ctrl_data = {
	.cmd0csratio = MT41K128M16JT125E_RATIO,
	.cmd0iclkout = MT41K128M16JT125E_INVERT_CLKOUT,

	.cmd1csratio = MT41K128M16JT125E_RATIO,
	.cmd1iclkout = MT41K128M16JT125E_INVERT_CLKOUT,

	.cmd2csratio = MT41K128M16JT125E_RATIO,
	.cmd2iclkout = MT41K128M16JT125E_INVERT_CLKOUT,
};

static const struct cmd_control ddr3_mt41k256m16ha_cmd_ctrl_data = {
	.cmd0csratio = MT41K256M16HA125E_RATIO,
	.cmd0iclkout = MT41K256M16HA125E_INVERT_CLKOUT,

	.cmd1csratio = MT41K256M16HA125E_RATIO,
	.cmd1iclkout = MT41K256M16HA125E_INVERT_CLKOUT,

	.cmd2csratio = MT41K256M16HA125E_RATIO,
	.cmd2iclkout = MT41K256M16HA125E_INVERT_CLKOUT,
};

static struct emif_regs ddr3_mt41k128m16jt_emif_reg_data = {
	.sdram_config = MT41K128M16JT125E_EMIF_SDCFG,
	.ref_ctrl = MT41K128M16JT125E_EMIF_SDREF,
	.sdram_tim1 = MT41K128M16JT125E_EMIF_TIM1,
	.sdram_tim2 = MT41K128M16JT125E_EMIF_TIM2,
	.sdram_tim3 = MT41K128M16JT125E_EMIF_TIM3,
	.zq_config = MT41K128M16JT125E_ZQ_CFG,
	.emif_ddr_phy_ctlr_1 = MT41K128M16JT125E_EMIF_READ_LATENCY |
				PHY_EN_DYN_PWRDN,
};

static struct emif_regs ddr3_mt41k256m16ha_emif_reg_data = {
	.sdram_config = MT41K256M16HA125E_EMIF_SDCFG,
	.ref_ctrl = MT41K256M16HA125E_EMIF_SDREF,
	.sdram_tim1 = MT41K256M16HA125E_EMIF_TIM1,
	.sdram_tim2 = MT41K256M16HA125E_EMIF_TIM2,
	.sdram_tim3 = MT41K256M16HA125E_EMIF_TIM3,
	.zq_config = MT41K256M16HA125E_ZQ_CFG,
	.emif_ddr_phy_ctlr_1 = MT41K256M16HA125E_EMIF_READ_LATENCY,
};

#ifdef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	/* break into full u-boot on 'c' */
	return (serial_tstc() && serial_getc() == 'c');
}
#endif

#define OSC	(V_OSCK/1000000)
const struct dpll_params dpll_ddr = {
		266, OSC-1, 1, -1, -1, -1, -1};
const struct dpll_params dpll_ddr_303 = {
		303, OSC-1, 1, -1, -1, -1, -1};
const struct dpll_params dpll_ddr_400 = {
		400, OSC-1, 1, -1, -1, -1, -1};


void am33xx_spl_board_init(void)
{
	struct am335x_baseboard_id header;
	int mpu_vdd;

	/* Get the frequency */
	dpll_mpu_opp100.m = am335x_get_efuse_mpu_max_freq(cdev);

	int sil_rev;

	/*
	 * The GP EVM, IDK and EVM SK use a TPS65910 PMIC.  For all
	 * MPU frequencies we support we use a CORE voltage of
	 * 1.1375V.  For MPU voltage we need to switch based on
	 * the frequency we are running at.
	 */
	if (i2c_probe(TPS65910_CTRL_I2C_ADDR))
			return;

	/*
	 * Depending on MPU clock and PG we will need a different
	 * VDD to drive at that speed.
	 */
	sil_rev = readl(&cdev->deviceid) >> 28;
	mpu_vdd = am335x_get_tps65910_mpu_vdd(sil_rev, dpll_mpu_opp100.m);

	/* Tell the TPS65910 to use i2c */
	tps65910_set_i2c_control();

	/* First update MPU voltage. */
	if (tps65910_voltage_update(MPU, mpu_vdd))
		return;

	/* Second, update the CORE voltage. */
	if (tps65910_voltage_update(CORE, TPS65910_OP_REG_SEL_1_1_3))
		return;

	/* Set CORE Frequencies to OPP100 */
	do_setup_dpll(&dpll_core_regs, &dpll_core_opp100);
	

	/* Set MPU Frequency to what we detected now that voltages are set */
	do_setup_dpll(&dpll_mpu_regs, &dpll_mpu_opp100);




}

const struct dpll_params *get_dpll_ddr_params(void)
{
	struct am335x_baseboard_id header;
	printf("dpll get par\n");
	enable_i2c0_pin_mux();
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	load_revision();

	return &dpll_ddr_400;

}

void set_uart_mux_conf(void)
{
#ifdef CONFIG_SERIAL1
	enable_uart0_pin_mux();
#endif /* CONFIG_SERIAL1 */
#ifdef CONFIG_SERIAL2
	enable_uart1_pin_mux();
#endif /* CONFIG_SERIAL2 */
#ifdef CONFIG_SERIAL3
	enable_uart2_pin_mux();
#endif /* CONFIG_SERIAL3 */
#ifdef CONFIG_SERIAL4
	enable_uart3_pin_mux();
#endif /* CONFIG_SERIAL4 */
#ifdef CONFIG_SERIAL5
	enable_uart4_pin_mux();
#endif /* CONFIG_SERIAL5 */
#ifdef CONFIG_SERIAL6
	enable_uart5_pin_mux();
#endif /* CONFIG_SERIAL6 */
}

void set_mux_conf_regs(void)
{
	__maybe_unused struct am335x_baseboard_id header;
	enable_board_pin_mux(&header);
}

void sdram_init(void)
{
	__maybe_unused struct am335x_baseboard_id header;



	/*
	 * EVM SK 1.2A and later use gpio0_7 to enable DDR3.
	 * This is safe enough to do on older revs.
	 */
	gpio_request(GPIO_DDR_VTT_EN, "ddr_vtt_en");
	gpio_direction_output(GPIO_DDR_VTT_EN, 1);
	
	printf("RAM Model = %d\n",the_som.ram_model);
	if (the_som.ram_model == MICRON_MT41K128M16JT) {
		printf("DDR3: 256MB - Micron MT41K128M16JT\n");
		config_ddr(400, MT41K128M16JT125E_IOCTRL_VALUE, &ddr3_mt41k128m16jt_data,
					&ddr3_mt41k128m16jt_cmd_ctrl_data, &ddr3_mt41k128m16jt_emif_reg_data, 0);
	}
	else if (the_som.ram_model == MICRON_MT41K256M16HA) {
		printf("DDR3: 512MB - Micron MT41K256M16HA\n");
		config_ddr(400, MT41K256M16HA125E_IOCTRL_VALUE,
			   &ddr3_mt41k256m16ha_data,
			   &ddr3_mt41k256m16ha_cmd_ctrl_data,
			   &ddr3_mt41k256m16ha_emif_reg_data, 0);
	}
	else {
		printf("DDR3: Unrecognized! \n");
	}

}
#endif

/*
 * Basic board specific setup.  Pinmux has been handled already.
 */
int board_init(void)
{
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
#if defined(CONFIG_NOR) || defined(CONFIG_NAND)
	gpmc_init();
#endif
	load_revision();
	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	return 0;
}
#endif

#if (defined(CONFIG_DRIVER_TI_CPSW) && !defined(CONFIG_SPL_BUILD)) || \
	(defined(CONFIG_SPL_ETH_SUPPORT) && defined(CONFIG_SPL_BUILD))
static void cpsw_control(int enabled)
{
	/* VTP can be added here */

	return;
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_addr	= 0,
	},
	{
		.slave_reg_ofs	= 0x308,
		.sliver_reg_ofs	= 0xdc0,
		.phy_addr	= 1,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 1,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};
#endif

#if defined(CONFIG_DRIVER_TI_CPSW) || \
	(defined(CONFIG_USB_ETHER) && defined(CONFIG_MUSB_GADGET))
int board_eth_init(bd_t *bis)
{
	int rv, n = 0;
	uint8_t mac_addr[6];
	uint32_t mac_hi, mac_lo;
	__maybe_unused struct am335x_baseboard_id header;

	/* try reading mac address from efuse */
	mac_lo = readl(&cdev->macid0l);
	mac_hi = readl(&cdev->macid0h);
	mac_addr[0] = mac_hi & 0xFF;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
	mac_addr[4] = mac_lo & 0xFF;
	mac_addr[5] = (mac_lo & 0xFF00) >> 8;

#if (defined(CONFIG_DRIVER_TI_CPSW) && !defined(CONFIG_SPL_BUILD)) || \
	(defined(CONFIG_SPL_ETH_SUPPORT) && defined(CONFIG_SPL_BUILD))
	if (!getenv("ethaddr")) {
		printf("<ethaddr> not set. Validating first E-fuse MAC\n");

		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("ethaddr", mac_addr);
	}

#ifdef CONFIG_DRIVER_TI_CPSW

	mac_lo = readl(&cdev->macid1l);
	mac_hi = readl(&cdev->macid1h);
	mac_addr[0] = mac_hi & 0xFF;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
	mac_addr[4] = mac_lo & 0xFF;
	mac_addr[5] = (mac_lo & 0xFF00) >> 8;

	if (!getenv("eth1addr")) {
		if (is_valid_ether_addr(mac_addr))
			eth_setenv_enetaddr("eth1addr", mac_addr);
	}
    writel(MII_MODE_ENABLE, &cdev->miisel);
	cpsw_slaves[0].phy_if = cpsw_slaves[1].phy_if =
				PHY_INTERFACE_MODE_MII;
	
	rv = cpsw_register(&cpsw_data);
	if (rv < 0)
		printf("Error %d registering CPSW switch\n", rv);
	else
		n += rv;
#endif


#endif
#if defined(CONFIG_USB_ETHER) && \
	(!defined(CONFIG_SPL_BUILD) || defined(CONFIG_SPL_USBETH_SUPPORT))
	if (is_valid_ether_addr(mac_addr))
		eth_setenv_enetaddr("usbnet_devaddr", mac_addr);

	rv = usb_eth_initialize(bis);
	if (rv < 0)
		printf("Error %d registering USB_ETHER\n", rv);
	else
		n += rv;
#endif
	return n;
}
#endif
