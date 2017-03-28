/*
 *
 *  Copyright (C) 2007-2014 SimplyCore, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 */

#include "usbd.h"
#define IIIIIIIl (0x238+217-0x212)
static dev_t IlllIll=MKDEV((0x17f6+3240-0x249e),(0x695+7364-0x2359));static void
*IIIllII[IIIIIIIl];ssize_t lIIlIIIll(struct file*,char __user*,size_t,loff_t*);
ssize_t llIIlIIl(struct file*,const char __user*,size_t,loff_t*);long lIIlIIlII(
struct file*,unsigned int,unsigned long);long lIllllIl(struct file*,unsigned int
,unsigned long);int IIlIIIIl(struct inode*,struct file*);int llllIIll(struct 
inode*,struct file*);unsigned int IllllIll(struct file*IllIIl,poll_table*lllIII)
;int llIIIlIIl(struct file*IllIIl,struct vm_area_struct*IlIIIll);static struct 
file_operations llIIIIlll={.owner=THIS_MODULE,.read=lIIlIIIll,.write=llIIlIIl,.
poll=IllllIll,.unlocked_ioctl=lIIlIIlII,
#ifdef CONFIG_COMPAT
.compat_ioctl=lIllllIl,
#endif
.mmap=llIIIlIIl,.open=IIlIIIIl,.release=llllIIll,};int llIlllII(int*IlIlIIll){
int lIlll=(0x786+5985-0x1ee7);IIIll(
"\x69\x6e\x69\x74\x5f\x63\x64\x65\x76\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x2b\x2b" "\n"
);if(IlllIll==MKDEV((0xa6a+673-0xd0b),(0xf42+1743-0x1611))){lIlll=
alloc_chrdev_region(&IlllIll,(0x5bb+262-0x6c1),(0x1ec+4284-0x11a9),IllIIllI);if(
lIlll!=(0x1049+2768-0x1b19))IIIll(
"\x69\x6e\x69\x74\x5f\x63\x64\x65\x76\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x61\x6c\x6c\x6f\x63\x5f\x63\x68\x72\x64\x65\x76\x5f\x72\x65\x67\x69\x6f\x6e\x20\x66\x61\x69\x6c\x65\x64\x2e\x20\x45\x72\x72\x6f\x72\x20\x6e\x75\x6d\x62\x65\x72\x20\x25\x64" "\n"
,lIlll);IIIll(
"\x69\x6e\x69\x74\x5f\x63\x64\x65\x76\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x61\x6c\x6c\x6f\x63\x61\x74\x65\x64\x20\x64\x65\x76\x6e\x75\x6d\x20\x72\x65\x67\x69\x6f\x6e\x3a\x20\x4d\x61\x6a\x6f\x72\x20\x25\x64\x20\x4d\x69\x6e\x6f\x72\x20\x25\x64" "\n"
,MAJOR(IlllIll),MINOR(IlllIll));memset(IIIllII,(0x32b+8649-0x24f4),sizeof(
IIIllII));*IlIlIIll=MAJOR(IlllIll);}else{IIIll(
"\x69\x6e\x69\x74\x5f\x63\x64\x65\x76\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x61\x74\x74\x65\x6d\x70\x74\x20\x74\x6f\x20\x72\x65\x69\x6e\x69\x74\x20\x69\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65\x64\x20\x63\x64\x65\x76\x20\x6d\x6f\x64\x75\x6c\x65" "\n"
);}IIIll(
"\x69\x6e\x69\x74\x5f\x63\x64\x65\x76\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}int IllllIII(void){if(IlllIll!=MKDEV((0x5d7+652-0x863),
(0x18cf+1092-0x1d13))){unregister_chrdev_region(IlllIll,(0x6e0+5448-0x1b29));
IlllIll=MKDEV((0x25c+3170-0xebe),(0xb31+6095-0x2300));}else{IIIll(
"\x64\x65\x69\x6e\x69\x74\x5f\x63\x64\x65\x76\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x61\x74\x74\x65\x6d\x70\x74\x20\x74\x6f\x20\x64\x65\x69\x6e\x69\x74\x20\x75\x6e\x69\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65\x64\x20\x63\x64\x65\x76\x20\x6d\x6f\x64\x75\x6c\x65" "\n"
);}return(0x323+2848-0xe43);}
int lIIIlIlII(struct cdev*IlIllIIl){int lIlll;dev_t devnum;IIIll(
"\x63\x72\x65\x61\x74\x65\x5f\x63\x64\x65\x76\x3a\x20\x2b\x2b" "\n");cdev_init(
IlIllIIl,&llIIIIlll);devnum=MKDEV(MAJOR(IlllIll),(0x8f7+1677-0xf84));lIlll=
cdev_add(IlIllIIl,devnum,IIIIIIIl);if(lIlll!=(0x46c+2419-0xddf))IIIll(
"\x63\x72\x65\x61\x74\x65\x5f\x63\x64\x65\x76\x3a\x20\x63\x64\x65\x76\x5f\x61\x64\x64\x20\x66\x61\x69\x6c\x65\x64\x2e\x20\x45\x72\x72\x6f\x72\x20\x6e\x75\x6d\x62\x65\x72\x20\x25\x64" "\n"
,lIlll);IIIll(
"\x63\x72\x65\x61\x74\x65\x5f\x63\x64\x65\x76\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}int IIIIIIIll(struct cdev*IlIllIIl){IIIll(
"\x64\x65\x73\x74\x72\x6f\x79\x5f\x63\x64\x65\x76" "\n");cdev_del(IlIllIIl);
return(0xd88+1869-0x14d5);}int IIIllIIl(struct llIll*lIIIIllI){int i;for(i=
(0xf9a+401-0x112b);i<IIIIIIIl;i++){if(IIIllII[i]==NULL){IIIllII[i]=lIIIIllI;
break;}}if(i==IIIIIIIl)return(-(0x217b+719-0x2449));lIIIIllI->IlllIl=i;return
(0x1312+212-0x13e6);}dev_t lllIllIII(int IlllIl){return MKDEV(MAJOR(IlllIll),
IlllIl);}void*IllIllllI(int IlllIl){if(IlllIl<IIIIIIIl)return IIIllII[IlllIl];
return NULL;}void IIllIlII(int IlllIl){if(IlllIl<IIIIIIIl&&IIIllII[IlllIl]!=NULL
){IIIllII[IlllIl]=NULL;}}
ssize_t lIIlIIIll(struct file*IllIIl,char __user*lIIIllIl,size_t IlIII,loff_t*
IllIllIlI){u32 m=iminor(IllIIl->f_dentry->d_inode);struct llIll*IIlII;
IIlII=IIIllII[m];if(IIlII){if(IIlII->ops.read)return IIlII->ops.read(IIlII->
context,lIIIllIl,IlIII);else return(0xad6+2871-0x160d);}return-ENODEV;}ssize_t 
llIIlIIl(struct file*IllIIl,const char __user*lIIIllIl,size_t IlIII,loff_t*
IllIllIlI){u32 m=iminor(IllIIl->f_dentry->d_inode);struct llIll*IIlII;
IIlII=IIIllII[m];if(IIlII){if(IIlII->ops.write)return IIlII->ops.write(IIlII->
context,lIIIllIl,IlIII);else return(0x1b6b+1713-0x221c);}return-ENODEV;}unsigned
 int IllllIll(struct file*IllIIl,poll_table*lllIII){u32 m=iminor(IllIIl->
f_dentry->d_inode);struct llIll*IIlII;
IIlII=IIIllII[m];if(IIlII){if(IIlII->ops.poll)return IIlII->ops.poll(IIlII->
context,IllIIl,lllIII);else return(0x3a1+3149-0xfee);}return-ENODEV;}long 
lIIlIIlII(struct file*IllIIl,unsigned int IlIllI,unsigned long IllIll){u32 m=
iminor(IllIIl->f_dentry->d_inode);struct llIll*IIlII;
IIlII=IIIllII[m];if(IIlII){if(IIlII->ops.unlocked_ioctl)return IIlII->ops.
unlocked_ioctl(IIlII->context,IlIllI,IllIll);else return(0x2037+1697-0x26d8);}
return-ENODEV;}
#ifdef CONFIG_COMPAT
long lIllllIl(struct file*IllIIl,unsigned int IlIllI,unsigned long IllIll){u32 m
=iminor(IllIIl->f_dentry->d_inode);struct llIll*IIlII;
IIlII=IIIllII[m];if(IIlII){if(IIlII->ops.compat_ioctl)return IIlII->ops.
compat_ioctl(IIlII->context,IlIllI,IllIll);else return(0x75+579-0x2b8);}return-
ENODEV;}
#endif
int llIIIlIIl(struct file*IllIIl,struct vm_area_struct*IlIIIll){u32 m=iminor(
IllIIl->f_dentry->d_inode);struct llIll*IIlII;IIlII=IIIllII[m];if(IIlII&&IIlII->
ops.mmap){if(IIlII->ops.mmap)return IIlII->ops.mmap(IIlII->context,IlIIIll);else
 return-EINVAL;}return-ENODEV;}int IIlIIIIl(struct inode*inode,struct file*
IllIIl){u32 m=iminor(inode);struct llIll*IIlII;
IllIIl->private_data=inode->i_cdev;IIlII=IIIllII[m];if(IIlII){if(IIlII->ops.open
)return IIlII->ops.open(IIlII->context);else return(0x16cf+2598-0x20f5);}return-
ENODEV;}int llllIIll(struct inode*inode,struct file*IllIIl){u32 m=iminor(inode);
struct llIll*IIlII;
IIlII=IIIllII[m];if(IIlII){if(IIlII->ops.release)return IIlII->ops.release(IIlII
->context);else return(0x15b5+1901-0x1d22);}return-ENODEV;}
