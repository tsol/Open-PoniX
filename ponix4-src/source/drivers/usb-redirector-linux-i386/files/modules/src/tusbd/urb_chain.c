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
int IllllllI=32768;
int IlIIlIII(struct lllIlI*lIllI,struct urb*IIIIIlIl);void IIIIllIl(struct urb*
lIlII
#if KERNEL_LT((0xd89+3493-0x1b2c),(0x5a0+7284-0x220e),(0xd54+3878-0x1c67))
,struct pt_regs*IIIllIl
#endif
);int IIIIIIlI(struct lllIlI*lIllI,int pipe,int interval,int lllIlIIl,int 
lIIIIll,struct usb_device*llllI,struct IIIIIlI*lllIIlI,void*context,
urb_chain_complete_t complete,gfp_t lIIllI){int i,lIlll=(0xf43+1769-0x162c);
struct usb_host_endpoint*ep;IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x69\x6e\x69\x74\x3a\x20\x2b\x2b" "\n");ep=
usb_pipein(pipe)?llllI->ep_in[usb_pipeendpoint(pipe)]:llllI->ep_out[
usb_pipeendpoint(pipe)];if(!ep){IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x69\x6e\x69\x74\x3a\x20\x65\x6e\x64\x70\x6f\x69\x6e\x74\x20\x69\x73\x20\x4e\x55\x4c\x4c" "\n"
);IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x69\x6e\x69\x74\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x2d\x31" "\n"
);return-(0xc34+4773-0x1ed8);}lIllI->pipe=pipe;lIllI->lllIlIIl=lllIlIIl;lIllI->
lIIIIll=lIIIIll;lIllI->llllI=llllI;lIllI->IlIII=lllIIlI->IlIII;lIllI->status=
(0x476+3982-0x1404);lIllI->lIlIlllI=(0xf16+2791-0x19fd);lIllI->context=context;
lIllI->complete=complete;spin_lock_init(&lIllI->lock);for(i=(0x93d+2820-0x1441);
i<lIllI->IlIII;i++){
if(lIlll<(0x95c+5152-0x1d7c))break;lIllI->IlIll[i]=usb_alloc_urb(lllIIlI->
lllIllI[i],lIIllI);if(!lIllI->IlIll[i]){IIIIlll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x69\x6e\x69\x74\x3a\x20\x66\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x61\x6c\x6c\x6f\x63\x61\x74\x65\x20\x61\x6e\x20\x75\x72\x62\x20\x2d\x20\x6e\x6f\x20\x6d\x65\x6d\x6f\x72\x79" "\n"
);lIlll=-ENOMEM;break;}lIllI->IlIll[i]->transfer_flags|=URB_NO_INTERRUPT;switch(
usb_pipetype(pipe)){case PIPE_BULK:usb_fill_bulk_urb(lIllI->IlIll[i],lIllI->
llllI,pipe,lllIIlI->IllIIIl[i],usb_pipein(pipe)?lllIIlI->llIlIl[i]:lllIIlI->
lIIIlII[i],IIIIllIl,lIllI);if(usb_pipein(pipe))lIllI->IlIll[i]->transfer_flags|=
URB_SHORT_NOT_OK;break;case PIPE_ISOCHRONOUS:lIllI->IlIll[i]->dev=llllI;lIllI->
IlIll[i]->pipe=pipe;lIllI->IlIll[i]->transfer_flags=URB_ISO_ASAP;lIllI->IlIll[i]
->transfer_buffer=lllIIlI->IllIIIl[i];lIllI->IlIll[i]->transfer_buffer_length=
usb_pipein(pipe)?lllIIlI->llIlIl[i]:lllIIlI->lIIIlII[i];lIllI->IlIll[i]->
start_frame=-(0x447+391-0x5cd);lIllI->IlIll[i]->number_of_packets=lllIIlI->
lllIllI[i];lIllI->IlIll[i]->context=lIllI;lIllI->IlIll[i]->complete=IIIIllIl;if(
interval){lIllI->IlIll[i]->interval=interval;}else{lIllI->IlIll[i]->interval=ep?
ep->desc.bInterval:(0x1065+1347-0x15a7);lIllI->IlIll[i]->interval=
(0xbad+2233-0x1465)<<(lIllI->IlIll[i]->interval-(0x18c+7338-0x1e35));}break;
default:IIIIlll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x69\x6e\x69\x74\x3a\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x70\x69\x70\x65\x20\x74\x79\x70\x65" "\n"
);lIlll=-EINVAL;break;}lIllI->IlIll[i]->dev=NULL;
}if(lIlll==(0x161+6661-0x1b66)){
lIllI->IlIll[lIllI->IlIII-(0x12f9+1718-0x19ae)]->transfer_flags&=~
URB_NO_INTERRUPT;}else{
for(i--;i>=(0x1c1b+2363-0x2556);i--){usb_free_urb(lIllI->IlIll[i]);lIllI->IlIll[
i]=NULL;}}IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x69\x6e\x69\x74\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}int lllIllll(struct lllIlI*lIllI){int i,lIlll=
(0x742+4666-0x197c);IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x63\x6c\x65\x61\x6e" "\n");lIlIlIII(lIllI);for
(i=(0xec5+3200-0x1b45);i<lIllI->IlIII;i++)if(lIllI->IlIll[i])usb_free_urb(lIllI
->IlIll[i]);return lIlll;}void IIIIllIl(struct urb*lIlII
#if KERNEL_LT((0x13f6+144-0x1484),(0x17f+7716-0x1f9d),(0x501+6849-0x1faf))
,struct pt_regs*IIIllIl
#endif
){struct lllIlI*lIllI=lIlII->context;

if(lIlII->status&&lIlII->status!=-ECONNRESET&&lIlII->status!=-ENOENT){int abort=
(0xc80+4205-0x1ced);spin_lock(&lIllI->lock);if(lIllI->status==
(0x25c+4504-0x13f4)){
if(lIlII->status==-EREMOTEIO&&lIllI->lllIlIIl){lIllI->status=
(0x1435+2999-0x1fec);IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x3a\x20\x73\x68\x6f\x72\x74\x20\x74\x72\x61\x6e\x73\x66\x65\x72\x20\x64\x65\x74\x65\x63\x74\x65\x64\x20\x61\x6e\x64\x20\x69\x73\x20\x6f\x6b\x2e\x20\x61\x63\x74\x75\x61\x6c\x5f\x6c\x65\x6e\x67\x74\x68\x3d\x25\x64" "\n"
,lIlII->actual_length);}else{lIllI->status=lIlII->status;IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x3a\x20\x65\x61\x72\x6c\x79\x20\x75\x72\x62\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x20\x77\x69\x74\x68\x20\x73\x74\x61\x74\x75\x73\x20\x25\x64" "\n"
,lIlII->status);}abort=(0xa71+1765-0x1155);}spin_unlock(&lIllI->lock);if(abort)
IlIIlIII(lIllI,lIlII);}
#if defined(_USBD_USE_EHCI_FIX_) && KERNEL_GT_EQ((0x58d+2977-0x112c),\
(0x10ed+2659-0x1b4a),(0x1a2f+567-0x1c4d)) && KERNEL_LT_EQ((0x2143+208-0x2211),\
(0x159d+2321-0x1ea8),(0x1f76+169-0x2003))
if(lIllI->lIIIIll&&usb_pipeisoc(lIlII->pipe)&&lIlII->status==(0x99d+37-0x9c2)){
if(atomic_read(&lIlII->kref.refcount)>(0x1d9b+565-0x1fcf))usb_put_urb(lIlII);}
#endif
lIlII->dev=NULL;
lIllI->lIlIlllI+=lIlII->actual_length;
if(atomic_dec_and_test(&lIllI->lllllIII)){IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x75\x72\x62\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x3a\x20\x61\x6c\x6c\x20\x75\x72\x62\x73\x20\x66\x69\x6e\x69\x73\x68\x65\x64\x20\x2d\x20\x63\x6f\x6d\x70\x6c\x65\x74\x69\x6e\x67\x20\x74\x68\x65\x20\x63\x68\x61\x69\x6e" "\n"
);lIllI->complete(lIllI);}}int lIIIlIII(struct lllIlI*lIllI){int i,IlIIIl,lIlll=
(0x6f3+652-0x97f);unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x73\x75\x62\x6d\x69\x74\x3a\x20\x2b\x2b" "\n")
;atomic_set(&lIllI->lllllIII,lIllI->IlIII);spin_lock_irqsave(&lIllI->lock,flags)
;for(i=(0xd3c+5379-0x223f),IlIIIl=(0x1d0+1597-0x80d);i<lIllI->IlIII;i++){if(
lIllI->status!=(0x445+4869-0x174a))break;lIllI->IlIll[i]->dev=lIllI->llllI;
lIlll=usb_submit_urb(lIllI->IlIll[i],GFP_ATOMIC);
if(lIlll!=(0x472+3453-0x11ef))lIllI->IlIll[i]->dev=NULL;
spin_unlock_irqrestore(&lIllI->lock,flags);if(lIlll==-ENXIO||lIlll==-EAGAIN||
lIlll==-ENOMEM){IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x73\x75\x62\x6d\x69\x74\x3a\x20\x72\x65\x74\x72\x79\x69\x6e\x67\x20\x66\x61\x69\x6c\x65\x64\x20\x75\x72\x62" "\n"
);
if(IlIIIl++<(0x10a9+5725-0x26fc)){i--;lIlll=(0x9c5+3540-0x1799);yield();}}else{
cpu_relax();IlIIIl=(0xd23+3184-0x1993);}spin_lock_irqsave(&lIllI->lock,flags);if
(lIlll!=(0x1505+2395-0x1e60)){IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x73\x75\x62\x6d\x69\x74\x3a\x20\x73\x75\x62\x6d\x69\x74\x20\x66\x61\x69\x6c\x65\x64" "\n"
);lIllI->status=-(0x2230+160-0x22cf);break;}}spin_unlock_irqrestore(&lIllI->lock
,flags);
if(lIlll<(0x2bd+5447-0x1804))IlIIlIII(lIllI,NULL);else{int IIllIlIIl=lIllI->
IlIII-i;
if(IIllIlIIl>(0xac7+2271-0x13a6)&&atomic_sub_and_test(IIllIlIIl,&lIllI->lllllIII
)){IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x73\x75\x62\x6d\x69\x74\x3a\x20\x69\x6f\x63\x6f\x6e\x74\x20\x69\x73\x20\x30\x2c\x20\x63\x61\x6c\x6c\x69\x6e\x67\x20\x63\x6f\x6d\x70\x6c\x65\x74\x69\x6f\x6e" "\n"
);lIllI->complete(lIllI);}}IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x73\x75\x62\x6d\x69\x74\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}int IIlIIlIII(struct lllIlI*lIllI){int i;for(i=
(0x1c89+1062-0x20af);i<lIllI->IlIII;i++)usb_get_urb(lIllI->IlIll[i]);return
(0x6f0+2025-0xed9);}int IlllIllll(struct lllIlI*lIllI){int i;for(i=
(0x159d+3002-0x2157);i<lIllI->IlIII;i++)usb_put_urb(lIllI->IlIll[i]);return
(0x13b8+2583-0x1dcf);}int lIlIlIII(struct lllIlI*lIllI){unsigned long flags;int 
abort=(0xdd7+5145-0x21f0);IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x63\x61\x6e\x63\x65\x6c\x3a\x20\x2b\x2b" "\n")
;spin_lock_irqsave(&lIllI->lock,flags);if(lIllI->status==(0x1ac+7602-0x1f5e)){
lIllI->status=-ECONNRESET;abort=(0x1b7+2243-0xa79);}spin_unlock_irqrestore(&
lIllI->lock,flags);if(abort)IlIIlIII(lIllI,NULL);IIIll(
"\x75\x73\x62\x64\x5f\x75\x63\x5f\x63\x61\x6e\x63\x65\x6c\x3a\x20\x2d\x2d" "\n")
;return(0x48+8001-0x1f89);}int IlIIlIII(struct lllIlI*lIllI,struct urb*IIIIIlIl)
{int i,lIIlllII=(0x5dd+8007-0x2524);for(i=(0x361+3003-0xf1c);i<lIllI->IlIII;i++)
{if(!lIIlllII&&IIIIIlIl){if(lIllI->IlIll[i]==IIIIIlIl)lIIlllII=
(0x1175+4495-0x2303);}else{if(lIllI->IlIll[i]&&lIllI->IlIll[i]->dev)
usb_unlink_urb(lIllI->IlIll[i]);}}return(0x932+6730-0x237c);}int lIIlIllll(
struct lllIlI*lIllI,struct urb*lIlII){int i;for(i=(0x7aa+4461-0x1917);i<lIllI->
IlIII;i++)if(lIllI->IlIll[i]==lIlII)break;return(i==lIllI->IlIII)?-
(0x749+7907-0x262b):i;}





