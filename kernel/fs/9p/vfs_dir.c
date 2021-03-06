

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/inet.h>
#include <linux/idr.h>
#include <linux/slab.h>
#include <net/9p/9p.h>
#include <net/9p/client.h>

#include "v9fs.h"
#include "v9fs_vfs.h"
#include "fid.h"


struct p9_rdir {
	struct mutex mutex;
	int head;
	int tail;
	uint8_t *buf;
};


static inline int dt_type(struct p9_wstat *mistat)
{
	unsigned long perm = mistat->mode;
	int rettype = DT_REG;

	if (perm & P9_DMDIR)
		rettype = DT_DIR;
	if (perm & P9_DMSYMLINK)
		rettype = DT_LNK;

	return rettype;
}

static void p9stat_init(struct p9_wstat *stbuf)
{
	stbuf->name  = NULL;
	stbuf->uid   = NULL;
	stbuf->gid   = NULL;
	stbuf->muid  = NULL;
	stbuf->extension = NULL;
}


static int v9fs_dir_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
	int over;
	struct p9_wstat st;
	int err = 0;
	struct p9_fid *fid;
	int buflen;
	int reclen = 0;
	struct p9_rdir *rdir;

	P9_DPRINTK(P9_DEBUG_VFS, "name %s\n", filp->f_path.dentry->d_name.name);
	fid = filp->private_data;

	buflen = fid->clnt->msize - P9_IOHDRSZ;

	/* allocate rdir on demand */
	if (!fid->rdir) {
		rdir = kmalloc(sizeof(struct p9_rdir) + buflen, GFP_KERNEL);

		if (rdir == NULL) {
			err = -ENOMEM;
			goto exit;
		}
		spin_lock(&filp->f_dentry->d_lock);
		if (!fid->rdir) {
			rdir->buf = (uint8_t *)rdir + sizeof(struct p9_rdir);
			mutex_init(&rdir->mutex);
			rdir->head = rdir->tail = 0;
			fid->rdir = (void *) rdir;
			rdir = NULL;
		}
		spin_unlock(&filp->f_dentry->d_lock);
		kfree(rdir);
	}
	rdir = (struct p9_rdir *) fid->rdir;

	err = mutex_lock_interruptible(&rdir->mutex);
	if (err)
		return err;
	while (err == 0) {
		if (rdir->tail == rdir->head) {
			err = v9fs_file_readn(filp, rdir->buf, NULL,
							buflen, filp->f_pos);
			if (err <= 0)
				goto unlock_and_exit;

			rdir->head = 0;
			rdir->tail = err;
		}
		while (rdir->head < rdir->tail) {
			p9stat_init(&st);
			err = p9stat_read(rdir->buf + rdir->head,
						rdir->tail - rdir->head, &st,
						fid->clnt->proto_version);
			if (err) {
				P9_DPRINTK(P9_DEBUG_VFS, "returned %d\n", err);
				err = -EIO;
				p9stat_free(&st);
				goto unlock_and_exit;
			}
			reclen = st.size+2;

			over = filldir(dirent, st.name, strlen(st.name),
			    filp->f_pos, v9fs_qid2ino(&st.qid), dt_type(&st));

			p9stat_free(&st);

			if (over) {
				err = 0;
				goto unlock_and_exit;
			}
			rdir->head += reclen;
			filp->f_pos += reclen;
		}
	}

unlock_and_exit:
	mutex_unlock(&rdir->mutex);
exit:
	return err;
}



int v9fs_dir_release(struct inode *inode, struct file *filp)
{
	struct p9_fid *fid;

	fid = filp->private_data;
	P9_DPRINTK(P9_DEBUG_VFS,
			"inode: %p filp: %p fid: %d\n", inode, filp, fid->fid);
	filemap_write_and_wait(inode->i_mapping);
	p9_client_clunk(fid);
	return 0;
}

const struct file_operations v9fs_dir_operations = {
	.read = generic_read_dir,
	.llseek = generic_file_llseek,
	.readdir = v9fs_dir_readdir,
	.open = v9fs_file_open,
	.release = v9fs_dir_release,
};

const struct file_operations v9fs_dir_operations_dotl = {
	.read = generic_read_dir,
	.llseek = generic_file_llseek,
	.readdir = v9fs_dir_readdir,
	.open = v9fs_file_open,
	.release = v9fs_dir_release,
};
