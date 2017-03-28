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

#ifdef _USBD_ENABLE_STUB_
#include "usbd.h"
int IlIlIllI(void*);int lIIIIlIlI(void*);unsigned int lllllIlIl(void*,struct 
file*,poll_table*lllIII);long lIllIIII(void*context,unsigned int IlIllI,unsigned
 long IllIll);long lIlIIIII(void*context,unsigned int IlIllI,unsigned long 
IllIll);int IllIllll(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,
struct IIIIl*lllIlII);int lIIllIIII(struct IllII*IlIlI,struct usb_device*lIIII,
llIII IIlIl);int lllIlllll(struct IllII*IlIlI,struct usb_device*lIIII,llIII 
IIlIl);int IllIIIlI(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,
struct IIIIl*lllIlII);int lIIIlIIl(struct IllII*IlIlI,struct usb_device*lIIII,
llIII IIlIl,struct IIIIl*lllIlII,gfp_t lIIllI);int llIlIIlIl(struct IllII*IlIlI,
struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lllIlII,gfp_t lIIllI);int 
llIlllIlI(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*
lllIlII,gfp_t lIIllI);int IlIlIIII(struct IllII*IlIlI,struct usb_device*lIIII,
llIII IIlIl,struct IIIIl*lllIlII);int IllllllIl(struct IllII*IlIlI,struct 
usb_device*lIIII,llIII IIlIl,struct IIIIl*lllIlII);int IIIllllIl(struct IllII*
IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lllIlII);int IlIlllIlI(
struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lllIlII);int
 IlIIIIlll(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl);int IIIIlIlII
(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl);int IIIIIIlII(struct 
IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl);int lIlllIIIl(struct IllII*
IlIlI,struct usb_device*lIIII,llIII IIlIl);int IlIllIIII(struct IllII*IlIlI,
struct usb_device*lIIII,llIII IIlIl);int IlIllllII(struct IllII*IlIlI,struct 
usb_device*lIIII,llIII IIlIl);
#if KERNEL_LT((0x22+552-0x248),(0x631+4505-0x17c4),(0xb11+877-0xe6b))
void IlIlIlII(struct urb*lIlII,struct pt_regs*IIIllIl);void lIIlIlll(struct urb*
lIlII,struct pt_regs*IIIllIl);void IlIIIIII(struct urb*lIlII,struct pt_regs*
IIIllIl);void llIIllII(struct urb*lIlII,struct pt_regs*IIIllIl);void llllIIIl(
struct urb*lIlII,struct pt_regs*IIIllIl);
#else
void IlIlIlII(struct urb*lIlII);void lIIlIlll(struct urb*lIlII);void IlIIIIII(
struct urb*lIlII);void llIIllII(struct urb*lIlII);void llllIIIl(struct urb*lIlII
);
#endif
void IlIIIlllI(struct lllIlI*lIllI);void IlIIlIlI(struct lllIlI*lIllI);void 
lIIlIl(struct kref*IlllIIl);int IIIllllI(struct IllII*IlIlI,struct usb_device*
lIlIIl);int llIlIlII(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,
struct IIIIl*lllIlII);void llIlIlIl(struct IllII*IlIlI);void IIlIlllI(struct 
IllII*IlIlI);struct llIll*IIlIIllI(struct IllII*IlIlI){struct llIll*IIlII;IIlII=
kmalloc(sizeof(*IIlII),GFP_KERNEL);if(IIlII){memset(IIlII,(0x148+4020-0x10fc),
sizeof(*IIlII));IIlII->ops.open=IlIlIllI;IIlII->ops.release=lIIIIlIlI;IIlII->ops
.poll=lllllIlIl;IIlII->ops.unlocked_ioctl=lIllIIII;
#ifdef CONFIG_COMPAT
IIlII->ops.compat_ioctl=lIlIIIII;
#endif
IIlII->context=IlIlI;IIlII->llIllI=IlIllIII();if(!IIlII->llIllI){kfree(IIlII);
IIlII=NULL;}}return IIlII;}void lIIIIIIlI(struct llIll*IIlII){if(IIlII){if(IIlII
->llIllI){llIlIIlI(IIlII->llIllI);IIlII->llIllI=NULL;}kfree(IIlII);}}int 
lIlIlllIl(struct IllII*IlIlI,void __user*IlIIl){ssize_t lIlll=(0x334+3167-0xf93)
;unsigned long flags;struct IIIIl*lIIlI;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x72\x65\x61\x64\x5f\x75\x6e\x72\x62\x3a\x20\x2b\x2b" "\n"
);lIIlI=NULL;spin_lock_irqsave(&IlIlI->lIlIIll,flags);if(!list_empty(&IlIlI->
IlllIlI)){lIIlI=list_entry(IlIlI->IlllIlI.next,struct IIIIl,lIIIl);list_del_init
(&lIIlI->lIIIl);}spin_unlock_irqrestore(&IlIlI->lIlIIll,flags);if(lIIlI){
lllllIIII(lIIlI,IlIIl,IlIllll);lllllll(lIIlI);}else{IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x72\x65\x61\x64\x5f\x75\x6e\x72\x62\x3a\x20\x6e\x6f\x20\x6d\x6f\x72\x65\x20\x64\x61\x74\x61" "\n"
);lIlll=-ENODATA;}IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x72\x65\x61\x64\x5f\x75\x6e\x72\x62\x3a\x20\x2d\x2d\x20\x72\x65\x74\x75\x72\x6e\x69\x6e\x67\x20\x25\x6c\x75" "\n"
,(unsigned long)lIlll);return lIlll;}int IIllIlllI(struct IllII*IlIlI,void 
__user*IlIIl){int lIlll=(0xe0b+1909-0x1580);llIII IIlIl=NULL;struct IIIIl*lIIlI;
struct usb_device*lIIII;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x77\x72\x69\x74\x65\x5f\x75\x6e\x72\x62\x3a\x20\x2b\x2b" "\n"
);if((lIIII=lIllIIlI(IlIlI))==NULL){IIIIlll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x77\x72\x69\x74\x65\x3a\x20\x2d\x2d\x20\x6e\x6f\x20\x64\x65\x76\x69\x63\x65" "\n"
);return-ENODEV;}do{lIIlI=lIlIlIlI(IlIlI,lIIII,IlIIl,IlIllll);if(!lIIlI){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x77\x72\x69\x74\x65\x5f\x75\x6e\x72\x62\x3a\x20\x63\x61\x6e\x6e\x6f\x74\x20\x61\x6c\x6c\x6f\x63\x61\x74\x65\x20\x55\x4e\x52\x42" "\n"
);lIlll=-ENOMEM;break;}IIlIl=(llIII)(lIIlI+(0x1322+1078-0x1757));IIllIII(IIlIl);






