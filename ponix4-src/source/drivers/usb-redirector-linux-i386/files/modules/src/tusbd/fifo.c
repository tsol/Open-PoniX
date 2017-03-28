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
struct IIlIlll*IlIllIII(void){struct IIlIlll*llllIl;llllIl=kmalloc(sizeof(*
llllIl),GFP_KERNEL);if(llllIl){memset(llllIl,(0x1a7d+3073-0x267e),sizeof(*llllIl
));INIT_LIST_HEAD(&llllIl->IIIIlIl);spin_lock_init(&llllIl->lock);
init_waitqueue_head(&llllIl->IIIllIll);return llllIl;}return NULL;}void llIlIIlI
(struct IIlIlll*llllIl){if(!llllIl)return;IllIllII(llllIl);kfree(llllIl);}void 
IllIllII(struct IIlIlll*llllIl){spin_lock(&llllIl->lock);while(!list_empty(&
llllIl->IIIIlIl)){struct list_head*entry=llllIl->IIIIlIl.next;list_del(entry);
kfree(entry);}llllIl->lllllIll=(0x66f+5254-0x1af5);spin_unlock(&llllIl->lock);}
int IIIIlIIlI(struct IIlIlll*llllIl,void*IlIIl,int IlIlll,int IIIlIll){int lIlll
=(0xe5a+5396-0x236e);if(!llllIl)return-EINVAL;if(!IlIIl)return-EINVAL;if(!IlIlll
)return-EINVAL;spin_lock(&llllIl->lock);if(!list_empty(&llllIl->IIIIlIl)){struct
 lIlllIIl*IIIIIl;IIIIIl=(struct lIlllIIl*)llllIl->IIIIlIl.next;
lIlll=min(IlIlll,IIIIIl->llIlIII-IIIIIl->read);if(IIIlIll){if(copy_to_user(IlIIl
,IIIIIl->IIIIlIlI+IIIIIl->read,lIlll)!=(0xa4c+2459-0x13e7)){IIIll(
"\x75\x73\x62\x64\x5f\x66\x69\x66\x6f\x5f\x67\x65\x74\x3a\x20\x63\x61\x6e\x6e\x6f\x74\x20\x63\x6f\x70\x79\x20\x74\x6f\x20\x75\x73\x65\x72\x20\x62\x75\x66\x66\x65\x72" "\n"
);}}else{memcpy(IlIIl,IIIIIl->IIIIlIlI+IIIIIl->read,lIlll);}IIIIIl->read+=lIlll;
if(IIIIIl->read==IIIIIl->llIlIII){list_del(&IIIIIl->entry);kfree(IIIIIl);llllIl
->lllllIll--;}}spin_unlock(&llllIl->lock);return lIlll;}int IIIlIIIll(struct 
IIlIlll*llllIl,const void*IlIIl,int IlIlll,int IIIlIll){struct lIlllIIl*IIIIIl;
if(!llllIl)return-EINVAL;if(!IlIIl)return-EINVAL;if(!IlIlll)return-EINVAL;if(
in_atomic()){IIIIIl=kmalloc(sizeof(*IIIIIl)+IlIlll-(0xaa+5835-0x1774),GFP_ATOMIC
);}else{IIIIIl=kmalloc(sizeof(*IIIIIl)+IlIlll-(0x15e0+1143-0x1a56),GFP_KERNEL);}
if(IIIIIl){if(IIIlIll){if(copy_from_user(IIIIIl->IIIIlIlI,IlIIl,IlIlll)!=
(0x3c0+113-0x431)){IIIll(
"\x75\x73\x62\x64\x5f\x66\x69\x66\x6f\x5f\x67\x65\x74\x3a\x20\x63\x61\x6e\x6e\x6f\x74\x20\x63\x6f\x70\x79\x20\x74\x6f\x20\x75\x73\x65\x72\x20\x62\x75\x66\x66\x65\x72" "\n"
);}}else{memcpy(IIIIIl->IIIIlIlI,IlIIl,IlIlll);}IIIIIl->read=(0x4a6+6168-0x1cbe)
;IIIIIl->llIlIII=IlIlll;spin_lock(&llllIl->lock);list_add_tail(&IIIIIl->entry,&
llllIl->IIIIlIl);llllIl->lllllIll++;spin_unlock(&llllIl->lock);wake_up(&llllIl->
IIIllIll);return(0x27c+5787-0x1917);}return-ENOMEM;}int IIlIIIlIl(struct IIlIlll
*llllIl,const void*IlIIl,int IlIlll,int IIIlIll){struct lIlllIIl*IIIIIl;if(!
llllIl)return-EINVAL;if(!IlIIl)return-EINVAL;if(!IlIlll)return-EINVAL;if(
in_atomic()){IIIIIl=kmalloc(sizeof(*IIIIIl)+IlIlll-(0xe90+567-0x10c6),GFP_ATOMIC
);}else{IIIIIl=kmalloc(sizeof(*IIIIIl)+IlIlll-(0x387+4151-0x13bd),GFP_KERNEL);}
if(IIIIIl){if(IIIlIll){if(copy_from_user(IIIIIl->IIIIlIlI,IlIIl,IlIlll)!=
(0xb60+335-0xcaf)){IIIll(
"\x75\x73\x62\x64\x5f\x66\x69\x66\x6f\x5f\x67\x65\x74\x3a\x20\x63\x61\x6e\x6e\x6f\x74\x20\x63\x6f\x70\x79\x20\x74\x6f\x20\x75\x73\x65\x72\x20\x62\x75\x66\x66\x65\x72" "\n"
);}}else{memcpy(IIIIIl->IIIIlIlI,IlIIl,IlIlll);}IIIIIl->read=(0x362+7991-0x2299)
;IIIIIl->llIlIII=IlIlll;spin_lock(&llllIl->lock);list_add_tail(&IIIIIl->entry,&
llllIl->IIIIlIl);llllIl->lllllIll++;spin_unlock(&llllIl->lock);wake_up(&llllIl->
IIIllIll);return(0x161a+1538-0x1c1c);}return-ENOMEM;}
