#define __NO_VERSION__
#include "adriver.h"
#ifndef KERN_DEFAULT
#define KERN_DEFAULT	""
#endif

/*
 *  Misc and compatibility things
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <linux/init.h>
#include <linux/export.h>
#include <linux/moduleparam.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <sound/core.h>

#ifdef CONFIG_SND_DEBUG

#ifdef CONFIG_SND_DEBUG_VERBOSE
#define DEFAULT_DEBUG_LEVEL	2
#else
#define DEFAULT_DEBUG_LEVEL	1
#endif

static int debug = DEFAULT_DEBUG_LEVEL;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Debug level (0 = disable)");

#endif /* CONFIG_SND_DEBUG */

void release_and_free_resource(struct resource *res)
{
	if (res) {
		release_resource(res);
		kfree_nocheck(res);
	}
}

EXPORT_SYMBOL(release_and_free_resource);

#ifdef CONFIG_SND_VERBOSE_PRINTK
/* strip the leading path if the given path is absolute */
static const char *sanity_file_name(const char *path)
{
	if (*path == '/')
		return strrchr(path, '/') + 1;
	else
		return path;
}
/* print file and line with a certain printk prefix */
static int print_snd_pfx(unsigned int level, const char *path, int line,
			 const char *format)
{
	const char *file = sanity_file_name(path);
	char tmp[] = "<0>";
	const char *pfx = level ? KERN_DEBUG : KERN_DEFAULT;
	int ret = 0;

	if (format[0] == '<' && format[2] == '>') {
		tmp[1] = format[1];
		pfx = tmp;
		ret = 1;
	}
	printk("%sALSA %s:%d: ", pfx, file, line);
	return ret;
}
#else
#define print_snd_pfx(level, path, line, format)	0
#endif

#if defined(CONFIG_SND_DEBUG) || defined(CONFIG_SND_VERBOSE_PRINTK)
void __snd_printk(unsigned int level, const char *path, int line,
		  const char *format, ...)
{
	va_list args;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
#ifdef CONFIG_SND_VERBOSE_PRINTK
	struct va_format vaf;
	char verbose_fmt[] = KERN_DEFAULT "ALSA %s:%d %pV";
#endif
#endif

#ifdef CONFIG_SND_DEBUG	
	if (debug < level)
		return;
#endif

	va_start(args, format);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
	if (print_snd_pfx(level, path, line, format))
		format += 3; /* skip the printk level-prefix */
	vprintk(format, args);
#else
#ifdef CONFIG_SND_VERBOSE_PRINTK
	vaf.fmt = format;
	vaf.va = &args;
	if (format[0] == '<' && format[2] == '>') {
		memcpy(verbose_fmt, format, 3);
		vaf.fmt = format + 3;
	} else if (level)
		memcpy(verbose_fmt, KERN_DEBUG, 3);
	printk(verbose_fmt, sanity_file_name(path), line, &vaf);
#else
	vprintk(format, args);
#endif
	va_end(args);
#endif /* < 2.6.36 */
}
EXPORT_SYMBOL_GPL(__snd_printk);
#endif

#ifdef CONFIG_PCI
#include <linux/pci.h>
/**
 * snd_pci_quirk_lookup_id - look up a PCI SSID quirk list
 * @vendor: PCI SSV id
 * @device: PCI SSD id
 * @list: quirk list, terminated by a null entry
 *
 * Look through the given quirk list and finds a matching entry
 * with the same PCI SSID.  When subdevice is 0, all subdevice
 * values may match.
 *
 * Returns the matched entry pointer, or NULL if nothing matched.
 */
const struct snd_pci_quirk *
snd_pci_quirk_lookup_id(u16 vendor, u16 device,
			const struct snd_pci_quirk *list)
{
	const struct snd_pci_quirk *q;

	for (q = list; q->subvendor; q++) {
		if (q->subvendor != vendor)
			continue;
		if (!q->subdevice ||
		    (device & q->subdevice_mask) == q->subdevice)
			return q;
	}
	return NULL;
}
EXPORT_SYMBOL(snd_pci_quirk_lookup_id);

/**
 * snd_pci_quirk_lookup - look up a PCI SSID quirk list
 * @pci: pci_dev handle
 * @list: quirk list, terminated by a null entry
 *
 * Look through the given quirk list and finds a matching entry
 * with the same PCI SSID.  When subdevice is 0, all subdevice
 * values may match.
 *
 * Returns the matched entry pointer, or NULL if nothing matched.
 */
const struct snd_pci_quirk *
snd_pci_quirk_lookup(struct pci_dev *pci, const struct snd_pci_quirk *list)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 3, 0)
	return snd_pci_quirk_lookup_id(pci->subsystem_vendor,
				       pci->subsystem_device,
				       list);
#else
	unsigned short subsystem_vendor, subsystem_device;
	pci_read_config_word(pci, PCI_SUBSYSTEM_VENDOR_ID, &subsystem_vendor);
	pci_read_config_word(pci, PCI_SUBSYSTEM_ID, &subsystem_device);
	return snd_pci_quirk_lookup_id(subsystem_vendor,
				       subsystem_device,
				       list);
#endif
}
EXPORT_SYMBOL(snd_pci_quirk_lookup);
#endif

/* just for compatibility reason */
void snd_verbose_printk(const char *file, int line, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	if (print_snd_pfx(0, file, line, format))
		format += 3; /* skip the printk level-prefix */
	vprintk(format, args);
	va_end(args);
}
EXPORT_SYMBOL(snd_verbose_printk);

void snd_verbose_printd(const char *file, int line, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	if (print_snd_pfx(1, file, line, format))
		format += 3; /* skip the printk level-prefix */
	vprintk(format, args);
	va_end(args);

}
EXPORT_SYMBOL(snd_verbose_printd);
