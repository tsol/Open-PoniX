#include "adriver.h"
/*
 *  Advanced Linux Sound Architecture
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
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/device.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/minors.h>
#include <sound/info.h>
#include <sound/version.h>
#include <sound/control.h>
#include <sound/initval.h>
#include <linux/kmod.h>
#include <linux/mutex.h>

static int major = CONFIG_SND_MAJOR;
int snd_major;
EXPORT_SYMBOL(snd_major);

static int cards_limit = 1;
#ifdef CONFIG_DEVFS_FS
static int device_mode = S_IFCHR | S_IRUGO | S_IWUGO;
#endif

MODULE_AUTHOR("Jaroslav Kysela <perex@perex.cz>");
MODULE_DESCRIPTION("Advanced Linux Sound Architecture driver for soundcards.");
MODULE_LICENSE("GPL");
module_param(major, int, 0444);
MODULE_PARM_DESC(major, "Major # for sound driver.");
module_param(cards_limit, int, 0444);
MODULE_PARM_DESC(cards_limit, "Count of auto-loadable soundcards.");
#ifdef CONFIG_DEVFS_FS
module_param(device_mode, int, 0444);
MODULE_PARM_DESC(device_mode, "Device file permission mask for devfs.");
#endif
MODULE_ALIAS_CHARDEV_MAJOR(CONFIG_SND_MAJOR);

/* this one holds the actual max. card number currently available.
 * as default, it's identical with cards_limit option.  when more
 * modules are loaded manually, this limit number increases, too.
 */
int snd_ecards_limit;
EXPORT_SYMBOL(snd_ecards_limit);

static struct snd_minor *snd_minors[SNDRV_OS_MINORS];
static DEFINE_MUTEX(sound_mutex);

#ifdef CONFIG_MODULES

/**
 * snd_request_card - try to load the card module
 * @card: the card number
 *
 * Tries to load the module "snd-card-X" for the given card number
 * via request_module.  Returns immediately if already loaded.
 */
void snd_request_card(int card)
{
	if (snd_card_locked(card))
		return;
	if (card < 0 || card >= cards_limit)
		return;
	request_module("snd-card-%i", card);
}

EXPORT_SYMBOL(snd_request_card);

static void snd_request_other(int minor)
{
	char *str;

	switch (minor) {
	case SNDRV_MINOR_SEQUENCER:	str = "snd-seq";	break;
	case SNDRV_MINOR_TIMER:		str = "snd-timer";	break;
	default:			return;
	}
	request_module(str);
}

#endif	/* modular kernel */

/**
 * snd_lookup_minor_data - get user data of a registered device
 * @minor: the minor number
 * @type: device type (SNDRV_DEVICE_TYPE_XXX)
 *
 * Checks that a minor device with the specified type is registered, and returns
 * its user data pointer.
 */
void *snd_lookup_minor_data(unsigned int minor, int type)
{
	struct snd_minor *mreg;
	void *private_data;

	if (minor >= ARRAY_SIZE(snd_minors))
		return NULL;
	mutex_lock(&sound_mutex);
	mreg = snd_minors[minor];
	if (mreg && mreg->type == type)
		private_data = mreg->private_data;
	else
		private_data = NULL;
	mutex_unlock(&sound_mutex);
	return private_data;
}

EXPORT_SYMBOL(snd_lookup_minor_data);

#ifdef CONFIG_MODULES
static struct snd_minor *autoload_device(unsigned int minor)
{
	int dev;
	mutex_unlock(&sound_mutex); /* release lock temporarily */
	dev = SNDRV_MINOR_DEVICE(minor);
	if (dev == SNDRV_MINOR_CONTROL) {
		/* /dev/aloadC? */
		int card = SNDRV_MINOR_CARD(minor);
		if (snd_cards[card] == NULL)
			snd_request_card(card);
	} else if (dev == SNDRV_MINOR_GLOBAL) {
		/* /dev/aloadSEQ */
		snd_request_other(minor);
	}
	mutex_lock(&sound_mutex); /* reacuire lock */
	return snd_minors[minor];
}
#else /* !CONFIG_MODULES */
#define autoload_device(minor)	NULL
#endif /* CONFIG_MODULES */

static int snd_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct snd_minor *mptr = NULL;
	const struct file_operations *old_fops;
	int err = 0;

	if (minor >= ARRAY_SIZE(snd_minors))
		return -ENODEV;
	mutex_lock(&sound_mutex);
	mptr = snd_minors[minor];
	if (mptr == NULL) {
		mptr = autoload_device(minor);
		if (!mptr) {
			mutex_unlock(&sound_mutex);
			return -ENODEV;
		}
	}
	old_fops = file->f_op;
	file->f_op = fops_get(mptr->f_ops);
	if (file->f_op == NULL) {
		file->f_op = old_fops;
		err = -ENODEV;
	}
	mutex_unlock(&sound_mutex);
	if (err < 0)
		return err;

	if (file->f_op->open) {
		err = file->f_op->open(inode, file);
		if (err) {
			fops_put(file->f_op);
			file->f_op = fops_get(old_fops);
		}
	}
	fops_put(old_fops);
	return err;
}

