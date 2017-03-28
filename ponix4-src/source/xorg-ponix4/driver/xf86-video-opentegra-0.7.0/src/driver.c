/*
 * Copyright 2008 Tungsten Graphics, Inc., Cedar Park, Texas.
 * Copyright 2011 Dave Airlie
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Original Author: Alan Hourihane <alanh@tungstengraphics.com>
 * Rewrite: Dave Airlie <airlied@redhat.com> 
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <fcntl.h>

#include "xf86.h"
#include "xf86_OSproc.h"
#include "compiler.h"
#ifdef XSERVER_PLATFORM_BUS
#include "xf86platformBus.h"
#endif
#include "mipointer.h"
#include "micmap.h"

#include <X11/extensions/randr.h>

#include "dixstruct.h"
#include "scrnintstr.h"

#include "fb.h"
#include "shadow.h"

#include "xorg-server.h"
#include "xf86Crtc.h"
#include "xf86drm.h"
#include "xf86drmMode.h"

#include "compat-api.h"
#include "driver.h"

static SymTabRec Chipsets[] = {
    { 0, "kms" },
    { -1, NULL }
};

typedef enum
{
    OPTION_SW_CURSOR,
    OPTION_DEVICE_PATH,
    OPTION_SHADOW_FB,
} TegraOptions;

static const OptionInfoRec Options[] = {
    { OPTION_SW_CURSOR, "SWcursor", OPTV_BOOLEAN, { 0 }, FALSE },
    { OPTION_DEVICE_PATH, "device", OPTV_STRING, { 0 }, FALSE },
    { OPTION_SHADOW_FB, "ShadowFB", OPTV_BOOLEAN, { 0 }, FALSE },
    { -1, NULL, OPTV_NONE, { 0 }, FALSE }
};

int tegraEntityIndex = -1;

static int
dispatch_dirty_region(ScrnInfoPtr scrn, PixmapPtr pixmap, DamagePtr damage,
                      int fb_id)
{
    TegraPtr tegra = TegraPTR(scrn);
    RegionPtr dirty = DamageRegion(damage);
    unsigned num_cliprects = REGION_NUM_RECTS(dirty);

    if (num_cliprects) {
        drmModeClip *clip = malloc(num_cliprects * sizeof(drmModeClip));
        BoxPtr rect = REGION_RECTS(dirty);
        int i, ret;

        if (!clip)
            return -ENOMEM;

        /* XXX no need for copy? */
        for (i = 0; i < num_cliprects; i++, rect++) {
            clip[i].x1 = rect->x1;
            clip[i].y1 = rect->y1;
            clip[i].x2 = rect->x2;
            clip[i].y2 = rect->y2;
        }

        /* TODO query connector property to see if this is needed */
        ret = drmModeDirtyFB(tegra->fd, fb_id, clip, num_cliprects);
        free(clip);
        DamageEmpty(damage);
        if (ret) {
            if (ret == -EINVAL)
                return ret;
        }
    }

    return 0;
}

static void dispatch_dirty(ScreenPtr pScreen)
{
    ScrnInfoPtr scrn = xf86ScreenToScrn(pScreen);
    TegraPtr tegra = TegraPTR(scrn);
    PixmapPtr pixmap = pScreen->GetScreenPixmap(pScreen);
    int fb_id = tegra->drmmode.fb_id;
    int ret;

    ret = dispatch_dirty_region(scrn, pixmap, tegra->damage, fb_id);
    if (ret == -EINVAL || ret == -ENOSYS) {
        tegra->dirty_enabled = FALSE;
        DamageUnregister(&pScreen->GetScreenPixmap(pScreen)->drawable,
                         tegra->damage);
        DamageDestroy(tegra->damage);
        tegra->damage = NULL;
        xf86DrvMsg(scrn->scrnIndex, X_INFO, "Disabling kernel dirty updates, not required.\n");
        return;
    }
}

