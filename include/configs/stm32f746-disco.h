/*
 * (C) Copyright 2016
 * Vikas Manocha, <vikas.manocha@st.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_SYS_THUMB_BUILD
#define CONFIG_STM32F7DISCOVERY
/*#define CONFIG_SYS_NO_FLASH*/

#define CONFIG_BOARD_EARLY_INIT_F

#define CONFIG_SYS_FLASH_BASE		0x08000000
#define CONFIG_SYS_INIT_SP_ADDR		0x20050000
#define CONFIG_SYS_TEXT_BASE		0x08000000

#define CONFIG_SYS_ICACHE_OFF
#define CONFIG_SYS_DCACHE_OFF

/*
 * Configuration of the external SDRAM memory
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SRAM_BASE		0x20000000
#define CONFIG_SYS_SRAM_SIZE		((64 + 240) << 10)
#define CONFIG_SYS_RAM_CS		1
#define CONFIG_SYS_RAM_FREQ_DIV		2
#define CONFIG_SYS_SDRAM_BASE		0xC0000000
#define CONFIG_SYS_SDRAM_SIZE           (8 * 1024 * 1024)

#define CONFIG_LOADADDR			0xC0600000
#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

#define CONFIG_SYS_MAX_FLASH_SECT	8
#define CONFIG_SYS_MAX_FLASH_BANKS	1

#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE			(8 << 10)

#define CONFIG_STM32_GPIO
#define CONFIG_STM32_FLASH
#define CONFIG_STM32X7_SERIAL

#define CONFIG_SYS_CLK_FREQ		16*1000*1000 /* 180 MHz */
#define CONFIG_SYS_HZ_CLOCK		1000000	/* Timer is clocked at 1MHz */

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

#define CONFIG_SYS_CBSIZE		1024
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE \
					+ sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_MALLOC_LEN		(256 * 1024)
#define CONFIG_STACKSIZE		(64 << 10)

#define CONFIG_BAUDRATE			115200

#define CONFIG_EXTRA_ENV_SETTINGS \
	"consoledev=ttyS0\0" \
	"fdt_addr_r=0xC0580000\0" \
	"fdt_file=stm32f746-disco.dtb\0"

#define CONFIG_HAS_ETH0		1

#define CONFIG_IPADDR		192.168.1.2
#define CONFIG_HOSTNAME		stm32f746
#define CONFIG_ROOTPATH		"/var/lib/tftpboot"
#define CONFIG_BOOTFILE		"uImage"
#define CONFIG_SERVERIP		192.168.1.1
#define CONFIG_GATEWAYIP	192.168.1.1
#define CONFIG_NETMASK		255.255.255.0

#define CONFIG_NFSBOOTCOMMAND \
   "setenv bootargs root=/dev/nfs rw " \
      "nfsroot=$serverip:$rootpath " \
      "ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off " \
      "console=$consoledev,$baudrate earlyprintk consoleblank=0 ignore_loglevel $othbootargs;" \
   "tftp $loadaddr $bootfile;" \
   "tftp $fdt_addr_r $fdt_file;" \
   "fdt addr $fdt_addr_r; fdt resize; fdt chosen;" \
   "fdt list /chosen;" \
   "bootm $loadaddr - $fdt_addr_r"

#define CONFIG_BOOTCOMMAND	CONFIG_NFSBOOTCOMMAND

#define CONFIG_BOOTDELAY		2
#define CONFIG_LOADS_ECHO		1

/* Ethernet Hardware */
#define CONFIG_CMD_RMII
#define CONFIG_PHY_SMSC
#define CONFIG_RMII
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_DW_ALTDESCRIPTOR		1

/*
 * Command line configuration.
 */
#define CONFIG_SYS_LONGHELP
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING

#define CONFIG_CMD_MEM
#endif /* __CONFIG_H */
