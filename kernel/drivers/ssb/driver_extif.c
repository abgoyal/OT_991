

#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>

#include "ssb_private.h"


static inline u32 extif_read32(struct ssb_extif *extif, u16 offset)
{
	return ssb_read32(extif->dev, offset);
}

static inline void extif_write32(struct ssb_extif *extif, u16 offset, u32 value)
{
	ssb_write32(extif->dev, offset, value);
}

static inline u32 extif_write32_masked(struct ssb_extif *extif, u16 offset,
				       u32 mask, u32 value)
{
	value &= mask;
	value |= extif_read32(extif, offset) & ~mask;
	extif_write32(extif, offset, value);

	return value;
}

#ifdef CONFIG_SSB_SERIAL
static bool serial_exists(u8 *regs)
{
	u8 save_mcr, msr = 0;

	if (regs) {
		save_mcr = regs[UART_MCR];
		regs[UART_MCR] = (UART_MCR_LOOP | UART_MCR_OUT2 | UART_MCR_RTS);
		msr = regs[UART_MSR] & (UART_MSR_DCD | UART_MSR_RI
					| UART_MSR_CTS | UART_MSR_DSR);
		regs[UART_MCR] = save_mcr;
	}
	return (msr == (UART_MSR_DCD | UART_MSR_CTS));
}

int ssb_extif_serial_init(struct ssb_extif *extif, struct ssb_serial_port *ports)
{
	u32 i, nr_ports = 0;

	/* Disable GPIO interrupt initially */
	extif_write32(extif, SSB_EXTIF_GPIO_INTPOL, 0);
	extif_write32(extif, SSB_EXTIF_GPIO_INTMASK, 0);

	for (i = 0; i < 2; i++) {
		void __iomem *uart_regs;

		uart_regs = ioremap_nocache(SSB_EUART, 16);
		if (uart_regs) {
			uart_regs += (i * 8);

			if (serial_exists(uart_regs) && ports) {
				extif_write32(extif, SSB_EXTIF_GPIO_INTMASK, 2);

				nr_ports++;
				ports[i].regs = uart_regs;
				ports[i].irq = 2;
				ports[i].baud_base = 13500000;
				ports[i].reg_shift = 0;
			}
			iounmap(uart_regs);
		}
	}
	return nr_ports;
}
#endif /* CONFIG_SSB_SERIAL */

void ssb_extif_timing_init(struct ssb_extif *extif, unsigned long ns)
{
	u32 tmp;

	/* Initialize extif so we can get to the LEDs and external UART */
	extif_write32(extif, SSB_EXTIF_PROG_CFG, SSB_EXTCFG_EN);

	/* Set timing for the flash */
	tmp  = DIV_ROUND_UP(10, ns) << SSB_PROG_WCNT_3_SHIFT;
	tmp |= DIV_ROUND_UP(40, ns) << SSB_PROG_WCNT_1_SHIFT;
	tmp |= DIV_ROUND_UP(120, ns);
	extif_write32(extif, SSB_EXTIF_PROG_WAITCNT, tmp);

	/* Set programmable interface timing for external uart */
	tmp  = DIV_ROUND_UP(10, ns) << SSB_PROG_WCNT_3_SHIFT;
	tmp |= DIV_ROUND_UP(20, ns) << SSB_PROG_WCNT_2_SHIFT;
	tmp |= DIV_ROUND_UP(100, ns) << SSB_PROG_WCNT_1_SHIFT;
	tmp |= DIV_ROUND_UP(120, ns);
	extif_write32(extif, SSB_EXTIF_PROG_WAITCNT, tmp);
}

void ssb_extif_get_clockcontrol(struct ssb_extif *extif,
				u32 *pll_type, u32 *n, u32 *m)
{
	*pll_type = SSB_PLLTYPE_1;
	*n = extif_read32(extif, SSB_EXTIF_CLOCK_N);
	*m = extif_read32(extif, SSB_EXTIF_CLOCK_SB);
}

void ssb_extif_watchdog_timer_set(struct ssb_extif *extif,
				  u32 ticks)
{
	extif_write32(extif, SSB_EXTIF_WATCHDOG, ticks);
}

u32 ssb_extif_gpio_in(struct ssb_extif *extif, u32 mask)
{
	return extif_read32(extif, SSB_EXTIF_GPIO_IN) & mask;
}

u32 ssb_extif_gpio_out(struct ssb_extif *extif, u32 mask, u32 value)
{
	return extif_write32_masked(extif, SSB_EXTIF_GPIO_OUT(0),
				   mask, value);
}

u32 ssb_extif_gpio_outen(struct ssb_extif *extif, u32 mask, u32 value)
{
	return extif_write32_masked(extif, SSB_EXTIF_GPIO_OUTEN(0),
				   mask, value);
}

u32 ssb_extif_gpio_polarity(struct ssb_extif *extif, u32 mask, u32 value)
{
	return extif_write32_masked(extif, SSB_EXTIF_GPIO_INTPOL, mask, value);
}

u32 ssb_extif_gpio_intmask(struct ssb_extif *extif, u32 mask, u32 value)
{
	return extif_write32_masked(extif, SSB_EXTIF_GPIO_INTMASK, mask, value);
}
