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

#ifndef llIlllllI
#define llIlllllI
#include "apitypes.h"
#pragma pack(push,1)
enum{llIIIlII=(0x1352+3113-0x1f7b),llIIllIl,lIIIlIlI,Illlllll,IIlIllll,IIIlllII,
IlIlIlI,lIllllII,IlllIIIl,IlIIllll,lIIIlllIl,lllllIllI,llllIlll,IIIlIIII,
IlIllllI,};enum{IIIIllIll=(0xfb0+1624-0x1608),lllllIIIl,llIlIIlII,lllIIIIll,
llIIIIIlI,lIIIIIIll,llIlIlIII,IlIlIlIIl,lllllIlII,lIIIllIll,llllIllII,lIllIIIlI,
IIllllIlI,IIIlIllIl,IIIllIlll,IIIIIlllI,IIlIIIIII,llIIlIIll};
#define llllII 			(0x467+944-0x816)
#define IIlIllI 		(0x19db+465-0x1baa)
#define IIlIlIlI          	(0x18ed+3251-0x259c)
#define IlIlIIIII 			(0xe3b+5128-0x223b)
typedef struct{lIlIIlI IIIIll;IIIlll lIlIl;IIIlll lIlllI;IIIlll Status;IIIlll 
Context;}__attribute__((packed))llIIlII,*IllllIIIl;typedef struct{llIIlII IIlIll
;lllIl llIIlll;lllIl IIlllIlI;lllIl lIIlIlII;lllIl llIIllll;IIIlIl llIllIIl;
IIIlll llIlI;}__attribute__((packed))IIIIllIII,*IIlllIIIl;typedef struct{llIIlII
 IIlIll;lllIl llIlIIIl;lllIl llllIIII;struct{lllIl IIIlIIl;lllIl lllIIIl;}
__attribute__((packed))lIllllI[(0x4d6+644-0x759)];}__attribute__((packed))
IlIlIIlII,*IIIlIlIIl;typedef struct{llIIlII IIlIll;lllIl IIIlIIl;lllIl lllIIIl;}
__attribute__((packed))lIIlIIIIl,*lIIlIIllI;
#define lIlIIlIlI			(0x100c+5297-0x24bd)
#define Illllllll				(0xe11+2054-0x1616)
#define IlIIlIIIl				(0xa60+6284-0x22ea)
#define lIlIlIlII			(0x4c+9227-0x2454)
#define lllIlllII		(0x26b+3685-0x10d0)
#define IlIIlIllI		(0x579+6046-0x1d16)
#define llIlIllll		(0x6e9+5157-0x1b0c)
#define llIIlllll			(0x7eb+2006-0xfbe)
#define lIlllIIlI			(0x16d+2875-0xca7)
#define lllIllIll			(0x949+211-0xa1c)
typedef struct{llIIlII IIlIll;lllIl Endpoint;lllIl Flags;union{struct{lllIl 
lIIlIllIl:(0x1abd+467-0x1c8b);lllIl lllllllIl:(0x4ff+2020-0xce1);lllIl lIIlIIlIl
:(0x86+8130-0x2047);}__attribute__((packed))llllllIIl;lllIl llIIlll;}
__attribute__((packed));lllIl lIllIlll;IIIlIl lllIIIIl;IIIlIl IIllIllI;IIIlll 
llIlI;}__attribute__((packed))lllIlIII,*IIlllIIll;typedef struct{llIIlII IIlIll;
IIIlll llIlI;lllIl Endpoint;lllIl Flags;}__attribute__((packed))lIIIllIIl,*
IIIlllllI;typedef struct{llIIlII IIlIll;IIIlll llIlI;IIIlll Interval;lllIl 
Endpoint;lllIl Flags;}__attribute__((packed))lIIllllI,*IllIlIlII;typedef struct{
llIIlII IIlIll;lllIl Flags;lllIl Endpoint;}__attribute__((packed))IIIlllIll,*
IlllllIII;typedef struct{llIIlII IIlIll;IIIlll IIlIIIlII;}__attribute__((packed)
)llIlIIIlI,*IlIlIIlll;
#define IlIlIIl(IIlIl) (sizeof(lllIllII) - sizeof(llllIII) + \
			sizeof(llllIII)*(IIlIl)->IIlll.llIlIll)
typedef struct{IIIlll Offset;IIIlll Length;IIIlll Status;}__attribute__((packed)
)llllIII,*IIIlIIIlI;typedef struct{llIIlII IIlIll;lllIl Endpoint;lllIl Flags;
IIIlll Interval;IIIlll lIlllII;IIIlll llIlIll;IIIlll lIlIIII;IIIlll llIlI;
llllIII IIlIlI[(0x159a+955-0x1954)];}__attribute__((packed))lllIllII,*lIlIIIIIl;
#define lIlIIllll(llllllIll) (sizeof(lIIIIIIl)-(0xfaf+3158-0x1c04)+(llllllIll))
#define lIlllllIl(IIIIIIlIl) ((IIIIIIlIl)->IIIlI.lIlIl-(sizeof(lIIIIIIl)-\
(0xc06+136-0xc8d)))
typedef struct{llIIlII IIlIll;lllIl IllllIIlI[(0x13a6+2535-0x1d8c)];}
__attribute__((packed))lIIIIIIl,*lIIIIllII;
#define llIIIlIl		(0x696+6081-0x1e56)
#define IlIlIlIl		(0x1a67+21-0x1a7a)
typedef struct{llIIlII IIlIll;IIIlll lIlllIlI;}__attribute__((packed))lllIlllIl,
*IIIIIIIIl;typedef struct{llIIlII IIlIll;}__attribute__((packed))IlIllIlI,*
lIIIIIIII;typedef struct{llIIlII IIlIll;}__attribute__((packed))lIIIIlIl,*
lIIIllIII;typedef struct{llIIlII IIlIll;lllIl Endpoint;lllIl Flags;}
__attribute__((packed))IlIIIIIlI,*IlIlIlIll;typedef union{llIIlII IIIlI;
IIIIllIII lllllI;IlIlIIlII IIIIIII;lIIlIIIIl IIIIIll;lllIlIII llIIlI;lIIIllIIl 
llIIl;lIIllllI lIllIl;IIIlllIll IlIIIII;llIlIIIlI lIlIIlll;lllIllII IIlll;
lIIIIIIl llIIlIlII;lllIlllIl IIlIIlIl;IlIllIlI IIIllllll;lIIIIlIl IIIIlIllI;
IlIIIIIlI lIlIIllI;}__attribute__((packed))llllIIIll,*llIII;
#pragma pack(pop)
#endif 

