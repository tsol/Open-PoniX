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

#ifndef lIlIlIlll
#define lIlIlIlll
#ifdef _USBD_ENABLE_STUB_
#ifndef IllIlII
#define IllIlII (0x215+6643-0x1bf4)
#endif
struct IllII{struct list_head llIIll;struct llIll*IIlII;struct kref IlIlIl;u32 
IIIlII;u16 vid;u16 IIlIIl;char bus_id[IllIlII];IIlIllII state;struct usb_device*
lIIII;spinlock_t lIIIIII;int lIllIlI;int lIIIIll;int IIllllIl;int lIlIllII;
int llIllIll;int lllIlllI[(0x128+3063-0xd0f)];int IlIIIIlI[(0x5ff+5549-0x1b9c)];
int IIllIlIl[(0x9e2+662-0xc68)];int IIllIIll[(0x868+6440-0x2180)];
wait_queue_head_t lllIII;spinlock_t IIllI;struct list_head lllIll;spinlock_t 
lIlIIll;struct list_head IlllIlI;wait_queue_head_t IlIIIIIl;
spinlock_t lIIIll;int lIIlIIl;int IlIlllll;struct IIIIl*llllIll;struct IIIIl*
IIIlIlI;int lllIIIlI;int IlIIIlIl;};struct IlIIIlll{struct list_head lIIIl;u32 
IIIlII;u16 vid;u16 IIlIIl;char bus_id[IllIlII];};int IIlllIII(void);void 
IIIlIlIll(void);struct IllII*llllIIlI(u16 vid,u16 IIlIIl,const char*bus_id);void
 IlIlllII(struct IllII*IlIlI);struct IllII*IlllIIlI(u16 vid,u16 IIlIIl,char*
bus_id);struct IllII*IIllIIIl(u32 IIIlII);struct IllII*IlllIIIlI(int IlllIl);int
 IllIIlll(struct IllII*IlIlI);int IllIIlIl(struct IllII*IlIlI,IIlIllII IIllIIII)
;void IlllIlll(struct IllII*IlIlI,struct usb_device*lIIII,int IIIIllll);int 
lIIllIll(struct IllII*IlIlI,struct usb_device*lIIII,int lIlIIIll,int IIIIlIIII);
int IIIIlIII(struct IllII*IlIlI,int pipe);int lIIIIlI(struct IllII*IlIlI,int 
pipe);void IlIIIIlII(struct IllII*IlIlI,int pipe);int lIlllIII(struct IllII*
IlIlI,int pipe);void lIIlIIIl(struct IllII*IlIlI,int pipe,int IIIllll);
#define IIlIIII(IlIlIIlI) {if((IlIlIIlI)) kref_get(&(IlIlIIlI)->IlIlIl);}
#define llIllII(IlIlIIlI) {if((IlIlIIlI)) kref_put(&(IlIlIIlI)->IlIlIl, llIIIllI\
);}
void llIIIllI(struct kref*IlllIIl);struct usb_device*lIllIIlI(struct IllII*IlIlI
);void IIlllllI(struct IllII*IlIlI,struct usb_device*lIIII);


#pragma pack(push,1)


struct llIIIIIll{IIIlll lIIIIlllI;IIIlll IllIlllII;IIIlll IIlIlIlIl;lllIl 
lIlIlIIII;lllIl IlIlIlIII;lllIl IlIIIlIII;lllIl lIllIIlll[(0xec+9055-0x243b)];};

struct lllIllllI{IIIlll IlIIllIII;IIIlll IIIllIlIl;IIIlll lllIlIIlI;lllIl 
IlIllIllI;};
#pragma pack(pop)
int IIIllllI(struct IllII*IlIlI,struct usb_device*lIlIIl);int llIlIlII(struct 
IllII*IlIlI,struct usb_device*lIIII,llIII IIlIl,struct IIIIl*lIIlI);
#endif 
#endif 

