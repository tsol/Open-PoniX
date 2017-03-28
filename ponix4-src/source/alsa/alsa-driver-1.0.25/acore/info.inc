#include "adriver.h"
#include "alsa-autoconf.h"
#define __NO_VERSION__

#include <linux/proc_fs.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 0)
#include <sound/core.h>
#include <sound/info.h>
#include <sound/version.h>

static int snd_info_card_readlink(struct dentry *dentry,
				  char *buffer, int buflen)
{
        char *s = PDE(dentry->d_inode)->data;
	int len;
	
	if (s == NULL)
		return -EIO;
	len = strlen(s);
	if (len > buflen)
		len = buflen;
	if (copy_to_user(buffer, s, len))
		return -EFAULT;
	return len;
}

static struct dentry *snd_info_card_followlink(struct dentry *dentry,
					       struct dentry *base,
					       unsigned int follow)
{
	char *s = PDE(dentry->d_inode)->data;
	return lookup_dentry(s, base, follow);
}

static struct file_operations snd_info_card_link_operations =
{
	NULL
};

struct inode_operations snd_info_card_link_inode_operations =
{
	.default_file_ops =	&snd_info_card_link_operations,
	.readlink =		snd_info_card_readlink,
	.follow_link =		snd_info_card_followlink,
};

static struct proc_dir_entry *proc_symlink(const char *name,
					   struct proc_dir_entry *parent,
					   const char *dest)
{
	char *s = kstrdup(dest, GFP_KERNEL);
	struct proc_dir_entry *p;

	if (s == NULL)
		return NULL;
	p = create_proc_entry(name, S_IFLNK | S_IRUGO | S_IWUGO | S_IXUGO, parent);
 	if (p == NULL) {
		kfree(s);
 		return NULL;
	}
	p->data = s;
	p->ops = &snd_info_card_link_inode_operations;
	return p;
}

static void snd_info_fill_inode(struct inode *inode, int fill)
{
	if (fill)
		MOD_INC_USE_COUNT;
	else
		MOD_DEC_USE_COUNT;
}

static inline void snd_info_entry_prepare(struct proc_dir_entry *de)
{
	de->fill_inode = snd_info_fill_inode;
}

void snd_remove_proc_entry(struct proc_dir_entry *parent,
			   struct proc_dir_entry *de)
{
	if (parent && de) {
		if (S_ISLNK(de->mode) && de->data) {
			kfree(de->data);
			de->data = 0;
		}
		proc_unregister(parent, de->low_ino);
	}
}

#else /* LINUX_VERSION_CODE < 2.4.0 */

static void snd_remove_proc_entry(struct proc_dir_entry *parent,
                                  struct proc_dir_entry *de)
{
	if (de)
		remove_proc_entry(de->name, parent);
}

#endif

#ifdef HAVE_PROCFS_OWNER
static inline void snd_info_entry_prepare(struct proc_dir_entry *de)
{
	de->owner = THIS_MODULE;
}

/**
 * snd_create_proc_entry - create a procfs entry
 * @name: the name of the proc file
 * @mode: the file permission bits, S_Ixxx
 * @parent: the parent proc-directory entry
 *
 * Creates a new proc file entry with the given name and permission
 * on the given directory.
 *
 * Returns the pointer of new instance or NULL on failure.
 */
static struct proc_dir_entry *
	snd_create_proc_entry(const char *name, mode_t mode,
			      struct proc_dir_entry *parent)
{
	struct proc_dir_entry *p;
	p = create_proc_entry(name, mode, parent);
	if (p)
		snd_info_entry_prepare(p);
	return p;
}
static void snd_remove_proc_entry(struct proc_dir_entry *parent,
                                  struct proc_dir_entry *de)
{
        if (de)
                remove_proc_entry(de->name, parent);
}
#else
static inline struct proc_dir_entry *
	snd_create_proc_entry(const char *name, mode_t mode,
			      struct proc_dir_entry *parent)
{
	return create_proc_entry(name, mode, parent);
}
#endif /* HAVE_PROCFS_OWNER */
