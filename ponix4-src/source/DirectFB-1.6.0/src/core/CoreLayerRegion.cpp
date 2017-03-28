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

#include <config.h>

#include "CoreLayerRegion.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>

#include <core/CoreDFB_CallMode.h>
}

D_DEBUG_DOMAIN( DirectFB_CoreLayerRegion, "DirectFB/CoreLayerRegion", "DirectFB CoreLayerRegion" );

/*********************************************************************************************************************/

DFBResult
CoreLayerRegion_GetSurface(
                    CoreLayerRegion                           *obj,
                    CoreSurface                              **ret_surface
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayerRegion_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.GetSurface( ret_surface );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayerRegion_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.GetSurface( ret_surface );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreLayerRegion_FlipUpdate(
                    CoreLayerRegion                           *obj,
                    const DFBRegion                           *update,
                    DFBSurfaceFlipFlags                        flags
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayerRegion_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.FlipUpdate( update, flags );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayerRegion_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.FlipUpdate( update, flags );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreLayerRegion_FlipUpdateStereo(
                    CoreLayerRegion                           *obj,
                    const DFBRegion                           *left,
                    const DFBRegion                           *right,
                    DFBSurfaceFlipFlags                        flags
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayerRegion_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.FlipUpdateStereo( left, right, flags );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayerRegion_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.FlipUpdateStereo( left, right, flags );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
CoreLayerRegion_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    CoreLayerRegion *obj = (CoreLayerRegion*) ctx;
    DirectFB::CoreLayerRegionDispatch__Dispatch( obj, caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void CoreLayerRegion_Init_Dispatch(
                    CoreDFB              *core,
                    CoreLayerRegion      *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreLayerRegion_Dispatch, obj, core->world );
}

void  CoreLayerRegion_Deinit_Dispatch(
                    FusionCall           *call
)
{
     fusion_call_destroy( call );
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ILayerRegion_Requestor::GetSurface(
                    CoreSurface                              **ret_surface
)
{
    DFBResult           ret;
    CoreSurface *surface = NULL;
    CoreLayerRegionGetSurface       *args = (CoreLayerRegionGetSurface*) alloca( sizeof(CoreLayerRegionGetSurface) );
    CoreLayerRegionGetSurfaceReturn *return_args = (CoreLayerRegionGetSurfaceReturn*) alloca( sizeof(CoreLayerRegionGetSurfaceReturn) );

    D_DEBUG_AT( DirectFB_CoreLayerRegion, "ILayerRegion_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_surface != NULL );


    ret = (DFBResult) CoreLayerRegion_Call( obj, FCEF_NONE, CoreLayerRegion_GetSurface, args, sizeof(CoreLayerRegionGetSurface), return_args, sizeof(CoreLayerRegionGetSurfaceReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayerRegion_Call( CoreLayerRegion_GetSurface ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayerRegion_GetSurface failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    ret = (DFBResult) CoreSurface_Catch( core, return_args->surface_ptr, &surface );
    if (ret) {
         D_DERROR( ret, "%s: Catching surface by ID %u failed!\n", __FUNCTION__, return_args->surface_id );
         return ret;
    }

    *ret_surface = surface;

    return DFB_OK;
}


DFBResult
ILayerRegion_Requestor::FlipUpdate(
                    const DFBRegion                           *update,
                    DFBSurfaceFlipFlags                        flags
)
{
    DFBResult           ret;
    CoreLayerRegionFlipUpdate       *args = (CoreLayerRegionFlipUpdate*) alloca( sizeof(CoreLayerRegionFlipUpdate) );
    CoreLayerRegionFlipUpdateReturn *return_args = (CoreLayerRegionFlipUpdateReturn*) alloca( sizeof(CoreLayerRegionFlipUpdateReturn) );

    D_DEBUG_AT( DirectFB_CoreLayerRegion, "ILayerRegion_Requestor::%s()\n", __FUNCTION__ );


  if (update) {
    args->update = *update;
    args->update_set = true;
  }
  else
    args->update_set = false;
    args->flags = flags;

    ret = (DFBResult) CoreLayerRegion_Call( obj, FCEF_NONE, CoreLayerRegion_FlipUpdate, args, sizeof(CoreLayerRegionFlipUpdate), return_args, sizeof(CoreLayerRegionFlipUpdateReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayerRegion_Call( CoreLayerRegion_FlipUpdate ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayerRegion_FlipUpdate failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
ILayerRegion_Requestor::FlipUpdateStereo(
                    const DFBRegion                           *left,
                    const DFBRegion                           *right,
                    DFBSurfaceFlipFlags                        flags
)
{
    DFBResult           ret;
    CoreLayerRegionFlipUpdateStereo       *args = (CoreLayerRegionFlipUpdateStereo*) alloca( sizeof(CoreLayerRegionFlipUpdateStereo) );
    CoreLayerRegionFlipUpdateStereoReturn *return_args = (CoreLayerRegionFlipUpdateStereoReturn*) alloca( sizeof(CoreLayerRegionFlipUpdateStereoReturn) );

    D_DEBUG_AT( DirectFB_CoreLayerRegion, "ILayerRegion_Requestor::%s()\n", __FUNCTION__ );


  if (left) {
    args->left = *left;
    args->left_set = true;
  }
  else
    args->left_set = false;
  if (right) {
    args->right = *right;
    args->right_set = true;
  }
  else
    args->right_set = false;
    args->flags = flags;

    ret = (DFBResult) CoreLayerRegion_Call( obj, FCEF_NONE, CoreLayerRegion_FlipUpdateStereo, args, sizeof(CoreLayerRegionFlipUpdateStereo), return_args, sizeof(CoreLayerRegionFlipUpdateStereoReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayerRegion_Call( CoreLayerRegion_FlipUpdateStereo ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayerRegion_FlipUpdateStereo failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

static DFBResult
__CoreLayerRegionDispatch__Dispatch( CoreLayerRegion *obj,
                                FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    D_UNUSED
    DFBResult ret;


    DirectFB::ILayerRegion_Real real( core_dfb, obj );


    switch (method) {
        case CoreLayerRegion_GetSurface: {
    CoreSurface *surface = NULL;
            D_UNUSED
            CoreLayerRegionGetSurface       *args        = (CoreLayerRegionGetSurface *) ptr;
            CoreLayerRegionGetSurfaceReturn *return_args = (CoreLayerRegionGetSurfaceReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayerRegion, "=-> CoreLayerRegion_GetSurface\n" );

            return_args->result = real.GetSurface( &surface );
            if (return_args->result == DFB_OK) {
                CoreSurface_Throw( surface, caller, &return_args->surface_id );
                return_args->surface_ptr = (void*) surface;
            }

            *ret_length = sizeof(CoreLayerRegionGetSurfaceReturn);

            return DFB_OK;
        }

        case CoreLayerRegion_FlipUpdate: {
            D_UNUSED
            CoreLayerRegionFlipUpdate       *args        = (CoreLayerRegionFlipUpdate *) ptr;
            CoreLayerRegionFlipUpdateReturn *return_args = (CoreLayerRegionFlipUpdateReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayerRegion, "=-> CoreLayerRegion_FlipUpdate\n" );

            return_args->result = real.FlipUpdate( args->update_set ? &args->update : NULL, args->flags );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreLayerRegionFlipUpdateReturn);

            return DFB_OK;
        }

        case CoreLayerRegion_FlipUpdateStereo: {
            D_UNUSED
            CoreLayerRegionFlipUpdateStereo       *args        = (CoreLayerRegionFlipUpdateStereo *) ptr;
            CoreLayerRegionFlipUpdateStereoReturn *return_args = (CoreLayerRegionFlipUpdateStereoReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayerRegion, "=-> CoreLayerRegion_FlipUpdateStereo\n" );

            return_args->result = real.FlipUpdateStereo( args->left_set ? &args->left : NULL, args->right_set ? &args->right : NULL, args->flags );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreLayerRegionFlipUpdateStereoReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}
/*********************************************************************************************************************/

DFBResult
CoreLayerRegionDispatch__Dispatch( CoreLayerRegion *obj,
                                FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreLayerRegion, "CoreLayerRegionDispatch::%s( %p )\n", __FUNCTION__, obj );

    Core_PushIdentity( caller );

    ret = __CoreLayerRegionDispatch__Dispatch( obj, caller, method, ptr, length, ret_ptr, ret_size, ret_length );

    Core_PopIdentity();

    return ret;
}

}
