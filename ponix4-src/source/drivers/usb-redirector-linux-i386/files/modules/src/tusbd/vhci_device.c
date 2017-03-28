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
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <asm/page.h>
#include <asm/unaligned.h>
struct usb_hcd*IIlIIlI=NULL;
#if KERNEL_GT_EQ((0x12a+1832-0x850),(0x21bd+1167-0x2646),(0x89b+7074-0x2416)) ||\
 RHEL_RELEASE_GT_EQ((0x115c+694-0x140c),(0xb46+636-0xdbf)) 
struct usb_hcd*llIIllI=NULL;
#define llIIIlI(IlIIlI) (((IlIIlI)->speed == HCD_USB3) ? "\x75\x73\x62\x33" : \
"\x75\x73\x62\x32")
#else
#define llIIIlI(IlIIlI) "\x55\x53\x42\x32"
#endif
#if defined(IIlIlIll) && !defined(_USBD_VHCI_NO_DMA_)
int lIIIlIl=(0x10d9+4107-0x20e4);
#endif


static int lIlllIIll(struct platform_device*);static int lIlllllI(struct 
platform_device*);static void lllIlIll(struct device*dev);static int IlIIIllll(
struct usb_hcd*IllIl);static int IlIlIIIl(struct usb_hcd*IllIl);static void 
lIlllIlII(struct usb_hcd*IllIl);static int IlIllIIll(struct usb_hcd*IllIl);
#if KERNEL_LT((0xe7d+228-0xf5f),(0x742+8064-0x26bc),(0x1ec1+744-0x2191))
static int lllllIlI(struct usb_hcd*IllIl,struct urb*urb);static int lIlIllIl(
struct usb_hcd*IllIl,struct usb_host_endpoint*ep,struct urb*urb,gfp_t lIIllI);
#else
static int lllllIlI(struct usb_hcd*IllIl,struct urb*urb,int status);static int 
lIlIllIl(struct usb_hcd*IllIl,struct urb*urb,gfp_t lIIllI);
#endif
static void lIIllIlI(struct usb_hcd*IllIl,struct usb_host_endpoint*ep);static 
int lIllIllI(struct usb_hcd*IllIl,u16 llIlIllI,u16 wValue,u16 wIndex,char*IlIIl,
u16 wLength);static int lIlIllIII(struct usb_hcd*IllIl,char*IlIIl);static int 
lIIlllIl(struct usb_hcd*IllIl);static int llIIIlll(struct usb_hcd*IllIl);
#if KERNEL_GT_EQ((0x50b+6234-0x1d63),(0x515+2081-0xd30),(0x1a14+1160-0x1e75)) ||\
 RHEL_RELEASE_GT_EQ((0x11bb+1987-0x1978),(0x76f+5511-0x1cf3)) 
static int lllIIlIl(struct usb_hcd*IllIl,struct usb_device*lIlIIl,struct 
usb_host_endpoint**lIIIlIll,unsigned int IIlIIIlI,unsigned int IlIIlIIll,gfp_t 
lIIllI);static int IIIIIllII(struct usb_hcd*IllIl,struct usb_device*lIlIIl,
struct usb_host_endpoint**lIIIlIll,unsigned int IIlIIIlI,gfp_t lIIllI);
#endif
static int IIIIIIII(struct IIlllI*Illll,int IlllI,int IlIIIIl,int IIlIIll);
static int IllIlIIl(struct IIlllI*Illll,int IlllI,int IlIIIIl);static void 
lIlIlI(struct IIlllI*Illll,int IlllI,int lIIIlll);static int IIIlllI(struct 
IIlllI*Illll,int IlllI,int IllllI,int IIlIIll);
#if KERNEL_GT_EQ((0xe2b+3918-0x1d77),(0x2256+484-0x2434),(0x1b6f+1740-0x2214)) \
|| RHEL_RELEASE_GT_EQ((0x914+5611-0x1ef9),(0x107b+5753-0x26f1)) 
static int IIlIIIll(struct IIlllI*Illll,int IlllI,int IlIIIIl,int IIlIIll);
static int llIllIlI(struct IIlllI*Illll,int IlllI,int IlIIIIl);static int 
IlIIlII(struct IIlllI*Illll,int IlllI,int IllllI,int IIlIIll);static void 
lllllIl(struct IIlllI*Illll,int IlllI,int lIIIlll);static void IlIIlIl(struct 
IIlllI*Illll,int IlllI,int lIllIIll);
#endif
#define IIllIl(IllIl) ((struct IIlllI*)(IllIl->hcd_priv))
static struct platform_driver lllllIlll={.probe=lIlllIIll,.remove=lIlllllI,.
driver={.name=IllIIll,.owner=THIS_MODULE,},};static struct platform_device 
lIlIllll={.name=IllIIll,.id=-(0x19f9+1492-0x1fcc),.dev={
.release=lllIlIll,},};static struct hc_driver IIIlllll={.description=IllIIll,.
product_desc=
"\x56\x69\x72\x74\x75\x61\x6c\x20\x55\x53\x42\x20\x48\x6f\x73\x74\x20\x43\x6f\x6e\x74\x72\x6f\x6c\x6c\x65\x72"
,.hcd_priv_size=sizeof(struct IIlllI),
#if KERNEL_GT_EQ((0xd00+1155-0x1181),(0x11d2+2953-0x1d55),(0x1135+3496-0x1eb6)) \
|| RHEL_RELEASE_GT_EQ((0x1880+94-0x18d8),(0x23f2+3-0x23f2)) 
.flags=HCD_USB3|HCD_SHARED,
#else
.flags=HCD_USB2,
#endif
.reset=IlIIIllll,.start=IlIlIIIl,.stop=lIlllIlII,.urb_enqueue=lIlIllIl,.
urb_dequeue=lllllIlI,.endpoint_disable=lIIllIlI,.get_frame_number=IlIllIIll,.
hub_status_data=lIlIllIII,.hub_control=lIllIllI,.bus_suspend=lIIlllIl,.
bus_resume=llIIIlll,
#if KERNEL_GT_EQ((0x515+3674-0x136d),(0x837+1405-0xdae),(0x1a05+2839-0x24f5)) ||\
 RHEL_RELEASE_GT_EQ((0x926+3924-0x1874),(0xaef+4874-0x1df6)) 
.alloc_streams=lllIIlIl,.free_streams=IIIIIllII,
#endif
};static struct list_head IlllllI;static spinlock_t lIllII;
#if defined(IIlIlIll) && !defined(_USBD_VHCI_NO_DMA_)
#  if KERNEL_LT((0x7a6+7866-0x265e),(0x1ff7+1460-0x25a5),(0x54b+8622-0x26e1))
static u64 IlllIllIl=IIIIllIIl;
#  else
static u64 IlllIllIl=IIlIllllI((0x66b+2138-0xea5));
#  endif
#endif
static int lIlllIIll(struct platform_device*llllI){struct IIlllI*Illll;int lIlll
=(0x273+1327-0x7a2);int i;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x70\x6c\x61\x74\x66\x6f\x72\x6d\x5f\x64\x72\x69\x76\x65\x72\x5f\x70\x72\x6f\x62\x65\x3a\x20\x2b\x2b" "\n"
);do{if(llllI->dev.dma_mask){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x70\x6c\x61\x74\x66\x6f\x72\x6d\x5f\x64\x72\x69\x76\x65\x72\x5f\x70\x72\x6f\x62\x65\x3a\x20\x64\x6d\x61\x20\x6e\x6f\x74\x20\x73\x75\x70\x70\x6f\x72\x74\x65\x64\x21" "\n"
);return-EINVAL;}
#if defined(IIlIlIll) && !defined(_USBD_VHCI_NO_DMA_)
if(lIIIlIl)llllI->dev.dma_mask=&IlllIllIl;
#endif

IIlIIlI=usb_create_hcd(&IIIlllll,&llllI->dev,IIlIIIl(&llllI->dev));if(IIlIIlI==
NULL){lIlll=-ENOMEM;break;}
#if KERNEL_GT_EQ((0x2080+82-0x20d0),(0x32c+3464-0x10ae),(0x3d2+6379-0x1ca3))
IIlIIlI->has_tt=(0x273+337-0x3c3);
#endif
Illll=IIllIl(IIlIIlI);memset(Illll,(0x1248+3694-0x20b6),sizeof(*Illll));Illll->
IllIl=IIlIIlI;spin_lock_init(&Illll->lock);for(i=(0xb0b+2211-0x13ae);i<IlIlII;i
++){IIIlllI(Illll,i,IIllllII,(0x57f+8500-0x26b3));}lIlll=usb_add_hcd(IIlIIlI,
(0x6c0+165-0x765),(0x1880+2467-0x2223));if(lIlll!=(0x1fd5+680-0x227d)){break;}
#if KERNEL_GT_EQ((0xe0+2280-0x9c6),(0x12bf+4606-0x24b7),(0xda7+5178-0x21ba)) || \
RHEL_RELEASE_GT_EQ((0x1d50+1501-0x2327),(0x558+3849-0x145e)) 

llIIllI=usb_create_shared_hcd(&IIIlllll,&llllI->dev,IIlIIIl(&llllI->dev),IIlIIlI
);if(llIIllI==NULL){lIlll=-ENOMEM;break;}Illll=IIllIl(llIIllI);memset(Illll,
(0x48b+3682-0x12ed),sizeof(*Illll));Illll->IllIl=llIIllI;spin_lock_init(&Illll->
lock);for(i=(0x50f+866-0x871);i<IlIlII;i++){IlIIlII(Illll,i,IIllllII,
(0x14e4+1794-0x1be6));}lIlll=usb_add_hcd(llIIllI,(0x7b9+5900-0x1ec5),
(0xe6f+5546-0x2419));if(lIlll!=(0x522+1168-0x9b2)){break;}IIllIl(IIlIIlI)->
lllllII=llIIllI;IIllIl(llIIllI)->lllllII=IIlIIlI;
#endif 
}while((0xa08+1908-0x117c));if(lIlll!=(0x1425+1191-0x18cc)){if(IIlIIlI){
usb_put_hcd(IIlIIlI);IIlIIlI=NULL;}
#if KERNEL_GT_EQ((0x2bd+5283-0x175e),(0xee9+4416-0x2023),(0xb21+6644-0x24ee)) ||\
 RHEL_RELEASE_GT_EQ((0x1225+601-0x1478),(0x11cf+678-0x1472)) 
if(llIIllI){usb_put_hcd(llIIllI);llIIllI=NULL;}
#endif
}IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x70\x6c\x61\x74\x66\x6f\x72\x6d\x5f\x64\x72\x69\x76\x65\x72\x5f\x70\x72\x6f\x62\x65\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x20\x3d\x20\x25\x64" "\n"
,lIlll);return lIlll;}static int lIlllllI(struct platform_device*llllI){struct 
usb_hcd*IllIl=platform_get_drvdata(llllI);struct IIlllI*Illll=IIllIl(IllIl);
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x70\x6c\x61\x74\x66\x6f\x72\x6d\x5f\x64\x72\x69\x76\x65\x72\x5f\x72\x65\x6d\x6f\x76\x65\x5b\x25\x73\x5d\x3a\x20\x2b\x2b" "\n"
,llIIIlI(IllIl));if(Illll->lllllII){usb_remove_hcd(Illll->lllllII);usb_put_hcd(
Illll->lllllII);Illll->lllllII=NULL;}usb_remove_hcd(IllIl);usb_put_hcd(IllIl);
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x70\x6c\x61\x74\x66\x6f\x72\x6d\x5f\x64\x72\x69\x76\x65\x72\x5f\x72\x65\x6d\x6f\x76\x65\x3a\x20\x2d\x2d" "\n"
);return(0x22ca+1059-0x26ed);}static void lllIlIll(struct device*dev){return;}
static int IlIIIllll(struct usb_hcd*IllIl){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x72\x65\x73\x65\x74\x5b\x25\x73\x5d" "\n"
,llIIIlI(IllIl));
#if KERNEL_GT_EQ((0x1b32+421-0x1cd5),(0x159f+410-0x1733),(0x1c6a+912-0x1fd3)) ||\
 RHEL_RELEASE_GT_EQ((0x1e0d+1192-0x22af),(0x1097+4799-0x2353)) 
