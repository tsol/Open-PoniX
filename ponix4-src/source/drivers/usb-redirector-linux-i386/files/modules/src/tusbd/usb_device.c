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
#include "../public/filter.h"
#if KERNEL_LT_EQ((0x1223+3476-0x1fb5),(0x328+1161-0x7ab),(0x4f8+7327-0x2171))
#include <linux/smp_lock.h>
#endif
int lIIIIIII=(0x92a+1668-0xfae);static struct list_head IlllllI;static 
spinlock_t lIllII;

static struct usb_device_id IlllIIIll[]={
{.driver_info=(0x12d6+335-0x1424)},{}};MODULE_DEVICE_TABLE(usb,IlllIIIll);static
 int IllllIlI(struct usb_interface*lIIllII,const struct usb_device_id*id);static
 void lIlIIIlI(struct usb_interface*lIIllII);
#if KERNEL_GT_EQ((0x7bd+3739-0x1656),(0xb37+6463-0x2470),(0x8c1+1520-0xe9a))
static int IIlIlIIl(struct usb_interface*IIIllI);static int IlIlIlll(struct 
usb_interface*IIIllI);
#elif KERNEL_GT_EQ((0x9e9+6772-0x245b),(0x5c+4703-0x12b5),(0x8c5+4340-0x19a7))
static void IIlIlIIl(struct usb_interface*IIIllI);static void IlIlIlll(struct 
usb_interface*IIIllI);
#endif
void lIIlIl(struct kref*IlllIIl);void IIlIlllI(struct IllII*IlIlI);void llIlIlIl
(struct IllII*IlIlI);
#if KERNEL_GT_EQ((0x445+7175-0x204a),(0x12c9+2263-0x1b9a),(0x146c+382-0x15d6))
int llIlllIII(struct notifier_block*self,unsigned long IlIIIlIIl,void*dev);
struct notifier_block lIIlIlIlI={.notifier_call=llIlllIII,};
#endif
struct usb_driver IllIIIll={.name=llllIlII,.id_table=IlllIIIll,.probe=IllllIlI,.
disconnect=lIlIIIlI,
#if KERNEL_GT_EQ((0x1588+696-0x183e),(0x1fe6+1666-0x2662),(0x1b4+3129-0xddb))
.pre_reset=IIlIlIIl,.post_reset=IlIlIlll,
#endif
};void llIlIIIll(struct usb_device*lIlIIl,u8*lIlIIlIl,u8*IllIllIl,u8*IIIlIIll){
int i,interface=-(0x114f+1608-0x1796);if(lIlIIlIl)*lIlIIlIl=(0x189b+1924-0x201f)
;if(IllIllIl)*IllIllIl=(0x13c0+1141-0x1835);if(IIIlIIll)*IIIlIIll=
(0xf29+1143-0x13a0);














if(lIlIIl->actconfig&&lIlIIl->actconfig->desc.bNumInterfaces>(0x34f+4039-0x1315)
){

if(lIlIIl->descriptor.bDeviceClass==(0x454+5237-0x18c9))return;
if((lIlIIl->descriptor.bDeviceClass==(0x1382+4151-0x22ca))&&(lIlIIl->descriptor.
bDeviceSubClass==(0x22a0+603-0x24f9))&&(lIlIIl->descriptor.bDeviceProtocol==
(0x845+6917-0x2349)))return;
}


if(lIlIIlIl)*lIlIIlIl=lIlIIl->descriptor.bDeviceClass;if(IllIllIl)*IllIllIl=
lIlIIl->descriptor.bDeviceSubClass;if(IIIlIIll)*IIIlIIll=lIlIIl->descriptor.
bDeviceProtocol;

if(lIlIIl->descriptor.bDeviceClass)return;


if(!lIlIIl->actconfig)return;

for(i=(0xdc+9720-0x26d4);i<USB_MAXINTERFACES;i++){if(lIlIIl->actconfig->
interface[i]&&lIlIIl->actconfig->interface[i]->cur_altsetting){if(interface<
(0x305+7877-0x21ca)||lIlIIl->actconfig->interface[i]->cur_altsetting->desc.
bInterfaceNumber<lIlIIl->actconfig->interface[interface]->cur_altsetting->desc.
bInterfaceNumber){interface=i;}}}if(interface>=(0x1c03+1867-0x234e)){if(lIlIIlIl
)*lIlIIlIl=lIlIIl->actconfig->interface[interface]->cur_altsetting->desc.
bInterfaceClass;if(IllIllIl)*IllIllIl=lIlIIl->actconfig->interface[interface]->
cur_altsetting->desc.bInterfaceSubClass;if(IIIlIIll)*IIIlIIll=lIlIIl->actconfig
->interface[interface]->cur_altsetting->desc.bInterfaceProtocol;}}int IllllIlI(
struct usb_interface*lIIllII,const struct usb_device_id*id){int lIlll=-ENODEV;
IIIll("\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x2b\x2b" "\n");do{struct
 usb_device*lIlIIl=interface_to_usbdev(lIIllII);struct IllII*IlIlI;u16 vid,
IIlIIl,lIIIllI;u8 class,IIIIIlIlI,IIIllllII;unsigned long flags;int lIlIlIIl;
struct IlIIIlll*IIllll;if(strcmp(lIlIIl->bus->controller->driver->name,IllIIll)
==(0x2129+378-0x22a3)){IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x6f\x75\x72\x20\x76\x69\x72\x74\x75\x61\x6c\x20\x64\x65\x76\x69\x63\x65\x21" "\n"
);break;}vid=le16_to_cpu(lIlIIl->descriptor.idVendor);IIlIIl=le16_to_cpu(lIlIIl
->descriptor.idProduct);lIIIllI=le16_to_cpu(lIlIIl->descriptor.bcdDevice);
llIlIIIll(lIlIIl,&class,&IIIIIlIlI,&IIIllllII);IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x6e\x65\x77\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x20\x76\x69\x64\x3d\x25\x30\x34\x58\x20\x70\x69\x64\x3d\x25\x30\x34\x58\x20\x62\x75\x73\x5f\x69\x64\x3d" "\"" "\x25\x73" "\"" "\x20\x63\x75\x72\x5f\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x3d\x25\x75\x20\x74\x6f\x74\x61\x6c\x5f\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x73\x3d\x25\x75" "\n"
,vid,IIlIIl,IIlIIIl(&lIlIIl->dev),lIIllII->cur_altsetting->desc.bInterfaceNumber
,lIlIIl->actconfig->desc.bNumInterfaces);
lIlIlIIl=(0x1675+3059-0x2268);spin_lock_irqsave(&lIlIlIl,flags);
list_for_each_entry(IIllll,&IIllIIl,lIIIl){if(IIllll->vid==vid&&IIllll->IIlIIl==
IIlIIl&&!strcmp(IIllll->bus_id,IIlIIIl(&lIlIIl->dev))){lIlIlIIl=
(0xf0b+153-0xfa3);IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x64\x65\x76\x69\x63\x65\x20\x69\x73\x20\x65\x78\x63\x6c\x75\x64\x65\x64" "\n"
);break;}}spin_unlock_irqrestore(&lIlIlIl,flags);if(lIlIlIIl){IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x64\x65\x76\x69\x63\x65\x20\x69\x73\x20\x65\x78\x63\x6c\x75\x64\x65\x64" "\n"
);break;}
IlIlI=IlllIIlI(vid,IIlIIl,(char*)IIlIIIl(&lIlIIl->dev));

