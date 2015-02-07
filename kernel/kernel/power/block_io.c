

#include <linux/bio.h>
#include <linux/kernel.h>
#include <linux/pagemap.h>
#include <linux/swap.h>

#include "power.h"

static int submit(int rw, struct block_device *bdev, sector_t sector,
		struct page *page, struct bio **bio_chain)
{
	const int bio_rw = rw | (1 << BIO_RW_SYNCIO) | (1 << BIO_RW_UNPLUG);
	struct bio *bio;

	bio = bio_alloc(__GFP_WAIT | __GFP_HIGH, 1);
	bio->bi_sector = sector;
	bio->bi_bdev = bdev;
	bio->bi_end_io = end_swap_bio_read;

	if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE) {
		printk(KERN_ERR "PM: Adding page to bio failed at %llu\n",
			(unsigned long long)sector);
		bio_put(bio);
		return -EFAULT;
	}

	lock_page(page);
	bio_get(bio);

	if (bio_chain == NULL) {
		submit_bio(bio_rw, bio);
		wait_on_page_locked(page);
		if (rw == READ)
			bio_set_pages_dirty(bio);
		bio_put(bio);
	} else {
		if (rw == READ)
			get_page(page);	/* These pages are freed later */
		bio->bi_private = *bio_chain;
		*bio_chain = bio;
		submit_bio(bio_rw, bio);
	}
	return 0;
}

int hib_bio_read_page(pgoff_t page_off, void *addr, struct bio **bio_chain)
{
	return submit(READ, hib_resume_bdev, page_off * (PAGE_SIZE >> 9),
			virt_to_page(addr), bio_chain);
}

int hib_bio_write_page(pgoff_t page_off, void *addr, struct bio **bio_chain)
{
	return submit(WRITE, hib_resume_bdev, page_off * (PAGE_SIZE >> 9),
			virt_to_page(addr), bio_chain);
}

int hib_wait_on_bio_chain(struct bio **bio_chain)
{
	struct bio *bio;
	struct bio *next_bio;
	int ret = 0;

	if (bio_chain == NULL)
		return 0;

	bio = *bio_chain;
	if (bio == NULL)
		return 0;
	while (bio) {
		struct page *page;

		next_bio = bio->bi_private;
		page = bio->bi_io_vec[0].bv_page;
		wait_on_page_locked(page);
		if (!PageUptodate(page) || PageError(page))
			ret = -EIO;
		put_page(page);
		bio_put(bio);
		bio = next_bio;
	}
	*bio_chain = NULL;
	return ret;
}
