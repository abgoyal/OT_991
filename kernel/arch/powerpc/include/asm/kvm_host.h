

#ifndef __POWERPC_KVM_HOST_H__
#define __POWERPC_KVM_HOST_H__

#include <linux/mutex.h>
#include <linux/hrtimer.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/kvm_types.h>
#include <asm/kvm_asm.h>

#define KVM_MAX_VCPUS 1
#define KVM_MEMORY_SLOTS 32
/* memory slots that does not exposed to userspace */
#define KVM_PRIVATE_MEM_SLOTS 4

#define KVM_COALESCED_MMIO_PAGE_OFFSET 1

/* We don't currently support large pages. */
#define KVM_NR_PAGE_SIZES	1
#define KVM_PAGES_PER_HPAGE(x)	(1UL<<31)

#define HPTEG_CACHE_NUM 1024

struct kvm;
struct kvm_run;
struct kvm_vcpu;

struct kvm_vm_stat {
	u32 remote_tlb_flush;
};

struct kvm_vcpu_stat {
	u32 sum_exits;
	u32 mmio_exits;
	u32 dcr_exits;
	u32 signal_exits;
	u32 light_exits;
	/* Account for special types of light exits: */
	u32 itlb_real_miss_exits;
	u32 itlb_virt_miss_exits;
	u32 dtlb_real_miss_exits;
	u32 dtlb_virt_miss_exits;
	u32 syscall_exits;
	u32 isi_exits;
	u32 dsi_exits;
	u32 emulated_inst_exits;
	u32 dec_exits;
	u32 ext_intr_exits;
	u32 halt_wakeup;
#ifdef CONFIG_PPC_BOOK3S
	u32 pf_storage;
	u32 pf_instruc;
	u32 sp_storage;
	u32 sp_instruc;
	u32 queue_intr;
	u32 ld;
	u32 ld_slow;
	u32 st;
	u32 st_slow;
#endif
};

enum kvm_exit_types {
	MMIO_EXITS,
	DCR_EXITS,
	SIGNAL_EXITS,
	ITLB_REAL_MISS_EXITS,
	ITLB_VIRT_MISS_EXITS,
	DTLB_REAL_MISS_EXITS,
	DTLB_VIRT_MISS_EXITS,
	SYSCALL_EXITS,
	ISI_EXITS,
	DSI_EXITS,
	EMULATED_INST_EXITS,
	EMULATED_MTMSRWE_EXITS,
	EMULATED_WRTEE_EXITS,
	EMULATED_MTSPR_EXITS,
	EMULATED_MFSPR_EXITS,
	EMULATED_MTMSR_EXITS,
	EMULATED_MFMSR_EXITS,
	EMULATED_TLBSX_EXITS,
	EMULATED_TLBWE_EXITS,
	EMULATED_RFI_EXITS,
	DEC_EXITS,
	EXT_INTR_EXITS,
	HALT_WAKEUP,
	USR_PR_INST,
	FP_UNAVAIL,
	DEBUG_EXITS,
	TIMEINGUEST,
	__NUMBER_OF_KVM_EXIT_TYPES
};

/* allow access to big endian 32bit upper/lower parts and 64bit var */
struct kvmppc_exit_timing {
	union {
		u64 tv64;
		struct {
			u32 tbu, tbl;
		} tv32;
	};
};

struct kvm_arch {
};

struct kvmppc_pte {
	ulong eaddr;
	u64 vpage;
	ulong raddr;
	bool may_read		: 1;
	bool may_write		: 1;
	bool may_execute	: 1;
};

struct kvmppc_mmu {
	/* book3s_64 only */
	void (*slbmte)(struct kvm_vcpu *vcpu, u64 rb, u64 rs);
	u64  (*slbmfee)(struct kvm_vcpu *vcpu, u64 slb_nr);
	u64  (*slbmfev)(struct kvm_vcpu *vcpu, u64 slb_nr);
	void (*slbie)(struct kvm_vcpu *vcpu, u64 slb_nr);
	void (*slbia)(struct kvm_vcpu *vcpu);
	/* book3s */
	void (*mtsrin)(struct kvm_vcpu *vcpu, u32 srnum, ulong value);
	u32  (*mfsrin)(struct kvm_vcpu *vcpu, u32 srnum);
	int  (*xlate)(struct kvm_vcpu *vcpu, gva_t eaddr, struct kvmppc_pte *pte, bool data);
	void (*reset_msr)(struct kvm_vcpu *vcpu);
	void (*tlbie)(struct kvm_vcpu *vcpu, ulong addr, bool large);
	int  (*esid_to_vsid)(struct kvm_vcpu *vcpu, ulong esid, u64 *vsid);
	u64  (*ea_to_vp)(struct kvm_vcpu *vcpu, gva_t eaddr, bool data);
	bool (*is_dcbz32)(struct kvm_vcpu *vcpu);
};

