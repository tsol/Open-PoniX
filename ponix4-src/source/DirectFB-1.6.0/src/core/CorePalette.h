/*
   (c) Copyright 2001-2011  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef ___CorePalette__H___
#define ___CorePalette__H___

#include <core/CorePalette_includes.h>

/**********************************************************************************************************************
 * CorePalette
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CorePalette_SetEntries(
                    CorePalette                               *obj,
                    const DFBColor                            *colors,
                    u32                                        num,
                    u32                                        offset);

DFBResult CorePalette_SetEntriesYUV(
                    CorePalette                               *obj,
                    const DFBColorYUV                         *colors,
                    u32                                        num,
                    u32                                        offset);


void CorePalette_Init_Dispatch(
                    CoreDFB              *core,
                    CorePalette          *obj,
                    FusionCall           *call
);

void  CorePalette_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CorePalette Calls
 */
typedef enum {
    CorePalette_SetEntries = 1,
    CorePalette_SetEntriesYUV = 2,
} CorePaletteCall;

/*
 * CorePalette_SetEntries
 */
typedef struct {
    u32                                        num;
    u32                                        offset;
    /* 'num' DFBColor follow (colors) */
} CorePaletteSetEntries;

typedef struct {
    DFBResult                                  result;
} CorePaletteSetEntriesReturn;


/*
 * CorePalette_SetEntriesYUV
 */
typedef struct {
    u32                                        num;
    u32                                        offset;
    /* 'num' DFBColorYUV follow (colors) */
} CorePaletteSetEntriesYUV;

typedef struct {
    DFBResult                                  result;
} CorePaletteSetEntriesYUVReturn;





class IPalette : public Interface
{
public:
    IPalette( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult SetEntries(
                    const DFBColor                            *colors,
                    u32                                        num,
                    u32                                        offset
    ) = 0;

    virtual DFBResult SetEntriesYUV(
                    const DFBColorYUV                         *colors,
                    u32                                        num,
                    u32                                        offset
    ) = 0;

};



class IPalette_Real : public IPalette
{
private:
    CorePalette *obj;

public:
    IPalette_Real( CoreDFB *core, CorePalette *obj )
        :
        IPalette( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetEntries(
                    const DFBColor                            *colors,
                    u32                                        num,
                    u32                                        offset
    );

    virtual DFBResult SetEntriesYUV(
                    const DFBColorYUV                         *colors,
                    u32                                        num,
                    u32                                        offset
    );

};



class IPalette_Requestor : public IPalette
{
private:
    CorePalette *obj;

public:
    IPalette_Requestor( CoreDFB *core, CorePalette *obj )
        :
        IPalette( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetEntries(
                    const DFBColor                            *colors,
                    u32                                        num,
                    u32                                        offset
    );

    virtual DFBResult SetEntriesYUV(
                    const DFBColorYUV                         *colors,
                    u32                                        num,
                    u32                                        offset
    );

};


DFBResult CorePaletteDispatch__Dispatch( CorePalette *obj,
                    FusionID      caller,
                    int           method,
                    void         *ptr,
                    unsigned int  length,
                    void         *ret_ptr,
                    unsigned int  ret_size,
                    unsigned int *ret_length );

}


#endif

#endif