static const struct file_operations snd_fops =
{
#ifndef LINUX_2_2
	.owner =	THIS_MODULE,
#endif
	.open =		snd_open,
	.llseek =	noop_llseek,
};

#ifdef CONFIG_SND_DYNAMIC_MINORS
static int snd_find_free_minor(int type)
{
	int minor;

	/* static minors for module auto loading */
	if (type == SNDRV_DEVICE_TYPE_SEQUENCER)
		return SNDRV_MINOR_SEQUENCER;
	if (type == SNDRV_DEVICE_TYPE_TIMER)
		return SNDRV_MINOR_TIMER;

	for (minor = 0; minor < ARRAY_SIZE(snd_minors); ++minor) {
		/* skip static minors still used for module auto loading */
		if (SNDRV_MINOR_DEVICE(minor) == SNDRV_MINOR_CONTROL)
			continue;
		if (minor == SNDRV_MINOR_SEQUENCER ||
		    minor == SNDRV_MINOR_TIMER)
			continue;
		if (!snd_minors[minor])
			return minor;
	}
	return -EBUSY;
}
#else
static int snd_kernel_minor(int type, struct snd_card *card, int dev)
{
	int minor;

	switch (type) {
	case SNDRV_DEVICE_TYPE_SEQUENCER:
	case SNDRV_DEVICE_TYPE_TIMER:
		minor = type;
		break;
	case SNDRV_DEVICE_TYPE_CONTROL:
		if (snd_BUG_ON(!card))
			return -EINVAL;
		minor = SNDRV_MINOR(card->number, type);
		break;
	case SNDRV_DEVICE_TYPE_HWDEP:
	case SNDRV_DEVICE_TYPE_RAWMIDI:
	case SNDRV_DEVICE_TYPE_PCM_PLAYBACK:
	case SNDRV_DEVICE_TYPE_PCM_CAPTURE:
	case SNDRV_DEVICE_TYPE_COMPRESS:
		if (snd_BUG_ON(!card))
			return -EINVAL;
		minor = SNDRV_MINOR(card->number, type + dev);
		break;
	default:
		return -EINVAL;
	}
	if (snd_BUG_ON(minor < 0 || minor >= SNDRV_OS_MINORS))
		return -EINVAL;
	return minor;
}
#endif

/**
 * snd_register_device_for_dev - Register the ALSA device file for the card
 * @type: the device type, SNDRV_DEVICE_TYPE_XXX
 * @card: the card instance
 * @dev: the device index
 * @f_ops: the file operations
 * @private_data: user pointer for f_ops->open()
 * @name: the device file name
 * @device: the &struct device to link this new device to
 *
 * Registers an ALSA device file for the given card.
 * The operators have to be set in reg parameter.
 *
 * Returns zero if successful, or a negative error code on failure.
 */
int snd_register_device_for_dev(int type, struct snd_card *card, int dev,
				const struct file_operations *f_ops,
				void *private_data,
				const char *name, struct device *device)
{
	int minor;
	struct snd_minor *preg;

	if (snd_BUG_ON(!name))
		return -EINVAL;
#ifdef CONFIG_DEVFS_FS
	preg = kmalloc(sizeof(*preg) + strlen(name) + 1, GFP_KERNEL);
#else
	preg = kmalloc(sizeof *preg, GFP_KERNEL);
#endif
	if (preg == NULL)
		return -ENOMEM;
	preg->type = type;
	preg->card = card ? card->number : -1;
	preg->device = dev;
	preg->f_ops = f_ops;
	preg->private_data = private_data;
#ifdef CONFIG_DEVFS_FS
	strcpy((char *)(preg + 1), name);
#endif
	mutex_lock(&sound_mutex);
#ifdef CONFIG_SND_DYNAMIC_MINORS
	minor = snd_find_free_minor(type);
#else
	minor = snd_kernel_minor(type, card, dev);
	if (minor >= 0 && snd_minors[minor])
		minor = -EBUSY;
#endif
	if (minor < 0) {
		mutex_unlock(&sound_mutex);
		kfree(preg);
		return minor;
	}
	snd_minors[minor] = preg;
#ifdef CONFIG_DEVFS_FS
	if (type != SNDRV_DEVICE_TYPE_CONTROL || preg->card >= cards_limit)
		devfs_mk_cdev(MKDEV(major, minor), S_IFCHR | device_mode,
			      "snd/%s", name);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
#ifdef CONFIG_SND_HAS_DEVICE_CREATE_DRVDATA
	preg->dev = device_create_drvdata(sound_class, device,
					  MKDEV(major, minor),
					  private_data, "%s", name);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
	preg->dev = device_create(sound_class, device, MKDEV(major, minor),
				  private_data, "%s", name);
#else
  	preg->dev = device_create(sound_class, device, MKDEV(major, minor),
  				  "%s", name);
#endif /* SND_HAS_DEVICE_CREATE_DRVDATA */
	if (IS_ERR(preg->dev)) {
		snd_minors[minor] = NULL;
		mutex_unlock(&sound_mutex);
		minor = PTR_ERR(preg->dev);
		kfree(preg);
		return minor;
	}
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 26) && \
	!defined(CONFIG_SND_HAS_DEVICE_CREATE_DRVDATA)
  	if (preg->dev)
  		dev_set_drvdata(preg->dev, private_data);
