

#include <linux/platform_device.h>
#include <linux/dma-mapping.h>

#include <mach/dma.h>
#include <mach/map.h>
#include <mach/irqs.h>
#include <mach/gpio.h>
#include <mach/spi-clocks.h>

#include <plat/s3c64xx-spi.h>
#include <plat/gpio-cfg.h>

static char *spi_src_clks[] = {
	[S5P6440_SPI_SRCCLK_PCLK] = "pclk",
	[S5P6440_SPI_SRCCLK_SCLK] = "spi_epll",
};

/* SPI Controller platform_devices */

static int s5p6440_spi_cfg_gpio(struct platform_device *pdev)
{
	switch (pdev->id) {
	case 0:
		s3c_gpio_cfgpin(S5P6440_GPC(0), S3C_GPIO_SFN(2));
		s3c_gpio_cfgpin(S5P6440_GPC(1), S3C_GPIO_SFN(2));
		s3c_gpio_cfgpin(S5P6440_GPC(2), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5P6440_GPC(0), S3C_GPIO_PULL_UP);
		s3c_gpio_setpull(S5P6440_GPC(1), S3C_GPIO_PULL_UP);
		s3c_gpio_setpull(S5P6440_GPC(2), S3C_GPIO_PULL_UP);
		break;

	case 1:
		s3c_gpio_cfgpin(S5P6440_GPC(4), S3C_GPIO_SFN(2));
		s3c_gpio_cfgpin(S5P6440_GPC(5), S3C_GPIO_SFN(2));
		s3c_gpio_cfgpin(S5P6440_GPC(6), S3C_GPIO_SFN(2));
		s3c_gpio_setpull(S5P6440_GPC(4), S3C_GPIO_PULL_UP);
		s3c_gpio_setpull(S5P6440_GPC(5), S3C_GPIO_PULL_UP);
		s3c_gpio_setpull(S5P6440_GPC(6), S3C_GPIO_PULL_UP);
		break;

	default:
		dev_err(&pdev->dev, "Invalid SPI Controller number!");
		return -EINVAL;
	}

	return 0;
}

static struct resource s5p6440_spi0_resource[] = {
	[0] = {
		.start = S5P6440_PA_SPI0,
		.end   = S5P6440_PA_SPI0 + 0x100 - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = DMACH_SPI0_TX,
		.end   = DMACH_SPI0_TX,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.start = DMACH_SPI0_RX,
		.end   = DMACH_SPI0_RX,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.start = IRQ_SPI0,
		.end   = IRQ_SPI0,
		.flags = IORESOURCE_IRQ,
	},
};

static struct s3c64xx_spi_info s5p6440_spi0_pdata = {
	.cfg_gpio = s5p6440_spi_cfg_gpio,
	.fifo_lvl_mask = 0x1ff,
	.rx_lvl_offset = 15,
};

static u64 spi_dmamask = DMA_BIT_MASK(32);

struct platform_device s5p6440_device_spi0 = {
	.name		  = "s3c64xx-spi",
	.id		  = 0,
	.num_resources	  = ARRAY_SIZE(s5p6440_spi0_resource),
	.resource	  = s5p6440_spi0_resource,
	.dev = {
		.dma_mask		= &spi_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data = &s5p6440_spi0_pdata,
	},
};

static struct resource s5p6440_spi1_resource[] = {
	[0] = {
		.start = S5P6440_PA_SPI1,
		.end   = S5P6440_PA_SPI1 + 0x100 - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = DMACH_SPI1_TX,
		.end   = DMACH_SPI1_TX,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.start = DMACH_SPI1_RX,
		.end   = DMACH_SPI1_RX,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.start = IRQ_SPI1,
		.end   = IRQ_SPI1,
		.flags = IORESOURCE_IRQ,
	},
};

static struct s3c64xx_spi_info s5p6440_spi1_pdata = {
	.cfg_gpio = s5p6440_spi_cfg_gpio,
	.fifo_lvl_mask = 0x7f,
	.rx_lvl_offset = 15,
};

struct platform_device s5p6440_device_spi1 = {
	.name		  = "s3c64xx-spi",
	.id		  = 1,
	.num_resources	  = ARRAY_SIZE(s5p6440_spi1_resource),
	.resource	  = s5p6440_spi1_resource,
	.dev = {
		.dma_mask		= &spi_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data = &s5p6440_spi1_pdata,
	},
};

void __init s5p6440_spi_set_info(int cntrlr, int src_clk_nr, int num_cs)
{
	struct s3c64xx_spi_info *pd;

	/* Reject invalid configuration */
	if (!num_cs || src_clk_nr < 0
			|| src_clk_nr > S5P6440_SPI_SRCCLK_SCLK) {
		printk(KERN_ERR "%s: Invalid SPI configuration\n", __func__);
		return;
	}

	switch (cntrlr) {
	case 0:
		pd = &s5p6440_spi0_pdata;
		break;
	case 1:
		pd = &s5p6440_spi1_pdata;
		break;
	default:
		printk(KERN_ERR "%s: Invalid SPI controller(%d)\n",
							__func__, cntrlr);
		return;
	}

	pd->num_cs = num_cs;
	pd->src_clk_nr = src_clk_nr;
	pd->src_clk_name = spi_src_clks[src_clk_nr];
}