if(IlIlI==NULL&&lIIIIIII&&lIlIIl->descriptor.bDeviceClass!=(0x11c8+2833-0x1cd0)
&&lIIllIIl(vid,IIlIIl,lIIIllI,class,IIIIIlIlI,IIIllllII,NULL)){IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x61\x75\x74\x6f\x73\x68\x61\x72\x69\x6e\x67\x20\x74\x68\x69\x73\x20\x64\x65\x76\x69\x63\x65" "\n"
);if((IlIlI=llllIIlI(vid,IIlIIl,IIlIIIl(&lIlIIl->dev)))){


}}if(IlIlI){if(IllIIlIl(IlIlI,IIIlIIlI)==(0xf72+1564-0x158e)){IlIlI->IIllllIl=
lIlIIl->actconfig?lIlIIl->actconfig->desc.bNumInterfaces:(0x1e02+1150-0x2280);
IlIlI->lIIIIll=(strcmp(lIlIIl->bus->controller->driver->name,
"\x65\x68\x63\x69\x5f\x68\x63\x64")==(0x539+3694-0x13a7));spin_lock_irqsave(&
IlIlI->lIIIIII,flags);if(IlIlI->lIIII)usb_put_dev(IlIlI->lIIII);IlIlI->lIIII=
usb_get_dev(lIlIIl);spin_unlock_irqrestore(&IlIlI->lIIIIII,flags);IIIllllI(IlIlI
,lIlIIl);}if(++IlIlI->lIlIllII==IlIlI->IIllllIl){if(IlIlI->lIllIlI&IIlllIIII)
IlllIlll(IlIlI,IlIlI->lIIII,IIlIlIIll);IllIIlIl(IlIlI,llIIIlIlI);}IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x69\x6e\x69\x74\x5f\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x73\x3d\x25\x64\x20\x74\x6f\x74\x61\x6c\x5f\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x73\x3d\x25\x64" "\n"
,IlIlI->lIlIllII,IlIlI->IIllllIl);usb_set_intfdata(lIIllII,IlIlI);lIlll=
(0x214f+637-0x23cc);IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x6f\x75\x72\x20\x64\x65\x76\x69\x63\x65\x21" "\n"
);}else{IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x4e\x4f\x54\x20\x6f\x75\x72\x20\x64\x65\x76\x69\x63\x65\x21" "\n"
);}}while((0x1036+3750-0x1edc));if(lIlll!=(0x1087+2808-0x1b7f)){IIIll(
"\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x65\x72\x72\x6f\x72\x20\x25\x64" "\n"
,lIlll);}IIIll("\x75\x73\x62\x64\x5f\x70\x72\x6f\x62\x65\x3a\x20\x2d\x2d" "\n");
return lIlll;}static void lIlIIIlI(struct usb_interface*lIIllII){struct IllII*
IlIlI;IIIll(
"\x75\x73\x62\x64\x5f\x64\x69\x73\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x2b\x2b" "\n"
);
#if KERNEL_LT_EQ((0x1e44+1170-0x22d4),(0x10cb+94-0x1123),(0x2199+821-0x24a8))
lock_kernel();
#endif

IlIlI=usb_get_intfdata(lIIllII);usb_set_intfdata(lIIllII,NULL);
#if KERNEL_LT_EQ((0xbba+925-0xf55),(0x629+256-0x723),(0x1fbf+312-0x20d1))
unlock_kernel();
#endif
if(IlIlI){IllIIlIl(IlIlI,IIIlIIlI);if(--IlIlI->lIlIllII==(0x11f6+1725-0x18b3)){
unsigned long flags;IIlIlllI(IlIlI);
IllIIlIl(IlIlI,llIIIIII);spin_lock_irqsave(&IlIlI->lIIIIII,flags);if(IlIlI->
lIIII)usb_put_dev(IlIlI->lIIII);IlIlI->lIIII=NULL;spin_unlock_irqrestore(&IlIlI
->lIIIIII,flags);}IIIll(
"\x75\x73\x62\x64\x5f\x64\x69\x73\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x69\x6e\x69\x74\x5f\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x73\x3d\x25\x64\x20\x74\x6f\x74\x61\x6c\x5f\x69\x6e\x74\x65\x72\x66\x61\x63\x65\x73\x3d\x25\x64" "\n"
,IlIlI->lIlIllII,IlIlI->IIllllIl);llIllII(IlIlI);
}IIIll(
"\x75\x73\x62\x64\x5f\x64\x69\x73\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x2d\x2d" "\n"
);}
#if KERNEL_GT_EQ((0x3d+7483-0x1d76),(0x1e49+234-0x1f2d),(0xcdb+5585-0x2295))
static int IIlIlIIl(struct usb_interface*IIIllI){return(0x11b0+365-0x131d);}
static int IlIlIlll(struct usb_interface*IIIllI){return(0xa17+2722-0x14b9);}
#elif KERNEL_GT_EQ((0x1358+3384-0x208e),(0x1406+3063-0x1ff7),(0x8f2+111-0x94f))
static void IIlIlIIl(struct usb_interface*IIIllI){return;}static void IlIlIlll(
struct usb_interface*IIIllI){return;}
#endif


