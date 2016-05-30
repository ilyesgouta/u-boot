/*
 * (C) Copyright 2016
 * Vikas Manocha, <vikas.manocha@st.com>
 * Ilyes Gouta, <ilyes.gouta@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/armv7m.h>
#include <asm/arch/stm32.h>
#include <asm/arch/gpio.h>
#include <dm/platdata.h>
#include <dm/platform_data/serial_stm32x7.h>
#include <asm/arch/stm32_periph.h>
#include <asm/arch/stm32_defs.h>
#include <miiphy.h>
#include <asm/nvic.h>
#include <libfdt.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

const struct stm32_gpio_ctl gpio_ctl_gpout = {
	.mode = STM32_GPIO_MODE_OUT,
	.otype = STM32_GPIO_OTYPE_PP,
	.speed = STM32_GPIO_SPEED_50M,
	.pupd = STM32_GPIO_PUPD_NO,
	.af = STM32_GPIO_AF0
};

const struct stm32_gpio_ctl gpio_ctl_usart = {
	.mode = STM32_GPIO_MODE_AF,
	.otype = STM32_GPIO_OTYPE_PP,
	.speed = STM32_GPIO_SPEED_50M,
	.pupd = STM32_GPIO_PUPD_UP,
	.af = STM32_GPIO_AF8
};

const struct stm32_gpio_ctl gpio_ctl_eth = {
	.mode = STM32_GPIO_MODE_AF,
	.otype = STM32_GPIO_OTYPE_PP,
	.speed = STM32_GPIO_SPEED_100M,
	.pupd = STM32_GPIO_PUPD_NO,
	.af = STM32_GPIO_AF11
};

const struct stm32_gpio_ctl gpio_ctl_sdram = {
	.mode = STM32_GPIO_MODE_AF,
	.otype = STM32_GPIO_OTYPE_PP,
	.speed = STM32_GPIO_SPEED_50M,
	.pupd = STM32_GPIO_PUPD_UP,
	.af = STM32_GPIO_AF12
};

static const struct stm32_gpio_dsc usart_gpio[] = {
	{STM32_GPIO_PORT_C, STM32_GPIO_PIN_6},	/* TX */
	{STM32_GPIO_PORT_C, STM32_GPIO_PIN_7},	/* RX */
};

static const struct stm32_gpio_dsc eth_gpio[] = {
	{STM32_GPIO_PORT_A, STM32_GPIO_PIN_1},	/* RMII_REF_CLK */
	{STM32_GPIO_PORT_A, STM32_GPIO_PIN_2},	/* RMII_MDIO */
	{STM32_GPIO_PORT_C, STM32_GPIO_PIN_1},	/* RMII_MDC */
	{STM32_GPIO_PORT_A, STM32_GPIO_PIN_7},	/* RMII_MII_CRS_DV */
	{STM32_GPIO_PORT_C, STM32_GPIO_PIN_4},	/* RMII_MII_RXD0 */
	{STM32_GPIO_PORT_C, STM32_GPIO_PIN_5},	/* RMII_MII_RXD1 */
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_2},	/* RMII_MII_RXER */
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_11},	/* RMII_MII_TX_EN */
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_13},	/* RMII_MII_TXD0 */
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_14},	/* RMII_MII_TXD1 */
};

static const struct stm32_gpio_dsc sdram_gpio[] = {
	{STM32_GPIO_PORT_C, STM32_GPIO_PIN_3},
	{STM32_GPIO_PORT_D, STM32_GPIO_PIN_0},
	{STM32_GPIO_PORT_D, STM32_GPIO_PIN_1},
	{STM32_GPIO_PORT_D, STM32_GPIO_PIN_3},
	{STM32_GPIO_PORT_D, STM32_GPIO_PIN_8},
	{STM32_GPIO_PORT_D, STM32_GPIO_PIN_9},
	{STM32_GPIO_PORT_D, STM32_GPIO_PIN_10},
	{STM32_GPIO_PORT_D, STM32_GPIO_PIN_14},
	{STM32_GPIO_PORT_D, STM32_GPIO_PIN_15},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_0},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_1},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_7},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_8},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_9},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_10},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_11},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_12},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_13},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_14},
	{STM32_GPIO_PORT_E, STM32_GPIO_PIN_15},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_0},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_1},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_2},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_3},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_4},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_5},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_11},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_12},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_13},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_14},
	{STM32_GPIO_PORT_F, STM32_GPIO_PIN_15},
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_0},
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_1},
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_4},
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_5},
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_8},
	{STM32_GPIO_PORT_G, STM32_GPIO_PIN_15},
	{STM32_GPIO_PORT_H, STM32_GPIO_PIN_3},
	{STM32_GPIO_PORT_H, STM32_GPIO_PIN_5}
};

