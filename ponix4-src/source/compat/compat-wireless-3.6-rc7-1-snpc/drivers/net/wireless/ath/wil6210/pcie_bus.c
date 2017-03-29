/*
 * Copyright (c) 2012 Qualcomm Atheros, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>
#include <linux/pci.h>
#include <linux/moduleparam.h>

#include "wil6210.h"

static int use_msi; /* TODO: set default to 1 or 3 when hardware fixed */
module_param(use_msi, int, S_IRUGO);
MODULE_PARM_DESC(use_msi,
		" Use MSI interrupt: 0 - (default) - don't, 1 - single, or 3");

/* debug */
void send_echo(struct wil6210_priv *wil);

/* Bus ops */
static int if_pcie_enable(struct wil6210_priv *wil)
{
	struct pci_dev *pdev = wil->pdev;
	int rc;
	wil_info(wil, "%s()\n", __func__);
	/*
	 * how many MSI interrupts to request?
	 */
	wil->n_msi = use_msi;
	/* TODO: how to deal with 3 MSI? */
	if (wil->n_msi) {
		wil_info(wil, "Setup %d MSI interrupts\n", use_msi);
		rc = pci_enable_msi_block(pdev, wil->n_msi);
		if (rc) {
			wil_err(wil, "pci_enable_msi failed, use INTx\n");
			wil->n_msi = 0;
		}
	} else {
		wil_info(wil, "MSI interrupts disabled, use INTx\n");
	}
	rc = wil6210_init_irq(wil, pdev->irq);
	if (rc)
		return rc;
	/* need reset here to obtain MAC */
	rc = wil_reset(wil);
	if (rc)
		goto release_irq;
	pci_set_master(pdev);

	return 0;
 release_irq:
	wil6210_fini_irq(wil, pdev->irq);
	/* safe to call if no MSI */
	pci_disable_msi(pdev);
	return rc;
}

static int if_pcie_disable(struct wil6210_priv *wil)
{
	struct pci_dev *pdev = wil->pdev;
	struct device *dev = wil_to_dev(wil);
	dev_info(dev, "%s()\n", __func__);

	pci_clear_master(pdev);
	/* disable IRQ */
	/* release IRQ */
	wil6210_fini_irq(wil, pdev->irq);
	/* safe to call if no MSI */
	pci_disable_msi(pdev);
	/* disable HW */
#if 0
	wil_reset(wil);
#endif
	return 0;
}

static int wil_pcie_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	struct wil6210_priv *wil;
	struct device *dev = &pdev->dev;
	void __iomem *csr;
	int rc;
	dev_info(dev, "%s()\n", __func__);

	/* check HW */
	dev_info(&pdev->dev, WIL_NAME " device found [%04x:%04x] (rev %x)\n",
		 (int)pdev->vendor, (int)pdev->device, (int)pdev->revision);
	if (pci_resource_len(pdev, 0) != WIL6210_MEM_SIZE) {
		dev_err(&pdev->dev, "Not " WIL_NAME "? "
				"BAR0 size is %lu while expecting %lu\n",
				(unsigned long)pci_resource_len(pdev, 0),
				WIL6210_MEM_SIZE);
		return -ENODEV;
	}
	rc = pci_enable_device(pdev);
	if (rc) {
		dev_err(&pdev->dev, "pci_enable_device failed\n");
		return -ENODEV;
	}
	/* rollback to err_disable_pdev */

	rc = pci_request_region(pdev, 0, WIL_NAME);
	if (rc) {
		dev_err(&pdev->dev, "pci_request_region failed\n");
		goto err_disable_pdev;
	}
	/* rollback to err_release_reg */
	csr = pci_ioremap_bar(pdev, 0);
	if (!csr) {
		dev_err(&pdev->dev, "pci_ioremap_bar failed\n");
		rc = -ENODEV;
		goto err_release_reg;
	}
	/* rollback to err_iounmap */
	dev_info(&pdev->dev, "CSR at %pR -> %p\n", &pdev->resource[0],
			csr);

	wil = wil_if_alloc(dev, csr);
	if (IS_ERR(wil)) {
		rc = (int)PTR_ERR(wil);
		dev_err(dev, "wil_if_alloc failed: %d\n", rc);
		goto err_iounmap;
	}
	/* rollback to if_free */

	pci_set_drvdata(pdev, wil);
	wil->pdev = pdev;

	/* FW should raise IRQ when ready */
	rc = if_pcie_enable(wil);
	if (rc) {
		wil_err(wil, "Enable device failed\n");
		goto if_free;
	}
	/* rollback to bus_disable */

	rc = wil_if_add(wil);
	if (rc) {
		wil_err(wil, "wil_if_add failed: %d\n", rc);
		goto bus_disable;
	}
	wil6210_debugfs_init(wil);
	/* rollback to debugfs_exit */

	{ /* print various info */
		struct net_device *ndev = wil_to_ndev(wil);
		const char *pdev_name = pci_name(pdev);
		const char *wiphydev_name = dev_name(wil_to_dev(wil));
		const char *ndev_name = netdev_name(ndev);
		const char *ifc_name = ndev->name;
		struct pci_driver *drv = pci_dev_driver(pdev);
		const char *drv_name = drv ? drv->name : "(no drv)";
		pr_info("Driver  : <%s>\n", drv_name ?: "(null)");
		pr_info("PCI dev : <%s>\n", pdev_name ?: "(null)");
		pr_info("Net dev : <%s>\n", ndev_name ?: "(null)");
		pr_info("Net ifc : <%s>\n", ifc_name ?: "(null)");
		pr_info("Wiphy   : <%s>\n", wiphydev_name ?: "(null)");

	}
	send_echo(wil);
	return 0;
#if 0
 debugfs_exit:
	wil6210_debugfs_remove(wil);
#endif
 bus_disable:
	if_pcie_disable(wil);
 if_free:
	wil_if_free(wil);
 err_iounmap:
	pci_iounmap(pdev, csr);
 err_release_reg:
	pci_release_region(pdev, 0);
 err_disable_pdev:
	pci_disable_device(pdev);
	return rc;
}

static void wil_pcie_remove(struct pci_dev *pdev)
{
	struct wil6210_priv *wil = pci_get_drvdata(pdev);
	wil_info(wil, "%s()\n", __func__);

	wil6210_debugfs_remove(wil);
	if_pcie_disable(wil);
	wil_if_remove(wil);
	wil_if_free(wil);
	pci_iounmap(pdev, wil->csr);
	pci_release_region(pdev, 0);
	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);
}

static DEFINE_PCI_DEVICE_TABLE(wil6210_pcie_ids) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_WIL6210, PCI_DEVICE_ID_WIL6210),
			/*.driver_data  = (kernel_ulong_t)0,*/},
	{ /* end: all zeroes */	},
};
MODULE_DEVICE_TABLE(pci, wil6210_pcie_ids);

static struct pci_driver wil_driver = {
	.probe		= wil_pcie_probe,
	.remove		= __devexit_p(wil_pcie_remove),
	.id_table	= wil6210_pcie_ids,
	.name		= WIL_NAME,
};


static int __init wil_init_module(void)
{
	return pci_register_driver(&wil_driver);
}

static void __exit wil_exit_module(void)
{
	pci_unregister_driver(&wil_driver);
}

module_init(wil_init_module);
module_exit(wil_exit_module);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Qualcomm Atheros <wil6210@qualcomm.com>");
MODULE_DESCRIPTION("Driver for 60g WiFi WIL6210 card");
