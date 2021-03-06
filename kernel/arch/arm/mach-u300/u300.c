
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/memory.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

static void __init u300_init_machine(void)
{
	u300_init_devices();
}

#ifdef CONFIG_MACH_U300_BS2X
#define MACH_U300_STRING "Ericsson AB U300 S25/S26/B25/B26 Prototype Board"
#endif

#ifdef CONFIG_MACH_U300_BS330
#define MACH_U300_STRING "Ericsson AB U330 S330/B330 Prototype Board"
#endif

#ifdef CONFIG_MACH_U300_BS335
#define MACH_U300_STRING "Ericsson AB U335 S335/B335 Prototype Board"
#endif

#ifdef CONFIG_MACH_U300_BS365
#define MACH_U300_STRING "Ericsson AB U365 S365/B365 Prototype Board"
#endif

MACHINE_START(U300, MACH_U300_STRING)
	/* Maintainer: Linus Walleij <linus.walleij@stericsson.com> */
	.phys_io	= U300_AHB_PER_PHYS_BASE,
	.io_pg_offst	= ((U300_AHB_PER_VIRT_BASE) >> 18) & 0xfffc,
	.boot_params	= BOOT_PARAMS_OFFSET,
	.map_io		= u300_map_io,
	.init_irq	= u300_init_irq,
	.timer		= &u300_timer,
	.init_machine	= u300_init_machine,
MACHINE_END