if(usb_hcd_is_primary_hcd(IllIl)){
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x72\x65\x73\x65\x74\x3a\x20\x73\x65\x74\x74\x69\x6e\x67\x20\x75\x70\x20\x55\x53\x42\x32\x20\x68\x63\x64" "\n"
);IllIl->speed=HCD_USB2;IllIl->self.root_hub->speed=USB_SPEED_HIGH;}
#endif
return(0x87c+5827-0x1f3f);}static int IlIlIIIl(struct usb_hcd*IllIl){int i;IIIll
(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x73\x74\x61\x72\x74\x5b\x25\x73\x5d" "\n"
,llIIIlI(IllIl));IllIl->power_budget=(0x1e7d+239-0x1f6c);IllIl->state=
HC_STATE_RUNNING;IllIl->uses_new_polling=(0x17eb+3758-0x2698);for(i=
(0x885+4339-0x1978);i<IlIlII;i++){
#if KERNEL_GT_EQ((0x1560+3305-0x2247),(0x965+4326-0x1a45),(0x1468+528-0x1651)) \
|| RHEL_RELEASE_GT_EQ((0x1549+4256-0x25e3),(0x378+2805-0xe6a)) 
if(IllIl->speed==HCD_USB3){IlIIlII(IIllIl(IllIl),i,IIIlIIIl,(0xceb+5149-0x2108))
;}else
#endif
{IIIlllI(IIllIl(IllIl),i,IIIlIIIl,(0x694+4277-0x1749));}}return
(0x9e7+1162-0xe71);}static void lIlllIlII(struct usb_hcd*IllIl){int i;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x73\x74\x6f\x70\x5b\x25\x73\x5d" "\n",
llIIIlI(IllIl));for(i=(0x2fb+7060-0x1e8f);i<IlIlII;i++){
#if KERNEL_GT_EQ((0x1bab+32-0x1bc9),(0x393+602-0x5e7),(0x30+8615-0x21b0)) || \
RHEL_RELEASE_GT_EQ((0x766+2330-0x107a),(0x19d7+218-0x1aae)) 
if(IllIl->speed==HCD_USB3){IlIIlII(IIllIl(IllIl),i,llllIllI,(0xac9+263-0xbd0));}
else
#endif
{IIIlllI(IIllIl(IllIl),i,llllIllI,(0x282+2258-0xb54));}}}static int IlIllIIll(
struct usb_hcd*IllIl){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x67\x65\x74\x5f\x66\x72\x61\x6d\x65\x5f\x6e\x75\x6d\x62\x65\x72\x5b\x25\x73\x5d" "\n"
,llIIIlI(IllIl));return(0xd3d+5938-0x246f);}int IlllIlII(struct IIIII*llllI,
struct lIlIII**llllIIl,struct urb*lIlII){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x61\x64\x64\x5f\x75\x72\x62\x3a\x20\x75\x72\x62\x3d\x25\x70" "\n"
,lIlII);if(*llllIIl){struct lIllIll*IllIII;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x61\x64\x64\x5f\x75\x72\x62\x3a\x20\x65\x78\x69\x73\x74\x69\x6e\x67\x20\x70\x71\x64\x61\x74\x61" "\n"
);IllIII=kmalloc(sizeof(struct lIllIll),GFP_ATOMIC);if(IllIII){IllIII->lIlII=
lIlII;list_add_tail(&IllIII->llIIll,&(*llllIIl)->llIIlIl);}else{IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x61\x64\x64\x5f\x75\x72\x62\x3a\x20\x6e\x6f\x20\x6d\x65\x6d\x6f\x72\x79\x20\x66\x6f\x72\x20\x75\x72\x62\x5f\x6c\x69\x73\x74" "\n"
);return-ENOMEM;}}else{IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x61\x64\x64\x5f\x75\x72\x62\x3a\x20\x6e\x65\x77\x20\x70\x71\x64\x61\x74\x61" "\n"
);*llllIIl=kmalloc(sizeof(struct lIlIII),GFP_ATOMIC);if(*llllIIl==NULL){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x61\x64\x64\x5f\x75\x72\x62\x3a\x20\x6e\x6f\x20\x6d\x65\x6d\x6f\x72\x79\x20\x66\x6f\x72\x20\x71\x75\x65\x75\x65\x5f\x65\x6e\x74\x72\x79" "\n"
);return-ENOMEM;}else{INIT_LIST_HEAD(&(*llllIIl)->lIIIl);(*llllIIl)->llllI=llllI
;(*llllIIl)->IIlIl=NULL;(*llllIIl)->urb.lIlII=lIlII;(*llllIIl)->IlIIII=IlIlllIl(
);INIT_LIST_HEAD(&(*llllIIl)->llIIlIl);list_add_tail(&(*llllIIl)->urb.llIIll,&(*
llllIIl)->llIIlIl);}}return(0x13e2+3186-0x2054);}
#if KERNEL_LT((0x1bf3+505-0x1dea),(0x1c51+1175-0x20e2),(0x1e4+1472-0x78c))
static int lIlIllIl(struct usb_hcd*IllIl,struct usb_host_endpoint*ep,struct urb*
urb,gfp_t lIIllI)
#else
static int lIlIllIl(struct usb_hcd*IllIl,struct urb*urb,gfp_t lIIllI)
#endif
{int lIlll=-EINPROGRESS;struct IIlllI*Illll=IIllIl(IllIl);struct IIIII*llllI=
NULL;unsigned long flags;int status=-(0x1651+1255-0x1b37);int llllllII=
(0x344+8026-0x229e);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x65\x6e\x71\x75\x65\x75\x65\x5b\x25\x73\x5d\x3a\x20\x2b\x2b\x20\x75\x72\x62\x3d\x30\x78\x25\x70\x20\x75\x73\x62\x5f\x64\x65\x76\x69\x63\x65\x3d\x30\x78\x25\x70\x20\x70\x61\x72\x65\x6e\x74\x3d\x30\x78\x25\x70\x20\x64\x65\x76\x6e\x75\x6d\x3d\x25\x64\x20\x6e\x75\x6d\x5f\x73\x67\x73\x3d\x25\x64" "\n"
,llIIIlI(IllIl),urb,urb->dev,urb->dev->dev.parent,urb->dev->devnum,
#if KERNEL_GT_EQ((0xbc3+992-0xfa1),(0x27d+7484-0x1fb3),(0x2b1+4469-0x1407))
urb->num_sgs);
#else
(0x1995+1239-0x1e6c));
#endif
IIlIIlll(urb,(0xca6+3416-0x19fe));do
{struct lIlIII*lIIll;
#if KERNEL_GT_EQ((0xdd7+1277-0x12d2),(0x69f+356-0x7fd),(0xf0c+3831-0x1deb))
struct usb_host_endpoint*ep=urb->ep;
#endif
#if KERNEL_GT_EQ((0x1bb+8392-0x2281),(0x12dc+4923-0x2611),(0xb70+711-0xe10)) || \
RHEL_RELEASE_GT_EQ((0x6ca+5602-0x1ca6),(0xf2f+3258-0x1be6)) 








if(IllIl->speed==HCD_USB3){
llllI=IlIlllIII(IllIl,urb->dev->portnum);}else
#endif
{
llllI=lIIIlllll(IllIl,usb_pipedevice(urb->pipe));}if(llllI==NULL){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x65\x6e\x71\x75\x65\x75\x65\x3a\x20\x64\x65\x76\x69\x63\x65\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
);lIlll=-ENODEV;break;}urb->hcpriv=llllI;if(usb_pipedevice(urb->pipe)==
(0xd3d+1350-0x1283)&&usb_pipetype(urb->pipe)==PIPE_CONTROL){struct 
usb_ctrlrequest*Illlll=(struct usb_ctrlrequest*)urb->setup_packet;if(!Illlll){
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x65\x6e\x71\x75\x65\x75\x65\x3a\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x63\x6f\x6e\x74\x72\x6f\x6c\x20\x74\x72\x61\x6e\x73\x66\x65\x72\x20\x75\x72\x62" "\n"
);lIlll=-EINVAL;break;}if(Illlll->bRequest==USB_REQ_SET_ADDRESS){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x65\x6e\x71\x75\x65\x75\x65\x3a\x20\x55\x53\x42\x5f\x52\x45\x51\x5f\x53\x45\x54\x5f\x41\x44\x44\x52\x45\x53\x53\x20\x61\x64\x64\x72\x65\x73\x73\x3d\x25\x64" "\n"
,Illlll->wValue);spin_lock_irqsave(&Illll->lock,flags);llllI->llIlIIl=
le16_to_cpu(Illlll->wValue);spin_unlock_irqrestore(&Illll->lock,flags);status=
(0xc99+5690-0x22d3);lIlll=(0xe6+2857-0xc0f);break;}}spin_lock_irqsave(&llllI->
IIllII,flags);spin_lock(&Illll->lock);
#if KERNEL_LT((0x53a+5558-0x1aee),(0xbc0+3410-0x190c),(0x6bc+1746-0xd76))
spin_lock(&urb->lock);status=urb->status;spin_unlock(&urb->lock);if(status!=-
EINPROGRESS){lIlll=(0x1084+5746-0x26f6);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x65\x6e\x71\x75\x65\x75\x65\x3a\x20\x75\x72\x62\x20\x61\x6c\x72\x65\x61\x64\x79\x20\x75\x6e\x6c\x69\x6e\x6b\x65\x64" "\n"
);spin_unlock(&Illll->lock);spin_unlock_irqrestore(&llllI->IIllII,flags);break;}
#else


lIlll=usb_hcd_link_urb_to_ep(IllIl,urb);if(lIlll!=(0x642+5939-0x1d75)){
spin_unlock(&Illll->lock);spin_unlock_irqrestore(&llllI->IIllII,flags);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x65\x6e\x71\x75\x65\x75\x65\x3a\x20\x75\x72\x62\x20\x61\x6c\x72\x65\x61\x64\x79\x20\x75\x6e\x6c\x69\x6e\x6b\x65\x64" "\n"
);break;}
#endif
lIIll=ep->hcpriv;lIlll=IlllIlII(llllI,&lIIll,urb);if(lIlll!=(0x10dc+1434-0x1676)
){spin_unlock(&Illll->lock);spin_unlock_irqrestore(&llllI->IIllII,flags);break;}
if((usb_pipetype(urb->pipe)==PIPE_BULK)&&(urb->transfer_flags&URB_NO_INTERRUPT)
&&((usb_pipein(urb->pipe)&&(urb->transfer_flags&URB_SHORT_NOT_OK))||(usb_pipeout
(urb->pipe)&&!(urb->transfer_flags&URB_ZERO_PACKET)))){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x65\x6e\x71\x75\x65\x75\x65\x3a\x20\x6d\x65\x72\x67\x65\x20\x73\x70\x6c\x69\x74\x74\x65\x64\x20\x75\x72\x62" "\n"
);if(!ep->hcpriv){ep->hcpriv=lIIll;list_add_tail(&lIIll->lIIIl,&llllI->lllIll);}
}else{if(ep->hcpriv){ep->hcpriv=NULL;list_del(&lIIll->lIIIl);}list_add_tail(&
lIIll->lIIIl,&llllI->llIIIIl);llllllII=(0x256+5173-0x168a);}spin_unlock(&Illll->
lock);spin_unlock_irqrestore(&llllI->IIllII,flags);if(llllllII){wake_up(&llllI->
lllIII);}lIlll=-EINPROGRESS;}while((0x3d6+2355-0xd09));switch(lIlll){case-
EINPROGRESS:lIlll=(0xc81+4326-0x1d67);break;case(0x1c67+559-0x1e96):IllIIIIl(urb
,status);break;default:if(llllI)IIIIllI(llllI);break;}IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x65\x6e\x71\x75\x65\x75\x65\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}
#if KERNEL_LT((0x1045+3728-0x1ed3),(0xed7+2170-0x174b),(0x3eb+567-0x60a))
static int lllllIlI(struct usb_hcd*IllIl,struct urb*urb)
#else
static int lllllIlI(struct usb_hcd*IllIl,struct urb*urb,int status)
#endif
{struct IIIII*llllI=NULL;struct lIlIII*lIIll=NULL;llIII IIlIl;int lIlll=
(0x12a3+787-0x15b6);unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x64\x65\x71\x75\x65\x75\x65\x5b\x25\x73\x5d\x3a\x20\x2b\x2b\x20\x75\x72\x62\x3d\x30\x78\x25\x70" "\n"
,llIIIlI(IllIl),urb);do{llllI=(struct IIIII*)urb->hcpriv;if(!llllI){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x64\x65\x71\x75\x65\x75\x65\x3a\x20\x64\x65\x76\x69\x63\x65\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64\x21" "\n"
);break;}
#if KERNEL_GT_EQ((0xc80+2063-0x148d),(0xba9+4472-0x1d1b),(0x788+1363-0xcc3))
lIlll=usb_hcd_check_unlink_urb(IllIl,urb,status);if(lIlll!=(0x86b+1772-0xf57))
break;
#endif
lIIll=llIIllIll(llllI,urb);if(!lIIll){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x64\x65\x71\x75\x65\x75\x65\x3a\x20\x75\x72\x62\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64\x20\x69\x6e\x20\x71\x75\x65\x75\x65" "\n"
);break;}
#if KERNEL_LT((0x6a6+1303-0xbbb),(0x15ad+539-0x17c2),(0x119+7386-0x1ddb))
llIllll(llllI,lIIll,-(0x10dd+721-0x13ad));
#else
llIllll(llllI,lIIll,status);
#endif
IIlIl=kmalloc(sizeof(lIIIIlIl),GFP_ATOMIC);if(!IIlIl){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x64\x65\x71\x75\x65\x75\x65\x3a\x20\x6e\x6f\x20\x6d\x65\x6d\x6f\x72\x79\x20\x66\x6f\x72\x20\x75\x6e\x72\x62" "\n"
);break;}IIlIl->IIIlI.IIIIll=lIIll->IlIIII;IIlIl->IIIlI.lIlIl=sizeof(lIIIIlIl);
IIlIl->IIIlI.lIlllI=IIIlIIII;IIlIl->IIIlI.Status=(0x12c8+1370-0x1822);IIlIl->
IIIlI.Context=(0x52c+3078-0x1132);INIT_LIST_HEAD(&lIIll->llIIlIl);lIIll->urb.
lIlII=NULL;lIIll->IIlIl=IIlIl;spin_lock_irqsave(&llllI->IIllII,flags);
list_add_tail(&lIIll->lIIIl,&llllI->llIIIIl);spin_unlock_irqrestore(&llllI->
IIllII,flags);wake_up(&llllI->lllIII);}while((0xad6+614-0xd3c));IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x72\x62\x5f\x64\x65\x71\x75\x65\x75\x65\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);return lIlll;}void lIIllIlI(struct usb_hcd*IllIl,struct 
usb_host_endpoint*ep){struct IIlllI*Illll=IIllIl(IllIl);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x5b\x25\x73\x5d\x3a\x20\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x65\x6e\x64\x70\x6f\x69\x6e\x74\x5f\x64\x69\x73\x61\x62\x6c\x65\x2b\x2b\x20\x65\x70\x3d\x25\x70" "\n"
,llIIIlI(IllIl),ep);spin_lock(&Illll->lock);if(ep->hcpriv){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x63\x6c\x65\x61\x6e\x69\x6e\x67\x20\x75\x70\x20\x68\x63\x70\x72\x69\x76" "\n"
);list_del_init(&((struct lIlIII*)ep->hcpriv)->lIIIl);ep->hcpriv=NULL;}
spin_unlock(&Illll->lock);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x65\x6e\x64\x70\x6f\x69\x6e\x74\x5f\x64\x69\x73\x61\x62\x6c\x65\x2d\x2d" "\n"
);}
#if KERNEL_GT_EQ((0xc9b+480-0xe79),(0x1cf1+1784-0x23e3),(0x215+7782-0x2054)) || \
RHEL_RELEASE_GT_EQ((0x7b4+6481-0x20ff),(0x8d9+3240-0x157e)) 
#define lIllIlIl \
		( USB_PORT_STAT_C_CONNECTION \
		| USB_PORT_STAT_C_ENABLE \
		| USB_PORT_STAT_C_SUSPEND \
		| USB_PORT_STAT_C_OVERCURRENT \
		| USB_PORT_STAT_C_RESET \
		| USB_PORT_STAT_C_BH_RESET \
		| USB_PORT_STAT_C_LINK_STATE \
		| USB_PORT_STAT_C_CONFIG_ERROR )
#else
#define lIllIlIl \
		( USB_PORT_STAT_C_CONNECTION \
		| USB_PORT_STAT_C_ENABLE \
		| USB_PORT_STAT_C_SUSPEND \
		| USB_PORT_STAT_C_OVERCURRENT \
		| USB_PORT_STAT_C_RESET )
#endif
#if KERNEL_GT_EQ((0x3c0+3737-0x1257),(0x63b+7155-0x2228),(0x31a+6395-0x1bee)) ||\
 RHEL_RELEASE_GT_EQ((0x146+6312-0x19e8),(0x17cb+483-0x19ab)) 
size_t llllIlllI(void*IlIIl,size_t IIIIII){struct{struct usb_bos_descriptor 
lIIllIII;struct usb_ss_cap_descriptor ss_cap;}__packed IIlllIl;memset(&IIlllIl,
(0x5e1+5119-0x19e0),sizeof(IIlllIl));IIlllIl.lIIllIII.bLength=USB_DT_BOS_SIZE,
IIlllIl.lIIllIII.bDescriptorType=USB_DT_BOS,IIlllIl.lIIllIII.wTotalLength=
cpu_to_le16(sizeof(IIlllIl)),IIlllIl.lIIllIII.bNumDeviceCaps=(0x48a+1845-0xbbe),
IIlllIl.ss_cap.bLength=USB_DT_USB_SS_CAP_SIZE,IIlllIl.ss_cap.bDescriptorType=
USB_DT_DEVICE_CAPABILITY,IIlllIl.ss_cap.bDevCapabilityType=USB_SS_CAP_TYPE,
IIlllIl.ss_cap.wSpeedSupported=cpu_to_le16(USB_5GBPS_OPERATION),IIlllIl.ss_cap.
bFunctionalitySupport=ilog2(USB_5GBPS_OPERATION),IIIIII=min(sizeof(IIlllIl),
IIIIII);memcpy(IlIIl,&IIlllIl,IIIIII);return IIIIII;}size_t lIIIIlIll(void*IlIIl
,size_t IIIIII){struct usb_hub_descriptor desc;memset(&desc,(0xb9+8734-0x22d7),
sizeof(desc));desc.bDescLength=(0x10df+4064-0x20b3);desc.bDescriptorType=
(0x549+7503-0x226e);desc.bNbrPorts=IlIlII;
desc.wHubCharacteristics=cpu_to_le16((0x12b9+2404-0x1c1c));desc.u.ss.
bHubHdrDecLat=(0x9cc+4187-0x1a23);
desc.u.ss.DeviceRemovable=cpu_to_le16(65534&(65535>>((0x66c+4657-0x188e)-IlIlII)
));IIIIII=min((size_t)desc.bDescLength,IIIIII);memcpy(IlIIl,&desc,IIIIII);return
 IIIIII;}
#endif 
size_t llllllllI(void*IlIIl,size_t IIIIII){__u8*IlIIIlII;struct 
usb_hub_descriptor desc;memset(&desc,(0xdd3+3854-0x1ce1),sizeof(desc));desc.
bDescLength=(0xac0+3366-0x17df)+lllIIlll*(0x18f8+2215-0x219d);desc.
bDescriptorType=(0x8e3+1746-0xf8c);desc.bNbrPorts=IlIlII;desc.
wHubCharacteristics=cpu_to_le16((0x12b7+3028-0x1e8a));
#if KERNEL_GT_EQ((0x6e5+1653-0xd58),(0x203+8027-0x2158),(0x26f+9209-0x2641)) || \
RHEL_RELEASE_GT_EQ((0xa76+5117-0x1e6d),(0x1e9c+2144-0x26fa))
IlIIIlII=desc.u.hs.DeviceRemovable;
#else
IlIIIlII=desc.DeviceRemovable;
#endif
memset(&IlIIIlII[(0x778+2539-0x1163)],(0x970+4703-0x1bcf),lllIIlll);memset(&
IlIIIlII[lllIIlll],(0xb72+5412-0x1f97),lllIIlll);IIIIII=min((size_t)desc.
bDescLength,IIIIII);memcpy(IlIIl,&desc,IIIIII);return IIIIII;}static int 
lIllIllI(struct usb_hcd*IllIl,u16 llIlIllI,u16 wValue,u16 wIndex,char*IlIIl,u16 
wLength){struct IIlllI*Illll=IIllIl(IllIl);int lIlll=(0x3c+193-0xfd);int IlllI=-
(0x209b+1038-0x24a8);unsigned long flags;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x5b\x25\x73\x5d\x3a\x20\x2b\x2b" "\n"
,llIIIlI(IllIl));if(!test_bit(HCD_FLAG_HW_ACCESSIBLE,&IllIl->flags))return-
ETIMEDOUT;spin_lock_irqsave(&Illll->lock,flags);switch(llIlIllI){case 
GetHubDescriptor:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x47\x65\x74\x48\x75\x62\x44\x65\x73\x63\x72\x69\x70\x74\x6f\x72" "\n"
);
#if KERNEL_GT_EQ((0x1595+998-0x1979),(0x222b+36-0x2249),(0x60c+1649-0xc56)) || \
RHEL_RELEASE_GT_EQ((0xc07+6193-0x2432),(0x335+1587-0x965)) 
if(IllIl->speed==HCD_USB3){if((wValue>>(0xfba+5760-0x2632))!=USB_DT_SS_HUB){
lIlll=-EPIPE;break;}

lIIIIlIll(IlIIl,wLength);}else
#endif
{if((wValue>>(0x6a3+4834-0x197d))!=USB_DT_HUB){lIlll=-EPIPE;break;}llllllllI(
IlIIl,wLength);}break;
#if KERNEL_GT_EQ((0x9c6+6116-0x21a8),(0x1c66+160-0x1d00),(0x937+3133-0x154d)) ||\
 RHEL_RELEASE_GT_EQ((0x451+4677-0x1690),(0xdca+1340-0x1303)) 
