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

#ifndef ___CoreWindowStack__H___
#define ___CoreWindowStack__H___

#include <core/CoreWindowStack_includes.h>

/**********************************************************************************************************************
 * CoreWindowStack
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult CoreWindowStack_RepaintAll(
                    CoreWindowStack                           *obj
);

DFBResult CoreWindowStack_GetInsets(
                    CoreWindowStack                           *obj,
                    CoreWindow                                *window,
                    DFBInsets                                 *ret_insets);

DFBResult CoreWindowStack_CursorEnable(
                    CoreWindowStack                           *obj,
                    bool                                       enable);

DFBResult CoreWindowStack_CursorSetShape(
                    CoreWindowStack                           *obj,
                    CoreSurface                               *shape,
                    const DFBPoint                            *hotspot);

DFBResult CoreWindowStack_CursorSetOpacity(
                    CoreWindowStack                           *obj,
                    u8                                         opacity);

DFBResult CoreWindowStack_CursorSetAcceleration(
                    CoreWindowStack                           *obj,
                    u32                                        numerator,
                    u32                                        denominator,
                    u32                                        threshold);

DFBResult CoreWindowStack_CursorWarp(
                    CoreWindowStack                           *obj,
                    const DFBPoint                            *position);

DFBResult CoreWindowStack_CursorGetPosition(
                    CoreWindowStack                           *obj,
                    DFBPoint                                  *ret_position);

DFBResult CoreWindowStack_BackgroundSetMode(
                    CoreWindowStack                           *obj,
                    DFBDisplayLayerBackgroundMode              mode);

DFBResult CoreWindowStack_BackgroundSetImage(
                    CoreWindowStack                           *obj,
                    CoreSurface                               *image);

DFBResult CoreWindowStack_BackgroundSetColor(
                    CoreWindowStack                           *obj,
                    const DFBColor                            *color);

DFBResult CoreWindowStack_BackgroundSetColorIndex(
                    CoreWindowStack                           *obj,
                    s32                                        index);


void CoreWindowStack_Init_Dispatch(
                    CoreDFB              *core,
                    CoreWindowStack      *obj,
                    FusionCall           *call
);

void  CoreWindowStack_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * CoreWindowStack Calls
 */
typedef enum {
    CoreWindowStack_RepaintAll = 1,
    CoreWindowStack_GetInsets = 2,
    CoreWindowStack_CursorEnable = 3,
    CoreWindowStack_CursorSetShape = 4,
    CoreWindowStack_CursorSetOpacity = 5,
    CoreWindowStack_CursorSetAcceleration = 6,
    CoreWindowStack_CursorWarp = 7,
    CoreWindowStack_CursorGetPosition = 8,
    CoreWindowStack_BackgroundSetMode = 9,
    CoreWindowStack_BackgroundSetImage = 10,
    CoreWindowStack_BackgroundSetColor = 11,
    CoreWindowStack_BackgroundSetColorIndex = 12,
} CoreWindowStackCall;

/*
 * CoreWindowStack_RepaintAll
 */
typedef struct {
} CoreWindowStackRepaintAll;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackRepaintAllReturn;


/*
 * CoreWindowStack_GetInsets
 */
typedef struct {
    u32                                        window_id;
} CoreWindowStackGetInsets;

typedef struct {
    DFBResult                                  result;
    DFBInsets                                  insets;
} CoreWindowStackGetInsetsReturn;


/*
 * CoreWindowStack_CursorEnable
 */
typedef struct {
    bool                                       enable;
} CoreWindowStackCursorEnable;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackCursorEnableReturn;


/*
 * CoreWindowStack_CursorSetShape
 */
typedef struct {
    u32                                        shape_id;
    DFBPoint                                   hotspot;
} CoreWindowStackCursorSetShape;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackCursorSetShapeReturn;


/*
 * CoreWindowStack_CursorSetOpacity
 */
typedef struct {
    u8                                         opacity;
} CoreWindowStackCursorSetOpacity;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackCursorSetOpacityReturn;


/*
 * CoreWindowStack_CursorSetAcceleration
 */
typedef struct {
    u32                                        numerator;
    u32                                        denominator;
    u32                                        threshold;
} CoreWindowStackCursorSetAcceleration;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackCursorSetAccelerationReturn;


/*
 * CoreWindowStack_CursorWarp
 */
typedef struct {
    DFBPoint                                   position;
} CoreWindowStackCursorWarp;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackCursorWarpReturn;


/*
 * CoreWindowStack_CursorGetPosition
 */
typedef struct {
} CoreWindowStackCursorGetPosition;

typedef struct {
    DFBResult                                  result;
    DFBPoint                                   position;
} CoreWindowStackCursorGetPositionReturn;


/*
 * CoreWindowStack_BackgroundSetMode
 */
typedef struct {
    DFBDisplayLayerBackgroundMode              mode;
} CoreWindowStackBackgroundSetMode;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackBackgroundSetModeReturn;


