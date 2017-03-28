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

#include "DataBuffer.h"

extern "C" {
#include <directfb_util.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/memcpy.h>
#include <direct/messages.h>

#include <fusion/conf.h>

#include <core/core.h>
}

D_DEBUG_DOMAIN( DirectFB_DataBuffer, "DirectFB/DataBuffer", "DirectFB DataBuffer" );

/*********************************************************************************************************************/

DFBResult
DataBuffer_Flush(
                    DataBuffer                                *obj

)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.Flush(  );
}

DFBResult
DataBuffer_Finish(
                    DataBuffer                                *obj

)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.Finish(  );
}

DFBResult
DataBuffer_SeekTo(
                    DataBuffer                                *obj,
                    u64                                        offset
)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.SeekTo( offset );
}

DFBResult
DataBuffer_GetPosition(
                    DataBuffer                                *obj,
                    u64                                       *ret_offset
)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.GetPosition( ret_offset );
}

DFBResult
DataBuffer_GetLength(
                    DataBuffer                                *obj,
                    u64                                       *ret_length
)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.GetLength( ret_length );
}

DFBResult
DataBuffer_WaitForData(
                    DataBuffer                                *obj,
                    u64                                        length
)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.WaitForData( length );
}

DFBResult
DataBuffer_WaitForDataWithTimeout(
                    DataBuffer                                *obj,
                    u64                                        length,
                    u64                                        timeout_ms
)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.WaitForDataWithTimeout( length, timeout_ms );
}

DFBResult
DataBuffer_GetData(
                    DataBuffer                                *obj,
                    u32                                        length,
                    u8                                        *ret_data,
                    u32                                       *ret_read
)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.GetData( length, ret_data, ret_read );
}

DFBResult
DataBuffer_PeekData(
                    DataBuffer                                *obj,
                    u32                                        length,
                    s64                                        offset,
                    u8                                        *ret_data,
                    u32                                       *ret_read
)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.PeekData( length, offset, ret_data, ret_read );
}

DFBResult
DataBuffer_HasData(
                    DataBuffer                                *obj

)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.HasData(  );
}

DFBResult
DataBuffer_PutData(
                    DataBuffer                                *obj,
                    const u8                                  *data,
                    u32                                        length
)
{
    DirectFB::IDataBuffer_Requestor requestor( core_dfb, obj );

    return requestor.PutData( data, length );
}

/*********************************************************************************************************************/

static FusionCallHandlerResult
DataBuffer_Dispatch( int           caller,   /* fusion id of the caller */
                     int           call_arg, /* optional call parameter */
                     void         *ptr, /* optional call parameter */
                     unsigned int  length,
                     void         *ctx,      /* optional handler context */
                     unsigned int  serial,
                     void         *ret_ptr,
                     unsigned int  ret_size,
                     unsigned int *ret_length )
{
    IDirectFBDataBuffer *obj = (IDirectFBDataBuffer*) ctx;
    DirectFB::DataBufferDispatch__Dispatch( obj, caller, call_arg, ptr, length, ret_ptr, ret_size, ret_length );

    return FCHR_RETURN;
}

void DataBuffer_Init_Dispatch(
                    CoreDFB              *core,
                    IDirectFBDataBuffer  *obj,
                    FusionCall           *call
)
{
    fusion_call_init3( call, DataBuffer_Dispatch, obj, core->world );
}

void  DataBuffer_Deinit_Dispatch(
                    FusionCall           *call
)
{
     fusion_call_destroy( call );
}

/*********************************************************************************************************************/

