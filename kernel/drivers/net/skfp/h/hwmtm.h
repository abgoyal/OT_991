

#ifndef	_HWM_
#define	_HWM_

#include "h/mbuf.h"

#ifndef DRV_BUF_FLUSH
#define DRV_BUF_FLUSH(desc,flag)
#define DDI_DMA_SYNC_FORCPU
#define DDI_DMA_SYNC_FORDEV
#endif

	/*
	 * hardware modul dependent receive modes
	 */
#define	RX_ENABLE_PASS_SMT	21
#define	RX_DISABLE_PASS_SMT	22
#define	RX_ENABLE_PASS_NSA	23
#define	RX_DISABLE_PASS_NSA	24
#define	RX_ENABLE_PASS_DB	25
#define	RX_DISABLE_PASS_DB	26
#define	RX_DISABLE_PASS_ALL	27
#define	RX_DISABLE_LLC_PROMISC	28
#define	RX_ENABLE_LLC_PROMISC	29


#ifndef	DMA_RD
#define DMA_RD		1	/* memory -> hw */
#endif
#ifndef DMA_WR
#define DMA_WR		2	/* hw -> memory */
#endif
#define SMT_BUF		0x80

	/*
	 * bits of the frame status byte
	 */
#define EN_IRQ_EOF	0x02	/* get IRQ after end of frame transmission */
#define	LOC_TX		0x04	/* send frame to the local SMT */
#define LAST_FRAG	0x08	/* last TxD of the frame */
#define	FIRST_FRAG	0x10	/* first TxD of the frame */
#define	LAN_TX		0x20	/* send frame to network if set */
#define RING_DOWN	0x40	/* error: unable to send, ring down */
#define OUT_OF_TXD	0x80	/* error: not enough TxDs available */


#ifndef NULL
#define NULL 		0
#endif

#ifdef	LITTLE_ENDIAN
#define HWM_REVERSE(x)	(x)
#else
#define	HWM_REVERSE(x)		((((x)<<24L)&0xff000000L)	+	\
				 (((x)<< 8L)&0x00ff0000L)	+	\
				 (((x)>> 8L)&0x0000ff00L)	+	\
				 (((x)>>24L)&0x000000ffL))
#endif

#define C_INDIC		(1L<<25)
#define A_INDIC		(1L<<26)
#define	RD_FS_LOCAL	0x80

	/*
	 * DEBUG FLAGS
	 */
#define	DEBUG_SMTF	1
#define	DEBUG_SMT	2
#define	DEBUG_ECM	3
#define	DEBUG_RMT	4
#define	DEBUG_CFM	5
#define	DEBUG_PCM	6
#define	DEBUG_SBA	7
#define	DEBUG_ESS	8

#define	DB_HWM_RX	10
#define	DB_HWM_TX	11
#define DB_HWM_GEN	12

struct s_mbuf_pool {
#ifndef	MB_OUTSIDE_SMC
	SMbuf		mb[MAX_MBUF] ;		/* mbuf pool */
#endif
	SMbuf		*mb_start ;		/* points to the first mb */
	SMbuf		*mb_free ;		/* free queue */
} ;

struct hwm_r {
	/*
	 * hardware modul specific receive variables
	 */
	u_int			len ;		/* length of the whole frame */
	char			*mb_pos ;	/* SMbuf receive position */
} ;

struct hw_modul {
	/*
	 * All hardware modul specific variables
	 */
	struct	s_mbuf_pool	mbuf_pool ;
	struct	hwm_r	r ;

	union s_fp_descr volatile *descr_p ; /* points to the desriptor area */

	u_short pass_SMT ;		/* pass SMT frames */
	u_short pass_NSA ;		/* pass all NSA frames */
	u_short pass_DB ;		/* pass Direct Beacon Frames */
	u_short pass_llc_promisc ;	/* pass all llc frames (default ON) */

	SMbuf	*llc_rx_pipe ;		/* points to the first queued llc fr */
	SMbuf	*llc_rx_tail ;		/* points to the last queued llc fr */
	int	queued_rx_frames ;	/* number of queued frames */

	SMbuf	*txd_tx_pipe ;		/* points to first mb in the txd ring */
	SMbuf	*txd_tx_tail ;		/* points to last mb in the txd ring */
	int	queued_txd_mb ;		/* number of SMT MBufs in txd ring */

	int	rx_break ;		/* rev. was breaked because ind. off */
	int	leave_isr ;		/* leave fddi_isr immedeately if set */
	int	isr_flag ;		/* set, when HWM is entered from isr */
	/*
	 * variables for the current transmit frame
	 */
	struct s_smt_tx_queue *tx_p ;	/* pointer to the transmit queue */
	u_long	tx_descr ;		/* tx descriptor for FORMAC+ */
	int	tx_len ;		/* tx frame length */
	SMbuf	*tx_mb ;		/* SMT tx MBuf pointer */
	char	*tx_data ;		/* data pointer to the SMT tx Mbuf */