#if KERNEL_GT_EQ((0x895+5324-0x1d5f),(0xe0c+6082-0x25c8),(0x5a8+564-0x7c8))
int IIlIlIIIl(struct device*llllI){struct usb_interface*lIIllII=to_usb_interface
(llllI);IIIll(
"\x75\x73\x62\x64\x5f\x6e\x6f\x74\x69\x66\x79\x5f\x61\x64\x64\x3a\x20\x2b\x2b\x20\x62\x75\x73\x5f\x69\x64\x3d" "\"" "\x25\x73" "\"\n"
,IIlIIIl(llllI));if(!strchr(IIlIIIl(llllI),((char)(0x11e5+5355-0x2696)))){IIIll(
"\x75\x73\x62\x64\x5f\x6e\x6f\x74\x69\x66\x79\x5f\x61\x64\x64\x3a\x20\x2d\x2d\x20\x6e\x6f\x74\x20\x61\x20\x75\x73\x62\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65" "\n"
);return(0xc23+3011-0x17e6);}if(IllllIlI(lIIllII,NULL)==(0x8d1+1296-0xde1)){int 
lIlll;
#if KERNEL_GT_EQ((0x1d72+2300-0x266c),(0x1997+2046-0x218f),(0x8ba+1349-0xde5))
int state_in_sysfs;
#endif
IIIll(
"\x75\x73\x62\x64\x5f\x6e\x6f\x74\x69\x66\x79\x5f\x61\x64\x64\x3a\x20\x63\x6c\x61\x69\x6d\x69\x6e\x67\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65" "\n"
);
#if KERNEL_GT_EQ((0x14b8+4273-0x2567),(0x151+8657-0x231c),(0xe96+4215-0x1ef3))
state_in_sysfs=llllI->kobj.state_in_sysfs;llllI->kobj.state_in_sysfs=
(0x320+6764-0x1d8c);
#endif
lIlll=usb_driver_claim_interface(&IllIIIll,lIIllII,usb_get_intfdata(lIIllII));
#if KERNEL_GT_EQ((0xa08+1451-0xfb1),(0x394+6091-0x1b59),(0x791+241-0x868))
llllI->kobj.state_in_sysfs=state_in_sysfs;
#endif
if(lIlll<(0x6f7+6156-0x1f03)){IIIll(
"\x75\x73\x62\x64\x5f\x6e\x6f\x74\x69\x66\x79\x5f\x61\x64\x64\x3a\x20\x66\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x63\x6c\x61\x69\x6d\x20\x69\x6e\x74\x65\x72\x66\x61\x63\x65" "\n"
);lIlIIIlI(lIIllII);}}IIIll(
"\x75\x73\x62\x64\x5f\x6e\x6f\x74\x69\x66\x79\x5f\x61\x64\x64\x3a\x20\x2d\x2d" "\n"
);return(0x1610+3771-0x24cb);}void IlIlllIIl(struct device*llllI){
}int llIlllIII(struct notifier_block*self,unsigned long IlIIIlIIl,void*dev){
struct device*llllI=dev;if(!dev)return NOTIFY_OK;switch(IlIIIlIIl){case 
BUS_NOTIFY_ADD_DEVICE:if(IIlIlIIIl(llllI))return NOTIFY_BAD;break;case 
BUS_NOTIFY_DEL_DEVICE:IlIlllIIl(llllI);break;}return NOTIFY_OK;}
#endif 