/*
 * CoreWindowStack_BackgroundSetImage
 */
typedef struct {
    u32                                        image_id;
} CoreWindowStackBackgroundSetImage;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackBackgroundSetImageReturn;


/*
 * CoreWindowStack_BackgroundSetColor
 */
typedef struct {
    DFBColor                                   color;
} CoreWindowStackBackgroundSetColor;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackBackgroundSetColorReturn;


/*
 * CoreWindowStack_BackgroundSetColorIndex
 */
typedef struct {
    s32                                        index;
} CoreWindowStackBackgroundSetColorIndex;

typedef struct {
    DFBResult                                  result;
} CoreWindowStackBackgroundSetColorIndexReturn;





class IWindowStack : public Interface
{
public:
    IWindowStack( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult RepaintAll(

    ) = 0;

    virtual DFBResult GetInsets(
                    CoreWindow                                *window,
                    DFBInsets                                 *ret_insets
    ) = 0;

    virtual DFBResult CursorEnable(
                    bool                                       enable
    ) = 0;

    virtual DFBResult CursorSetShape(
                    CoreSurface                               *shape,
                    const DFBPoint                            *hotspot
    ) = 0;

    virtual DFBResult CursorSetOpacity(
                    u8                                         opacity
    ) = 0;

    virtual DFBResult CursorSetAcceleration(
                    u32                                        numerator,
                    u32                                        denominator,
                    u32                                        threshold
    ) = 0;

    virtual DFBResult CursorWarp(
                    const DFBPoint                            *position
    ) = 0;

    virtual DFBResult CursorGetPosition(
                    DFBPoint                                  *ret_position
    ) = 0;

    virtual DFBResult BackgroundSetMode(
                    DFBDisplayLayerBackgroundMode              mode
    ) = 0;

    virtual DFBResult BackgroundSetImage(
                    CoreSurface                               *image
    ) = 0;

    virtual DFBResult BackgroundSetColor(
                    const DFBColor                            *color
    ) = 0;

    virtual DFBResult BackgroundSetColorIndex(
                    s32                                        index
    ) = 0;

};



class IWindowStack_Real : public IWindowStack
{
private:
    CoreWindowStack *obj;

public:
    IWindowStack_Real( CoreDFB *core, CoreWindowStack *obj )
        :
        IWindowStack( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult RepaintAll(

    );

    virtual DFBResult GetInsets(
                    CoreWindow                                *window,
                    DFBInsets                                 *ret_insets
    );

    virtual DFBResult CursorEnable(
                    bool                                       enable
    );

    virtual DFBResult CursorSetShape(
                    CoreSurface                               *shape,
                    const DFBPoint                            *hotspot
    );

    virtual DFBResult CursorSetOpacity(
                    u8                                         opacity
    );

    virtual DFBResult CursorSetAcceleration(
                    u32                                        numerator,
                    u32                                        denominator,
                    u32                                        threshold
    );

    virtual DFBResult CursorWarp(
                    const DFBPoint                            *position
    );

    virtual DFBResult CursorGetPosition(
                    DFBPoint                                  *ret_position
    );

    virtual DFBResult BackgroundSetMode(
                    DFBDisplayLayerBackgroundMode              mode
    );

    virtual DFBResult BackgroundSetImage(
                    CoreSurface                               *image
    );

    virtual DFBResult BackgroundSetColor(
                    const DFBColor                            *color
    );

    virtual DFBResult BackgroundSetColorIndex(
                    s32                                        index
    );

};



class IWindowStack_Requestor : public IWindowStack
{
private:
    CoreWindowStack *obj;

public:
    IWindowStack_Requestor( CoreDFB *core, CoreWindowStack *obj )
        :
        IWindowStack( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult RepaintAll(

    );

    virtual DFBResult GetInsets(
                    CoreWindow                                *window,
                    DFBInsets                                 *ret_insets
    );

    virtual DFBResult CursorEnable(
                    bool                                       enable
    );

    virtual DFBResult CursorSetShape(
                    CoreSurface                               *shape,
                    const DFBPoint                            *hotspot
    );

    virtual DFBResult CursorSetOpacity(
                    u8                                         opacity
    );

    virtual DFBResult CursorSetAcceleration(
                    u32                                        numerator,
                    u32                                        denominator,
                    u32                                        threshold
    );

    virtual DFBResult CursorWarp(
                    const DFBPoint                            *position
    );

    virtual DFBResult CursorGetPosition(
                    DFBPoint                                  *ret_position
    );

    virtual DFBResult BackgroundSetMode(
                    DFBDisplayLayerBackgroundMode              mode
    );

    virtual DFBResult BackgroundSetImage(
                    CoreSurface                               *image
    );

    virtual DFBResult BackgroundSetColor(
                    const DFBColor                            *color
    );

    virtual DFBResult BackgroundSetColorIndex(
                    s32                                        index
    );

};


DFBResult CoreWindowStackDispatch__Dispatch( CoreWindowStack *obj,
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
