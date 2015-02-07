

#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/mfd/sh_mobile_sdhi.h>
#include <linux/mtd/physmap.h>
#include <linux/delay.h>
#include <linux/smc91x.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/input/sh_keysc.h>
#include <linux/usb/r8a66597.h>
#include <video/sh_mobile_lcdc.h>
#include <media/sh_mobile_ceu.h>
#include <sound/sh_fsi.h>
#include <asm/io.h>
#include <asm/heartbeat.h>
#include <asm/sh_eth.h>
#include <asm/clock.h>
#include <asm/suspend.h>
#include <cpu/sh7724.h>
#include <mach-se/mach/se7724.h>




/* Heartbeat */
static struct resource heartbeat_resource = {
	.start  = PA_LED,
	.end    = PA_LED,
	.flags  = IORESOURCE_MEM | IORESOURCE_MEM_16BIT,
};

static struct platform_device heartbeat_device = {
	.name           = "heartbeat",
	.id             = -1,
	.num_resources  = 1,
	.resource       = &heartbeat_resource,
};

/* LAN91C111 */
static struct smc91x_platdata smc91x_info = {
	.flags = SMC91X_USE_16BIT | SMC91X_NOWAIT,
};

static struct resource smc91x_eth_resources[] = {
	[0] = {
		.name   = "SMC91C111" ,
		.start  = 0x1a300300,
		.end    = 0x1a30030f,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = IRQ0_SMC,
		.flags  = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	},
};

static struct platform_device smc91x_eth_device = {
	.name	= "smc91x",
	.num_resources  = ARRAY_SIZE(smc91x_eth_resources),
	.resource       = smc91x_eth_resources,
	.dev	= {
		.platform_data	= &smc91x_info,
	},
};

/* MTD */
static struct mtd_partition nor_flash_partitions[] = {
	{
		.name = "uboot",
		.offset = 0,
		.size = (1 * 1024 * 1024),
		.mask_flags = MTD_WRITEABLE,	/* Read-only */
	}, {
		.name = "kernel",
		.offset = MTDPART_OFS_APPEND,
		.size = (2 * 1024 * 1024),
	}, {
		.name = "free-area",
		.offset = MTDPART_OFS_APPEND,
		.size = MTDPART_SIZ_FULL,
	},
};

static struct physmap_flash_data nor_flash_data = {
	.width		= 2,
	.parts		= nor_flash_partitions,
	.nr_parts	= ARRAY_SIZE(nor_flash_partitions),
};

static struct resource nor_flash_resources[] = {
	[0] = {
		.name	= "NOR Flash",
		.start	= 0x00000000,
		.end	= 0x01ffffff,
		.flags	= IORESOURCE_MEM,
	}
};

static struct platform_device nor_flash_device = {
	.name		= "physmap-flash",
	.resource	= nor_flash_resources,
	.num_resources	= ARRAY_SIZE(nor_flash_resources),
	.dev		= {
		.platform_data = &nor_flash_data,
	},
};

/* LCDC */
static struct sh_mobile_lcdc_info lcdc_info = {
	.clock_source = LCDC_CLK_EXTERNAL,
	.ch[0] = {
		.chan = LCDC_CHAN_MAINLCD,
		.bpp = 16,
		.clock_divider = 1,
		.lcd_cfg = {
			.name = "LB070WV1",
			.sync = 0, /* hsync and vsync are active low */
		},
		.lcd_size_cfg = { /* 7.0 inch */
			.width = 152,
			.height = 91,
		},
		.board_cfg = {
		},
	}
};