int uart_setup_gpio(void)
{
	int i;
	int rv = 0;

	clock_setup(GPIO_C_CLOCK_CFG);
	for (i = 0; i < ARRAY_SIZE(usart_gpio); i++) {
		rv = stm32_gpio_config(&usart_gpio[i], &gpio_ctl_usart);
		if (rv)
			goto out;
	}

out:
	return rv;
}

int eth_setup_gpio(void)
{
	int i;
	int rv = 0;

	writel(SYSCFG_PMC_MII_RMII_SEL, SYSCFG_BASE + SYSCFG_PMC); /* RMII interface */

	clock_setup(GPIO_A_CLOCK_CFG);
	clock_setup(GPIO_C_CLOCK_CFG);
	clock_setup(GPIO_G_CLOCK_CFG);

	for (i = 0; i < ARRAY_SIZE(eth_gpio); i++) {
		rv = stm32_gpio_config(&eth_gpio[i], &gpio_ctl_eth);
		if (rv)
			goto out;
	}

	writel(ETH_DMABMR_SR, ETH_MAC_BASE + ETH_DMABMR); /* reset */

out:
	return rv;
}

int sdram_setup_gpio(void)
{
	int i;
	int rv = 0;

	clock_setup(GPIO_C_CLOCK_CFG);
	clock_setup(GPIO_D_CLOCK_CFG);
	clock_setup(GPIO_E_CLOCK_CFG);
	clock_setup(GPIO_F_CLOCK_CFG);
	clock_setup(GPIO_G_CLOCK_CFG);
	clock_setup(GPIO_H_CLOCK_CFG);

	for (i = 0; i < ARRAY_SIZE(sdram_gpio); i++) {
		rv = stm32_gpio_config(&sdram_gpio[i], &gpio_ctl_sdram);
		if (rv)
			goto out;
	}

out:
	return rv;
}

static const struct stm32x7_serial_platdata serial_platdata = {
	.base = (struct stm32_usart *)USART6_BASE,
	.clock = CONFIG_SYS_CLK_FREQ,
};

U_BOOT_DEVICE(stm32x7_serials) = {
	.name = "serial_stm32x7",
	.platdata = &serial_platdata,
};

u32 get_board_rev(void)
{
	return 0;
}

int board_early_init_f(void)
{
	int res;

	res = uart_setup_gpio();
	clock_setup(USART6_CLOCK_CFG);
	if (res)
		return res;

	clock_setup(SYSCFG_CLOCK_CFG);

	res = eth_setup_gpio();
	clock_setup(ETH_CLOCK_CFG);
	if (res)
		return res;

	res = sdram_setup_gpio();
	clock_setup(FMC_CLOCK_CFG);
	if (res)
		return res;

	nvic_irq_enable(USART6_IRQ);
	nvic_irq_enable(FMC_IRQ);
	nvic_irq_enable(ETH_IRQ);

	return 0;
}

int board_init(void)
{
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100; /* Not used */

	return 0;
}

