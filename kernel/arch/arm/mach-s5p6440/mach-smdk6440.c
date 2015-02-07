

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/clk.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <mach/hardware.h>
#include <mach/map.h>

#include <asm/irq.h>
#include <asm/mach-types.h>

#include <plat/regs-serial.h>

#include <plat/s5p6440.h>
#include <plat/clock.h>
#include <mach/regs-clock.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <plat/pll.h>
#include <plat/adc.h>
#include <plat/ts.h>

#define S5P6440_UCON_DEFAULT    (S3C2410_UCON_TXILEVEL |	\
				S3C2410_UCON_RXILEVEL |		\
				S3C2410_UCON_TXIRQMODE |	\
				S3C2410_UCON_RXIRQMODE |	\
				S3C2410_UCON_RXFIFO_TOI |	\
				S3C2443_UCON_RXERR_IRQEN)

#define S5P6440_ULCON_DEFAULT	S3C2410_LCON_CS8

#define S5P6440_UFCON_DEFAULT   (S3C2410_UFCON_FIFOMODE |	\
				S3C2440_UFCON_TXTRIG16 |	\
				S3C2410_UFCON_RXTRIG8)

static struct s3c2410_uartcfg smdk6440_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = S5P6440_UCON_DEFAULT,
		.ulcon	     = S5P6440_ULCON_DEFAULT,
		.ufcon	     = S5P6440_UFCON_DEFAULT,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = S5P6440_UCON_DEFAULT,
		.ulcon	     = S5P6440_ULCON_DEFAULT,
		.ufcon	     = S5P6440_UFCON_DEFAULT,
	},
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = S5P6440_UCON_DEFAULT,
		.ulcon	     = S5P6440_ULCON_DEFAULT,
		.ufcon	     = S5P6440_UFCON_DEFAULT,
	},
	[3] = {
		.hwport	     = 3,
		.flags	     = 0,
		.ucon	     = S5P6440_UCON_DEFAULT,
		.ulcon	     = S5P6440_ULCON_DEFAULT,
		.ufcon	     = S5P6440_UFCON_DEFAULT,
	},
};

static struct platform_device *smdk6440_devices[] __initdata = {
	&s5p6440_device_iis,
	&s3c_device_adc,
	&s3c_device_ts,
	&s3c_device_wdt,
};

static struct s3c2410_ts_mach_info s3c_ts_platform __initdata = {
	.delay			= 10000,
	.presc			= 49,
	.oversampling_shift	= 2,
};

static void __init smdk6440_map_io(void)
{
	s5p_init_io(NULL, 0, S5P_SYS_ID);
	s3c24xx_init_clocks(12000000);
	s3c24xx_init_uarts(smdk6440_uartcfgs, ARRAY_SIZE(smdk6440_uartcfgs));
}

static void __init smdk6440_machine_init(void)
{
	s3c24xx_ts_set_platdata(&s3c_ts_platform);

	platform_add_devices(smdk6440_devices, ARRAY_SIZE(smdk6440_devices));
}

MACHINE_START(SMDK6440, "SMDK6440")
	/* Maintainer: Kukjin Kim <kgene.kim@samsung.com> */
	.phys_io	= S3C_PA_UART & 0xfff00000,
	.io_pg_offst	= (((u32)S3C_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S5P_PA_SDRAM + 0x100,

	.init_irq	= s5p6440_init_irq,
	.map_io		= smdk6440_map_io,
	.init_machine	= smdk6440_machine_init,
	.timer		= &s3c24xx_timer,
MACHINE_END