case DeviceRequest|USB_REQ_GET_DESCRIPTOR:if(IllIl->speed!=HCD_USB3){lIlll=-
EPIPE;break;}if((wValue>>(0xc4+4934-0x1402))!=USB_DT_BOS){lIlll=-EPIPE;break;}
lIlll=llllIlllI(IlIIl,wLength);break;case GetPortErrorCount:
if(IllIl->speed!=HCD_USB3){lIlll=-EPIPE;break;}
*(__le16*)IlIIl=cpu_to_le16((0xaa1+2370-0x13e3));break;case SetHubDepth:
if(IllIl->speed!=HCD_USB3){lIlll=-EPIPE;break;}
break;
#endif
case GetHubStatus:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x47\x65\x74\x48\x75\x62\x53\x74\x61\x74\x75\x73" "\n"
);*(__le32*)IlIIl=cpu_to_le32((0xe04+6118-0x25ea));break;case SetHubFeature:
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x48\x75\x62\x46\x65\x61\x74\x75\x72\x65" "\n"
);
lIlll=-EPIPE;break;case ClearHubFeature:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x48\x75\x62\x46\x65\x61\x74\x75\x72\x65" "\n"
);
break;case GetPortStatus:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x47\x65\x74\x50\x6f\x72\x74\x53\x74\x61\x74\x75\x73" "\n"
);IlllI=(wIndex&(0x1958+1667-0x1edc))-(0xc06+3396-0x1949);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x70\x6f\x72\x74\x3d\x25\x64" "\n"
,IlllI);if(IlllI<(0x87d+3366-0x15a3)||IlllI>=IlIlII){IlllI=-(0x18c6+1289-0x1dce)
;lIlll=-EPIPE;break;}

if(Illll->lIlIlII[IlllI]&&time_after_eq(jiffies,Illll->lIlIlII[IlllI])){if(Illll
->llIllIl[IlllI]==IIlIlII){Illll->IlIIll[IlllI]->llIlIIl=(0x211a+949-0x24cf);}
#if KERNEL_GT_EQ((0x13b7+3248-0x2065),(0x1050+1088-0x148a),(0x73f+228-0x7fc)) ||\
 RHEL_RELEASE_GT_EQ((0x70a+6752-0x2164),(0x677+4124-0x1690)) 
if(IllIl->speed==HCD_USB3){IlIIlII(Illll,IlllI,llIIlIII,(0x1247+3953-0x21b8));}
else
#endif
{IIIlllI(Illll,IlllI,llIIlIII,(0x155+7789-0x1fc2));}}((__le16*)IlIIl)[
(0xc85+5794-0x2327)]=cpu_to_le16(Illll->IIIIIIl[IlllI]);((__le16*)IlIIl)[
(0x54d+5538-0x1aee)]=cpu_to_le16(Illll->lIIIIl[IlllI]);break;case SetPortFeature
:IlllI=(wIndex&(0xbf3+239-0xbe3))-(0xf6c+5604-0x254f);if(IlllI<
(0x420+8659-0x25f3)||IlllI>=IlIlII){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x70\x6f\x72\x74\x20\x6e\x75\x6d\x62\x65\x72\x20\x28\x25\x64\x29" "\n"
,IlllI);IlllI=-(0x1f6f+183-0x2025);lIlll=-EPIPE;break;}
#if KERNEL_GT_EQ((0x3a+4769-0x12d9),(0x7d8+7904-0x26b2),(0x3d3+2000-0xb7c)) || \
RHEL_RELEASE_GT_EQ((0x7fb+7806-0x2673),(0x1375+2403-0x1cd5)) 
if(IllIl->speed==HCD_USB3){lIlll=IIlIIIll(Illll,IlllI,wValue,wIndex>>
(0x53d+5902-0x1c43));}else
#endif
{lIlll=IIIIIIII(Illll,IlllI,wValue,wIndex>>(0xf29+3651-0x1d64));}break;case 
ClearPortFeature:IlllI=(wIndex&(0x744+1354-0xb8f))-(0xab+2002-0x87c);if(IlllI<
(0x1157+5321-0x2620)||IlllI>=IlIlII){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x70\x6f\x72\x74\x20\x6e\x75\x6d\x62\x65\x72\x20\x28\x25\x64\x29" "\n"
,IlllI);IlllI=-(0x1602+3199-0x2280);lIlll=-EPIPE;break;}
#if KERNEL_GT_EQ((0xb03+4208-0x1b71),(0xc3+5326-0x158b),(0xdf4+3600-0x1bdd)) || \
RHEL_RELEASE_GT_EQ((0x24e6+544-0x2700),(0x242+2694-0xcc5)) 
if(IllIl->speed==HCD_USB3){lIlll=llIllIlI(Illll,IlllI,wValue);}else
#endif
{lIlll=IllIlIIl(Illll,IlllI,wValue);}break;default:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x52\x65\x71\x3d\x30\x78\x25\x30\x34\x58\x20\x56\x61\x6c\x75\x65\x3d\x30\x78\x25\x30\x34\x58\x20\x49\x6e\x64\x65\x78\x3d\x30\x78\x25\x30\x34\x58\x69\x20\x4c\x65\x6e\x67\x74\x68\x3d\x25\x64" "\n"
,llIlIllI,wValue,wIndex,wLength);
lIlll=-EPIPE;break;}spin_unlock_irqrestore(&Illll->lock,flags);
if(IlllI!=-(0xd59+5020-0x20f4)&&(Illll->lIIIIl[IlllI]&lIllIlIl)!=
(0x600+7580-0x239c))usb_hcd_poll_rh_status(IllIl);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x20\x3d\x20\x25\x64" "\n"
,lIlll);return lIlll;}static int IIIIIIII(struct IIlllI*Illll,int IlllI,int 
IlIIIIl,int IIlIIll){int lIlll=(0x1575+1154-0x19f7);switch(IlIIIIl){case 
USB_PORT_FEAT_ENABLE:IIIll(
"\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x45\x4e\x41\x42\x4c\x45\x29" "\n"
,IlllI);


lIlll=-EPIPE;break;case USB_PORT_FEAT_SUSPEND:IIIll(
"\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x53\x55\x53\x50\x45\x4e\x44\x29" "\n"
,IlllI);




IIIlllI(Illll,IlllI,lIIlIIll,(0xb2d+6533-0x24b2));break;case USB_PORT_FEAT_RESET
:IIIll(
"\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x52\x45\x53\x45\x54\x29" "\n"
,IlllI);





IIIlllI(Illll,IlllI,llIIIll,(0x5bc+6375-0x1ea3));if(Illll->IlIIll[IlllI]&&Illll
->IlIIll[IlllI]->llIlIIl>(0x1def+1657-0x2468)){IlllllIl(Illll->IlIIll[IlllI]);}
break;case USB_PORT_FEAT_POWER:IIIll(
"\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x50\x4f\x57\x45\x52\x29" "\n"
,IlllI);


IIIlllI(Illll,IlllI,IlIIllIl,(0x143f+2505-0x1e08));break;case USB_PORT_FEAT_TEST
:IIIll(
"\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x54\x45\x53\x54\x29" "\n"
,IlllI);
break;case USB_PORT_FEAT_INDICATOR:IIIll(
"\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x49\x4e\x44\x49\x43\x41\x54\x4f\x52\x29" "\n"
,IlllI);
break;case USB_PORT_FEAT_CONNECTION:case USB_PORT_FEAT_OVER_CURRENT:case 
USB_PORT_FEAT_C_CONNECTION:case USB_PORT_FEAT_C_OVER_CURRENT:case 
USB_PORT_FEAT_C_RESET:case USB_PORT_FEAT_LOWSPEED:case USB_PORT_FEAT_C_ENABLE:
case USB_PORT_FEAT_C_SUSPEND:


break;default:
IIIll(
"\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x20\x25\x64\x29" "\n"
,IlllI,IlIIIIl);lIlll=-EPIPE;break;}return lIlll;}static int IllIlIIl(struct 
IIlllI*Illll,int IlllI,int IlIIIIl){int lIlll=(0x1091+2553-0x1a8a);switch(
IlIIIIl){case USB_PORT_FEAT_ENABLE:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x45\x4e\x41\x42\x4c\x45\x29" "\n"
,IlllI);



IIIlllI(Illll,IlllI,IlllIllI,(0x18e0+1309-0x1dfd));break;case 
USB_PORT_FEAT_SUSPEND:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x53\x55\x53\x50\x45\x4e\x44\x29" "\n"
,IlllI);



IIIlllI(Illll,IlllI,llIIIIIII,(0x7e8+387-0x96b));break;case USB_PORT_FEAT_POWER:
IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x50\x4f\x57\x45\x52\x29" "\n"
,IlllI);

IIIlllI(Illll,IlllI,IllIlll,(0x1bb5+252-0x1cb1));break;case 
USB_PORT_FEAT_C_CONNECTION:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x43\x4f\x4e\x4e\x45\x43\x54\x49\x4f\x4e\x29" "\n"
,IlllI);IIIlllI(Illll,IlllI,IlIllIll,(0x45d+1914-0xbd7));break;case 
USB_PORT_FEAT_C_ENABLE:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x45\x4e\x41\x42\x4c\x45\x29" "\n"
,IlllI);IIIlllI(Illll,IlllI,lIlIIIIlI,(0x52+8904-0x231a));break;case 
USB_PORT_FEAT_C_SUSPEND:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x53\x55\x53\x50\x45\x4e\x44\x29" "\n"
,IlllI);IIIlllI(Illll,IlllI,lIIlIIlI,(0xab0+744-0xd98));break;case 
USB_PORT_FEAT_C_OVER_CURRENT:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x4f\x56\x45\x52\x5f\x43\x55\x52\x52\x45\x4e\x54\x29" "\n"
,IlllI);
break;case USB_PORT_FEAT_C_RESET:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x52\x45\x53\x45\x54\x29" "\n"
,IlllI);IIIlllI(Illll,IlllI,llllllIl,(0x9+559-0x238));break;case 
USB_PORT_FEAT_INDICATOR:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x49\x4e\x44\x49\x43\x41\x54\x4f\x52\x29" "\n"
,IlllI);
break;case USB_PORT_FEAT_CONNECTION:case USB_PORT_FEAT_OVER_CURRENT:case 
USB_PORT_FEAT_RESET:case USB_PORT_FEAT_TEST:


break;default:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x20\x25\x64\x29" "\n"
,IlllI,IlIIIIl);lIlll=-EPIPE;break;}return lIlll;}
#if KERNEL_GT_EQ((0x1096+3061-0x1c89),(0x66f+4673-0x18aa),(0x8f1+7291-0x2545)) \
|| RHEL_RELEASE_GT_EQ((0x21a0+947-0x254d),(0x788+3579-0x1580)) 
static int IIlIIIll(struct IIlllI*Illll,int IlllI,int IlIIIIl,int IIlIIll){int 
lIlll=(0x1032+898-0x13b4);switch(IlIIIIl){case USB_PORT_FEAT_BH_PORT_RESET:IIIll
(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x42\x48\x5f\x50\x4f\x52\x54\x5f\x52\x45\x53\x45\x54\x29" "\n"
,IlllI);







case USB_PORT_FEAT_RESET:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x52\x45\x53\x45\x54\x29" "\n"
,IlllI);





IlIIlII(Illll,IlllI,llIIIll,(0x7dd+4705-0x1a3e));if(Illll->IlIIll[IlllI]&&Illll
->IlIIll[IlllI]->llIlIIl>(0x1233+2289-0x1b24)){IlllllIl(Illll->IlIIll[IlllI]);}
break;case USB_PORT_FEAT_LINK_STATE:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x4c\x49\x4e\x4b\x5f\x53\x54\x41\x54\x45\x2c\x20\x30\x78\x25\x30\x34\x78\x29" "\n"
,IlllI,IIlIIll);


IlIIlII(Illll,IlllI,lllIlll,IIlIIll);break;case USB_PORT_FEAT_POWER:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x50\x4f\x57\x45\x52\x29" "\n"
,IlllI);

IlIIlII(Illll,IlllI,IlIIllIl,(0x1976+483-0x1b59));break;case 
USB_PORT_FEAT_U1_TIMEOUT:case USB_PORT_FEAT_U2_TIMEOUT:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x55\x31\x2f\x55\x32\x5f\x54\x49\x4d\x45\x4f\x55\x54\x29" "\n"
,IlllI);


break;case USB_PORT_FEAT_REMOTE_WAKE_MASK:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x52\x45\x4d\x4f\x54\x45\x5f\x57\x41\x4b\x45\x5f\x4d\x41\x53\x4b\x29" "\n"
,IlllI);

break;case USB_PORT_FEAT_FORCE_LINKPM_ACCEPT:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x46\x4f\x52\x43\x45\x5f\x4c\x49\x4e\x4b\x50\x4d\x5f\x41\x43\x43\x45\x50\x54\x29" "\n"
,IlllI);
break;case USB_PORT_FEAT_CONNECTION:case USB_PORT_FEAT_OVER_CURRENT:case 
USB_PORT_FEAT_C_CONNECTION:case USB_PORT_FEAT_C_OVER_CURRENT:case 
USB_PORT_FEAT_C_RESET:case USB_PORT_FEAT_C_PORT_LINK_STATE:case 
USB_PORT_FEAT_C_PORT_CONFIG_ERROR:case USB_PORT_FEAT_C_BH_PORT_RESET:


break;default:
IIIll(
"\x53\x65\x74\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x20\x25\x64\x29" "\n"
,IlllI,IlIIIIl);lIlll=-EPIPE;break;}return lIlll;}static int llIllIlI(struct 
IIlllI*Illll,int IlllI,int IlIIIIl){int lIlll=(0x1160+3440-0x1ed0);switch(
IlIIIIl){case USB_PORT_FEAT_POWER:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x50\x4f\x57\x45\x52\x29" "\n"
,IlllI);

IlIIlII(Illll,IlllI,IllIlll,(0x356+1323-0x881));break;case 
USB_PORT_FEAT_C_CONNECTION:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x43\x4f\x4e\x4e\x45\x43\x54\x49\x4f\x4e\x29" "\n"
,IlllI);IlIIlII(Illll,IlllI,IlIllIll,(0xa0+2170-0x91a));break;case 
USB_PORT_FEAT_C_OVER_CURRENT:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x4f\x56\x45\x52\x5f\x43\x55\x52\x52\x45\x4e\x54\x29" "\n"
,IlllI);
break;case USB_PORT_FEAT_C_RESET:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x52\x45\x53\x45\x54\x29" "\n"
,IlllI);IlIIlII(Illll,IlllI,llllllIl,(0x8f6+2532-0x12da));break;case 
USB_PORT_FEAT_C_PORT_LINK_STATE:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x50\x4f\x52\x54\x5f\x4c\x49\x4e\x4b\x5f\x53\x54\x41\x54\x45\x29" "\n"
,IlllI);IlIIlII(Illll,IlllI,IlIlIlIlI,(0xa80+3816-0x1968));break;case 
USB_PORT_FEAT_C_PORT_CONFIG_ERROR:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x50\x4f\x52\x54\x5f\x43\x4f\x4e\x46\x49\x47\x5f\x45\x52\x52\x4f\x52\x29" "\n"
,IlllI);
break;case USB_PORT_FEAT_C_BH_PORT_RESET:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x43\x5f\x42\x48\x5f\x50\x4f\x52\x54\x5f\x52\x45\x53\x45\x54\x29" "\n"
,IlllI);IlIIlII(Illll,IlllI,lIlllIll,(0x31b+4089-0x1314));break;case 
USB_PORT_FEAT_FORCE_LINKPM_ACCEPT:IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x63\x6f\x6e\x74\x72\x6f\x6c\x3a\x20\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x55\x53\x42\x5f\x50\x4f\x52\x54\x5f\x46\x45\x41\x54\x5f\x46\x4f\x52\x43\x45\x5f\x4c\x49\x4e\x4b\x50\x4d\x5f\x41\x43\x43\x45\x50\x54\x29" "\n"
,IlllI);
break;case USB_PORT_FEAT_CONNECTION:case USB_PORT_FEAT_OVER_CURRENT:case 
USB_PORT_FEAT_RESET:case USB_PORT_FEAT_LINK_STATE:case USB_PORT_FEAT_U1_TIMEOUT:
case USB_PORT_FEAT_U2_TIMEOUT:case USB_PORT_FEAT_REMOTE_WAKE_MASK:case 
USB_PORT_FEAT_BH_PORT_RESET:


