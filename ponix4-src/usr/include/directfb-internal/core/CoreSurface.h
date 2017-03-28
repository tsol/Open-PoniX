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

#ifndef ___CoreSurface__H___
#define ___CoreSurface__H___

#include <core/CoreSurface_includes.h>

/**********************************************************************************************************************
 * CoreSurface
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreSurface_SetConfig(
                    CoreSurface                               *obj,
                    const CoreSurfaceConfig                   *config);

DFBResult CoreSurface_Flip(
                    CoreSurface                               *obj,
                    bool                                       swap);

DFBResult CoreSurface_GetPalette(
                    CoreSurface                               *obj,
                    CorePalette                              **ret_palette);

DFBResult CoreSurface_SetPalette(
                    CoreSurface                               *obj,
                    CorePalette                               *palette);

DFBResult CoreSurface_SetAlphaRamp(
                    CoreSurface                               *obj,
                    u8                                         a0,
                    u8                                         a1,
                    u8                                         a2,
                    u8                                         a3);

DFBResult CoreSurface_SetField(
                    CoreSurface                               *obj,
                    s32                                        field);

DFBResult CoreSurface_PreLockBuffer(
                    CoreSurface                               *obj,
                    CoreSurfaceBuffer                         *buffer,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    CoreSurfaceAllocation                    **ret_allocation);

DFBResult CoreSurface_PreLockBuffer2(
                    CoreSurface                               *obj,
                    CoreSurfaceBufferRole                      role,
                    DFBSurfaceStereoEye                        eye,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    bool                                       lock,
                    CoreSurfaceAllocation                    **ret_allocation);

DFBResult CoreSurface_PreReadBuffer(
                    CoreSurface                               *obj,
                    CoreSurfaceBuffer                         *buffer,
                    const DFBRectangle                        *rect,
                    CoreSurfaceAllocation                    **ret_allocation);

DFBResult CoreSurface_PreWriteBuffer(
                    CoreSurface                               *obj,
                    CoreSurfaceBuffer                         *buffer,
                    const DFBRectangle                        *rect,
                    CoreSurfaceAllocation                    **ret_allocation);

DFBResult CoreSurface_PreLockBuffer3(
                    CoreSurface                               *obj,
                    CoreSurfaceBufferRole                      role,
                    u32                                        flip_count,
                    DFBSurfaceStereoEye                        eye,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    bool                                       lock,
                    CoreSurfaceAllocation                    **ret_allocation);

DFBResult CoreSurface_CreateClient(
                    CoreSurface                               *obj,
                    CoreSurfaceClient                        **ret_client);

DFBResult CoreSurface_BackToFrontCopy(
                    CoreSurface                               *obj,
                    DFBSurfaceStereoEye                        eye,
                    const DFBRegion                           *left_region,
                    const DFBRegion                           *right_region);


void CoreSurface_Init_Dispatch(
                    CoreDFB              *core,
                    CoreSurface          *obj,
                    FusionCall           *call
);

void  CoreSurface_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreSurface Calls
 */
typedef enum {
    CoreSurface_SetConfig = 1,
    CoreSurface_Flip = 2,
    CoreSurface_GetPalette = 3,
    CoreSurface_SetPalette = 4,
    CoreSurface_SetAlphaRamp = 5,
    CoreSurface_SetField = 6,
    CoreSurface_PreLockBuffer = 7,
    CoreSurface_PreLockBuffer2 = 8,
    CoreSurface_PreReadBuffer = 9,
    CoreSurface_PreWriteBuffer = 10,
    CoreSurface_PreLockBuffer3 = 11,
    CoreSurface_CreateClient = 12,
    CoreSurface_BackToFrontCopy = 13,
} CoreSurfaceCall;

/*
 * CoreSurface_SetConfig
 */
typedef struct {
    CoreSurfaceConfig                          config;
} CoreSurfaceSetConfig;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceSetConfigReturn;


/*
 * CoreSurface_Flip
 */
typedef struct {
    bool                                       swap;
} CoreSurfaceFlip;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceFlipReturn;


/*
 * CoreSurface_GetPalette
 */
typedef struct {
} CoreSurfaceGetPalette;

typedef struct {
    DFBResult                                  result;
    u32                                        palette_id;
    void*                                      palette_ptr;
} CoreSurfaceGetPaletteReturn;


/*
 * CoreSurface_SetPalette
 */