switch(IIlIl->IIIlI.lIlllI){case llIIIlII:lIlll=IllIllll(IlIlI,lIIII,IIlIl,lIIlI
);break;case lIIIlIlI:lIlll=lIIllIIII(IlIlI,lIIII,IIlIl);break;case Illlllll:
lIlll=lllIlllll(IlIlI,lIIII,IIlIl);break;case IIlIllll:lIlll=IllIIIlI(IlIlI,
lIIII,IIlIl,lIIlI);break;case IIIlllII:lIlll=llIlIlII(IlIlI,lIIII,IIlIl,lIIlI);
if(lIlll<(0xb86+5541-0x212b))lIlll=lIIIlIIl(IlIlI,lIIII,IIlIl,lIIlI,GFP_KERNEL);
break;case lIllllII:lIlll=IlIlIIII(IlIlI,lIIII,IIlIl,lIIlI);break;case IlIlIlI:
lIlll=IllllllIl(IlIlI,lIIII,IIlIl,lIIlI);break;case IlllIIIl:lIlll=IlIIIIlll(
IlIlI,lIIII,IIlIl);break;case IlIIllll:lIlll=IIIIlIlII(IlIlI,lIIII,IIlIl);break;
case lllllIllI:lIlll=IIIIIIlII(IlIlI,lIIII,IIlIl);break;case llllIlll:lIlll=
lIlllIIIl(IlIlI,lIIII,IIlIl);break;case IIIlIIII:
lIlll=IlIllIIII(IlIlI,lIIII,IIlIl);break;case IlIllllI:lIlll=IlIllllII(IlIlI,
lIIII,IIlIl);break;default:lIlll=-EINVAL;break;}}while((0x13c1+4827-0x269c));if(
lIIlI){if(lIlll<(0x16fd+664-0x1995)){
lllllll(lIIlI);}else if(lIlll>(0xdec+1578-0x1416)){unsigned long flags;IIIll(
"\x55\x4e\x52\x42\x20\x6e\x6f\x74\x20\x70\x65\x6e\x64\x69\x6e\x67" "\n");IIllIII
(IIlIl);
spin_lock_irqsave(&IlIlI->lIlIIll,flags);list_add_tail(&lIIlI->lIIIl,&IlIlI->
IlllIlI);spin_unlock_irqrestore(&IlIlI->lIlIIll,flags);wake_up(&IlIlI->lllIII);}
}if(lIlll>(0x1a6+8182-0x219c))lIlll=(0x956+593-0xba7);IIlllllI(IlIlI,lIIII);
IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x77\x72\x69\x74\x65\x5f\x75\x6e\x72\x62\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x6c\x75" "\n"
,(unsigned long)lIlll);return lIlll;}long IIllIlll(void*context,unsigned int 
IlIllI,void __user*IllIll){struct IllII*IlIlI=context;ssize_t lIlll=
(0x11a5+2720-0x1c45);IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x2b\x2b\x20\x63\x6d\x64\x3d\x25\x64\x20\x61\x72\x67\x3d\x25\x70" "\n"
,IlIllI,IllIll);switch(IlIllI){case IIlIIllll:lIlll=lIlIlllIl(IlIlI,IllIll);
break;case lIIIIIIIl:lIlll=IIllIlllI(IlIlI,IllIll);break;default:IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x72\x65\x71\x75\x65\x73\x74" "\n"
);lIlll=-EINVAL;break;}IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x69\x6f\x63\x74\x6c\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x6c\x75" "\n"
,(unsigned long)lIlll);return lIlll;}long lIllIIII(void*context,unsigned int 
IlIllI,unsigned long IllIll){return IIllIlll(context,IlIllI,(void __user*)IllIll
);}
#ifdef CONFIG_COMPAT
long lIlIIIII(void*context,unsigned int IlIllI,unsigned long IllIll){return 
IIllIlll(context,IlIllI,compat_ptr(IllIll));}
#endif
int IlIlIllI(void*context){int lIlll=(0x18a6+2445-0x2233);struct IllII*IlIlI=
context;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x6f\x70\x65\x6e\x3a\x20\x2b\x2b" "\n");IIlIIII
(IlIlI);IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x6f\x70\x65\x6e\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}int lIIIIlIlI(void*context){struct IllII*IlIlI=context;
IIIll("\x75\x73\x62\x64\x5f\x6d\x64\x5f\x72\x65\x6c\x65\x61\x73\x65" "\n");
llIllII(IlIlI);return(0xd11+2719-0x17b0);}unsigned int lllllIlIl(void*context,
struct file*IllIIl,poll_table*lllIII){struct IllII*IlIlI=context;int lllllIIl;
unsigned long flags;
poll_wait(IllIIl,&IlIlI->lllIII,lllIII);spin_lock_irqsave(&IlIlI->lIlIIll,flags)
;lllllIIl=list_empty(&IlIlI->IlllIlI);spin_unlock_irqrestore(&IlIlI->lIlIIll,
flags);if(!lllllIIl){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x70\x6f\x6c\x6c\x3a\x20\x71\x75\x65\x75\x65\x20\x6e\x6f\x74\x20\x65\x6d\x70\x74\x79" "\n"
);return((POLLOUT|POLLWRNORM)|(POLLIN|POLLRDNORM));}return(POLLOUT|POLLWRNORM);}
int IllIllll(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl
*lIIlI){int lIlll=(0x1a05+2286-0x22f3);unsigned long flags;struct urb*lIlII=NULL
;struct usb_ctrlrequest*lIIIlI=NULL;do{lIlII=usb_alloc_urb((0xeed+3305-0x1bd6),
GFP_KERNEL);if(!lIlII){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x67\x65\x74\x64\x65\x73\x63\x72\x69\x70\x74\x6f\x72\x3a\x20\x75\x73\x62\x5f\x61\x6c\x6c\x6f\x63\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIlll=-ENOMEM;break;}lIIIlI=kmalloc(sizeof(*lIIIlI),GFP_KERNEL);if(!lIIIlI){
IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x67\x65\x74\x64\x65\x73\x63\x72\x69\x70\x74\x6f\x72\x3a\x20\x75\x73\x62\x5f\x61\x6c\x6c\x6f\x63\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIlll=-ENOMEM;break;}lIIIlI->bRequest=USB_REQ_GET_DESCRIPTOR;lIIIlI->
bRequestType=USB_DIR_IN+((IIlIl->lllllI.llIIlll&(0x166a+3335-0x236e))<<
(0x855+6877-0x232d))+(IIlIl->lllllI.IIlllIlI&(0x325+4592-0x14f6));lIIIlI->wValue
=cpu_to_le16((IIlIl->lllllI.lIIlIlII<<(0x51d+4154-0x154f))+IIlIl->lllllI.
llIIllll);lIIIlI->wIndex=cpu_to_le16(IIlIl->lllllI.llIllIIl);lIIIlI->wLength=
cpu_to_le16(IIlIl->lllllI.llIlI);usb_fill_control_urb(lIlII,lIIII,
usb_rcvctrlpipe(lIIII,(0x372+7969-0x2293)),(unsigned char*)lIIIlI,lIIlI->lllII,
IIlIl->lllllI.llIlI,IlIlIlII,lIIlI);IIlIIII(IlIlI);lIIlI->lIlII=lIlII;lIIlI->
IlIlI=IlIlI;lIIlI->endpoint=(0xe92+2840-0x19aa);kref_init(&lIIlI->IlIlIl);
spin_lock_irqsave(&IlIlI->IIllI,flags);list_add_tail(&lIIlI->lIIIl,&IlIlI->
lllIll);spin_unlock_irqrestore(&IlIlI->IIllI,flags);lIlll=usb_submit_urb(lIlII,
GFP_KERNEL);if(lIlll<(0x10c8+1869-0x1815)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x67\x65\x74\x64\x65\x73\x63\x72\x69\x70\x74\x6f\x72\x3a\x20\x75\x73\x62\x5f\x73\x75\x62\x6d\x69\x74\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64" "\n"
,lIlll);spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->lllllI.llIlI=(0x22cd+155-0x2368);IIlIl->IIIlI.lIlIl=sizeof(IIlIl->lllllI)
+IIlIl->lllllI.llIlI;IIlIl->IIIlI.Status=lIlll;lIlll=(0x19a9+1950-0x2147);
kref_put(&lIIlI->IlIlIl,lIIlIl);}}while((0xa26+5110-0x1e1c));if(lIlll<
(0x1490+2027-0x1c7b)){if(lIlII)usb_free_urb(lIlII);if(lIIIlI)kfree(lIIIlI);}
return lIlll;}int lIIllIIII(struct IllII*IlIlI,struct usb_device*lIIII,llIII 
IIlIl){int i,lIlll;IIlIl->IIIlI.Status=(0x34+3430-0xd9a);
IIlIlllI(IlIlI);if(IIlIl->IIIIIII.llIlIIIl==(0x99a+5881-0x2093))return
(0x6db+4875-0x19e5);lIlll=lIIllIll(IlIlI,lIIII,IIlIl->IIIIIII.llIlIIIl,15000);if
(lIlll==-ERESTART){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x63\x68\x61\x6e\x67\x65\x5f\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x20\x69\x6e\x74\x65\x72\x72\x75\x70\x74\x65\x64" "\n"
);return-ERESTART;}if(lIlll<(0x206f+27-0x208a)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x63\x68\x61\x6e\x67\x65\x5f\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x20\x72\x65\x74\x75\x72\x6e\x65\x64\x20\x25\x64" "\n"
,lIlll);IIlIl->IIIlI.Status=-(0x947+94-0x9a4);return(0x779+4786-0x1a2a);}
lIIII=lIllIIlI(IlIlI);if(lIIII==NULL){IIIIlll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x64\x65\x76\x69\x63\x65\x20\x69\x73\x20\x6e\x75\x6c\x6c\x20\x61\x66\x74\x65\x72\x20\x63\x68\x61\x6e\x67\x69\x6e\x67\x20\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x2e" "\n"
);return(0x528+1919-0xca6);}
IIIllllI(IlIlI,lIIII);for(i=(0x1ab1+173-0x1b5e);i<IIlIl->IIIIIII.llllIIII;i++){
struct usb_interface*IIIllI;struct usb_host_interface*lIIlIlI;IIIllI=
usb_ifnum_to_if(lIIII,IIlIl->IIIIIII.lIllllI[i].IIIlIIl);if(!IIIllI){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x25\x64\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
,IIlIl->IIIIIII.lIllllI[i].IIIlIIl);continue;}lIIlIlI=usb_altnum_to_altsetting(
IIIllI,IIlIl->IIIIIII.lIllllI[i].lllIIIl);if(!lIIlIlI){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x25\x64\x20\x61\x6c\x74\x73\x65\x74\x74\x69\x6e\x67\x20\x25\x64\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
,IIlIl->IIIIIII.lIllllI[i].IIIlIIl,IIlIl->IIIIIII.lIllllI[i].lllIIIl);continue;}
if(IIIllI->cur_altsetting){if(IIIllI->num_altsetting==(0x680+2653-0x10dc)){IIIll
(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x25\x64\x20\x68\x61\x73\x20\x6f\x6e\x6c\x79\x20\x6f\x6e\x65\x20\x61\x6c\x74\x73\x65\x74\x74\x69\x6e\x67" "\n"
,IIlIl->IIIIIII.lIllllI[i].IIIlIIl);continue;}if(lIIlIlI->desc.bAlternateSetting
==IIIllI->cur_altsetting->desc.bAlternateSetting){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x25\x64\x20\x63\x75\x72\x72\x65\x6e\x74\x20\x61\x6c\x74\x73\x65\x74\x74\x69\x6e\x67\x20\x61\x6c\x72\x65\x61\x64\x79\x20\x73\x65\x74" "\n"
,IIlIl->IIIIIII.lIllllI[i].IIIlIIl);continue;}}lIlll=usb_set_interface(lIIII,
IIlIl->IIIIIII.lIllllI[i].IIIlIIl,IIlIl->IIIIIII.lIllllI[i].lllIIIl);if(lIlll!=
(0x102a+5569-0x25eb)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x75\x73\x62\x5f\x73\x65\x74\x5f\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64" "\n"
,lIlll);}if(IIIllI->cur_altsetting){int IlIIIl;int pipe;for(IlIIIl=
(0x256+1459-0x809);IlIIIl<IIIllI->cur_altsetting->desc.bNumEndpoints;IlIIIl++){
if(IIIllI->cur_altsetting->endpoint[IlIIIl].desc.bEndpointAddress&
(0xeed+1591-0x14a4))pipe=usb_rcvisocpipe(lIIII,IIIllI->cur_altsetting->endpoint[
IlIIIl].desc.bEndpointAddress&(0x170c+1707-0x1da8));else pipe=usb_sndisocpipe(
lIIII,IIIllI->cur_altsetting->endpoint[IlIIIl].desc.bEndpointAddress&
(0x9c+845-0x3da));lIIlIIIl(IlIlI,pipe,(0x13b2+2103-0x1be9));}}}IIlllllI(IlIlI,
lIIII);return(0x20c+3183-0xe7a);
}int IlIIIIlll(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl){
#if KERNEL_GT_EQ((0xf5+6339-0x19b6),(0x18e7+1663-0x1f60),(0x28+5976-0x1776))
int pipe=(0xdd9+4528-0x1f89),lIllIII;struct usb_host_endpoint*ep;ep=(IIlIl->
IlIIIII.Flags&llllII)?lIIII->ep_in[IIlIl->IlIIIII.Endpoint]:lIIII->ep_out[IIlIl
->IlIIIII.Endpoint];if(ep){switch(ep->desc.bmAttributes&
USB_ENDPOINT_XFERTYPE_MASK){case USB_ENDPOINT_XFER_ISOC:pipe=(IIlIl->IlIIIII.
Flags&llllII)?usb_rcvisocpipe(lIIII,IIlIl->IlIIIII.Endpoint):usb_sndisocpipe(
lIIII,IIlIl->IlIIIII.Endpoint);break;case USB_ENDPOINT_XFER_BULK:pipe=(IIlIl->
IlIIIII.Flags&llllII)?usb_rcvbulkpipe(lIIII,IIlIl->IlIIIII.Endpoint):
usb_sndbulkpipe(lIIII,IIlIl->IlIIIII.Endpoint);break;case USB_ENDPOINT_XFER_INT:
pipe=(IIlIl->IlIIIII.Flags&llllII)?usb_rcvintpipe(lIIII,IIlIl->IlIIIII.Endpoint)
:usb_sndintpipe(lIIII,IIlIl->IlIIIII.Endpoint);break;case 
USB_ENDPOINT_XFER_CONTROL:default:break;}}
#endif
if(ep&&(ep->desc.bmAttributes&USB_ENDPOINT_XFERTYPE_MASK)==
USB_ENDPOINT_XFER_ISOC){lIIlIIIl(IlIlI,pipe,(0x18f+3969-0x1110));lIllIII=
usb_pipeendpoint(pipe);
#if KERNEL_GT_EQ((0x1a09+3068-0x2603),(0x126a+2578-0x1c76),(0x2273+132-0x22d9))
if(usb_pipein(pipe))lIllIII|=USB_DIR_IN;usb_reset_endpoint(lIIII,lIllIII);
#else
usb_settoggle(lIIII,lIllIII,usb_pipeout(pipe),(0x332+6559-0x1cd1));
#endif
IIlIl->IIIlI.Status=(0x872+59-0x8ad);}else IIlIl->IIIlI.Status=usb_clear_halt(
lIIII,pipe);if(IIlIl->IIIlI.Status!=(0x182+746-0x46c)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x63\x6c\x65\x61\x72\x73\x74\x61\x6c\x6c\x3a\x20\x75\x73\x62\x5f\x63\x6c\x65\x61\x72\x5f\x68\x61\x6c\x74\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64" "\n"
,IIlIl->IIIlI.Status);}return(0xc71+4408-0x1da8);}int IIIIlIlII(struct IllII*
IlIlI,struct usb_device*lIIII,llIII IIlIl){IIlIl->IIIlI.Status=
(0x73b+3449-0x14b4);IIlIl->lIlIIlll.IIlIIIlII=usb_get_current_frame_number(lIIII
);return(0x554+2914-0x10b5);}int lllIlllll(struct IllII*IlIlI,struct usb_device*
lIIII,llIII IIlIl){struct usb_interface*IIIllI;struct usb_host_interface*lIIlIlI
;IIlIl->IIIlI.Status=(0x1aa3+275-0x1bb6);if(IlIlI->lIIlIIl&&
(IlIlI->IlIlllll==IIlIl->IIIIIll.IIIlIIl)){IIlIlllI(IlIlI);}do{IIIllI=
usb_ifnum_to_if(lIIII,IIlIl->IIIIIll.IIIlIIl);if(!IIIllI){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x3a\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x25\x64\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
,IIlIl->IIIIIll.lllIIIl);break;}lIIlIlI=usb_altnum_to_altsetting(IIIllI,IIlIl->
IIIIIll.lllIIIl);if(!lIIlIlI){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x3a\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x25\x64\x20\x61\x6c\x74\x73\x65\x74\x74\x69\x6e\x67\x20\x25\x64\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
,IIlIl->IIIIIll.IIIlIIl,IIlIl->IIIIIll.lllIIIl);break;}if(IIIllI->cur_altsetting
){if(IIIllI->num_altsetting==(0xc99+5003-0x2023)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x3a\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x25\x64\x20\x68\x61\x73\x20\x6f\x6e\x6c\x79\x20\x6f\x6e\x65\x20\x61\x6c\x74\x73\x65\x74\x74\x69\x6e\x67" "\n"
,IIlIl->IIIIIll.IIIlIIl);break;}if(lIIlIlI->desc.bAlternateSetting==IIIllI->
cur_altsetting->desc.bAlternateSetting){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x3a\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x25\x64\x20\x63\x75\x72\x72\x65\x6e\x74\x20\x61\x6c\x74\x73\x65\x74\x74\x69\x6e\x67\x20\x61\x6c\x72\x65\x61\x64\x79\x20\x73\x65\x74" "\n"
,IIlIl->IIIIIll.IIIlIIl);break;}}IIlIl->IIIlI.Status=usb_set_interface(lIIII,
IIlIl->IIIIIll.IIIlIIl,IIlIl->IIIIIll.lllIIIl);if(IIlIl->IIIlI.Status!=
(0xa61+7179-0x266c)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x65\x6c\x65\x63\x74\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x3a\x20\x75\x73\x62\x5f\x73\x65\x74\x5f\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64" "\n"
,IIlIl->IIIlI.Status);}if(IIIllI->cur_altsetting){int IlIIIl;int pipe;for(IlIIIl
=(0x275+9337-0x26ee);IlIIIl<IIIllI->cur_altsetting->desc.bNumEndpoints;IlIIIl++)
{if(IIIllI->cur_altsetting->endpoint[IlIIIl].desc.bEndpointAddress&
(0x1691+43-0x163c))pipe=usb_rcvisocpipe(lIIII,IIIllI->cur_altsetting->endpoint[
IlIIIl].desc.bEndpointAddress&(0x10e3+3624-0x1efc));else pipe=usb_sndisocpipe(
lIIII,IIIllI->cur_altsetting->endpoint[IlIIIl].desc.bEndpointAddress&
(0xcd3+4417-0x1e05));lIIlIIIl(IlIlI,pipe,(0x1595+1327-0x1ac4));}}}while(
(0x1038+802-0x135a));if(!IlIlI->lIIlIIl)
{
IIIllllI(IlIlI,lIIII);}return(0x1dd7+708-0x209a);}int IIIIIIlII(struct IllII*
IlIlI,struct usb_device*lIIII,llIII IIlIl){enum usb_device_state state=lIIII->
state;IIlIl->IIIlI.Status=(0x3b6+3538-0x1188);IIlIl->IIlIIlIl.lIlllIlI=
(0x172+2493-0xb2f);if(state!=USB_STATE_SUSPENDED&&state!=USB_STATE_NOTATTACHED){
IIlIl->IIlIIlIl.lIlllIlI|=IlIlIlIl;if(state==USB_STATE_CONFIGURED)IIlIl->
IIlIIlIl.lIlllIlI|=llIIIlIl;}return(0x231+1118-0x68e);}int lIlllIIIl(struct 
IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl){IIlIl->IIIlI.Status=-
(0xf77+3660-0x1dc2);if(IlIlI->lIIlIIl)
llIlIlIl(IlIlI);if(usb_lock_device_for_reset(lIIII,NULL)>=(0x5e1+5310-0x1a9f)){
IIlIl->IIIlI.Status=usb_reset_device(lIIII);usb_unlock_device(lIIII);}if(IIlIl->
IIIlI.Status!=(0x1998+2529-0x2379)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x72\x65\x73\x65\x74\x70\x6f\x72\x74\x3a\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64" "\n"
,IIlIl->IIIlI.Status);}return(0xc2a+6632-0x2611);}int IlIllIIII(struct IllII*
IlIlI,struct usb_device*lIIII,llIII IIlIl){struct IIIIl*lIIlI,*IIIlIII;unsigned 
long flags;IIlIl->IIIlI.Status=(0x1a9d+3068-0x2699);if(IlIlI->lIIlIIl)
{int abort=(0x127+5468-0x1683);llIII IllIlIll;spin_lock_irqsave(&IlIlI->lIIIll,
flags);if(IlIlI->llllIll){IllIlIll=(llIII)(IlIlI->llllIll+(0x1644+2375-0x1f8a));
abort=(IllIlIll->IIIlI.IIIIll==IIlIl->IIIlI.IIIIll);}if(IlIlI->IIIlIlI){IllIlIll
=(llIII)(IlIlI->IIIlIlI+(0x62+3344-0xd71));abort=(IllIlIll->IIIlI.IIIIll==IIlIl
->IIIlI.IIIIll);}spin_unlock_irqrestore(&IlIlI->lIIIll,flags);if(abort){llIlIlIl
(IlIlI);return(0x74a+4310-0x181f);}}



lIIlI=NULL;spin_lock_irqsave(&IlIlI->IIllI,flags);list_for_each_entry(IIIlIII,&
IlIlI->lllIll,lIIIl){if(((llIII)(IIIlIII+(0x71d+7654-0x2502)))->IIIlI.IIIIll==
IIlIl->IIIlI.IIIIll){



kref_get(&IIIlIII->IlIlIl);
lIIlI=IIIlIII;break;}}spin_unlock_irqrestore(&IlIlI->IIllI,flags);if(lIIlI){if(
lIIlI->lIlII){


usb_unlink_urb(lIIlI->lIlII);}else{lIlIlIII(&lIIlI->IlllII);}kref_put(&lIIlI->
IlIlIl,lIIlIl);}else{IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x63\x61\x6e\x63\x65\x6c\x3a\x20\x75\x72\x62\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
);}return(0xc6c+6185-0x2494);}int IlIllllII(struct IllII*IlIlI,struct usb_device
*lIIII,llIII IIlIl){struct IIIIl*lIIlI,*IIIlIII;unsigned long flags;IIlIl->IIIlI
.Status=(0x7e1+4111-0x17f0);if(IlIlI->lIIlIIl)
{int lIllIII=IIlIl->lIlIIllI.Endpoint|((IIlIl->lIlIIllI.Flags&llllII)?
(0xe73+1924-0x1577):(0x204+1442-0x7a6));if(lIllIII==IlIlI->lllIIIlI||lIllIII==
IlIlI->IlIIIlIl){llIlIlIl(IlIlI);}}




do{lIIlI=NULL;spin_lock_irqsave(&IlIlI->IIllI,flags);list_for_each_entry(IIIlIII
,&IlIlI->lllIll,lIIIl){
if(IIIlIII->endpoint==IIlIl->lIlIIllI.Endpoint){
IIIlIII->endpoint=(lllIl)-(0x5+4012-0xfb0);



kref_get(&IIIlIII->IlIlIl);
lIIlI=IIIlIII;break;}}spin_unlock_irqrestore(&IlIlI->IIllI,flags);if(lIIlI){if(
lIIlI->lIlII){


usb_unlink_urb(lIIlI->lIlII);}else{lIlIlIII(&lIIlI->IlllII);}kref_put(&lIIlI->
IlIlIl,lIIlIl);}}while(lIIlI);return(0x1943+2288-0x2232);}int IllIIIlI(struct 
IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lIIlI){int lIlll=
(0xcf3+4313-0x1dcc);unsigned long flags;struct urb*lIlII=NULL;struct 
usb_ctrlrequest*lIIIlI=NULL;do{int pipe;lIlII=usb_alloc_urb((0x1646+48-0x1676),
GFP_KERNEL);if(!lIlII){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x61\x6c\x6c\x6f\x63\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIlll=-ENOMEM;break;}lIIIlI=kmalloc(sizeof(*lIIIlI),GFP_KERNEL);if(!lIIIlI){
IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x61\x6c\x6c\x6f\x63\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIlll=-ENOMEM;break;}lIIIlI->bRequestType=IIlIl->llIIlI.llIIlll;lIIIlI->
bRequest=IIlIl->llIIlI.lIllIlll;lIIIlI->wValue=cpu_to_le16(IIlIl->llIIlI.
lllIIIIl);lIIIlI->wIndex=cpu_to_le16(IIlIl->llIIlI.IIllIllI);lIIIlI->wLength=
cpu_to_le16(IIlIl->llIIlI.llIlI);pipe=(IIlIl->llIIlI.Flags&llllII)?
usb_rcvctrlpipe(lIIII,IIlIl->llIIlI.Endpoint):usb_sndctrlpipe(lIIII,IIlIl->
llIIlI.Endpoint);usb_fill_control_urb(lIlII,lIIII,pipe,(unsigned char*)lIIIlI,
lIIlI->lllII,IIlIl->llIIlI.llIlI,lIIlIlll,lIIlI);IIlIIII(IlIlI);lIIlI->lIlII=
lIlII;lIIlI->IlIlI=IlIlI;lIIlI->endpoint=IIlIl->llIIlI.Endpoint;kref_init(&lIIlI
->IlIlIl);spin_lock_irqsave(&IlIlI->IIllI,flags);list_add_tail(&lIIlI->lIIIl,&
IlIlI->lllIll);spin_unlock_irqrestore(&IlIlI->IIllI,flags);lIlll=usb_submit_urb(
lIlII,GFP_KERNEL);if(lIlll<(0x747+5106-0x1b39)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x73\x75\x62\x6d\x69\x74\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64" "\n"
,lIlll);spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->llIIlI.llIlI=(0x3d0+4719-0x163f);IIlIl->IIIlI.lIlIl=sizeof(IIlIl->llIIlI)
+IIlIl->llIIlI.llIlI;IIlIl->IIIlI.Status=lIlll;lIlll=(0x86b+1264-0xd5b);kref_put
(&lIIlI->IlIlIl,lIIlIl);}}while((0x2362+429-0x250f));if(lIlll<(0xbd+4479-0x123c)
){if(lIlII)usb_free_urb(lIlII);if(lIIIlI)kfree(lIIIlI);}return lIlll;}int 
lIIIlIIl(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*
lIIlI,gfp_t lIIllI){if(lIIlI->IlllIII)return llIlIIlIl(IlIlI,lIIII,IIlIl,lIIlI,
lIIllI);else return llIlllIlI(IlIlI,lIIII,IIlIl,lIIlI,lIIllI);}int llIlIIlIl(
struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lIIlI,gfp_t 
lIIllI){int lIlll=(0xfd1+1320-0x14f9);unsigned long flags;struct urb*lIlII=NULL;
do{int pipe;lIlII=usb_alloc_urb((0x7d7+3174-0x143d),lIIllI);if(!lIlII){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x62\x75\x6c\x6b\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x61\x6c\x6c\x6f\x63\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIlll=-ENOMEM;break;}pipe=(IIlIl->llIIl.Flags&llllII)?usb_rcvbulkpipe(lIIII,
IIlIl->llIIl.Endpoint):usb_sndbulkpipe(lIIII,IIlIl->llIIl.Endpoint);
usb_fill_bulk_urb(lIlII,lIIII,pipe,lIIlI->lllII,IIlIl->llIIl.llIlI,IlIIIIII,
lIIlI);if(IIlIl->llIIl.Flags&llllII){if((IIlIl->llIIl.Flags&IIlIllI)==
(0xe38+1386-0x13a2)){lIlII->transfer_flags|=URB_SHORT_NOT_OK;}}IIlIIII(IlIlI);
lIIlI->lIlII=lIlII;lIIlI->IlIlI=IlIlI;lIIlI->endpoint=IIlIl->llIIl.Endpoint;
kref_init(&lIIlI->IlIlIl);spin_lock_irqsave(&IlIlI->IIllI,flags);list_add_tail(&
lIIlI->lIIIl,&IlIlI->lllIll);spin_unlock_irqrestore(&IlIlI->IIllI,flags);lIlll=
usb_submit_urb(lIlII,lIIllI);if(lIlll<(0x4c1+4089-0x14ba)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x6f\x6c\x69\x64\x5f\x62\x75\x6c\x6b\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x73\x75\x62\x6d\x69\x74\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64" "\n"
,lIlll);spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->llIIl.llIlI=(0x1554+2232-0x1e0c);IIlIl->IIIlI.lIlIl=sizeof(IIlIl->llIIl)+
IIlIl->llIIl.llIlI;IIlIl->IIIlI.Status=lIlll;kref_put(&lIIlI->IlIlIl,lIIlIl);
lIlll=(0x12f4+3074-0x1ef6);}}while((0x67d+1324-0xba9));if(lIlll<
(0x8b0+5263-0x1d3f)){if(lIlII)usb_free_urb(lIlII);}return lIlll;}int llIlllIlI(
struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lIIlI,gfp_t 
lIIllI){int lIlll=(0x5b2+8462-0x26c0);unsigned long flags;do{int pipe;pipe=(
IIlIl->llIIl.Flags&llllII)?usb_rcvbulkpipe(lIIII,IIlIl->llIIl.Endpoint):
usb_sndbulkpipe(lIIII,IIlIl->llIIl.Endpoint);lIlll=IIIIIIlI(&lIIlI->IlllII,pipe,
(0x9b8+2316-0x12c4),!!(IIlIl->llIIl.Flags&IIlIllI),IlIlI->lIIIIll,lIIII,&lIIlI->
IlIllII,lIIlI,IlIIIlllI,lIIllI);if(lIlll<(0x1948+1312-0x1e68)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x70\x61\x72\x74\x69\x74\x69\x6f\x6e\x65\x64\x5f\x62\x75\x6c\x6b\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x64\x5f\x75\x63\x5f\x69\x6e\x69\x74\x20\x66\x61\x69\x6c\x65\x64" "\n"
);break;}IIlIIII(IlIlI);lIIlI->lIlII=NULL;lIIlI->IlIlI=IlIlI;lIIlI->endpoint=
IIlIl->llIIl.Endpoint;kref_init(&lIIlI->IlIlIl);spin_lock_irqsave(&IlIlI->IIllI,
flags);list_add_tail(&lIIlI->lIIIl,&IlIlI->lllIll);spin_unlock_irqrestore(&IlIlI
->IIllI,flags);lIlll=lIIIlIII(&lIIlI->IlllII);if(lIlll<(0x119b+1436-0x1737)){
IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x70\x61\x72\x74\x69\x74\x69\x6f\x6e\x65\x64\x5f\x62\x75\x6c\x6b\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x64\x5f\x75\x63\x5f\x73\x75\x62\x6d\x69\x74\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64\x20\x66\x6f\x72\x20\x55\x6e\x69\x71\x75\x65\x49\x64\x3d\x30\x78\x25\x2e\x38\x58\x25\x2e\x38\x58" "\n"
,lIlll,(IIIlll)(IIlIl->IIIlI.IIIIll>>(0x1d6+6265-0x1a2f)),(IIIlll)(IIlIl->IIIlI.
IIIIll));spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->llIIl.llIlI=(0x531+6857-0x1ffa);IIlIl->IIIlI.lIlIl=sizeof(IIlIl->llIIl)+
IIlIl->llIIl.llIlI;IIlIl->IIIlI.Status=lIlll;kref_put(&lIIlI->IlIlIl,lIIlIl);
lIlll=(0x19ab+306-0x1add);}}while((0x6d1+5818-0x1d8b));if(lIlll<
(0x27d+1694-0x91b)){lllIllll(&lIIlI->IlllII);}return lIlll;}int IlIlIIII(struct 
IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lIIlI){int lIlll=
(0x883+1024-0xc83);unsigned long flags;struct urb*lIlII=NULL;do{int pipe;struct 
usb_host_endpoint*ep;lIlII=usb_alloc_urb((0xc08+6828-0x26b4),GFP_KERNEL);if(!
lIlII){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x69\x6e\x74\x65\x72\x72\x75\x70\x74\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x61\x6c\x6c\x6f\x63\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIlll=-ENOMEM;break;}pipe=(IIlIl->lIllIl.Flags&llllII)?usb_rcvintpipe(lIIII,
IIlIl->lIllIl.Endpoint):usb_sndintpipe(lIIII,IIlIl->lIllIl.Endpoint);IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x69\x6e\x74\x65\x72\x72\x75\x70\x74\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x69\x6e\x74\x65\x72\x76\x61\x6c\x20\x69\x73\x20\x25\x64" "\n"
,IIlIl->lIllIl.Interval);usb_fill_int_urb(lIlII,lIIII,pipe,lIIlI->lllII,IIlIl->
lIllIl.llIlI,llIIllII,lIIlI,(0x9f+8963-0x23a1));if(IIlIl->lIllIl.Interval){lIlII
->interval=IIlIl->lIllIl.Interval;}else{
#if KERNEL_GT_EQ((0xaa1+5222-0x1f05),(0x1708+2912-0x2262),(0x248+2277-0xb23))
ep=(IIlIl->lIllIl.Flags&llllII)?lIIII->ep_in[IIlIl->lIllIl.Endpoint]:lIIII->
ep_out[IIlIl->lIllIl.Endpoint];IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x69\x6e\x74\x65\x72\x72\x75\x70\x74\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x64\x69\x72\x65\x63\x74\x69\x6f\x6e\x3d\x25\x73\x20\x65\x70\x3d\x30\x78\x25\x70\x20\x65\x70\x5f\x69\x6e\x5b\x25\x64\x5d\x3d\x30\x78\x25\x70\x20\x65\x70\x5f\x6f\x75\x74\x5b\x25\x64\x5d\x3d\x30\x78\x25\x70" "\n"
,(IIlIl->lIllIl.Flags&llllII)?"\x69\x6e":"\x6f\x75\x74",ep,IIlIl->lIllIl.
Endpoint,lIIII->ep_in[IIlIl->lIllIl.Endpoint],IIlIl->lIllIl.Endpoint,lIIII->
ep_out[IIlIl->lIllIl.Endpoint]);if(ep){if(lIIII->speed==USB_SPEED_HIGH)lIlII->
interval=(0x1a65+2888-0x25ac)<<(ep->desc.bInterval-(0x4ba+2001-0xc8a));else 
lIlII->interval=ep->desc.bInterval;}else{lIlII->interval=(0xf24+6097-0x26f4);}
#else
lIlII->interval=(0x492+7363-0x2154);
#endif
}if(IIlIl->lIllIl.Flags&llllII){if((IIlIl->lIllIl.Flags&IIlIllI)==
(0x136a+1304-0x1882)){lIlII->transfer_flags|=URB_SHORT_NOT_OK;}}IIlIIII(IlIlI);
lIIlI->lIlII=lIlII;lIIlI->IlIlI=IlIlI;lIIlI->endpoint=IIlIl->lIllIl.Endpoint;
kref_init(&lIIlI->IlIlIl);spin_lock_irqsave(&IlIlI->IIllI,flags);list_add_tail(&
lIIlI->lIIIl,&IlIlI->lllIll);spin_unlock_irqrestore(&IlIlI->IIllI,flags);while(
(0x1aeb+423-0x1c91)){lIlll=usb_submit_urb(lIlII,GFP_KERNEL);if((lIlll==-ENOMEM)
&&(lIIII->speed==USB_SPEED_HIGH)&&(lIlII->interval<=(0x16b9+1880-0x1d91)))
lIlII->interval<<=(0x855+3627-0x167f);else break;yield();}if(lIlll<
(0x980+1729-0x1041)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x69\x6e\x74\x65\x72\x72\x75\x70\x74\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x73\x75\x62\x6d\x69\x74\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64\x20\x66\x6f\x72\x20\x55\x6e\x69\x71\x75\x65\x49\x64\x3d\x30\x78\x25\x2e\x38\x58\x25\x2e\x38\x58" "\n"
,lIlll,(IIIlll)(IIlIl->IIIlI.IIIIll>>(0x2d+1628-0x669)),(IIIlll)(IIlIl->IIIlI.
IIIIll));spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->lIllIl.llIlI=(0xde8+6023-0x256f);IIlIl->IIIlI.lIlIl=sizeof(IIlIl->lIllIl)
+IIlIl->lIllIl.llIlI;IIlIl->IIIlI.Status=lIlll;kref_put(&lIIlI->IlIlIl,lIIlIl);
lIlll=(0x465+8534-0x25bb);}}while((0x1ce1+870-0x2047));if(lIlll<
(0x1eec+1057-0x230d)){if(lIlII)usb_free_urb(lIlII);}return lIlll;}int IllllllIl(
struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lIIlI){if(
lIIlI->IlllIII)return IIIllllIl(IlIlI,lIIII,IIlIl,lIIlI);else return IlIlllIlI(
IlIlI,lIIII,IIlIl,lIIlI);}int IlIlllIlI(struct IllII*IlIlI,struct usb_device*
lIIII,llIII IIlIl,struct IIIIl*lIIlI){unsigned long flags;int pipe,lIlll=
(0x971+1416-0xef9);do{int i,IlIIIl,llllllI,IIIllll,lllIIll,llIIlllI;int IllIIII;
struct urb*lIlII;pipe=(IIlIl->IIlll.Flags&llllII)?usb_rcvisocpipe(lIIII,IIlIl->
IIlll.Endpoint):usb_sndisocpipe(lIIII,IIlIl->IIlll.Endpoint);llIIlllI=IIIIlIII(
IlIlI,pipe);lIlll=IIIIIIlI(&lIIlI->IlllII,pipe,IIlIl->IIlll.Interval,!!(IIlIl->
IIlll.Flags&IIlIllI),IlIlI->lIIIIll,lIIII,&lIIlI->IlIllII,lIIlI,IlIIlIlI,
GFP_KERNEL);if(lIlll<(0x190+200-0x258)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x70\x61\x72\x74\x69\x74\x69\x6f\x6e\x65\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x64\x5f\x75\x63\x5f\x69\x6e\x69\x74\x20\x66\x61\x69\x6c\x65\x64" "\n"
);break;}lIIlI->IlllII.IlIll[(0x476+8546-0x25d8)]->start_frame=IIlIl->IIlll.
lIlllII;if(!(IIlIl->IIlll.Flags&IIlIlIlI))lIIlI->IlllII.IlIll[
(0x1483+1999-0x1c52)]->transfer_flags&=~URB_ISO_ASAP;


lIlII=lIIlI->IlllII.IlIll[(0xff0+4587-0x21db)];IllIIII=(0x1aca+1131-0x1f35);for(
i=(0xe9f+2734-0x194d),IlIIIl=(0x1054+5187-0x2497),llllllI=(0x25f2+15-0x2601);i<
IIlIl->IIlll.llIlIll;i++,llllllI++){if(llllllI>=lIlII->number_of_packets){if(++
IlIIIl>=lIIlI->IlllII.IlIII)break;IllIIII=(0x5e7+3648-0x1427);lIlII=lIIlI->
IlllII.IlIll[IlIIIl];llllllI=(0x64b+749-0x938);}lIlII->iso_frame_desc[llllllI].
offset=IllIIII;lIlII->iso_frame_desc[llllllI].length=IIlIl->IIlll.IIlIlI[i].
Length;IllIIII+=lIlII->iso_frame_desc[llllllI].length;
}IIlIIII(IlIlI);lIIlI->lIlII=NULL;lIIlI->IlIlI=IlIlI;lIIlI->endpoint=IIlIl->
IIlll.Endpoint;kref_init(&lIIlI->IlIlIl);


IIIllll=lIlllIII(IlIlI,pipe);lllIIll=usb_get_current_frame_number(lIIII);if(!(
lIIlI->IlllII.IlIll[(0x15a5+3366-0x22cb)]->transfer_flags&URB_ISO_ASAP)&&(
IIIllll==(0xfea+2099-0x181d))){IIIllll=(lllIIll+(0x526+1456-0xad4))-lIIlI->
IlllII.IlIll[(0x538+3245-0x11e5)]->start_frame;lIIlIIIl(IlIlI,pipe,IIIllll);}if(
llIIlllI==(0xb17+3866-0x1a30)){if(lIIlI->IlllII.IlIll[(0x14c3+2154-0x1d2d)]->
transfer_flags&URB_ISO_ASAP){lIIlI->IlllII.IlIll[(0x241+1965-0x9ee)]->
transfer_flags&=~URB_ISO_ASAP;lIIlI->IlllII.IlIll[(0x1e7f+679-0x2126)]->
start_frame=lllIIll+(0x869+1496-0xe40)-IIIllll;}}if(!(lIIlI->IlllII.IlIll[
(0x159b+2335-0x1eba)]->transfer_flags&URB_ISO_ASAP)){lIIlI->IlllII.IlIll[
(0x170a+1356-0x1c56)]->start_frame+=IIIllll;if(lIIlI->IlllII.IlIll[
(0x441+8546-0x25a3)]->start_frame<lllIIll+(0x1374+3730-0x2205)){IIlIl->IIIlI.
Status=-EXDEV;IIlIl->IIIlI.lIlIl=IlIlIIl(IIlIl);IIlIl->IIlll.llIlI=
(0x18a3+2252-0x216f);IIlIl->IIlll.lIlIIII=IIlIl->IIlll.llIlIll;for(i=
(0x3b7+4669-0x15f4);i<IIlIl->IIlll.llIlIll;i++){IIlIl->IIlll.IIlIlI[i].Status=-
EINVAL;IIlIl->IIlll.IIlIlI[i].Length=(0x1cb9+2002-0x248b);}lIlll=
(0x1768+3541-0x253d);kref_put(&lIIlI->IlIlIl,lIIlIl);break;}}spin_lock_irqsave(&
IlIlI->IIllI,flags);list_add_tail(&lIIlI->lIIIl,&IlIlI->lllIll);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);lIlll=lIIIlIII(&lIIlI->IlllII);if(
lIlll<(0xfad+3624-0x1dd5)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x70\x61\x72\x74\x69\x74\x69\x6f\x6e\x65\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x64\x5f\x75\x63\x5f\x73\x75\x62\x6d\x69\x74\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64\x20\x66\x6f\x72\x20\x55\x6e\x69\x71\x75\x65\x49\x64\x3d\x30\x78\x25\x2e\x38\x58\x25\x2e\x38\x58" "\n"
,lIlll,(IIIlll)(IIlIl->IIIlI.IIIIll>>(0x1a99+2289-0x236a)),(IIIlll)(IIlIl->IIIlI
.IIIIll));lIIIIlI(IlIlI,pipe);spin_lock_irqsave(&IlIlI->IIllI,flags);
list_del_init(&lIIlI->lIIIl);spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->IIlll.llIlI=(0x678+4288-0x1738);IIlIl->IIIlI.lIlIl=IlIlIIl(IIlIl)+IIlIl->
IIlll.llIlI;IIlIl->IIIlI.Status=lIlll;kref_put(&lIIlI->IlIlIl,lIIlIl);lIlll=
(0x6ff+7759-0x254e);}}while((0xd1b+5909-0x2430));if(lIlll<(0x22bd+364-0x2429)){
lIIIIlI(IlIlI,pipe);lllIllll(&lIIlI->IlllII);}return lIlll;}int IIIllllIl(struct
 IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lIIlI){int pipe,
lIlll=(0x1a5c+596-0x1cb0);unsigned long flags;struct urb*lIlII=NULL;int IllIIII;
do{int i,IIIllll,lllIIll,llIIlllI;struct usb_host_endpoint*ep;pipe=(IIlIl->IIlll
.Flags&llllII)?usb_rcvisocpipe(lIIII,IIlIl->IIlll.Endpoint):usb_sndisocpipe(
lIIII,IIlIl->IIlll.Endpoint);llIIlllI=IIIIlIII(IlIlI,pipe);lIlII=usb_alloc_urb(
IIlIl->IIlll.llIlIll,GFP_KERNEL);if(!lIlII){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x6f\x6c\x69\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x61\x6c\x6c\x6f\x63\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIlll=-ENOMEM;break;}lIlII->dev=lIIII;lIlII->pipe=pipe;lIlII->transfer_flags=(
IIlIl->IIlll.Flags&IIlIlIlI)?URB_ISO_ASAP:(0xd87+5311-0x2246);lIlII->
transfer_buffer_length=IIlIl->IIlll.llIlI;lIlII->start_frame=IIlIl->IIlll.
lIlllII;lIlII->number_of_packets=IIlIl->IIlll.llIlIll;lIlII->context=lIIlI;lIlII
->complete=llllIIIl;lIlII->transfer_buffer=lIIlI->lllII;if(IIlIl->IIlll.Interval
){lIlII->interval=IIlIl->IIlll.Interval;}else{
#if KERNEL_GT_EQ((0xe38+4014-0x1de4),(0x96d+2670-0x13d5),(0x3c5+1299-0x8ce))

ep=(IIlIl->IIlll.Flags&llllII)?lIIII->ep_in[IIlIl->IIlll.Endpoint]:lIIII->ep_out
[IIlIl->IIlll.Endpoint];if(ep){if(lIIII->speed==USB_SPEED_HIGH)lIlII->interval=
(0x1112+2512-0x1ae1)<<(ep->desc.bInterval-(0x649+344-0x7a0));else lIlII->
interval=ep->desc.bInterval;}else{lIlII->interval=(0x59f+1384-0xb06);}
#else
lIlII->interval=(0x1513+3991-0x24a9);
#endif
}IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x6f\x6c\x69\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x69\x6e\x74\x65\x72\x76\x61\x6c\x20\x69\x73\x20\x25\x64" "\n"
,lIlII->interval);for(IllIIII=(0xa75+6348-0x2341),i=(0x1829+569-0x1a62);i<IIlIl
->IIlll.llIlIll;i++){lIlII->iso_frame_desc[i].offset=IllIIII;lIlII->
iso_frame_desc[i].length=IIlIl->IIlll.IIlIlI[i].Length;IllIIII+=lIlII->
iso_frame_desc[i].length;}IIlIIII(IlIlI);lIIlI->lIlII=lIlII;lIIlI->IlIlI=IlIlI;
lIIlI->endpoint=IIlIl->IIlll.Endpoint;kref_init(&lIIlI->IlIlIl);


IIIllll=lIlllIII(IlIlI,pipe);lllIIll=usb_get_current_frame_number(lIIII);if(!(
lIlII->transfer_flags&URB_ISO_ASAP)&&(IIIllll==(0x12a9+2869-0x1dde))){IIIllll=(
lllIIll+(0x5e8+2410-0xf50))-lIlII->start_frame;lIIlIIIl(IlIlI,pipe,IIIllll);}if(
llIIlllI==(0x9f+2753-0xb5f)){if(lIlII->transfer_flags&URB_ISO_ASAP){lIlII->
transfer_flags&=~URB_ISO_ASAP;lIlII->start_frame=lllIIll+(0x4a5+914-0x836)-
IIIllll;}}if(!(lIlII->transfer_flags&URB_ISO_ASAP)){lIlII->start_frame+=IIIllll;
if(lIlII->start_frame<lllIIll+(0x1873+1550-0x1e80)){IIlIl->IIIlI.Status=-EXDEV;
IIlIl->IIIlI.lIlIl=IlIlIIl(IIlIl);IIlIl->IIlll.llIlI=(0xd65+3205-0x19ea);IIlIl->
IIlll.lIlIIII=IIlIl->IIlll.llIlIll;for(i=(0x1bc7+108-0x1c33);i<IIlIl->IIlll.
llIlIll;i++){IIlIl->IIlll.IIlIlI[i].Status=-EINVAL;IIlIl->IIlll.IIlIlI[i].Length
=(0x2ac+7686-0x20b2);}lIlll=(0x298+203-0x363);kref_put(&lIIlI->IlIlIl,lIIlIl);
break;}}spin_lock_irqsave(&IlIlI->IIllI,flags);list_add_tail(&lIIlI->lIIIl,&
IlIlI->lllIll);spin_unlock_irqrestore(&IlIlI->IIllI,flags);IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x6f\x6c\x69\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x66\x6c\x61\x67\x73\x3d\x25\x64\x20\x73\x74\x61\x72\x74\x66\x72\x61\x6d\x65\x3d\x25\x64\x20\x63\x75\x72\x66\x72\x61\x6d\x65\x3d\x25\x64\x20\x64\x65\x6c\x74\x61\x3d\x25\x64" "\n"
,lIlII->transfer_flags,lIlII->start_frame,usb_get_current_frame_number(lIIII),
lIlllIII(IlIlI,pipe));lIlll=usb_submit_urb(lIlII,GFP_KERNEL);if(lIlll<
(0x60+659-0x2f3)){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x68\x61\x6e\x64\x6c\x65\x5f\x73\x6f\x6c\x69\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x73\x62\x5f\x73\x75\x62\x6d\x69\x74\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64\x20\x77\x69\x74\x68\x20\x65\x72\x72\x6f\x72\x20\x25\x64\x20\x66\x6f\x72\x20\x55\x6e\x69\x71\x75\x65\x49\x64\x3d\x30\x78\x25\x2e\x38\x58\x25\x2e\x38\x58" "\n"
,lIlll,(IIIlll)(IIlIl->IIIlI.IIIIll>>(0x2a4+3554-0x1066)),(IIIlll)(IIlIl->IIIlI.
IIIIll));lIIIIlI(IlIlI,pipe);spin_lock_irqsave(&IlIlI->IIllI,flags);
list_del_init(&lIIlI->lIIIl);spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->IIlll.llIlI=(0x253+1807-0x962);IIlIl->IIIlI.lIlIl=IlIlIIl(IIlIl)+IIlIl->
IIlll.llIlI;IIlIl->IIIlI.Status=lIlll;kref_put(&lIIlI->IlIlIl,lIIlIl);lIlll=
(0xa54+1730-0x1116);}}while((0xe96+6235-0x26f1));if(lIlll<(0xec7+5115-0x22c2)){
if(lIlII)usb_free_urb(lIlII);lIIIIlI(IlIlI,pipe);}return lIlll;}void IlIlIlII(
struct urb*lIlII
#if KERNEL_LT((0x1318+1659-0x1991),(0xb43+3946-0x1aa7),(0x459+84-0x49a))
,struct pt_regs*IIIllIl
#endif
){struct IIIIl*lIIlI=lIlII->context;struct IllII*IlIlI=lIIlI->IlIlI;llIII IIlIl=
(llIII)(lIIlI+(0x8b+243-0x17d));unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x67\x65\x74\x64\x65\x73\x63\x72\x69\x70\x74\x6f\x72" "\n"
);
spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->lllllI.llIlI=(lIlII->actual_length<(0x1f19+1035-0x2324))?
(0xc15+4145-0x1c46):lIlII->actual_length;IIlIl->lllllI.IIlIll.lIlIl=sizeof(IIlIl
->lllllI)+IIlIl->lllllI.llIlI;IIlIl->lllllI.IIlIll.Status=lIlII->status;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x67\x65\x74\x64\x65\x73\x63\x72\x69\x70\x74\x6f\x72\x3a\x20\x75\x72\x62\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x20\x77\x69\x74\x68\x20\x73\x74\x61\x74\x75\x73\x20\x25\x64" "\n"
,lIlII->status);IIllIII(IIlIl);
kref_put(&lIIlI->IlIlIl,lIIlIl);}void lIIlIlll(struct urb*lIlII
#if KERNEL_LT((0x10e3+774-0x13e7),(0x232d+967-0x26ee),(0x120+8161-0x20ee))
,struct pt_regs*IIIllIl
#endif
){struct IIIIl*lIIlI=lIlII->context;struct IllII*IlIlI=lIIlI->IlIlI;llIII IIlIl=
(llIII)(lIIlI+(0x5e9+328-0x730));unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x74\x72\x61\x6e\x73\x66\x65\x72" "\n"
);
spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);
IIlIl->llIIlI.llIlI=(lIlII->actual_length<(0x1b76+2142-0x23d4))?
(0xa85+2059-0x1290):lIlII->actual_length;IIlIl->IIIlI.lIlIl=sizeof(IIlIl->llIIlI
);if(usb_pipein(lIlII->pipe))IIlIl->IIIlI.lIlIl+=IIlIl->llIIlI.llIlI;IIlIl->
llIIlI.IIlIll.Status=lIlII->status;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x72\x62\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x20\x77\x69\x74\x68\x20\x73\x74\x61\x74\x75\x73\x20\x25\x64" "\n"
,lIlII->status);IIllIII(IIlIl);
kref_put(&lIIlI->IlIlIl,lIIlIl);}void IlIIIIII(struct urb*lIlII
#if KERNEL_LT((0x8f9+5441-0x1e38),(0x327+4898-0x1643),(0xbdb+6508-0x2534))
,struct pt_regs*IIIllIl
#endif
){struct IIIIl*lIIlI=lIlII->context;struct IllII*IlIlI=lIIlI->IlIlI;llIII IIlIl=
(llIII)(lIIlI+(0x18ab+1194-0x1d54));unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x62\x75\x6c\x6b\x74\x72\x61\x6e\x73\x66\x65\x72" "\n"
);
spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);IIlIl->llIIl.llIlI=lIlII->
actual_length;IIlIl->IIIlI.lIlIl=sizeof(IIlIl->llIIl);if(usb_pipein(lIlII->pipe)
)IIlIl->IIIlI.lIlIl+=IIlIl->llIIl.llIlI;IIlIl->llIIl.IIlIll.Status=lIlII->status
;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x62\x75\x6c\x6b\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x72\x62\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x20\x77\x69\x74\x68\x20\x73\x74\x61\x74\x75\x73\x20\x25\x64" "\n"
,lIlII->status);IIllIII(IIlIl);
kref_put(&lIIlI->IlIlIl,lIIlIl);}void llIIllII(struct urb*lIlII
#if KERNEL_LT((0xaa8+2473-0x144f),(0x12e0+715-0x15a5),(0x1957+928-0x1ce4))
,struct pt_regs*IIIllIl
#endif
){struct IIIIl*lIIlI=lIlII->context;struct IllII*IlIlI=lIIlI->IlIlI;llIII IIlIl=
(llIII)(lIIlI+(0x6a3+7393-0x2383));unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x69\x6e\x74\x65\x72\x72\x75\x70\x74\x74\x72\x61\x6e\x73\x66\x65\x72" "\n"
);
spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);IIlIl->lIllIl.llIlI=lIlII->
actual_length;IIlIl->IIIlI.lIlIl=sizeof(IIlIl->lIllIl);if(usb_pipein(lIlII->pipe
))IIlIl->IIIlI.lIlIl+=IIlIl->lIllIl.llIlI;IIlIl->lIllIl.IIlIll.Status=lIlII->
status;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x69\x6e\x74\x65\x72\x72\x75\x70\x74\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x72\x62\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x20\x77\x69\x74\x68\x20\x73\x74\x61\x74\x75\x73\x20\x25\x64" "\n"
,lIlII->status);IIllIII(IIlIl);
kref_put(&lIIlI->IlIlIl,lIIlIl);}void llllIIIl(struct urb*lIlII
#if KERNEL_LT((0xb92+1492-0x1164),(0xa92+6204-0x22c8),(0x946+675-0xbd6))
,struct pt_regs*IIIllIl
#endif
){int i;struct IIIIl*lIIlI=lIlII->context;struct IllII*IlIlI=lIIlI->IlIlI;llIII 
IIlIl=(llIII)(lIIlI+(0x7ff+7103-0x23bd));unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x73\x6f\x6c\x69\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x72\x62\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x20\x77\x69\x74\x68\x20\x73\x74\x61\x74\x75\x73\x20\x25\x64" "\n"
,lIlII->status);lIIIIlI(IlIlI,lIlII->pipe);lIlII->start_frame-=lIlllIII(IlIlI,
lIlII->pipe);
spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);
#if KERNEL_LT((0x1a7d+2861-0x25a8),(0x449+8705-0x2644),(0x38+8673-0x2201))