int IlllIlIlI(struct IIIIIlI*lIllI,size_t length,int llIIllIII){size_t IllIlIlI,
lIIIIIl;int i;IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x3a\x20\x2b\x2b" "\n");
IllIlIlI=IllllllI-(IllllllI%llIIllIII);IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x3a\x20\x70\x61\x72\x74\x73\x69\x7a\x65\x20\x3d\x20\x25\x6c\x75" "\n"
,(unsigned long)IllIlIlI);lIllI->IlIII=(0x984+3825-0x1875);for(i=
(0xc32+6868-0x2706);i<lIIllIl&&length>(0x19ea+294-0x1b10);i++,length-=lIIIIIl){
lIIIIIl=min(IllIlIlI,length);IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x3a\x20\x61\x6c\x6c\x6f\x63\x61\x74\x69\x6e\x67\x20\x25\x6c\x75\x20\x62\x79\x74\x65\x73" "\n"
,(unsigned long)lIIIIIl);lIllI->IllIIIl[i]=kmalloc(lIIIIIl,GFP_KERNEL);lIllI->
llIlIl[i]=lIIIIIl;lIllI->lIIIlII[i]=(0x10ed+3430-0x1e53);lIllI->lllIllI[i]=
(0x66+3924-0xfba);if(!lIllI->IllIIIl[i]){IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x3a\x20\x6b\x6d\x61\x6c\x6c\x6f\x63\x20\x66\x61\x69\x6c\x65\x64" "\n"
);break;}}
if(length)for(i--;i>=(0xc40+5569-0x2201);i--){kfree(lIllI->IllIIIl[i]);lIllI->
IllIIIl[i]=NULL;lIllI->llIlIl[i]=(0x776+4118-0x178c);}else lIllI->IlIII=i;IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIllI->IlIII?(0x62f+5865-0x1d18):-(0x277+9199-0x2665));return lIllI->IlIII?
(0xd57+709-0x101c):-(0x2295+865-0x25f5);}


