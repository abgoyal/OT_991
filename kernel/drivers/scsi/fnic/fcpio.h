
#ifndef _FCPIO_H_
#define _FCPIO_H_

#include <linux/if_ether.h>


#define FCPIO_HOST_EXCH_RANGE_START         0x1000
#define FCPIO_HOST_EXCH_RANGE_END           0x1fff
#define FCPIO_HOST_SEQ_ID_RANGE_START       0x80
#define FCPIO_HOST_SEQ_ID_RANGE_END         0xff

enum fcpio_type {
	/*
	 * Initiator request types
	 */
	FCPIO_ICMND_16 = 0x1,
	FCPIO_ICMND_32,
	FCPIO_ICMND_CMPL,
	FCPIO_ITMF,
	FCPIO_ITMF_CMPL,

	/*
	 * Target request types
	 */
	FCPIO_TCMND_16 = 0x11,
	FCPIO_TCMND_32,
	FCPIO_TDATA,
	FCPIO_TXRDY,
	FCPIO_TRSP,
	FCPIO_TDRSP_CMPL,
	FCPIO_TTMF,
	FCPIO_TTMF_ACK,
	FCPIO_TABORT,
	FCPIO_TABORT_CMPL,

	/*
	 * Misc request types
	 */
	FCPIO_ACK = 0x20,
	FCPIO_RESET,
	FCPIO_RESET_CMPL,
	FCPIO_FLOGI_REG,
	FCPIO_FLOGI_REG_CMPL,
	FCPIO_ECHO,
	FCPIO_ECHO_CMPL,
	FCPIO_LUNMAP_CHNG,
	FCPIO_LUNMAP_REQ,
	FCPIO_LUNMAP_REQ_CMPL,
	FCPIO_FLOGI_FIP_REG,
	FCPIO_FLOGI_FIP_REG_CMPL,
};

enum fcpio_status {
	FCPIO_SUCCESS = 0,              /* request was successful */

	/*
	 * If a request to the firmware is rejected, the original request
	 * header will be returned with the status set to one of the following:
	 */
	FCPIO_INVALID_HEADER,    /* header contains invalid data */
	FCPIO_OUT_OF_RESOURCE,   /* out of resources to complete request */
	FCPIO_INVALID_PARAM,     /* some parameter in request is invalid */
	FCPIO_REQ_NOT_SUPPORTED, /* request type is not supported */
	FCPIO_IO_NOT_FOUND,      /* requested I/O was not found */

	/*
	 * Once a request is processed, the firmware will usually return
	 * a cmpl message type.  In cases where errors occurred,
	 * the header status field would be filled in with one of the following:
	 */
	FCPIO_ABORTED = 0x41,     /* request was aborted */
	FCPIO_TIMEOUT,            /* request was timed out */
	FCPIO_SGL_INVALID,        /* request was aborted due to sgl error */
	FCPIO_MSS_INVALID,        /* request was aborted due to mss error */
	FCPIO_DATA_CNT_MISMATCH,  /* recv/sent more/less data than exp. */
	FCPIO_FW_ERR,             /* request was terminated due to fw error */
	FCPIO_ITMF_REJECTED,      /* itmf req was rejected by remote node */
	FCPIO_ITMF_FAILED,        /* itmf req was failed by remote node */
	FCPIO_ITMF_INCORRECT_LUN, /* itmf req targeted incorrect LUN */
	FCPIO_CMND_REJECTED,      /* request was invalid and rejected */
	FCPIO_NO_PATH_AVAIL,      /* no paths to the lun was available */
	FCPIO_PATH_FAILED,        /* i/o sent to current path failed */
	FCPIO_LUNMAP_CHNG_PEND,   /* i/o rejected due to lunmap change */
};

struct fcpio_tag {
	union {
		u32 req_id;
		struct {
			u16 rx_id;
			u16 ox_id;
		} ex_id;
	} u;
};

static inline void
fcpio_tag_id_enc(struct fcpio_tag *tag, u32 id)
{
	tag->u.req_id = id;
}

static inline void
fcpio_tag_id_dec(struct fcpio_tag *tag, u32 *id)
{
	*id = tag->u.req_id;
}

static inline void
fcpio_tag_exid_enc(struct fcpio_tag *tag, u16 ox_id, u16 rx_id)
{
	tag->u.ex_id.rx_id = rx_id;
	tag->u.ex_id.ox_id = ox_id;
}

