
#include <linux/module.h>
#include <linux/init.h>
#include <linux/genalloc.h>

#include <mach/common.h>
#include <mach/sram.h>

static struct gen_pool *sram_pool;

void *sram_alloc(size_t len, dma_addr_t *dma)
{
	unsigned long vaddr;
	dma_addr_t dma_base = davinci_soc_info.sram_dma;

	if (dma)
		*dma = 0;
	if (!sram_pool || (dma && !dma_base))
		return NULL;

	vaddr = gen_pool_alloc(sram_pool, len);
	if (!vaddr)
		return NULL;

	if (dma)
		*dma = dma_base + (vaddr - SRAM_VIRT);
	return (void *)vaddr;

}
EXPORT_SYMBOL(sram_alloc);

void sram_free(void *addr, size_t len)
{
	gen_pool_free(sram_pool, (unsigned long) addr, len);
}
EXPORT_SYMBOL(sram_free);


static int __init sram_init(void)
{
	unsigned len = davinci_soc_info.sram_len;
	int status = 0;

	if (len) {
		len = min_t(unsigned, len, SRAM_SIZE);
		sram_pool = gen_pool_create(ilog2(SRAM_GRANULARITY), -1);
		if (!sram_pool)
			status = -ENOMEM;
	}
	if (sram_pool)
		status = gen_pool_add(sram_pool, SRAM_VIRT, len, -1);
	WARN_ON(status < 0);
	return status;
}
core_initcall(sram_init);