int IIlllIII(void){int lIlll=(0x111f+251-0x121a);INIT_LIST_HEAD(&IlllllI);
spin_lock_init(&lIllII);lIlll=usb_register(&IllIIIll);if(lIlll!=
(0x359+912-0x6e9)){IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x69\x6e\x69\x74\x5f\x6d\x6f\x64\x75\x6c\x65\x3a\x20\x75\x73\x62\x5f\x72\x65\x67\x69\x73\x74\x65\x72\x20\x66\x61\x69\x6c\x65\x64\x2e\x20\x45\x72\x72\x6f\x72\x20\x6e\x75\x6d\x62\x65\x72\x20\x25\x64" "\n"
,lIlll);}
#if KERNEL_GT_EQ((0x220+6610-0x1bf0),(0x494+3528-0x1256),(0x45c+3778-0x130a))
lIlll=bus_register_notifier(IllIIIll.drvwrap.driver.bus,&lIIlIlIlI);if(lIlll!=
(0x758+5883-0x1e53)){IIIll(
"\x75\x73\x62\x64\x5f\x69\x6e\x69\x74\x3a\x20\x62\x75\x73\x5f\x72\x65\x67\x69\x73\x74\x65\x72\x5f\x6e\x6f\x74\x69\x66\x69\x65\x72\x20\x66\x61\x69\x6c\x65\x64\x2e\x20\x45\x72\x72\x6f\x72\x20\x6e\x75\x6d\x62\x65\x72\x20\x25\x64" "\n"
,lIlll);}
#endif
return lIlll;}void IIIlIlIll(void){static struct list_head lIIIIlII;struct IllII
*IIIIIlll;usb_deregister(&IllIIIll);
#if KERNEL_GT_EQ((0x1189+5297-0x2638),(0xa39+1505-0x1014),(0x1f7b+1214-0x2425))
bus_unregister_notifier(IllIIIll.drvwrap.driver.bus,&lIIlIlIlI);
#endif
INIT_LIST_HEAD(&lIIIIlII);spin_lock(&lIllII);list_splice_init(&IlllllI,&lIIIIlII
);spin_unlock(&lIllII);while(!list_empty(&lIIIIlII)){IIIIIlll=list_entry(
lIIIIlII.next,struct IllII,llIIll);list_del(&IIIIIlll->llIIll);llIllII(IIIIIlll)
;}}struct IllII*llllIIlI(u16 vid,u16 IIlIIl,const char*bus_id){struct IllII*
IlIlI;IlIlI=kmalloc(sizeof(*IlIlI),GFP_KERNEL);if(IlIlI){memset(IlIlI,
(0x11d8+38-0x11fe),sizeof(*IlIlI));IlIlI->IIlII=IIlIIllI(IlIlI);if(IlIlI->IIlII)
{IIIllIIl(IlIlI->IIlII);if(IlIlI->IIlII->IlllIl==-(0x1514+1277-0x1a10)){IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x63\x72\x65\x61\x74\x65\x3a\x20\x75\x73\x62\x64\x5f\x63\x64\x65\x76\x5f\x61\x6c\x6c\x6f\x63\x5f\x6d\x69\x6e\x6f\x72\x20\x66\x61\x69\x6c\x65\x64\x2e" "\n"
);}else{init_waitqueue_head(&IlIlI->lllIII);INIT_LIST_HEAD(&IlIlI->IlllIlI);
spin_lock_init(&IlIlI->lIlIIll);INIT_LIST_HEAD(&IlIlI->lllIll);spin_lock_init(&
IlIlI->IIllI);kref_init(&IlIlI->IlIlIl);spin_lock_init(&IlIlI->lIIIIII);
init_waitqueue_head(&IlIlI->IlIIIIIl);IlIlI->vid=vid;IlIlI->IIlIIl=IIlIIl;
strncpy(IlIlI->bus_id,bus_id,IllIlII-(0x1bf7+1726-0x22b4));IlIlI->IIIlII=-
(0xd9b+4973-0x2107);IlIlI->lIllIlI=(0x11d2+4995-0x2555);IlIlI->state=llIIIIII;
IlIlI->lIIII=NULL;IlIlI->IIllllIl=(0x1a8+7456-0x1ec8);IlIlI->lIlIllII=
(0x46c+6378-0x1d56);spin_lock_init(&IlIlI->lIIIll);IlIlI->lIIlIIl=
(0x894+7625-0x265d);IlIlI->IlIlllll=(0x46d+8573-0x25ea);IlIlI->lllIIIlI=-
(0x1f5+5294-0x16a2);IlIlI->IlIIIlIl=-(0x6c1+5729-0x1d21);IlIlI->llllIll=NULL;
IlIlI->IIIlIlI=NULL;IIlIIII(IlIlI);spin_lock(&lIllII);list_add(&IlIlI->llIIll,&
IlllllI);spin_unlock(&lIllII);return IlIlI;}}kfree(IlIlI);}return NULL;}void 
llIIIllI(struct kref*IlllIIl){struct IllII*IlIlI=container_of(IlllIIl,struct 
IllII,IlIlIl);IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x64\x65\x73\x74\x72\x6f\x79\x2b\x2b" "\n"
);spin_lock(&lIllII);if(IlIlI->llIIll.next!=LIST_POISON1)list_del(&IlIlI->llIIll
);spin_unlock(&lIllII);if(IlIlI->IIlII){IIllIlII(IlIlI->IIlII->IlllIl);lIIIIIIlI
(IlIlI->IIlII);}while(!list_empty(&IlIlI->IlllIlI)){struct IIIIl*lIIlI=
list_entry(IlIlI->IlllIlI.next,struct IIIIl,lIIIl);list_del(&lIIlI->lIIIl);
lllllll(lIIlI);}while(!list_empty(&IlIlI->lllIll)){struct IIIIl*lIIlI=list_entry
(IlIlI->lllIll.next,struct IIIIl,lIIIl);list_del(&lIIlI->lIIIl);lllllll(lIIlI);}
kfree(IlIlI);IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x64\x65\x73\x74\x72\x6f\x79\x2d\x2d" "\n"
);}void IlIlllII(struct IllII*IlIlI){int lIIlllII=(0x14b4+2438-0x1e3a);struct 
IllII*IIIIIlll;spin_lock(&lIllII);list_for_each_entry(IIIIIlll,&IlllllI,llIIll){
if(IIIIIlll==IlIlI){list_del(&IlIlI->llIIll);lIIlllII=(0x510+8661-0x26e4);break;
}}spin_unlock(&lIllII);if(lIIlllII)llIllII(IlIlI);IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x66\x72\x65\x65" "\n");}struct IllII*
IlllIIlI(u16 vid,u16 IIlIIl,char*bus_id){struct IllII*IlIlI;IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x66\x69\x6e\x64\x31\x3a\x20\x2b\x2b" "\n"
);spin_lock(&lIllII);list_for_each_entry(IlIlI,&IlllllI,llIIll){if(IlIlI->vid==
vid&&IlIlI->IIlIIl==IIlIIl&&!strcmp(IlIlI->bus_id,bus_id)){IIlIIII(IlIlI);
spin_unlock(&lIllII);IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x66\x69\x6e\x64\x31\x3a\x20\x2d\x2d\x20\x66\x6f\x75\x6e\x64\x21" "\n"
);return IlIlI;}}spin_unlock(&lIllII);IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x66\x69\x6e\x64\x31\x3a\x20\x2d\x2d\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64" "\n"
);return NULL;}struct IllII*IIllIIIl(u32 IIIlII){struct IllII*IlIlI;spin_lock(&
lIllII);list_for_each_entry(IlIlI,&IlllllI,llIIll){if(IlIlI->IIIlII==IIIlII){
IIlIIII(IlIlI);spin_unlock(&lIllII);return IlIlI;}}spin_unlock(&lIllII);return 
NULL;}struct IllII*IlllIIIlI(int IlllIl){struct IllII*IlIlI;spin_lock(&lIllII);
list_for_each_entry(IlIlI,&IlllllI,llIIll){if(IlIlI->IIlII->IlllIl==IlllIl){
IIlIIII(IlIlI);spin_unlock(&lIllII);return IlIlI;}}spin_unlock(&lIllII);return 
NULL;}int IllIIlIl(struct IllII*IlIlI,IIlIllII IIllIIII){if(!IlIlI)return-
(0x58+4251-0x10f2);if(IlIlI->state!=IIllIIII){IlIlI->state=IIllIIII;if(IlIlI->
state==llIIIlIlI){IlIlI->llIllIll=(0x13c1+2894-0x1f0e);wake_up(&IlIlI->IlIIIIIl)
;}return IllIIlll(IlIlI);}return-(0x1021+3384-0x1d58);}int lIIllIll(struct IllII
*IlIlI,struct usb_device*lIIII,int lIlIIIll,int IIIIlIIII){int lIlll;char s[
(0xcf9+5913-0x23e0)];if(!IlIlI)return-(0x1bd6+237-0x1cc2);if(lIlIIIll<=
(0x1171+1900-0x18dd)||lIlIIIll>(0x1b51+348-0x1bae))return-(0x41a+4352-0x1519);if
(lIIII->actconfig&&lIIII->actconfig->desc.bConfigurationValue==lIlIIIll){if(
IlIlI->lIllIlI&IllllIlIl)return(0x2e0+1436-0x87c);if(usb_lock_device_for_reset(
lIIII,NULL)>=(0xda+2272-0x9ba)){lIlll=usb_reset_configuration(lIIII);
usb_unlock_device(lIIII);}return(0x260+417-0x401);}IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x63\x68\x61\x6e\x67\x65\x5f\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x3a\x20\x66\x72\x6f\x6d\x20\x25\x64\x20\x74\x6f\x20\x25\x64" "\n"
,lIIII->actconfig?lIIII->actconfig->desc.bConfigurationValue:(0x31b+6768-0x1d8b)
,lIlIIIll);IlIlI->llIllIll=(0xb9+5432-0x15f1);snprintf(s,sizeof(s)/sizeof(s[
(0x3b5+6812-0x1e51)]),
"\x63\x6f\x6e\x66\x69\x67\x75\x72\x61\x74\x69\x6f\x6e\x20\x25\x2e\x38\x78\x20\x25\x64"
,(u32)IlIlI->IIIlII,lIlIIIll);lllIlIlI(Illllll,s);lIlll=
wait_event_interruptible_timeout(IlIlI->IlIIIIIl,IlIlI->llIllIll,
msecs_to_jiffies(IIIIlIIII));if(lIlll==(0x140+509-0x33d))lIlll=-ETIMEDOUT;else 
if(lIlll>(0xc3a+644-0xebe))lIlll=jiffies_to_msecs(lIlll);else if(lIlll==-
ERESTARTSYS)lIlll=-ERESTART;return lIlll;}int IllIIlll(struct IllII*IlIlI){char 
s[(0x376+8156-0x2320)];if(!IlIlI)return-(0x9e1+2285-0x12cd);if(IlIlI->IIIlII==(
u32)-(0xa86+867-0xde8))return(0x23e1+752-0x26d1);snprintf(s,sizeof(s)/sizeof(s[
(0x9c8+1544-0xfd0)]),"\x73\x74\x61\x74\x65\x20\x25\x2e\x38\x78\x20\x25\x64",(u32
)IlIlI->IIIlII,IlIlI->state);lllIlIlI(Illllll,s);return(0x15d4+3350-0x22ea);}int
 lllIlIlll(struct IllII*IlIlI,struct usb_device*lIIII){char IlIIl[
(0x9af+6235-0x21f8)];return usb_control_msg(lIIII,usb_rcvctrlpipe(lIIII,
(0x77+515-0x27a)),USB_REQ_GET_DESCRIPTOR,USB_DIR_IN,((0xa14+6816-0x24b3)<<
(0x19eb+3341-0x26f0))+(0x119a+1087-0x15d9),(0x1495+1953-0x1c36),IlIIl,sizeof(
IlIIl),USB_CTRL_GET_TIMEOUT/(0x418+2802-0xf08));}void IlllIlll(struct IllII*
IlIlI,struct usb_device*lIIII,int IIIIllll){struct IIIIl*lIIlI,*IIIlIII;unsigned
 long flags;IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x72\x65\x73\x65\x74\x2b\x2b" "\n");if(
IlIlI->lIIlIIl)
llIlIlIl(IlIlI);