static inline void
fcpio_tag_exid_dec(struct fcpio_tag *tag, u16 *ox_id, u16 *rx_id)
{
	*rx_id = tag->u.ex_id.rx_id;
	*ox_id = tag->u.ex_id.ox_id;
}

struct fcpio_header {
	u8            type;           /* enum fcpio_type */
	u8            status;         /* header status entry */
	u16           _resvd;         /* reserved */
	struct fcpio_tag    tag;      /* header tag */
};

static inline void
fcpio_header_enc(struct fcpio_header *hdr,
		 u8 type, u8 status,
		 struct fcpio_tag tag)
{
	hdr->type = type;
	hdr->status = status;
	hdr->_resvd = 0;
	hdr->tag = tag;
}

static inline void
fcpio_header_dec(struct fcpio_header *hdr,
		 u8 *type, u8 *status,
		 struct fcpio_tag *tag)
{
	*type = hdr->type;
	*status = hdr->status;
	*tag = hdr->tag;
}

#define CDB_16      16
#define CDB_32      32
#define LUN_ADDRESS 8

struct fcpio_icmnd_16 {
	u32	  lunmap_id;		/* index into lunmap table */
	u8	  special_req_flags;	/* special exchange request flags */
	u8	  _resvd0[3];	        /* reserved */
	u32	  sgl_cnt;		/* scatter-gather list count */
	u32	  sense_len;		/* sense buffer length */
	u64	  sgl_addr;		/* scatter-gather list addr */
	u64	  sense_addr;		/* sense buffer address */
	u8	  crn;			/* SCSI Command Reference No. */
	u8	  pri_ta;		/* SCSI Priority and Task attribute */
	u8	  _resvd1;		/* reserved: should be 0 */
	u8	  flags;		/* command flags */
	u8	  scsi_cdb[CDB_16];	/* SCSI Cmnd Descriptor Block */
	u32	  data_len;		/* length of data expected */
	u8	  lun[LUN_ADDRESS];	/* FC vNIC only: LUN address */
	u8	  _resvd2;		/* reserved */
	u8	  d_id[3];		/* FC vNIC only: Target D_ID */
	u16	  mss;			/* FC vNIC only: max burst */
	u16	  _resvd3;		/* reserved */
	u32	  r_a_tov;		/* FC vNIC only: Res. Alloc Timeout */
	u32	  e_d_tov;	        /* FC vNIC only: Err Detect Timeout */
};

#define FCPIO_ICMND_SRFLAG_RETRY 0x01   /* Enable Retry handling on exchange */

#define FCPIO_ICMND_PTA_SIMPLE      0   /* simple task attribute */
#define FCPIO_ICMND_PTA_HEADQ       1   /* head of queue task attribute */
#define FCPIO_ICMND_PTA_ORDERED     2   /* ordered task attribute */
#define FCPIO_ICMND_PTA_ACA         4   /* auto contingent allegiance */
#define FCPIO_ICMND_PRI_SHIFT       3   /* priority field starts in bit 3 */

#define FCPIO_ICMND_RDDATA      0x02    /* read data */
#define FCPIO_ICMND_WRDATA      0x01    /* write data */

struct fcpio_icmnd_32 {
	u32   lunmap_id;              /* index into lunmap table */
	u8    special_req_flags;      /* special exchange request flags */
	u8    _resvd0[3];             /* reserved */
	u32   sgl_cnt;                /* scatter-gather list count */
	u32   sense_len;              /* sense buffer length */
	u64   sgl_addr;               /* scatter-gather list addr */
	u64   sense_addr;             /* sense buffer address */
	u8    crn;                    /* SCSI Command Reference No. */
	u8    pri_ta;                 /* SCSI Priority and Task attribute */
	u8    _resvd1;                /* reserved: should be 0 */
	u8    flags;                  /* command flags */
	u8    scsi_cdb[CDB_32];       /* SCSI Cmnd Descriptor Block */
	u32   data_len;               /* length of data expected */
	u8    lun[LUN_ADDRESS];       /* FC vNIC only: LUN address */
	u8    _resvd2;                /* reserved */
	u8    d_id[3];		      /* FC vNIC only: Target D_ID */
	u16   mss;                    /* FC vNIC only: max burst */
	u16   _resvd3;                /* reserved */
	u32   r_a_tov;                /* FC vNIC only: Res. Alloc Timeout */
	u32   e_d_tov;                /* FC vNIC only: Error Detect Timeout */
};