#ifdef TEGRA_OUTPUT_SLAVE_SUPPORT
static void dispatch_dirty_crtc(ScrnInfoPtr scrn, xf86CrtcPtr crtc)
{
    TegraPtr tegra = TegraPTR(scrn);
    PixmapPtr pixmap = crtc->randr_crtc->scanout_pixmap;
    TegraPixmapPrivPtr ppriv = TegraGetPixmapPriv(&tegra->drmmode, pixmap);
    drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
    DamagePtr damage = drmmode_crtc->slave_damage;
    int fb_id = ppriv->fb_id;
    int ret;

    ret = dispatch_dirty_region(scrn, pixmap, damage, fb_id);
    if (ret) {
    }
}

static void dispatch_slave_dirty(ScreenPtr pScreen)
{
    ScrnInfoPtr scrn = xf86ScreenToScrn(pScreen);
    xf86CrtcConfigPtr xf86_config = XF86_CRTC_CONFIG_PTR(scrn);
    int c;

    for (c = 0; c < xf86_config->num_crtc; c++) {
        xf86CrtcPtr crtc = xf86_config->crtc[c];

        if (!crtc->randr_crtc)
            continue;

        if (!crtc->randr_crtc->scanout_pixmap)
            continue;

        dispatch_dirty_crtc(scrn, crtc);
    }
}
#endif

static Bool
GetRec(ScrnInfoPtr pScrn)
{
    if (pScrn->driverPrivate)
        return TRUE;

    pScrn->driverPrivate = xnfcalloc(sizeof(TegraRec), 1);

    return TRUE;
}

static void
FreeRec(ScrnInfoPtr pScrn)
{
    if (!pScrn)
        return;

    if (!pScrn->driverPrivate)
        return;

    free(pScrn->driverPrivate);

    pScrn->driverPrivate = NULL;
}

#ifdef TEGRA_OUTPUT_SLAVE_SUPPORT
static Bool
TegraSetSharedPixmapBacking(PixmapPtr ppix, void *fd_handle)
{
    ScreenPtr screen = ppix->drawable.pScreen;
    ScrnInfoPtr scrn = xf86ScreenToScrn(screen);
    TegraPtr tegra = TegraPTR(scrn);
    Bool ret;
    int size = ppix->devKind * ppix->drawable.height;
    int ihandle = (int)(long)fd_handle;

    ret = drmmode_SetSlaveBO(ppix, &tegra->drmmode, ihandle, ppix->devKind,
                             size);
    if (ret == FALSE)
        return ret;

    return TRUE;
}
#endif

static void
TegraIdentify(int flags)
{
    xf86PrintChipsets("opentegra", "Open Source Driver for NVIDIA Tegra",
                      Chipsets);
}

static int
TegraOpenHardware(const char *dev)
{
    int fd;

    if (dev)
        fd = open(dev, O_RDWR, 0);
    else {
        dev = getenv("KMSDEVICE");
        if ((dev == NULL) || ((fd = open(dev, O_RDWR, 0)) == -1)) {
            dev = "/dev/dri/card0";
            fd = open(dev,O_RDWR, 0);
        }
    }

    if (fd == -1)
        xf86DrvMsg(-1, X_ERROR, "open %s: %s\n", dev, strerror(errno));

    return fd;
}

static Bool
TegraProbeHardware(const char *dev)
{
    int fd;

    fd = TegraOpenHardware(dev);
    if (fd != -1) {
        close(fd);
        return TRUE;
    }

    return FALSE;
}

static const OptionInfoRec *
TegraAvailableOptions(int chipid, int busid)
{
    return Options;
}

static Bool
TegraDriverFunc(ScrnInfoPtr pScrn, xorgDriverFuncOp op, void *data)
{
    xorgHWFlags *flag;

    switch (op) {
        case GET_REQUIRED_HW_INTERFACES:
            flag = (CARD32 *)data;
            (*flag) = 0;
            return TRUE;
        default:
            return FALSE;
    }
}