do{lIIlI=NULL;spin_lock_irqsave(&IlIlI->IIllI,flags);list_for_each_entry(IIIlIII
,&IlIlI->lllIll,lIIIl){if(IIIlIII->endpoint!=(lllIl)-(0x950+6997-0x24a4)){IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x72\x65\x73\x65\x74\x3a\x20\x75\x72\x62\x20\x74\x6f\x20\x62\x65\x20\x63\x61\x6e\x63\x65\x6c\x6c\x65\x64\x20\x6f\x6e\x20\x25\x64\x20\x65\x6e\x64\x70\x6f\x69\x6e\x74" "\n"
,IIIlIII->endpoint);
IIIlIII->endpoint=(lllIl)-(0x3bd+5762-0x1a3e);



kref_get(&IIIlIII->IlIlIl);
lIIlI=IIIlIII;break;}}spin_unlock_irqrestore(&IlIlI->IIllI,flags);if(lIIlI){if(
lIIlI->lIlII)usb_unlink_urb(lIIlI->lIlII);else lIlIlIII(&lIIlI->IlllII);kref_put
(&lIIlI->IlIlIl,lIIlIl);}}while(lIIlI);IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x72\x65\x73\x65\x74\x3a\x20\x75\x72\x62\x73\x20\x63\x61\x6e\x63\x65\x6c\x6c\x65\x64" "\n"
);if(usb_lock_device_for_reset(lIIII,NULL)>=(0x170+8005-0x20b5)){int i,IIIIlllI;
if(lIIII->actconfig){for(i=(0xa12+4570-0x1bec);i<lIIII->actconfig->desc.
bNumInterfaces;i++){struct usb_interface*lIIllII=lIIII->actconfig->interface[i];
if(lIIllII&&lIIllII->num_altsetting>(0x70f+2282-0xff8)){struct 
usb_host_interface*lIIlIlI=usb_altnum_to_altsetting(lIIllII,(0xadf+5957-0x2224))
;if(lIIlIlI){IIIIlllI=usb_set_interface(lIIII,lIIlIlI->desc.bInterfaceNumber,
lIIlIlI->desc.bAlternateSetting);}}}}if(IIIIllll==IIlIlIIll){
if((IlIlI->lIllIlI&IllIlllIl)==(0xe9d+4381-0x1fba)){IIIIlll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x72\x65\x73\x65\x74\x3a\x20\x74\x72\x79\x69\x6e\x67\x20\x74\x6f\x20\x72\x65\x73\x65\x74\x20\x64\x65\x76\x69\x63\x65\x20\x76\x69\x64\x20\x25\x30\x34\x78\x20\x70\x69\x64\x20\x25\x30\x34\x78" "\n"
,IlIlI->vid,IlIlI->IIlIIl);IIIIlllI=usb_reset_device(lIIII);IIIIlll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x72\x65\x73\x65\x74\x3a\x20\x75\x73\x62\x5f\x72\x65\x73\x65\x74\x5f\x64\x65\x76\x69\x63\x65\x20\x72\x65\x74\x75\x72\x6e\x65\x64\x20\x25\x64" "\n"
,IIIIlllI);}}usb_unlock_device(lIIII);}memset(IlIlI->lllIlllI,
(0x8b6+5590-0x1e8c),sizeof(IlIlI->lllIlllI));memset(IlIlI->IlIIIIlI,
(0x12fc+2555-0x1cf7),sizeof(IlIlI->IlIIIIlI));IIIll(
"\x75\x73\x62\x64\x5f\x73\x74\x75\x62\x5f\x72\x65\x73\x65\x74\x2d\x2d" "\n");}
int IIIIlIII(struct IllII*IlIlI,int pipe){if(!usb_pipeisoc(pipe))return
(0xedf+3449-0x1c58);if(usb_pipein(pipe))return(++IlIlI->IIllIlIl[
usb_pipeendpoint(pipe)]);else return(++IlIlI->IIllIIll[usb_pipeendpoint(pipe)]);
}int lIIIIlI(struct IllII*IlIlI,int pipe){if(!usb_pipeisoc(pipe))return
(0x199f+2072-0x21b7);if(usb_pipein(pipe))return(--IlIlI->IIllIlIl[
usb_pipeendpoint(pipe)]);else return(--IlIlI->IIllIIll[usb_pipeendpoint(pipe)]);
}void IlIIIIlII(struct IllII*IlIlI,int pipe){if(!usb_pipeisoc(pipe))return;if(
usb_pipein(pipe))IlIlI->IIllIlIl[usb_pipeendpoint(pipe)]=(0xc3+2705-0xb54);else 
IlIlI->IIllIIll[usb_pipeendpoint(pipe)]=(0x3ff+8795-0x265a);}int lIlllIII(struct
 IllII*IlIlI,int pipe){if(!usb_pipeisoc(pipe))return(0x131f+3474-0x20b1);if(
usb_pipein(pipe))return IlIlI->lllIlllI[usb_pipeendpoint(pipe)];else return 
IlIlI->IlIIIIlI[usb_pipeendpoint(pipe)];}void lIIlIIIl(struct IllII*IlIlI,int 
pipe,int IIIllll){if(!usb_pipeisoc(pipe))return;if(usb_pipein(pipe))IlIlI->
lllIlllI[usb_pipeendpoint(pipe)]=IIIllll;else IlIlI->IlIIIIlI[usb_pipeendpoint(
pipe)]=IIIllll;}struct usb_device*lIllIIlI(struct IllII*IlIlI){unsigned long 
flags;struct usb_device*lIlll=NULL;spin_lock_irqsave(&IlIlI->lIIIIII,flags);if(
IlIlI->lIIII){lIlll=usb_get_dev(IlIlI->lIIII);}spin_unlock_irqrestore(&IlIlI->
lIIIIII,flags);return lIlll;}void IIlllllI(struct IllII*IlIlI,struct usb_device*
lIIII){usb_put_dev(lIIII);}
#endif 

