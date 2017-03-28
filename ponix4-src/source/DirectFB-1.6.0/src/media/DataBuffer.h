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

#ifndef ___DataBuffer__H___
#define ___DataBuffer__H___

#include "DataBuffer_includes.h"

/**********************************************************************************************************************
 * DataBuffer
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult DataBuffer_Flush(
                    DataBuffer                                *obj
);

DFBResult DataBuffer_Finish(
                    DataBuffer                                *obj
);

DFBResult DataBuffer_SeekTo(
                    DataBuffer                                *obj,
                    u64                                        offset);

DFBResult DataBuffer_GetPosition(
                    DataBuffer                                *obj,
                    u64                                       *ret_offset);

DFBResult DataBuffer_GetLength(
                    DataBuffer                                *obj,
                    u64                                       *ret_length);

DFBResult DataBuffer_WaitForData(
                    DataBuffer                                *obj,
                    u64                                        length);

DFBResult DataBuffer_WaitForDataWithTimeout(
                    DataBuffer                                *obj,
                    u64                                        length,
                    u64                                        timeout_ms);

DFBResult DataBuffer_GetData(
                    DataBuffer                                *obj,
                    u32                                        length,
                    u8                                        *ret_data,
                    u32                                       *ret_read);

DFBResult DataBuffer_PeekData(
                    DataBuffer                                *obj,
                    u32                                        length,
                    s64                                        offset,
                    u8                                        *ret_data,
                    u32                                       *ret_read);

DFBResult DataBuffer_HasData(
                    DataBuffer                                *obj
);

DFBResult DataBuffer_PutData(
                    DataBuffer                                *obj,
                    const u8                                  *data,
                    u32                                        length);


void DataBuffer_Init_Dispatch(
                    CoreDFB              *core,
                    IDirectFBDataBuffer  *obj,
                    FusionCall           *call
);

void  DataBuffer_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * DataBuffer Calls
 */
typedef enum {
    DataBuffer_Flush = 1,
    DataBuffer_Finish = 2,
    DataBuffer_SeekTo = 3,
    DataBuffer_GetPosition = 4,
    DataBuffer_GetLength = 5,
    DataBuffer_WaitForData = 6,
    DataBuffer_WaitForDataWithTimeout = 7,
    DataBuffer_GetData = 8,
    DataBuffer_PeekData = 9,
    DataBuffer_HasData = 10,
    DataBuffer_PutData = 11,
} DataBufferCall;

/*
 * DataBuffer_Flush
 */
typedef struct {
} DataBufferFlush;

typedef struct {
    DFBResult                                  result;
} DataBufferFlushReturn;


/*
 * DataBuffer_Finish
 */
typedef struct {
} DataBufferFinish;

typedef struct {
    DFBResult                                  result;
} DataBufferFinishReturn;


/*
 * DataBuffer_SeekTo
 */
typedef struct {
    u64                                        offset;
} DataBufferSeekTo;

typedef struct {
    DFBResult                                  result;
} DataBufferSeekToReturn;


/*
 * DataBuffer_GetPosition
 */
typedef struct {
} DataBufferGetPosition;

typedef struct {
    DFBResult                                  result;
    u64                                        offset;
} DataBufferGetPositionReturn;


/*
 * DataBuffer_GetLength
 */
typedef struct {
} DataBufferGetLength;

typedef struct {
    DFBResult                                  result;
    u64                                        length;
} DataBufferGetLengthReturn;


/*
 * DataBuffer_WaitForData
 */
typedef struct {
    u64                                        length;
} DataBufferWaitForData;

typedef struct {
    DFBResult                                  result;
} DataBufferWaitForDataReturn;


/*
 * DataBuffer_WaitForDataWithTimeout
 */
typedef struct {
    u64                                        length;
    u64                                        timeout_ms;
} DataBufferWaitForDataWithTimeout;

typedef struct {
    DFBResult                                  result;
} DataBufferWaitForDataWithTimeoutReturn;


/*
 * DataBuffer_GetData
 */
typedef struct {
    u32                                        length;
} DataBufferGetData;

typedef struct {
    DFBResult                                  result;
    u32                                        read;
    /* 'read' u8 follow (data) */
} DataBufferGetDataReturn;


/*
 * DataBuffer_PeekData
 */