static Bool
TegraPreInit(ScrnInfoPtr pScrn, int flags)
{
    TegraPtr tegra;
    rgb defaultWeight = { 0, 0, 0 };
    EntityInfoPtr pEnt;
    EntPtr tegraEnt = NULL;
    Bool prefer_shadow = TRUE;
    uint64_t value = 0;
    int ret;
    int bppflags;
    int defaultdepth, defaultbpp;
    Gamma zeros = { 0.0, 0.0, 0.0 };
    const char *path;

    if (pScrn->numEntities != 1)
        return FALSE;

    pEnt = xf86GetEntityInfo(pScrn->entityList[0]);

    if (flags & PROBE_DETECT)
        return FALSE;

    /* Allocate driverPrivate */
    if (!GetRec(pScrn))
        return FALSE;

    tegra = TegraPTR(pScrn);
    tegra->pEnt = pEnt;

    pScrn->displayWidth = 640; /* default it */

    /* Allocate an entity private if necessary */
    if (xf86IsEntityShared(pScrn->entityList[0])) {
        tegraEnt = xf86GetEntityPrivate(pScrn->entityList[0],
                                        tegraEntityIndex)->ptr;
        tegra->entityPrivate = tegraEnt;
    } else
        tegra->entityPrivate = NULL;

    if (xf86IsEntityShared(pScrn->entityList[0])) {
        if (xf86IsPrimInitDone(pScrn->entityList[0])) {
            /* do something */
        } else {
            xf86SetPrimInitDone(pScrn->entityList[0]);
        }
    }

    pScrn->monitor = pScrn->confScreen->monitor;
    pScrn->progClock = TRUE;
    pScrn->rgbBits = 8;

    switch (pEnt->location.type) {
#ifdef XSERVER_PLATFORM_BUS
    case BUS_PLATFORM:
        path = xf86_get_platform_device_attrib(pEnt->location.id.plat,
                                               ODEV_ATTRIB_PATH);
        break;
#endif

    default:
        path = xf86GetOptValString(tegra->pEnt->device->options,
                                   OPTION_DEVICE_PATH);
        break;
    }

    tegra->fd = TegraOpenHardware(path);
    if (tegra->fd < 0)
        return FALSE;

    tegra->drmmode.fd = tegra->fd;

#ifdef TEGRA_OUTPUT_SLAVE_SUPPORT
    pScrn->capabilities = 0;
#ifdef DRM_CAP_PRIME
    ret = drmGetCap(tegra->fd, DRM_CAP_PRIME, &value);
    if (ret == 0) {
        if (value & DRM_PRIME_CAP_IMPORT)
            pScrn->capabilities |= RR_Capability_SinkOutput;
    }
#endif
#endif
    drmmode_get_default_bpp(pScrn, &tegra->drmmode, &defaultdepth, &defaultbpp);
    if (defaultdepth == 24 && defaultbpp == 24)
        bppflags = Support24bppFb;
    else
        bppflags = PreferConvert24to32 | SupportConvert24to32 | Support32bppFb;

    if (!xf86SetDepthBpp(pScrn, defaultdepth, defaultdepth, defaultbpp,
                         bppflags))
        return FALSE;

    switch (pScrn->depth) {
    case 15:
    case 16:
    case 24:
        break;

    default:
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "Given depth (%d) is not supported by the driver\n",
                   pScrn->depth);
        return FALSE;
    }

    xf86PrintDepthBpp(pScrn);

    /* Process the options */
    xf86CollectOptions(pScrn, NULL);

    tegra->Options = malloc(sizeof(Options));
    if (!tegra->Options)
        return FALSE;

    memcpy(tegra->Options, Options, sizeof(Options));
    xf86ProcessOptions(pScrn->scrnIndex, pScrn->options, tegra->Options);

    if (!xf86SetWeight(pScrn, defaultWeight, defaultWeight))
        return FALSE;

    if (!xf86SetDefaultVisual(pScrn, -1))
        return FALSE;

    if (xf86ReturnOptValBool(tegra->Options, OPTION_SW_CURSOR, FALSE))
        tegra->drmmode.want_sw_cursor = TRUE;

    ret = drmGetCap(tegra->fd, DRM_CAP_DUMB_PREFER_SHADOW, &value);
    if (!ret)
        prefer_shadow = !!value;

    tegra->drmmode.shadow_enable = xf86ReturnOptValBool(tegra->Options,
                                                        OPTION_SHADOW_FB,
                                                        prefer_shadow);

    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
               "ShadowFB: preferred %s, enabled %s\n",
               prefer_shadow ? "YES" : "NO",
               tegra->drmmode.shadow_enable ? "YES" : "NO");

    if (!drmmode_pre_init(pScrn, &tegra->drmmode, pScrn->bitsPerPixel / 8)) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "KMS setup failed\n");
        return FALSE;
    }

    if (tegra->drmmode.need_sw_cursor)
        tegra->drmmode.want_sw_cursor = TRUE;

    /*
     * If the driver can do gamma correction, it should call xf86SetGamma() here.
     */
    if (!xf86SetGamma(pScrn, zeros))
        return FALSE;

    if (pScrn->modes == NULL) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No modes.\n");
        return FALSE;
    }

    pScrn->currentMode = pScrn->modes;

    /* Set display resolution */
    xf86SetDpi(pScrn, 0, 0);

    /* Load the required sub modules */
    if (!xf86LoadSubModule(pScrn, "fb"))
        return FALSE;

    if (tegra->drmmode.shadow_enable) {
        if (!xf86LoadSubModule(pScrn, "shadow"))
            return FALSE;
    }

    return TRUE;
}