if(lIlII->status!=-ECONNRESET&&lIlII->status!=-ENOENT&&lIlII->status!=-ESHUTDOWN
)lIlII->status=(0x1095+4592-0x2285);
#endif
IIlIl->IIlll.IIlIll.Status=lIlII->status;IIlIl->IIlll.lIlIIII=lIlII->error_count
;IIlIl->IIlll.lIlllII=lIlII->start_frame;IIlIl->IIlll.llIlI=IllIIIII(lIlII->
iso_frame_desc,lIlII->number_of_packets,lIlII->transfer_buffer,lIlII->
transfer_buffer,(0x3d6+7769-0x222e));for(i=(0x78a+2322-0x109c);i<IIlIl->IIlll.
llIlIll;i++){IIlIl->IIlll.IIlIlI[i].Length=lIlII->iso_frame_desc[i].
actual_length;IIlIl->IIlll.IIlIlI[i].Status=lIlII->iso_frame_desc[i].status;}
IIlIl->IIIlI.lIlIl=IlIlIIl(IIlIl);if(usb_pipein(lIlII->pipe))IIlIl->IIIlI.lIlIl
+=IIlIl->IIlll.llIlI;IIllIII(IIlIl);
#if defined(_USBD_USE_EHCI_FIX_) && KERNEL_GT_EQ((0x980+6859-0x2449),\
(0xc53+5034-0x1ff7),(0xe67+326-0xf94)) && KERNEL_LT_EQ((0x894+4432-0x19e2),\
(0x579+867-0x8d6),(0x421+3923-0x1358))
if(IlIlI->lIIIIll&&(lIlII->status==(0x311+3734-0x11a7))&&(atomic_read(&lIlII->
kref.refcount)>(0xd32+1773-0x141e)))usb_put_urb(lIlII);
#endif