break;default:IIIll(
"\x43\x6c\x65\x61\x72\x50\x6f\x72\x74\x46\x65\x61\x74\x75\x72\x65\x28\x25\x64\x2c\x20\x25\x64\x29" "\n"
,IlllI,IlIIIIl);lIlll=-EPIPE;break;}return lIlll;}
#endif 
static int lIlIllIII(struct usb_hcd*IllIl,char*IlIIl){int lIlll=(0xf+3273-0xcd8)
;struct IIlllI*Illll=IIllIl(IllIl);u32*llIIlIll=(u32*)IlIIl;unsigned long flags;
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x73\x74\x61\x74\x75\x73\x5f\x64\x61\x74\x61\x5b\x25\x73\x5d\x2b\x2b" "\n"
,llIIIlI(IllIl));spin_lock_irqsave(&Illll->lock,flags);do
{int i;int llIllllI=(0x15ab+2034-0x1d9d);if(!test_bit(HCD_FLAG_HW_ACCESSIBLE,&
IllIl->flags))break;for(i=(0x7a7+989-0xb84);i<IlIlII;i++){if(Illll->lIIIIl[i]&
lIllIlIl){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x73\x74\x61\x74\x75\x73\x5f\x64\x61\x74\x61\x3a\x20\x70\x6f\x72\x74\x20\x25\x64\x20\x68\x61\x73\x20\x63\x68\x61\x6e\x67\x65\x64\x2e\x20\x77\x50\x6f\x72\x74\x53\x74\x61\x74\x75\x73\x3d\x30\x78\x25\x30\x34\x58\x20\x77\x50\x6f\x72\x74\x43\x68\x61\x6e\x67\x65\x3d\x30\x78\x25\x30\x34\x58" "\n"
,i,Illll->IIIIIIl[i],Illll->lIIIIl[i]);if(llIllllI==(0x3b3+987-0x78e))*llIIlIll=
(0x827+4290-0x18e9);llIllllI=(0x7b3+4701-0x1a0f);
*llIIlIll|=(0x9d8+4435-0x1b2a)<<(i+(0x890+5027-0x1c32));}}if(llIllllI){lIlll=
lllIIlll;if(Illll->llIIlIlI){usb_hcd_resume_root_hub(IllIl);}}}while(
(0x15a6+3453-0x2323));spin_unlock_irqrestore(&Illll->lock,flags);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x68\x75\x62\x5f\x73\x74\x61\x74\x75\x73\x5f\x64\x61\x74\x61\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x20\x3d\x20\x25\x64\x20\x6d\x61\x73\x6b\x20\x3d\x20\x30\x78\x25\x30\x38\x58" "\n"
,lIlll,(u32)*llIIlIll);return lIlll;}static int lIIlllIl(struct usb_hcd*IllIl){
struct IIlllI*Illll=IIllIl(IllIl);Illll->llIIlIlI=(0x962+1907-0x10d4);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x62\x75\x73\x5f\x73\x75\x73\x70\x65\x6e\x64\x5b\x25\x73\x5d" "\n"
,llIIIlI(IllIl));return(0x692+6107-0x1e6d);}static int llIIIlll(struct usb_hcd*
IllIl){
struct IIlllI*Illll=IIllIl(IllIl);Illll->llIIlIlI=(0x714+4344-0x180c);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x62\x75\x73\x5f\x72\x65\x73\x75\x6d\x65\x5b\x25\x73\x5d" "\n"
,llIIIlI(IllIl));return(0x3+7760-0x1e53);}
#if KERNEL_GT_EQ((0x690+7387-0x2369),(0x1196+1321-0x16b9),(0x1940+233-0x1a02)) \
|| RHEL_RELEASE_GT_EQ((0xf42+4026-0x1ef6),(0xb11+5319-0x1fd5)) 
static int lllIIlIl(struct usb_hcd*IllIl,struct usb_device*lIlIIl,struct 
usb_host_endpoint**lIIIlIll,unsigned int IIlIIIlI,unsigned int IlIIlIIll,gfp_t 
lIIllI){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x61\x6c\x6c\x6f\x63\x5f\x73\x74\x72\x65\x61\x6d\x73\x5b\x25\x73\x5d" "\n"
,llIIIlI(IllIl));
return(0x1056+4676-0x229a);}static int IIIIIllII(struct usb_hcd*IllIl,struct 
usb_device*lIlIIl,struct usb_host_endpoint**lIIIlIll,unsigned int IIlIIIlI,gfp_t
 lIIllI){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x66\x72\x65\x65\x5f\x73\x74\x72\x65\x61\x6d\x73\x5b\x25\x73\x5d" "\n"
,llIIIlI(IllIl));
return(0xd47+1874-0x1499);}
#endif 


int llIlIIIIl(void){int lIlll=(0x16a6+825-0x19df);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x69\x6e\x69\x74\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x2b\x2b" "\n"
);do{struct sysinfo lIlllIl;INIT_LIST_HEAD(&IlllllI);spin_lock_init(&lIllII);
si_meminfo(&lIlllIl);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x69\x6e\x69\x74\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x74\x6f\x74\x61\x6c\x72\x61\x6d\x3d\x25\x6c\x75\x20\x62\x79\x74\x65\x73\x20\x74\x6f\x74\x61\x6c\x68\x69\x67\x68\x3d\x25\x6c\x75\x20\x62\x79\x74\x65\x73" "\n"
,lIlllIl.totalram*lIlllIl.mem_unit,lIlllIl.totalhigh*lIlllIl.mem_unit);
#if defined(IIlIlIll) && !defined(_USBD_VHCI_NO_DMA_)
















