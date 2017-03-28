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

#include "CoreScreen.h"

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

D_DEBUG_DOMAIN( DirectFB_CoreScreen, "DirectFB/CoreScreen", "DirectFB CoreScreen" );

/*********************************************************************************************************************/

DFBResult
CoreScreen_SetPowerMode(
                    CoreScreen                                *obj,
                    DFBScreenPowerMode                         mode
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.SetPowerMode( mode );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.SetPowerMode( mode );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_WaitVSync(
                    CoreScreen                                *obj

)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.WaitVSync(  );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

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

DFBResult
CoreScreen_GetVSyncCount(
                    CoreScreen                                *obj,
                    u64                                       *ret_count
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.GetVSyncCount( ret_count );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.GetVSyncCount( ret_count );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_TestMixerConfig(
                    CoreScreen                                *obj,
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config,
                    DFBScreenMixerConfigFlags                 *ret_failed
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.TestMixerConfig( mixer, config, ret_failed );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.TestMixerConfig( mixer, config, ret_failed );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_SetMixerConfig(
                    CoreScreen                                *obj,
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.SetMixerConfig( mixer, config );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.SetMixerConfig( mixer, config );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_TestEncoderConfig(
                    CoreScreen                                *obj,
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config,
                    DFBScreenEncoderConfigFlags               *ret_failed
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.TestEncoderConfig( encoder, config, ret_failed );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.TestEncoderConfig( encoder, config, ret_failed );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_SetEncoderConfig(
                    CoreScreen                                *obj,
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.SetEncoderConfig( encoder, config );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.SetEncoderConfig( encoder, config );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_TestOutputConfig(
                    CoreScreen                                *obj,
                    u32                                        output,
                    const DFBScreenOutputConfig               *config,
                    DFBScreenOutputConfigFlags                *ret_failed
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.TestOutputConfig( output, config, ret_failed );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.TestOutputConfig( output, config, ret_failed );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_SetOutputConfig(
                    CoreScreen                                *obj,
                    u32                                        output,
                    const DFBScreenOutputConfig               *config
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.SetOutputConfig( output, config );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.SetOutputConfig( output, config );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_GetScreenSize(
                    CoreScreen                                *obj,
                    DFBDimension                              *ret_size
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.GetScreenSize( ret_size );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.GetScreenSize( ret_size );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreScreen_GetLayerDimension(
                    CoreScreen                                *obj,
                    CoreLayer                                 *layer,
                    DFBDimension                              *ret_size
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IScreen_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.GetLayerDimension( layer, ret_size );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IScreen_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.GetLayerDimension( layer, ret_size );
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
CoreScreen_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    CoreScreen *obj = (CoreScreen*) ctx;
    DirectFB::CoreScreenDispatch__Dispatch( obj, caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void CoreScreen_Init_Dispatch(
                    CoreDFB              *core,
                    CoreScreen           *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreScreen_Dispatch, obj, core->world );
}

void  CoreScreen_Deinit_Dispatch(
                    FusionCall           *call
)
{
     fusion_call_destroy( call );
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
IScreen_Requestor::SetPowerMode(
                    DFBScreenPowerMode                         mode
)
{
    DFBResult           ret;
    CoreScreenSetPowerMode       *args = (CoreScreenSetPowerMode*) alloca( sizeof(CoreScreenSetPowerMode) );
    CoreScreenSetPowerModeReturn *return_args = (CoreScreenSetPowerModeReturn*) alloca( sizeof(CoreScreenSetPowerModeReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );


    args->mode = mode;

    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_SetPowerMode, args, sizeof(CoreScreenSetPowerMode), return_args, sizeof(CoreScreenSetPowerModeReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_SetPowerMode ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_SetPowerMode failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IScreen_Requestor::WaitVSync(

)
{
    DFBResult           ret;
    CoreScreenWaitVSync       *args = (CoreScreenWaitVSync*) alloca( sizeof(CoreScreenWaitVSync) );
    CoreScreenWaitVSyncReturn *return_args = (CoreScreenWaitVSyncReturn*) alloca( sizeof(CoreScreenWaitVSyncReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_WaitVSync, args, sizeof(CoreScreenWaitVSync), return_args, sizeof(CoreScreenWaitVSyncReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_WaitVSync ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_WaitVSync failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IScreen_Requestor::GetVSyncCount(
                    u64                                       *ret_count
)
{
    DFBResult           ret;
    CoreScreenGetVSyncCount       *args = (CoreScreenGetVSyncCount*) alloca( sizeof(CoreScreenGetVSyncCount) );
    CoreScreenGetVSyncCountReturn *return_args = (CoreScreenGetVSyncCountReturn*) alloca( sizeof(CoreScreenGetVSyncCountReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_GetVSyncCount, args, sizeof(CoreScreenGetVSyncCount), return_args, sizeof(CoreScreenGetVSyncCountReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_GetVSyncCount ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_GetVSyncCount failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_count = return_args->count;

    return DFB_OK;
}


DFBResult
IScreen_Requestor::TestMixerConfig(
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config,
                    DFBScreenMixerConfigFlags                 *ret_failed
)
{
    DFBResult           ret;
    CoreScreenTestMixerConfig       *args = (CoreScreenTestMixerConfig*) alloca( sizeof(CoreScreenTestMixerConfig) );
    CoreScreenTestMixerConfigReturn *return_args = (CoreScreenTestMixerConfigReturn*) alloca( sizeof(CoreScreenTestMixerConfigReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    args->mixer = mixer;
    args->config = *config;

    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_TestMixerConfig, args, sizeof(CoreScreenTestMixerConfig), return_args, sizeof(CoreScreenTestMixerConfigReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_TestMixerConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_TestMixerConfig failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    if (ret_failed)
        *ret_failed = return_args->failed;

    return DFB_OK;
}


DFBResult
IScreen_Requestor::SetMixerConfig(
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config
)
{
    DFBResult           ret;
    CoreScreenSetMixerConfig       *args = (CoreScreenSetMixerConfig*) alloca( sizeof(CoreScreenSetMixerConfig) );
    CoreScreenSetMixerConfigReturn *return_args = (CoreScreenSetMixerConfigReturn*) alloca( sizeof(CoreScreenSetMixerConfigReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    args->mixer = mixer;
    args->config = *config;

    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_SetMixerConfig, args, sizeof(CoreScreenSetMixerConfig), return_args, sizeof(CoreScreenSetMixerConfigReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_SetMixerConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_SetMixerConfig failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IScreen_Requestor::TestEncoderConfig(
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config,
                    DFBScreenEncoderConfigFlags               *ret_failed
)
{
    DFBResult           ret;
    CoreScreenTestEncoderConfig       *args = (CoreScreenTestEncoderConfig*) alloca( sizeof(CoreScreenTestEncoderConfig) );
    CoreScreenTestEncoderConfigReturn *return_args = (CoreScreenTestEncoderConfigReturn*) alloca( sizeof(CoreScreenTestEncoderConfigReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    args->encoder = encoder;
    args->config = *config;

    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_TestEncoderConfig, args, sizeof(CoreScreenTestEncoderConfig), return_args, sizeof(CoreScreenTestEncoderConfigReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_TestEncoderConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_TestEncoderConfig failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    if (ret_failed)
        *ret_failed = return_args->failed;

    return DFB_OK;
}


DFBResult
IScreen_Requestor::SetEncoderConfig(
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config
)
{
    DFBResult           ret;
    CoreScreenSetEncoderConfig       *args = (CoreScreenSetEncoderConfig*) alloca( sizeof(CoreScreenSetEncoderConfig) );
    CoreScreenSetEncoderConfigReturn *return_args = (CoreScreenSetEncoderConfigReturn*) alloca( sizeof(CoreScreenSetEncoderConfigReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    args->encoder = encoder;
    args->config = *config;

    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_SetEncoderConfig, args, sizeof(CoreScreenSetEncoderConfig), return_args, sizeof(CoreScreenSetEncoderConfigReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_SetEncoderConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_SetEncoderConfig failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IScreen_Requestor::TestOutputConfig(
                    u32                                        output,
                    const DFBScreenOutputConfig               *config,
                    DFBScreenOutputConfigFlags                *ret_failed
)
{
    DFBResult           ret;
    CoreScreenTestOutputConfig       *args = (CoreScreenTestOutputConfig*) alloca( sizeof(CoreScreenTestOutputConfig) );
    CoreScreenTestOutputConfigReturn *return_args = (CoreScreenTestOutputConfigReturn*) alloca( sizeof(CoreScreenTestOutputConfigReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    args->output = output;
    args->config = *config;

    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_TestOutputConfig, args, sizeof(CoreScreenTestOutputConfig), return_args, sizeof(CoreScreenTestOutputConfigReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_TestOutputConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_TestOutputConfig failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    if (ret_failed)
        *ret_failed = return_args->failed;

    return DFB_OK;
}


DFBResult
IScreen_Requestor::SetOutputConfig(
                    u32                                        output,
                    const DFBScreenOutputConfig               *config
)
{
    DFBResult           ret;
    CoreScreenSetOutputConfig       *args = (CoreScreenSetOutputConfig*) alloca( sizeof(CoreScreenSetOutputConfig) );
    CoreScreenSetOutputConfigReturn *return_args = (CoreScreenSetOutputConfigReturn*) alloca( sizeof(CoreScreenSetOutputConfigReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( config != NULL );

    args->output = output;
    args->config = *config;

    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_SetOutputConfig, args, sizeof(CoreScreenSetOutputConfig), return_args, sizeof(CoreScreenSetOutputConfigReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_SetOutputConfig ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_SetOutputConfig failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IScreen_Requestor::GetScreenSize(
                    DFBDimension                              *ret_size
)
{
    DFBResult           ret;
    CoreScreenGetScreenSize       *args = (CoreScreenGetScreenSize*) alloca( sizeof(CoreScreenGetScreenSize) );
    CoreScreenGetScreenSizeReturn *return_args = (CoreScreenGetScreenSizeReturn*) alloca( sizeof(CoreScreenGetScreenSizeReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_size != NULL );


    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_GetScreenSize, args, sizeof(CoreScreenGetScreenSize), return_args, sizeof(CoreScreenGetScreenSizeReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_GetScreenSize ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_GetScreenSize failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_size = return_args->size;

    return DFB_OK;
}


DFBResult
IScreen_Requestor::GetLayerDimension(
                    CoreLayer                                 *layer,
                    DFBDimension                              *ret_size
)
{
    DFBResult           ret;
    CoreScreenGetLayerDimension       *args = (CoreScreenGetLayerDimension*) alloca( sizeof(CoreScreenGetLayerDimension) );
    CoreScreenGetLayerDimensionReturn *return_args = (CoreScreenGetLayerDimensionReturn*) alloca( sizeof(CoreScreenGetLayerDimensionReturn) );

    D_DEBUG_AT( DirectFB_CoreScreen, "IScreen_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( layer != NULL );
    D_ASSERT( ret_size != NULL );

    args->layer_id = CoreLayer_GetID( layer );

    ret = (DFBResult) CoreScreen_Call( obj, FCEF_NONE, CoreScreen_GetLayerDimension, args, sizeof(CoreScreenGetLayerDimension), return_args, sizeof(CoreScreenGetLayerDimensionReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreScreen_Call( CoreScreen_GetLayerDimension ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreScreen_GetLayerDimension failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_size = return_args->size;

    return DFB_OK;
}

/*********************************************************************************************************************/

static DFBResult
__CoreScreenDispatch__Dispatch( CoreScreen *obj,
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


    DirectFB::IScreen_Real real( core_dfb, obj );


    switch (method) {
        case CoreScreen_SetPowerMode: {
            D_UNUSED
            CoreScreenSetPowerMode       *args        = (CoreScreenSetPowerMode *) ptr;
            CoreScreenSetPowerModeReturn *return_args = (CoreScreenSetPowerModeReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_SetPowerMode\n" );

            return_args->result = real.SetPowerMode( args->mode );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenSetPowerModeReturn);

            return DFB_OK;
        }

        case CoreScreen_WaitVSync: {
            D_UNUSED
            CoreScreenWaitVSync       *args        = (CoreScreenWaitVSync *) ptr;
            CoreScreenWaitVSyncReturn *return_args = (CoreScreenWaitVSyncReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_WaitVSync\n" );

            return_args->result = real.WaitVSync(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenWaitVSyncReturn);

            return DFB_OK;
        }

        case CoreScreen_GetVSyncCount: {
            D_UNUSED
            CoreScreenGetVSyncCount       *args        = (CoreScreenGetVSyncCount *) ptr;
            CoreScreenGetVSyncCountReturn *return_args = (CoreScreenGetVSyncCountReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_GetVSyncCount\n" );

            return_args->result = real.GetVSyncCount( &return_args->count );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenGetVSyncCountReturn);

            return DFB_OK;
        }

        case CoreScreen_TestMixerConfig: {
            D_UNUSED
            CoreScreenTestMixerConfig       *args        = (CoreScreenTestMixerConfig *) ptr;
            CoreScreenTestMixerConfigReturn *return_args = (CoreScreenTestMixerConfigReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_TestMixerConfig\n" );

            return_args->result = real.TestMixerConfig( args->mixer, &args->config, &return_args->failed );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenTestMixerConfigReturn);

            return DFB_OK;
        }

        case CoreScreen_SetMixerConfig: {
            D_UNUSED
            CoreScreenSetMixerConfig       *args        = (CoreScreenSetMixerConfig *) ptr;
            CoreScreenSetMixerConfigReturn *return_args = (CoreScreenSetMixerConfigReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_SetMixerConfig\n" );

            return_args->result = real.SetMixerConfig( args->mixer, &args->config );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenSetMixerConfigReturn);

            return DFB_OK;
        }

        case CoreScreen_TestEncoderConfig: {
            D_UNUSED
            CoreScreenTestEncoderConfig       *args        = (CoreScreenTestEncoderConfig *) ptr;
            CoreScreenTestEncoderConfigReturn *return_args = (CoreScreenTestEncoderConfigReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_TestEncoderConfig\n" );

            return_args->result = real.TestEncoderConfig( args->encoder, &args->config, &return_args->failed );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenTestEncoderConfigReturn);

            return DFB_OK;
        }

        case CoreScreen_SetEncoderConfig: {
            D_UNUSED
            CoreScreenSetEncoderConfig       *args        = (CoreScreenSetEncoderConfig *) ptr;
            CoreScreenSetEncoderConfigReturn *return_args = (CoreScreenSetEncoderConfigReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_SetEncoderConfig\n" );

            return_args->result = real.SetEncoderConfig( args->encoder, &args->config );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenSetEncoderConfigReturn);

            return DFB_OK;
        }

        case CoreScreen_TestOutputConfig: {
            D_UNUSED
            CoreScreenTestOutputConfig       *args        = (CoreScreenTestOutputConfig *) ptr;
            CoreScreenTestOutputConfigReturn *return_args = (CoreScreenTestOutputConfigReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_TestOutputConfig\n" );

            return_args->result = real.TestOutputConfig( args->output, &args->config, &return_args->failed );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenTestOutputConfigReturn);

            return DFB_OK;
        }

        case CoreScreen_SetOutputConfig: {
            D_UNUSED
            CoreScreenSetOutputConfig       *args        = (CoreScreenSetOutputConfig *) ptr;
            CoreScreenSetOutputConfigReturn *return_args = (CoreScreenSetOutputConfigReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_SetOutputConfig\n" );

            return_args->result = real.SetOutputConfig( args->output, &args->config );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenSetOutputConfigReturn);

            return DFB_OK;
        }

        case CoreScreen_GetScreenSize: {
            D_UNUSED
            CoreScreenGetScreenSize       *args        = (CoreScreenGetScreenSize *) ptr;
            CoreScreenGetScreenSizeReturn *return_args = (CoreScreenGetScreenSizeReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_GetScreenSize\n" );

            return_args->result = real.GetScreenSize( &return_args->size );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenGetScreenSizeReturn);

            return DFB_OK;
        }

        case CoreScreen_GetLayerDimension: {
    CoreLayer *layer = NULL;
            D_UNUSED
            CoreScreenGetLayerDimension       *args        = (CoreScreenGetLayerDimension *) ptr;
            CoreScreenGetLayerDimensionReturn *return_args = (CoreScreenGetLayerDimensionReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreScreen, "=-> CoreScreen_GetLayerDimension\n" );

            ret = (DFBResult) CoreLayer_Lookup( core_dfb, args->layer_id, caller, &layer );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up layer by ID %u failed!\n", __FUNCTION__, args->layer_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real.GetLayerDimension( layer, &return_args->size );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreScreenGetLayerDimensionReturn);

            if (layer)
                CoreLayer_Unref( layer );

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}
/*********************************************************************************************************************/

DFBResult
CoreScreenDispatch__Dispatch( CoreScreen *obj,
                                FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreScreen, "CoreScreenDispatch::%s( %p )\n", __FUNCTION__, obj );

    Core_PushIdentity( caller );

    ret = __CoreScreenDispatch__Dispatch( obj, caller, method, ptr, length, ret_ptr, ret_size, ret_length );

    Core_PopIdentity();

    return ret;
}

}
