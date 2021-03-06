

#include <mach/regs-icu.h>

	.macro	disable_fiq
	.endm

	.macro	arch_ret_to_user, tmp1, tmp2
	.endm

	.macro	get_irqnr_preamble, base, tmp
	mrc	p15, 0, \tmp, c0, c0, 0		@ CPUID
	and	\tmp, \tmp, #0xff00
	cmp	\tmp, #0x5800
	ldr	\base, =ICU_VIRT_BASE
	addne	\base, \base, #0x10c		@ PJ1 AP INT SEL register
	addeq	\base, \base, #0x104		@ PJ4 IRQ SEL register
	.endm

	.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\tmp, [\base, #0]
	and	\irqnr, \tmp, #0x3f
	tst	\tmp, #(1 << 6)
	.endm