#endif

#elif defined(CONFIG_SND_HAVE_CLASS_SIMPLE)
	class_simple_device_add((struct class_simple *)sound_class,
				MKDEV(major, minor), device, name);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 2)
	preg->dev = (struct device *)class_device_create(sound_class, NULL,
							 MKDEV(major, minor),
						         device, "%s", name);
	if (preg->dev)
		class_set_devdata((struct class_device *)preg->dev, private_data);
#endif /* 2.6.20 */

	mutex_unlock(&sound_mutex);
	return 0;
}

EXPORT_SYMBOL(snd_register_device_for_dev);

/* find the matching minor record
 * return the index of snd_minor, or -1 if not found
 */
static int find_snd_minor(int type, struct snd_card *card, int dev)
{
	int cardnum, minor;
	struct snd_minor *mptr;

	cardnum = card ? card->number : -1;
	for (minor = 0; minor < ARRAY_SIZE(snd_minors); ++minor)
		if ((mptr = snd_minors[minor]) != NULL &&
		    mptr->type == type &&
		    mptr->card == cardnum &&
		    mptr->device == dev)
			return minor;
	return -1;
}

/**
 * snd_unregister_device - unregister the device on the given card
 * @type: the device type, SNDRV_DEVICE_TYPE_XXX
 * @card: the card instance
 * @dev: the device index
 *
 * Unregisters the device file already registered via
 * snd_register_device().
 *
 * Returns zero if sucecessful, or a negative error code on failure
 */
int snd_unregister_device(int type, struct snd_card *card, int dev)
{
	int minor;
#ifdef CONFIG_DEVFS_FS
	struct snd_minor *mptr;
#endif

	mutex_lock(&sound_mutex);
	minor = find_snd_minor(type, card, dev);
	if (minor < 0) {
		mutex_unlock(&sound_mutex);
		return -EINVAL;
	}

#ifdef CONFIG_DEVFS_FS
	mptr = snd_minors[minor];
	if (mptr->type != SNDRV_DEVICE_TYPE_CONTROL ||
	    mptr->card >= cards_limit) /* created in sound.c */
		devfs_remove("snd/%s", (char *)(mptr + 1));
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	device_destroy(sound_class, MKDEV(major, minor));
#elif defined(CONFIG_SND_HAVE_CLASS_SIMPLE)
	class_simple_device_remove(MKDEV(major, minor));
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 2)
	class_device_destroy(sound_class, MKDEV(major, minor));
#endif

	kfree(snd_minors[minor]);
	snd_minors[minor] = NULL;
	mutex_unlock(&sound_mutex);
	return 0;
}

EXPORT_SYMBOL(snd_unregister_device);

int snd_add_device_sysfs_file(int type, struct snd_card *card, int dev,
			      struct device_attribute *attr)
{
#if defined(CONFIG_SND_HAVE_CLASS_SIMPLE) || LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 2)
	return -EINVAL;
#else
	int minor, ret = -EINVAL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	struct device *d;
#else
	struct class_device *d;
#endif