struct fcpio_itmf {
	u32   lunmap_id;              /* index into lunmap table */
	u32   tm_req;                 /* SCSI Task Management request */
	u32   t_tag;                  /* header tag of fcpio to be aborted */
	u32   _resvd;                 /* _reserved */
	u8    lun[LUN_ADDRESS];       /* FC vNIC only: LUN address */
	u8    _resvd1;                /* reserved */
	u8    d_id[3];		      /* FC vNIC only: Target D_ID */
	u32   r_a_tov;                /* FC vNIC only: R_A_TOV in msec */
	u32   e_d_tov;                /* FC vNIC only: E_D_TOV in msec */
};

enum fcpio_itmf_tm_req_type {
	FCPIO_ITMF_ABT_TASK_TERM = 0x01,    /* abort task and terminate */
	FCPIO_ITMF_ABT_TASK,                /* abort task and issue abts */
	FCPIO_ITMF_ABT_TASK_SET,            /* abort task set */
	FCPIO_ITMF_CLR_TASK_SET,            /* clear task set */
	FCPIO_ITMF_LUN_RESET,               /* logical unit reset task mgmt */
	FCPIO_ITMF_CLR_ACA,                 /* Clear ACA condition */
};

struct fcpio_tdata {
	u16   rx_id;                  /* FC rx_id of target command */
	u16   flags;                  /* command flags */
	u32   rel_offset;             /* data sequence relative offset */
	u32   sgl_cnt;                /* scatter-gather list count */
	u32   data_len;               /* length of data expected to send */
	u64   sgl_addr;               /* scatter-gather list address */
};

#define FCPIO_TDATA_SCSI_RSP    0x01    /* send a scsi resp. after last frame */

struct fcpio_txrdy {
	u16   rx_id;                  /* FC rx_id of target command */
	u16   _resvd0;                /* reserved */
	u32   rel_offset;             /* data sequence relative offset */
	u32   sgl_cnt;                /* scatter-gather list count */
	u32   data_len;               /* length of data expected to send */
	u64   sgl_addr;               /* scatter-gather list address */
};

struct fcpio_trsp {
	u16   rx_id;                  /* FC rx_id of target command */
	u16   _resvd0;                /* reserved */
	u32   sense_len;              /* sense data buffer length */
	u64   sense_addr;             /* sense data buffer address */
	u16   _resvd1;                /* reserved */
	u8    flags;                  /* response request flags */
	u8    scsi_status;            /* SCSI status */
	u32   residual;               /* SCSI data residual value of I/O */
};

#define FCPIO_TRSP_RESID_UNDER  0x08   /* residual is valid and is underflow */
#define FCPIO_TRSP_RESID_OVER   0x04   /* residual is valid and is overflow */

struct fcpio_ttmf_ack {
	u16   rx_id;                  /* FC rx_id of target command */
	u16   _resvd0;                /* reserved */
	u32   tmf_status;             /* SCSI task management status */
};

struct fcpio_tabort {
	u16   rx_id;                  /* rx_id of the target request */
};

struct fcpio_reset {
	u32   _resvd;
};

enum fcpio_flogi_reg_format_type {
	FCPIO_FLOGI_REG_DEF_DEST = 0,    /* Use the oui | s_id mac format */
	FCPIO_FLOGI_REG_GW_DEST,         /* Use the fixed gateway mac */
};

struct fcpio_flogi_reg {
	u8 format;
	u8 s_id[3];			/* FC vNIC only: Source S_ID */
	u8 gateway_mac[ETH_ALEN];	/* Destination gateway mac */
	u16 _resvd;
	u32 r_a_tov;			/* R_A_TOV in msec */
	u32 e_d_tov;			/* E_D_TOV in msec */
};

struct fcpio_echo {
	u32 _resvd;
};

struct fcpio_lunmap_req {
	u64 addr;                     /* address of the buffer */
	u32 len;                      /* len of the buffer */
};

