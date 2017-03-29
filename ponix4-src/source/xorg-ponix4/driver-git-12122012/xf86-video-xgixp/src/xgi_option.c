/***************************************************************************
 * Copyright (C) 2003-2006 by XGI Technology, Taiwan.			   *
 *									   *
 * All Rights Reserved.							   *
 *									   *
 * Permission is hereby granted, free of charge, to any person obtaining   *
 * a copy of this software and associated documentation files (the	   *
 * "Software"), to deal in the Software without restriction, including	   *
 * without limitation on the rights to use, copy, modify, merge,	   *
 * publish, distribute, sublicense, and/or sell copies of the Software,	   *
 * and to permit persons to whom the Software is furnished to do so,	   *
 * subject to the following conditions:					   *
 *									   *
 * The above copyright notice and this permission notice (including the	   *
 * next paragraph) shall be included in all copies or substantial	   *
 * portions of the Software.						   *
 *									   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	   *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF	   *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND		   *
 * NON-INFRINGEMENT.  IN NO EVENT SHALL XGI AND/OR			   *
 * ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,	   *
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER	   *
 * DEALINGS IN THE SOFTWARE.						   *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"

#include "xgi.h"
#include "xgi_regs.h"
#include "xgi_option.h"

const OptionInfoRec XGIOptions[] = {
    { OPTION_SW_CURSOR,         "SWcursor",         OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_NOACCEL,           "NoAccel",          OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_SHADOW_FB,         "ShadowFB",         OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_ROTATE,            "Rotate",           OPTV_ANYSTR,    {0}, FALSE },
    { OPTION_VIDEO_KEY,         "VideoKey",         OPTV_INTEGER,   {0}, FALSE },
    { OPTION_NOMMIO,            "NoMMIO",           OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_MMIO_ONLY,         "MMIOonly",         OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_DAC_8BIT,          "8Bit Dac",         OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_NEED_SHADOW,       "shadow",           OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_NEED_STRETCH,      "stretch",          OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_MUX_THRESHOLD,     "MUXThreshold",     OPTV_INTEGER,   {0}, FALSE },
    { OPTION_DISPLAY,           "Display",          OPTV_ANYSTR,    {0}, FALSE },
    { OPTION_GAMMA_BRIGHT,      "GammaBrightness",  OPTV_ANYSTR,    {0}, FALSE },
#ifdef __DEBUG_FUNC__
    { OPTION_SOLID_LINE,        "SolidLine",        OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_SOLID_FILL,        "SolidFill",        OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_SRC_TO_SRC,        "Src2Src",          OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_8X8_PATTERN,       "8x8Pattern",       OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_8X8_MONOPAT,       "8x8MonoPat",       OPTV_BOOLEAN,   {0}, FALSE },
    { OPTION_IMAGE_WRITE,       "ImageWrite",       OPTV_BOOLEAN,   {0}, FALSE },
#endif
    { -1,                        NULL,              OPTV_NONE,      {0}, FALSE }
};

const unsigned long XGIOptionSize = sizeof(XGIOptions);

/*
 * XGIAvailableOptions --
 *
 * Return recognised options that are intended for public consumption.
 */
const OptionInfoRec *XGIAvailableOptions(int chipid, int busid)
{
    return XGIOptions;
}

Bool XGIProcessOptions(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    MessageType from;
    char        *s;

    /* Collect all of the option flags (fill in pScrn->options) */
    /* Collect the options from each of the config file sections used
     * by the screen (pScrn) and return the merged list as pScrn->options.
     * This function requires that pScrn->confScreen, pScrn->display,
     * pScrn->monitor, pScrn->numEntities, and pScrn->entityList are initialised.
     * extraOpts may optionally be set to an additional list of options
     * to be combined with the others. The order of precedence for options is
     * extraOpts, display, confScreen, monitor, device.
     */

    xf86CollectOptions(pScrn, NULL);

    if (!(pXGI->pOptionInfo = malloc(XGIOptionSize)))
        return FALSE;
    memcpy(pXGI->pOptionInfo, XGIOptions, XGIOptionSize);

    /*
     * Process the options based on the information in XGIOptions.
     * The results are written to pXGI->pOptionInfo. If all of the options
     * processing is done within this function a local variable "options"
     * can be used instead of pXGI->Options.
     */
    xf86ProcessOptions(pScrn->scrnIndex, pScrn->options, pXGI->pOptionInfo);

    /*
     * Set various fields of ScrnInfoRec and/or XGIRec based on
     * the options found.
     */

    from = X_DEFAULT;
    pXGI->isHWCursor = TRUE;
    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_SW_CURSOR, FALSE))
    {
        from = X_CONFIG;
        pXGI->isHWCursor = FALSE;
    }

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_NOACCEL, FALSE))
    {
        pXGI->noAccel = TRUE;
        xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Acceleration disabled\n");
    }

#ifdef XvExtension
    if(xf86GetOptValInteger(pXGI->pOptionInfo, OPTION_VIDEO_KEY, &(pXGI->videoKey)))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "video key set to 0x%x\n",
                   pXGI->videoKey);
    }
    else
    {
        pXGI->videoKey =  (1 << pScrn->offset.red) | (1 << pScrn->offset.green)
                           | (((pScrn->mask.blue >> pScrn->offset.blue) - 1) << pScrn->offset.blue);
    }
