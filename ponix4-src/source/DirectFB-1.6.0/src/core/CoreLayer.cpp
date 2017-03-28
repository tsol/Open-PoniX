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

#include "CoreLayer.h"

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

D_DEBUG_DOMAIN( DirectFB_CoreLayer, "DirectFB/CoreLayer", "DirectFB CoreLayer" );

/*********************************************************************************************************************/

DFBResult
CoreLayer_CreateContext(
                    CoreLayer                                 *obj,
                    CoreLayerContext                         **ret_context
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayer_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.CreateContext( ret_context );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayer_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.CreateContext( ret_context );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreLayer_GetPrimaryContext(
                    CoreLayer                                 *obj,
                    bool                                       activate,
                    CoreLayerContext                         **ret_context
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayer_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.GetPrimaryContext( activate, ret_context );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayer_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.GetPrimaryContext( activate, ret_context );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreLayer_ActivateContext(
                    CoreLayer                                 *obj,
                    CoreLayerContext                          *context
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayer_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.ActivateContext( context );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayer_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.ActivateContext( context );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreLayer_GetCurrentOutputField(
                    CoreLayer                                 *obj,
                    s32                                       *ret_field
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayer_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.GetCurrentOutputField( ret_field );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayer_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.GetCurrentOutputField( ret_field );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreLayer_SetLevel(
                    CoreLayer                                 *obj,
                    s32                                        level
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayer_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.SetLevel( level );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayer_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.SetLevel( level );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreLayer_WaitVSync(
                    CoreLayer                                 *obj

)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::ILayer_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.WaitVSync(  );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::ILayer_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.WaitVSync(  );
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
CoreLayer_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    CoreLayer *obj = (CoreLayer*) ctx;
    DirectFB::CoreLayerDispatch__Dispatch( obj, caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void CoreLayer_Init_Dispatch(
                    CoreDFB              *core,
                    CoreLayer            *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreLayer_Dispatch, obj, core->world );
}

void  CoreLayer_Deinit_Dispatch(
                    FusionCall           *call
)
{
     fusion_call_destroy( call );
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
ILayer_Requestor::CreateContext(
                    CoreLayerContext                         **ret_context
)
{
    DFBResult           ret;
    CoreLayerContext *context = NULL;
    CoreLayerCreateContext       *args = (CoreLayerCreateContext*) alloca( sizeof(CoreLayerCreateContext) );
    CoreLayerCreateContextReturn *return_args = (CoreLayerCreateContextReturn*) alloca( sizeof(CoreLayerCreateContextReturn) );

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_context != NULL );


    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_CreateContext, args, sizeof(CoreLayerCreateContext), return_args, sizeof(CoreLayerCreateContextReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_CreateContext ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayer_CreateContext failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    ret = (DFBResult) CoreLayerContext_Catch( core, return_args->context_ptr, &context );
    if (ret) {
         D_DERROR( ret, "%s: Catching context by ID %u failed!\n", __FUNCTION__, return_args->context_id );
         return ret;
    }

    *ret_context = context;

    return DFB_OK;
}


DFBResult
ILayer_Requestor::GetPrimaryContext(
                    bool                                       activate,
                    CoreLayerContext                         **ret_context
)
{
    DFBResult           ret;
    CoreLayerContext *context = NULL;
    CoreLayerGetPrimaryContext       *args = (CoreLayerGetPrimaryContext*) alloca( sizeof(CoreLayerGetPrimaryContext) );
    CoreLayerGetPrimaryContextReturn *return_args = (CoreLayerGetPrimaryContextReturn*) alloca( sizeof(CoreLayerGetPrimaryContextReturn) );

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_context != NULL );

    args->activate = activate;

    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_GetPrimaryContext, args, sizeof(CoreLayerGetPrimaryContext), return_args, sizeof(CoreLayerGetPrimaryContextReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_GetPrimaryContext ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayer_GetPrimaryContext failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    ret = (DFBResult) CoreLayerContext_Catch( core, return_args->context_ptr, &context );
    if (ret) {
         D_DERROR( ret, "%s: Catching context by ID %u failed!\n", __FUNCTION__, return_args->context_id );
         return ret;
    }

    *ret_context = context;

    return DFB_OK;
}


DFBResult
ILayer_Requestor::ActivateContext(
                    CoreLayerContext                          *context
)
{
    DFBResult           ret;
    CoreLayerActivateContext       *args = (CoreLayerActivateContext*) alloca( sizeof(CoreLayerActivateContext) );
    CoreLayerActivateContextReturn *return_args = (CoreLayerActivateContextReturn*) alloca( sizeof(CoreLayerActivateContextReturn) );

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( context != NULL );

    args->context_id = CoreLayerContext_GetID( context );

    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_ActivateContext, args, sizeof(CoreLayerActivateContext), return_args, sizeof(CoreLayerActivateContextReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_ActivateContext ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayer_ActivateContext failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
ILayer_Requestor::GetCurrentOutputField(
                    s32                                       *ret_field
)
{
    DFBResult           ret;
    CoreLayerGetCurrentOutputField       *args = (CoreLayerGetCurrentOutputField*) alloca( sizeof(CoreLayerGetCurrentOutputField) );
    CoreLayerGetCurrentOutputFieldReturn *return_args = (CoreLayerGetCurrentOutputFieldReturn*) alloca( sizeof(CoreLayerGetCurrentOutputFieldReturn) );

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_GetCurrentOutputField, args, sizeof(CoreLayerGetCurrentOutputField), return_args, sizeof(CoreLayerGetCurrentOutputFieldReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_GetCurrentOutputField ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayer_GetCurrentOutputField failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_field = return_args->field;

    return DFB_OK;
}


DFBResult
ILayer_Requestor::SetLevel(
                    s32                                        level
)
{
    DFBResult           ret;
    CoreLayerSetLevel       *args = (CoreLayerSetLevel*) alloca( sizeof(CoreLayerSetLevel) );
    CoreLayerSetLevelReturn *return_args = (CoreLayerSetLevelReturn*) alloca( sizeof(CoreLayerSetLevelReturn) );

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );


    args->level = level;

    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_SetLevel, args, sizeof(CoreLayerSetLevel), return_args, sizeof(CoreLayerSetLevelReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_SetLevel ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayer_SetLevel failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
ILayer_Requestor::WaitVSync(

)
{
    DFBResult           ret;
    CoreLayerWaitVSync       *args = (CoreLayerWaitVSync*) alloca( sizeof(CoreLayerWaitVSync) );
    CoreLayerWaitVSyncReturn *return_args = (CoreLayerWaitVSyncReturn*) alloca( sizeof(CoreLayerWaitVSyncReturn) );

    D_DEBUG_AT( DirectFB_CoreLayer, "ILayer_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreLayer_Call( obj, FCEF_NONE, CoreLayer_WaitVSync, args, sizeof(CoreLayerWaitVSync), return_args, sizeof(CoreLayerWaitVSyncReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreLayer_Call( CoreLayer_WaitVSync ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreLayer_WaitVSync failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

static DFBResult
__CoreLayerDispatch__Dispatch( CoreLayer *obj,
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


    DirectFB::ILayer_Real real( core_dfb, obj );


    switch (method) {
        case CoreLayer_CreateContext: {
    CoreLayerContext *context = NULL;
            D_UNUSED
            CoreLayerCreateContext       *args        = (CoreLayerCreateContext *) ptr;
            CoreLayerCreateContextReturn *return_args = (CoreLayerCreateContextReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_CreateContext\n" );

            return_args->result = real.CreateContext( &context );
            if (return_args->result == DFB_OK) {
                CoreLayerContext_Throw( context, caller, &return_args->context_id );
                return_args->context_ptr = (void*) context;
            }

            *ret_length = sizeof(CoreLayerCreateContextReturn);

            return DFB_OK;
        }

        case CoreLayer_GetPrimaryContext: {
    CoreLayerContext *context = NULL;
            D_UNUSED
            CoreLayerGetPrimaryContext       *args        = (CoreLayerGetPrimaryContext *) ptr;
            CoreLayerGetPrimaryContextReturn *return_args = (CoreLayerGetPrimaryContextReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_GetPrimaryContext\n" );

            return_args->result = real.GetPrimaryContext( args->activate, &context );
            if (return_args->result == DFB_OK) {
                CoreLayerContext_Throw( context, caller, &return_args->context_id );
                return_args->context_ptr = (void*) context;
            }

            *ret_length = sizeof(CoreLayerGetPrimaryContextReturn);

            return DFB_OK;
        }

        case CoreLayer_ActivateContext: {
    CoreLayerContext *context = NULL;
            D_UNUSED
            CoreLayerActivateContext       *args        = (CoreLayerActivateContext *) ptr;
            CoreLayerActivateContextReturn *return_args = (CoreLayerActivateContextReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_ActivateContext\n" );

            ret = (DFBResult) CoreLayerContext_Lookup( core_dfb, args->context_id, caller, &context );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up context by ID %u failed!\n", __FUNCTION__, args->context_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real.ActivateContext( context );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreLayerActivateContextReturn);

            if (context)
                CoreLayerContext_Unref( context );

            return DFB_OK;
        }

        case CoreLayer_GetCurrentOutputField: {
            D_UNUSED
            CoreLayerGetCurrentOutputField       *args        = (CoreLayerGetCurrentOutputField *) ptr;
            CoreLayerGetCurrentOutputFieldReturn *return_args = (CoreLayerGetCurrentOutputFieldReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_GetCurrentOutputField\n" );

            return_args->result = real.GetCurrentOutputField( &return_args->field );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreLayerGetCurrentOutputFieldReturn);

            return DFB_OK;
        }

        case CoreLayer_SetLevel: {
            D_UNUSED
            CoreLayerSetLevel       *args        = (CoreLayerSetLevel *) ptr;
            CoreLayerSetLevelReturn *return_args = (CoreLayerSetLevelReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_SetLevel\n" );

            return_args->result = real.SetLevel( args->level );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreLayerSetLevelReturn);

            return DFB_OK;
        }

        case CoreLayer_WaitVSync: {
            D_UNUSED
            CoreLayerWaitVSync       *args        = (CoreLayerWaitVSync *) ptr;
            CoreLayerWaitVSyncReturn *return_args = (CoreLayerWaitVSyncReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreLayer, "=-> CoreLayer_WaitVSync\n" );

            return_args->result = real.WaitVSync(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreLayerWaitVSyncReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}
/*********************************************************************************************************************/

DFBResult
CoreLayerDispatch__Dispatch( CoreLayer *obj,
                                FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreLayer, "CoreLayerDispatch::%s( %p )\n", __FUNCTION__, obj );

    Core_PushIdentity( caller );

    ret = __CoreLayerDispatch__Dispatch( obj, caller, method, ptr, length, ret_ptr, ret_size, ret_length );

    Core_PopIdentity();

    return ret;
}

}
