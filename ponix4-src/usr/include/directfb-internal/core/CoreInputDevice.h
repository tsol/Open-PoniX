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

#ifndef ___CoreInputDevice__H___
#define ___CoreInputDevice__H___

#include <core/CoreInputDevice_includes.h>

/**********************************************************************************************************************
 * CoreInputDevice
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreInputDevice_SetKeymapEntry(
                    CoreInputDevice                           *obj,
                    s32                                        key_code,
                    const DFBInputDeviceKeymapEntry           *entry);

DFBResult CoreInputDevice_ReloadKeymap(
                    CoreInputDevice                           *obj
);

DFBResult CoreInputDevice_SetConfiguration(
                    CoreInputDevice                           *obj,
                    const DFBInputDeviceConfig                *config);


void CoreInputDevice_Init_Dispatch(
                    CoreDFB              *core,
                    CoreInputDevice      *obj,
                    FusionCall           *call
);

void  CoreInputDevice_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreInputDevice Calls
 */
typedef enum {
    CoreInputDevice_SetKeymapEntry = 1,
    CoreInputDevice_ReloadKeymap = 2,
    CoreInputDevice_SetConfiguration = 3,
} CoreInputDeviceCall;

/*
 * CoreInputDevice_SetKeymapEntry
 */
typedef struct {
    s32                                        key_code;
    DFBInputDeviceKeymapEntry                  entry;
} CoreInputDeviceSetKeymapEntry;

typedef struct {
    DFBResult                                  result;
} CoreInputDeviceSetKeymapEntryReturn;


/*
 * CoreInputDevice_ReloadKeymap
 */
typedef struct {
} CoreInputDeviceReloadKeymap;

typedef struct {
    DFBResult                                  result;
} CoreInputDeviceReloadKeymapReturn;


/*
 * CoreInputDevice_SetConfiguration
 */
typedef struct {
    DFBInputDeviceConfig                       config;
} CoreInputDeviceSetConfiguration;

typedef struct {
    DFBResult                                  result;
} CoreInputDeviceSetConfigurationReturn;





class IInputDevice : public Interface
{
public:
    IInputDevice( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult SetKeymapEntry(
                    s32                                        key_code,
                    const DFBInputDeviceKeymapEntry           *entry
    ) = 0;

    virtual DFBResult ReloadKeymap(

    ) = 0;

    virtual DFBResult SetConfiguration(
                    const DFBInputDeviceConfig                *config
    ) = 0;

};



class IInputDevice_Real : public IInputDevice
{
private:
    CoreInputDevice *obj;

public:
    IInputDevice_Real( CoreDFB *core, CoreInputDevice *obj )
        :
        IInputDevice( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetKeymapEntry(
                    s32                                        key_code,
                    const DFBInputDeviceKeymapEntry           *entry
    );

    virtual DFBResult ReloadKeymap(

    );

    virtual DFBResult SetConfiguration(
                    const DFBInputDeviceConfig                *config
    );

};



class IInputDevice_Requestor : public IInputDevice
{
private:
    CoreInputDevice *obj;

public:
    IInputDevice_Requestor( CoreDFB *core, CoreInputDevice *obj )
        :
        IInputDevice( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult SetKeymapEntry(
                    s32                                        key_code,
                    const DFBInputDeviceKeymapEntry           *entry
    );

    virtual DFBResult ReloadKeymap(

    );

    virtual DFBResult SetConfiguration(
                    const DFBInputDeviceConfig                *config
    );

};


DFBResult CoreInputDeviceDispatch__Dispatch( CoreInputDevice *obj,
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
