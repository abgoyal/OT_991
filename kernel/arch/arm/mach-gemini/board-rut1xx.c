
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#include "common.h"

static struct gpio_keys_button rut1xx_keys[] = {
	{
		.code		= KEY_SETUP,
		.gpio		= 60,
		.active_low	= 1,
		.desc		= "Reset to defaults",
		.type		= EV_KEY,
	},
};

static struct gpio_keys_platform_data rut1xx_keys_data = {
	.buttons	= rut1xx_keys,
	.nbuttons	= ARRAY_SIZE(rut1xx_keys),
};

static struct platform_device rut1xx_keys_device = {
	.name	= "gpio-keys",
	.id	= -1,
	.dev	= {
		.platform_data = &rut1xx_keys_data,
	},
};

static struct gpio_led rut100_leds[] = {
	{
		.name			= "Power",
		.default_trigger	= "heartbeat",
		.gpio			= 17,
	},
	{
		.name			= "GSM",
		.default_trigger	= "default-on",
		.gpio			= 7,
		.active_low		= 1,
	},
};

static struct gpio_led_platform_data rut100_leds_data = {
	.num_leds	= ARRAY_SIZE(rut100_leds),
	.leds		= rut100_leds,
};

static struct platform_device rut1xx_leds = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data = &rut100_leds_data,
	},
};

static struct sys_timer rut1xx_timer = {
	.init	= gemini_timer_init,
};

static void __init rut1xx_init(void)
{
	gemini_gpio_init();
	platform_register_uart();
	platform_register_pflash(SZ_8M, NULL, 0);
	platform_device_register(&rut1xx_leds);
	platform_device_register(&rut1xx_keys_device);
}

MACHINE_START(RUT100, "Teltonika RUT100")
	.phys_io	= 0x7fffc000,
	.io_pg_offst	= ((0xffffc000) >> 18) & 0xfffc,
	.boot_params	= 0x100,
	.map_io		= gemini_map_io,
	.init_irq	= gemini_init_irq,
	.timer		= &rut1xx_timer,
	.init_machine	= rut1xx_init,
MACHINE_END