lIIIlIl=(lIlllIl.totalhigh>(0xc+7435-0x1d17));
#endif
lIlll=platform_driver_register(&lllllIlll);if(lIlll!=(0x6e8+3863-0x15ff)){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x69\x6e\x69\x74\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x70\x6c\x61\x74\x66\x6f\x72\x6d\x5f\x64\x72\x69\x76\x65\x72\x5f\x72\x65\x67\x69\x73\x74\x65\x72\x20\x66\x61\x69\x6c\x65\x64\x2e\x20\x45\x72\x72\x6f\x72\x20\x6e\x75\x6d\x62\x65\x72\x20\x25\x64" "\n"
,lIlll);break;}lIlll=platform_device_register(&lIlIllll);if(lIlll!=
(0x1c87+1125-0x20ec)){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x69\x6e\x69\x74\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x70\x6c\x61\x74\x66\x6f\x72\x6d\x5f\x64\x65\x76\x69\x63\x65\x5f\x72\x65\x67\x69\x73\x74\x65\x72\x20\x66\x61\x69\x6c\x65\x64\x2e\x20\x45\x72\x72\x6f\x72\x20\x6e\x75\x6d\x62\x65\x72\x20\x25\x64" "\n"
,lIlll);break;}}while((0xa28+2171-0x12a3));IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x69\x6e\x69\x74\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x2d\x2d\x20\x72\x65\x73\x75\x6c\x74\x20\x3d\x20\x25\x64" "\n"
,lIlll);return lIlll;}void IllIIllIl(void){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x64\x65\x69\x6e\x69\x74\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x2b\x2b" "\n"
);spin_lock(&lIllII);while(!list_empty(&IlllllI)){struct IIIII*llllI=(struct 
IIIII*)IlllllI.next;lIIIllII(llllI);spin_unlock(&lIllII);IllllIIl(llllI);IIIIllI
(llllI);spin_lock(&lIllII);}spin_unlock(&lIllII);platform_device_unregister(&
lIlIllll);platform_driver_unregister(&lllllIlll);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x64\x65\x69\x6e\x69\x74\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x2d\x2d" "\n"
);}struct IIIII*IlIIIIlIl(u32 IlIlIll,u32 IllllIl,int speed){struct IIIII*lllll;
do{lllll=kmalloc(sizeof(*lllll),GFP_KERNEL);if(!lllll){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x72\x65\x61\x74\x65\x3a\x20\x6b\x6d\x61\x6c\x6c\x6f\x63\x20\x66\x61\x69\x6c\x65\x64\x2e" "\n"
);break;}memset(lllll,(0x154c+2155-0x1db7),sizeof(*lllll));lllll->IIlII=
lIlIlIIIl(lllll);if(!lllll->IIlII){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x72\x65\x61\x74\x65\x3a\x20\x75\x73\x62\x64\x5f\x6d\x76\x5f\x61\x6c\x6c\x6f\x63\x5f\x64\x65\x73\x63\x72\x69\x70\x74\x6f\x72\x20\x66\x61\x69\x6c\x65\x64\x2e" "\n"
);break;}IIIllIIl(lllll->IIlII);if(lllll->IIlII->IlllIl==-(0x1214+4030-0x21d1)){
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x72\x65\x61\x74\x65\x3a\x20\x75\x73\x62\x64\x5f\x63\x64\x65\x76\x5f\x61\x6c\x6c\x6f\x63\x5f\x6d\x69\x6e\x6f\x72\x20\x66\x61\x69\x6c\x65\x64\x2e" "\n"
);break;}init_waitqueue_head(&lllll->lllIII);INIT_LIST_HEAD(&lllll->lllIll);
spin_lock_init(&lllll->IIllI);INIT_LIST_HEAD(&lllll->llIIIIl);spin_lock_init(&
lllll->IIllII);INIT_LIST_HEAD(&lllll->IlIIlll);spin_lock_init(&lllll->lIlIlll);
kref_init(&lllll->IlIlIl);lllll->IlIlIll=IlIlIll;lllll->IllllIl=IllllIl;lllll->
speed=speed;
#if KERNEL_GT_EQ((0xdd0+4323-0x1eb1),(0xfb+2936-0xc6d),(0x1321+2428-0x1c76)) || \
RHEL_RELEASE_GT_EQ((0x2c3+890-0x637),(0xe0d+3903-0x1d49)) 
lllll->parent=(speed==USB_SPEED_SUPER)?llIIllI:IIlIIlI;
#else
lllll->parent=IIlIIlI;
#endif
lllll->IlllI=-(0x10a2+913-0x1432);spin_lock(&lIllII);list_add(&lllll->llIIll,&
IlllllI);spin_unlock(&lIllII);return lllll;}while((0xd56+6032-0x24e6));if(lllll)
{if(lllll->IIlII){IIllIlII(lllll->IIlII->IlllIl);llIllIII(lllll->IIlII);}kfree(
lllll);}return NULL;}void IllllIIl(struct IIIII*lllll){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x72\x65\x65\x2b\x2b" "\n"
);if(!lllll)return;llIIIIll(lllll);spin_lock(&lIllII);list_del_init(&lllll->
llIIll);spin_unlock(&lIllII);while(!list_empty(&lllll->lllIll)){struct lIlIII*
lIIll=list_entry(lllll->lllIll.next,struct lIlIII,lIIIl);list_del(&lIIll->lIIIl)
;kfree(lIIll);}while(!list_empty(&lllll->llIIIIl)){struct lIlIII*lIIll=
list_entry(lllll->llIIIIl.next,struct lIlIII,lIIIl);list_del(&lIIll->lIIIl);
kfree(lIIll);}while(!list_empty(&lllll->IlIIlll)){struct lIlIII*lIIll=list_entry
(lllll->IlIIlll.next,struct lIlIII,lIIIl);list_del(&lIIll->lIIIl);kfree(lIIll);}
IIIIllI(lllll);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x72\x65\x65\x2d\x2d" "\n"
);}void llIlIlIll(struct IIIII*lllll){unsigned long flags;int IlllI;struct 
IIlllI*Illll;struct IIlllI*IIlllll;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x2b\x2b" "\n"
);Illll=IIllIl(lllll->parent);IIlllll=Illll->lllllII?IIllIl(Illll->lllllII):NULL
;spin_lock_irqsave(&Illll->lock,flags);if(IIlllll)spin_lock(&IIlllll->lock);for(
IlllI=(0x9d6+58-0xa10);IlllI<IlIlII;IlllI++){if((Illll->IlIIll[IlllI]==NULL)&&((
IIlllll==NULL)||(IIlllll->IlIIll[IlllI]==NULL))){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x66\x6f\x75\x6e\x64\x20\x66\x72\x65\x65\x20\x70\x6f\x72\x74\x20\x25\x64" "\n"
,IlllI);lIIIllII(lllll);lllll->IlllI=IlllI;lllll->llIlIIl=-(0xd71+113-0xde1);
Illll->IlIIll[IlllI]=lllll;
#if KERNEL_GT_EQ((0xbf3+3543-0x19c8),(0xc23+1175-0x10b4),(0x17b0+1503-0x1d68)) \
|| RHEL_RELEASE_GT_EQ((0x7a8+3051-0x138d),(0xa31+4403-0x1b61)) 
if(lllll->parent->speed==HCD_USB3){IlIIlII(Illll,IlllI,llIlllIl,
(0x14f5+303-0x1624));}else
#endif
{IIIlllI(Illll,IlllI,llIlllIl,(0x76c+6049-0x1f0d));}break;}}if(IIlllll)
spin_unlock(&IIlllll->lock);spin_unlock_irqrestore(&Illll->lock,flags);
usb_hcd_poll_rh_status(lllll->parent);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x2d\x2d" "\n"
);}void llIIIIll(struct IIIII*lllll){unsigned long flags;struct IIlllI*Illll=
IIllIl(lllll->parent);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x64\x69\x73\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x2b\x2b" "\n"
);spin_lock_irqsave(&Illll->lock,flags);if(lllll->IlllI!=-(0x811+6365-0x20ed)){
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x64\x69\x73\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x64\x69\x73\x63\x6f\x6e\x6e\x65\x63\x74\x69\x6e\x67\x20\x70\x6f\x72\x74\x20\x25\x64" "\n"
,lllll->IlllI);Illll->IlIIll[lllll->IlllI]=NULL;
#if KERNEL_GT_EQ((0x6a9+2594-0x10c9),(0x163b+4061-0x2612),(0xd3+507-0x2a7)) || \
RHEL_RELEASE_GT_EQ((0x133b+812-0x1661),(0x123a+4317-0x2314)) 
if(lllll->parent->speed==HCD_USB3){IlIIlII(Illll,lllll->IlllI,IIllIlI,
(0x1457+3609-0x2270));}else
#endif
{IIIlllI(Illll,lllll->IlllI,IIllIlI,(0x10d+6844-0x1bc9));}lllll->IlllI=-
(0xb37+4932-0x1e7a);spin_unlock_irqrestore(&Illll->lock,flags);
usb_hcd_poll_rh_status(lllll->parent);IIlIIlII(lllll);IIIIllI(lllll);}else{
spin_unlock_irqrestore(&Illll->lock,flags);}IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x64\x69\x73\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x2d\x2d" "\n"
);}struct IIIII*lIIlIlIl(u32 IlIlIll,u32 IllllIl){struct IIIII*lllll;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x69\x6e\x64\x31\x3a\x20\x2b\x2b" "\n"
);spin_lock(&lIllII);list_for_each_entry(lllll,&IlllllI,llIIll){if(lllll->
IlIlIll==IlIlIll&&lllll->IllllIl==IllllIl){lIIIllII(lllll);spin_unlock(&lIllII);
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x69\x6e\x64\x31\x3a\x20\x2d\x2d\x20\x66\x6f\x75\x6e\x64\x21" "\n"
);return lllll;}}spin_unlock(&lIllII);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x69\x6e\x64\x31\x3a\x20\x2d\x2d\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
);return NULL;}struct IIIII*lIIIlllll(struct usb_hcd*IllIl,int llIlIIl){struct 
IIlllI*Illll=IIllIl(IllIl);struct IIIII*lllll=NULL;int IlllI;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x69\x6e\x64\x32\x28\x29\x3a\x20\x2b\x2b\x20\x61\x64\x64\x72\x65\x73\x73\x20\x3d\x20\x25\x64" "\n"
,llIlIIl);if(Illll==NULL)return NULL;if(llIlIIl<(0xba1+5019-0x1f3c))return NULL;
spin_lock(&Illll->lock);for(IlllI=(0x962+2513-0x1333);IlllI<IlIlII;IlllI++){if(
Illll->IlIIll[IlllI]&&Illll->IlIIll[IlllI]->llIlIIl==llIlIIl){lllll=Illll->
IlIIll[IlllI];lIIIllII(lllll);break;}}spin_unlock(&Illll->lock);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x69\x6e\x64\x32\x3a\x20\x2d\x2d\x20\x25\x73" "\n"
,lllll?"\x66\x6f\x75\x6e\x64":"\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64");return 
lllll;}struct IIIII*IlIlllIII(struct usb_hcd*IllIl,int IlllI){struct IIlllI*
Illll=IIllIl(IllIl);struct IIIII*lllll=NULL;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x69\x6e\x33\x32\x28\x29\x3a\x20\x2b\x2b\x20\x70\x6f\x72\x74\x20\x3d\x20\x25\x64" "\n"
,IlllI);if(Illll==NULL)return NULL;if(IlllI<=(0x1817+1521-0x1e08)||IlllI>IlIlII)
return NULL;IlllI--;
spin_lock(&Illll->lock);if(Illll->IlIIll[IlllI]){lllll=Illll->IlIIll[IlllI];
lIIIllII(lllll);}spin_unlock(&Illll->lock);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x66\x69\x6e\x64\x33\x3a\x20\x2d\x2d\x20\x25\x73" "\n"
,lllll?"\x66\x6f\x75\x6e\x64":"\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64");return 
lllll;}void IlllllIl(struct IIIII*llllI){llIII IIlIl;struct lIlIII*lIIll;
unsigned long flags;lIIll=kmalloc(sizeof(struct lIlIII),GFP_ATOMIC);if(!lIIll){
IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x73\x65\x6e\x64\x5f\x72\x65\x73\x65\x74\x5f\x70\x6f\x72\x74\x3a\x20\x6e\x6f\x20\x6d\x65\x6d\x6f\x72\x79" "\n"
);return;}IIlIl=kmalloc(sizeof(IlIllIlI),GFP_ATOMIC);if(!IIlIl){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x73\x65\x6e\x64\x5f\x72\x65\x73\x65\x74\x5f\x70\x6f\x72\x74\x3a\x20\x6e\x6f\x20\x6d\x65\x6d\x6f\x72\x79" "\n"
);kfree(lIIll);return;}INIT_LIST_HEAD(&lIIll->lIIIl);INIT_LIST_HEAD(&lIIll->
llIIlIl);INIT_LIST_HEAD(&lIIll->urb.llIIll);lIIll->IlIIII=IlIlllIl();lIIll->urb.
lIlII=NULL;lIIll->IIlIl=IIlIl;lIIll->llllI=llllI;IIlIl->IIIlI.IIIIll=lIIll->
IlIIII;IIlIl->IIIlI.lIlIl=sizeof(IlIllIlI);IIlIl->IIIlI.lIlllI=llllIlll;IIlIl->
IIIlI.Status=(0x229d+1137-0x270e);IIlIl->IIIlI.Context=llllI->IlllI;
spin_lock_irqsave(&llllI->IIllII,flags);list_add_tail(&lIIll->lIIIl,&llllI->
llIIIIl);spin_unlock_irqrestore(&llllI->IIllII,flags);wake_up(&llllI->lllIII);}
void IllIIIIl(struct urb*lIlII,int status){struct IIIII*llllI=lIlII->hcpriv;
#if KERNEL_LT((0x6bd+6493-0x2018),(0x2ea+7462-0x200a),(0x1f34+1855-0x265b))
if(status!=-(0x75c+2660-0x11bf)){spin_lock(&lIlII->lock);if(lIlII->status==-
EINPROGRESS)lIlII->status=status;spin_unlock(&lIlII->lock);}
#endif
lIlII->hcpriv=NULL;if(llllI){
#if KERNEL_LT((0x123d+301-0x1368),(0xa9f+4823-0x1d70),(0xbf1+5832-0x22a6))
usb_hcd_giveback_urb(llllI->parent,lIlII,NULL);
#elif KERNEL_LT((0xb14+3055-0x1701),(0x1cb0+1557-0x22bf),(0x1b6+2276-0xa82))
usb_hcd_giveback_urb(llllI->parent,lIlII);
#else
usb_hcd_giveback_urb(llllI->parent,lIlII,status);
#endif
IIIIllI(llllI);}}struct lIlIII*llIIllIll(struct IIIII*llllI,struct urb*lIlII){
unsigned long flags;struct lIlIII*lIIll;struct usb_host_endpoint*ep;
spin_lock_irqsave(&llllI->IIllII,flags);spin_lock(&llllI->lIlIlll);
#if KERNEL_LT((0xd86+4552-0x1f4c),(0x4f1+595-0x73e),(0x27b+310-0x399))
ep=(usb_pipein(lIlII->pipe)?lIlII->dev->ep_in:lIlII->dev->ep_out)[
usb_pipeendpoint(lIlII->pipe)];
#else
ep=lIlII->ep;
#endif
if(ep){spin_lock(&IIllIl(llllI->parent)->lock);
lIIll=ep->hcpriv;if(lIIll){struct lIllIll*IllIII;list_for_each_entry(IllIII,&
lIIll->llIIlIl,llIIll){if(IllIII->lIlII==lIlII){ep->hcpriv=NULL;
list_del_init(&lIIll->lIIIl);spin_unlock(&IIllIl(llllI->parent)->lock);
spin_unlock(&llllI->lIlIlll);spin_unlock_irqrestore(&llllI->IIllII,flags);return
 lIIll;}}}spin_unlock(&IIllIl(llllI->parent)->lock);}
list_for_each_entry(lIIll,&llllI->IlIIlll,lIIIl){struct lIllIll*IllIII;
list_for_each_entry(IllIII,&lIIll->llIIlIl,llIIll){if(IllIII->lIlII==lIlII){
list_del_init(&lIIll->lIIIl);spin_unlock(&llllI->lIlIlll);spin_unlock_irqrestore
(&llllI->IIllII,flags);return lIIll;}}}
list_for_each_entry(lIIll,&llllI->llIIIIl,lIIIl){struct lIllIll*IllIII;
list_for_each_entry(IllIII,&lIIll->llIIlIl,llIIll){if(IllIII->lIlII==lIlII){
list_del_init(&lIIll->lIIIl);spin_unlock(&llllI->lIlIlll);spin_unlock_irqrestore
(&llllI->IIllII,flags);return lIIll;}}}spin_unlock(&llllI->lIlIlll);
spin_unlock_irqrestore(&llllI->IIllII,flags);return NULL;}int lIlIIlIIl(struct 
lIlIII*lIIll){struct lIllIll*IllIII;
list_for_each_entry(IllIII,&lIIll->llIIlIl,llIIll){
#if KERNEL_LT((0xf1f+3654-0x1d63),(0xd9a+559-0xfc3),(0x1ef+2824-0xcdf))
spin_lock(&IllIII->lIlII->lock);if(IllIII->lIlII->status!=-EINPROGRESS)
#else
if(IllIII->lIlII->unlinked)
#endif
{
#if KERNEL_LT((0x1692+3504-0x2440),(0x1223+1114-0x1677),(0x164+9130-0x24f6))
spin_unlock(&IllIII->lIlII->lock);
#endif
return(0x446+7505-0x2196);}
#if KERNEL_LT((0x1bd+6844-0x1c77),(0x394+5137-0x179f),(0x8b1+3079-0x14a0))
spin_unlock(&IllIII->lIlII->lock);
#endif
}return(0x6cc+5947-0x1e07);}void IIlIIlII(struct IIIII*llllI){unsigned long 
flags;struct list_head IlIlIII;struct lIlIII*lIIll;INIT_LIST_HEAD(&IlIlIII);
spin_lock_irqsave(&llllI->IIllII,flags);spin_lock(&llllI->lIlIlll);spin_lock(&
IIllIl(llllI->parent)->lock);list_for_each_entry(lIIll,&llllI->lllIll,lIIIl){
struct lIllIll*lIlIll;list_for_each_entry(lIlIll,&lIIll->llIIlIl,llIIll){struct 
usb_host_endpoint*ep;
#if KERNEL_LT((0x867+6930-0x2377),(0x4e4+7309-0x216b),(0x86c+6971-0x238f))
ep=(usb_pipein(lIlIll->lIlII->pipe)?lIlIll->lIlII->dev->ep_in:lIlIll->lIlII->dev
->ep_out)[usb_pipeendpoint(lIlIll->lIlII->pipe)];
#else
ep=lIlIll->lIlII->ep;
#endif
if(ep)ep->hcpriv=NULL;}}list_splice_init(&llllI->lllIll,&IlIlIII);
list_splice_init(&llllI->llIIIIl,&IlIlIII);list_splice_init(&llllI->IlIIlll,&
IlIlIII);spin_unlock(&IIllIl(llllI->parent)->lock);spin_unlock(&llllI->lIlIlll);
spin_unlock_irqrestore(&llllI->IIllII,flags);while(!list_empty(&IlIlIII)){struct
 lIlIII*lIIll=list_entry(IlIlIII.next,struct lIlIII,lIIIl);list_del_init(&lIIll
->lIIIl);llIllll(llllI,lIIll,-ENODEV);kfree(lIIll);}}void llIllll(struct IIIII*
llllI,struct lIlIII*lIIll,int status){while(!list_empty(&lIIll->llIIlIl)){struct
 lIllIll*IllIII=list_entry(lIIll->llIIlIl.next,struct lIllIll,llIIll);if(IllIII
->lIlII){int llIlIIII=status;if(usb_pipetype(IllIII->lIlII->pipe)==PIPE_BULK&&
status==(0xa12+4866-0x1d14)){






if(IllIII!=&lIIll->urb&&IllIII->lIlII->actual_length==(0x393+7020-0x1eff)){IIIll
(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x62\x75\x6c\x6b\x20\x74\x72\x61\x6e\x73\x66\x65\x72\x20\x61\x63\x74\x75\x61\x6c\x5f\x6c\x65\x6e\x67\x74\x68\x3d\x3d\x30" "\n"
);llIlIIII=-ECONNRESET;}else if((IllIII->lIlII->transfer_flags&URB_SHORT_NOT_OK)
&&(IllIII->lIlII->actual_length<IllIII->lIlII->transfer_buffer_length)){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x73\x68\x6f\x72\x74\x20\x62\x75\x6c\x6b\x20\x74\x72\x61\x6e\x73\x66\x65\x72" "\n"
);llIlIIII=-EREMOTEIO;}}else if(usb_pipetype(IllIII->lIlII->pipe)==
PIPE_INTERRUPT&&status==(0x53f+4506-0x16d9)){if((IllIII->lIlII->transfer_flags&
URB_SHORT_NOT_OK)&&(IllIII->lIlII->actual_length<IllIII->lIlII->
transfer_buffer_length)){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x73\x68\x6f\x72\x74\x20\x69\x6e\x74\x65\x72\x72\x75\x70\x74\x20\x74\x72\x61\x6e\x73\x66\x65\x72" "\n"
);llIlIIII=-EREMOTEIO;}}
#if KERNEL_GT_EQ((0xcab+6345-0x2572),(0x1611+2239-0x1eca),(0x1583+2904-0x20c3))
{unsigned long flags;struct IIlllI*Illll=IIllIl(llllI->parent);spin_lock_irqsave
(&Illll->lock,flags);

usb_hcd_unlink_urb_from_ep(llllI->parent,IllIII->lIlII);spin_unlock_irqrestore(&
Illll->lock,flags);}
#endif
IllIIIIl(IllIII->lIlII,llIlIIII);IllIII->lIlII=NULL;}list_del_init(&IllIII->
llIIll);if(IllIII!=&lIIll->urb)kfree(IllIII);}if(lIIll->IIlIl){if(lIIll->IIlIl->
IIIlI.lIlllI==llllIlll){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x72\x65\x73\x65\x74\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64" "\n"
);llllI->llllllll=(0xab+1858-0x7ed);wake_up(&llllI->lllIII);}IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x63\x6f\x6d\x70\x6c\x65\x74\x65\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x66\x72\x65\x65\x69\x6e\x67\x20\x75\x6e\x72\x62" "\n"
);kfree(lIIll->IIlIl);lIIll->IIlIl=NULL;}}int IlIIllIll(struct IIIII*llllI,
struct lIlIII*lIIll,llIII IIlIl){int lIlll=(0x142a+1928-0x1bb2);do
{unsigned long flags;if(!list_empty(&lIIll->llIIlIl)){lIlll=lllIllIl(&lIIll->
llIIlIl,lIIll->IlIIII,IIlIl);if(lIlll!=(0x92+676-0x336)){IIIIlll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x70\x61\x63\x6b\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x63\x61\x6e\x20\x6e\x6f\x74\x20\x70\x61\x63\x6b\x20\x72\x65\x71\x75\x65\x73\x74\x2c\x20\x72\x65\x73\x75\x6c\x74\x3d\x25\x64" "\n"
,lIlll);lIlll=-ENODATA;llIllll(llllI,lIIll,-EINVAL);break;}IIllIII(IIlIl);
spin_lock_irqsave(&llllI->lIlIlll,flags);if(lIlIIlIIl(lIIll)){
spin_unlock_irqrestore(&llllI->lIlIlll,flags);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x70\x61\x63\x6b\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x72\x65\x71\x75\x65\x73\x74\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20\x63\x61\x6e\x63\x65\x6c\x6c\x65\x64\x21" "\n"
);llIllll(llllI,lIIll,-(0xe4+4949-0x1438));lIlll=-ENODATA;break;}
list_add_tail(&lIIll->lIIIl,&llllI->IlIIlll);spin_unlock_irqrestore(&llllI->
lIlIlll,flags);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x70\x61\x63\x6b\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x72\x65\x71\x75\x65\x73\x74\x20\x63\x6f\x70\x69\x65\x64\x20\x73\x75\x63\x63\x65\x73\x73\x66\x75\x6c\x6c\x79" "\n"
);
lIIll=NULL;break;}if(lIIll->IIlIl){memcpy(IIlIl,lIIll->IIlIl,lIIll->IIlIl->IIIlI
.lIlIl);IIllIII(IIlIl);if(IIlIl->IIIlI.lIlllI==llllIlll){llllI->llllllll=
(0x5c8+2476-0xf73);
spin_lock_irqsave(&llllI->lIlIlll,flags);list_add_tail(&lIIll->lIIIl,&llllI->
IlIIlll);spin_unlock_irqrestore(&llllI->lIlIlll,flags);lIIll=NULL;}else kfree(
lIIll->IIlIl);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x70\x61\x63\x6b\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x75\x6e\x72\x62\x20\x63\x6f\x70\x69\x65\x64\x20\x73\x75\x63\x63\x65\x73\x73\x66\x75\x6c\x6c\x79\x28\x32\x29" "\n"
);break;}lIlll=-ENODATA;}while((0x252+1143-0x6c9));if(lIIll)kfree(lIIll);return 
lIlll;}int IIIIIIll(struct IIIII*llllI,struct lIlIII*lIIll,llIII IIlIl){do
{if(!list_empty(&lIIll->llIIlIl)){int status;IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x75\x6e\x70\x61\x63\x6b\x69\x6e\x67\x20\x75\x72\x62" "\n"
);status=llIllllII(IIlIl,&lIIll->llIIlIl);llIllll(llllI,lIIll,status);}if(lIIll
->IIlIl){if(IIlIl->IIIlI.lIlllI==llllIlll){IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x72\x65\x73\x65\x74\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64" "\n"
);llllI->llllllll=(0x168b+2301-0x1f88);wake_up(&llllI->lllIII);}IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x72\x65\x71\x75\x65\x73\x74\x3a\x20\x66\x72\x65\x65\x69\x6e\x67\x20\x75\x6e\x72\x62" "\n"
);kfree(lIIll->IIlIl);}kfree(lIIll);}while((0xd62+4903-0x2089));return
(0x9e6+206-0xab4);}void lllllllI(struct kref*IlllIIl){struct IIIII*lllll=
container_of(IlllIIl,struct IIIII,IlIlIl);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x64\x65\x76\x69\x63\x65\x5f\x64\x65\x73\x74\x72\x6f\x79\x28\x70\x64\x65\x76\x3d\x30\x78\x25\x70\x29" "\n"
,lllll);if(lllll->IIlII){IIllIlII(lllll->IIlII->IlllIl);llIllIII(lllll->IIlII);}
kfree(lllll);}
static inline size_t llIlIlIlI(struct urb*lIlII,void*lllII){int i;void*lIIIII;if
(!lIlII->transfer_buffer)return(0x35d+9102-0x26eb);lIIIII=lllII;for(i=
(0x170f+1294-0x1c1d);i<lIlII->number_of_packets;i++){memcpy(lIlII->
transfer_buffer+lIlII->iso_frame_desc[i].offset,lIIIII,lIlII->iso_frame_desc[i].
actual_length);lIIIII+=lIlII->iso_frame_desc[i].actual_length;}return(size_t)(
lIIIII-lllII);}
#if defined(IIlIlIll) && !defined(_USBD_VHCI_NO_DMA_)