static void sdram_init(void)
{
	uint32_t val = 0;

	/* SDRAM control */

	val = readl(FMC_BASE + FMC_SDCR1);

	val &= (uint32_t)~(FMC_SDCR1_NC | FMC_SDCR1_NR | FMC_SDCR1_MWID |
			   FMC_SDCR1_NB | FMC_SDCR1_CAS | FMC_SDCR1_WP |
			   FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE);

	val |= (uint32_t)(FMC_SDRAM_COLUMN_BITS_NUM_8
			  | FMC_SDRAM_ROW_BITS_NUM_12
			  | FMC_SDRAM_MEM_BUS_WIDTH_16
			  | FMC_SDRAM_INTERN_BANKS_NUM_4
			  | FMC_SDRAM_CAS_LATENCY_2
			  | FMC_SDRAM_WRITE_PROTECTION_DISABLE
			  | FMC_SDRAM_CLOCK_PERIOD_2
			  | FMC_SDRAM_RBURST_ENABLE
			  | FMC_SDRAM_RPIPE_DELAY_0);

	writel(val, FMC_BASE + FMC_SDCR1);

	/* SDRAM timing */

	val = readl(FMC_BASE + FMC_SDTR1);

	val &= ((uint32_t)~(FMC_SDTR1_TMRD  | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS |
			    FMC_SDTR1_TRC  | FMC_SDTR1_TWR | FMC_SDTR1_TRP | FMC_SDTR1_TRCD));

	val |= (uint32_t)((2 - 1)		/* TMRD */
			  | ((7 - 1) << 4)	/* TXSR */
			  | ((4 - 1) << 8)	/* TRAS */
			  | ((7 - 1) << 12)	/* TRC */
			  | ((2 - 1) << 16)	/* TWR */
			  | ((2 - 1) << 20)	/* TRP */
			  | ((2 - 1) << 24));	/* TRCD */

	writel(val, FMC_BASE + FMC_SDTR1);
}

static void sdram_command(int command, int refresh, int mode)
{
	uint32_t val;

	val = (uint32_t)(command
			 | FMC_SDCMR_CTB1
			 | ((refresh - 1) << 5)
			 | (mode << 9));

	writel(val, FMC_BASE + FMC_SDCMR);

	do {
		val = readl(FMC_BASE + FMC_SDSR);
		val &= FMC_SDSR_BUSY;
	} while (val);
}

static void sdram_enable(void)
{
	uint32_t val;

	sdram_command(FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);

	udelay(1000);

	sdram_command(FMC_SDRAM_CMD_PALL, 1, 0);
	sdram_command(FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);

	val = (uint32_t)(SDRAM_MODEREG_BURST_LENGTH_1
			 | SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL
			 | SDRAM_MODEREG_CAS_LATENCY_2
			 | SDRAM_MODEREG_OPERATING_MODE_STANDARD
			 | SDRAM_MODEREG_WRITEBURST_MODE_SINGLE);

	sdram_command(FMC_SDRAM_CMD_LOAD_MODE, 1, val);

	val = readl(FMC_BASE + FMC_SDRTR);
	val |= (0x603 << 1); /* refresh count */
	writel(val, FMC_BASE + FMC_SDRTR);
}

int dram_init(void)
{
	uint32_t val;

	sdram_init();
	sdram_enable();

	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE; /* kernel decompression overwrites u-boot? */
	gd->bd->bi_dram[0].size  = CONFIG_SYS_SDRAM_SIZE;

	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;

	/* MPU configuration for the SDRAM region */

	writel(0, MPU_BASE + MPU_RNR);
	writel(CONFIG_SYS_SDRAM_BASE | 0, MPU_BASE + MPU_RBAR); /* configure SDRAM bank */

	val = 1 | (22 << 1) | (0x29 << 16) | (0x3 << 24); /* clear XN */
	writel(val, MPU_BASE + MPU_RASR);

	writel(0x5, MPU_BASE + MPU_CTRL); /* enable MPU */

	return 0;
}

int checkboard (void)
{
	puts("Board: STM32F746 Discovery\n");
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int ret = 0;

#if defined(CONFIG_ETH_DESIGNWARE)
	u32 interface = PHY_INTERFACE_MODE_RMII;
	if (designware_initialize(ETH_MAC_BASE, interface) >= 0)
		ret++;
#endif
	return ret;
}
#endif

#if defined(CONFIG_OF_BOARD_SETUP)
void ft_cpu_setup(void *blob, bd_t *bd)
{
	/* Fixup ethernet MAC addresses */
	fdt_fixup_ethernet(blob);
}

int ft_system_setup(void *blob, bd_t *bd)
{
	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	return 0;
}
#endif
