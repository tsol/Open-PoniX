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

#include "CoreSlave.h"

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

D_DEBUG_DOMAIN( DirectFB_CoreSlave, "DirectFB/CoreSlave", "DirectFB CoreSlave" );

/*********************************************************************************************************************/

DFBResult
CoreSlave_GetData(
                    CoreSlave                                 *obj,
                    void*                                      address,
                    u32                                        bytes,
                    u8                                        *ret_data
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ICoreSlave_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.GetData( address, bytes, ret_data );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreSlave_PutData(
                    CoreSlave                                 *obj,
                    void*                                      address,
                    u32                                        bytes,
                    const u8                                  *data
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ICoreSlave_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.PutData( address, bytes, data );
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
CoreSlave_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    CoreDFB *obj = (CoreDFB*) ctx;
    DirectFB::CoreSlaveDispatch__Dispatch( obj, caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void CoreSlave_Init_Dispatch(
                    CoreDFB              *core,
                    CoreDFB              *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreSlave_Dispatch, obj, core->world );
}

void  CoreSlave_Deinit_Dispatch(
                    FusionCall           *call
)
{
     fusion_call_destroy( call );
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ICoreSlave_Requestor::GetData(
                    void*                                      address,
                    u32                                        bytes,
                    u8                                        *ret_data
)
{
    DFBResult           ret;
    CoreSlaveGetData       *args = (CoreSlaveGetData*) alloca( sizeof(CoreSlaveGetData) );
    CoreSlaveGetDataReturn *return_args = (CoreSlaveGetDataReturn*) alloca( sizeof(CoreSlaveGetDataReturn) + bytes * sizeof(u8) );

    D_DEBUG_AT( DirectFB_CoreSlave, "ICoreSlave_Requestor::%s()\n", __FUNCTION__ );


    args->address = address;
    args->bytes = bytes;

    ret = (DFBResult) CoreSlave_Call( obj, FCEF_NONE, CoreSlave_GetData, args, sizeof(CoreSlaveGetData), return_args, sizeof(CoreSlaveGetDataReturn) + bytes * sizeof(u8), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSlave_Call( CoreSlave_GetData ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreSlave_GetData failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    direct_memcpy( ret_data, (char*) (return_args + 1), args->bytes * sizeof(u8) );

    return DFB_OK;
}


DFBResult
ICoreSlave_Requestor::PutData(
                    void*                                      address,
                    u32                                        bytes,
                    const u8                                  *data
)
{
    DFBResult           ret;
    CoreSlavePutData       *args = (CoreSlavePutData*) alloca( sizeof(CoreSlavePutData) + bytes * sizeof(u8) );
    CoreSlavePutDataReturn *return_args = (CoreSlavePutDataReturn*) alloca( sizeof(CoreSlavePutDataReturn) );

    D_DEBUG_AT( DirectFB_CoreSlave, "ICoreSlave_Requestor::%s()\n", __FUNCTION__ );


    args->address = address;
    args->bytes = bytes;
    direct_memcpy( (char*) (args + 1), data, bytes * sizeof(u8) );

    ret = (DFBResult) CoreSlave_Call( obj, FCEF_NONE, CoreSlave_PutData, args, sizeof(CoreSlavePutData) + bytes * sizeof(u8), return_args, sizeof(CoreSlavePutDataReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreSlave_Call( CoreSlave_PutData ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreSlave_PutData failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

static DFBResult
__CoreSlaveDispatch__Dispatch( CoreDFB *obj,
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


    DirectFB::ICoreSlave_Real real( core_dfb, obj );


    switch (method) {
        case CoreSlave_GetData: {
            D_UNUSED
            CoreSlaveGetData       *args        = (CoreSlaveGetData *) ptr;
            CoreSlaveGetDataReturn *return_args = (CoreSlaveGetDataReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSlave, "=-> CoreSlave_GetData\n" );

            return_args->result = real.GetData( args->address, args->bytes, (u8*) ((char*)(return_args + 1)) );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSlaveGetDataReturn) + args->bytes * sizeof(u8);

            return DFB_OK;
        }

        case CoreSlave_PutData: {
            D_UNUSED
            CoreSlavePutData       *args        = (CoreSlavePutData *) ptr;
            CoreSlavePutDataReturn *return_args = (CoreSlavePutDataReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreSlave, "=-> CoreSlave_PutData\n" );

            return_args->result = real.PutData( args->address, args->bytes, (u8*) ((char*)(args + 1)) );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreSlavePutDataReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}
/*********************************************************************************************************************/

DFBResult
CoreSlaveDispatch__Dispatch( CoreDFB *obj,
                                FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreSlave, "CoreSlaveDispatch::%s( %p )\n", __FUNCTION__, obj );

    Core_PushIdentity( caller );

    ret = __CoreSlaveDispatch__Dispatch( obj, caller, method, ptr, length, ret_ptr, ret_size, ret_length );

    Core_PopIdentity();

    return ret;
}

}