int llIIIIIl(void*lllII,struct urb*lIlII,int IlIII){
if(IlIII==(0xfd5+3188-0x1c49))return(0x1993+2511-0x2362);if(lIIIlIl&&((lIlII->
transfer_buffer==NULL)||(lIlII->transfer_flags&IllIIlII))&&(lIlII->transfer_dma
!=(0xc0+9660-0x267c))&&(lIlII->transfer_dma!=~(dma_addr_t)(0x17d9+3410-0x252b)))
{llIlIlll(lllII,lIlII->transfer_dma,IlIII);}else if(lIlII->transfer_buffer){
memcpy(lllII,lIlII->transfer_buffer,IlIII);}else return-EINVAL;return IlIII;}





int lIlIIlII(struct urb*lIlII,void*lllII,int IlIII){
if(IlIII==(0x30+706-0x2f2))return(0x13a6+994-0x1788);if(lIIIlIl&&((lIlII->
transfer_buffer==NULL)||(lIlII->transfer_flags&IllIIlII))&&(lIlII->transfer_dma
!=(0x18f7+1174-0x1d8d))&&(lIlII->transfer_dma!=~(dma_addr_t)(0xd9f+1230-0x126d))
){IIIlIllI(lIlII->transfer_dma,lllII,IlIII);}else if(lIlII->transfer_buffer){
memcpy(lIlII->transfer_buffer,lllII,IlIII);}else return-EINVAL;return IlIII;}





int llIIlIIlI(void*lllII,struct urb*lIlII,int IlIII){
if(IlIII==(0x388+7909-0x226d))return(0x1273+3351-0x1f8a);
#if KERNEL_LT_EQ((0x2432+530-0x2642),(0x836+7386-0x250a),(0x1709+1676-0x1d73))
if(lIIIlIl&&((lIlII->setup_packet==NULL)||(lIlII->transfer_flags&llllIIlIl))&&(
lIlII->setup_dma!=(0x1668+3703-0x24df))&&(lIlII->setup_dma!=~(dma_addr_t)
(0x8b1+2326-0x11c7))){llIlIlll(lllII,lIlII->setup_dma,IlIII);}else
#endif
if(lIlII->setup_packet){memcpy(lllII,lIlII->setup_packet,IlIII);}else return-
EINVAL;return IlIII;}





int IlIlIIIlI(struct urb*lIlII,void*lllII,int IlIII){
if(IlIII==(0x9f+8405-0x2174))return(0x196b+2347-0x2296);
#if KERNEL_LT_EQ((0x521+5068-0x18eb),(0x302+5294-0x17aa),(0x639+3684-0x147b))
if(lIIIlIl&&((lIlII->setup_packet==NULL)||(lIlII->transfer_flags&llllIIlIl))&&(
lIlII->setup_dma!=(0x6b6+2085-0xedb))&&(lIlII->setup_dma!=~(dma_addr_t)
(0x146f+1571-0x1a92))){IIIlIllI(lIlII->setup_dma,lllII,IlIII);}else
#endif
if(lIlII->setup_packet){memcpy(lIlII->setup_packet,lllII,IlIII);}else return-
EINVAL;return IlIII;}


static inline size_t IIlllIIl(void*lllII,struct urb*lIlII){if(lIIIlIl&&((lIlII->
transfer_buffer==NULL)||(lIlII->transfer_flags&IllIIlII))&&(lIlII->transfer_dma
!=(0xa90+112-0xb00))&&(lIlII->transfer_dma!=~(dma_addr_t)(0x513+622-0x781))){int
 i;void*IIIIlI=lllII;dma_addr_t lIIIII;void*llIlII;unsigned long flags;unsigned 
long IIlIII,IIIIII;unsigned long IllIIlI,llIlIlI,lIIlllI,lIlllll;lIIlllI=
(0x803+5498-0x1d7d);llIlII=NULL;local_irq_save(flags);for(i=(0xad8+5980-0x2234);
i<lIlII->number_of_packets;i++){IIIIII=lIlII->iso_frame_desc[i].length;lIIIII=
lIlII->transfer_dma+lIlII->iso_frame_desc[i].offset;while(IIIIII){lIlllll=lIIIII
>>PAGE_SHIFT;if(lIlllll!=lIIlllI){if(lIIlllI)
#if KERNEL_GT_EQ((0x155+8160-0x2133),(0x63a+4268-0x16e0),(0x17c1+1513-0x1d85))
kunmap_atomic(llIlII);
#else
kunmap_atomic(llIlII,KM_IRQ0);
#endif
lIIlllI=lIlllll;
#if KERNEL_GT_EQ((0x3e8+2857-0xf0f),(0x33a+3729-0x11c5),(0x1a0c+1296-0x1ef7))
llIlII=kmap_atomic(pfn_to_page(lIlllll));
#else
llIlII=kmap_atomic(pfn_to_page(lIlllll),KM_IRQ0);
#endif
}IllIIlI=lIIIII&(PAGE_SIZE-(0x577+5876-0x1c6a));llIlIlI=PAGE_SIZE-IllIIlI;IIlIII
=(llIlIlI<IIIIII)?llIlIlI:IIIIII;memcpy(IIIIlI,llIlII+IllIIlI,IIlIII);lIIIII+=
IIlIII;IIIIlI+=IIlIII;IIIIII-=IIlIII;}}if(lIIlllI)
#if KERNEL_GT_EQ((0x1639+1903-0x1da6),(0x1d7d+2113-0x25b8),(0x1139+4799-0x23d3))
kunmap_atomic(llIlII);
#else
kunmap_atomic(llIlII,KM_IRQ0);
#endif
local_irq_restore(flags);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x70\x61\x63\x6b\x5f\x69\x73\x6f\x63\x68\x5f\x62\x75\x66\x3a\x20\x70\x61\x63\x6b\x65\x64\x3d\x25\x6c\x75" "\n"
,(unsigned long)(IIIIlI-lllII));return(size_t)(IIIIlI-lllII);}else if(lIlII->
transfer_buffer){return IllIIIII(lIlII->iso_frame_desc,lIlII->number_of_packets,
lllII,lIlII->transfer_buffer,(0xe4a+664-0x10e2));}IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x70\x61\x63\x6b\x5f\x69\x73\x6f\x63\x68\x5f\x62\x75\x66\x3a\x20\x6e\x6f\x20\x62\x75\x66\x66\x65\x72" "\n"
);return(0xa4b+1132-0xeb7);}static inline size_t IlllIIlll(struct urb*lIlII,void
*lllII){if(lIIIlIl&&((lIlII->transfer_buffer==NULL)||(lIlII->transfer_flags&
IllIIlII))&&(lIlII->transfer_dma!=(0x10ed+1861-0x1832))&&(lIlII->transfer_dma!=~
(dma_addr_t)(0x221c+1265-0x270d))){int i;void*lIIIII=lllII;dma_addr_t IIIIlI;
void*llIlII;unsigned long flags;unsigned long IIlIII,IIIIII;unsigned long 
IllIIlI,llIlIlI,lIIlllI,lIlllll;lIIlllI=(0x978+2984-0x1520);llIlII=NULL;
local_irq_save(flags);for(i=(0x1f70+1003-0x235b);i<lIlII->number_of_packets;i++)
{IIIIII=lIlII->iso_frame_desc[i].actual_length;IIIIlI=lIlII->transfer_dma+lIlII
->iso_frame_desc[i].offset;while(IIIIII){lIlllll=IIIIlI>>PAGE_SHIFT;if(lIlllll!=
lIIlllI){if(lIIlllI)
#if KERNEL_GT_EQ((0xb59+244-0xc4b),(0x993+5139-0x1da0),(0x11b3+799-0x14ad))
kunmap_atomic(llIlII);
#else
kunmap_atomic(llIlII,KM_IRQ0);
#endif
lIIlllI=lIlllll;
#if KERNEL_GT_EQ((0x2e5+9030-0x2629),(0xe5a+2504-0x181c),(0x955+5969-0x2081))
llIlII=kmap_atomic(pfn_to_page(lIlllll));
#else
llIlII=kmap_atomic(pfn_to_page(lIlllll),KM_IRQ0);
#endif
}IllIIlI=IIIIlI&(PAGE_SIZE-(0xbf7+4147-0x1c29));llIlIlI=PAGE_SIZE-IllIIlI;IIlIII
=(llIlIlI<IIIIII)?llIlIlI:IIIIII;memcpy(llIlII+IllIIlI,lIIIII,IIlIII);lIIIII+=
IIlIII;IIIIlI+=IIlIII;IIIIII-=IIlIII;}}if(lIIlllI)
#if KERNEL_GT_EQ((0xe25+3271-0x1aea),(0x1255+2324-0x1b63),(0x26c+1278-0x745))
kunmap_atomic(llIlII);
#else
kunmap_atomic(llIlII,KM_IRQ0);
#endif
local_irq_restore(flags);IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x69\x73\x6f\x63\x68\x5f\x62\x75\x66\x3a\x20\x70\x61\x63\x6b\x65\x64\x3d\x25\x6c\x75" "\n"
,(unsigned long)(lIIIII-lllII));return(size_t)(lIIIII-lllII);}else if(lIlII->
transfer_buffer){return llIlIlIlI(lIlII,lllII);}IIIll(
"\x75\x73\x62\x64\x5f\x76\x68\x63\x69\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x69\x73\x6f\x63\x68\x5f\x62\x75\x66\x3a\x20\x6e\x6f\x20\x62\x75\x66\x66\x65\x72" "\n"
);return(0x5e7+4649-0x1810);}
#else 

static inline int lIlIIlII(struct urb*lIlII,void*lllII,int IlIII){if(!IlIII)
return(0x1744+1807-0x1e53);if(!lIlII->transfer_buffer)return-EINVAL;memcpy(lIlII
->transfer_buffer,lllII,IlIII);return IlIII;}static inline int llIIIIIl(void*
lllII,struct urb*lIlII,int IlIII){if(!IlIII)return(0xf62+3131-0x1b9d);if(!lIlII
->transfer_buffer)return-EINVAL;memcpy(lllII,lIlII->transfer_buffer,IlIII);
return IlIII;}static inline int IlIlIIIlI(struct urb*lIlII,void*lllII,int IlIII)
{if(!IlIII)return(0x4dc+5923-0x1bff);if(!lIlII->setup_packet)return-EINVAL;
memcpy(lIlII->setup_packet,lllII,IlIII);return IlIII;}static inline int 
llIIlIIlI(void*lllII,struct urb*lIlII,int IlIII){if(!IlIII)return
(0x936+2243-0x11f9);if(!lIlII->setup_packet)return-EINVAL;memcpy(lllII,lIlII->
setup_packet,IlIII);return IlIII;}static inline size_t IIlllIIl(void*lllII,
struct urb*lIlII){if(!lIlII->transfer_buffer)return(0x6b2+7543-0x2429);return 
IllIIIII(lIlII->iso_frame_desc,lIlII->number_of_packets,lllII,lIlII->
transfer_buffer,(0x6f0+6550-0x2086));}

static inline size_t IlllIIlll(struct urb*lIlII,void*lllII){return llIlIlIlI(
lIlII,lllII);}
#endif 




