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

#ifndef ___CoreScreen__H___
#define ___CoreScreen__H___

#include <core/CoreScreen_includes.h>

/**********************************************************************************************************************
 * CoreScreen
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreScreen_SetPowerMode(
                    CoreScreen                                *obj,
                    DFBScreenPowerMode                         mode);

DFBResult CoreScreen_WaitVSync(
                    CoreScreen                                *obj
);

DFBResult CoreScreen_GetVSyncCount(
                    CoreScreen                                *obj,
                    u64                                       *ret_count);

DFBResult CoreScreen_TestMixerConfig(
                    CoreScreen                                *obj,
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config,
                    DFBScreenMixerConfigFlags                 *ret_failed);

DFBResult CoreScreen_SetMixerConfig(
                    CoreScreen                                *obj,
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config);

DFBResult CoreScreen_TestEncoderConfig(
                    CoreScreen                                *obj,
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config,
                    DFBScreenEncoderConfigFlags               *ret_failed);

DFBResult CoreScreen_SetEncoderConfig(
                    CoreScreen                                *obj,
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config);

DFBResult CoreScreen_TestOutputConfig(
                    CoreScreen                                *obj,
                    u32                                        output,
                    const DFBScreenOutputConfig               *config,
                    DFBScreenOutputConfigFlags                *ret_failed);

DFBResult CoreScreen_SetOutputConfig(
                    CoreScreen                                *obj,
                    u32                                        output,
                    const DFBScreenOutputConfig               *config);

DFBResult CoreScreen_GetScreenSize(
                    CoreScreen                                *obj,
                    DFBDimension                              *ret_size);

DFBResult CoreScreen_GetLayerDimension(
                    CoreScreen                                *obj,
                    CoreLayer                                 *layer,
                    DFBDimension                              *ret_size);


void CoreScreen_Init_Dispatch(
                    CoreDFB              *core,
                    CoreScreen           *obj,
                    FusionCall           *call
);

void  CoreScreen_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreScreen Calls
 */
typedef enum {
    CoreScreen_SetPowerMode = 1,
    CoreScreen_WaitVSync = 2,
    CoreScreen_GetVSyncCount = 3,
    CoreScreen_TestMixerConfig = 4,
    CoreScreen_SetMixerConfig = 5,
    CoreScreen_TestEncoderConfig = 6,
    CoreScreen_SetEncoderConfig = 7,
    CoreScreen_TestOutputConfig = 8,
    CoreScreen_SetOutputConfig = 9,
    CoreScreen_GetScreenSize = 10,
    CoreScreen_GetLayerDimension = 11,
} CoreScreenCall;

/*
 * CoreScreen_SetPowerMode
 */
typedef struct {
    DFBScreenPowerMode                         mode;
} CoreScreenSetPowerMode;

typedef struct {
    DFBResult                                  result;
} CoreScreenSetPowerModeReturn;


/*
 * CoreScreen_WaitVSync
 */
typedef struct {
} CoreScreenWaitVSync;

typedef struct {
    DFBResult                                  result;
} CoreScreenWaitVSyncReturn;


/*
 * CoreScreen_GetVSyncCount
 */
typedef struct {
} CoreScreenGetVSyncCount;

typedef struct {
    DFBResult                                  result;
    u64                                        count;
} CoreScreenGetVSyncCountReturn;


/*
 * CoreScreen_TestMixerConfig
 */
typedef struct {
    u32                                        mixer;
    DFBScreenMixerConfig                       config;
} CoreScreenTestMixerConfig;

typedef struct {
    DFBResult                                  result;
    DFBScreenMixerConfigFlags                  failed;
} CoreScreenTestMixerConfigReturn;


/*
 * CoreScreen_SetMixerConfig
 */
typedef struct {
    u32                                        mixer;
    DFBScreenMixerConfig                       config;
} CoreScreenSetMixerConfig;

typedef struct {
    DFBResult                                  result;
} CoreScreenSetMixerConfigReturn;


/*
 * CoreScreen_TestEncoderConfig
 */
typedef struct {
    u32                                        encoder;
    DFBScreenEncoderConfig                     config;
} CoreScreenTestEncoderConfig;

typedef struct {
    DFBResult                                  result;
    DFBScreenEncoderConfigFlags                failed;
} CoreScreenTestEncoderConfigReturn;


/*
 * CoreScreen_SetEncoderConfig
 */
typedef struct {
    u32                                        encoder;
    DFBScreenEncoderConfig                     config;
} CoreScreenSetEncoderConfig;

typedef struct {
    DFBResult                                  result;
} CoreScreenSetEncoderConfigReturn;


/*
 * CoreScreen_TestOutputConfig
 */
typedef struct {
    u32                                        output;
    DFBScreenOutputConfig                      config;
} CoreScreenTestOutputConfig;

typedef struct {
    DFBResult                                  result;
    DFBScreenOutputConfigFlags                 failed;
} CoreScreenTestOutputConfigReturn;


/*
 * CoreScreen_SetOutputConfig
 */