/*
 * This gets called when gaining control of the VT, and from ScreenInit().
 */
static Bool
TegraEnterVT(VT_FUNC_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);
    TegraPtr tegra = TegraPTR(pScrn);

    pScrn->vtSema = TRUE;

    if (drmSetMaster(tegra->fd))
        xf86DrvMsg(pScrn->scrnIndex, X_WARNING, "drmSetMaster failed: %s\n",
                   strerror(errno));

    if (!drmmode_set_desired_modes(pScrn, &tegra->drmmode))
        return FALSE;

    return TRUE;
}

static void
TegraLeaveVT(VT_FUNC_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);
    TegraPtr tegra = TegraPTR(pScrn);
    xf86_hide_cursors(pScrn);

    pScrn->vtSema = FALSE;

    drmDropMaster(tegra->fd);
}

static Bool
TegraShadowInit(ScreenPtr pScreen)
{
    if (!shadowSetup(pScreen))
        return FALSE;

    return TRUE;
}

static void *
TegraShadowWindow(ScreenPtr screen, CARD32 row, CARD32 offset, int mode,
                  CARD32 *size, void *closure)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(screen);
    TegraPtr tegra = TegraPTR(pScrn);
    int stride;

    stride = (pScrn->displayWidth * pScrn->bitsPerPixel) / 8;
    *size = stride;

    return ((uint8_t *)tegra->drmmode.front_bo->ptr + row * stride + offset);
}

static Bool
TegraCreateScreenResources(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    TegraPtr tegra = TegraPTR(pScrn);
    PixmapPtr rootPixmap;
    Bool ret;
    void *pixels;

    pScreen->CreateScreenResources = tegra->createScreenResources;
    ret = pScreen->CreateScreenResources(pScreen);
    pScreen->CreateScreenResources = TegraCreateScreenResources;

    if (!drmmode_set_desired_modes(pScrn, &tegra->drmmode))
      return FALSE;

    drmmode_uevent_init(pScrn, &tegra->drmmode);

    if (!tegra->drmmode.want_sw_cursor)
        drmmode_map_cursor_bos(pScrn, &tegra->drmmode);

    pixels = drmmode_map_front_bo(&tegra->drmmode);
    if (!pixels)
        return FALSE;

    rootPixmap = pScreen->GetScreenPixmap(pScreen);

    if (tegra->drmmode.shadow_enable)
        pixels = tegra->drmmode.shadow_fb;

    if (!pScreen->ModifyPixmapHeader(rootPixmap, -1, -1, -1, -1, -1, pixels))
        FatalError("Couldn't adjust screen pixmap\n");

    if (tegra->drmmode.shadow_enable) {
        if (!shadowAdd(pScreen, rootPixmap, shadowUpdatePackedWeak(),
                       TegraShadowWindow, 0, 0))
            return FALSE;
    }

    tegra->damage = DamageCreate(NULL, NULL, DamageReportNone, TRUE, pScreen,
                                 rootPixmap);
    if (tegra->damage) {
        DamageRegister(&rootPixmap->drawable, tegra->damage);
        tegra->dirty_enabled = TRUE;
        xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Damage tracking initialized\n");
    } else {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "Failed to create screen damage record\n");
        return FALSE;
    }

    return ret;
}