	mutex_lock(&sound_mutex);
	minor = find_snd_minor(type, card, dev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	if (minor >= 0 && (d = snd_minors[minor]->dev) != NULL)
		ret = device_create_file(d, attr);
#else
	if (minor >= 0 && (d = (struct class_device *)snd_minors[minor]->dev) != NULL)
		ret = class_device_create_file(d, (const struct class_device_attribute *)attr);
#endif
	mutex_unlock(&sound_mutex);
	return ret;
#endif
}

EXPORT_SYMBOL(snd_add_device_sysfs_file);

#ifdef CONFIG_PROC_FS
/*
 *  INFO PART
 */

static struct snd_info_entry *snd_minor_info_entry;

static const char *snd_device_type_name(int type)
{
	switch (type) {
	case SNDRV_DEVICE_TYPE_CONTROL:
		return "control";
	case SNDRV_DEVICE_TYPE_HWDEP:
		return "hardware dependent";
	case SNDRV_DEVICE_TYPE_RAWMIDI:
		return "raw midi";
	case SNDRV_DEVICE_TYPE_PCM_PLAYBACK:
		return "digital audio playback";
	case SNDRV_DEVICE_TYPE_PCM_CAPTURE:
		return "digital audio capture";
	case SNDRV_DEVICE_TYPE_SEQUENCER:
		return "sequencer";
	case SNDRV_DEVICE_TYPE_TIMER:
		return "timer";
	default:
		return "?";
	}
}

static void snd_minor_info_read(struct snd_info_entry *entry, struct snd_info_buffer *buffer)
{
	int minor;
	struct snd_minor *mptr;

	mutex_lock(&sound_mutex);
	for (minor = 0; minor < SNDRV_OS_MINORS; ++minor) {
		if (!(mptr = snd_minors[minor]))
			continue;
		if (mptr->card >= 0) {
			if (mptr->device >= 0)
				snd_iprintf(buffer, "%3i: [%2i-%2i]: %s\n",
					    minor, mptr->card, mptr->device,
					    snd_device_type_name(mptr->type));
			else
				snd_iprintf(buffer, "%3i: [%2i]   : %s\n",
					    minor, mptr->card,
					    snd_device_type_name(mptr->type));
		} else
			snd_iprintf(buffer, "%3i:        : %s\n", minor,
				    snd_device_type_name(mptr->type));
	}
	mutex_unlock(&sound_mutex);
}

int __init snd_minor_info_init(void)
{
	struct snd_info_entry *entry;

	entry = snd_info_create_module_entry(THIS_MODULE, "devices", NULL);
	if (entry) {
		entry->c.text.read = snd_minor_info_read;
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	snd_minor_info_entry = entry;
	return 0;
}

int __exit snd_minor_info_done(void)
{
	snd_info_free_entry(snd_minor_info_entry);
	return 0;
}
#endif /* CONFIG_PROC_FS */

/*
 *  INIT PART
 */

#ifdef CONFIG_SND_DEBUG_MEMORY
extern void snd_memory_done(void);
#else
#define snd_memory_done()
#endif
#if defined(CONFIG_SND_DEBUG_MEMORY) && defined(CONFIG_PROC_FS)
extern int snd_memory_info_init(void);
extern void snd_memory_info_done(void);
#else
#define snd_memory_info_init()
#define snd_memory_info_done()
#endif

static int __init alsa_sound_init(void)
{
	snd_major = major;
	snd_ecards_limit = cards_limit;
#ifdef CONFIG_DEVFS_FS
	devfs_mk_dir("snd");
#endif
	if (register_chrdev(major, "alsa", &snd_fops)) {
		snd_printk(KERN_ERR "unable to register native major device number %d\n", major);
#ifdef CONFIG_DEVFS_FS
		devfs_remove("snd");
#endif
		return -EIO;
	}
	if (snd_info_init() < 0) {
		unregister_chrdev(major, "alsa");
		snd_memory_done();
		return -ENOMEM;
	}
	snd_memory_info_init();
	snd_info_minor_register();
#ifdef CONFIG_DEVFS_FS
	{
		short controlnum;
		for (controlnum = 0; controlnum < cards_limit; controlnum++)
			devfs_mk_cdev(MKDEV(major, controlnum<<5),
				      S_IFCHR | device_mode ,
				      "snd/controlC%d", controlnum);
	}
#endif
#ifndef MODULE
	printk(KERN_INFO "Advanced Linux Sound Architecture Driver Version " CONFIG_SND_VERSION CONFIG_SND_DATE ".\n");
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0) && defined(CONFIG_APM)
	pm_init();
#endif
	return 0;
}

static void __exit alsa_sound_exit(void)
{
#ifdef CONFIG_DEVFS_FS
	short controlnum;
	for (controlnum = 0; controlnum < cards_limit; controlnum++)
		devfs_remove("snd/controlC%d", controlnum);
#endif

	snd_info_minor_unregister();
	snd_memory_info_done();
	snd_info_done();
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0) && defined(CONFIG_APM)
	pm_done();
#endif
	unregister_chrdev(major, "alsa");
	snd_memory_done();
#ifdef CONFIG_DEVFS_FS
	devfs_remove("snd");
#endif
}

subsys_initcall(alsa_sound_init);
module_exit(alsa_sound_exit);

#include "sound.inc"
