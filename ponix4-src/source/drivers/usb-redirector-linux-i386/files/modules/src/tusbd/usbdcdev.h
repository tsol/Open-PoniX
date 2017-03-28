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

#ifndef lIlllllII
#define lIlllllII
struct llIll{struct{ssize_t(*read)(void*,char __user*,size_t);ssize_t(*write)(
void*,const char __user*,size_t);long(*unlocked_ioctl)(void*,unsigned int,
unsigned long);long(*compat_ioctl)(void*,unsigned int,unsigned long);int(*open)(
void*);int(*release)(void*);unsigned int(*poll)(void*,struct file*IllIIl,
poll_table*);int(*mmap)(void*,struct vm_area_struct*IlIIIll);}ops;int IlllIl;
void*context;struct IIlIlll*llIllI;};int llIlllII(int*IlIlIIll);int IllllIII(
void);int lIIIlIlII(struct cdev*IlIllIIl);int IIIIIIIll(struct cdev*IlIllIIl);
int IIIllIIl(struct llIll*lIIIIllI);void IIllIlII(int IlllIl);void*IllIllllI(int
 IlllIl);dev_t lllIllIII(int IlllIl);
#endif 