static Bool
TegraCloseScreen(CLOSE_SCREEN_ARGS_DECL)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    TegraPtr tegra = TegraPTR(pScrn);

    if (tegra->damage) {
        DamageUnregister(&pScreen->GetScreenPixmap(pScreen)->drawable,
                         tegra->damage);
        DamageDestroy(tegra->damage);
        tegra->damage = NULL;
    }

    if (tegra->drmmode.shadow_enable) {
        shadowRemove(pScreen, pScreen->GetScreenPixmap(pScreen));
        free(tegra->drmmode.shadow_fb);
        tegra->drmmode.shadow_fb = NULL;
    }

    drmmode_uevent_fini(pScrn, &tegra->drmmode);
    drmmode_free_bos(pScrn, &tegra->drmmode);

    if (pScrn->vtSema)
        TegraLeaveVT(VT_FUNC_ARGS);

    pScreen->CreateScreenResources = tegra->createScreenResources;
    pScreen->BlockHandler = tegra->BlockHandler;

    pScrn->vtSema = FALSE;
    pScreen->CloseScreen = tegra->CloseScreen;
    return (*pScreen->CloseScreen)(CLOSE_SCREEN_ARGS);
}

static void TegraBlockHandler(BLOCKHANDLER_ARGS_DECL)
{
    SCREEN_PTR(arg);
    TegraPtr tegra = TegraPTR(xf86ScreenToScrn(pScreen));

    pScreen->BlockHandler = tegra->BlockHandler;
    pScreen->BlockHandler(BLOCKHANDLER_ARGS);
    pScreen->BlockHandler = TegraBlockHandler;
#ifdef TEGRA_OUTPUT_SLAVE_SUPPORT
    if (pScreen->isGPU)
        dispatch_slave_dirty(pScreen);
    else
#endif
    if (tegra->dirty_enabled)
        dispatch_dirty(pScreen);
}

static Bool
TegraScreenInit(SCREEN_INIT_ARGS_DECL)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    TegraPtr tegra = TegraPTR(pScrn);
    VisualPtr visual;
    int ret;

    pScrn->pScreen = pScreen;

    ret = drmSetMaster(tegra->fd);
    if (ret) {
        ErrorF("Unable to set master\n");
        return FALSE;
    }

    /* HW dependent - FIXME */
    pScrn->displayWidth = pScrn->virtualX;

    if (!drmmode_create_initial_bos(pScrn, &tegra->drmmode))
        return FALSE;

    if (tegra->drmmode.shadow_enable) {
        tegra->drmmode.shadow_fb = calloc(1, pScrn->displayWidth * pScrn->virtualY *
                                             ((pScrn->bitsPerPixel + 7) >> 3));
        if (!tegra->drmmode.shadow_fb)
            tegra->drmmode.shadow_enable = FALSE;
    }

    miClearVisualTypes();

    if (!miSetVisualTypes(pScrn->depth, miGetDefaultVisualMask(pScrn->depth),
                          pScrn->rgbBits, pScrn->defaultVisual))
        return FALSE;

    if (!miSetPixmapDepths())
        return FALSE;

#ifdef TEGRA_OUTPUT_SLAVE_SUPPORT
    if (!dixRegisterScreenSpecificPrivateKey(pScreen,
                                             &tegra->drmmode.pixmapPrivateKeyRec,
                                             PRIVATE_PIXMAP,
                                             sizeof(TegraPixmapPrivRec)))
        return FALSE;
