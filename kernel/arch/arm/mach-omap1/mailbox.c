

#include <linux/kernel.h>
#include <linux/resource.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <plat/mailbox.h>
#include <mach/irqs.h>

#define MAILBOX_ARM2DSP1		0x00
#define MAILBOX_ARM2DSP1b		0x04
#define MAILBOX_DSP2ARM1		0x08
#define MAILBOX_DSP2ARM1b		0x0c
#define MAILBOX_DSP2ARM2		0x10
#define MAILBOX_DSP2ARM2b		0x14
#define MAILBOX_ARM2DSP1_Flag		0x18
#define MAILBOX_DSP2ARM1_Flag		0x1c
#define MAILBOX_DSP2ARM2_Flag		0x20

static void __iomem *mbox_base;

struct omap_mbox1_fifo {
	unsigned long cmd;
	unsigned long data;
	unsigned long flag;
};

struct omap_mbox1_priv {
	struct omap_mbox1_fifo tx_fifo;
	struct omap_mbox1_fifo rx_fifo;
};

static inline int mbox_read_reg(size_t ofs)
{
	return __raw_readw(mbox_base + ofs);
}

static inline void mbox_write_reg(u32 val, size_t ofs)
{
	__raw_writew(val, mbox_base + ofs);
}

/* msg */
static mbox_msg_t omap1_mbox_fifo_read(struct omap_mbox *mbox)
{
	struct omap_mbox1_fifo *fifo =
		&((struct omap_mbox1_priv *)mbox->priv)->rx_fifo;
	mbox_msg_t msg;

	msg = mbox_read_reg(fifo->data);
	msg |= ((mbox_msg_t) mbox_read_reg(fifo->cmd)) << 16;

	return msg;
}

static void
omap1_mbox_fifo_write(struct omap_mbox *mbox, mbox_msg_t msg)
{
	struct omap_mbox1_fifo *fifo =
		&((struct omap_mbox1_priv *)mbox->priv)->tx_fifo;

	mbox_write_reg(msg & 0xffff, fifo->data);
	mbox_write_reg(msg >> 16, fifo->cmd);
}

static int omap1_mbox_fifo_empty(struct omap_mbox *mbox)
{
	return 0;
}

static int omap1_mbox_fifo_full(struct omap_mbox *mbox)
{
	struct omap_mbox1_fifo *fifo =
		&((struct omap_mbox1_priv *)mbox->priv)->rx_fifo;

	return (mbox_read_reg(fifo->flag));
}

/* irq */
static void
omap1_mbox_enable_irq(struct omap_mbox *mbox, omap_mbox_type_t irq)
{
	if (irq == IRQ_RX)
		enable_irq(mbox->irq);
}

static void
omap1_mbox_disable_irq(struct omap_mbox *mbox, omap_mbox_type_t irq)
{
	if (irq == IRQ_RX)
		disable_irq(mbox->irq);
}

static int
omap1_mbox_is_irq(struct omap_mbox *mbox, omap_mbox_type_t irq)
{
	if (irq == IRQ_TX)
		return 0;
	return 1;
}

static struct omap_mbox_ops omap1_mbox_ops = {
	.type		= OMAP_MBOX_TYPE1,
	.fifo_read	= omap1_mbox_fifo_read,
	.fifo_write	= omap1_mbox_fifo_write,
	.fifo_empty	= omap1_mbox_fifo_empty,
	.fifo_full	= omap1_mbox_fifo_full,
	.enable_irq	= omap1_mbox_enable_irq,
	.disable_irq	= omap1_mbox_disable_irq,
	.is_irq		= omap1_mbox_is_irq,
};

/* FIXME: the following struct should be created automatically by the user id */

/* DSP */
static struct omap_mbox1_priv omap1_mbox_dsp_priv = {
	.tx_fifo = {
		.cmd	= MAILBOX_ARM2DSP1b,
		.data	= MAILBOX_ARM2DSP1,
		.flag	= MAILBOX_ARM2DSP1_Flag,
	},
	.rx_fifo = {
		.cmd	= MAILBOX_DSP2ARM1b,
		.data	= MAILBOX_DSP2ARM1,
		.flag	= MAILBOX_DSP2ARM1_Flag,
	},
};

struct omap_mbox mbox_dsp_info = {
	.name	= "dsp",
	.ops	= &omap1_mbox_ops,
	.priv	= &omap1_mbox_dsp_priv,
};
EXPORT_SYMBOL(mbox_dsp_info);

static int __devinit omap1_mbox_probe(struct platform_device *pdev)
{
	struct resource *res;

	if (pdev->num_resources != 2) {
		dev_err(&pdev->dev, "invalid number of resources: %d\n",
			pdev->num_resources);
		return -ENODEV;
	}

	/* MBOX base */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (unlikely(!res)) {
		dev_err(&pdev->dev, "invalid mem resource\n");
		return -ENODEV;
	}

	mbox_base = ioremap(res->start, resource_size(res));
	if (!mbox_base) {
		dev_err(&pdev->dev, "ioremap failed\n");
		return -ENODEV;
	}

	/* DSP IRQ */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (unlikely(!res)) {
		dev_err(&pdev->dev, "invalid irq resource\n");
		iounmap(mbox_base);
		return -ENODEV;
	}
	mbox_dsp_info.irq = res->start;

	return omap_mbox_register(&pdev->dev, &mbox_dsp_info);
}

static int __devexit omap1_mbox_remove(struct platform_device *pdev)
{
	omap_mbox_unregister(&mbox_dsp_info);

	return 0;
}

static struct platform_driver omap1_mbox_driver = {
	.probe	= omap1_mbox_probe,
	.remove	= __devexit_p(omap1_mbox_remove),
	.driver	= {
		.name	= "omap1-mailbox",
	},
};

static int __init omap1_mbox_init(void)
{
	return platform_driver_register(&omap1_mbox_driver);
}

static void __exit omap1_mbox_exit(void)
{
	platform_driver_unregister(&omap1_mbox_driver);
}

module_init(omap1_mbox_init);
module_exit(omap1_mbox_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("omap mailbox: omap1 architecture specific functions");
MODULE_AUTHOR("Hiroshi DOYU <Hiroshi.DOYU@nokia.com>");
MODULE_ALIAS("platform:omap1-mailbox");