typedef struct {
    u32                                        palette_id;
} CoreSurfaceSetPalette;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceSetPaletteReturn;


/*
 * CoreSurface_SetAlphaRamp
 */
typedef struct {
    u8                                         a0;
    u8                                         a1;
    u8                                         a2;
    u8                                         a3;
} CoreSurfaceSetAlphaRamp;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceSetAlphaRampReturn;


/*
 * CoreSurface_SetField
 */
typedef struct {
    s32                                        field;
} CoreSurfaceSetField;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceSetFieldReturn;


/*
 * CoreSurface_PreLockBuffer
 */
typedef struct {
    u32                                        buffer_id;
    CoreSurfaceAccessorID                      accessor;
    CoreSurfaceAccessFlags                     access;
} CoreSurfacePreLockBuffer;

typedef struct {
    DFBResult                                  result;
    u32                                        allocation_id;
    void*                                      allocation_ptr;
} CoreSurfacePreLockBufferReturn;


/*
 * CoreSurface_PreLockBuffer2
 */
typedef struct {
    CoreSurfaceBufferRole                      role;
    DFBSurfaceStereoEye                        eye;
    CoreSurfaceAccessorID                      accessor;
    CoreSurfaceAccessFlags                     access;
    bool                                       lock;
} CoreSurfacePreLockBuffer2;

typedef struct {
    DFBResult                                  result;
    u32                                        allocation_id;
    void*                                      allocation_ptr;
} CoreSurfacePreLockBuffer2Return;


/*
 * CoreSurface_PreReadBuffer
 */
typedef struct {
    u32                                        buffer_id;
    DFBRectangle                               rect;
} CoreSurfacePreReadBuffer;

typedef struct {
    DFBResult                                  result;
    u32                                        allocation_id;
    void*                                      allocation_ptr;
} CoreSurfacePreReadBufferReturn;


/*
 * CoreSurface_PreWriteBuffer
 */
typedef struct {
    u32                                        buffer_id;
    DFBRectangle                               rect;
} CoreSurfacePreWriteBuffer;

typedef struct {
    DFBResult                                  result;
    u32                                        allocation_id;
    void*                                      allocation_ptr;
} CoreSurfacePreWriteBufferReturn;


/*
 * CoreSurface_PreLockBuffer3
 */
typedef struct {
    CoreSurfaceBufferRole                      role;
    u32                                        flip_count;
    DFBSurfaceStereoEye                        eye;
    CoreSurfaceAccessorID                      accessor;
    CoreSurfaceAccessFlags                     access;
    bool                                       lock;
} CoreSurfacePreLockBuffer3;

typedef struct {
    DFBResult                                  result;
    u32                                        allocation_id;
    void*                                      allocation_ptr;
} CoreSurfacePreLockBuffer3Return;


/*
 * CoreSurface_CreateClient
 */
typedef struct {
} CoreSurfaceCreateClient;

typedef struct {
    DFBResult                                  result;
    u32                                        client_id;
    void*                                      client_ptr;
} CoreSurfaceCreateClientReturn;


/*
 * CoreSurface_BackToFrontCopy
 */
typedef struct {
    DFBSurfaceStereoEye                        eye;
    bool                                       left_region_set;
    DFBRegion                                  left_region;
    bool                                       right_region_set;
    DFBRegion                                  right_region;
} CoreSurfaceBackToFrontCopy;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceBackToFrontCopyReturn;





class ISurface : public Interface
{
public:
    ISurface( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult SetConfig(
                    const CoreSurfaceConfig                   *config
    ) = 0;

    virtual DFBResult Flip(
                    bool                                       swap
    ) = 0;

    virtual DFBResult GetPalette(
                    CorePalette                              **ret_palette
    ) = 0;

    virtual DFBResult SetPalette(
                    CorePalette                               *palette
    ) = 0;

    virtual DFBResult SetAlphaRamp(
                    u8                                         a0,
                    u8                                         a1,
                    u8                                         a2,
                    u8                                         a3
    ) = 0;

    virtual DFBResult SetField(
                    s32                                        field
    ) = 0;

    virtual DFBResult PreLockBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    CoreSurfaceAllocation                    **ret_allocation
    ) = 0;

    virtual DFBResult PreLockBuffer2(
                    CoreSurfaceBufferRole                      role,
                    DFBSurfaceStereoEye                        eye,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    bool                                       lock,
                    CoreSurfaceAllocation                    **ret_allocation
    ) = 0;

