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

#ifndef ___CoreLayer__H___
#define ___CoreLayer__H___

#include <core/CoreLayer_includes.h>

/**********************************************************************************************************************
 * CoreLayer
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreLayer_CreateContext(
                    CoreLayer                                 *obj,
                    CoreLayerContext                         **ret_context);

DFBResult CoreLayer_GetPrimaryContext(
                    CoreLayer                                 *obj,
                    bool                                       activate,
                    CoreLayerContext                         **ret_context);

DFBResult CoreLayer_ActivateContext(
                    CoreLayer                                 *obj,
                    CoreLayerContext                          *context);

DFBResult CoreLayer_GetCurrentOutputField(
                    CoreLayer                                 *obj,
                    s32                                       *ret_field);

DFBResult CoreLayer_SetLevel(
                    CoreLayer                                 *obj,
                    s32                                        level);

DFBResult CoreLayer_WaitVSync(
                    CoreLayer                                 *obj
);


void CoreLayer_Init_Dispatch(
                    CoreDFB              *core,
                    CoreLayer            *obj,
                    FusionCall           *call
);

void  CoreLayer_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreLayer Calls
 */
typedef enum {
    CoreLayer_CreateContext = 1,
    CoreLayer_GetPrimaryContext = 2,
    CoreLayer_ActivateContext = 3,
    CoreLayer_GetCurrentOutputField = 4,
    CoreLayer_SetLevel = 5,
    CoreLayer_WaitVSync = 6,
} CoreLayerCall;

/*
 * CoreLayer_CreateContext
 */
typedef struct {
} CoreLayerCreateContext;

typedef struct {
    DFBResult                                  result;
    u32                                        context_id;
    void*                                      context_ptr;
} CoreLayerCreateContextReturn;


/*
 * CoreLayer_GetPrimaryContext
 */
typedef struct {
    bool                                       activate;
} CoreLayerGetPrimaryContext;

typedef struct {
    DFBResult                                  result;
    u32                                        context_id;
    void*                                      context_ptr;
} CoreLayerGetPrimaryContextReturn;


/*
 * CoreLayer_ActivateContext
 */
typedef struct {
    u32                                        context_id;
} CoreLayerActivateContext;

typedef struct {
    DFBResult                                  result;
} CoreLayerActivateContextReturn;


/*
 * CoreLayer_GetCurrentOutputField
 */
typedef struct {
} CoreLayerGetCurrentOutputField;

typedef struct {
    DFBResult                                  result;
    s32                                        field;
} CoreLayerGetCurrentOutputFieldReturn;


/*
 * CoreLayer_SetLevel
 */
typedef struct {
    s32                                        level;
} CoreLayerSetLevel;

typedef struct {
    DFBResult                                  result;
} CoreLayerSetLevelReturn;


/*
 * CoreLayer_WaitVSync
 */
typedef struct {
} CoreLayerWaitVSync;

typedef struct {
    DFBResult                                  result;
} CoreLayerWaitVSyncReturn;





class ILayer : public Interface
{
public:
    ILayer( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult CreateContext(
                    CoreLayerContext                         **ret_context
    ) = 0;

    virtual DFBResult GetPrimaryContext(
                    bool                                       activate,
                    CoreLayerContext                         **ret_context
    ) = 0;

    virtual DFBResult ActivateContext(
                    CoreLayerContext                          *context
    ) = 0;

    virtual DFBResult GetCurrentOutputField(
                    s32                                       *ret_field
    ) = 0;

    virtual DFBResult SetLevel(
                    s32                                        level
    ) = 0;

    virtual DFBResult WaitVSync(

    ) = 0;

};



class ILayer_Real : public ILayer
{
private:
    CoreLayer *obj;

public:
    ILayer_Real( CoreDFB *core, CoreLayer *obj )
        :
        ILayer( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult CreateContext(
                    CoreLayerContext                         **ret_context
    );

    virtual DFBResult GetPrimaryContext(
                    bool                                       activate,
                    CoreLayerContext                         **ret_context
    );

    virtual DFBResult ActivateContext(
                    CoreLayerContext                          *context
    );

    virtual DFBResult GetCurrentOutputField(
                    s32                                       *ret_field
    );

    virtual DFBResult SetLevel(
                    s32                                        level
    );

    virtual DFBResult WaitVSync(

    );

};



class ILayer_Requestor : public ILayer
{
private:
    CoreLayer *obj;

public:
    ILayer_Requestor( CoreDFB *core, CoreLayer *obj )
        :
        ILayer( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult CreateContext(
                    CoreLayerContext                         **ret_context
    );

    virtual DFBResult GetPrimaryContext(
                    bool                                       activate,
                    CoreLayerContext                         **ret_context
    );

    virtual DFBResult ActivateContext(
                    CoreLayerContext                          *context
    );

    virtual DFBResult GetCurrentOutputField(
                    s32                                       *ret_field
    );

    virtual DFBResult SetLevel(
                    s32                                        level
    );

    virtual DFBResult WaitVSync(

    );

};


DFBResult CoreLayerDispatch__Dispatch( CoreLayer *obj,
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