#endif

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_NOMMIO, FALSE))
    {
        pXGI->noMMIO = TRUE;
        xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "MMIO Disabled\n");
    }
    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_MMIO_ONLY, FALSE))
    {
        if (pXGI->noMMIO)
        {
            pXGI->noAccel = TRUE;
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "MMIO only cannot be set with NoMMIO\n");
        }
        else
        {
            pXGI->isMmioOnly = TRUE;
            xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "MMIO only enabled\n");
        }
    }

    pXGI->displayDevice = 0;

	/* Jong 11/09/2006; pXGI->displayDevice is still 0 after this cheking */
    if ((s = xf86GetOptValString(pXGI->pOptionInfo, OPTION_DISPLAY)))
    {
        if(!xf86NameCmp(s, "CRT"))
        {
            pXGI->displayDevice = ST_DISP_CRT;
            xf86DrvMsg(pScrn->scrnIndex,X_CONFIG, "LCD off CRT on\n");
        }
        else if (!xf86NameCmp(s, "LCD"))
        {
            pXGI->displayDevice = ST_DISP_LCD;
            xf86DrvMsg(pScrn->scrnIndex,X_CONFIG, "LCD on CRT off\n");
        }
        else if (!xf86NameCmp(s, "TV"))
        {
            pXGI->displayDevice = ST_DISP_TV;
            xf86DrvMsg(pScrn->scrnIndex,X_CONFIG, "TV on\n");
        }
        else if (!xf86NameCmp(s, "DVI"))
        {
            pXGI->displayDevice = ST_DISP_DVI;
            xf86DrvMsg(pScrn->scrnIndex,X_CONFIG, "DVI on\n");
        }
        else
        {
            xf86DrvMsg(pScrn->scrnIndex,X_ERROR,
                       "%s is an unknown display option\n",s);
        }
    }

    if ((s = xf86GetOptValString(pXGI->pOptionInfo, OPTION_GAMMA_BRIGHT)))
    {
        int     brightness = -1;
        double  gamma = -1.0;
        Bool    error = FALSE;
        int     i;

        i = sscanf(s,"%lf %i",&gamma,&brightness);

        if (i != 2 || brightness == -1 || gamma == -1.0)
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Invalid Gamma/Brightness argument: %s\n", s);
            error = TRUE;
        }
        else
        {
            if (brightness < 0 || brightness > 128)
            {
                xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "brightness out of range [0,128]: %i\n",brightness);
                error = TRUE;
            }
            if (gamma <= 0.0 || gamma > 10.0)
            {
                xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "gamma out of range (0,10.0]: %f\n",gamma);
                error = TRUE;
            }
        }

        if (!error)
        {
            pXGI->isGammaBrightnessOn = TRUE;
            pXGI->gamma = gamma;
            pXGI->brightness = brightness;
            xf86DrvMsg(pScrn->scrnIndex,X_CONFIG,"Gamma: %f Brightness: %i\n", gamma, brightness);
        }
    }

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_NEED_SHADOW, FALSE))
    {
        pXGI->isNeedShadow = TRUE;
        xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Shadow enabled\n");
    }
    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_NEED_STRETCH, FALSE))
    {
        pXGI->isNeedStretch = TRUE;
        xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Stretch enabled\n");
    }

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_SHADOW_FB, FALSE))
    {
        if (pScrn->depth < 8)
        {
            xf86DrvMsg(pScrn->scrnIndex, X_WARNING, "Ignoring Option SHADOW_FB when depth < 8");
        }
        else
        {
            pXGI->isShadowFB = TRUE;
            pXGI->noAccel = TRUE;
            xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Using Shadow Framebuffer - acceleration disabled\n");
        }
    }

    pXGI->rotate = 0;
    if ((s = xf86GetOptValString(pXGI->pOptionInfo, OPTION_ROTATE)))
    {
        if (pScrn->depth < 8)
        {
            xf86DrvMsg(pScrn->scrnIndex, X_WARNING, "Ignoring Option ROTATE when depth < 8");
        }
        else
        {
            if(!xf86NameCmp(s, "CW"))
            {
                /* accel is disabled below for shadowFB */
                pXGI->isShadowFB = TRUE;
                pXGI->noAccel = TRUE;
                pXGI->isHWCursor = FALSE;
                pXGI->rotate = 1;
                xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Rotating screen clockwise - acceleration disabled\n");
            }
            else if(!xf86NameCmp(s, "CCW"))
            {
                pXGI->isShadowFB = TRUE;
                pXGI->noAccel = TRUE;
                pXGI->isHWCursor = FALSE;
                pXGI->rotate = -1;
                xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "Rotating screen counter clockwise - acceleration disabled\n");
            }
            else
            {
                xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "\"%s\" is not a valid value for Option \"Rotate\"\n", s);
                xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Valid options are \"CW\" or \"CCW\"\n");
            }
        }
    }

#ifdef __DEBUG_FUNC__
    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_SOLID_LINE, TRUE))
        pXGI->EnableSolidLine = TRUE;
    else
        pXGI->EnableSolidLine = FALSE;

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_SOLID_FILL, TRUE))
        pXGI->EnableSolidFill = TRUE;
    else
        pXGI->EnableSolidFill = FALSE;

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_SRC_TO_SRC, TRUE))
        pXGI->EnableSrc2Src = TRUE;
    else
        pXGI->EnableSrc2Src = FALSE;

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_8X8_PATTERN, TRUE))
        pXGI->Enable8x8Pattern = TRUE;
    else
        pXGI->Enable8x8Pattern = FALSE;

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_8X8_MONOPAT, TRUE))
        pXGI->Enable8x8MonoPat = TRUE;
    else
        pXGI->Enable8x8MonoPat = FALSE;

    if (xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_IMAGE_WRITE, TRUE))
        pXGI->EnableImageWrite = TRUE;
    else
        pXGI->EnableImageWrite = FALSE;
#endif

    return TRUE;
}
