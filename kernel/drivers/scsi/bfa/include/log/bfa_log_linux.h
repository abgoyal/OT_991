

/* messages define for LINUX Module */
#ifndef	__BFA_LOG_LINUX_H__
#define	__BFA_LOG_LINUX_H__
#include  <cs/bfa_log.h>
#define BFA_LOG_LINUX_DEVICE_CLAIMED \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 1)
#define BFA_LOG_LINUX_HASH_INIT_FAILED \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 2)
#define BFA_LOG_LINUX_SYSFS_FAILED \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 3)
#define BFA_LOG_LINUX_MEM_ALLOC_FAILED \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 4)
#define BFA_LOG_LINUX_DRIVER_REGISTRATION_FAILED \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 5)
#define BFA_LOG_LINUX_ITNIM_FREE \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 6)
#define BFA_LOG_LINUX_ITNIM_ONLINE \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 7)
#define BFA_LOG_LINUX_ITNIM_OFFLINE \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 8)
#define BFA_LOG_LINUX_SCSI_HOST_FREE \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 9)
#define BFA_LOG_LINUX_SCSI_ABORT \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 10)
#define BFA_LOG_LINUX_SCSI_ABORT_COMP \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 11)
#define BFA_LOG_LINUX_DRIVER_CONFIG_ERROR \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 12)
#define BFA_LOG_LINUX_BNA_STATE_MACHINE \
		(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 13)
#define BFA_LOG_LINUX_IOC_ERROR \
	(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 14)
#define BFA_LOG_LINUX_RESOURCE_ALLOC_ERROR \
	(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 15)
#define BFA_LOG_LINUX_RING_BUFFER_ERROR \
	(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 16)
#define BFA_LOG_LINUX_DRIVER_ERROR \
	(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 17)
#define BFA_LOG_LINUX_DRIVER_DIAG \
	(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 18)
#define BFA_LOG_LINUX_DRIVER_AEN \
	(((u32) BFA_LOG_LINUX_ID << BFA_LOG_MODID_OFFSET) | 19)
#endif
