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

#ifndef ___ImageProvider__H___
#define ___ImageProvider__H___

#include "ImageProvider_includes.h"

/**********************************************************************************************************************
 * ImageProvider
 */

#ifdef __cplusplus
#include <core/Interface.h>

extern "C" {
#endif


DFBResult ImageProvider_Dispose(
                    ImageProvider                             *obj
);

DFBResult ImageProvider_GetSurfaceDescription(
                    ImageProvider                             *obj,
                    DFBSurfaceDescription                     *ret_description);

DFBResult ImageProvider_GetImageDescription(
                    ImageProvider                             *obj,
                    DFBImageDescription                       *ret_description);

DFBResult ImageProvider_RenderTo(
                    ImageProvider                             *obj,
                    CoreSurface                               *destination,
                    const DFBRectangle                        *rect);


void ImageProvider_Init_Dispatch(
                    CoreDFB              *core,
                    ImageProviderDispatch *obj,
                    FusionCall           *call
);

void  ImageProvider_Deinit_Dispatch(
                    FusionCall           *call
);


#ifdef __cplusplus
}


namespace DirectFB {


/*
 * ImageProvider Calls
 */
typedef enum {
    ImageProvider_Dispose = 1,
    ImageProvider_GetSurfaceDescription = 2,
    ImageProvider_GetImageDescription = 3,
    ImageProvider_RenderTo = 4,
} ImageProviderCall;

/*
 * ImageProvider_Dispose
 */
typedef struct {
} ImageProviderDispose;

typedef struct {
    DFBResult                                  result;
} ImageProviderDisposeReturn;


/*
 * ImageProvider_GetSurfaceDescription
 */
typedef struct {
} ImageProviderGetSurfaceDescription;

typedef struct {
    DFBResult                                  result;
    DFBSurfaceDescription                      description;
} ImageProviderGetSurfaceDescriptionReturn;


/*
 * ImageProvider_GetImageDescription
 */
typedef struct {
} ImageProviderGetImageDescription;

typedef struct {
    DFBResult                                  result;
    DFBImageDescription                        description;
} ImageProviderGetImageDescriptionReturn;


/*
 * ImageProvider_RenderTo
 */
typedef struct {
    u32                                        destination_id;
    bool                                       rect_set;
    DFBRectangle                               rect;
} ImageProviderRenderTo;

typedef struct {
    DFBResult                                  result;
} ImageProviderRenderToReturn;





class IImageProvider : public Interface
{
public:
    IImageProvider( CoreDFB *core )
        :
        Interface( core )
    {
    }

public:
    virtual DFBResult Dispose(

    ) = 0;

    virtual DFBResult GetSurfaceDescription(
                    DFBSurfaceDescription                     *ret_description
    ) = 0;

    virtual DFBResult GetImageDescription(
                    DFBImageDescription                       *ret_description
    ) = 0;

    virtual DFBResult RenderTo(
                    CoreSurface                               *destination,
                    const DFBRectangle                        *rect
    ) = 0;

};



class IImageProvider_Real : public IImageProvider
{
private:
    ImageProviderDispatch *obj;

public:
    IImageProvider_Real( CoreDFB *core, ImageProviderDispatch *obj )
        :
        IImageProvider( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult Dispose(

    );

    virtual DFBResult GetSurfaceDescription(
                    DFBSurfaceDescription                     *ret_description
    );

    virtual DFBResult GetImageDescription(
                    DFBImageDescription                       *ret_description
    );

    virtual DFBResult RenderTo(
                    CoreSurface                               *destination,
                    const DFBRectangle                        *rect
    );

};



class IImageProvider_Requestor : public IImageProvider
{
private:
    ImageProvider *obj;

public:
    IImageProvider_Requestor( CoreDFB *core, ImageProvider *obj )
        :
        IImageProvider( core ),
        obj( obj )
    {
    }

public:
    virtual DFBResult Dispose(

    );

    virtual DFBResult GetSurfaceDescription(
                    DFBSurfaceDescription                     *ret_description
    );

    virtual DFBResult GetImageDescription(
                    DFBImageDescription                       *ret_description
    );

    virtual DFBResult RenderTo(
                    CoreSurface                               *destination,
                    const DFBRectangle                        *rect
    );

};


DFBResult ImageProviderDispatch__Dispatch( ImageProviderDispatch *obj,
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