typedef struct {
    u32                                        length;
    s64                                        offset;
} DataBufferPeekData;

typedef struct {
    DFBResult                                  result;
    u32                                        read;
    /* 'read' u8 follow (data) */
} DataBufferPeekDataReturn;


/*
 * DataBuffer_HasData
 */
typedef struct {
} DataBufferHasData;

typedef struct {
    DFBResult                                  result;
} DataBufferHasDataReturn;


/*
 * DataBuffer_PutData
 */
typedef struct {
    u32                                        length;
    /* 'length' u8 follow (data) */
} DataBufferPutData;

typedef struct {
    DFBResult                                  result;
} DataBufferPutDataReturn;





class IDataBuffer : public Interface
{
public:
    IDataBuffer( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult Flush(

    ) = 0;

    virtual DFBResult Finish(

    ) = 0;

    virtual DFBResult SeekTo(
                    u64                                        offset
    ) = 0;

    virtual DFBResult GetPosition(
                    u64                                       *ret_offset
    ) = 0;

    virtual DFBResult GetLength(
                    u64                                       *ret_length
    ) = 0;

    virtual DFBResult WaitForData(
                    u64                                        length
    ) = 0;

    virtual DFBResult WaitForDataWithTimeout(
                    u64                                        length,
                    u64                                        timeout_ms
    ) = 0;

    virtual DFBResult GetData(
                    u32                                        length,
                    u8                                        *ret_data,
                    u32                                       *ret_read
    ) = 0;

    virtual DFBResult PeekData(
                    u32                                        length,
                    s64                                        offset,
                    u8                                        *ret_data,
                    u32                                       *ret_read
    ) = 0;

    virtual DFBResult HasData(

    ) = 0;

    virtual DFBResult PutData(
                    const u8                                  *data,
                    u32                                        length
    ) = 0;

};



class IDataBuffer_Real : public IDataBuffer
{
private:
    IDirectFBDataBuffer *obj;

public:
    IDataBuffer_Real( CoreDFB *core, IDirectFBDataBuffer *obj )
        :
        IDataBuffer( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult Flush(

    );

    virtual DFBResult Finish(

    );

    virtual DFBResult SeekTo(
                    u64                                        offset
    );

    virtual DFBResult GetPosition(
                    u64                                       *ret_offset
    );

    virtual DFBResult GetLength(
                    u64                                       *ret_length
    );

    virtual DFBResult WaitForData(
                    u64                                        length
    );

    virtual DFBResult WaitForDataWithTimeout(
                    u64                                        length,
                    u64                                        timeout_ms
    );

    virtual DFBResult GetData(
                    u32                                        length,
                    u8                                        *ret_data,
                    u32                                       *ret_read
    );

    virtual DFBResult PeekData(
                    u32                                        length,
                    s64                                        offset,
                    u8                                        *ret_data,
                    u32                                       *ret_read
    );

    virtual DFBResult HasData(

    );

    virtual DFBResult PutData(
                    const u8                                  *data,
                    u32                                        length
    );

};



class IDataBuffer_Requestor : public IDataBuffer
{
private:
    DataBuffer *obj;

public:
    IDataBuffer_Requestor( CoreDFB *core, DataBuffer *obj )
        :
        IDataBuffer( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult Flush(

    );

    virtual DFBResult Finish(

    );

    virtual DFBResult SeekTo(
                    u64                                        offset
    );

    virtual DFBResult GetPosition(
                    u64                                       *ret_offset
    );

    virtual DFBResult GetLength(
                    u64                                       *ret_length
    );

    virtual DFBResult WaitForData(
                    u64                                        length
    );

    virtual DFBResult WaitForDataWithTimeout(
                    u64                                        length,
                    u64                                        timeout_ms
    );

    virtual DFBResult GetData(
                    u32                                        length,
                    u8                                        *ret_data,
                    u32                                       *ret_read
    );

    virtual DFBResult PeekData(
                    u32                                        length,
                    s64                                        offset,
                    u8                                        *ret_data,
                    u32                                       *ret_read
    );

    virtual DFBResult HasData(

    );

    virtual DFBResult PutData(
                    const u8                                  *data,
                    u32                                        length
    );

};


DFBResult DataBufferDispatch__Dispatch( IDirectFBDataBuffer *obj,
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
