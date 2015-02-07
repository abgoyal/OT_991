
#ifndef _ASM_MMAN_H
#define _ASM_MMAN_H

#define PROT_NONE	0x00		/* page can not be accessed */
#define PROT_READ	0x01		/* page can be read */
#define PROT_WRITE	0x02		/* page can be written */
#define PROT_EXEC	0x04		/* page can be executed */
/*			0x08		   reserved for PROT_EXEC_NOFLUSH */
#define PROT_SEM	0x10		/* page may be used for atomic ops */
#define PROT_GROWSDOWN	0x01000000	/* mprotect flag: extend change to start of growsdown vma */
#define PROT_GROWSUP	0x02000000	/* mprotect flag: extend change to end of growsup vma */

#define MAP_SHARED	0x001		/* Share changes */
#define MAP_PRIVATE	0x002		/* Changes are private */
#define MAP_TYPE	0x00f		/* Mask for type of mapping */
#define MAP_FIXED	0x010		/* Interpret addr exactly */

/* not used by linux, but here to make sure we don't clash with ABI defines */
#define MAP_RENAME	0x020		/* Assign page to file */
#define MAP_AUTOGROW	0x040		/* File may grow by writing */
#define MAP_LOCAL	0x080		/* Copy on fork/sproc */
#define MAP_AUTORSRV	0x100		/* Logical swap reserved on demand */

/* These are linux-specific */
#define MAP_NORESERVE	0x0400		/* don't check for reservations */
#define MAP_ANONYMOUS	0x0800		/* don't use a file */
#define MAP_GROWSDOWN	0x1000		/* stack-like segment */
#define MAP_DENYWRITE	0x2000		/* ETXTBSY */
#define MAP_EXECUTABLE	0x4000		/* mark it as an executable */
#define MAP_LOCKED	0x8000		/* pages are locked */
#define MAP_POPULATE	0x10000		/* populate (prefault) pagetables */
#define MAP_NONBLOCK	0x20000		/* do not block on IO */
#define MAP_STACK	0x40000		/* give out an address that is best suited for process/thread stacks */
#define MAP_HUGETLB	0x80000		/* create a huge page mapping */

#define MS_ASYNC	0x0001		/* sync memory asynchronously */
#define MS_INVALIDATE	0x0002		/* invalidate mappings & caches */
#define MS_SYNC		0x0004		/* synchronous memory sync */

#define MCL_CURRENT	1		/* lock all current mappings */
#define MCL_FUTURE	2		/* lock all future mappings */

#define MADV_NORMAL	0		/* no further special treatment */
#define MADV_RANDOM	1		/* expect random page references */
#define MADV_SEQUENTIAL	2		/* expect sequential page references */
#define MADV_WILLNEED	3		/* will need these pages */
#define MADV_DONTNEED	4		/* don't need these pages */

/* common parameters: try to keep these consistent across architectures */
#define MADV_REMOVE	9		/* remove these pages & resources */
#define MADV_DONTFORK	10		/* don't inherit across fork */
#define MADV_DOFORK	11		/* do inherit across fork */

#define MADV_MERGEABLE   12		/* KSM may merge identical pages */
#define MADV_UNMERGEABLE 13		/* KSM may not merge identical pages */
#define MADV_HWPOISON    100		/* poison a page for testing */

/* compatibility flags */
#define MAP_FILE	0

#endif /* _ASM_MMAN_H */
