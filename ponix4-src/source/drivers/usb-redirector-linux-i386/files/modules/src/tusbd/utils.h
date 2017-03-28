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

#ifndef IIIlIlllI
#define IIIlIlllI
#include "urb_chain.h"
struct IIIIl{size_t llIlIII;struct list_head lIIIl;struct IllII*IlIlI;struct 
kref IlIlIl;
struct urb*lIlII;
void*lllII;
struct lllIlI IlllII;
struct IIIIIlI IlIllII;
lllIl endpoint;
char IlllIII:(0x731+4149-0x1765);
char IIlIIIII:(0x194d+609-0x1bad);


};
#ifdef _USBD_ENABLE_STUB_
struct IIIIl*lIlIlIlI(struct IllII*IlIlI,struct usb_device*lIIII,const void 
__user*IlIIl,size_t IlIII);size_t lllllIIII(struct IIIIl*lIIlI,void __user*IlIIl
,size_t IlIII);struct IIIIl*llIIlIIIl(struct IIIIl*lIIlI);void lllllll(struct 
IIIIl*lIIlI);
#endif
void IIlIIlll(struct urb*lIlII,lIlIIlI IlIIII);void IIllIII(llIII IIlIl);
#ifdef _USBD_ENABLE_VHCI_
int lllIllIl(struct list_head*lIllIIl,lIlIIlI IlIIII,void*IlIIl);int llIllllII(
llIII IIlIl,struct list_head*lIllIIl);
#endif
int IlIIlIIl(IIIlll status);int IlIIIIIII(void*IIIllIII,struct vm_area_struct*
IlIIIll);lIlIIlI IlIlllIl(void);size_t IllIIIII(struct usb_iso_packet_descriptor
*lIIIllll,int IlIII,void*IIIIlI,void*lIIIII,int lIIlllIII);void IIIlIllI(
dma_addr_t IIIIlI,void*lIIIII,unsigned long IIIIII);void llIlIlll(void*IIIIlI,
dma_addr_t lIIIII,unsigned long IIIIII);
#if KERNEL_GT_EQ((0x869+3755-0x1712),(0xd7d+937-0x1120),(0x1542+1930-0x1cae))
#define IIlIIIl(IlIIllII) dev_name((IlIIllII))
#else
#define IIlIIIl(IlIIllII) (IlIIllII)->bus_id
#endif
#endif 