static inline int llllIIlII(struct list_head*lIllIIl,lIlIIlI IlIIII,void*IlIIl){
int lIlll=(0x36d+1413-0x8f2);int IlIIllI=sizeof(lIIIllIIl);llIII IIlIl=IlIIl;
struct lIllIll*lIlIll;IIlIl->IIIlI.IIIIll=IlIIII;IIlIl->IIIlI.lIlIl=IlIIllI;
IIlIl->IIIlI.lIlllI=IIIlllII;IIlIl->IIIlI.Status=(0x1648+387-0x17cb);IIlIl->
IIIlI.Context=(0x1c63+1762-0x2345);lIlIll=list_entry(lIllIIl->prev,struct 
lIllIll,llIIll);IIlIl->llIIl.llIlI=(0x1756+3173-0x23bb);IIlIl->llIIl.Endpoint=
usb_pipeendpoint(lIlIll->lIlII->pipe);IIlIl->llIIl.Flags=usb_pipein(lIlIll->
lIlII->pipe)?llllII:(0x2c+468-0x200);
if(usb_pipein(lIlIll->lIlII->pipe)&&!(lIlIll->lIlII->transfer_flags&
URB_SHORT_NOT_OK)){IIlIl->llIIl.Flags|=IIlIllI;}


list_for_each_entry(lIlIll,lIllIIl,llIIll){if(usb_pipeout(lIlIll->lIlII->pipe)){
if(llIIIIIl((char*)IlIIl+IIlIl->IIIlI.lIlIl,lIlIll->lIlII,lIlIll->lIlII->
transfer_buffer_length)<(0x1a44+691-0x1cf7))return-EINVAL;IIlIl->IIIlI.lIlIl+=
lIlIll->lIlII->transfer_buffer_length;}IIlIl->llIIl.llIlI+=lIlIll->lIlII->
transfer_buffer_length;}return lIlll;}static inline int lIIllIllI(struct urb*
lIlII,lIlIIlI IlIIII,void*IlIIl){int lIlll=(0x1352+3523-0x2115);int IlIIllI=
sizeof(lIIllllI);llIII IIlIl=IlIIl;IIlIl->IIIlI.IIIIll=IlIIII;IIlIl->IIIlI.lIlIl
=IlIIllI;IIlIl->IIIlI.lIlllI=lIllllII;IIlIl->IIIlI.Status=(0x11c0+5282-0x2662);
IIlIl->IIIlI.Context=(0xcb4+2410-0x161e);IIlIl->lIllIl.llIlI=lIlII->
transfer_buffer_length;IIlIl->lIllIl.Interval=lIlII->interval;IIlIl->lIllIl.
Endpoint=usb_pipeendpoint(lIlII->pipe);IIlIl->lIllIl.Flags=usb_pipein(lIlII->
pipe)?llllII:(0xf17+3629-0x1d44);IIlIl->lIllIl.Flags|=(lIlII->transfer_flags&
URB_SHORT_NOT_OK)?(0xd00+3056-0x18f0):IIlIllI;if(usb_pipeout(lIlII->pipe)){if(
llIIIIIl((char*)IlIIl+IlIIllI,lIlII,lIlII->transfer_buffer_length)<
(0x16e8+2450-0x207a))return-EINVAL;IIlIl->IIIlI.lIlIl+=lIlII->
transfer_buffer_length;}return lIlll;}static inline int lllIlIllI(struct urb*
lIlII,lIlIIlI IlIIII,void*IlIIl){int lIlll=(0x1589+2775-0x2060);llIII IIlIl=
IlIIl;int i;int IlIIllI=sizeof(lllIllII)-sizeof(llllIII)+sizeof(llllIII)*lIlII->
number_of_packets;IIlIl->IIIlI.IIIIll=IlIIII;IIlIl->IIIlI.lIlIl=IlIIllI;IIlIl->
IIIlI.lIlllI=IlIlIlI;IIlIl->IIIlI.Status=(0x1224+1539-0x1827);IIlIl->IIIlI.
Context=(0x168+7010-0x1cca);IIlIl->IIlll.Endpoint=usb_pipeendpoint(lIlII->pipe);
IIlIl->IIlll.Flags=usb_pipein(lIlII->pipe)?llllII:(0x1686+727-0x195d);IIlIl->
IIlll.Flags|=(lIlII->transfer_flags&URB_SHORT_NOT_OK)?(0x6b0+4813-0x197d):
IIlIllI;IIlIl->IIlll.Flags|=(lIlII->transfer_flags&URB_ISO_ASAP)?IIlIlIlI:
(0x1293+2553-0x1c8c);IIlIl->IIlll.llIlI=lIlII->transfer_buffer_length;IIlIl->
IIlll.Interval=lIlII->interval;IIlIl->IIlll.lIlllII=lIlII->start_frame;IIlIl->
IIlll.lIlIIII=(0x663+879-0x9d2);IIlIl->IIlll.llIlIll=lIlII->number_of_packets;
for(i=(0xaeb+2981-0x1690);i<lIlII->number_of_packets;i++){IIlIl->IIlll.IIlIlI[i]
.Offset=lIlII->iso_frame_desc[i].offset;IIlIl->IIlll.IIlIlI[i].Length=lIlII->
iso_frame_desc[i].length;IIlIl->IIlll.IIlIlI[i].Status=(0x26d+5068-0x1639);}if(
usb_pipeout(lIlII->pipe))IIlIl->IIIlI.lIlIl+=IIlllIIl((char*)IlIIl+IlIIllI,lIlII
);return lIlll;}static inline int llIlIlIIl(struct urb*lIlII,lIlIIlI IlIIII,
struct usb_ctrlrequest*lIIlIII,void*IlIIl){int lIlll=(0x1a4a+2807-0x2541);llIII 
IIlIl=IlIIl;int IlIIllI=sizeof(lllIlIII);IIlIl->IIIlI.IIIIll=IlIIII;IIlIl->IIIlI
.lIlIl=IlIIllI;IIlIl->IIIlI.lIlllI=IIlIllll;IIlIl->IIIlI.Status=
(0x186d+1165-0x1cfa);IIlIl->IIIlI.Context=(0x254+7832-0x20ec);IIlIl->llIIlI.
Endpoint=usb_pipeendpoint(lIlII->pipe);IIlIl->llIIlI.Flags=usb_pipein(lIlII->
pipe)?llllII:(0x1648+2249-0x1f11);if(usb_pipein(lIlII->pipe))IIlIl->llIIlI.Flags
|=(lIlII->transfer_flags&URB_SHORT_NOT_OK)?(0x4ca+3547-0x12a5):IIlIllI;IIlIl->
llIIlI.llIIlll=lIIlIII->bRequestType;IIlIl->llIIlI.lIllIlll=lIIlIII->bRequest;
IIlIl->llIIlI.lllIIIIl=le16_to_cpu(lIIlIII->wValue);IIlIl->llIIlI.IIllIllI=
le16_to_cpu(lIIlIII->wIndex);IIlIl->llIIlI.llIlI=lIlII->transfer_buffer_length;
if(usb_pipeout(lIlII->pipe)){if(llIIIIIl((char*)IlIIl+IlIIllI,lIlII,lIlII->
transfer_buffer_length)<(0x508+2013-0xce5))return-EINVAL;IIlIl->IIIlI.lIlIl+=
lIlII->transfer_buffer_length;}return lIlll;}static inline int IIIIIllll(struct 
urb*lIlII,lIlIIlI IlIIII,struct usb_ctrlrequest*lIIlIII,void*IlIIl,int IIIIIllI)
{int lIlll=(0x67a+2498-0x103c);llIII IIlIl=IlIIl;int IlIIllI=sizeof(IIIIllIII);
IIlIl->IIIlI.IIIIll=IlIIII;IIlIl->IIIlI.lIlIl=IlIIllI;IIlIl->IIIlI.lIlllI=
IIIIIllI?llIIIlII:llIIllIl;IIlIl->IIIlI.Status=(0x64d+4426-0x1797);IIlIl->IIIlI.
Context=(0x15fa+86-0x1650);IIlIl->lllllI.llIllIIl=le16_to_cpu(lIIlIII->wIndex);
IIlIl->lllllI.llIlI=lIlII->transfer_buffer_length;IIlIl->lllllI.lIIlIlII=(
le16_to_cpu(lIIlIII->wValue)>>(0x440+6002-0x1baa))&(0x435+4354-0x1438);IIlIl->
lllllI.llIIllll=le16_to_cpu(lIIlIII->wValue)&(0x200c+1501-0x24ea);switch(lIIlIII
->bRequestType&USB_TYPE_MASK){case USB_TYPE_STANDARD:IIlIl->lllllI.llIIlll=
lIlIIlIlI;break;case USB_TYPE_CLASS:IIlIl->lllllI.llIIlll=Illllllll;break;case 
USB_TYPE_VENDOR:IIlIl->lllllI.llIIlll=IlIIlIIIl;break;case USB_TYPE_RESERVED:
IIlIl->lllllI.llIIlll=lIlIlIlII;break;default:IIlIl->lllllI.llIIlll=
(0x1aef+1575-0x2116);}switch(lIIlIII->bRequestType&USB_RECIP_MASK){case 
USB_RECIP_DEVICE:IIlIl->lllllI.IIlllIlI=lllIlllII;break;case USB_RECIP_INTERFACE
:IIlIl->lllllI.IIlllIlI=IlIIlIllI;break;case USB_RECIP_ENDPOINT:IIlIl->lllllI.
IIlllIlI=llIlIllll;break;case USB_RECIP_OTHER:IIlIl->lllllI.IIlllIlI=llIIlllll;
break;default:IIlIl->lllllI.llIIlll=(0x7e3+7882-0x26ad);}if(!IIIIIllI){if(
llIIIIIl((char*)IlIIl+IlIIllI,lIlII,lIlII->transfer_buffer_length)<
(0x1e49+395-0x1fd4))return-EINVAL;IIlIl->IIIlI.lIlIl+=lIlII->
transfer_buffer_length;}return lIlll;}static inline int lIlIIIIII(struct urb*
lIlII,lIlIIlI IlIIII,struct usb_ctrlrequest*lIIlIII,void*IlIIl){int lIlll=
(0x320+5587-0x18f3);llIII IIlIl=IlIIl;int lIIIIIll=sizeof(lIIlIIIIl);IIlIl->
IIIlI.IIIIll=IlIIII;IIlIl->IIIlI.lIlIl=lIIIIIll;IIlIl->IIIlI.lIlllI=Illlllll;
IIlIl->IIIlI.Status=(0xc79+6328-0x2531);IIlIl->IIIlI.Context=(0xada+6873-0x25b3)
;IIlIl->IIIIIll.IIIlIIl=(lllIl)le16_to_cpu(lIIlIII->wIndex);IIlIl->IIIIIll.
lllIIIl=(lllIl)le16_to_cpu(lIIlIII->wValue);return lIlll;}static inline int 
IIlIlIlII(struct urb*lIlII,lIlIIlI IlIIII,struct usb_ctrlrequest*lIIlIII,void*
IlIIl){int lIlll=(0x81d+4064-0x17fd);llIII IIlIl=IlIIl;int lIIIIIll=sizeof(
IlIlIIlII);IIlIl->IIIlI.IIIIll=IlIIII;IIlIl->IIIlI.lIlIl=lIIIIIll;IIlIl->IIIlI.
lIlllI=lIIIlIlI;IIlIl->IIIlI.Status=(0x1416+3292-0x20f2);IIlIl->IIIlI.Context=
(0x1653+2136-0x1eab);IIlIl->IIIIIII.llIlIIIl=(lllIl)le16_to_cpu(lIIlIII->wValue)
;IIlIl->IIIIIII.llllIIII=(0x155d+217-0x1636);return lIlll;}static inline int 
llIIlIllI(struct urb*lIlII,lIlIIlI IlIIII,struct usb_ctrlrequest*lIIlIII,void*
IlIIl){int lIlll=(0x36c+2985-0xf15);llIII IIlIl=IlIIl;int lIIIIIll=sizeof(
IIIlllIll);IIlIl->IIIlI.IIIIll=IlIIII;IIlIl->IIIlI.lIlIl=lIIIIIll;IIlIl->IIIlI.
lIlllI=IlllIIIl;IIlIl->IIIlI.Status=(0x758+6266-0x1fd2);IIlIl->IIIlI.Context=
(0x1573+4147-0x25a6);IIlIl->IlIIIII.Endpoint=lIIlIII->wIndex&
(0x2095+1171-0x2519);
IIlIl->IlIIIII.Flags=(lIIlIII->wIndex&USB_DIR_IN)?llllII:(0x1c17+2197-0x24ac);
return lIlll;}int lllIllIl(struct list_head*lIllIIl,lIlIIlI IlIIII,void*IlIIl){
int lIlll=-EINVAL;do{struct usb_ctrlrequest Illlll;struct urb*lIlII;struct 
lIllIll*lIlIll;lIlIll=list_entry(lIllIIl->next,struct lIllIll,llIIll);lIlII=
lIlIll->lIlII;if(!lIlII){IIIll(
"\x75\x73\x62\x64\x5f\x70\x61\x63\x6b\x5f\x75\x72\x62\x5f\x6c\x69\x73\x74\x3a\x20\x70\x75\x72\x62\x20\x69\x73\x20\x6e\x75\x6c\x6c\x28\x30\x78\x25\x70\x29\x2c\x20\x70\x76\x75\x72\x62\x3d\x30\x78\x25\x70\x20\x70\x75\x72\x62\x5f\x6c\x69\x73\x74\x3d\x30\x78\x25\x70" "\n"
,lIlII,lIlIll,lIllIIl);break;}switch(usb_pipetype(lIlII->pipe)){case PIPE_BULK:
lIlll=llllIIlII(lIllIIl,IlIIII,IlIIl);break;case PIPE_INTERRUPT:lIlll=lIIllIllI(
lIlII,IlIIII,IlIIl);break;case PIPE_ISOCHRONOUS:lIlll=lllIlIllI(lIlII,IlIIII,
IlIIl);break;case PIPE_CONTROL:if(llIIlIIlI(&Illlll,lIlII,sizeof(Illlll))<
(0xcd5+2424-0x164d))break;if(Illlll.bRequestType==(USB_DIR_IN|USB_TYPE_STANDARD|
USB_RECIP_DEVICE)&&Illlll.bRequest==USB_REQ_GET_DESCRIPTOR){
lIlll=IIIIIllll(lIlII,IlIIII,&Illlll,IlIIl,(0xe65+3259-0x1b1f));}else if(Illlll.
bRequestType==(USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_DEVICE)&&Illlll.bRequest
==USB_REQ_SET_DESCRIPTOR){
lIlll=IIIIIllll(lIlII,IlIIII,&Illlll,IlIIl,(0x731+7597-0x24de));}else if(Illlll.
bRequestType==(USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_DEVICE)&&Illlll.bRequest
==USB_REQ_SET_CONFIGURATION){
lIlll=IIlIlIlII(lIlII,IlIIII,&Illlll,IlIIl);}else if(Illlll.bRequestType==(
USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_INTERFACE)&&Illlll.bRequest==
USB_REQ_SET_INTERFACE){
lIlll=lIlIIIIII(lIlII,IlIIII,&Illlll,IlIIl);}else if(Illlll.bRequestType==(
USB_DIR_OUT|USB_TYPE_STANDARD|USB_RECIP_ENDPOINT)&&Illlll.bRequest==
USB_REQ_CLEAR_FEATURE&&Illlll.wValue==USB_ENDPOINT_HALT){
lIlll=llIIlIllI(lIlII,IlIIII,&Illlll,IlIIl);}else{
lIlll=llIlIlIIl(lIlII,IlIIII,&Illlll,IlIIl);}break;default:IIIIlll(
"\x55\x6e\x6b\x6e\x6f\x77\x6e\x20\x70\x69\x70\x65\x20\x74\x79\x70\x65\x20\x25\x64\x20\x69\x6e\x20\x75\x72\x62" "\n"
,usb_pipetype(lIlII->pipe));break;}}while((0x1b0+687-0x45f));return lIlll;}



static inline int IIIlIlII(llIII IIlIl,struct urb*lIlII,int IIIIIllI){int lIlll=
(0x1dc1+543-0x1fe0);lIlII->actual_length=min(lIlII->transfer_buffer_length,IIlIl
->lllllI.llIlI);if(IIIIIllI){lIlIIlII(lIlII,(char*)IIlIl+sizeof(IIlIl->lllllI),
lIlII->actual_length);}return lIlll;}static inline int lIllIIIII(llIII IIlIl,
struct list_head*lIllIIl){int lIlll=(0x1ca+6928-0x1cda);struct lIllIll*lIlIll;
unsigned long IIIIIlII;unsigned char*lllII;IIIIIlII=IIlIl->llIIl.llIlI;lllII=(
unsigned char*)IIlIl+sizeof(IIlIl->llIIl);


list_for_each_entry(lIlIll,lIllIIl,llIIll){lIlIll->lIlII->actual_length=min((
unsigned long)lIlIll->lIlII->transfer_buffer_length,IIIIIlII);if(usb_pipein(
lIlIll->lIlII->pipe)){IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x5f\x62\x75\x6c\x6b\x3a\x20\x63\x6f\x70\x79\x20\x25\x64\x20\x62\x79\x74\x65\x73\x20\x74\x6f\x20\x75\x72\x62\x20\x25\x70" "\n"
,lIlIll->lIlII->actual_length,lIlIll->lIlII);lIlIIlII(lIlIll->lIlII,lllII,lIlIll
->lIlII->actual_length);}IIIIIlII-=lIlIll->lIlII->actual_length;lllII+=lIlIll->
lIlII->actual_length;}return lIlll;}static inline int IllIIIlII(llIII IIlIl,
struct urb*lIlII){int i;int lIlll=(0xa1b+6141-0x2218);lllIl*lllllIIlI=(lllIl*)
IIlIl+IlIlIIl(IIlIl);if(IIlIl->IIlll.llIlIll!=lIlII->number_of_packets)return 
lIlll;lIlII->start_frame=IIlIl->IIlll.lIlllII;lIlII->error_count=IIlIl->IIlll.
lIlIIII;lIlII->actual_length=(0x19d8+1253-0x1ebd);for(i=(0x103a+4476-0x21b6);i<
lIlII->number_of_packets;i++){lIlII->iso_frame_desc[i].status=IlIIlIIl(IIlIl->
IIlll.IIlIlI[i].Status);lIlII->iso_frame_desc[i].actual_length=IIlIl->IIlll.
IIlIlI[i].Length;lIlII->actual_length+=lIlII->iso_frame_desc[i].actual_length;}
if(usb_pipein(lIlII->pipe))IlllIIlll(lIlII,lllllIIlI);return lIlll;}static 
inline int lIIIlIIll(llIII IIlIl,struct urb*lIlII){int lIlll=(0x838+2969-0x13d1)
;lIlII->actual_length=min(lIlII->transfer_buffer_length,IIlIl->llIIlI.llIlI);if(
usb_pipein(lIlII->pipe)){lIlIIlII(lIlII,(char*)IIlIl+sizeof(IIlIl->llIIlI),lIlII
->actual_length);}return lIlll;}static inline int IlIlllIll(llIII IIlIl,struct 
urb*lIlII){int lIlll=(0x16f+9497-0x2688);lIlII->actual_length=min(lIlII->
transfer_buffer_length,IIlIl->lIllIl.llIlI);if(usb_pipein(lIlII->pipe)){lIlIIlII
(lIlII,(char*)IIlIl+sizeof(IIlIl->lIllIl),lIlII->actual_length);}return lIlll;}
int llIllllII(llIII IIlIl,struct list_head*lIllIIl){int status=-
(0x5c1+1975-0xd77);struct lIllIll*lIlIll=list_entry(lIllIIl->next,struct lIllIll
,llIIll);struct urb*lIlII=lIlIll->lIlII;status=IlIIlIIl(IIlIl->IIIlI.Status);
switch(IIlIl->IIIlI.lIlllI){case llIIIlII:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x47\x65\x74\x44\x65\x73\x63\x72\x69\x70\x74\x6f\x72" "\n"
);IIIlIlII(IIlIl,lIlII,(0x11ab+2770-0x1c7c));break;case llIIllIl:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x53\x65\x74\x44\x65\x73\x63\x72\x69\x70\x74\x6f\x72" "\n"
);IIIlIlII(IIlIl,lIlII,(0x1c79+1057-0x209a));break;case lIIIlIlI:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x53\x65\x6c\x65\x63\x74\x43\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e" "\n"
);break;case Illlllll:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x53\x65\x6c\x65\x63\x74\x49\x6e\x74\x65\x72\x66\x61\x63\x65" "\n"
);break;case IIlIllll:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x43\x6f\x6e\x74\x72\x6f\x6c\x54\x72\x61\x6e\x73\x66\x65\x72" "\n"
);lIIIlIIll(IIlIl,lIlII);break;case IIIlllII:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x42\x75\x6c\x6b\x54\x72\x61\x6e\x73\x66\x65\x72" "\n"
);lIllIIIII(IIlIl,lIllIIl);break;case IlIlIlI:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x49\x73\x6f\x63\x68\x54\x72\x61\x6e\x73\x66\x65\x72" "\n"
);IllIIIlII(IIlIl,lIlII);break;case lIllllII:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x49\x6e\x74\x65\x72\x72\x75\x70\x74\x54\x72\x61\x6e\x73\x66\x65\x72" "\n"
);IlIlllIll(IIlIl,lIlII);break;case IlllIIIl:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x43\x6c\x65\x61\x72\x53\x74\x61\x6c\x6c" "\n"
);break;default:IIIll(
"\x75\x73\x62\x64\x5f\x75\x6e\x70\x61\x63\x6b\x5f\x75\x72\x62\x3a\x20\x75\x6e\x6b\x6e\x6f\x77\x6e\x20\x66\x75\x6e\x63\x74\x69\x6f\x6e" "\n"
);status=-EINVAL;break;}
#if KERNEL_LT((0xf48+1671-0x15cd),(0x15fd+1160-0x1a7f),(0xdc2+2990-0x1958))
lIlII->status=status;
#endif
return status;}
























