typedef struct {
    u32                                        output;
    DFBScreenOutputConfig                      config;
} CoreScreenSetOutputConfig;

typedef struct {
    DFBResult                                  result;
} CoreScreenSetOutputConfigReturn;


/*
 * CoreScreen_GetScreenSize
 */
typedef struct {
} CoreScreenGetScreenSize;

typedef struct {
    DFBResult                                  result;
    DFBDimension                               size;
} CoreScreenGetScreenSizeReturn;


/*
 * CoreScreen_GetLayerDimension
 */
typedef struct {
    u32                                        layer_id;
} CoreScreenGetLayerDimension;

typedef struct {
    DFBResult                                  result;
    DFBDimension                               size;
} CoreScreenGetLayerDimensionReturn;





class IScreen : public Interface
{
public:
    IScreen( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult SetPowerMode(
                    DFBScreenPowerMode                         mode
    ) = 0;

    virtual DFBResult WaitVSync(

    ) = 0;

    virtual DFBResult GetVSyncCount(
                    u64                                       *ret_count
    ) = 0;

    virtual DFBResult TestMixerConfig(
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config,
                    DFBScreenMixerConfigFlags                 *ret_failed
    ) = 0;

    virtual DFBResult SetMixerConfig(
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config
    ) = 0;

    virtual DFBResult TestEncoderConfig(
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config,
                    DFBScreenEncoderConfigFlags               *ret_failed
    ) = 0;

    virtual DFBResult SetEncoderConfig(
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config
    ) = 0;

    virtual DFBResult TestOutputConfig(
                    u32                                        output,
                    const DFBScreenOutputConfig               *config,
                    DFBScreenOutputConfigFlags                *ret_failed
    ) = 0;

    virtual DFBResult SetOutputConfig(
                    u32                                        output,
                    const DFBScreenOutputConfig               *config
    ) = 0;

    virtual DFBResult GetScreenSize(
                    DFBDimension                              *ret_size
    ) = 0;

    virtual DFBResult GetLayerDimension(
                    CoreLayer                                 *layer,
                    DFBDimension                              *ret_size
    ) = 0;

};



class IScreen_Real : public IScreen
{
private:
    CoreScreen *obj;

public:
    IScreen_Real( CoreDFB *core, CoreScreen *obj )
        :
        IScreen( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetPowerMode(
                    DFBScreenPowerMode                         mode
    );

    virtual DFBResult WaitVSync(

    );

    virtual DFBResult GetVSyncCount(
                    u64                                       *ret_count
    );

    virtual DFBResult TestMixerConfig(
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config,
                    DFBScreenMixerConfigFlags                 *ret_failed
    );

    virtual DFBResult SetMixerConfig(
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config
    );

    virtual DFBResult TestEncoderConfig(
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config,
                    DFBScreenEncoderConfigFlags               *ret_failed
    );

    virtual DFBResult SetEncoderConfig(
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config
    );

    virtual DFBResult TestOutputConfig(
                    u32                                        output,
                    const DFBScreenOutputConfig               *config,
                    DFBScreenOutputConfigFlags                *ret_failed
    );

    virtual DFBResult SetOutputConfig(
                    u32                                        output,
                    const DFBScreenOutputConfig               *config
    );

    virtual DFBResult GetScreenSize(
                    DFBDimension                              *ret_size
    );

    virtual DFBResult GetLayerDimension(
                    CoreLayer                                 *layer,
                    DFBDimension                              *ret_size
    );

};



class IScreen_Requestor : public IScreen
{
private:
    CoreScreen *obj;

public:
    IScreen_Requestor( CoreDFB *core, CoreScreen *obj )
        :
        IScreen( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetPowerMode(
                    DFBScreenPowerMode                         mode
    );

    virtual DFBResult WaitVSync(

    );

    virtual DFBResult GetVSyncCount(
                    u64                                       *ret_count
    );

    virtual DFBResult TestMixerConfig(
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config,
                    DFBScreenMixerConfigFlags                 *ret_failed
    );

    virtual DFBResult SetMixerConfig(
                    u32                                        mixer,
                    const DFBScreenMixerConfig                *config
    );

    virtual DFBResult TestEncoderConfig(
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config,
                    DFBScreenEncoderConfigFlags               *ret_failed
    );

    virtual DFBResult SetEncoderConfig(
                    u32                                        encoder,
                    const DFBScreenEncoderConfig              *config
    );

    virtual DFBResult TestOutputConfig(
                    u32                                        output,
                    const DFBScreenOutputConfig               *config,
                    DFBScreenOutputConfigFlags                *ret_failed
    );

    virtual DFBResult SetOutputConfig(
                    u32                                        output,
                    const DFBScreenOutputConfig               *config
    );

    virtual DFBResult GetScreenSize(
                    DFBDimension                              *ret_size
    );

    virtual DFBResult GetLayerDimension(
                    CoreLayer                                 *layer,
                    DFBDimension                              *ret_size
    );

};


DFBResult CoreScreenDispatch__Dispatch( CoreScreen *obj,
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