#endif

    pScrn->memPhysBase = 0;
    pScrn->fbOffset = 0;

    if (!fbScreenInit(pScreen, NULL, pScrn->virtualX, pScrn->virtualY,
                      pScrn->xDpi, pScrn->yDpi, pScrn->displayWidth,
                      pScrn->bitsPerPixel))
        return FALSE;

    if (pScrn->bitsPerPixel > 8) {
        /* Fixup RGB ordering */
        visual = pScreen->visuals + pScreen->numVisuals;

        while (--visual >= pScreen->visuals) {
            if ((visual->class | DynamicClass) == DirectColor) {
                visual->offsetRed = pScrn->offset.red;
                visual->offsetGreen = pScrn->offset.green;
                visual->offsetBlue = pScrn->offset.blue;
                visual->redMask = pScrn->mask.red;
                visual->greenMask = pScrn->mask.green;
                visual->blueMask = pScrn->mask.blue;
            }
        }
    }

    fbPictureInit(pScreen, NULL, 0);

    if (tegra->drmmode.shadow_enable && !TegraShadowInit(pScreen)) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "shadow fb init failed\n");
        return FALSE;
    }

    tegra->createScreenResources = pScreen->CreateScreenResources;
    pScreen->CreateScreenResources = TegraCreateScreenResources;

    xf86SetBlackWhitePixels(pScreen);

    xf86SetBackingStore(pScreen);
    xf86SetSilkenMouse(pScreen);
    miDCInitialize(pScreen, xf86GetPointerScreenFuncs());

    /* Need to extend HWcursor support to handle mask interleave */
    if (!tegra->drmmode.want_sw_cursor)
        xf86_cursors_init(pScreen, 64, 64,
                          HARDWARE_CURSOR_SOURCE_MASK_INTERLEAVE_64 |
                          HARDWARE_CURSOR_ARGB);

    /* Must force it before EnterVT, so we are in control of VT and
     * later memory should be bound when allocating, e.g rotate_mem */
    pScrn->vtSema = TRUE;

    pScreen->SaveScreen = xf86SaveScreen;
    tegra->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = TegraCloseScreen;

    tegra->BlockHandler = pScreen->BlockHandler;
    pScreen->BlockHandler = TegraBlockHandler;

#ifdef TEGRA_OUTPUT_SLAVE_SUPPORT
    pScreen->SetSharedPixmapBacking = TegraSetSharedPixmapBacking;
#endif

    if (!xf86CrtcScreenInit(pScreen))
        return FALSE;

    if (!miCreateDefColormap(pScreen))
        return FALSE;

    xf86DPMSInit(pScreen, xf86DPMSSet, 0);

    if (serverGeneration == 1)
        xf86ShowUnusedOptions(pScrn->scrnIndex, pScrn->options);

    return TegraEnterVT(VT_FUNC_ARGS);
}

static Bool
TegraSwitchMode(SWITCH_MODE_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);

    return xf86SetSingleMode(pScrn, mode, RR_Rotate_0);
}

static void
TegraAdjustFrame(ADJUST_FRAME_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);
    xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR(pScrn);
    xf86OutputPtr output = config->output[config->compat_output];
    xf86CrtcPtr crtc = output->crtc;

    if (crtc && crtc->enabled) {
        crtc->funcs->mode_set(crtc, pScrn->currentMode, pScrn->currentMode, x,
                              y);
        crtc->x = output->initial_x + x;
        crtc->y = output->initial_y + y;
    }
}

static void
TegraFreeScreen(FREE_SCREEN_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);
    FreeRec(pScrn);
}

static ModeStatus
TegraValidMode(SCRN_ARG_TYPE arg, DisplayModePtr mode, Bool verbose, int flags)
{
    return MODE_OK;
}

#ifdef XSERVER_PLATFORM_BUS
static Bool
TegraPlatformProbe(DriverPtr driver, int entity_num, int flags,
                   struct xf86_platform_device *dev, intptr_t match_data)
{
    char *path = xf86_get_platform_device_attrib(dev, ODEV_ATTRIB_PATH);
    ScrnInfoPtr scrn = NULL;
    int fd;

