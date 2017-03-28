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

#ifndef lIlIlIlIl
#define lIlIlIlIl
#ifdef _USBD_ENABLE_VHCI_
#include "vhci_hcd.h"

#if KERNEL_LT((0x1372+2817-0x1e71),(0x8d+2979-0xc2a),(0x8dd+2754-0x138b))
#  define IlIlII	(0x57+8490-0x2172)
#else
#  if USB_MAXCHILDREN < (0x24a+457-0x404)
#    define IlIlII USB_MAXCHILDREN
#  else
#    define IlIlII (0x6da+1374-0xc29)
#  endif
#endif

#define lllIIlll ((IlIlII / (0xb15+3673-0x1966)) + (0x1795+71-0x17db))
struct IIIII{struct list_head llIIll;struct llIll*IIlII;struct kref IlIlIl;int 
IlllI;int llIlIIl;int speed;int llllllll;u32 IlIlIll;u32 IllllIl;struct usb_hcd*
parent;wait_queue_head_t lllIII;spinlock_t IIllI;struct list_head lllIll;
spinlock_t IIllII;struct list_head llIIIIl;spinlock_t lIlIlll;struct list_head 
IlIIlll;void*IIllllI;
void*IIIllIllI;
void*IIIIlIIl;
};struct lIllIll{struct list_head llIIll;struct urb*lIlII;};

struct lIlIII{struct list_head lIIIl;struct IIIII*llllI;lIlIIlI IlIIII;struct 
list_head llIIlIl;struct lIllIll urb;llIII IIlIl;};struct IIlllI{spinlock_t lock
;struct IIIII*IlIIll[IlIlII];int llIllIl[IlIlII];int llIlll[IlIlII];unsigned 
short IIIIIIl[IlIlII];unsigned short lIIIIl[IlIlII];unsigned long lIlIlII[IlIlII
];struct usb_hcd*IllIl;struct usb_hcd*lllllII;unsigned llIIlIlI:
(0xc56+3496-0x19fd);};enum lIlIllIlI{lIIlIll=(0x24e6+400-0x2676),
lIllIIIl,
IIlllII,
IllIllI,
IlIlllI,
IIlIlII,
IllIlllI,
lllIIllI,
};enum IIlllllII{IIllllII,
IIIlIIIl,
llllIllI,
IlIIllIl,
IllIlll,
llIlllIl,
IIllIlI,
IlllIllI,
llIIIll,
lIIlIIll,
llIIIIIII,
IlllIIlIl,
llIIlIII,
lllIlll,
IlIllIll,
lIlIIIIlI,
lIIlIIlI,
llllllIl,
lIlllIll,
IlIlIlIlI,
};int llIlIIIIl(void);void IllIIllIl(void);struct IIIII*IlIIIIlIl(u32 IlIlIll,
u32 IllllIl,int speed);void IllllIIl(struct IIIII*lllll);void llIlIlIll(struct 
IIIII*lllll);void llIIIIll(struct IIIII*lllll);struct IIIII*lIIlIlIl(u32 IlIlIll
,u32 IllllIl);struct IIIII*lIIIlllll(struct usb_hcd*IllIl,int llIlIIl);struct 
IIIII*IlIlllIII(struct usb_hcd*IllIl,int IlllI);void IlllllIl(struct IIIII*llllI
);struct lIlIII*llIIllIll(struct IIIII*llllI,struct urb*lIlII);void IllIIIIl(
struct urb*lIlII,int status);void IIlIIlII(struct IIIII*llllI);void llIllll(
struct IIIII*llllI,struct lIlIII*lIIll,int status);int IlIIllIll(struct IIIII*
llllI,struct lIlIII*IllllIlII,llIII IIlIl);int IIIIIIll(struct IIIII*llllI,
struct lIlIII*lIIll,llIII IIlIl);int IlllIlII(struct IIIII*llllI,struct lIlIII**
llllIIl,struct urb*lIlII);int lIlIIlIIl(struct lIlIII*lIIll);
#define lIIIllII(lllll) kref_get(&(lllll)->IlIlIl)
#define IIIIllI(lllll) kref_put(&(lllll)->IlIlIl, lllllllI)
void lllllllI(struct kref*IlllIIl);
#endif 
#endif 

