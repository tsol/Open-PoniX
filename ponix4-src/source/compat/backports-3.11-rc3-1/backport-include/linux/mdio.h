#ifndef __BACKPORT_LINUX_MDIO_H
#define __BACKPORT_LINUX_MDIO_H
#include_next <linux/mdio.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
#define mdio45_probe LINUX_BACKPORT(mdio45_probe)
#define mdio_set_flag LINUX_BACKPORT(mdio_set_flag)
#define mdio45_links_ok LINUX_BACKPORT(mdio45_links_ok)
#define mdio45_nway_restart LINUX_BACKPORT(mdio45_nway_restart)

#define mdio45_ethtool_gset_npage LINUX_BACKPORT(mdio45_ethtool_gset_npage)
#define mdio45_ethtool_spauseparam_an LINUX_BACKPORT(mdio45_ethtool_spauseparam_an)
#define mdio_mii_ioctl LINUX_BACKPORT(mdio_mii_ioctl)
#endif

#endif /* __BACKPORT_LINUX_MDIO_H */