    fd = open(path, O_RDWR);
    if (fd >= 0) {
        scrn = xf86AllocateScreen(driver, 0);

        xf86AddEntityToScreen(scrn, entity_num);

        scrn->driverName = "opentegra";
        scrn->name = "opentegra";
        scrn->PreInit = TegraPreInit;
        scrn->ScreenInit = TegraScreenInit;
        scrn->SwitchMode = TegraSwitchMode;
        scrn->AdjustFrame = TegraAdjustFrame;
        scrn->EnterVT = TegraEnterVT;
        scrn->LeaveVT = TegraLeaveVT;
        scrn->FreeScreen = TegraFreeScreen;
        scrn->ValidMode = TegraValidMode;

        close(fd);
    }

    return scrn != NULL;
}
#endif

static Bool
TegraProbe(DriverPtr drv, int flags)
{
    int i, numDevSections;
    GDevPtr *devSections;
    Bool foundScreen = FALSE;
    const char *dev;
    ScrnInfoPtr scrn = NULL;

    /* For now, just bail out for PROBE_DETECT. */
    if (flags & PROBE_DETECT)
        return FALSE;

    /*
     * Find the config file Device sections that match this
     * driver, and return if there are none.
     */
    if ((numDevSections = xf86MatchDevice("opentegra", &devSections)) <= 0)
        return FALSE;

    for (i = 0; i < numDevSections; i++) {
        dev = xf86FindOptionValue(devSections[i]->options, "device");
        if (TegraProbeHardware(dev)) {
            int entity = xf86ClaimFbSlot(drv, 0, devSections[i], TRUE);
            scrn = xf86ConfigFbEntity(scrn, 0, entity, NULL, NULL, NULL,
                                      NULL);
        }

        if (scrn) {
            foundScreen = TRUE;
            scrn->driverVersion = 1;
            scrn->driverName = "opentegra";
            scrn->name = "opentegra";
            scrn->Probe = TegraProbe;
            scrn->PreInit = TegraPreInit;
            scrn->ScreenInit = TegraScreenInit;
            scrn->SwitchMode = TegraSwitchMode;
            scrn->AdjustFrame = TegraAdjustFrame;
            scrn->EnterVT = TegraEnterVT;
            scrn->LeaveVT = TegraLeaveVT;
            scrn->FreeScreen = TegraFreeScreen;
            scrn->ValidMode = TegraValidMode;

            xf86DrvMsg(scrn->scrnIndex, X_INFO, "using %s\n",
                       dev ? dev : "default device");
        }
    }

    free(devSections);

    return foundScreen;
}

_X_EXPORT DriverRec tegra = {
    1,
    "opentegra",
    TegraIdentify,
    TegraProbe,
    TegraAvailableOptions,
    NULL,
    0,
    TegraDriverFunc,
    NULL,
    NULL,
#ifdef XSERVER_PLATFORM_BUS
    TegraPlatformProbe,
#endif
};

static MODULESETUPPROTO(Setup);

static pointer
Setup(pointer module, pointer opts, int *errmaj, int *errmin)
{
    static Bool setupDone = 0;

    /*
     * This module should be loaded only once, but check to be sure.
     */
    if (!setupDone) {
        setupDone = 1;
        xf86AddDriver(&tegra, module, HaveDriverFuncs);

        /*
         * The return value must be non-NULL on success even though there
         * is no TearDownProc.
         */
        return (pointer)1;
    } else {
        if (errmaj)
            *errmaj = LDR_ONCEONLY;

        return NULL;
    }
}

static XF86ModuleVersionInfo VersRec = {
    "opentegra",
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XORG_VERSION_CURRENT,
    PACKAGE_VERSION_MAJOR, PACKAGE_VERSION_MINOR, PACKAGE_VERSION_PATCHLEVEL,
    ABI_CLASS_VIDEODRV,
    ABI_VIDEODRV_VERSION,
    MOD_CLASS_VIDEODRV,
    { 0, 0, 0, 0 }
};

_X_EXPORT XF86ModuleData opentegraModuleData = { &VersRec, Setup, NULL };

/* vim: set et sts=4 sw=4 ts=4: */
