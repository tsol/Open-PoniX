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

#include "CoreSurfaceClient.h"

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

D_DEBUG_DOMAIN( DirectFB_CoreSurfaceClient, "DirectFB/CoreSurfaceClient", "DirectFB CoreSurfaceClient" );

/*********************************************************************************************************************/

DFBResult
CoreSurfaceClient_FrameAck(
                    CoreSurfaceClient                         *obj,
                    u32                                        flip_count
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ISurfaceClient_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.FrameAck( flip_count );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ISurfaceClient_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.FrameAck( flip_count );
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
CoreSurfaceClient_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    CoreSurfaceClient *obj = (CoreSurfaceClient*) ctx;
    DirectFB::CoreSurfaceClientDispatch__Dispatch( obj, caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void CoreSurfaceClient_Init_Dispatch(
                    CoreDFB              *core,
                    CoreSurfaceClient    *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreSurfaceClient_Dispatch, obj, core->world );
}

void  CoreSurfaceClient_Deinit_Dispatch(
                    FusionCall           *call
)
{
     fusion_call_destroy( call );
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ISurfaceClient_Requestor::FrameAck(
                    u32                                        flip_count
)
{
    DFBResult           ret;
    CoreSurfaceClientFrameAck       *args = (CoreSurfaceClientFrameAck*) alloca( sizeof(CoreSurfaceClientFrameAck) );
    CoreSurfaceClientFrameAckReturn *return_args = (CoreSurfaceClientFrameAckReturn*) alloca( sizeof(CoreSurfaceClientFrameAckReturn) );

    D_DEBUG_AT( DirectFB_CoreSurfaceClient, "ISurfaceClient_Requestor::%s()\n", __FUNCTION__ );


    args->flip_count = flip_count;

    ret = (DFBResult) CoreSurfaceClient_Call( obj, FCEF_NONE, CoreSurfaceClient_FrameAck, args, sizeof(CoreSurfaceClientFrameAck), return_args, sizeof(CoreSurfaceClientFrameAckReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSurfaceClient_Call( CoreSurfaceClient_FrameAck ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreSurfaceClient_FrameAck failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

static DFBResult
__CoreSurfaceClientDispatch__Dispatch( CoreSurfaceClient *obj,
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


    DirectFB::ISurfaceClient_Real real( core_dfb, obj );


    switch (method) {
        case CoreSurfaceClient_FrameAck: {
            D_UNUSED
            CoreSurfaceClientFrameAck       *args        = (CoreSurfaceClientFrameAck *) ptr;
            CoreSurfaceClientFrameAckReturn *return_args = (CoreSurfaceClientFrameAckReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSurfaceClient, "=-> CoreSurfaceClient_FrameAck\n" );

            return_args->result = real.FrameAck( args->flip_count );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSurfaceClientFrameAckReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}
/*********************************************************************************************************************/

DFBResult
CoreSurfaceClientDispatch__Dispatch( CoreSurfaceClient *obj,
                                FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreSurfaceClient, "CoreSurfaceClientDispatch::%s( %p )\n", __FUNCTION__, obj );

    Core_PushIdentity( caller );

    ret = __CoreSurfaceClientDispatch__Dispatch( obj, caller, method, ptr, length, ret_ptr, ret_size, ret_length );

    Core_PopIdentity();

    return ret;
}

}
