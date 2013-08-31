/*
 * Userspace I/O platform driver with generic IRQ handling code.
 *
 * Based on uio_pdrv_genirq.c by Magnus Damm
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/uio_driver.h>
#include <linux/spinlock.h>
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/stringify.h>
#include <linux/slab.h>

#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

#define DRIVER_NAME "uflow"

struct uflow_platdata {
	struct uio_info *uioinfo;
	spinlock_t lock;
	unsigned long flags;
	struct platform_device *pdev;
};

static irqreturn_t uflow_handler(int irq, struct uio_info *dev_info)
{
	struct uflow_platdata *priv = dev_info->priv;

	/* Just disable the interrupt in the interrupt controller, and
	 * remember the state so we can allow user space to enable it later.
	 */

	if (!test_and_set_bit(0, &priv->flags))
		disable_irq_nosync(irq);

	return IRQ_HANDLED;
}

static int uflow_irqcontrol(struct uio_info *dev_info, s32 irq_on)
{
	struct uflow_platdata *priv = dev_info->priv;
	unsigned long flags;

	/* Allow user space to enable and disable the interrupt
	 * in the interrupt controller, but keep track of the
	 * state to prevent per-irq depth damage.
	 *
	 * Serialize this operation to support multiple tasks.
	 */

	spin_lock_irqsave(&priv->lock, flags);
	if (irq_on) {
		if (test_and_clear_bit(0, &priv->flags))
			enable_irq(dev_info->irq);
	} else {
		if (!test_and_set_bit(0, &priv->flags))
			disable_irq(dev_info->irq);
	}
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static int uflow_probe(struct platform_device *pdev)
{
	struct uio_info *uioinfo = pdev->dev.platform_data;
	struct uflow_platdata *priv;
	struct uio_mem *uiomem;
	int ret = -EINVAL;
	int i;

	if (pdev->dev.of_node) {
		/* alloc uioinfo for one device */
		uioinfo = kzalloc(sizeof(*uioinfo), GFP_KERNEL);
		if (!uioinfo) {
			ret = -ENOMEM;
			dev_err(&pdev->dev, "unable to kmalloc\n");
			goto bad2;
		}
		uioinfo->name = pdev->dev.of_node->name;
		uioinfo->version = "devicetree";
	}

	if (!uioinfo || !uioinfo->name || !uioinfo->version) {
		dev_err(&pdev->dev, "missing platform_data\n");
		goto bad0;
	}

	if (uioinfo->handler || uioinfo->irqcontrol ||
	    uioinfo->irq_flags & IRQF_SHARED) {
		dev_err(&pdev->dev, "interrupt configuration error\n");
		goto bad0;
	}

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		ret = -ENOMEM;
		dev_err(&pdev->dev, "unable to kmalloc\n");
		goto bad0;
	}

	priv->uioinfo = uioinfo;
	spin_lock_init(&priv->lock);
	priv->flags = 0;	/* interrupt is enabled to begin with */
	priv->pdev = pdev;

	uiomem = &uioinfo->mem[0];

	for (i = 0; i < pdev->num_resources; ++i) {
		struct resource *r = &pdev->resource[i];

		if (r->flags != IORESOURCE_MEM)
			continue;

		if (uiomem >= &uioinfo->mem[MAX_UIO_MAPS]) {
			dev_warn(&pdev->dev, "device has more than "
				 __stringify(MAX_UIO_MAPS)
				 " I/O memory resources.\n");
			break;
		}

		uiomem->memtype = UIO_MEM_PHYS;
		uiomem->addr = r->start;
		uiomem->size = resource_size(r);
		uiomem->name = r->name;
		++uiomem;
	}

	while (uiomem < &uioinfo->mem[MAX_UIO_MAPS]) {
		uiomem->size = 0;
		++uiomem;
	}

	/* This driver requires no hardware specific kernel code to handle
	 * interrupts. Instead, the interrupt handler simply disables the
	 * interrupt in the interrupt controller. User space is responsible
	 * for performing hardware specific acknowledge and re-enabling of
	 * the interrupt in the interrupt controller.
	 *
	 * Interrupt sharing is not supported.
	 */

	ret = platform_get_irq(pdev, 0);
	if (ret >= 0) {
		uioinfo->irq = ret;
		uioinfo->handler = uflow_handler;
		uioinfo->irqcontrol = uflow_irqcontrol;
	}

	uioinfo->priv = priv;

	ret = uio_register_device(&pdev->dev, priv->uioinfo);
	if (ret) {
		dev_err(&pdev->dev, "unable to register uio device\n");
		goto bad1;
	}

	platform_set_drvdata(pdev, priv);
	return 0;
 bad1:
	kfree(priv);
 bad0:
	/* kfree uioinfo for OF */
	if (pdev->dev.of_node)
		kfree(uioinfo);
 bad2:
	return ret;
}

static int uflow_remove(struct platform_device *pdev)
{
	struct uflow_platdata *priv = platform_get_drvdata(pdev);

	uio_unregister_device(priv->uioinfo);

	priv->uioinfo->handler = NULL;
	priv->uioinfo->irqcontrol = NULL;

	/* kfree uioinfo for OF */
	if (pdev->dev.of_node)
		kfree(priv->uioinfo);

	kfree(priv);
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id uio_of_genirq_match[] = {
	{.compatible = "generic-uio",},
	{ /* empty for now */ },
};

MODULE_DEVICE_TABLE(of, uio_of_genirq_match);
#else
# define uio_of_genirq_match NULL
#endif

static struct platform_driver uflow_driver = {
	.probe = uflow_probe,
	.remove = uflow_remove,
	.driver = {
		   .name = DRIVER_NAME,
		   .owner = THIS_MODULE,
		   .of_match_table = uio_of_genirq_match,
		   },
};

module_platform_driver(uflow_driver);

MODULE_AUTHOR("elab");
MODULE_DESCRIPTION("Userspace I/O platform driver for conv devices");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);
