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

#ifdef _USBD_ENABLE_VHCI_
#include "usbd.h"
ssize_t IlIIlllI(void*,const char __user*,size_t);ssize_t IIlIllIlI(void*,char 
__user*,size_t);long IlllIlIl(void*,unsigned int,unsigned long);long lllIIllIl(
void*,unsigned int,unsigned long);int IlIlIllll(void*);int lIIlIllI(void*);
unsigned int IlllllIIl(void*,struct file*,poll_table*lllIII);int llllIlIl(void*,
struct vm_area_struct*);struct llIll*lIlIlIIIl(struct IIIII*llllI){struct llIll*
lllll;lllll=kmalloc(sizeof(*lllll),GFP_KERNEL);if(lllll){memset(lllll,
(0x1496+2145-0x1cf7),sizeof(*lllll));lllll->ops.open=IlIlIllll;lllll->ops.
release=lIIlIllI;lllll->ops.unlocked_ioctl=IlllIlIl;
#ifdef CONFIG_COMPAT
lllll->ops.compat_ioctl=lllIIllIl;
#endif
lllll->ops.read=IIlIllIlI;lllll->ops.write=IlIIlllI;lllll->ops.poll=IlllllIIl;
lllll->ops.mmap=llllIlIl;lllll->context=llllI;lllll->llIllI=IlIllIII();if(!lllll
->llIllI){kfree(lllll);lllll=NULL;}}return lllll;}void llIllIII(struct llIll*
lllll){if(lllll){if(lllll->llIllI){llIlIIlI(lllll->llIllI);lllll->llIllI=NULL;}
kfree(lllll);}}ssize_t IIlIllIlI(void*context,char __user*IlIIl,size_t IlIII){
IIIll("\x75\x73\x62\x64\x5f\x6d\x76\x5f\x72\x65\x61\x64\x3a" "\n");return
(0xd22+1868-0x146e);}ssize_t IlIIlllI(void*context,const char __user*IlIIl,
size_t IlIII){IIIll("\x75\x73\x62\x64\x5f\x6d\x76\x5f\x77\x72\x69\x74\x65" "\n")
;return(0xd6c+734-0x104a);}long IlllIlIl(void*context,unsigned int IlIllI,
unsigned long IllIll){struct IIIII*llllI=context;int lIlll=(0xe50+934-0x11f6);
unsigned long flags;struct lIlIII*lIIll;struct lIlIII*llIIIIlI;llIII IIlIl;IIIll
(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x2b\x2b\x20\x63\x6d\x64\x3d\x25\x64\x20\x61\x72\x67\x3d\x25\x6c\x75" "\n"
,IlIllI,IllIll);switch(IlIllI){case(0x97a+4381-0x1a97):if(llllI->llllllll){IIIll
(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x72\x65\x73\x65\x74\x20\x69\x6e\x20\x70\x72\x6f\x67\x72\x65\x73\x73" "\n"
);lIlll=-ENODATA;break;}IIlIl=(llIII)((char*)llllI->IIIllIllI+IllIll);IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x72\x65\x61\x64\x20\x75\x6e\x72\x62" "\n"
);lIIll=NULL;
spin_lock_irqsave(&llllI->IIllII,flags);if(!list_empty(&llllI->llIIIIl)){lIIll=
list_entry(llllI->llIIIIl.next,struct lIlIII,lIIIl);list_del_init(&lIIll->lIIIl)
;}spin_unlock_irqrestore(&llllI->IIllII,flags);IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x70\x71\x64\x61\x74\x61\x20\x3d\x20\x25\x70" "\n"
,lIIll);if(lIIll==NULL){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x6e\x6f\x20\x65\x6e\x74\x72\x79" "\n"
);lIlll=-ENODATA;break;}lIlll=IlIIllIll(llllI,lIIll,IIlIl);break;case
(0xf45+4561-0x2115):IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x77\x72\x69\x74\x65\x20\x75\x6e\x72\x62" "\n"
);IIlIl=(llIII)((char*)llllI->IIIIlIIl+IllIll);IIllIII(IIlIl);lIIll=NULL;
spin_lock_irqsave(&llllI->lIlIlll,flags);list_for_each_entry(llIIIIlI,&llllI->
IlIIlll,lIIIl){if(llIIIIlI->IlIIII==IIlIl->IIIlI.IIIIll){
list_del_init(&llIIIIlI->lIIIl);
lIIll=llIIIIlI;break;}}spin_unlock_irqrestore(&llllI->lIlIlll,flags);if(!lIIll){
IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x75\x72\x62\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
);break;}lIlll=IIIIIIll(llllI,lIIll,IIlIl);break;default:IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x69\x6f\x63\x74\x6c" "\n"
);lIlll=-EINVAL;break;}IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}
#ifdef CONFIG_COMPAT
long lllIIllIl(void*context,unsigned int IlIllI,unsigned long IllIll){return 
IlllIlIl(context,IlIllI,IllIll);}
#endif
int llllIlIl(void*context,struct vm_area_struct*IlIIIll){int lIlll;struct IIIII*
llllI=context;unsigned long IIIIII;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x6d\x6d\x61\x70\x3a\x20\x2b\x2b" "\n");IIIIII=
IlIIIll->vm_end-IlIIIll->vm_start;if((IIIIII<IlIllll*(0x2c4+1697-0x963))||(
IIIIII>IlIllll*(0x69d+6901-0x218f))){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x6d\x6d\x61\x70\x3a\x20\x2d\x2d\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x6c\x65\x6e\x67\x74\x68" "\n"
);return-EINVAL;}
lIlll=IlIIIIIII(llllI->IIllllI,IlIIIll);IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x6d\x6d\x61\x70\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}int IlIlIllll(void*context){int lIlll=(0x928+3543-0x16ff);
struct IIIII*llllI=context;unsigned char*IIllllI;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x6f\x70\x65\x6e\x3a\x20\x2b\x2b" "\n");
lIIIllII(llllI);IIllllI=vmalloc(IlIllll*(0x1b1+1821-0x8cc));if(IIllllI==NULL){
IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x6f\x70\x65\x6e\x3a\x20\x6e\x6f\x20\x6d\x65\x6d\x6f\x72\x79\x20\x66\x6f\x72\x20\x69\x6f\x62\x75\x66" "\n"
);lIlll=-ENOMEM;}else{llllI->IIllllI=IIllllI;llllI->IIIllIllI=IIllllI+
(0x17d+2-0x17f);llllI->IIIIlIIl=IIllllI+IlIllll;}IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x6f\x70\x65\x6e\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}int lIIlIllI(void*context){struct IIIII*llllI=context;
IIIll("\x75\x73\x62\x64\x5f\x6d\x76\x5f\x72\x65\x6c\x65\x61\x73\x65" "\n");if(
llllI->IIllllI){vfree(llllI->IIllllI);llllI->IIllllI=NULL;}IIIIllI(llllI);return
(0xb43+1500-0x111f);}unsigned int IlllllIIl(void*context,struct file*IllIIl,
poll_table*lllIII){int lllllIIl;unsigned long flags;struct IIIII*llllI=context;
poll_wait(IllIIl,&llllI->lllIII,lllIII);spin_lock_irqsave(&llllI->IIllII,flags);
lllllIIl=list_empty(&llllI->llIIIIl);spin_unlock_irqrestore(&llllI->IIllII,flags
);if(!lllllIIl&&!llllI->llllllll){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x76\x5f\x70\x6f\x6c\x6c\x3a\x20\x71\x75\x65\x75\x65\x20\x6e\x6f\x74\x20\x65\x6d\x70\x74\x79" "\n"
);return((POLLOUT|POLLWRNORM)|(POLLIN|POLLRDNORM));}return(POLLOUT|POLLWRNORM);}
#endif 