namespace DirectFB {



DFBResult
IDataBuffer_Requestor::Flush(

)
{
    DFBResult           ret;
    DataBufferFlush       *args = (DataBufferFlush*) alloca( sizeof(DataBufferFlush) );
    DataBufferFlushReturn *return_args = (DataBufferFlushReturn*) alloca( sizeof(DataBufferFlushReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_Flush, args, sizeof(DataBufferFlush), return_args, sizeof(DataBufferFlushReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_Flush ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_Flush failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::Finish(

)
{
    DFBResult           ret;
    DataBufferFinish       *args = (DataBufferFinish*) alloca( sizeof(DataBufferFinish) );
    DataBufferFinishReturn *return_args = (DataBufferFinishReturn*) alloca( sizeof(DataBufferFinishReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_Finish, args, sizeof(DataBufferFinish), return_args, sizeof(DataBufferFinishReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_Finish ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_Finish failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::SeekTo(
                    u64                                        offset
)
{
    DFBResult           ret;
    DataBufferSeekTo       *args = (DataBufferSeekTo*) alloca( sizeof(DataBufferSeekTo) );
    DataBufferSeekToReturn *return_args = (DataBufferSeekToReturn*) alloca( sizeof(DataBufferSeekToReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );


    args->offset = offset;

    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_SeekTo, args, sizeof(DataBufferSeekTo), return_args, sizeof(DataBufferSeekToReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_SeekTo ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_SeekTo failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::GetPosition(
                    u64                                       *ret_offset
)
{
    DFBResult           ret;
    DataBufferGetPosition       *args = (DataBufferGetPosition*) alloca( sizeof(DataBufferGetPosition) );
    DataBufferGetPositionReturn *return_args = (DataBufferGetPositionReturn*) alloca( sizeof(DataBufferGetPositionReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_GetPosition, args, sizeof(DataBufferGetPosition), return_args, sizeof(DataBufferGetPositionReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_GetPosition ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_GetPosition failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_offset = return_args->offset;

    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::GetLength(
                    u64                                       *ret_length
)
{
    DFBResult           ret;
    DataBufferGetLength       *args = (DataBufferGetLength*) alloca( sizeof(DataBufferGetLength) );
    DataBufferGetLengthReturn *return_args = (DataBufferGetLengthReturn*) alloca( sizeof(DataBufferGetLengthReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_GetLength, args, sizeof(DataBufferGetLength), return_args, sizeof(DataBufferGetLengthReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_GetLength ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_GetLength failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_length = return_args->length;

    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::WaitForData(
                    u64                                        length
)
{
    DFBResult           ret;
    DataBufferWaitForData       *args = (DataBufferWaitForData*) alloca( sizeof(DataBufferWaitForData) );
    DataBufferWaitForDataReturn *return_args = (DataBufferWaitForDataReturn*) alloca( sizeof(DataBufferWaitForDataReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );


    args->length = length;

    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_WaitForData, args, sizeof(DataBufferWaitForData), return_args, sizeof(DataBufferWaitForDataReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_WaitForData ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_WaitForData failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::WaitForDataWithTimeout(
                    u64                                        length,
                    u64                                        timeout_ms
)
{
    DFBResult           ret;
    DataBufferWaitForDataWithTimeout       *args = (DataBufferWaitForDataWithTimeout*) alloca( sizeof(DataBufferWaitForDataWithTimeout) );
    DataBufferWaitForDataWithTimeoutReturn *return_args = (DataBufferWaitForDataWithTimeoutReturn*) alloca( sizeof(DataBufferWaitForDataWithTimeoutReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );


    args->length = length;
    args->timeout_ms = timeout_ms;

    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_WaitForDataWithTimeout, args, sizeof(DataBufferWaitForDataWithTimeout), return_args, sizeof(DataBufferWaitForDataWithTimeoutReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_WaitForDataWithTimeout ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_WaitForDataWithTimeout failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::GetData(
                    u32                                        length,
                    u8                                        *ret_data,
                    u32                                       *ret_read
)
{
    DFBResult           ret;
    DataBufferGetData       *args = (DataBufferGetData*) alloca( sizeof(DataBufferGetData) );
    DataBufferGetDataReturn *return_args = (DataBufferGetDataReturn*) alloca( sizeof(DataBufferGetDataReturn) + length * sizeof(u8) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );


    args->length = length;

    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_GetData, args, sizeof(DataBufferGetData), return_args, sizeof(DataBufferGetDataReturn) + length * sizeof(u8), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_GetData ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_GetData failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_read = return_args->read;
    direct_memcpy( ret_data, (char*) (return_args + 1), return_args->read * sizeof(u8) );

    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::PeekData(
                    u32                                        length,
                    s64                                        offset,
                    u8                                        *ret_data,
                    u32                                       *ret_read
)
{
    DFBResult           ret;
    DataBufferPeekData       *args = (DataBufferPeekData*) alloca( sizeof(DataBufferPeekData) );
    DataBufferPeekDataReturn *return_args = (DataBufferPeekDataReturn*) alloca( sizeof(DataBufferPeekDataReturn) + length * sizeof(u8) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );


    args->length = length;
    args->offset = offset;

    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_PeekData, args, sizeof(DataBufferPeekData), return_args, sizeof(DataBufferPeekDataReturn) + length * sizeof(u8), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_PeekData ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_PeekData failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }

    *ret_read = return_args->read;
    direct_memcpy( ret_data, (char*) (return_args + 1), return_args->read * sizeof(u8) );

    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::HasData(

)
{
    DFBResult           ret;
    DataBufferHasData       *args = (DataBufferHasData*) alloca( sizeof(DataBufferHasData) );
    DataBufferHasDataReturn *return_args = (DataBufferHasDataReturn*) alloca( sizeof(DataBufferHasDataReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );



    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_HasData, args, sizeof(DataBufferHasData), return_args, sizeof(DataBufferHasDataReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_HasData ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_HasData failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}


DFBResult
IDataBuffer_Requestor::PutData(
                    const u8                                  *data,
                    u32                                        length
)
{
    DFBResult           ret;
    DataBufferPutData       *args = (DataBufferPutData*) alloca( sizeof(DataBufferPutData) + length * sizeof(u8) );
    DataBufferPutDataReturn *return_args = (DataBufferPutDataReturn*) alloca( sizeof(DataBufferPutDataReturn) );

    D_DEBUG_AT( DirectFB_DataBuffer, "IDataBuffer_Requestor::%s()\n", __FUNCTION__ );


    args->length = length;
    direct_memcpy( (char*) (args + 1), data, length * sizeof(u8) );

    ret = (DFBResult) DataBuffer_Call( obj, FCEF_NONE, DataBuffer_PutData, args, sizeof(DataBufferPutData) + length * sizeof(u8), return_args, sizeof(DataBufferPutDataReturn), NULL );
    if (ret) {
        D_DERROR( ret, "%s: DataBuffer_Call( DataBuffer_PutData ) failed!\n", __FUNCTION__ );
        return ret;
    }

    if (return_args->result) {
         /*D_DERROR( return_args->result, "%s: DataBuffer_PutData failed!\n", __FUNCTION__ );*/
         return return_args->result;
    }


    return DFB_OK;
}

/*********************************************************************************************************************/

static DFBResult
__DataBufferDispatch__Dispatch( IDirectFBDataBuffer *obj,
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


    DirectFB::IDataBuffer_Real real( core_dfb, obj );


    switch (method) {
        case DataBuffer_Flush: {
            D_UNUSED
            DataBufferFlush       *args        = (DataBufferFlush *) ptr;
            DataBufferFlushReturn *return_args = (DataBufferFlushReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_Flush\n" );

            return_args->result = real.Flush(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferFlushReturn);

            return DFB_OK;
        }

        case DataBuffer_Finish: {
            D_UNUSED
            DataBufferFinish       *args        = (DataBufferFinish *) ptr;
            DataBufferFinishReturn *return_args = (DataBufferFinishReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_Finish\n" );

            return_args->result = real.Finish(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferFinishReturn);

            return DFB_OK;
        }

        case DataBuffer_SeekTo: {
            D_UNUSED
            DataBufferSeekTo       *args        = (DataBufferSeekTo *) ptr;
            DataBufferSeekToReturn *return_args = (DataBufferSeekToReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_SeekTo\n" );

            return_args->result = real.SeekTo( args->offset );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferSeekToReturn);

            return DFB_OK;
        }

        case DataBuffer_GetPosition: {
            D_UNUSED
            DataBufferGetPosition       *args        = (DataBufferGetPosition *) ptr;
            DataBufferGetPositionReturn *return_args = (DataBufferGetPositionReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_GetPosition\n" );

            return_args->result = real.GetPosition( &return_args->offset );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferGetPositionReturn);

            return DFB_OK;
        }

        case DataBuffer_GetLength: {
            D_UNUSED
            DataBufferGetLength       *args        = (DataBufferGetLength *) ptr;
            DataBufferGetLengthReturn *return_args = (DataBufferGetLengthReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_GetLength\n" );

            return_args->result = real.GetLength( &return_args->length );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferGetLengthReturn);

            return DFB_OK;
        }

        case DataBuffer_WaitForData: {
            D_UNUSED
            DataBufferWaitForData       *args        = (DataBufferWaitForData *) ptr;
            DataBufferWaitForDataReturn *return_args = (DataBufferWaitForDataReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_WaitForData\n" );

            return_args->result = real.WaitForData( args->length );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferWaitForDataReturn);

            return DFB_OK;
        }

        case DataBuffer_WaitForDataWithTimeout: {
            D_UNUSED
            DataBufferWaitForDataWithTimeout       *args        = (DataBufferWaitForDataWithTimeout *) ptr;
            DataBufferWaitForDataWithTimeoutReturn *return_args = (DataBufferWaitForDataWithTimeoutReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_WaitForDataWithTimeout\n" );

            return_args->result = real.WaitForDataWithTimeout( args->length, args->timeout_ms );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferWaitForDataWithTimeoutReturn);

            return DFB_OK;
        }

        case DataBuffer_GetData: {
            D_UNUSED
            DataBufferGetData       *args        = (DataBufferGetData *) ptr;
            DataBufferGetDataReturn *return_args = (DataBufferGetDataReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_GetData\n" );

            return_args->result = real.GetData( args->length, (u8*) ((char*)(return_args + 1)), &return_args->read );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferGetDataReturn) + return_args->read * sizeof(u8);

            return DFB_OK;
        }

        case DataBuffer_PeekData: {
            D_UNUSED
            DataBufferPeekData       *args        = (DataBufferPeekData *) ptr;
            DataBufferPeekDataReturn *return_args = (DataBufferPeekDataReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_PeekData\n" );

            return_args->result = real.PeekData( args->length, args->offset, (u8*) ((char*)(return_args + 1)), &return_args->read );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferPeekDataReturn) + return_args->read * sizeof(u8);

            return DFB_OK;
        }

        case DataBuffer_HasData: {
            D_UNUSED
            DataBufferHasData       *args        = (DataBufferHasData *) ptr;
            DataBufferHasDataReturn *return_args = (DataBufferHasDataReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_HasData\n" );

            return_args->result = real.HasData(  );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferHasDataReturn);

            return DFB_OK;
        }

        case DataBuffer_PutData: {
            D_UNUSED
            DataBufferPutData       *args        = (DataBufferPutData *) ptr;
            DataBufferPutDataReturn *return_args = (DataBufferPutDataReturn *) ret_ptr;

            D_DEBUG_AT( DirectFB_DataBuffer, "=-> DataBuffer_PutData\n" );

            return_args->result = real.PutData( (u8*) ((char*)(args + 1)), args->length );
            if (return_args->result == DFB_OK) {
            }

            *ret_length = sizeof(DataBufferPutDataReturn);

            return DFB_OK;
        }

    }

    return DFB_NOSUCHMETHOD;
}
/*********************************************************************************************************************/

DFBResult
DataBufferDispatch__Dispatch( IDirectFBDataBuffer *obj,
                                FusionID      caller,
                                int           method,
                                void         *ptr,
                                unsigned int  length,
                                void         *ret_ptr,
                                unsigned int  ret_size,
                                unsigned int *ret_length )
{
    DFBResult ret;

    D_DEBUG_AT( DirectFB_DataBuffer, "DataBufferDispatch::%s( %p )\n", __FUNCTION__, obj );

    Core_PushIdentity( caller );

    ret = __DataBufferDispatch__Dispatch( obj, caller, method, ptr, length, ret_ptr, ret_size, ret_length );

    Core_PopIdentity();

    return ret;
}

}
