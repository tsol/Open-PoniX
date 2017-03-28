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

#ifndef IIlIIIIIl
#define IIlIIIIIl
struct IIlIlll{struct list_head IIIIlIl;
spinlock_t lock;int lllllIll;
wait_queue_head_t IIIllIll;};struct lIlllIIl{struct list_head entry;int llIlIII;
int read;
char lIllIlllI:(0xd7b+3653-0x1bbf);char llIIIllll:(0x10e6+4762-0x237f);union{u8*
IllllIlII;u8 IIIIlIlI[(0x964+5197-0x1db0)];};};struct IIlIlll*IlIllIII(void);
void llIlIIlI(struct IIlIlll*IIlIIIIlI);void IllIllII(struct IIlIlll*IIlIIIIlI);
int IIIlIIIll(struct IIlIlll*llllIl,const void*IlIIl,int IlIlll,int IIIlIll);
int IIlIIIlIl(struct IIlIlll*llllIl,const void*IlIIl,int IlIlll,int IIIlIll);int
 IIIIlIIlI(struct IIlIlll*IIlIIIIlI,void*IlIIl,int IlIlll,int IIIlIll);
#endif 