kref_put(&lIIlI->IlIlIl,lIIlIl);}void IlIIIlllI(struct lllIlI*lIllI){int i;
struct IIIIl*lIIlI=lIllI->context;struct IllII*IlIlI=lIIlI->IlIlI;llIII IIlIl=(
llIII)(lIIlI+(0x1bf9+120-0x1c70));unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x70\x61\x72\x74\x69\x74\x69\x6f\x6e\x65\x64\x5f\x62\x75\x6c\x6b\x74\x72\x61\x6e\x73\x66\x65\x72" "\n"
);
spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);IIlIl->llIIl.llIlI=lIllI->lIlIlllI;
IIlIl->IIIlI.lIlIl=sizeof(IIlIl->llIIl);if(usb_pipein(lIllI->pipe))IIlIl->llIIl.
IIlIll.lIlIl+=IIlIl->llIIl.llIlI;IIlIl->llIIl.IIlIll.Status=lIllI->status;for(i=
(0x571+4347-0x166c);i<lIllI->IlIII;i++)lIIlI->IlIllII.lIIIlII[i]=lIllI->IlIll[i]
->actual_length;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x70\x61\x72\x74\x69\x74\x69\x6f\x6e\x65\x64\x5f\x62\x75\x6c\x6b\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x72\x62\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x20\x77\x69\x74\x68\x20\x73\x74\x61\x74\x75\x73\x20\x25\x64" "\n"
,lIllI->status);IIllIII(IIlIl);
kref_put(&lIIlI->IlIlIl,lIIlIl);}void IlIIlIlI(struct lllIlI*lIllI){int i,IlIIIl
,llllllI;struct IIIIl*lIIlI=lIllI->context;struct IllII*IlIlI=lIIlI->IlIlI;llIII
 IIlIl=(llIII)(lIIlI+(0x18f7+717-0x1bc3));unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x70\x61\x72\x74\x69\x74\x69\x6f\x6e\x65\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72" "\n"
);lIIIIlI(IlIlI,lIllI->pipe);lIllI->IlIll[(0xf9+667-0x394)]->start_frame-=
lIlllIII(IlIlI,lIllI->pipe);
spin_lock_irqsave(&IlIlI->IIllI,flags);list_del_init(&lIIlI->lIIIl);
spin_unlock_irqrestore(&IlIlI->IIllI,flags);IIlIl->IIlll.IIlIll.Status=lIllI->
status;IIlIl->IIlll.lIlllII=lIllI->IlIll[(0x1653+2703-0x20e2)]->start_frame;
IIlIl->IIlll.lIlIIII=(0x9b5+6485-0x230a);IIlIl->IIlll.llIlI=(0x4ec+4561-0x16bd);
for(i=(0x1a45+502-0x1c3b),llllllI=(0x71a+6447-0x2049);i<lIllI->IlIII;i++){struct
 urb*lIlII=lIllI->IlIll[i];lIIlI->IlIllII.lIIIlII[i]=IllIIIII(lIlII->
iso_frame_desc,lIlII->number_of_packets,lIlII->transfer_buffer,lIlII->
transfer_buffer,(0x1aa1+2527-0x247f));IIlIl->IIlll.llIlI+=lIIlI->IlIllII.lIIIlII
[i];IIlIl->IIlll.lIlIIII+=lIlII->error_count;for(IlIIIl=(0x48+655-0x2d7);IlIIIl<
lIlII->number_of_packets;IlIIIl++){
IIlIl->IIlll.IIlIlI[llllllI].Length=lIlII->iso_frame_desc[IlIIIl].actual_length;
IIlIl->IIlll.IIlIlI[llllllI].Status=lIlII->iso_frame_desc[IlIIIl].status;llllllI
++;}}IIlIl->IIIlI.lIlIl=IlIlIIl(IIlIl);if(usb_pipein(lIllI->pipe))IIlIl->IIIlI.
lIlIl+=IIlIl->IIlll.llIlI;IIIll(
"\x75\x73\x62\x64\x5f\x6d\x64\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x70\x61\x72\x74\x69\x74\x69\x6f\x6e\x65\x64\x5f\x69\x73\x6f\x63\x68\x74\x72\x61\x6e\x73\x66\x65\x72\x3a\x20\x75\x72\x62\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x20\x77\x69\x74\x68\x20\x73\x74\x61\x74\x75\x73\x20\x25\x64" "\n"
,lIllI->status);IIllIII(IIlIl);
kref_put(&lIIlI->IlIlIl,lIIlIl);}void lIIlIl(struct kref*IlllIIl){struct IIIIl*
lIIlI=container_of(IlllIIl,struct IIIIl,IlIlIl);struct IllII*IlIlI;

if(lIIlI->IlllIII){if(lIIlI->lIlII){if(lIIlI->lIlII->setup_packet)kfree(lIIlI->
lIlII->setup_packet);usb_free_urb(lIIlI->lIlII);lIIlI->lIlII=NULL;}}else{
lllIllll(&lIIlI->IlllII);}
IlIlI=lIIlI->IlIlI;lIIlI->IlIlI=NULL;
if(lIIlI->IIlIIIII){lllllll(lIIlI);}else{unsigned long flags;spin_lock_irqsave(&
IlIlI->lIlIIll,flags);list_add_tail(&lIIlI->lIIIl,&IlIlI->IlllIlI);
spin_unlock_irqrestore(&IlIlI->lIlIIll,flags);wake_up(&IlIlI->lllIII);}
if(IlIlI)llIllII(IlIlI);}int IIIllllI(struct IllII*IlIlI,struct usb_device*
lIlIIl){int i;int IlIllIl,IIIlllIl;IlIllIl=IIIlllIl=-(0xb03+2577-0x1513);
for(i=(0x135c+1615-0x19ab);i<USB_MAXINTERFACES;i++){struct usb_interface*
interface=lIlIIl->actconfig->interface[i];if(interface==NULL)continue;if(
interface->cur_altsetting==NULL)continue;IIIll(
"\x44\x65\x74\x65\x63\x74\x65\x64\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x63\x6c\x61\x73\x73\x3d\x30\x78\x25\x30\x32\x78\x20\x73\x75\x62\x63\x6c\x61\x73\x73\x3d\x30\x78\x25\x30\x32\x78\x20\x70\x72\x6f\x74\x6f\x63\x6f\x6c\x3d\x30\x78\x25\x30\x32\x78"
,interface->cur_altsetting->desc.bInterfaceClass,interface->cur_altsetting->desc
.bInterfaceSubClass,interface->cur_altsetting->desc.bInterfaceProtocol);if(
interface->cur_altsetting->desc.bInterfaceClass==(0x63b+7314-0x22c5)&&interface
->cur_altsetting->desc.bInterfaceSubClass==(0x1706+884-0x1a74)&&interface->
cur_altsetting->desc.bInterfaceProtocol==(0x118b+780-0x1447)){int IlIIIl;IlIllIl
=IIIlllIl=-(0x1481+1210-0x193a);for(IlIIIl=(0x1625+3353-0x233e);IlIIIl<interface
->cur_altsetting->desc.bNumEndpoints;IlIIIl++){struct usb_host_endpoint*endpoint
=&interface->cur_altsetting->endpoint[IlIIIl];if((endpoint->desc.bmAttributes&
USB_ENDPOINT_XFERTYPE_MASK)==USB_ENDPOINT_XFER_BULK){if(endpoint->desc.
bEndpointAddress&USB_ENDPOINT_DIR_MASK)IlIllIl=endpoint->desc.bEndpointAddress;
else IIIlllIl=endpoint->desc.bEndpointAddress;}}if(IlIllIl!=-
(0x16e1+3076-0x22e4)&&IIIlllIl!=-(0x3d0+3891-0x1302)){unsigned long flags;IIIll(
"\x44\x65\x74\x65\x63\x74\x65\x64\x20\x65\x6e\x64\x70\x6f\x69\x6e\x74\x73\x20\x30\x78\x25\x30\x32\x78\x20\x61\x6e\x64\x20\x30\x78\x25\x30\x32\x78"
,IlIllIl,IIIlllIl);

spin_lock_irqsave(&IlIlI->lIIIll,flags);if(!IlIlI->lIIlIIl){IlIlI->IlIlllll=
interface->cur_altsetting->desc.bInterfaceNumber;IlIlI->lllIIIlI=IlIllIl;IlIlI->
IlIIIlIl=IIIlllIl;IlIlI->lIIlIIl=(0x1e46+1971-0x25f8);}else{
}spin_unlock_irqrestore(&IlIlI->lIIIll,flags);break;
}}}
return(0x888+2156-0x10f4);}void lIllIIlIl(struct IllII*IlIlI,struct IIIIl*IllIlI
,struct IIIIl*llllIlI){if(IllIlI){if(IllIlI->lIlII)
{usb_unlink_urb(IllIlI->lIlII);
kref_put(&IllIlI->IlIlIl,lIIlIl);
}else{lllllll(IllIlI);
}}if(llllIlI){llIII llllll;unsigned long flags;llllll=(llIII)(llllIlI+
(0x756+8035-0x26b8));llllll->llIIl.IIlIll.lIlIl=sizeof(llllll->llIIl);llllll->
llIIl.IIlIll.Status=-ECONNRESET;llllll->llIIl.llIlI=(0xb3b+6206-0x2379);
spin_lock_irqsave(&IlIlI->lIlIIll,flags);list_add_tail(&llllIlI->lIIIl,&IlIlI->
IlllIlI);spin_unlock_irqrestore(&IlIlI->lIlIIll,flags);wake_up(&IlIlI->lllIII);}
}void llIlIlIl(struct IllII*IlIlI){unsigned long flags;struct IIIIl*IllIlI,*
llllIlI;spin_lock_irqsave(&IlIlI->lIIIll,flags);IllIlI=IlIlI->llllIll;llllIlI=
IlIlI->IIIlIlI;if(IllIlI){
kref_get(&IllIlI->IlIlIl);}IlIlI->llllIll=NULL;IlIlI->IIIlIlI=NULL;
spin_unlock_irqrestore(&IlIlI->lIIIll,flags);lIllIIlIl(IlIlI,IllIlI,llllIlI);}
void IIlIlllI(struct IllII*IlIlI){unsigned long flags;struct IIIIl*IllIlI,*
llllIlI;spin_lock_irqsave(&IlIlI->lIIIll,flags);IllIlI=IlIlI->llllIll;llllIlI=
IlIlI->IIIlIlI;if(IllIlI){
kref_get(&IllIlI->IlIlIl);}IlIlI->llllIll=NULL;IlIlI->IIIlIlI=NULL;IlIlI->
lIIlIIl=(0x1c05+2044-0x2401);IlIlI->lllIIIlI=-(0x1af0+1424-0x207f);IlIlI->
IlIIIlIl=-(0xc78+222-0xd55);IlIlI->IlIlllll=-(0x1936+102-0x199b);
spin_unlock_irqrestore(&IlIlI->lIIIll,flags);lIllIIlIl(IlIlI,IllIlI,llllIlI);}
void lIllllll(struct urb*lIlII
#if KERNEL_LT((0x9f0+6284-0x227a),(0x60+2787-0xb3d),(0x475+1624-0xaba))
,struct pt_regs*IIIllIl
#endif
){struct IIIIl*lIIlI=lIlII->context;struct IllII*IlIlI=lIIlI->IlIlI;struct IIIIl
*IllIlI,*llllIlI;llIII IIlIl=(llIII)(lIIlI+(0x20b+3288-0xee2));llIII llllll;
unsigned long flags;
spin_lock_irqsave(&IlIlI->lIIIll,flags);IllIlI=IlIlI->llllIll;llllIlI=IlIlI->
IIIlIlI;if(!IllIlI||lIIlI!=IllIlI){

spin_unlock_irqrestore(&IlIlI->lIIIll,flags);}else if(!llllIlI){

IlIlI->llllIll=NULL;spin_unlock_irqrestore(&IlIlI->lIIIll,flags);}else{llllll=(
llIII)(llllIlI+(0x151b+1938-0x1cac));IlIlI->llllIll=NULL;IlIlI->IIIlIlI=NULL;if(
lIlII->status<(0x32a+2308-0xc2e)){spin_unlock_irqrestore(&IlIlI->lIIIll,flags);
llllll->llIIl.IIlIll.lIlIl=sizeof(IIlIl->llIIl);llllll->llIIl.IIlIll.Status=
lIlII->status;llllll->llIIl.llIlI=(0x9d0+5751-0x2047);spin_lock_irqsave(&IlIlI->
lIlIIll,flags);list_add_tail(&llllIlI->lIIIl,&IlIlI->IlllIlI);
spin_unlock_irqrestore(&IlIlI->lIlIIll,flags);wake_up(&IlIlI->lllIII);}else{
struct usb_device*lIIII;lIIII=lIllIIlI(IlIlI);if(lIIII){int lIlll;lIlll=lIIIlIIl
(IlIlI,lIIII,llllll,llllIlI,GFP_ATOMIC);



spin_unlock_irqrestore(&IlIlI->lIIIll,flags);if(lIlll!=(0x154+8585-0x22dd)){if(
lIlll<(0x262+6682-0x1c7c)){llllll->llIIl.IIlIll.lIlIl=sizeof(llllll->llIIl);
llllll->llIIl.IIlIll.Status=lIlII->status;llllll->llIIl.llIlI=
(0x1cd4+677-0x1f79);}spin_lock_irqsave(&IlIlI->lIlIIll,flags);list_add_tail(&
llllIlI->lIIIl,&IlIlI->IlllIlI);spin_unlock_irqrestore(&IlIlI->lIlIIll,flags);
wake_up(&IlIlI->lllIII);}IIlllllI(IlIlI,lIIII);}else{spin_unlock_irqrestore(&
IlIlI->lIIIll,flags);llllll->llIIl.IIlIll.lIlIl=sizeof(llllll->llIIl);llllll->
llIIl.IIlIll.Status=-ENODEV;llllll->llIIl.llIlI=(0x152a+3548-0x2306);
spin_lock_irqsave(&IlIlI->lIlIIll,flags);list_add_tail(&llllIlI->lIIIl,&IlIlI->
IlllIlI);spin_unlock_irqrestore(&IlIlI->lIlIIll,flags);wake_up(&IlIlI->lllIII);}
}}
IIllIII(IIlIl);
kref_put(&lIIlI->IlIlIl,lIIlIl);
}int IlIIlIIII(struct IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct 
IIIIl*lIIlI){int lIlll=(0x22a7+648-0x252f);struct urb*lIlII=NULL;do{int pipe;
lIlII=usb_alloc_urb((0xd91+1293-0x129e),GFP_ATOMIC);
if(!lIlII){IIIll(
"\x75\x73\x62\x64\x5f\x6d\x73\x74\x6f\x72\x5f\x70\x72\x65\x70\x61\x72\x65\x5f\x63\x6f\x6d\x6d\x61\x6e\x64\x3a\x20\x75\x73\x62\x5f\x61\x6c\x6c\x6f\x63\x5f\x75\x72\x62\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIlll=-ENOMEM;break;}pipe=(IIlIl->llIIl.Flags&llllII)?usb_rcvbulkpipe(lIIII,
IIlIl->llIIl.Endpoint):usb_sndbulkpipe(lIIII,IIlIl->llIIl.Endpoint);
usb_fill_bulk_urb(lIlII,lIIII,pipe,lIIlI->lllII,IIlIl->llIIl.llIlI,lIllllll,
lIIlI);if(IIlIl->llIIl.Flags&llllII){if((IIlIl->llIIl.Flags&IIlIllI)==
(0x10ac+4201-0x2115)){lIlII->transfer_flags|=URB_SHORT_NOT_OK;}}IIlIIII(IlIlI);
lIIlI->IIlIIIII=(0x141a+1953-0x1bba);lIIlI->lIlII=lIlII;lIIlI->IlIlI=IlIlI;lIIlI
->endpoint=IIlIl->llIIl.Endpoint;kref_init(&lIIlI->IlIlIl);}while(
(0x127a+5063-0x2641));if(lIlll<(0x924+822-0xc5a)){if(lIlII)usb_free_urb(lIlII);}
return lIlll;}int llIlIlII(struct IllII*IlIlI,struct usb_device*lIIII,llIII 
IIlIl,struct IIIIl*lIIlI){int lIllIII;unsigned long flags;struct IIIIl*IllIlI;
llIII lllllllll;
if(!IlIlI->lIIlIIl)return-(0x25f+6481-0x1baf);
lIllIII=IIlIl->llIIl.Endpoint|((IIlIl->llIIl.Flags&llllII)?(0xec1+4303-0x1f10):
(0xd87+562-0xfb9));
if(lIllIII!=IlIlI->lllIIIlI&&lIllIII!=IlIlI->IlIIIlIl)return-
(0x1264+4854-0x2559);spin_lock_irqsave(&IlIlI->lIIIll,flags);if(!IlIlI->llllIll)
{struct llIIIIIll*IlIIIIll;


if(!lIIlI->IlllIII)
{spin_unlock_irqrestore(&IlIlI->lIIIll,flags);return-(0x95d+1526-0xf52);}
IlIIIIll=lIIlI->lllII;if(IlIIIIll&&lIllIII==IlIlI->IlIIIlIl&&IIlIl->llIIl.llIlI
==sizeof(struct llIIIIIll)&&IlIIIIll->lIIIIlllI==1128420181&&IlIIIIll->IIlIlIlIl
){
IllIlI=llIIlIIIl(lIIlI);if(IllIlI){IlIlI->llllIll=IllIlI;spin_unlock_irqrestore(
&IlIlI->lIIIll,flags);IIlIl->llIIl.IIlIll.lIlIl=sizeof(IIlIl->llIIl);IIlIl->
llIIl.IIlIll.Status=(0xd3d+2182-0x15c3);return(0x1896+3121-0x24c6);
}else{IIIIlll(
"\x75\x73\x62\x64\x3a\x20\x6f\x75\x74\x20\x6f\x66\x20\x6b\x65\x72\x6e\x65\x6c\x20\x6d\x65\x6d\x6f\x72\x79\x21"
);}}}else if(!IlIlI->IIIlIlI){


int lIlll;IllIlI=IlIlI->llllIll;lllllllll=(llIII)(IllIlI+(0x15e5+2767-0x20b3));
lIlll=IlIIlIIII(IlIlI,lIIII,lllllllll,IllIlI);if(lIlll<(0x545+2111-0xd84)){
lllllll(IlIlI->llllIll);IlIlI->llllIll=NULL;spin_unlock_irqrestore(&IlIlI->
lIIIll,flags);IIlIl->llIIl.IIlIll.lIlIl=sizeof(IIlIl->llIIl);IIlIl->llIIl.IIlIll
.Status=lIlll;IIlIl->llIIl.llIlI=(0xf07+515-0x110a);return(0x848+109-0x8b4);
}IlIlI->IIIlIlI=lIIlI;spin_unlock_irqrestore(&IlIlI->lIIIll,flags);lIlll=
usb_submit_urb(IllIlI->lIlII,GFP_KERNEL);
if(lIlll<(0x1168+2332-0x1a84)){

IllIlI->lIlII->status=lIlll;
#if KERNEL_LT((0x1117+5583-0x26e4),(0x1a93+3115-0x26b8),(0x128+8999-0x243c))
lIllllll(IllIlI->lIlII,NULL);
#else
lIllllll(IllIlI->lIlII);
#endif
}return(0xd8a+343-0xee1);
}else{


llIII llllll;llllll=(llIII)(IlIlI->IIIlIlI+(0x3f+9542-0x2584));
if((llllll->llIIl.Endpoint==IIlIl->llIIl.Endpoint)&&((llllll->llIIl.Flags&llllII
)==(IIlIl->llIIl.Flags&llllII))){





while(IlIlI->llllIll||IlIlI->IIIlIlI){spin_unlock_irqrestore(&IlIlI->lIIIll,
flags);msleep_interruptible((0x13b3+168-0x145a));spin_lock_irqsave(&IlIlI->
lIIIll,flags);}}}spin_unlock_irqrestore(&IlIlI->lIIIll,flags);return-
(0xca+4116-0x10dd);
}
#endif 