    virtual DFBResult PreReadBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    const DFBRectangle                        *rect,
                    CoreSurfaceAllocation                    **ret_allocation
    ) = 0;

    virtual DFBResult PreWriteBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    const DFBRectangle                        *rect,
                    CoreSurfaceAllocation                    **ret_allocation
    ) = 0;

    virtual DFBResult PreLockBuffer3(
                    CoreSurfaceBufferRole                      role,
                    u32                                        flip_count,
                    DFBSurfaceStereoEye                        eye,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    bool                                       lock,
                    CoreSurfaceAllocation                    **ret_allocation
    ) = 0;

    virtual DFBResult CreateClient(
                    CoreSurfaceClient                        **ret_client
    ) = 0;

    virtual DFBResult BackToFrontCopy(
                    DFBSurfaceStereoEye                        eye,
                    const DFBRegion                           *left_region,
                    const DFBRegion                           *right_region
    ) = 0;

};



class ISurface_Real : public ISurface
{
private:
    CoreSurface *obj;

public:
    ISurface_Real( CoreDFB *core, CoreSurface *obj )
        :
        ISurface( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetConfig(
                    const CoreSurfaceConfig                   *config
    );

    virtual DFBResult Flip(
                    bool                                       swap
    );

    virtual DFBResult GetPalette(
                    CorePalette                              **ret_palette
    );

    virtual DFBResult SetPalette(
                    CorePalette                               *palette
    );

    virtual DFBResult SetAlphaRamp(
                    u8                                         a0,
                    u8                                         a1,
                    u8                                         a2,
                    u8                                         a3
    );

    virtual DFBResult SetField(
                    s32                                        field
    );

    virtual DFBResult PreLockBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult PreLockBuffer2(
                    CoreSurfaceBufferRole                      role,
                    DFBSurfaceStereoEye                        eye,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    bool                                       lock,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult PreReadBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    const DFBRectangle                        *rect,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult PreWriteBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    const DFBRectangle                        *rect,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult PreLockBuffer3(
                    CoreSurfaceBufferRole                      role,
                    u32                                        flip_count,
                    DFBSurfaceStereoEye                        eye,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    bool                                       lock,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult CreateClient(
                    CoreSurfaceClient                        **ret_client
    );

    virtual DFBResult BackToFrontCopy(
                    DFBSurfaceStereoEye                        eye,
                    const DFBRegion                           *left_region,
                    const DFBRegion                           *right_region
    );

};



class ISurface_Requestor : public ISurface
{
private:
    CoreSurface *obj;

public:
    ISurface_Requestor( CoreDFB *core, CoreSurface *obj )
        :
        ISurface( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetConfig(
                    const CoreSurfaceConfig                   *config
    );

    virtual DFBResult Flip(
                    bool                                       swap
    );

    virtual DFBResult GetPalette(
                    CorePalette                              **ret_palette
    );

    virtual DFBResult SetPalette(
                    CorePalette                               *palette
    );

    virtual DFBResult SetAlphaRamp(
                    u8                                         a0,
                    u8                                         a1,
                    u8                                         a2,
                    u8                                         a3
    );

    virtual DFBResult SetField(
                    s32                                        field
    );

    virtual DFBResult PreLockBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult PreLockBuffer2(
                    CoreSurfaceBufferRole                      role,
                    DFBSurfaceStereoEye                        eye,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    bool                                       lock,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult PreReadBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    const DFBRectangle                        *rect,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult PreWriteBuffer(
                    CoreSurfaceBuffer                         *buffer,
                    const DFBRectangle                        *rect,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult PreLockBuffer3(
                    CoreSurfaceBufferRole                      role,
                    u32                                        flip_count,
                    DFBSurfaceStereoEye                        eye,
                    CoreSurfaceAccessorID                      accessor,
                    CoreSurfaceAccessFlags                     access,
                    bool                                       lock,
                    CoreSurfaceAllocation                    **ret_allocation
    );

    virtual DFBResult CreateClient(
                    CoreSurfaceClient                        **ret_client
    );

    virtual DFBResult BackToFrontCopy(
                    DFBSurfaceStereoEye                        eye,
                    const DFBRegion                           *left_region,
                    const DFBRegion                           *right_region
    );

};


DFBResult CoreSurfaceDispatch__Dispatch( CoreSurface *obj,
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
