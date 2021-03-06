

#include <asm/asm.h>
#include <asm/regdef.h>

/* Maximum number of arguments supported.  Must be even!  */
#define O32_ARGC	32
/* Number of static registers we save.  */
#define O32_STATC	11
/* Frame size for static register  */
#define O32_FRAMESZ	(SZREG * O32_STATC)
/* Frame size on new stack */
#define O32_FRAMESZ_NEW (SZREG + 4 * O32_ARGC)

		.text

NESTED(call_o32, O32_FRAMESZ, ra)
		REG_SUBU	sp,O32_FRAMESZ

		REG_S		ra,O32_FRAMESZ-1*SZREG(sp)
		REG_S		fp,O32_FRAMESZ-2*SZREG(sp)
		REG_S		gp,O32_FRAMESZ-3*SZREG(sp)
		REG_S		s7,O32_FRAMESZ-4*SZREG(sp)
		REG_S		s6,O32_FRAMESZ-5*SZREG(sp)
		REG_S		s5,O32_FRAMESZ-6*SZREG(sp)
		REG_S		s4,O32_FRAMESZ-7*SZREG(sp)
		REG_S		s3,O32_FRAMESZ-8*SZREG(sp)
		REG_S		s2,O32_FRAMESZ-9*SZREG(sp)
		REG_S		s1,O32_FRAMESZ-10*SZREG(sp)
		REG_S		s0,O32_FRAMESZ-11*SZREG(sp)

		move		jp,a0
		REG_SUBU	s0,a1,O32_FRAMESZ_NEW
		REG_S		sp,O32_FRAMESZ_NEW-1*SZREG(s0)

		sll		a0,a2,zero
		sll		a1,a3,zero
		sll		a2,a4,zero
		sll		a3,a5,zero
		sw		a6,0x10(s0)
		sw		a7,0x14(s0)

		PTR_LA		t0,O32_FRAMESZ(sp)
		PTR_LA		t1,0x18(s0)
		li		t2,O32_ARGC-6
1:
		lw		t3,(t0)
		REG_ADDU	t0,SZREG
		sw		t3,(t1)
		REG_SUBU	t2,1
		REG_ADDU	t1,4
		bnez		t2,1b

		move		sp,s0

		jalr		jp

		REG_L		sp,O32_FRAMESZ_NEW-1*SZREG(sp)

		REG_L		s0,O32_FRAMESZ-11*SZREG(sp)
		REG_L		s1,O32_FRAMESZ-10*SZREG(sp)
		REG_L		s2,O32_FRAMESZ-9*SZREG(sp)
		REG_L		s3,O32_FRAMESZ-8*SZREG(sp)
		REG_L		s4,O32_FRAMESZ-7*SZREG(sp)
		REG_L		s5,O32_FRAMESZ-6*SZREG(sp)
		REG_L		s6,O32_FRAMESZ-5*SZREG(sp)
		REG_L		s7,O32_FRAMESZ-4*SZREG(sp)
		REG_L		gp,O32_FRAMESZ-3*SZREG(sp)
		REG_L		fp,O32_FRAMESZ-2*SZREG(sp)
		REG_L		ra,O32_FRAMESZ-1*SZREG(sp)

		REG_ADDU	sp,O32_FRAMESZ
		jr		ra
END(call_o32)