struct hpte_cache {
	u64 host_va;
	u64 pfn;
	ulong slot;
	struct kvmppc_pte pte;
};

struct kvm_vcpu_arch {
	ulong host_stack;
	u32 host_pid;
#ifdef CONFIG_PPC_BOOK3S
	ulong host_msr;
	ulong host_r2;
	void *host_retip;
	ulong trampoline_lowmem;
	ulong trampoline_enter;
	ulong highmem_handler;
	ulong rmcall;
	ulong host_paca_phys;
	struct kvmppc_mmu mmu;
#endif

	ulong gpr[32];

	u64 fpr[32];
	u64 fpscr;

#ifdef CONFIG_ALTIVEC
	vector128 vr[32];
	vector128 vscr;
#endif

#ifdef CONFIG_VSX
	u64 vsr[32];
#endif

#ifdef CONFIG_PPC_BOOK3S
	/* For Gekko paired singles */
	u32 qpr[32];
#endif

#ifdef CONFIG_BOOKE
	ulong pc;
	ulong ctr;
	ulong lr;

	ulong xer;
	u32 cr;
#endif

	ulong msr;
#ifdef CONFIG_PPC_BOOK3S
	ulong shadow_msr;
	ulong hflags;
	ulong guest_owned_ext;
#endif
	u32 mmucr;
	ulong sprg0;
	ulong sprg1;
	ulong sprg2;
	ulong sprg3;
	ulong sprg4;
	ulong sprg5;
	ulong sprg6;
	ulong sprg7;
	ulong srr0;
	ulong srr1;
	ulong csrr0;
	ulong csrr1;
	ulong dsrr0;
	ulong dsrr1;
	ulong dear;
	ulong esr;
	u32 dec;
	u32 decar;
	u32 tbl;
	u32 tbu;
	u32 tcr;
	u32 tsr;
	u32 ivor[64];
	ulong ivpr;
	u32 pir;
	u32 pvr;

	u32 shadow_pid;
	u32 pid;
	u32 swap_pid;

	u32 ccr0;
	u32 ccr1;
	u32 dbcr0;
	u32 dbcr1;
	u32 dbsr;

#ifdef CONFIG_KVM_EXIT_TIMING
	struct kvmppc_exit_timing timing_exit;
	struct kvmppc_exit_timing timing_last_enter;
	u32 last_exit_type;
	u32 timing_count_type[__NUMBER_OF_KVM_EXIT_TYPES];
	u64 timing_sum_duration[__NUMBER_OF_KVM_EXIT_TYPES];
	u64 timing_sum_quad_duration[__NUMBER_OF_KVM_EXIT_TYPES];
	u64 timing_min_duration[__NUMBER_OF_KVM_EXIT_TYPES];
	u64 timing_max_duration[__NUMBER_OF_KVM_EXIT_TYPES];
	u64 timing_last_exit;
	struct dentry *debugfs_exit_timing;
#endif

#ifdef CONFIG_BOOKE
	u32 last_inst;
	ulong fault_dear;
	ulong fault_esr;
	ulong queued_dear;
	ulong queued_esr;
#endif
	gpa_t paddr_accessed;

	u8 io_gpr; /* GPR used as IO source/target */
	u8 mmio_is_bigendian;
	u8 mmio_sign_extend;
	u8 dcr_needed;
	u8 dcr_is_write;
	u8 osi_needed;
	u8 osi_enabled;

	u32 cpr0_cfgaddr; /* holds the last set cpr0_cfgaddr */

	struct hrtimer dec_timer;
	struct tasklet_struct tasklet;
	u64 dec_jiffies;
	unsigned long pending_exceptions;

#ifdef CONFIG_PPC_BOOK3S
	struct hpte_cache hpte_cache[HPTEG_CACHE_NUM];
	int hpte_cache_offset;
#endif
};

#endif /* __POWERPC_KVM_HOST_H__ */
