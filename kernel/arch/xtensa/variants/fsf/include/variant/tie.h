

#ifndef _XTENSA_CORE_TIE_H
#define _XTENSA_CORE_TIE_H

#define XCHAL_CP_NUM			0	/* number of coprocessors */
#define XCHAL_CP_MAX			0	/* max CP ID + 1 (0 if none) */
#define XCHAL_CP_MASK			0x00	/* bitmask of all CPs by ID */
#define XCHAL_CP_PORT_MASK		0x00	/* bitmask of only port CPs */

/*  Basic parameters of each coprocessor:  */
#define XCHAL_CP7_NAME			"XTIOP"
#define XCHAL_CP7_IDENT			XTIOP
#define XCHAL_CP7_SA_SIZE		0	/* size of state save area */
#define XCHAL_CP7_SA_ALIGN		1	/* min alignment of save area */
#define XCHAL_CP_ID_XTIOP		7	/* coprocessor ID (0..7) */

/*  Filler info for unassigned coprocessors, to simplify arrays etc:  */
#define XCHAL_NCP_SA_SIZE		0
#define XCHAL_NCP_SA_ALIGN		1
#define XCHAL_CP0_SA_SIZE		0
#define XCHAL_CP0_SA_ALIGN		1
#define XCHAL_CP1_SA_SIZE		0
#define XCHAL_CP1_SA_ALIGN		1
#define XCHAL_CP2_SA_SIZE		0
#define XCHAL_CP2_SA_ALIGN		1
#define XCHAL_CP3_SA_SIZE		0
#define XCHAL_CP3_SA_ALIGN		1
#define XCHAL_CP4_SA_SIZE		0
#define XCHAL_CP4_SA_ALIGN		1
#define XCHAL_CP5_SA_SIZE		0
#define XCHAL_CP5_SA_ALIGN		1
#define XCHAL_CP6_SA_SIZE		0
#define XCHAL_CP6_SA_ALIGN		1

/*  Save area for non-coprocessor optional and custom (TIE) state:  */
#define XCHAL_NCP_SA_SIZE		0
#define XCHAL_NCP_SA_ALIGN		1

/*  Total save area for optional and custom state (NCP + CPn):  */
#define XCHAL_TOTAL_SA_SIZE		0	/* with 16-byte align padding */
#define XCHAL_TOTAL_SA_ALIGN		1	/* actual minimum alignment */

#define XCHAL_NCP_SA_NUM	0
#define XCHAL_NCP_SA_LIST(s)
#define XCHAL_CP0_SA_NUM	0
#define XCHAL_CP0_SA_LIST(s)
#define XCHAL_CP1_SA_NUM	0
#define XCHAL_CP1_SA_LIST(s)
#define XCHAL_CP2_SA_NUM	0
#define XCHAL_CP2_SA_LIST(s)
#define XCHAL_CP3_SA_NUM	0
#define XCHAL_CP3_SA_LIST(s)
#define XCHAL_CP4_SA_NUM	0
#define XCHAL_CP4_SA_LIST(s)
#define XCHAL_CP5_SA_NUM	0
#define XCHAL_CP5_SA_LIST(s)
#define XCHAL_CP6_SA_NUM	0
#define XCHAL_CP6_SA_LIST(s)
#define XCHAL_CP7_SA_NUM	0
#define XCHAL_CP7_SA_LIST(s)

/* Byte length of instruction from its first nibble (op0 field), per FLIX.  */
#define XCHAL_OP0_FORMAT_LENGTHS	3,3,3,3,3,3,3,3,2,2,2,2,2,2,3,3

#endif /*_XTENSA_CORE_TIE_H*/