int llIIIIlII(struct IIIIIlI*lIllI,size_t length,llllIII*lllIllI,IIIlll IIIIlII)
{int i,lIlIlIll;size_t IIlIlIl;IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x5f\x69\x73\x6f\x3a\x20\x2b\x2b\x20\x69\x73\x6f\x70\x61\x63\x6b\x65\x74\x73\x3d\x25\x64\x20\x6c\x65\x6e\x67\x74\x68\x3d\x25\x6c\x75" "\n"
,IIIIlII,(unsigned long)length);lIlIlIll=(0x1f9a+1183-0x2439);lIllI->IlIII=
IIlIlIl=(0x93a+488-0xb22);
for(i=(0x58+1358-0x5a6);i<IIIIlII&&lIllI->IlIII<lIIllIl&&length>
(0x371+4747-0x15fc);i++){IIIlll lllllIIll,lIIIIIlll;int IlllllII=(i==IIIIlII-
(0x1802+2860-0x232d));

lllllIIll=lllIllI[i].Offset;lIIIIIlll=IlllllII?lllIllI[i].Offset+lllIllI[i].
Length:lllIllI[i+(0x9d5+3190-0x164a)].Offset;
if(lllllIIll>lIIIIIlll){
IIIIlll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x5f\x69\x73\x6f\x3a\x20\x42\x55\x47\x21\x21\x21\x20\x4e\x6f\x6e\x2d\x6c\x69\x6e\x65\x61\x72\x20\x69\x73\x6f\x20\x62\x75\x66\x66\x65\x72\x2e\x20\x50\x6c\x65\x61\x73\x65\x20\x72\x65\x70\x6f\x72\x74\x20\x74\x68\x69\x73\x2e" "\n"
);break;}
if((IIlIlIl+lllIllI[i].Length)>IllllllI||IlllllII){size_t lIIIIIl;if(IlllllII)
IIlIlIl+=lllIllI[i].Length;lIIIIIl=min(IIlIlIl,length);lIllI->IllIIIl[lIllI->
IlIII]=kmalloc(lIIIIIl,GFP_KERNEL);lIllI->llIlIl[lIllI->IlIII]=lIIIIIl;lIllI->
lIIIlII[lIllI->IlIII]=(0xc5c+2143-0x14bb);lIllI->lllIllI[lIllI->IlIII]=(i-
lIlIlIll);if(IlllllII)lIllI->lllIllI[lIllI->IlIII]++;IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x5f\x69\x73\x6f\x3a\x20\x61\x6c\x6c\x6f\x63\x61\x74\x69\x6e\x67\x20\x62\x75\x66\x66\x65\x72\x20\x25\x6c\x75\x20\x62\x79\x74\x65\x73\x2c\x20\x69\x73\x6f\x70\x61\x63\x6b\x65\x74\x73\x3d\x25\x64" "\n"
,(unsigned long)lIIIIIl,lIllI->lllIllI[lIllI->IlIII]);if(!lIllI->IllIIIl[lIllI->
IlIII]){IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x5f\x69\x73\x6f\x3a\x20\x6b\x6d\x61\x6c\x6c\x6f\x63\x20\x66\x61\x69\x6c\x65\x64" "\n"
);break;}lIllI->IlIII++;length-=lIIIIIl;IIlIlIl=(0xb8+8043-0x2023);lIlIlIll=i;}
IIlIlIl+=lllIllI[i].Length;}
if(i<IIIIlII){for(i=(0x1bd3+1626-0x222d);i<lIllI->IlIII;i++){kfree(lIllI->
IllIIIl[i]);lIllI->IllIIIl[i]=NULL;}lIllI->IlIII=(0x210+2122-0xa5a);}IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x61\x6c\x6c\x6f\x63\x5f\x69\x73\x6f\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIllI->IlIII?(0xcd6+6031-0x2465):-(0x9e4+7327-0x2682));return lIllI->IlIII?
(0x86c+6862-0x233a):-(0x94a+7461-0x266e);}int IIIIllII(struct IIIIIlI*lIllI,
const void*IlIIl,size_t length){int i;size_t lIIlll,lIlll=(0x6c4+5812-0x1d78);
IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x63\x6f\x70\x79\x5f\x66\x72\x6f\x6d\x5f\x75\x73\x65\x72\x3a\x20\x63\x6f\x75\x6e\x74\x3d\x25\x64" "\n"
,lIllI->IlIII);for(i=(0xea4+3889-0x1dd5);i<lIllI->IlIII&&length>
(0x10d0+2488-0x1a88);i++){lIIlll=min(length,lIllI->llIlIl[i]);if(
__copy_from_user(lIllI->IllIIIl[i],IlIIl,lIIlll)!=(0x21af+791-0x24c6)){IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x63\x6f\x70\x79\x5f\x66\x72\x6f\x6d\x5f\x75\x73\x65\x72\x3a\x20\x63\x61\x6e\x6e\x6f\x74\x20\x63\x6f\x70\x79\x20\x66\x72\x6f\x6d\x20\x75\x73\x65\x72\x20\x62\x75\x66\x66\x65\x72" "\n"
);break;}lIllI->lIIIlII[i]=lIIlll;IlIIl+=lIIlll;length-=lIIlll;lIlll+=lIIlll;}
return lIlll;}int IIllllll(struct IIIIIlI*lIllI,void*IlIIl,size_t length){int i;
size_t lIIlll,lIlll=(0x485+1594-0xabf);IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x63\x6f\x70\x79\x5f\x74\x6f\x5f\x75\x73\x65\x72\x3a\x20\x63\x6f\x75\x6e\x74\x3d\x25\x64" "\n"
,lIllI->IlIII);for(i=(0x1ad+3161-0xe06);i<lIllI->IlIII&&length>
(0xb0d+6148-0x2311);i++){lIIlll=min(length,lIllI->lIIIlII[i]);if(__copy_to_user(
IlIIl,lIllI->IllIIIl[i],lIIlll)!=(0xef5+5864-0x25dd)){IIIll(
"\x75\x73\x62\x64\x5f\x62\x63\x5f\x63\x6f\x70\x79\x5f\x74\x6f\x5f\x75\x73\x65\x72\x3a\x20\x63\x61\x6e\x6e\x6f\x74\x20\x63\x6f\x70\x79\x20\x66\x72\x6f\x6d\x20\x75\x73\x65\x72\x20\x62\x75\x66\x66\x65\x72" "\n"
);break;}IlIIl+=lIIlll;length-=lIIlll;lIlll+=lIIlll;}return lIlll;}int lIlIIIIl(
struct IIIIIlI*lIllI){int i;for(i=(0xde6+1360-0x1336);i<lIllI->IlIII;i++){if(
lIllI->IllIIIl[i]){kfree(lIllI->IllIIIl[i]);lIllI->IllIIIl[i]=NULL;}}return
(0x56f+166-0x615);}
#endif 