struct fcpio_flogi_fip_reg {
	u8    _resvd0;
	u8     s_id[3];               /* FC vNIC only: Source S_ID */
	u8     fcf_mac[ETH_ALEN];     /* FCF Target destination mac */
	u16   _resvd1;
	u32   r_a_tov;                /* R_A_TOV in msec */
	u32   e_d_tov;                /* E_D_TOV in msec */
	u8    ha_mac[ETH_ALEN];       /* Host adapter source mac */
	u16   _resvd2;
};

#define FCPIO_HOST_REQ_LEN      128     /* expected length of host requests */

struct fcpio_host_req {
	struct fcpio_header hdr;

	union {
		/*
		 * Defines space needed for request
		 */
		u8 buf[FCPIO_HOST_REQ_LEN - sizeof(struct fcpio_header)];

		/*
		 * Initiator host requests
		 */
		struct fcpio_icmnd_16               icmnd_16;
		struct fcpio_icmnd_32               icmnd_32;
		struct fcpio_itmf                   itmf;

		/*
		 * Target host requests
		 */
		struct fcpio_tdata                  tdata;
		struct fcpio_txrdy                  txrdy;
		struct fcpio_trsp                   trsp;
		struct fcpio_ttmf_ack               ttmf_ack;
		struct fcpio_tabort                 tabort;

		/*
		 * Misc requests
		 */
		struct fcpio_reset                  reset;
		struct fcpio_flogi_reg              flogi_reg;
		struct fcpio_echo                   echo;
		struct fcpio_lunmap_req             lunmap_req;
		struct fcpio_flogi_fip_reg          flogi_fip_reg;
	} u;
};

struct fcpio_icmnd_cmpl {
	u8    _resvd0[6];             /* reserved */
	u8    flags;                  /* response flags */
	u8    scsi_status;            /* SCSI status */
	u32   residual;               /* SCSI data residual length */
	u32   sense_len;              /* SCSI sense length */
};

#define FCPIO_ICMND_CMPL_RESID_UNDER    0x08    /* resid under and valid */
#define FCPIO_ICMND_CMPL_RESID_OVER     0x04    /* resid over and valid */

struct fcpio_itmf_cmpl {
	u32    _resvd;                /* reserved */
};

struct fcpio_tcmnd_16 {
	u8    lun[LUN_ADDRESS];       /* FC vNIC only: LUN address */
	u8    crn;                    /* SCSI Command Reference No. */
	u8    pri_ta;                 /* SCSI Priority and Task attribute */
	u8    _resvd2;                /* reserved: should be 0 */
	u8    flags;                  /* command flags */
	u8    scsi_cdb[CDB_16];       /* SCSI Cmnd Descriptor Block */
	u32   data_len;               /* length of data expected */
	u8    _resvd1;                /* reserved */
	u8    s_id[3];		      /* FC vNIC only: Source S_ID */
};

#define FCPIO_TCMND_PTA_SIMPLE      0   /* simple task attribute */
#define FCPIO_TCMND_PTA_HEADQ       1   /* head of queue task attribute */
#define FCPIO_TCMND_PTA_ORDERED     2   /* ordered task attribute */
#define FCPIO_TCMND_PTA_ACA         4   /* auto contingent allegiance */
#define FCPIO_TCMND_PRI_SHIFT       3   /* priority field starts in bit 3 */

#define FCPIO_TCMND_RDDATA      0x02    /* read data */
#define FCPIO_TCMND_WRDATA      0x01    /* write data */

struct fcpio_tcmnd_32 {
	u8    lun[LUN_ADDRESS];       /* FC vNIC only: LUN address */
	u8    crn;                    /* SCSI Command Reference No. */
	u8    pri_ta;                 /* SCSI Priority and Task attribute */
	u8    _resvd2;                /* reserved: should be 0 */
	u8    flags;                  /* command flags */
	u8    scsi_cdb[CDB_32];       /* SCSI Cmnd Descriptor Block */
	u32   data_len;               /* length of data expected */
	u8    _resvd0;                /* reserved */
	u8    s_id[3];		      /* FC vNIC only: Source S_ID */
};

struct fcpio_tdrsp_cmpl {
	u16   rx_id;                  /* rx_id of the target request */
	u16   _resvd0;                /* reserved */
};

