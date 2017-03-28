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

#ifndef IllllIlll
#define IllllIlll
#define lIIllIl (0x1246+303-0x1365)
struct lllIlI;extern int IllllllI;typedef void(*urb_chain_complete_t)(struct 
lllIlI*);struct lllIlI{spinlock_t lock;struct urb*IlIll[lIIllIl];struct 
usb_device*llllI;int pipe;int lIIIIll;int lllIlIIl;int status;size_t lIlIlllI;
unsigned char IlIII;atomic_t lllllIII;void*context;urb_chain_complete_t complete
;};struct IIIIIlI{void*IllIIIl[lIIllIl];
size_t llIlIl[lIIllIl];
size_t lIIIlII[lIIllIl];
char lllIllI[lIIllIl];
unsigned char IlIII;};int IIIIIIlI(struct lllIlI*lIllI,int pipe,int interval,int
 lllIlIIl,int lIIIIll,struct usb_device*llllI,struct IIIIIlI*lllIIlI,void*
context,urb_chain_complete_t complete,gfp_t lIIllI);int lllIllll(struct lllIlI*
lIllI);int lIIIlIII(struct lllIlI*lIllI);int IIlIIlIII(struct lllIlI*lIllI);int 
IlllIllll(struct lllIlI*lIllI);int lIlIlIII(struct lllIlI*lIllI);int lIIlIllll(
struct lllIlI*lIllI,struct urb*lIlII);int IlllIlIlI(struct IIIIIlI*lIllI,size_t 
length,int llIIllIII);int llIIIIlII(struct IIIIIlI*lIllI,size_t length,llllIII*
lllIllI,IIIlll IIIIlII);int IIIIllII(struct IIIIIlI*lIllI,const void*IlIIl,
size_t length);int IIllllll(struct IIIIIlI*lIllI,void*IlIIl,size_t length);int 
lIlIIIIl(struct IIIIIlI*lIllI);size_t llllIIlll(struct IIIIIlI*lIllI,llllIII*
lllIllI,IIIlll IIIIlII);size_t IlllllllI(struct IIIIIlI*lIllI,llllIII*lllIllI,
IIIlll IIIIlII);
#endif 

