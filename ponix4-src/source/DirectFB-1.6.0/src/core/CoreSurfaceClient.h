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

#ifndef ___CoreSurfaceClient__H___
#define ___CoreSurfaceClient__H___

#include <core/CoreSurfaceClient_includes.h>

/**********************************************************************************************************************
 * CoreSurfaceClient
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreSurfaceClient_FrameAck(
                    CoreSurfaceClient                         *obj,
                    u32                                        flip_count);


void CoreSurfaceClient_Init_Dispatch(
                    CoreDFB              *core,
                    CoreSurfaceClient    *obj,
                    FusionCall           *call
);

void  CoreSurfaceClient_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreSurfaceClient Calls
 */
typedef enum {
    CoreSurfaceClient_FrameAck = 1,
} CoreSurfaceClientCall;

/*
 * CoreSurfaceClient_FrameAck
 */
typedef struct {
    u32                                        flip_count;
} CoreSurfaceClientFrameAck;

typedef struct {
    DFBResult                                  result;
} CoreSurfaceClientFrameAckReturn;





class ISurfaceClient : public Interface
{
public:
    ISurfaceClient( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult FrameAck(
                    u32                                        flip_count
    ) = 0;

};



class ISurfaceClient_Real : public ISurfaceClient
{
private:
    CoreSurfaceClient *obj;

public:
    ISurfaceClient_Real( CoreDFB *core, CoreSurfaceClient *obj )
        :
        ISurfaceClient( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult FrameAck(
                    u32                                        flip_count
    );

};



class ISurfaceClient_Requestor : public ISurfaceClient
{
private:
    CoreSurfaceClient *obj;

public:
    ISurfaceClient_Requestor( CoreDFB *core, CoreSurfaceClient *obj )
        :
        ISurfaceClient( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult FrameAck(
                    u32                                        flip_count
    );

};


DFBResult CoreSurfaceClientDispatch__Dispatch( CoreSurfaceClient *obj,
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