struct fcpio_ttmf {
	u8    _resvd0;                /* reserved */
	u8    s_id[3];		      /* FC vNIC only: Source S_ID */
	u8    lun[LUN_ADDRESS];       /* FC vNIC only: LUN address */
	u8    crn;                    /* SCSI Command Reference No. */
	u8    _resvd2[3];             /* reserved */
	u32   tmf_type;               /* task management request type */
};

#define FCPIO_TTMF_CLR_ACA      0x40    /* Clear ACA condition */
#define FCPIO_TTMF_LUN_RESET    0x10    /* logical unit reset task mgmt */
#define FCPIO_TTMF_CLR_TASK_SET 0x04    /* clear task set */
#define FCPIO_TTMF_ABT_TASK_SET 0x02    /* abort task set */
#define FCPIO_TTMF_ABT_TASK     0x01    /* abort task */

struct fcpio_tabort_cmpl {
	u16   rx_id;                  /* rx_id of the target request */
	u16   _resvd0;                /* reserved */
};

struct fcpio_ack {
	u16  request_out;             /* last host entry received */
	u16  _resvd;
};

struct fcpio_reset_cmpl {
	u16   vnic_id;
};

struct fcpio_flogi_reg_cmpl {
	u32 _resvd;
};

struct fcpio_echo_cmpl {
	u32 _resvd;
};

struct fcpio_lunmap_chng {
	u32 _resvd;
};

struct fcpio_lunmap_req_cmpl {
	u32 _resvd;
};

#define FCPIO_FW_REQ_LEN        64      /* expected length of fw requests */
struct fcpio_fw_req {
	struct fcpio_header hdr;

	union {
		/*
		 * Defines space needed for request
		 */
		u8 buf[FCPIO_FW_REQ_LEN - sizeof(struct fcpio_header)];

		/*
		 * Initiator firmware responses
		 */
		struct fcpio_icmnd_cmpl         icmnd_cmpl;
		struct fcpio_itmf_cmpl          itmf_cmpl;

		/*
		 * Target firmware new requests
		 */
		struct fcpio_tcmnd_16           tcmnd_16;
		struct fcpio_tcmnd_32           tcmnd_32;

		/*
		 * Target firmware responses
		 */
		struct fcpio_tdrsp_cmpl         tdrsp_cmpl;
		struct fcpio_ttmf               ttmf;
		struct fcpio_tabort_cmpl        tabort_cmpl;

		/*
		 * Firmware response to work received
		 */
		struct fcpio_ack                ack;

		/*
		 * Misc requests
		 */
		struct fcpio_reset_cmpl         reset_cmpl;
		struct fcpio_flogi_reg_cmpl     flogi_reg_cmpl;
		struct fcpio_echo_cmpl          echo_cmpl;
		struct fcpio_lunmap_chng        lunmap_chng;
		struct fcpio_lunmap_req_cmpl    lunmap_req_cmpl;
	} u;
};

static inline void fcpio_color_enc(struct fcpio_fw_req *fw_req, u8 color)
{
	u8 *c = ((u8 *) fw_req) + sizeof(struct fcpio_fw_req) - 1;

	if (color)
		*c |= 0x80;
	else
		*c &= ~0x80;
}

static inline void fcpio_color_dec(struct fcpio_fw_req *fw_req, u8 *color)
{
	u8 *c = ((u8 *) fw_req) + sizeof(struct fcpio_fw_req) - 1;

	*color = *c >> 7;

	/*
	 * Make sure color bit is read from desc *before* other fields
	 * are read from desc.  Hardware guarantees color bit is last
	 * bit (byte) written.  Adding the rmb() prevents the compiler
	 * and/or CPU from reordering the reads which would potentially
	 * result in reading stale values.
	 */

	rmb();

}

#define FCPIO_LUNMAP_TABLE_SIZE     256
#define FCPIO_FLAGS_LUNMAP_VALID    0x80
#define FCPIO_FLAGS_BOOT            0x01
struct fcpio_lunmap_entry {
	u8    bus;
	u8    target;
	u8    lun;
	u8    path_cnt;
	u16   flags;
	u16   update_cnt;
};

struct fcpio_lunmap_tbl {
	u32                   update_cnt;
	struct fcpio_lunmap_entry   lunmaps[FCPIO_LUNMAP_TABLE_SIZE];
};

#endif /* _FCPIO_H_ */
