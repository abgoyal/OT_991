

#ifndef __RECOVERY_DOT_H__
#define __RECOVERY_DOT_H__

#include "incore.h"

static inline void gfs2_replay_incr_blk(struct gfs2_sbd *sdp, unsigned int *blk)
{
	if (++*blk == sdp->sd_jdesc->jd_blocks)
	        *blk = 0;
}

extern int gfs2_replay_read_block(struct gfs2_jdesc *jd, unsigned int blk,
			   struct buffer_head **bh);

extern int gfs2_revoke_add(struct gfs2_sbd *sdp, u64 blkno, unsigned int where);
extern int gfs2_revoke_check(struct gfs2_sbd *sdp, u64 blkno, unsigned int where);
extern void gfs2_revoke_clean(struct gfs2_sbd *sdp);

extern int gfs2_find_jhead(struct gfs2_jdesc *jd,
		    struct gfs2_log_header_host *head);
extern int gfs2_recover_journal(struct gfs2_jdesc *gfs2_jd);
extern struct slow_work_ops gfs2_recover_ops;

#endif /* __RECOVERY_DOT_H__ */
