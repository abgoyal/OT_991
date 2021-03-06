

#include <linux/buffer_head.h>
#include <linux/mpage.h>
#include <linux/hash.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include "nilfs.h"
#include "page.h"
#include "mdt.h"
#include "dat.h"
#include "ifile.h"

static const struct address_space_operations def_gcinode_aops = {
	.sync_page		= block_sync_page,
};

int nilfs_gccache_submit_read_data(struct inode *inode, sector_t blkoff,
				   sector_t pbn, __u64 vbn,
				   struct buffer_head **out_bh)
{
	struct buffer_head *bh;
	int err;

	bh = nilfs_grab_buffer(inode, inode->i_mapping, blkoff, 0);
	if (unlikely(!bh))
		return -ENOMEM;

	if (buffer_uptodate(bh))
		goto out;

	if (pbn == 0) {
		struct inode *dat_inode = NILFS_I_NILFS(inode)->ns_dat;
					  /* use original dat, not gc dat. */
		err = nilfs_dat_translate(dat_inode, vbn, &pbn);
		if (unlikely(err)) { /* -EIO, -ENOMEM, -ENOENT */
			brelse(bh);
			goto failed;
		}
	}

	lock_buffer(bh);
	if (buffer_uptodate(bh)) {
		unlock_buffer(bh);
		goto out;
	}

	if (!buffer_mapped(bh)) {
		bh->b_bdev = NILFS_I_NILFS(inode)->ns_bdev;
		set_buffer_mapped(bh);
	}
	bh->b_blocknr = pbn;
	bh->b_end_io = end_buffer_read_sync;
	get_bh(bh);
	submit_bh(READ, bh);
	if (vbn)
		bh->b_blocknr = vbn;
 out:
	err = 0;
	*out_bh = bh;

 failed:
	unlock_page(bh->b_page);
	page_cache_release(bh->b_page);
	return err;
}

int nilfs_gccache_submit_read_node(struct inode *inode, sector_t pbn,
				   __u64 vbn, struct buffer_head **out_bh)
{
	int ret = nilfs_btnode_submit_block(&NILFS_I(inode)->i_btnode_cache,
					    vbn ? : pbn, pbn, out_bh);
	if (ret == -EEXIST) /* internal code (cache hit) */
		ret = 0;
	return ret;
}

int nilfs_gccache_wait_and_mark_dirty(struct buffer_head *bh)
{
	wait_on_buffer(bh);
	if (!buffer_uptodate(bh))
		return -EIO;
	if (buffer_dirty(bh))
		return -EEXIST;

	if (buffer_nilfs_node(bh))
		nilfs_btnode_mark_dirty(bh);
	else
		nilfs_mdt_mark_buffer_dirty(bh);
	return 0;
}

int nilfs_init_gccache(struct the_nilfs *nilfs)
{
	int loop;

	BUG_ON(nilfs->ns_gc_inodes_h);

	INIT_LIST_HEAD(&nilfs->ns_gc_inodes);

	nilfs->ns_gc_inodes_h =
		kmalloc(sizeof(struct hlist_head) * NILFS_GCINODE_HASH_SIZE,
			GFP_NOFS);
	if (nilfs->ns_gc_inodes_h == NULL)
		return -ENOMEM;

	for (loop = 0; loop < NILFS_GCINODE_HASH_SIZE; loop++)
		INIT_HLIST_HEAD(&nilfs->ns_gc_inodes_h[loop]);
	return 0;
}

void nilfs_destroy_gccache(struct the_nilfs *nilfs)
{
	if (nilfs->ns_gc_inodes_h) {
		nilfs_remove_all_gcinode(nilfs);
		kfree(nilfs->ns_gc_inodes_h);
		nilfs->ns_gc_inodes_h = NULL;
	}
}

static struct inode *alloc_gcinode(struct the_nilfs *nilfs, ino_t ino,
				   __u64 cno)
{
	struct inode *inode;
	struct nilfs_inode_info *ii;

	inode = nilfs_mdt_new_common(nilfs, NULL, ino, GFP_NOFS, 0);
	if (!inode)
		return NULL;

	inode->i_op = NULL;
	inode->i_fop = NULL;
	inode->i_mapping->a_ops = &def_gcinode_aops;

	ii = NILFS_I(inode);
	ii->i_cno = cno;
	ii->i_flags = 0;
	ii->i_state = 1 << NILFS_I_GCINODE;
	ii->i_bh = NULL;
	nilfs_bmap_init_gc(ii->i_bmap);

	return inode;
}

static unsigned long ihash(ino_t ino, __u64 cno)
{
	return hash_long((unsigned long)((ino << 2) + cno),
			 NILFS_GCINODE_HASH_BITS);
}

struct inode *nilfs_gc_iget(struct the_nilfs *nilfs, ino_t ino, __u64 cno)
{
	struct hlist_head *head = nilfs->ns_gc_inodes_h + ihash(ino, cno);
	struct hlist_node *node;
	struct inode *inode;

	hlist_for_each_entry(inode, node, head, i_hash) {
		if (inode->i_ino == ino && NILFS_I(inode)->i_cno == cno)
			return inode;
	}

	inode = alloc_gcinode(nilfs, ino, cno);
	if (likely(inode)) {
		hlist_add_head(&inode->i_hash, head);
		list_add(&NILFS_I(inode)->i_dirty, &nilfs->ns_gc_inodes);
	}
	return inode;
}

void nilfs_clear_gcinode(struct inode *inode)
{
	nilfs_mdt_destroy(inode);
}

void nilfs_remove_all_gcinode(struct the_nilfs *nilfs)
{
	struct hlist_head *head = nilfs->ns_gc_inodes_h;
	struct hlist_node *node, *n;
	struct inode *inode;
	int loop;

	for (loop = 0; loop < NILFS_GCINODE_HASH_SIZE; loop++, head++) {
		hlist_for_each_entry_safe(inode, node, n, head, i_hash) {
			hlist_del_init(&inode->i_hash);
			list_del_init(&NILFS_I(inode)->i_dirty);
			nilfs_clear_gcinode(inode); /* might sleep */
		}
	}
}
