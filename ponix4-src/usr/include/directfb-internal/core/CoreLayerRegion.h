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

#ifndef ___CoreLayerRegion__H___
#define ___CoreLayerRegion__H___

#include <core/CoreLayerRegion_includes.h>

/**********************************************************************************************************************
 * CoreLayerRegion
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreLayerRegion_GetSurface(
                    CoreLayerRegion                           *obj,
                    CoreSurface                              **ret_surface);

DFBResult CoreLayerRegion_FlipUpdate(
                    CoreLayerRegion                           *obj,
                    const DFBRegion                           *update,
                    DFBSurfaceFlipFlags                        flags);

DFBResult CoreLayerRegion_FlipUpdateStereo(
                    CoreLayerRegion                           *obj,
                    const DFBRegion                           *left,
                    const DFBRegion                           *right,
                    DFBSurfaceFlipFlags                        flags);


void CoreLayerRegion_Init_Dispatch(
                    CoreDFB              *core,
                    CoreLayerRegion      *obj,
                    FusionCall           *call
);

void  CoreLayerRegion_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreLayerRegion Calls
 */
typedef enum {
    CoreLayerRegion_GetSurface = 1,
    CoreLayerRegion_FlipUpdate = 2,
    CoreLayerRegion_FlipUpdateStereo = 3,
} CoreLayerRegionCall;

/*
 * CoreLayerRegion_GetSurface
 */
typedef struct {
} CoreLayerRegionGetSurface;

typedef struct {
    DFBResult                                  result;
    u32                                        surface_id;
    void*                                      surface_ptr;
} CoreLayerRegionGetSurfaceReturn;


/*
 * CoreLayerRegion_FlipUpdate
 */
typedef struct {
    bool                                       update_set;
    DFBRegion                                  update;
    DFBSurfaceFlipFlags                        flags;
} CoreLayerRegionFlipUpdate;

typedef struct {
    DFBResult                                  result;
} CoreLayerRegionFlipUpdateReturn;


/*
 * CoreLayerRegion_FlipUpdateStereo
 */
typedef struct {
    bool                                       left_set;
    DFBRegion                                  left;
    bool                                       right_set;
    DFBRegion                                  right;
    DFBSurfaceFlipFlags                        flags;
} CoreLayerRegionFlipUpdateStereo;

typedef struct {
    DFBResult                                  result;
} CoreLayerRegionFlipUpdateStereoReturn;





class ILayerRegion : public Interface
{
public:
    ILayerRegion( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult GetSurface(
                    CoreSurface                              **ret_surface
    ) = 0;

    virtual DFBResult FlipUpdate(
                    const DFBRegion                           *update,
                    DFBSurfaceFlipFlags                        flags
    ) = 0;

    virtual DFBResult FlipUpdateStereo(
                    const DFBRegion                           *left,
                    const DFBRegion                           *right,
                    DFBSurfaceFlipFlags                        flags
    ) = 0;

};



class ILayerRegion_Real : public ILayerRegion
{
private:
    CoreLayerRegion *obj;

public:
    ILayerRegion_Real( CoreDFB *core, CoreLayerRegion *obj )
        :
        ILayerRegion( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult GetSurface(
                    CoreSurface                              **ret_surface
    );

    virtual DFBResult FlipUpdate(
                    const DFBRegion                           *update,
                    DFBSurfaceFlipFlags                        flags
    );

    virtual DFBResult FlipUpdateStereo(
                    const DFBRegion                           *left,
                    const DFBRegion                           *right,
                    DFBSurfaceFlipFlags                        flags
    );

};



class ILayerRegion_Requestor : public ILayerRegion
{
private:
    CoreLayerRegion *obj;

public:
    ILayerRegion_Requestor( CoreDFB *core, CoreLayerRegion *obj )
        :
        ILayerRegion( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult GetSurface(
                    CoreSurface                              **ret_surface
    );

    virtual DFBResult FlipUpdate(
                    const DFBRegion                           *update,
                    DFBSurfaceFlipFlags                        flags
    );

    virtual DFBResult FlipUpdateStereo(
                    const DFBRegion                           *left,
                    const DFBRegion                           *right,
                    DFBSurfaceFlipFlags                        flags
    );

};


DFBResult CoreLayerRegionDispatch__Dispatch( CoreLayerRegion *obj,
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