	int	detec_count ;		/* counter for out of RxD condition */
	u_long	rx_len_error ;		/* rx len FORMAC != sum of fragments */
} ;



#ifdef	DEBUG
struct os_debug {
	int	hwm_rx ;
	int	hwm_tx ;
	int	hwm_gen ;
} ;
#endif

#ifdef	DEBUG
#ifdef	DEBUG_BRD
#define	DB_P	smc->debug
#else
#define DB_P	debug
#endif

#define DB_RX(a,b,c,lev) if (DB_P.d_os.hwm_rx >= (lev))	printf(a,b,c)
#define DB_TX(a,b,c,lev) if (DB_P.d_os.hwm_tx >= (lev))	printf(a,b,c)
#define DB_GEN(a,b,c,lev) if (DB_P.d_os.hwm_gen >= (lev)) printf(a,b,c)
#else	/* DEBUG */
#define DB_RX(a,b,c,lev)
#define DB_TX(a,b,c,lev)
#define DB_GEN(a,b,c,lev)
#endif	/* DEBUG */

#ifndef	SK_BREAK
#define	SK_BREAK()
#endif



#define	HWM_GET_TX_PHYS(txd)		(u_long)AIX_REVERSE((txd)->txd_tbadr)

#define	HWM_GET_TX_LEN(txd)	((int)AIX_REVERSE((txd)->txd_tbctrl)& RD_LENGTH)

#define	HWM_GET_TX_USED(smc,queue)	(int) (smc)->hw.fp.tx_q[queue].tx_used

#define	HWM_GET_CURR_TXD(smc,queue)	(struct s_smt_fp_txd volatile *)\
					(smc)->hw.fp.tx_q[queue].tx_curr_put

#define	HWM_GET_RX_FRAG_LEN(rxd)	((int)AIX_REVERSE((rxd)->rxd_rbctrl)& \
				RD_LENGTH)

#define	HWM_GET_RX_PHYS(rxd)	(u_long)AIX_REVERSE((rxd)->rxd_rbadr)

#define	HWM_GET_RX_USED(smc)	((int)(smc)->hw.fp.rx_q[QUEUE_R1].rx_used)

#define	HWM_GET_RX_FREE(smc)	((int)(smc)->hw.fp.rx_q[QUEUE_R1].rx_free-1)

#define	HWM_GET_CURR_RXD(smc)	(struct s_smt_fp_rxd volatile *)\
				(smc)->hw.fp.rx_q[QUEUE_R1].rx_curr_put

#ifndef HWM_NO_FLOW_CTL
#define	HWM_RX_CHECK(smc,low_water) {\
	if ((low_water) >= (smc)->hw.fp.rx_q[QUEUE_R1].rx_used) {\
		mac_drv_fill_rxd(smc) ;\
	}\
}
#else
#define	HWM_RX_CHECK(smc,low_water)		mac_drv_fill_rxd(smc)
#endif

#ifndef	HWM_EBASE
#define	HWM_EBASE	500
#endif

#define	HWM_E0001	HWM_EBASE + 1
#define	HWM_E0001_MSG	"HWM: Wrong size of s_rxd_os struct"
#define	HWM_E0002	HWM_EBASE + 2
#define	HWM_E0002_MSG	"HWM: Wrong size of s_txd_os struct"
#define	HWM_E0003	HWM_EBASE + 3
#define	HWM_E0003_MSG	"HWM: smt_free_mbuf() called with NULL pointer"
#define	HWM_E0004	HWM_EBASE + 4
#define	HWM_E0004_MSG	"HWM: Parity error rx queue 1"
#define	HWM_E0005	HWM_EBASE + 5
#define	HWM_E0005_MSG	"HWM: Encoding error rx queue 1"
#define	HWM_E0006	HWM_EBASE + 6
#define	HWM_E0006_MSG	"HWM: Encoding error async tx queue"
#define	HWM_E0007	HWM_EBASE + 7
#define	HWM_E0007_MSG	"HWM: Encoding error sync tx queue"
#define	HWM_E0008	HWM_EBASE + 8
#define	HWM_E0008_MSG	""
#define	HWM_E0009	HWM_EBASE + 9
#define	HWM_E0009_MSG	"HWM: Out of RxD condition detected"
#define	HWM_E0010	HWM_EBASE + 10
#define	HWM_E0010_MSG	"HWM: A protocol layer has tried to send a frame with an invalid frame control"
#define HWM_E0011	HWM_EBASE + 11
#define HWM_E0011_MSG	"HWM: mac_drv_clear_tx_queue was called although the hardware wasn't stopped"
#define HWM_E0012	HWM_EBASE + 12
#define HWM_E0012_MSG	"HWM: mac_drv_clear_rx_queue was called although the hardware wasn't stopped"
#define HWM_E0013	HWM_EBASE + 13
#define HWM_E0013_MSG	"HWM: mac_drv_repair_descr was called although the hardware wasn't stopped"

#endif
