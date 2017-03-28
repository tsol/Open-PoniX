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

#include "CoreWindowStack.h"

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

D_DEBUG_DOMAIN( DirectFB_CoreWindowStack, "DirectFB/CoreWindowStack", "DirectFB CoreWindowStack" );

/*********************************************************************************************************************/

DFBResult
CoreWindowStack_RepaintAll(
                    CoreWindowStack                           *obj

)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.RepaintAll(  );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.RepaintAll(  );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_GetInsets(
                    CoreWindowStack                           *obj,
                    CoreWindow                                *window,
                    DFBInsets                                 *ret_insets
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.GetInsets( window, ret_insets );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.GetInsets( window, ret_insets );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_CursorEnable(
                    CoreWindowStack                           *obj,
                    bool                                       enable
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.CursorEnable( enable );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.CursorEnable( enable );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_CursorSetShape(
                    CoreWindowStack                           *obj,
                    CoreSurface                               *shape,
                    const DFBPoint                            *hotspot
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.CursorSetShape( shape, hotspot );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.CursorSetShape( shape, hotspot );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_CursorSetOpacity(
                    CoreWindowStack                           *obj,
                    u8                                         opacity
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.CursorSetOpacity( opacity );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.CursorSetOpacity( opacity );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_CursorSetAcceleration(
                    CoreWindowStack                           *obj,
                    u32                                        numerator,
                    u32                                        denominator,
                    u32                                        threshold
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.CursorSetAcceleration( numerator, denominator, threshold );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.CursorSetAcceleration( numerator, denominator, threshold );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_CursorWarp(
                    CoreWindowStack                           *obj,
                    const DFBPoint                            *position
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.CursorWarp( position );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.CursorWarp( position );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_CursorGetPosition(
                    CoreWindowStack                           *obj,
                    DFBPoint                                  *ret_position
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.CursorGetPosition( ret_position );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.CursorGetPosition( ret_position );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_BackgroundSetMode(
                    CoreWindowStack                           *obj,
                    DFBDisplayLayerBackgroundMode              mode
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.BackgroundSetMode( mode );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.BackgroundSetMode( mode );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_BackgroundSetImage(
                    CoreWindowStack                           *obj,
                    CoreSurface                               *image
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.BackgroundSetImage( image );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.BackgroundSetImage( image );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_BackgroundSetColor(
                    CoreWindowStack                           *obj,
                    const DFBColor                            *color
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.BackgroundSetColor( color );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.BackgroundSetColor( color );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_DENY:
            return DFB_DEAD;
    }

    return DFB_UNIMPLEMENTED;
}

DFBResult
CoreWindowStack_BackgroundSetColorIndex(
                    CoreWindowStack                           *obj,
                    s32                                        index
)
{
    DFBResult ret;

    switch (CoreDFB_CallMode( core_dfb )) {
        case COREDFB_CALL_DIRECT:{
            DirectFB::IWindowStack_Real real( core_dfb, obj );

            Core_PushCalling();
            ret = real.BackgroundSetColorIndex( index );
            Core_PopCalling();

            return ret;
        }
        case COREDFB_CALL_INDIRECT: {
            DirectFB::IWindowStack_Requestor requestor( core_dfb, obj );

            Core_PushCalling();
            ret = requestor.BackgroundSetColorIndex( index );
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
CoreWindowStack_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    CoreWindowStack *obj = (CoreWindowStack*) ctx;
    DirectFB::CoreWindowStackDispatch__Dispatch( obj, caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void CoreWindowStack_Init_Dispatch(
                    CoreDFB              *core,
                    CoreWindowStack      *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, CoreWindowStack_Dispatch, obj, core->world );
}

void  CoreWindowStack_Deinit_Dispatch(
                    FusionCall           *call
)
{
     fusion_call_destroy( call );
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
IWindowStack_Requestor::RepaintAll(

)
{
    DFBResult           ret;
    CoreWindowStackRepaintAll       *args = (CoreWindowStackRepaintAll*) alloca( sizeof(CoreWindowStackRepaintAll) );
    CoreWindowStackRepaintAllReturn *return_args = (CoreWindowStackRepaintAllReturn*) alloca( sizeof(CoreWindowStackRepaintAllReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_RepaintAll, args, sizeof(CoreWindowStackRepaintAll), return_args, sizeof(CoreWindowStackRepaintAllReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_RepaintAll ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_RepaintAll failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::GetInsets(
                    CoreWindow                                *window,
                    DFBInsets                                 *ret_insets
)
{
    DFBResult           ret;
    CoreWindowStackGetInsets       *args = (CoreWindowStackGetInsets*) alloca( sizeof(CoreWindowStackGetInsets) );
    CoreWindowStackGetInsetsReturn *return_args = (CoreWindowStackGetInsetsReturn*) alloca( sizeof(CoreWindowStackGetInsetsReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( window != NULL );
    D_ASSERT( ret_insets != NULL );

    args->window_id = CoreWindow_GetID( window );

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_GetInsets, args, sizeof(CoreWindowStackGetInsets), return_args, sizeof(CoreWindowStackGetInsetsReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_GetInsets ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_GetInsets failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_insets = return_args->insets;

    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorEnable(
                    bool                                       enable
)
{
    DFBResult           ret;
    CoreWindowStackCursorEnable       *args = (CoreWindowStackCursorEnable*) alloca( sizeof(CoreWindowStackCursorEnable) );
    CoreWindowStackCursorEnableReturn *return_args = (CoreWindowStackCursorEnableReturn*) alloca( sizeof(CoreWindowStackCursorEnableReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    args->enable = enable;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorEnable, args, sizeof(CoreWindowStackCursorEnable), return_args, sizeof(CoreWindowStackCursorEnableReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorEnable ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_CursorEnable failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorSetShape(
                    CoreSurface                               *shape,
                    const DFBPoint                            *hotspot
)
{
    DFBResult           ret;
    CoreWindowStackCursorSetShape       *args = (CoreWindowStackCursorSetShape*) alloca( sizeof(CoreWindowStackCursorSetShape) );
    CoreWindowStackCursorSetShapeReturn *return_args = (CoreWindowStackCursorSetShapeReturn*) alloca( sizeof(CoreWindowStackCursorSetShapeReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( shape != NULL );
    D_ASSERT( hotspot != NULL );

    args->shape_id = CoreSurface_GetID( shape );
    args->hotspot = *hotspot;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorSetShape, args, sizeof(CoreWindowStackCursorSetShape), return_args, sizeof(CoreWindowStackCursorSetShapeReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorSetShape ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_CursorSetShape failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorSetOpacity(
                    u8                                         opacity
)
{
    DFBResult           ret;
    CoreWindowStackCursorSetOpacity       *args = (CoreWindowStackCursorSetOpacity*) alloca( sizeof(CoreWindowStackCursorSetOpacity) );
    CoreWindowStackCursorSetOpacityReturn *return_args = (CoreWindowStackCursorSetOpacityReturn*) alloca( sizeof(CoreWindowStackCursorSetOpacityReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    args->opacity = opacity;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorSetOpacity, args, sizeof(CoreWindowStackCursorSetOpacity), return_args, sizeof(CoreWindowStackCursorSetOpacityReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorSetOpacity ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_CursorSetOpacity failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorSetAcceleration(
                    u32                                        numerator,
                    u32                                        denominator,
                    u32                                        threshold
)
{
    DFBResult           ret;
    CoreWindowStackCursorSetAcceleration       *args = (CoreWindowStackCursorSetAcceleration*) alloca( sizeof(CoreWindowStackCursorSetAcceleration) );
    CoreWindowStackCursorSetAccelerationReturn *return_args = (CoreWindowStackCursorSetAccelerationReturn*) alloca( sizeof(CoreWindowStackCursorSetAccelerationReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    args->numerator = numerator;
    args->denominator = denominator;
    args->threshold = threshold;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorSetAcceleration, args, sizeof(CoreWindowStackCursorSetAcceleration), return_args, sizeof(CoreWindowStackCursorSetAccelerationReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorSetAcceleration ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_CursorSetAcceleration failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorWarp(
                    const DFBPoint                            *position
)
{
    DFBResult           ret;
    CoreWindowStackCursorWarp       *args = (CoreWindowStackCursorWarp*) alloca( sizeof(CoreWindowStackCursorWarp) );
    CoreWindowStackCursorWarpReturn *return_args = (CoreWindowStackCursorWarpReturn*) alloca( sizeof(CoreWindowStackCursorWarpReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( position != NULL );

    args->position = *position;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorWarp, args, sizeof(CoreWindowStackCursorWarp), return_args, sizeof(CoreWindowStackCursorWarpReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorWarp ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_CursorWarp failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::CursorGetPosition(
                    DFBPoint                                  *ret_position
)
{
    DFBResult           ret;
    CoreWindowStackCursorGetPosition       *args = (CoreWindowStackCursorGetPosition*) alloca( sizeof(CoreWindowStackCursorGetPosition) );
    CoreWindowStackCursorGetPositionReturn *return_args = (CoreWindowStackCursorGetPositionReturn*) alloca( sizeof(CoreWindowStackCursorGetPositionReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( ret_position != NULL );


    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_CursorGetPosition, args, sizeof(CoreWindowStackCursorGetPosition), return_args, sizeof(CoreWindowStackCursorGetPositionReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_CursorGetPosition ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_CursorGetPosition failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_position = return_args->position;

    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::BackgroundSetMode(
                    DFBDisplayLayerBackgroundMode              mode
)
{
    DFBResult           ret;
    CoreWindowStackBackgroundSetMode       *args = (CoreWindowStackBackgroundSetMode*) alloca( sizeof(CoreWindowStackBackgroundSetMode) );
    CoreWindowStackBackgroundSetModeReturn *return_args = (CoreWindowStackBackgroundSetModeReturn*) alloca( sizeof(CoreWindowStackBackgroundSetModeReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    args->mode = mode;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_BackgroundSetMode, args, sizeof(CoreWindowStackBackgroundSetMode), return_args, sizeof(CoreWindowStackBackgroundSetModeReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_BackgroundSetMode ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_BackgroundSetMode failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::BackgroundSetImage(
                    CoreSurface                               *image
)
{
    DFBResult           ret;
    CoreWindowStackBackgroundSetImage       *args = (CoreWindowStackBackgroundSetImage*) alloca( sizeof(CoreWindowStackBackgroundSetImage) );
    CoreWindowStackBackgroundSetImageReturn *return_args = (CoreWindowStackBackgroundSetImageReturn*) alloca( sizeof(CoreWindowStackBackgroundSetImageReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( image != NULL );

    args->image_id = CoreSurface_GetID( image );

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_BackgroundSetImage, args, sizeof(CoreWindowStackBackgroundSetImage), return_args, sizeof(CoreWindowStackBackgroundSetImageReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_BackgroundSetImage ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_BackgroundSetImage failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::BackgroundSetColor(
                    const DFBColor                            *color
)
{
    DFBResult           ret;
    CoreWindowStackBackgroundSetColor       *args = (CoreWindowStackBackgroundSetColor*) alloca( sizeof(CoreWindowStackBackgroundSetColor) );
    CoreWindowStackBackgroundSetColorReturn *return_args = (CoreWindowStackBackgroundSetColorReturn*) alloca( sizeof(CoreWindowStackBackgroundSetColorReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );

    D_ASSERT( color != NULL );

    args->color = *color;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_BackgroundSetColor, args, sizeof(CoreWindowStackBackgroundSetColor), return_args, sizeof(CoreWindowStackBackgroundSetColorReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_BackgroundSetColor ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_BackgroundSetColor failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IWindowStack_Requestor::BackgroundSetColorIndex(
                    s32                                        index
)
{
    DFBResult           ret;
    CoreWindowStackBackgroundSetColorIndex       *args = (CoreWindowStackBackgroundSetColorIndex*) alloca( sizeof(CoreWindowStackBackgroundSetColorIndex) );
    CoreWindowStackBackgroundSetColorIndexReturn *return_args = (CoreWindowStackBackgroundSetColorIndexReturn*) alloca( sizeof(CoreWindowStackBackgroundSetColorIndexReturn) );

    D_DEBUG_AT( DirectFB_CoreWindowStack, "IWindowStack_Requestor::%s()\n", __FUNCTION__ );


    args->index = index;

    ret = (DFBResult) CoreWindowStack_Call( obj, FCEF_NONE, CoreWindowStack_BackgroundSetColorIndex, args, sizeof(CoreWindowStackBackgroundSetColorIndex), return_args, sizeof(CoreWindowStackBackgroundSetColorIndexReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: CoreWindowStack_Call( CoreWindowStack_BackgroundSetColorIndex ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: CoreWindowStack_BackgroundSetColorIndex failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

static DFBResult
__CoreWindowStackDispatch__Dispatch( CoreWindowStack *obj,
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


    DirectFB::IWindowStack_Real real( core_dfb, obj );


    switch (method) {
        case CoreWindowStack_RepaintAll: {
            D_UNUSED
            CoreWindowStackRepaintAll       *args        = (CoreWindowStackRepaintAll *) ptr;
            CoreWindowStackRepaintAllReturn *return_args = (CoreWindowStackRepaintAllReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_RepaintAll\n" );

            return_args->result = real.RepaintAll(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackRepaintAllReturn);

            return DFB_OK;
        }

        case CoreWindowStack_GetInsets: {
    CoreWindow *window = NULL;
            D_UNUSED
            CoreWindowStackGetInsets       *args        = (CoreWindowStackGetInsets *) ptr;
            CoreWindowStackGetInsetsReturn *return_args = (CoreWindowStackGetInsetsReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_GetInsets\n" );

            ret = (DFBResult) CoreWindow_Lookup( core_dfb, args->window_id, caller, &window );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up window by ID %u failed!\n", __FUNCTION__, args->window_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real.GetInsets( window, &return_args->insets );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackGetInsetsReturn);

            if (window)
                CoreWindow_Unref( window );

            return DFB_OK;
        }

        case CoreWindowStack_CursorEnable: {
            D_UNUSED
            CoreWindowStackCursorEnable       *args        = (CoreWindowStackCursorEnable *) ptr;
            CoreWindowStackCursorEnableReturn *return_args = (CoreWindowStackCursorEnableReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorEnable\n" );

            return_args->result = real.CursorEnable( args->enable );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorEnableReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorSetShape: {
    CoreSurface *shape = NULL;
            D_UNUSED
            CoreWindowStackCursorSetShape       *args        = (CoreWindowStackCursorSetShape *) ptr;
            CoreWindowStackCursorSetShapeReturn *return_args = (CoreWindowStackCursorSetShapeReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorSetShape\n" );

            ret = (DFBResult) CoreSurface_Lookup( core_dfb, args->shape_id, caller, &shape );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up shape by ID %u failed!\n", __FUNCTION__, args->shape_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real.CursorSetShape( shape, &args->hotspot );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorSetShapeReturn);

            if (shape)
                CoreSurface_Unref( shape );

            return DFB_OK;
        }

        case CoreWindowStack_CursorSetOpacity: {
            D_UNUSED
            CoreWindowStackCursorSetOpacity       *args        = (CoreWindowStackCursorSetOpacity *) ptr;
            CoreWindowStackCursorSetOpacityReturn *return_args = (CoreWindowStackCursorSetOpacityReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorSetOpacity\n" );

            return_args->result = real.CursorSetOpacity( args->opacity );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorSetOpacityReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorSetAcceleration: {
            D_UNUSED
            CoreWindowStackCursorSetAcceleration       *args        = (CoreWindowStackCursorSetAcceleration *) ptr;
            CoreWindowStackCursorSetAccelerationReturn *return_args = (CoreWindowStackCursorSetAccelerationReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorSetAcceleration\n" );

            return_args->result = real.CursorSetAcceleration( args->numerator, args->denominator, args->threshold );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorSetAccelerationReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorWarp: {
            D_UNUSED
            CoreWindowStackCursorWarp       *args        = (CoreWindowStackCursorWarp *) ptr;
            CoreWindowStackCursorWarpReturn *return_args = (CoreWindowStackCursorWarpReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorWarp\n" );

            return_args->result = real.CursorWarp( &args->position );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorWarpReturn);

            return DFB_OK;
        }

        case CoreWindowStack_CursorGetPosition: {
            D_UNUSED
            CoreWindowStackCursorGetPosition       *args        = (CoreWindowStackCursorGetPosition *) ptr;
            CoreWindowStackCursorGetPositionReturn *return_args = (CoreWindowStackCursorGetPositionReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_CursorGetPosition\n" );

            return_args->result = real.CursorGetPosition( &return_args->position );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackCursorGetPositionReturn);

            return DFB_OK;
        }

        case CoreWindowStack_BackgroundSetMode: {
            D_UNUSED
            CoreWindowStackBackgroundSetMode       *args        = (CoreWindowStackBackgroundSetMode *) ptr;
            CoreWindowStackBackgroundSetModeReturn *return_args = (CoreWindowStackBackgroundSetModeReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_BackgroundSetMode\n" );

            return_args->result = real.BackgroundSetMode( args->mode );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackBackgroundSetModeReturn);

            return DFB_OK;
        }

        case CoreWindowStack_BackgroundSetImage: {
    CoreSurface *image = NULL;
            D_UNUSED
            CoreWindowStackBackgroundSetImage       *args        = (CoreWindowStackBackgroundSetImage *) ptr;
            CoreWindowStackBackgroundSetImageReturn *return_args = (CoreWindowStackBackgroundSetImageReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_BackgroundSetImage\n" );

            ret = (DFBResult) CoreSurface_Lookup( core_dfb, args->image_id, caller, &image );
            if (ret) {
                 D_DERROR( ret, "%s: Looking up image by ID %u failed!\n", __FUNCTION__, args->image_id );
                 return_args->result = ret;
                 return DFB_OK;
            }

            return_args->result = real.BackgroundSetImage( image );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackBackgroundSetImageReturn);

            if (image)
                CoreSurface_Unref( image );

            return DFB_OK;
        }

        case CoreWindowStack_BackgroundSetColor: {
            D_UNUSED
            CoreWindowStackBackgroundSetColor       *args        = (CoreWindowStackBackgroundSetColor *) ptr;
            CoreWindowStackBackgroundSetColorReturn *return_args = (CoreWindowStackBackgroundSetColorReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_BackgroundSetColor\n" );

            return_args->result = real.BackgroundSetColor( &args->color );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackBackgroundSetColorReturn);

            return DFB_OK;
        }

        case CoreWindowStack_BackgroundSetColorIndex: {
            D_UNUSED
            CoreWindowStackBackgroundSetColorIndex       *args        = (CoreWindowStackBackgroundSetColorIndex *) ptr;
            CoreWindowStackBackgroundSetColorIndexReturn *return_args = (CoreWindowStackBackgroundSetColorIndexReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_CoreWindowStack, "=-> CoreWindowStack_BackgroundSetColorIndex\n" );

            return_args->result = real.BackgroundSetColorIndex( args->index );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(CoreWindowStackBackgroundSetColorIndexReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}
/*********************************************************************************************************************/

DFBResult
CoreWindowStackDispatch__Dispatch( CoreWindowStack *obj,
                                FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    DFBResult ret;

    D_DEBUG_AT( DirectFB_CoreWindowStack, "CoreWindowStackDispatch::%s( %p )\n", __FUNCTION__, obj );

    Core_PushIdentity( caller );

    ret = __CoreWindowStackDispatch__Dispatch( obj, caller, method, ptr, length, ret_ptr, ret_size, ret_length );

    Core_PopIdentity();

    return ret;
}

}