int IIIlllI(struct IIlllI*Illll,int IlllI,int IllllI,int IIlIIll){
switch(IllllI){case IIllllII:case IIIlIIIl:case llllIllI:lIlIlI(Illll,IlllI,
lIIlIll);break;case IllIlll:lIlIlI(Illll,IlllI,lIIlIll);break;case IlIllIll:
Illll->lIIIIl[IlllI]&=~USB_PORT_STAT_C_CONNECTION;break;case lIlIIIIlI:Illll->
lIIIIl[IlllI]&=~USB_PORT_STAT_C_ENABLE;break;case lIIlIIlI:Illll->lIIIIl[IlllI]
&=~USB_PORT_STAT_C_SUSPEND;break;case llllllIl:Illll->lIIIIl[IlllI]&=~
USB_PORT_STAT_C_RESET;break;}
switch(Illll->llIllIl[IlllI]){case lIIlIll:if(IllllI==IlIIllIl)lIlIlI(Illll,
IlllI,IIlllII);break;case IIlllII:if(IllllI==llIlllIl)lIlIlI(Illll,IlllI,IllIllI
);break;case IllIllI:if(IllllI==IIllIlI)lIlIlI(Illll,IlllI,IIlllII);if(IllllI==
llIIIll)lIlIlI(Illll,IlllI,IIlIlII);break;case IIlIlII:if(IllllI==IIllIlI)lIlIlI
(Illll,IlllI,IIlllII);if(IllllI==IlllIllI)lIlIlI(Illll,IlllI,IllIllI);if(IllllI
==llIIlIII)lIlIlI(Illll,IlllI,IlIlllI);break;case IlIlllI:if(IllllI==IIllIlI)
lIlIlI(Illll,IlllI,IIlllII);if(IllllI==IlllIllI)lIlIlI(Illll,IlllI,IllIllI);if(
IllllI==llIIIll)lIlIlI(Illll,IlllI,IIlIlII);if(IllllI==lIIlIIll)lIlIlI(Illll,
IlllI,IllIlllI);break;case IllIlllI:if(IllllI==IIllIlI)lIlIlI(Illll,IlllI,
IIlllII);if(IllllI==IlllIllI)lIlIlI(Illll,IlllI,IllIllI);if(IllllI==llIIIll)
lIlIlI(Illll,IlllI,IIlIlII);if(IllllI==llIIIIIII||IllllI==IlllIIlIl)lIlIlI(Illll
,IlllI,lllIIllI);break;case lllIIllI:if(IllllI==IIllIlI)lIlIlI(Illll,IlllI,
IIlllII);if(IllllI==IlllIllI)lIlIlI(Illll,IlllI,IllIllI);if(IllllI==llIIIll)
lIlIlI(Illll,IlllI,IIlIlII);if(IllllI==llIIlIII)lIlIlI(Illll,IlllI,IlIlllI);
break;}return(0xcc8+590-0xf15);}static inline int IlIIIllI(int speed){switch(
speed){case USB_SPEED_HIGH:return USB_PORT_STAT_HIGH_SPEED;case USB_SPEED_LOW:
return USB_PORT_STAT_LOW_SPEED;case USB_SPEED_FULL:return(0xc92+6466-0x25d4);}
return(0xcb5+2051-0x14b8);}
void lIlIlI(struct IIlllI*Illll,int IlllI,int lIIIlll){
int lllIIII=Illll->llIllIl[IlllI];if(lIIIlll==lllIIII)return;Illll->llIllIl[
IlllI]=lIIIlll;Illll->lIlIlII[IlllI]=(0x92d+3157-0x1582);if(lllIIII==IIlIlII)
Illll->lIIIIl[IlllI]|=USB_PORT_STAT_C_RESET;if(lllIIII==lllIIllI)Illll->lIIIIl[
IlllI]|=USB_PORT_STAT_C_SUSPEND;switch(lIIIlll){case lIIlIll:Illll->IIIIIIl[
IlllI]=(0x1338+1654-0x19ae);Illll->lIIIIl[IlllI]=(0x9d0+7486-0x270e);
break;case IIlllII:Illll->IIIIIIl[IlllI]=USB_PORT_STAT_POWER;if(lllIIII!=lIIlIll
)Illll->lIIIIl[IlllI]|=USB_PORT_STAT_C_CONNECTION;
if(Illll->IlIIll[IlllI]){lIlIlI(Illll,IlllI,IllIllI);}break;case IllIllI:Illll->
IIIIIIl[IlllI]=USB_PORT_STAT_POWER|USB_PORT_STAT_CONNECTION;if(lllIIII==IIlllII)
Illll->lIIIIl[IlllI]|=USB_PORT_STAT_C_CONNECTION;break;case IIlIlII:Illll->
IIIIIIl[IlllI]=USB_PORT_STAT_POWER|USB_PORT_STAT_CONNECTION|USB_PORT_STAT_RESET;
Illll->lIlIlII[IlllI]=jiffies+msecs_to_jiffies((0x616+1118-0xa42));
Illll->lIlIlII[IlllI]+=Illll->lIlIlII[IlllI]?(0xda1+6414-0x26af):
(0xf25+2572-0x1930);break;case IlIlllI:Illll->IIIIIIl[IlllI]=USB_PORT_STAT_POWER
|USB_PORT_STAT_CONNECTION|USB_PORT_STAT_ENABLE|IlIIIllI(Illll->IlIIll[IlllI]->
speed);break;case IllIlllI:Illll->IIIIIIl[IlllI]=USB_PORT_STAT_POWER|
USB_PORT_STAT_CONNECTION|USB_PORT_STAT_ENABLE|USB_PORT_STAT_SUSPEND|IlIIIllI(
Illll->IlIIll[IlllI]->speed);break;case lllIIllI:Illll->IIIIIIl[IlllI]=
USB_PORT_STAT_POWER|USB_PORT_STAT_CONNECTION|USB_PORT_STAT_ENABLE|
USB_PORT_STAT_SUSPEND|IlIIIllI(Illll->IlIIll[IlllI]->speed);
Illll->lIlIlII[IlllI]=jiffies+msecs_to_jiffies((0xdaf+4372-0x1eaf));
Illll->lIlIlII[IlllI]+=Illll->lIlIlII[IlllI]?(0x189+2885-0xcce):
(0xed4+5887-0x25d2);break;default:IIIIlll(
"\x69\x6e\x76\x61\x6c\x69\x64\x20\x6e\x65\x77\x20\x70\x6f\x72\x74\x20\x73\x74\x61\x74\x65" "\n"
);break;}}
#if KERNEL_GT_EQ((0x20f+9311-0x266c),(0x183a+1221-0x1cf9),(0x1ecd+1880-0x25fe)) \
|| RHEL_RELEASE_GT_EQ((0x1167+3974-0x20e7),(0x84+4078-0x106f)) 

























int IlIIlII(struct IIlllI*Illll,int IlllI,int IllllI,int IIlIIll){
switch(IllllI){case IIllllII:case llllIllI:lllllIl(Illll,IlllI,lIIlIll);return
(0xd09+5836-0x23d4);case IlIllIll:Illll->lIIIIl[IlllI]&=~
USB_PORT_STAT_C_CONNECTION;return(0x13bc+1633-0x1a1c);case llllllIl:Illll->
lIIIIl[IlllI]&=~USB_PORT_STAT_C_RESET;return(0x782+6876-0x225d);case lIlllIll:
Illll->lIIIIl[IlllI]&=~USB_PORT_STAT_C_BH_RESET;return(0x20f+8278-0x2264);case 
IlIlIlIlI:Illll->lIIIIl[IlllI]&=~USB_PORT_STAT_C_LINK_STATE;return
(0x1b2d+1461-0x20e1);case lllIlll:if(IIlIIll==(0x19ff+1344-0x1f3b)){
lllllIl(Illll,IlllI,IllIllI);return(0x150d+3732-0x23a0);}break;}
switch(Illll->llIllIl[IlllI]){case lIIlIll:if(IllllI==IIIlIIIl)lllllIl(Illll,
IlllI,IIlllII);break;case lIllIIIl:if(IllllI==IlIIllIl)lllllIl(Illll,IlllI,
IIlllII);if(IllllI==llIIIll)lllllIl(Illll,IlllI,IIlIlII);break;case IIlllII:if(
IllllI==IllIlll)lllllIl(Illll,IlllI,lIllIIIl);if(IllllI==llIlllIl)lllllIl(Illll,
IlllI,IlIlllI);break;case IllIllI:if(IllllI==IllIlll)lllllIl(Illll,IlllI,
lIllIIIl);if(IllllI==llIIIll)lllllIl(Illll,IlllI,IIlIlII);if(IllllI==lllIlll&&
IIlIIll==(0x7f8+7931-0x26ee)){


lllllIl(Illll,IlllI,IIlllII);}break;case IlIlllI:if(IllllI==IllIlll)lllllIl(
Illll,IlllI,lIllIIIl);if(IllllI==IIllIlI)lllllIl(Illll,IlllI,IIlllII);if(IllllI
==llIIIll)lllllIl(Illll,IlllI,IIlIlII);if(IllllI==lllIlll&&IIlIIll==
(0x1239+2247-0x1b00)){
if(Illll->llIlll[IlllI]==USB_SS_PORT_LS_U3){

Illll->lIIIIl[IlllI]|=USB_PORT_STAT_C_LINK_STATE;}IlIIlIl(Illll,IlllI,
USB_SS_PORT_LS_U0);}if(IllllI==lllIlll&&IIlIIll==(0x28a+3843-0x118c)){

if(Illll->llIlll[IlllI]==USB_SS_PORT_LS_U0){IlIIlIl(Illll,IlllI,
USB_SS_PORT_LS_U1);}}if(IllllI==lllIlll&&IIlIIll==(0xd29+468-0xefb)){

if(Illll->llIlll[IlllI]==USB_SS_PORT_LS_U0){IlIIlIl(Illll,IlllI,
USB_SS_PORT_LS_U2);}}if(IllllI==lllIlll&&IIlIIll==(0x4bc+6388-0x1dad)){

if(Illll->llIlll[IlllI]==USB_SS_PORT_LS_U0||Illll->llIlll[IlllI]==
USB_SS_PORT_LS_U1||Illll->llIlll[IlllI]==USB_SS_PORT_LS_U2){

if(Illll->llIlll[IlllI]!=USB_SS_PORT_LS_U0){IlIIlIl(Illll,IlllI,
USB_SS_PORT_LS_U0);}IlIIlIl(Illll,IlllI,USB_SS_PORT_LS_U3);}}break;case IIlIlII:
if(IllllI==IllIlll)lllllIl(Illll,IlllI,lIllIIIl);if(IllllI==IIllIlI)lllllIl(
Illll,IlllI,IIlllII);if(IllllI==llIIlIII)lllllIl(Illll,IlllI,IlIlllI);break;}
return(0xd3b+1318-0x1260);}static inline int IlIlIllII(int speed){switch(speed){
case USB_SPEED_SUPER:return USB_PORT_STAT_SPEED_5GBPS;default:return
(0xc04+5971-0x2357);}return(0x131d+4074-0x2307);}
void lllllIl(struct IIlllI*Illll,int IlllI,int lIIIlll){int lIIIIIlI=Illll->
IIIIIIl[IlllI];int lllIIII=Illll->llIllIl[IlllI];if(lIIIlll==lllIIII)return;
Illll->llIllIl[IlllI]=lIIIlll;Illll->lIlIlII[IlllI]=(0x118+8432-0x2208);switch(
lIIIlll){case lIIlIll:IlIIlIl(Illll,IlllI,USB_SS_PORT_LS_SS_DISABLED);Illll->
IIIIIIl[IlllI]=Illll->llIlll[IlllI];Illll->lIIIIl[IlllI]=(0x1d0a+647-0x1f91);
break;case lIllIIIl:IlIIlIl(Illll,IlllI,USB_SS_PORT_LS_RX_DETECT);Illll->IIIIIIl
[IlllI]=Illll->llIlll[IlllI];Illll->lIIIIl[IlllI]=(0x14ab+1099-0x18f6);
break;case IllIllI:IlIIlIl(Illll,IlllI,USB_SS_PORT_LS_SS_DISABLED);Illll->
IIIIIIl[IlllI]=Illll->llIlll[IlllI]|USB_SS_PORT_STAT_POWER;if(lIIIIIlI&
USB_PORT_STAT_CONNECTION)Illll->lIIIIl[IlllI]|=USB_PORT_STAT_C_CONNECTION;
break;case IIlllII:IlIIlIl(Illll,IlllI,USB_SS_PORT_LS_RX_DETECT);Illll->IIIIIIl[
IlllI]=Illll->llIlll[IlllI]|USB_SS_PORT_STAT_POWER;if(lIIIIIlI&
USB_PORT_STAT_CONNECTION)Illll->lIIIIl[IlllI]|=USB_PORT_STAT_C_CONNECTION;

if(Illll->IlIIll[IlllI]){lllllIl(Illll,IlllI,IlIlllI);}break;case IlIlllI:
IlIIlIl(Illll,IlllI,USB_SS_PORT_LS_U0);Illll->IIIIIIl[IlllI]=Illll->llIlll[IlllI
]|USB_SS_PORT_STAT_POWER|USB_PORT_STAT_CONNECTION|USB_PORT_STAT_ENABLE|IlIlIllII
(Illll->IlIIll[IlllI]->speed);if(lllIIII==IIlIlII)Illll->lIIIIl[IlllI]|=
USB_PORT_STAT_C_RESET;
if(lllIIII==IIlllII)Illll->lIIIIl[IlllI]|=USB_PORT_STAT_C_CONNECTION;
break;case IIlIlII:Illll->IIIIIIl[IlllI]=Illll->llIlll[IlllI]|
USB_SS_PORT_STAT_POWER|USB_PORT_STAT_RESET|(lIIIIIlI&USB_PORT_STAT_CONNECTION);

Illll->lIlIlII[IlllI]=jiffies+msecs_to_jiffies((0x17ab+829-0x1ab6));
Illll->lIlIlII[IlllI]+=Illll->lIlIlII[IlllI]?(0x16c5+863-0x1a24):
(0x82f+434-0x9e0);break;default:IIIll(
"\x69\x6e\x76\x61\x6c\x69\x64\x20\x70\x6f\x72\x74\x20\x73\x74\x61\x74\x65" "\n")
;break;}}
void IlIIlIl(struct IIlllI*Illll,int IlllI,int lIllIIll){switch(lIllIIll){case 
USB_SS_PORT_LS_U0:IIIll(
"\x73\x65\x74\x5f\x6c\x69\x6e\x6b\x5f\x73\x74\x61\x74\x65\x20\x55\x53\x42\x5f\x53\x53\x5f\x50\x4f\x52\x54\x5f\x4c\x53\x5f\x55\x30" "\n"
);
Illll->llIlll[IlllI]=USB_SS_PORT_LS_U0;break;case USB_SS_PORT_LS_U1:IIIll(
"\x73\x65\x74\x5f\x6c\x69\x6e\x6b\x5f\x73\x74\x61\x74\x65\x20\x55\x53\x42\x5f\x53\x53\x5f\x50\x4f\x52\x54\x5f\x4c\x53\x5f\x55\x31" "\n"
);
Illll->llIlll[IlllI]=USB_SS_PORT_LS_U1;break;case USB_SS_PORT_LS_U2:IIIll(
"\x73\x65\x74\x5f\x6c\x69\x6e\x6b\x5f\x73\x74\x61\x74\x65\x20\x55\x53\x42\x5f\x53\x53\x5f\x50\x4f\x52\x54\x5f\x4c\x53\x5f\x55\x32" "\n"
);
Illll->llIlll[IlllI]=USB_SS_PORT_LS_U2;break;case USB_SS_PORT_LS_U3:IIIll(
"\x73\x65\x74\x5f\x6c\x69\x6e\x6b\x5f\x73\x74\x61\x74\x65\x20\x55\x53\x42\x5f\x53\x53\x5f\x50\x4f\x52\x54\x5f\x4c\x53\x5f\x55\x33" "\n"
);
Illll->llIlll[IlllI]=USB_SS_PORT_LS_U3;break;case USB_SS_PORT_LS_SS_DISABLED:
IIIll(
"\x73\x65\x74\x5f\x6c\x69\x6e\x6b\x5f\x73\x74\x61\x74\x65\x20\x55\x53\x42\x5f\x53\x53\x5f\x50\x4f\x52\x54\x5f\x4c\x53\x5f\x53\x53\x5f\x44\x49\x53\x41\x42\x4c\x45\x44" "\n"
);
Illll->llIlll[IlllI]=USB_SS_PORT_LS_SS_DISABLED;break;case 
USB_SS_PORT_LS_RX_DETECT:IIIll(
"\x73\x65\x74\x5f\x6c\x69\x6e\x6b\x5f\x73\x74\x61\x74\x65\x20\x55\x53\x42\x5f\x53\x53\x5f\x50\x4f\x52\x54\x5f\x4c\x53\x5f\x52\x58\x5f\x44\x45\x54\x45\x43\x54" "\n"
);
Illll->llIlll[IlllI]=USB_SS_PORT_LS_RX_DETECT;break;}Illll->IIIIIIl[IlllI]&=~
USB_PORT_STAT_LINK_STATE;Illll->IIIIIIl[IlllI]|=Illll->llIlll[IlllI];}
#endif 
#endif 

