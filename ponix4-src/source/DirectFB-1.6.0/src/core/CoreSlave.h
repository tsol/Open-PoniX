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

#ifndef ___CoreSlave__H___
#define ___CoreSlave__H___

#include <core/CoreSlave_includes.h>

/**********************************************************************************************************************
 * CoreSlave
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreSlave_GetData(
                    CoreSlave                                 *obj,
                    void*                                      address,
                    u32                                        bytes,
                    u8                                        *ret_data);

DFBResult CoreSlave_PutData(
                    CoreSlave                                 *obj,
                    void*                                      address,
                    u32                                        bytes,
                    const u8                                  *data);


void CoreSlave_Init_Dispatch(
                    CoreDFB              *core,
                    CoreDFB              *obj,
                    FusionCall           *call
);

void  CoreSlave_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreSlave Calls
 */
typedef enum {
    CoreSlave_GetData = 1,
    CoreSlave_PutData = 2,
} CoreSlaveCall;

/*
 * CoreSlave_GetData
 */
typedef struct {
    void*                                      address;
    u32                                        bytes;
} CoreSlaveGetData;

typedef struct {
    DFBResult                                  result;
    /* 'bytes' u8 follow (data) */
} CoreSlaveGetDataReturn;


/*
 * CoreSlave_PutData
 */
typedef struct {
    void*                                      address;
    u32                                        bytes;
    /* 'bytes' u8 follow (data) */
} CoreSlavePutData;

typedef struct {
    DFBResult                                  result;
} CoreSlavePutDataReturn;





class ICoreSlave : public Interface
{
public:
    ICoreSlave( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult GetData(
                    void*                                      address,
                    u32                                        bytes,
                    u8                                        *ret_data
    ) = 0;

    virtual DFBResult PutData(
                    void*                                      address,
                    u32                                        bytes,
                    const u8                                  *data
    ) = 0;

};



class ICoreSlave_Real : public ICoreSlave
{
private:
    CoreDFB *obj;

public:
    ICoreSlave_Real( CoreDFB *core, CoreDFB *obj )
        :
        ICoreSlave( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult GetData(
                    void*                                      address,
                    u32                                        bytes,
                    u8                                        *ret_data
    );

    virtual DFBResult PutData(
                    void*                                      address,
                    u32                                        bytes,
                    const u8                                  *data
    );

};



class ICoreSlave_Requestor : public ICoreSlave
{
private:
    CoreSlave *obj;

public:
    ICoreSlave_Requestor( CoreDFB *core, CoreSlave *obj )
        :
        ICoreSlave( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult GetData(
                    void*                                      address,
                    u32                                        bytes,
                    u8                                        *ret_data
    );

    virtual DFBResult PutData(
                    void*                                      address,
                    u32                                        bytes,
                    const u8                                  *data
    );

};


DFBResult CoreSlaveDispatch__Dispatch( CoreDFB *obj,
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