static struct resource lcdc_resources[] = {
	[0] = {
		.name	= "LCDC",
		.start	= 0xfe940000,
		.end	= 0xfe942fff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= 106,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device lcdc_device = {
	.name		= "sh_mobile_lcdc_fb",
	.num_resources	= ARRAY_SIZE(lcdc_resources),
	.resource	= lcdc_resources,
	.dev		= {
		.platform_data	= &lcdc_info,
	},
	.archdata = {
		.hwblk_id = HWBLK_LCDC,
	},
};

/* CEU0 */
static struct sh_mobile_ceu_info sh_mobile_ceu0_info = {
	.flags = SH_CEU_FLAG_USE_8BIT_BUS,
};

static struct resource ceu0_resources[] = {
	[0] = {
		.name	= "CEU0",
		.start	= 0xfe910000,
		.end	= 0xfe91009f,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start  = 52,
		.flags  = IORESOURCE_IRQ,
	},
	[2] = {
		/* place holder for contiguous memory */
	},
};

static struct platform_device ceu0_device = {
	.name		= "sh_mobile_ceu",
	.id             = 0, /* "ceu0" clock */
	.num_resources	= ARRAY_SIZE(ceu0_resources),
	.resource	= ceu0_resources,
	.dev	= {
		.platform_data	= &sh_mobile_ceu0_info,
	},
	.archdata = {
		.hwblk_id = HWBLK_CEU0,
	},
};

/* CEU1 */
static struct sh_mobile_ceu_info sh_mobile_ceu1_info = {
	.flags = SH_CEU_FLAG_USE_8BIT_BUS,
};

static struct resource ceu1_resources[] = {
	[0] = {
		.name	= "CEU1",
		.start	= 0xfe914000,
		.end	= 0xfe91409f,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start  = 63,
		.flags  = IORESOURCE_IRQ,
	},
	[2] = {
		/* place holder for contiguous memory */
	},
};

static struct platform_device ceu1_device = {
	.name		= "sh_mobile_ceu",
	.id             = 1, /* "ceu1" clock */
	.num_resources	= ARRAY_SIZE(ceu1_resources),
	.resource	= ceu1_resources,
	.dev	= {
		.platform_data	= &sh_mobile_ceu1_info,
	},
	.archdata = {
		.hwblk_id = HWBLK_CEU1,
	},
};

/* FSI */
#define FCLKACR		0xa4150008
static void fsimck_init(struct clk *clk)
{
	u32 status = __raw_readl(clk->enable_reg);

	/* use external clock */
	status &= ~0x000000ff;
	status |= 0x00000080;
	__raw_writel(status, clk->enable_reg);
}

static struct clk_ops fsimck_clk_ops = {
	.init = fsimck_init,
};

static struct clk fsimcka_clk = {
	.ops		= &fsimck_clk_ops,
	.enable_reg	= (void __iomem *)FCLKACR,
	.rate		= 0, /* unknown */
};

/* change J20, J21, J22 pin to 1-2 connection to use slave mode */
static struct sh_fsi_platform_info fsi_info = {
	.porta_flags = SH_FSI_BRS_INV |
		       SH_FSI_OUT_SLAVE_MODE |
		       SH_FSI_IN_SLAVE_MODE |
		       SH_FSI_OFMT(PCM) |
		       SH_FSI_IFMT(PCM),
};

static struct resource fsi_resources[] = {
	[0] = {
		.name	= "FSI",
		.start	= 0xFE3C0000,
		.end	= 0xFE3C021d,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start  = 108,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device fsi_device = {
	.name		= "sh_fsi",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(fsi_resources),
	.resource	= fsi_resources,
	.dev	= {
		.platform_data	= &fsi_info,
	},
	.archdata = {
		.hwblk_id = HWBLK_SPU, /* FSI needs SPU hwblk */
	},
};

/* KEYSC in SoC (Needs SW33-2 set to ON) */
static struct sh_keysc_info keysc_info = {
	.mode = SH_KEYSC_MODE_1,
	.scan_timing = 3,
	.delay = 50,
	.keycodes = {
		KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
		KEY_6, KEY_7, KEY_8, KEY_9, KEY_A,
		KEY_B, KEY_C, KEY_D, KEY_E, KEY_F,
		KEY_G, KEY_H, KEY_I, KEY_K, KEY_L,
		KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q,
		KEY_R, KEY_S, KEY_T, KEY_U, KEY_V,
	},
};

static struct resource keysc_resources[] = {
	[0] = {
		.name	= "KEYSC",
		.start  = 0x044b0000,
		.end    = 0x044b000f,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = 79,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device keysc_device = {
	.name           = "sh_keysc",
	.id             = 0, /* "keysc0" clock */
	.num_resources  = ARRAY_SIZE(keysc_resources),
	.resource       = keysc_resources,
	.dev	= {
		.platform_data	= &keysc_info,
	},
	.archdata = {
		.hwblk_id = HWBLK_KEYSC,
	},
};

/* SH Eth */
static struct resource sh_eth_resources[] = {
	[0] = {
		.start = SH_ETH_ADDR,
		.end   = SH_ETH_ADDR + 0x1FC,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = 91,
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	},
};

static struct sh_eth_plat_data sh_eth_plat = {
	.phy = 0x1f, /* SMSC LAN8187 */
	.edmac_endian = EDMAC_LITTLE_ENDIAN,
};

static struct platform_device sh_eth_device = {
	.name = "sh-eth",
	.id	= 0,
	.dev = {
		.platform_data = &sh_eth_plat,
	},
	.num_resources = ARRAY_SIZE(sh_eth_resources),
	.resource = sh_eth_resources,
	.archdata = {
		.hwblk_id = HWBLK_ETHER,
	},
};

static struct r8a66597_platdata sh7724_usb0_host_data = {
	.on_chip = 1,
};

static struct resource sh7724_usb0_host_resources[] = {
	[0] = {
		.start	= 0xa4d80000,
		.end	= 0xa4d80124 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= 65,
		.end	= 65,
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_LOW,
	},
};

static struct platform_device sh7724_usb0_host_device = {
	.name		= "r8a66597_hcd",
	.id		= 0,
	.dev = {
		.dma_mask		= NULL,         /*  not use dma */
		.coherent_dma_mask	= 0xffffffff,
		.platform_data		= &sh7724_usb0_host_data,
	},
	.num_resources	= ARRAY_SIZE(sh7724_usb0_host_resources),
	.resource	= sh7724_usb0_host_resources,
	.archdata = {
		.hwblk_id = HWBLK_USB0,
	},
};

static struct r8a66597_platdata sh7724_usb1_gadget_data = {
	.on_chip = 1,
};

static struct resource sh7724_usb1_gadget_resources[] = {
	[0] = {
		.start	= 0xa4d90000,
		.end	= 0xa4d90123,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= 66,
		.end	= 66,
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_LOW,
	},
};

static struct platform_device sh7724_usb1_gadget_device = {
	.name		= "r8a66597_udc",
	.id		= 1, /* USB1 */
	.dev = {
		.dma_mask		= NULL,         /*  not use dma */
		.coherent_dma_mask	= 0xffffffff,
		.platform_data		= &sh7724_usb1_gadget_data,
	},
	.num_resources	= ARRAY_SIZE(sh7724_usb1_gadget_resources),
	.resource	= sh7724_usb1_gadget_resources,
};

static struct resource sdhi0_cn7_resources[] = {
	[0] = {
		.name	= "SDHI0",
		.start  = 0x04ce0000,
		.end    = 0x04ce01ff,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = 100,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct sh_mobile_sdhi_info sh7724_sdhi0_data = {
	.dma_slave_tx	= SHDMA_SLAVE_SDHI0_TX,
	.dma_slave_rx	= SHDMA_SLAVE_SDHI0_RX,
};

static struct platform_device sdhi0_cn7_device = {
	.name           = "sh_mobile_sdhi",
	.id		= 0,
	.num_resources  = ARRAY_SIZE(sdhi0_cn7_resources),
	.resource       = sdhi0_cn7_resources,
	.dev = {
		.platform_data	= &sh7724_sdhi0_data,
	},
	.archdata = {
		.hwblk_id = HWBLK_SDHI0,
	},
};

static struct resource sdhi1_cn8_resources[] = {
	[0] = {
		.name	= "SDHI1",
		.start  = 0x04cf0000,
		.end    = 0x04cf01ff,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = 23,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct sh_mobile_sdhi_info sh7724_sdhi1_data = {
	.dma_slave_tx	= SHDMA_SLAVE_SDHI1_TX,
	.dma_slave_rx	= SHDMA_SLAVE_SDHI1_RX,
};

static struct platform_device sdhi1_cn8_device = {
	.name           = "sh_mobile_sdhi",
	.id		= 1,
	.num_resources  = ARRAY_SIZE(sdhi1_cn8_resources),
	.resource       = sdhi1_cn8_resources,
	.dev = {
		.platform_data	= &sh7724_sdhi1_data,
	},
	.archdata = {
		.hwblk_id = HWBLK_SDHI1,
	},
};

/* IrDA */
static struct resource irda_resources[] = {
	[0] = {
		.name	= "IrDA",
		.start  = 0xA45D0000,
		.end    = 0xA45D0049,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = 20,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device irda_device = {
	.name           = "sh_sir",
	.num_resources  = ARRAY_SIZE(irda_resources),
	.resource       = irda_resources,
};

#include <media/ak881x.h>
#include <media/sh_vou.h>

static struct ak881x_pdata ak881x_pdata = {
	.flags = AK881X_IF_MODE_SLAVE,
};

static struct i2c_board_info ak8813 = {
	/* With open J18 jumper address is 0x21 */
	I2C_BOARD_INFO("ak8813", 0x20),
	.platform_data = &ak881x_pdata,
};

static struct sh_vou_pdata sh_vou_pdata = {
	.bus_fmt	= SH_VOU_BUS_8BIT,
	.flags		= SH_VOU_HSYNC_LOW | SH_VOU_VSYNC_LOW,
	.board_info	= &ak8813,
	.i2c_adap	= 0,
	.module_name	= "ak881x",
};

static struct resource sh_vou_resources[] = {
	[0] = {
		.start  = 0xfe960000,
		.end    = 0xfe962043,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = 55,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device vou_device = {
	.name           = "sh-vou",
	.id		= -1,
	.num_resources  = ARRAY_SIZE(sh_vou_resources),
	.resource       = sh_vou_resources,
	.dev		= {
		.platform_data	= &sh_vou_pdata,
	},
	.archdata	= {
		.hwblk_id	= HWBLK_VOU,
	},
};

static struct platform_device *ms7724se_devices[] __initdata = {
	&heartbeat_device,
	&smc91x_eth_device,
	&lcdc_device,
	&nor_flash_device,
	&ceu0_device,
	&ceu1_device,
	&keysc_device,
	&sh_eth_device,
	&sh7724_usb0_host_device,
	&sh7724_usb1_gadget_device,
	&fsi_device,
	&sdhi0_cn7_device,
	&sdhi1_cn8_device,
	&irda_device,
	&vou_device,
};

/* I2C device */
static struct i2c_board_info i2c0_devices[] = {
	{
		I2C_BOARD_INFO("ak4642", 0x12),
	},
};

#define EEPROM_OP   0xBA206000
#define EEPROM_ADR  0xBA206004
#define EEPROM_DATA 0xBA20600C
#define EEPROM_STAT 0xBA206010
#define EEPROM_STRT 0xBA206014
static int __init sh_eth_is_eeprom_ready(void)
{
	int t = 10000;

	while (t--) {
		if (!__raw_readw(EEPROM_STAT))
			return 1;
		udelay(1);
	}

	printk(KERN_ERR "ms7724se can not access to eeprom\n");
	return 0;
}

static void __init sh_eth_init(void)
{
	int i;
	u16 mac;

	/* check EEPROM status */
	if (!sh_eth_is_eeprom_ready())
		return;

	/* read MAC addr from EEPROM */
	for (i = 0 ; i < 3 ; i++) {
		__raw_writew(0x0, EEPROM_OP); /* read */
		__raw_writew(i*2, EEPROM_ADR);
		__raw_writew(0x1, EEPROM_STRT);
		if (!sh_eth_is_eeprom_ready())
			return;

		mac = __raw_readw(EEPROM_DATA);
		sh_eth_plat.mac_addr[i << 1] = mac & 0xff;
		sh_eth_plat.mac_addr[(i << 1) + 1] = mac >> 8;
	}
}

#define SW4140    0xBA201000
#define FPGA_OUT  0xBA200400
#define PORT_HIZA 0xA4050158
#define PORT_MSELCRB 0xA4050182

#define SW41_A    0x0100
#define SW41_B    0x0200
#define SW41_C    0x0400
#define SW41_D    0x0800
#define SW41_E    0x1000
#define SW41_F    0x2000
#define SW41_G    0x4000
#define SW41_H    0x8000

extern char ms7724se_sdram_enter_start;
extern char ms7724se_sdram_enter_end;
extern char ms7724se_sdram_leave_start;
extern char ms7724se_sdram_leave_end;


static int __init arch_setup(void)
{
	/* enable I2C device */
	i2c_register_board_info(0, i2c0_devices,
				ARRAY_SIZE(i2c0_devices));
	return 0;
}
arch_initcall(arch_setup);

static int __init devices_setup(void)
{
	u16 sw = __raw_readw(SW4140); /* select camera, monitor */
	struct clk *clk;
	u16 fpga_out;

	/* register board specific self-refresh code */
	sh_mobile_register_self_refresh(SUSP_SH_STANDBY | SUSP_SH_SF |
					SUSP_SH_RSTANDBY,
					&ms7724se_sdram_enter_start,
					&ms7724se_sdram_enter_end,
					&ms7724se_sdram_leave_start,
					&ms7724se_sdram_leave_end);
	/* Reset Release */
	fpga_out = __raw_readw(FPGA_OUT);
	/* bit4: NTSC_PDN, bit5: NTSC_RESET */
	fpga_out &= ~((1 << 1)  | /* LAN */
		      (1 << 4)  | /* AK8813 PDN */
		      (1 << 5)  | /* AK8813 RESET */
		      (1 << 6)  | /* VIDEO DAC */
		      (1 << 7)  | /* AK4643 */
		      (1 << 8)  | /* IrDA */
		      (1 << 12) | /* USB0 */
		      (1 << 14)); /* RMII */
	__raw_writew(fpga_out | (1 << 4), FPGA_OUT);

	udelay(10);

	/* AK8813 RESET */
	__raw_writew(fpga_out | (1 << 5), FPGA_OUT);

	udelay(10);

	__raw_writew(fpga_out, FPGA_OUT);

	/* turn on USB clocks, use external clock */
	__raw_writew((__raw_readw(PORT_MSELCRB) & ~0xc000) | 0x8000, PORT_MSELCRB);

	/* Let LED9 show STATUS2 */
	gpio_request(GPIO_FN_STATUS2, NULL);

	/* Lit LED10 show STATUS0 */
	gpio_request(GPIO_FN_STATUS0, NULL);

	/* Lit LED11 show PDSTATUS */
	gpio_request(GPIO_FN_PDSTATUS, NULL);

	/* enable USB0 port */
	__raw_writew(0x0600, 0xa40501d4);

	/* enable USB1 port */
	__raw_writew(0x0600, 0xa4050192);

	/* enable IRQ 0,1,2 */
	gpio_request(GPIO_FN_INTC_IRQ0, NULL);
	gpio_request(GPIO_FN_INTC_IRQ1, NULL);
	gpio_request(GPIO_FN_INTC_IRQ2, NULL);

	/* enable SCIFA3 */
	gpio_request(GPIO_FN_SCIF3_I_SCK, NULL);
	gpio_request(GPIO_FN_SCIF3_I_RXD, NULL);
	gpio_request(GPIO_FN_SCIF3_I_TXD, NULL);
	gpio_request(GPIO_FN_SCIF3_I_CTS, NULL);
	gpio_request(GPIO_FN_SCIF3_I_RTS, NULL);

	/* enable LCDC */
	gpio_request(GPIO_FN_LCDD23,   NULL);
	gpio_request(GPIO_FN_LCDD22,   NULL);
	gpio_request(GPIO_FN_LCDD21,   NULL);
	gpio_request(GPIO_FN_LCDD20,   NULL);
	gpio_request(GPIO_FN_LCDD19,   NULL);
	gpio_request(GPIO_FN_LCDD18,   NULL);
	gpio_request(GPIO_FN_LCDD17,   NULL);
	gpio_request(GPIO_FN_LCDD16,   NULL);
	gpio_request(GPIO_FN_LCDD15,   NULL);
	gpio_request(GPIO_FN_LCDD14,   NULL);
	gpio_request(GPIO_FN_LCDD13,   NULL);
	gpio_request(GPIO_FN_LCDD12,   NULL);
	gpio_request(GPIO_FN_LCDD11,   NULL);
	gpio_request(GPIO_FN_LCDD10,   NULL);
	gpio_request(GPIO_FN_LCDD9,    NULL);
	gpio_request(GPIO_FN_LCDD8,    NULL);
	gpio_request(GPIO_FN_LCDD7,    NULL);
	gpio_request(GPIO_FN_LCDD6,    NULL);
	gpio_request(GPIO_FN_LCDD5,    NULL);
	gpio_request(GPIO_FN_LCDD4,    NULL);
	gpio_request(GPIO_FN_LCDD3,    NULL);
	gpio_request(GPIO_FN_LCDD2,    NULL);
	gpio_request(GPIO_FN_LCDD1,    NULL);
	gpio_request(GPIO_FN_LCDD0,    NULL);
	gpio_request(GPIO_FN_LCDDISP,  NULL);
	gpio_request(GPIO_FN_LCDHSYN,  NULL);
	gpio_request(GPIO_FN_LCDDCK,   NULL);
	gpio_request(GPIO_FN_LCDVSYN,  NULL);
	gpio_request(GPIO_FN_LCDDON,   NULL);
	gpio_request(GPIO_FN_LCDVEPWC, NULL);
	gpio_request(GPIO_FN_LCDVCPWC, NULL);
	gpio_request(GPIO_FN_LCDRD,    NULL);
	gpio_request(GPIO_FN_LCDLCLK,  NULL);
	__raw_writew((__raw_readw(PORT_HIZA) & ~0x0001), PORT_HIZA);

	/* enable CEU0 */
	gpio_request(GPIO_FN_VIO0_D15, NULL);
	gpio_request(GPIO_FN_VIO0_D14, NULL);
	gpio_request(GPIO_FN_VIO0_D13, NULL);
	gpio_request(GPIO_FN_VIO0_D12, NULL);
	gpio_request(GPIO_FN_VIO0_D11, NULL);
	gpio_request(GPIO_FN_VIO0_D10, NULL);
	gpio_request(GPIO_FN_VIO0_D9,  NULL);
	gpio_request(GPIO_FN_VIO0_D8,  NULL);
	gpio_request(GPIO_FN_VIO0_D7,  NULL);
	gpio_request(GPIO_FN_VIO0_D6,  NULL);
	gpio_request(GPIO_FN_VIO0_D5,  NULL);
	gpio_request(GPIO_FN_VIO0_D4,  NULL);
	gpio_request(GPIO_FN_VIO0_D3,  NULL);
	gpio_request(GPIO_FN_VIO0_D2,  NULL);
	gpio_request(GPIO_FN_VIO0_D1,  NULL);
	gpio_request(GPIO_FN_VIO0_D0,  NULL);
	gpio_request(GPIO_FN_VIO0_VD,  NULL);
	gpio_request(GPIO_FN_VIO0_CLK, NULL);
	gpio_request(GPIO_FN_VIO0_FLD, NULL);
	gpio_request(GPIO_FN_VIO0_HD,  NULL);
	platform_resource_setup_memory(&ceu0_device, "ceu0", 4 << 20);

	/* enable CEU1 */
	gpio_request(GPIO_FN_VIO1_D7,  NULL);
	gpio_request(GPIO_FN_VIO1_D6,  NULL);
	gpio_request(GPIO_FN_VIO1_D5,  NULL);
	gpio_request(GPIO_FN_VIO1_D4,  NULL);
	gpio_request(GPIO_FN_VIO1_D3,  NULL);
	gpio_request(GPIO_FN_VIO1_D2,  NULL);
	gpio_request(GPIO_FN_VIO1_D1,  NULL);
	gpio_request(GPIO_FN_VIO1_D0,  NULL);
	gpio_request(GPIO_FN_VIO1_FLD, NULL);
	gpio_request(GPIO_FN_VIO1_HD,  NULL);
	gpio_request(GPIO_FN_VIO1_VD,  NULL);
	gpio_request(GPIO_FN_VIO1_CLK, NULL);
	platform_resource_setup_memory(&ceu1_device, "ceu1", 4 << 20);

	/* KEYSC */
	gpio_request(GPIO_FN_KEYOUT5_IN5, NULL);
	gpio_request(GPIO_FN_KEYOUT4_IN6, NULL);
	gpio_request(GPIO_FN_KEYIN4,      NULL);
	gpio_request(GPIO_FN_KEYIN3,      NULL);
	gpio_request(GPIO_FN_KEYIN2,      NULL);
	gpio_request(GPIO_FN_KEYIN1,      NULL);
	gpio_request(GPIO_FN_KEYIN0,      NULL);
	gpio_request(GPIO_FN_KEYOUT3,     NULL);
	gpio_request(GPIO_FN_KEYOUT2,     NULL);
	gpio_request(GPIO_FN_KEYOUT1,     NULL);
	gpio_request(GPIO_FN_KEYOUT0,     NULL);

	/* enable FSI */
	gpio_request(GPIO_FN_FSIMCKB,    NULL);
	gpio_request(GPIO_FN_FSIMCKA,    NULL);
	gpio_request(GPIO_FN_FSIOASD,    NULL);
	gpio_request(GPIO_FN_FSIIABCK,   NULL);
	gpio_request(GPIO_FN_FSIIALRCK,  NULL);
	gpio_request(GPIO_FN_FSIOABCK,   NULL);
	gpio_request(GPIO_FN_FSIOALRCK,  NULL);
	gpio_request(GPIO_FN_CLKAUDIOAO, NULL);
	gpio_request(GPIO_FN_FSIIBSD,    NULL);
	gpio_request(GPIO_FN_FSIOBSD,    NULL);
	gpio_request(GPIO_FN_FSIIBBCK,   NULL);
	gpio_request(GPIO_FN_FSIIBLRCK,  NULL);
	gpio_request(GPIO_FN_FSIOBBCK,   NULL);
	gpio_request(GPIO_FN_FSIOBLRCK,  NULL);
	gpio_request(GPIO_FN_CLKAUDIOBO, NULL);
	gpio_request(GPIO_FN_FSIIASD,    NULL);

	/* set SPU2 clock to 83.4 MHz */
	clk = clk_get(NULL, "spu_clk");
	if (clk) {
		clk_set_rate(clk, clk_round_rate(clk, 83333333));
		clk_put(clk);
	}

	/* change parent of FSI A */
	clk = clk_get(NULL, "fsia_clk");
	if (clk) {
		clk_register(&fsimcka_clk);
		clk_set_parent(clk, &fsimcka_clk);
		clk_set_rate(clk, 11000);
		clk_set_rate(&fsimcka_clk, 11000);
		clk_put(clk);
	}

	/* SDHI0 connected to cn7 */
	gpio_request(GPIO_FN_SDHI0CD, NULL);
	gpio_request(GPIO_FN_SDHI0WP, NULL);
	gpio_request(GPIO_FN_SDHI0D3, NULL);
	gpio_request(GPIO_FN_SDHI0D2, NULL);
	gpio_request(GPIO_FN_SDHI0D1, NULL);
	gpio_request(GPIO_FN_SDHI0D0, NULL);
	gpio_request(GPIO_FN_SDHI0CMD, NULL);
	gpio_request(GPIO_FN_SDHI0CLK, NULL);

	/* SDHI1 connected to cn8 */
	gpio_request(GPIO_FN_SDHI1CD, NULL);
	gpio_request(GPIO_FN_SDHI1WP, NULL);
	gpio_request(GPIO_FN_SDHI1D3, NULL);
	gpio_request(GPIO_FN_SDHI1D2, NULL);
	gpio_request(GPIO_FN_SDHI1D1, NULL);
	gpio_request(GPIO_FN_SDHI1D0, NULL);
	gpio_request(GPIO_FN_SDHI1CMD, NULL);
	gpio_request(GPIO_FN_SDHI1CLK, NULL);

	/* enable IrDA */
	gpio_request(GPIO_FN_IRDA_OUT, NULL);
	gpio_request(GPIO_FN_IRDA_IN,  NULL);

	/*
	 * enable SH-Eth
	 *
	 * please remove J33 pin from your board !!
	 *
	 * ms7724 board should not use GPIO_FN_LNKSTA pin
	 * So, This time PTX5 is set to input pin
	 */
	gpio_request(GPIO_FN_RMII_RXD0,    NULL);
	gpio_request(GPIO_FN_RMII_RXD1,    NULL);
	gpio_request(GPIO_FN_RMII_TXD0,    NULL);
	gpio_request(GPIO_FN_RMII_TXD1,    NULL);
	gpio_request(GPIO_FN_RMII_REF_CLK, NULL);
	gpio_request(GPIO_FN_RMII_TX_EN,   NULL);
	gpio_request(GPIO_FN_RMII_RX_ER,   NULL);
	gpio_request(GPIO_FN_RMII_CRS_DV,  NULL);
	gpio_request(GPIO_FN_MDIO,         NULL);
	gpio_request(GPIO_FN_MDC,          NULL);
	gpio_request(GPIO_PTX5, NULL);
	gpio_direction_input(GPIO_PTX5);
	sh_eth_init();

	if (sw & SW41_B) {
		/* 720p */
		lcdc_info.ch[0].lcd_cfg.xres         = 1280;
		lcdc_info.ch[0].lcd_cfg.yres         = 720;
		lcdc_info.ch[0].lcd_cfg.left_margin  = 220;
		lcdc_info.ch[0].lcd_cfg.right_margin = 110;
		lcdc_info.ch[0].lcd_cfg.hsync_len    = 40;
		lcdc_info.ch[0].lcd_cfg.upper_margin = 20;
		lcdc_info.ch[0].lcd_cfg.lower_margin = 5;
		lcdc_info.ch[0].lcd_cfg.vsync_len    = 5;
	} else {
		/* VGA */
		lcdc_info.ch[0].lcd_cfg.xres         = 640;
		lcdc_info.ch[0].lcd_cfg.yres         = 480;
		lcdc_info.ch[0].lcd_cfg.left_margin  = 105;
		lcdc_info.ch[0].lcd_cfg.right_margin = 50;
		lcdc_info.ch[0].lcd_cfg.hsync_len    = 96;
		lcdc_info.ch[0].lcd_cfg.upper_margin = 33;
		lcdc_info.ch[0].lcd_cfg.lower_margin = 10;
		lcdc_info.ch[0].lcd_cfg.vsync_len    = 2;
	}

	if (sw & SW41_A) {
		/* Digital monitor */
		lcdc_info.ch[0].interface_type = RGB18;
		lcdc_info.ch[0].flags          = 0;
	} else {
		/* Analog monitor */
		lcdc_info.ch[0].interface_type = RGB24;
		lcdc_info.ch[0].flags          = LCDC_FLAGS_DWPOL;
	}

	/* VOU */
	gpio_request(GPIO_FN_DV_D15, NULL);
	gpio_request(GPIO_FN_DV_D14, NULL);
	gpio_request(GPIO_FN_DV_D13, NULL);
	gpio_request(GPIO_FN_DV_D12, NULL);
	gpio_request(GPIO_FN_DV_D11, NULL);
	gpio_request(GPIO_FN_DV_D10, NULL);
	gpio_request(GPIO_FN_DV_D9, NULL);
	gpio_request(GPIO_FN_DV_D8, NULL);
	gpio_request(GPIO_FN_DV_CLKI, NULL);
	gpio_request(GPIO_FN_DV_CLK, NULL);
	gpio_request(GPIO_FN_DV_VSYNC, NULL);
	gpio_request(GPIO_FN_DV_HSYNC, NULL);

	return platform_add_devices(ms7724se_devices,
				    ARRAY_SIZE(ms7724se_devices));
}
device_initcall(devices_setup);

static struct sh_machine_vector mv_ms7724se __initmv = {
	.mv_name	= "ms7724se",
	.mv_init_irq	= init_se7724_IRQ,
	.mv_nr_irqs	= SE7724_FPGA_IRQ_BASE + SE7724_FPGA_IRQ_NR,
};
