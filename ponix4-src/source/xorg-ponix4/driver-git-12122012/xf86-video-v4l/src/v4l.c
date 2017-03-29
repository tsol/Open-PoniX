/*
 *  video4linux Xv Driver
 *  based on Michael Schimek's permedia 2 driver.
 *
 * Copyright (c) 2011 Mauro Carvalho Chehab <mchehab@redhat.com> for:
 *	- Major rewrite, as driver got ported to V4L2 API
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "videodev2.h"
#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86Pci.h"
#include "xf86PciInfo.h"
#include "xf86fbman.h"
#include "xf86xv.h"
#include <X11/extensions/Xv.h>
#include "regionstr.h"
#include "dgaproc.h"
#include "xf86str.h"
#include "fourcc.h"

#include <asm/ioctl.h>          /* _IORW(xxx) #defines are here */

#if 0
# define DEBUG(x) (x)
#else
# define DEBUG(x)
#endif

/***************************************************************************/

static void     V4LIdentify(int flags);
static Bool     V4LProbe(DriverPtr drv, int flags);
static const OptionInfoRec * V4LAvailableOptions(int chipid, int busid);

_X_EXPORT DriverRec V4L = {
    .driverVersion = 50000,
    .driverName = "v4l",
    .Identify = V4LIdentify,
    .Probe = V4LProbe,
    .AvailableOptions = V4LAvailableOptions,
    .module = NULL,
    .refCount = 0,
    .driverFunc = NULL,   /* FIXME: Need to implement for new probing mode */
};

#ifdef XFree86LOADER

static MODULESETUPPROTO(v4lSetup);

static XF86ModuleVersionInfo v4lVersRec =
{
    "v4l",
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XORG_VERSION_CURRENT,
    0, 1, 1,
    ABI_CLASS_VIDEODRV,
    ABI_VIDEODRV_VERSION,
    MOD_CLASS_NONE,
    {0,0,0,0}
};

_X_EXPORT XF86ModuleData v4lModuleData = { &v4lVersRec, v4lSetup, NULL };

static pointer
v4lSetup(pointer module, pointer opts, int *errmaj, int *errmin)
{
    const char *osname;
    static Bool setupDone = FALSE;

    if (setupDone) {
        if (errmaj)
            *errmaj = LDR_ONCEONLY;
        return NULL;
    }

    setupDone = TRUE;

    /* Check that we're being loaded on a Linux system */
    LoaderGetOS(&osname, NULL, NULL, NULL);
    if (!osname || strcmp(osname, "linux") != 0) {
        if (errmaj)
            *errmaj = LDR_BADOS;
        if (errmin)
            *errmin = 0;
        return NULL;
    } else {
        /* OK */

        xf86AddDriver (&V4L, module, 0);

        return (pointer)1;
    }
}

#else

#include <fcntl.h>
#include <sys/ioctl.h>

#endif

#define VIDEO_OFF     0  /* really off */
#define VIDEO_RGB     1  /* rgb overlay (directly to fb) */
#define VIDEO_YUV     2  /* yuv overlay (to offscreen memory + hw scaling) */
#define VIDEO_RECLIP  3  /* temporarly off, window clipping changes */

typedef struct _XvV4LCtrlRec {
    struct v4l2_queryctrl       qctrl;
    Atom                        xv;
} XvV4LCtrlRec, *XvV4LCtrlPtr;

typedef struct _PortPrivRec {
    ScrnInfoPtr                 pScrn;
    FBAreaPtr                   pFBArea[2];
    int                         VideoOn;
    Bool                        StreamOn;

    /* file handle */
    int                         nr;
    struct v4l2_capability      cap;

    /* RGB overlay */
    struct v4l2_framebuffer     rgb_fbuf;
    struct v4l2_window          rgb_win;
    int                         rgbdepth;

    /* attributes */
    CARD32                      pixelformat;

    XF86VideoEncodingPtr        enc;
    int                         *input;
    int                         *norm;
    int                         nenc,cenc;

    /* yuv to offscreen */
    XF86OffscreenImagePtr       format;   /* list */
    int                         nformat;  /* # if list entries */
    XF86OffscreenImagePtr       myfmt;    /* which one is YUY2 (packed) */
    int                         yuv_format;

    int                         yuv_width,yuv_height;
    XF86SurfacePtr              surface;
    struct v4l2_framebuffer     yuv_fbuf;
    struct v4l2_window          yuv_win;

    struct v4l2_standard        *standard; /* FIXME: can be removed */

    XvV4LCtrlPtr                XvV4LCtrl;
    int                         n_qctrl;
} PortPrivRec, *PortPrivPtr;

#define XV_ENCODING "XV_ENCODING"
#define XV_FREQ     "XV_FREQ"

#define MAKE_ATOM(a) MakeAtom(a, strlen(a), TRUE)

static Atom xvEncoding, xvFreq;

static XF86VideoFormatRec
InputVideoFormats[] = {
    { 15, TrueColor },
    { 16, TrueColor },
    { 24, TrueColor },
    { 32, TrueColor },
};

#define V4L_ATTR (sizeof(Attributes) / sizeof(XF86AttributeRec))

static const XF86AttributeRec Attributes[] = {
    {XvSettable | XvGettable, -1000,    1000, XV_ENCODING},
};
static const XF86AttributeRec FreqAttr =
    {XvSettable | XvGettable,     0, 16*1000, XV_FREQ};


#define MAX_V4L_DEVICES 4
#define V4L_FD   (v4l_devices[pPPriv->nr].fd)
#define V4L_REF  (v4l_devices[pPPriv->nr].useCount)
#define V4L_NAME (v4l_devices[pPPriv->nr].devName)

static struct V4L_DEVICE {
    int  fd;
    int  useCount;
    char devName[16];
} v4l_devices[MAX_V4L_DEVICES] = {
    { -1 },
    { -1 },
    { -1 },
    { -1 },
};

/* ---------------------------------------------------------------------- */

static int SetV4LFmt(int fd, CARD32 pixelformat)
{
    struct v4l2_framebuffer fbuf;
    char *p = (char *)&pixelformat;

    memset(&fbuf, 0, sizeof(fbuf));
    if (ioctl(fd, VIDIOC_G_FBUF, &fbuf) == -1) {
        xf86Msg(X_ERROR, "v4l: Error %d: Can't get FBUF\n", errno);
        return errno;
    }
    if (fbuf.fmt.pixelformat != pixelformat) {
        fbuf.fmt.pixelformat = pixelformat;
        if (ioctl(fd, VIDIOC_S_FBUF, &fbuf) == -1) {
            xf86Msg(X_ERROR, "v4l: Error %d: Can't set FBUF to %c%c%c%c\n",
                    errno, p[0], p[1], p[2], p[3]);
            return errno;
        }
    }
    DEBUG(xf86Msg(X_INFO, "v4l: Set overlay format to %c%c%c%c\n",
                  p[0], p[1], p[2], p[3]));
    return 0;
}
static int GetV4LFmt(int fd, CARD32 *pixelformat)
{
    struct v4l2_framebuffer fbuf;

    memset(&fbuf, 0, sizeof(fbuf));
    if (ioctl(fd, VIDIOC_G_FBUF, &fbuf) == -1) {
        xf86Msg(X_ERROR, "v4l: Error %d: Can't get FBUF\n", errno);
        return errno;
    }

    *pixelformat = fbuf.fmt.pixelformat;

    return 0;
}

#define ATTR_NAME_PREFIX "XV_"
static int AddControl(PortPrivPtr p, XF86AttributeRec **list, int *count,
                      struct v4l2_queryctrl *qctrl, int *n)
{
    char *ptr;

/* v4l_add_attr */
    if (qctrl->flags & V4L2_CTRL_FLAG_DISABLED)
        return 0;
    switch (qctrl->type) {
    case V4L2_CTRL_TYPE_INTEGER:
    case V4L2_CTRL_TYPE_BOOLEAN:
    case V4L2_CTRL_TYPE_MENU:
    case V4L2_CTRL_TYPE_BUTTON:
        break;
    default:
        return 0;
    }

    p->XvV4LCtrl = realloc(p->XvV4LCtrl, sizeof(XvV4LCtrlRec) * (*n + 1));
    if (!p->XvV4LCtrl) {
        if (*list) {
            free (*list);
            *count = 0;
            *n = 0;
        }
        return -1;
    }

    *list = realloc(*list, sizeof(XF86AttributeRec) * (*count + 1));
    if (NULL == *list) {
        if (p->XvV4LCtrl)
            free (p->XvV4LCtrl);
        *count = 0;
        return -1;
    }

    memset(*list + *count, 0, sizeof(XF86AttributeRec));
    (*list)[*count].flags = XvSettable | XvGettable;

    if (qctrl->flags & V4L2_CTRL_FLAG_READ_ONLY)
        (*list)[*count].flags &= ~XvSettable;
    if (qctrl->flags & V4L2_CTRL_FLAG_WRITE_ONLY)
        (*list)[*count].flags &= ~XvGettable;
    (*list)[*count].min_value = qctrl->minimum;
    (*list)[*count].max_value = qctrl->maximum;
    (*list)[*count].name = malloc(strlen((char *)qctrl->name) + 1 + sizeof (ATTR_NAME_PREFIX));
    strcpy((*list)[*count].name, ATTR_NAME_PREFIX);
    strcat((*list)[*count].name, (char *)qctrl->name);
    for (ptr = (*list)[*count].name; *ptr; ptr++) {
        *ptr = toupper(*ptr);
        if (*ptr == ' ')
            *ptr = '_';
    }

    p->XvV4LCtrl[*n].xv = MAKE_ATOM((*list)[*count].name);
    memcpy(&p->XvV4LCtrl[*n].qctrl, qctrl, sizeof(*qctrl));

    xf86Msg(X_INFO, "v4l: add attr %s (Xv/GPA %d) (%d to %d)\n",
            (*list)[*count].name, (int)p->XvV4LCtrl[*n].xv,
            p->XvV4LCtrl[*n].qctrl.minimum,
            p->XvV4LCtrl[*n].qctrl.maximum);

    (*count)++;
    (*n)++;

    return 0;
}

static void AddAllV4LControls(PortPrivPtr p, XF86AttributeRec **list,
                              int *count, int fd)
{
    int entries = 0;
    CARD32 id;
    struct v4l2_queryctrl qctrl;

    memset(&qctrl, 0, sizeof(qctrl));
    qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    while (!ioctl(fd, VIDIOC_QUERYCTRL, &qctrl)) {
        if (AddControl(p, list, count, &qctrl, &entries))
            return;
        qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
    if (qctrl.id != V4L2_CTRL_FLAG_NEXT_CTRL)
        return;
    for (id = V4L2_CID_USER_BASE; id < V4L2_CID_LASTP1; id++) {
        qctrl.id = id;
        if (!ioctl(fd, VIDIOC_QUERYCTRL, &qctrl)) {
            if (AddControl(p, list, count, &qctrl, &entries))
                return;
        }
    }
    qctrl.id = V4L2_CID_PRIVATE_BASE;
    while (!ioctl(fd, VIDIOC_QUERYCTRL, &qctrl)) {
        if (AddControl(p, list, count, &qctrl, &entries))
            return;
        qctrl.id++;
    }
    p->n_qctrl = entries;
}

/* ---------------------------------------------------------------------- */

/* setup yuv overlay + hw scaling: look if we find some common video
   format which both v4l driver and the X-Server can handle */
static int v4l_check_yuv(ScrnInfoPtr pScrn, PortPrivPtr pPPriv,
                          char *dev, int fd)
{
    static const struct {
        CARD32        v4l_palette;
        unsigned int  xv_id;
        unsigned int  xv_format;
    } yuvlist[] = {
        { V4L2_PIX_FMT_YUYV, FOURCC_YUY2, XvPacked },
        { V4L2_PIX_FMT_UYVY, FOURCC_UYVY, XvPacked },
        { 0 /* end of list */ },
    };
    /* FIXME: Why pScrn->scrnIndex? */
    ScreenPtr pScreen = screenInfo.screens[pScrn->scrnIndex];
    int fmt,i;

    pPPriv->format = xf86XVQueryOffscreenImages(pScreen, &pPPriv->nformat);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,
                   "v4l: Number of Xv formats: %d\n", pPPriv->nformat);
    if (!pPPriv->nformat)
        return FALSE;

    for (fmt = 0; yuvlist[fmt].v4l_palette != 0; fmt++) {
        pPPriv->pixelformat = yuvlist[fmt].v4l_palette;
        if (-1 == SetV4LFmt(fd, pPPriv->pixelformat))
            continue;
        GetV4LFmt(fd, &pPPriv->pixelformat);
        if (pPPriv->pixelformat != yuvlist[fmt].v4l_palette)
            continue;
        /* ... works, check available offscreen image formats now ... */
        for (i = 0; i < pPPriv->nformat; i++) {
            if (pPPriv->format[i].image->id == yuvlist[fmt].xv_id &&
                pPPriv->format[i].image->format == yuvlist[fmt].xv_format) {
                /* ... match found, good. */
                pPPriv->yuv_format = yuvlist[fmt].v4l_palette;
                pPPriv->myfmt = pPPriv->format+i;
                xf86DrvMsg(pScrn->scrnIndex, X_INFO,
                           "v4l[%s]: using hw video scaling [%4.4s].\n",
                           dev,(char*)&(pPPriv->format[i].image->id));
                return TRUE;
            }
        }
    }
    return TRUE;
}

static int V4lOpenDevice(PortPrivPtr pPPriv, ScrnInfoPtr pScrn)
{
    static int first = 1;

#if 0
    /*
     * It may be a good idea to probe here, but this would break
     * some things, as initialization uses yuv_format
     */
    if (!pPPriv->yuv_format && first)
        v4l_check_yuv(pScrn, pPPriv, V4L_NAME, V4L_FD);
#endif
    if (-1 == V4L_FD) {
        V4L_FD = open(V4L_NAME, O_RDWR, 0);

        if (-1 == V4L_FD)
            return errno;

        if (-1 == ioctl(V4L_FD, VIDIOC_G_FBUF, &pPPriv->rgb_fbuf)) {
            xf86Msg(X_ERROR, "v4l: Error %d: Can't get FBUF\n", errno);
            return errno;
        }
        pPPriv->rgb_fbuf.fmt.width        = pScrn->virtualX;
        pPPriv->rgb_fbuf.fmt.height       = pScrn->virtualY;
        pPPriv->rgb_fbuf.fmt.bytesperline = pScrn->displayWidth * ((pScrn->bitsPerPixel + 7)/8);
        pPPriv->rgb_fbuf.base             = (pointer)(pScrn->memPhysBase + pScrn->fbOffset);
        if (first) {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,
                           "v4l: memPhysBase=0x%lx\n", pScrn->memPhysBase);
            first = 0;
        }

        switch (pScrn->bitsPerPixel) {
        case 16:
            if (pScrn->weight.green == 5) {
                pPPriv->rgb_fbuf.fmt.pixelformat = V4L2_PIX_FMT_RGB555;
            } else {
                pPPriv->rgb_fbuf.fmt.pixelformat = V4L2_PIX_FMT_RGB565;
            }
            break;
        case 24:
            pPPriv->rgb_fbuf.fmt.pixelformat = V4L2_PIX_FMT_BGR24;
            break;
        case 32:
            pPPriv->rgb_fbuf.fmt.pixelformat = V4L2_PIX_FMT_BGR32;
            break;
        }
    }

    V4L_REF++;
    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,
                         "Xv/open: refcount=%d\n",V4L_REF));

    return 0;
}

static void V4lCloseDevice(PortPrivPtr pPPriv, ScrnInfoPtr pScrn)
{
    V4L_REF--;
    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,
                         "Xv/close: refcount=%d\n",V4L_REF));
    if (0 == V4L_REF && -1 != V4L_FD) {
        close(V4L_FD);
        V4L_FD = -1;
    }
}

static int
V4lPutVideo(ScrnInfoPtr pScrn,
    short vid_x, short vid_y, short drw_x, short drw_y,
    short vid_w, short vid_h, short drw_w, short drw_h,
    RegionPtr clipBoxes, pointer data, DrawablePtr pDraw)
{
    struct v4l2_format fmt;

    PortPrivPtr pPPriv = (PortPrivPtr) data;
    struct v4l2_clip *clip;
    BoxPtr pBox;
    RegionRec newReg;
    BoxRec    newBox;
    unsigned int i,dx,dy,dw,dh;
    int width,height;
    int one=1;

    /* Open a file handle to the device */
    if (VIDEO_OFF == pPPriv->VideoOn) {
        if (V4lOpenDevice(pPPriv, pScrn))
            return Success;
    }

    if (0 != pPPriv->yuv_format) {
        DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "Xv/PV yuv\n"));
        width  = pPPriv->enc[pPPriv->cenc].width;
        height = pPPriv->enc[pPPriv->cenc].height/2; /* no interlace */
        if (drw_w < width)
            width = drw_w;
        if (drw_h < height)
            height = drw_h;
        if ((height != pPPriv->yuv_height) || (width != pPPriv->yuv_width)) {
            /* new size -- free old surface */
            DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "  surface resize\n"));
            if (pPPriv->surface) {
                pPPriv->VideoOn = VIDEO_OFF;
                pPPriv->myfmt->stop(pPPriv->surface);
                pPPriv->myfmt->free_surface(pPPriv->surface);
                free(pPPriv->surface);
                pPPriv->surface    = NULL;
            }
            pPPriv->yuv_width  = width;
            pPPriv->yuv_height = height;
        }
        if (!pPPriv->surface) {
            /* allocate + setup offscreen surface */
            if (NULL == (pPPriv->surface = malloc(sizeof(XF86SurfaceRec))))
                return FALSE;
            if (Success != pPPriv->myfmt->alloc_surface
                (pScrn,pPPriv->myfmt->image->id,
                pPPriv->yuv_width,pPPriv->yuv_height,pPPriv->surface)) {
                free(pPPriv->surface);
                pPPriv->surface = NULL;
                goto fallback_to_rgb;
            }

            if (-1 == ioctl(V4L_FD, VIDIOC_G_FBUF, &pPPriv->yuv_fbuf)) {
                xf86Msg(X_ERROR, "v4l: Error %d: Can't get FBUF\n", errno);
                return errno;
            }
            pPPriv->yuv_fbuf.fmt.width        = pPPriv->surface->width;
            pPPriv->yuv_fbuf.fmt.height       = pPPriv->surface->height;
            pPPriv->yuv_fbuf.fmt.bytesperline = pPPriv->surface->pitches[0];
            pPPriv->yuv_fbuf.fmt.pixelformat = pPPriv->yuv_format;
            pPPriv->yuv_fbuf.base         =
                (pointer)(pScrn->memPhysBase + pPPriv->surface->offsets[0]);
            DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,
                                 "  surface: %p+%d = %p, %dx%d, pitch %d\n",
                                (void *)pScrn->memPhysBase, pPPriv->surface->offsets[0],
                                (void *)pScrn->memPhysBase+pPPriv->surface->offsets[0],
                                pPPriv->surface->width, pPPriv->surface->height,
                                pPPriv->surface->pitches[0]));
            memset(&pPPriv->yuv_win, 0, sizeof(pPPriv->yuv_win));
            pPPriv->yuv_win.w.left    = 0;
            pPPriv->yuv_win.w.top     = 0;
            pPPriv->yuv_win.w.width  = pPPriv->surface->width;
            pPPriv->yuv_win.w.height = pPPriv->surface->height;
        }

        /* program driver */
        if (-1 == ioctl(V4L_FD, VIDIOC_S_FBUF, &pPPriv->yuv_fbuf)) {
            xf86Msg(X_ERROR, "Error %d at VIDIOC_S_FBUF\n", errno);
            return BadValue;
        }
        pPPriv->pixelformat = pPPriv->yuv_format;
        if (-1 == SetV4LFmt(V4L_FD, pPPriv->pixelformat))
            return BadValue;

        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
        memcpy(&fmt.fmt.win, &pPPriv->yuv_win, sizeof(pPPriv->yuv_win));
        if (-1 == ioctl(V4L_FD, VIDIOC_S_FMT, &fmt)) {
            xf86Msg(X_ERROR, "Error %d at VIDIOC_S_FMT\n", errno);
            return BadValue;
        }
        if (-1 == ioctl(V4L_FD, VIDIOC_OVERLAY, &one)) {
            xf86Msg(X_ERROR, "v4l: Error %d while enabling Overlay\n", errno);
            return BadValue;
        }

        if (0 == (pPPriv->myfmt->flags & VIDEO_INVERT_CLIPLIST)) {
            /* invert cliplist */
            newBox.x1 = drw_x;
            newBox.y1 = drw_y;
            newBox.x2 = drw_x + drw_w;
            newBox.y2 = drw_y + drw_h;

            if (pPPriv->myfmt->flags & VIDEO_CLIP_TO_VIEWPORT) {
                /* trim to the viewport */
                if(newBox.x1 < pScrn->frameX0)
                    newBox.x1 = pScrn->frameX0;
                if(newBox.x2 > pScrn->frameX1)
                    newBox.x2 = pScrn->frameX1;

                if(newBox.y1 < pScrn->frameY0)
                    newBox.y1 = pScrn->frameY0;
                if(newBox.y2 > pScrn->frameY1)
                    newBox.y2 = pScrn->frameY1;
            }

            REGION_INIT(pScrn->pScreen, &newReg, &newBox, 1);
            REGION_SUBTRACT(pScrn->pScreen, &newReg, &newReg, clipBoxes);
            clipBoxes = &newReg;
        }

        /* start overlay */
        DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,
                             "over: - %d,%d -> %d,%d  (%dx%d) (yuv=%dx%d)\n",
                             drw_x, drw_y,
                             drw_x+drw_w, drw_y+drw_h,
                             drw_w, drw_h,
                             pPPriv->surface->width,pPPriv->surface->height));
        pPPriv->myfmt->display(pPPriv->surface,
                               0, 0, drw_x, drw_y,
                               pPPriv->surface->width,
                               pPPriv->surface->height,
                               drw_w, drw_h,
                               clipBoxes);
        if (0 == (pPPriv->myfmt->flags & VIDEO_INVERT_CLIPLIST)) {
            REGION_UNINIT(pScrn->pScreen, &newReg);
        }
        pPPriv->VideoOn = VIDEO_YUV;
        return Success;
    }

fallback_to_rgb:
    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "Xv/PV rgb\n"));
    /* FIXME: vid-* is ignored for now, not supported by v4l */

    dw = (drw_w < pPPriv->enc[pPPriv->cenc].width) ?
        drw_w : pPPriv->enc[pPPriv->cenc].width;
    dh = (drw_h < pPPriv->enc[pPPriv->cenc].height) ?
        drw_h : pPPriv->enc[pPPriv->cenc].height;
    /* if the window is too big, center the video */
    dx = drw_x + (drw_w - dw)/2;
    dy = drw_y + (drw_h - dh)/2;
    /* bttv prefeares aligned addresses */
    dx &= ~3;
    if (dx < drw_x)
        dx += 4;
    if (dx+dw > drw_x+drw_w)
        dw -= 4;

    /* window */
    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "  win: %dx%d+%d+%d\n",
                         drw_w,drw_h,drw_x,drw_y));
    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "  use: %dx%d+%d+%d\n",
                         dw,dh,dx,dy));
    memset(&pPPriv->rgb_win, 0, sizeof(pPPriv->rgb_win));
    pPPriv->rgb_win.w.left   = dx;
    pPPriv->rgb_win.w.top    = dy;
    pPPriv->rgb_win.w.width  = dw;
    pPPriv->rgb_win.w.height = dh;

    /* clipping */
    if (pPPriv->rgb_win.clips) {
        free(pPPriv->rgb_win.clips);
        pPPriv->rgb_win.clips = NULL;
    }
    pPPriv->rgb_win.clipcount = REGION_NUM_RECTS(clipBoxes);
    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,"  clip: have #%d\n",
                         pPPriv->rgb_win.clipcount));
    if (0 != pPPriv->rgb_win.clipcount) {
        pPPriv->rgb_win.clips = malloc(pPPriv->rgb_win.clipcount*sizeof(struct v4l2_clip));
        if (NULL != pPPriv->rgb_win.clips) {
            memset(pPPriv->rgb_win.clips,0,pPPriv->rgb_win.clipcount*sizeof(struct v4l2_clip));
            pBox = REGION_RECTS(clipBoxes);
            clip = pPPriv->rgb_win.clips;

            /*
             * FIXME: This code currently does nothing, as we don't emulate
             * The V4L1 clipping stuff
             */
            for (i = 0; i < REGION_NUM_RECTS(clipBoxes); i++, pBox++, clip++) {
                clip->c.left   = pBox->x1 - dx;
                clip->c.top    = pBox->y1 - dy;
                clip->c.width  = pBox->x2 - pBox->x1;
                clip->c.height = pBox->y2 - pBox->y1;
            }
        }
    }

    /* start */
    if (-1 == ioctl(V4L_FD, VIDIOC_S_FBUF, &pPPriv->rgb_fbuf)) {
        xf86Msg(X_ERROR, "Error %d at VIDIOC_S_FBUF\n", errno);
        return BadValue;
    }
    if (-1 == GetV4LFmt(V4L_FD, &pPPriv->pixelformat)) {
        xf86Msg(X_ERROR, "Error %d getting pixelformat\n", errno);
        return BadValue;
    }
    pPPriv->pixelformat = pPPriv->rgb_fbuf.fmt.pixelformat;
    if (-1 == SetV4LFmt(V4L_FD, pPPriv->pixelformat)) {
        xf86Msg(X_ERROR, "Error %d getting pixelformat\n", errno);
        return BadValue;
    }
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
    memcpy(&fmt.fmt.win, &pPPriv->rgb_win, sizeof(pPPriv->rgb_win));
    if (-1 == ioctl(V4L_FD, VIDIOC_S_FMT, &fmt)) {
        xf86Msg(X_ERROR, "Error %d at VIDIOC_S_FMT\n", errno);
        return BadValue;
    }
    if (-1 == ioctl(V4L_FD, VIDIOC_OVERLAY, &one)) {
        xf86Msg(X_ERROR, "v4l: Error %d while enabling Overlay\n", errno);
        return BadValue;
    }
    pPPriv->VideoOn = VIDEO_RGB;

    return Success;
}

static int
V4lPutStill(ScrnInfoPtr pScrn,
    short vid_x, short vid_y, short drw_x, short drw_y,
    short vid_w, short vid_h, short drw_w, short drw_h,
    RegionPtr clipBoxes, pointer data, DrawablePtr pDraw)
{
#if 0
    PortPrivPtr pPPriv = (PortPrivPtr) data;
#endif

    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "Xv/PS\n"));

    /* FIXME */
    return Success;
}

static void
V4lStopVideo(ScrnInfoPtr pScrn, pointer data, Bool shutdown)
{
    PortPrivPtr pPPriv = (PortPrivPtr) data;
    int zero=0;

    if (VIDEO_OFF == pPPriv->VideoOn) {
        DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,
              "Xv/StopVideo called with video already off\n"));
        return;
    }
    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "Xv/StopVideo shutdown=%d\n",shutdown));

    if (!shutdown) {
        /* just reclipping, we have to stop DMA transfers to the visible screen */
        if (VIDEO_RGB == pPPriv->VideoOn) {
            if (-1 == ioctl(V4L_FD, VIDIOC_OVERLAY, &zero))
                xf86Msg(X_ERROR, "v4l: Error %d while disabling Overlay\n", errno);
            pPPriv->VideoOn = VIDEO_RECLIP;
        }
    } else {
        /* video stop - turn off and free everything */
        if (VIDEO_YUV == pPPriv->VideoOn) {
            pPPriv->myfmt->stop(pPPriv->surface);
            pPPriv->myfmt->free_surface(pPPriv->surface);
            free(pPPriv->surface);
            pPPriv->surface = NULL;
        }
        if (-1 == ioctl(V4L_FD, VIDIOC_OVERLAY, &zero))
            xf86Msg(X_ERROR, "v4l: Error %d while disabling Overlay\n", errno);

        V4lCloseDevice(pPPriv,pScrn);
        pPPriv->VideoOn = VIDEO_OFF;
    }
}

static int
V4lSetPortAttribute(ScrnInfoPtr pScrn,
    Atom attribute, INT32 value, pointer data)
{
    struct v4l2_control ctrl;
    PortPrivPtr pPPriv = (PortPrivPtr) data;
    int i, ret = BadValue;

    if (V4lOpenDevice(pPPriv, pScrn))
        return Success;

    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "Xv/SPA %d, %d\n",
                        (int)attribute, (int)value));

    if (-1 == V4L_FD) {
        ret = Success;
    } else if (attribute == xvEncoding) {
        if (value < 0 || value >= pPPriv->nenc)
            goto err;
        if (ioctl(V4L_FD, VIDIOC_S_INPUT, &pPPriv->input[value]) == -1) {
            xf86Msg(X_ERROR, "v4l: Error %d while setting input\n", errno);
            goto err;
        }
        if (ioctl(V4L_FD, VIDIOC_S_STD, &pPPriv->norm[value]) == -1) {
            xf86Msg(X_ERROR, "v4l: Error %d while setting standard\n", errno);
            goto err;
        }
        pPPriv->cenc = value;
        ret = Success;
    } else if (attribute == xvFreq) {
        struct v4l2_frequency   freq;
        memset(&freq, 0, sizeof(freq));
        ioctl(V4L_FD, VIDIOC_G_FREQUENCY, &freq);
        freq.frequency = value;
        if (ioctl(V4L_FD, VIDIOC_S_FREQUENCY, &freq) == -1)
            xf86Msg(X_ERROR, "v4l: Error %d while setting frequency\n", errno);
        else
            ret = Success;
    } else {
        for (i = 0; i < pPPriv->n_qctrl; i++)
            if (pPPriv->XvV4LCtrl[i].xv == attribute)
                break;
        if (i == pPPriv->n_qctrl) {
            /* not mine -> pass to yuv scaler driver */
            if (0 != pPPriv->yuv_format && pPPriv->myfmt->setAttribute)
                ret = pPPriv->myfmt->setAttribute(pScrn, attribute, value);
            else
                ret = BadMatch;
            goto err;
        }
        if (pPPriv->XvV4LCtrl[i].qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
            goto err;
        ctrl.id = pPPriv->XvV4LCtrl[i].qctrl.id;
        ctrl.value = value;
        if (ioctl(V4L_FD, VIDIOC_S_CTRL, &ctrl) != 1)
            ret = Success;
    }

err:
    V4lCloseDevice(pPPriv,pScrn);
    return ret;
}

static int
V4lGetPortAttribute(ScrnInfoPtr pScrn,
    Atom attribute, INT32 *value, pointer data)
{
    struct v4l2_control ctrl;
    PortPrivPtr pPPriv = (PortPrivPtr) data;
    int i, ret = BadValue;

    if (V4lOpenDevice(pPPriv, pScrn))
        return Success;

    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "Xv/GPA %d\n",
                        (int)attribute));

    if (-1 == V4L_FD) {
        ret = Success;
    } else if (attribute == xvEncoding) {
        *value = pPPriv->cenc;
        ret = Success;
    } else if (attribute == xvFreq) {
        struct v4l2_frequency   freq;
        memset(&freq, 0, sizeof(freq));
        if (ioctl(V4L_FD, VIDIOC_G_FREQUENCY, &freq) != -1) {
            *value = freq.frequency;
            ret = Success;
        }
    } else {
        for (i = 0; i < pPPriv->n_qctrl; i++)
            if (pPPriv->XvV4LCtrl[i].xv == attribute)
                break;
        if (i == pPPriv->n_qctrl) {
            /* not mine -> pass to yuv scaler driver */
            if (0 != pPPriv->yuv_format &&  pPPriv->myfmt->getAttribute)
                ret = pPPriv->myfmt->getAttribute(pScrn, attribute, value);
            else
                ret = BadMatch;
            goto err;
        }
        if (pPPriv->XvV4LCtrl[i].qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
            goto err;
        ctrl.id = pPPriv->XvV4LCtrl[i].qctrl.id;
        if (ioctl(V4L_FD, VIDIOC_G_CTRL, &ctrl) != -1) {
            *value = ctrl.value;
            ret = Success;
        }
    }
    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "Xv/GPA %d, %d\n",
        (int)attribute, (int)*value));

err:
    V4lCloseDevice(pPPriv,pScrn);
    return ret;
}

static void
V4lQueryBestSize(ScrnInfoPtr pScrn, Bool motion,
    short vid_w, short vid_h, short drw_w, short drw_h,
    unsigned int *p_w, unsigned int *p_h, pointer data)
{
    PortPrivPtr pPPriv = (PortPrivPtr) data;
    int maxx = pPPriv->enc[pPPriv->cenc].width;
    int maxy = pPPriv->enc[pPPriv->cenc].height;

    if (0 != pPPriv->yuv_format) {
        *p_w = pPPriv->myfmt->max_width;
        *p_h = pPPriv->myfmt->max_height;
    } else {
        *p_w = (drw_w < maxx) ? drw_w : maxx;
        *p_h = (drw_h < maxy) ? drw_h : maxy;
    }

    DEBUG(xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2, "Xv/BS %d %dx%d %dx%d\n",
                         pPPriv->cenc,drw_w,drw_h,*p_w,*p_h));
}

static const OptionInfoRec *
V4LAvailableOptions(int chipid, int busid)
{
    return NULL;
}

static void
V4LIdentify(int flags)
{
    xf86Msg(X_INFO, "v4l driver for Video4Linux overlay mode (V4L2)\n");
}

static char*
fixname(char *str)
{
    int s,d;
    for (s=0, d=0;; s++) {
        if (str[s] == '-')
            continue;
        str[d++] = tolower(str[s]);
        if (0 == str[s])
            break;
    }
    return str;
}

static int
AddV4LEnc(XF86VideoEncodingPtr enc, int entry,
            char *norm, char *input, int width, int height, int n, int d)
{
    enc->id     = entry;
    enc->name   = malloc(strlen(norm) + strlen(input) + 2);
    if (!enc->name)
        return -1;

    enc->width  = width;
    enc->height = height;
    enc->rate.numerator   = n;
    enc->rate.denominator = d * 2; /* Refresh rate is twice, due to interlace */
    sprintf(enc->name,"%s-%s",norm,fixname(input));

    xf86Msg(X_INFO, "v4l: adding input %s, %dx%d %d fps\n",
            enc->name, enc->width, enc->height, (d + n - 1)/n);

    return 0;
}

static int
V4LGetStd(PortPrivPtr p, int fd)
{
    struct v4l2_standard standard;
    int entries = 0;

    /*
     * 128 is just an arbitrary large number. There aren't that many video stds
     * The max value is there just to avoid an endless loop, if the driver is
     * broken.
     */
    for (entries = 0; entries < 128; entries++) {
        memset(&standard, 0, sizeof(standard));
        standard.index = entries;
        if (-1 == ioctl(fd,VIDIOC_ENUMSTD, &standard))
            break;
        p->standard = realloc(p->standard, sizeof(standard) * (entries + 1));
        memcpy(&p->standard[entries], &standard, sizeof(standard));
    }

    /*
     * Some webcam drivers don't implement VIDIOC_ENUMSTD. Fake it
     * This is currently unused, as no webcam driver allows OVERLAY mode,
     * but the code is here, in the case some webcam driver starts allowing
     * overlay.
     * FIXME: Webcam drivers may use VIDIOC_ENUM_FRAMESIZES and
     * VIDIOC_ENUM_FRAMEINTERVALS ioctl's that may help to fine-tune
     * their needs. Those ioctl's could be used here in order to better
     * support webcams.
     */
    if (!entries) {
        xf86Msg(X_INFO, "v4l: VIDIOC_ENUMSTD error %d.\n",errno);
        p->standard = realloc(p->standard, sizeof(standard) * (entries + 1));
        p->standard[0].id = V4L2_STD_ALL;
        strcpy((char *)p->standard[0].name, "CAMERA");
        p->standard[0].frameperiod.numerator = 1001;
        p->standard[0].frameperiod.denominator = 300001;
        p->standard[0].framelines = 480;
        entries++;
    }
    return entries;
}

static void
V4LBuildEncodings(PortPrivPtr p, int fd)
{
    unsigned int inp, std, num_std;

    num_std =  V4LGetStd(p, fd);

    p->nenc = 0;

    /*
     * 256 is just an arbitrary large number. There aren't that many video
     * inputs on any driver. The max value is there just to avoid an endless
     * loop, if the driver is broken.
     */
    for (inp = 0; inp < 256; inp++) {
        struct v4l2_input       input;

        memset(&input, 0, sizeof(input));
        input.index = inp;
        if (ioctl(fd, VIDIOC_ENUMINPUT, &input) == -1)
            break;

        for (std = 0; std < num_std; std++) {
            int width, height;

	    /*
	     * Currently, this code is not reliable, due to driver
	     * non-compliance on both saa7134 and bttv. So, instead,
	     * just use the video standard information
	     */
#if 0
	    struct v4l2_framebuffer fbuf;

            /* Some webcam drivers will fail here, but that's OK */
            ioctl(fd, VIDIOC_S_STD, &p->standard[std].id);

            memset(&fbuf, 0, sizeof(fbuf));
            if (ioctl(fd, VIDIOC_G_FBUF, &fbuf) == -1) {
                xf86Msg(X_INFO, "v4l: Error %d: Can't get FBUF\n", errno);
                return;
            }
            height = fbuf.fmt.height;
            width = fbuf.fmt.width;

            /*
             * If the overlay method fails, get the resolution
             * via checking V4L2_BUF_TYPE_VIDEO_CAPTURE
             */
            if (!height || !width) {
                struct v4l2_format format;

                format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (ioctl(fd, VIDIOC_G_FMT, &format) == -1) {
                    /* Hmm... device doesn't support capture. */
                    height = p->standard[std].framelines;
                    if (height == 480)
                        width = 640;
                    else if (height == 576)
                        width = 768;
                    else
                        continue;
                } else {
                    height = format.fmt.pix.height;
                    width = format.fmt.pix.width;
                }
            }
#else
	    if (p->standard[std].id & V4L2_STD_525_60) {
		height = 480;
		width = 640;
	    } else {
		height = 576;
		width = 768;
	    }

#endif
            /* Fixup for some driver bug */
            if ((p->standard[std].id & V4L2_STD_525_60) && (height == 576))
                height = 480;

            p->enc = realloc(p->enc, sizeof(XF86VideoEncodingRec) * (p->nenc + 1));
            p->norm = realloc(p->norm, sizeof(int) * (p->nenc + 1));
            p->input = realloc(p->input, sizeof(int) * (p->nenc + 1));
            if (!p->enc || !p->norm || !p->input)
                goto fail;
            if (AddV4LEnc(&p->enc[p->nenc], p->nenc,
                          (char *)p->standard[std].name,
                          (char *)input.name, width, height,
                          p->standard[std].frameperiod.numerator,
                          p->standard[std].frameperiod.denominator))
                goto fail;
            p->norm[p->nenc]  = p->standard[std].id;
            p->input[p->nenc] = inp;
            p->nenc++;
        }
    }

    if (!p->nenc)
        xf86Msg(X_ERROR, "v4l: couldn't detect any valid input/standard\n");
    return;

fail:
    xf86Msg(X_ERROR, "v4l: Fail to get standards: %d\n", errno);
    if (p->input)
        free(p->input);
    p->input = NULL;
    if (p->norm)
        free(p->norm);
    p->norm = NULL;
    if (p->enc)
        free(p->enc);
    p->enc = NULL;
    p->nenc = 0;
}

/* add a attribute a list */
static void
v4l_add_attr(XF86AttributeRec **list, int *count,
             const XF86AttributeRec *attr)
{
    XF86AttributeRec *oldlist = *list;
    int i;
    Atom gpa;

    for (i = 0; i < *count; i++) {
        if (0 == strcmp((*list)[i].name,attr->name)) {
            DEBUG(xf86Msg(X_INFO, "v4l: skip dup attr %s\n",attr->name));
            return;
        }
    }

    *list = malloc((*count + 1) * sizeof(XF86AttributeRec));
    if (NULL == *list) {
        *count = 0;
        return;
    }
    if (*count)
        memcpy(*list, oldlist, *count * sizeof(XF86AttributeRec));
    memcpy(*list + *count, attr, sizeof(XF86AttributeRec));

    gpa = MAKE_ATOM((*list)[*count].name);
    xf86Msg(X_INFO, "v4l: add attr %s (Xv/GPA %d) (%d to %d)\n",
            attr->name, (int)gpa, attr->min_value, attr->max_value);

    (*count)++;
}

static int
V4LInit(ScrnInfoPtr pScrn, XF86VideoAdaptorPtr **adaptors)
{
    PortPrivPtr pPPriv;
    DevUnion *Private;
    XF86VideoAdaptorPtr *VAR = NULL;
    char dev[18];
    int  fd,i,j,d;

    for (i = 0, d = 0; d < MAX_V4L_DEVICES; d++) {
        sprintf(dev, "/dev/video%d", d);
        fd = open(dev, O_RDWR, 0);
        if (fd == -1) {
            sprintf(dev, "/dev/v4l/video%d", d);
            fd = open(dev, O_RDWR, 0);
            if (fd == -1)
                break;
        }
        DEBUG(xf86Msg(X_INFO,  "v4l: %s open ok\n",dev));

        /* our private data */
        pPPriv = malloc(sizeof(PortPrivRec));
        if (!pPPriv)
            return FALSE;
        memset(pPPriv,0,sizeof(PortPrivRec));
        pPPriv->nr = d;

        /* check device capabilities */
        memset(&pPPriv->cap, 0, sizeof(&pPPriv->cap));
        if (-1 == ioctl(fd, VIDIOC_QUERYCAP, &pPPriv->cap) ||
            0 == (pPPriv->cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)) {
            xf86Msg(X_ERROR, "v4l: %s: no overlay support\n",dev);
            free(pPPriv);
            close(fd);
            continue;
        }

        if (v4l_check_yuv(pScrn, pPPriv, dev, fd) == FALSE) {
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 2,
                               "Xv Overlay not supported. Can't use v4l driver\n");
                free(pPPriv);
                close(fd);
                continue;
        }

        xf86Msg(X_INFO, "v4l: enabling overlay mode for %s.\n", dev);
        strncpy(V4L_NAME, dev, 16);
        V4LBuildEncodings(pPPriv, fd);
        if (NULL == pPPriv->enc)
            return FALSE;

        /* alloc VideoAdaptorRec */
        VAR = realloc(VAR,sizeof(XF86VideoAdaptorPtr)*(i+1));
        VAR[i] = malloc(sizeof(XF86VideoAdaptorRec));
        if (!VAR[i])
            return FALSE;
        memset(VAR[i],0,sizeof(XF86VideoAdaptorRec));

        /* build attribute list */
        AddAllV4LControls(pPPriv, &VAR[i]->pAttributes, &VAR[i]->nAttributes, fd);

        for (j = 0; j < V4L_ATTR; j++) {
            /* Other video attributes */
            v4l_add_attr(&VAR[i]->pAttributes, &VAR[i]->nAttributes,
                         &Attributes[j]);
        }
        if (pPPriv->cap.capabilities & V4L2_CAP_TUNER) {
            /* tuner attributes */
            v4l_add_attr(&VAR[i]->pAttributes, &VAR[i]->nAttributes,
                         &FreqAttr);
        }

        /* Initialize yuv_format */
        if (0 != pPPriv->yuv_format) {
            /* pass throuth scaler attributes */
            for (j = 0; j < pPPriv->myfmt->num_attributes; j++) {
                v4l_add_attr(&VAR[i]->pAttributes, &VAR[i]->nAttributes,
                             pPPriv->myfmt->attributes+j);
            }
        }

        DEBUG(xf86Msg(X_INFO, "v4l: saving config into driver data\n"));
        /* hook in private data */
        Private = malloc(sizeof(DevUnion));
        if (!Private)
            return FALSE;
        memset(Private,0,sizeof(DevUnion));
        Private->ptr = (pointer)pPPriv;
        VAR[i]->pPortPrivates = Private;
        VAR[i]->nPorts = 1;

        /* init VideoAdaptorRec */
        VAR[i]->type  = XvInputMask | XvWindowMask | XvVideoMask;
        VAR[i]->name  = "video4linux";
        VAR[i]->flags = VIDEO_INVERT_CLIPLIST;

        VAR[i]->PutVideo = V4lPutVideo;
        VAR[i]->PutStill = V4lPutStill;
        VAR[i]->StopVideo = V4lStopVideo;
        VAR[i]->SetPortAttribute = V4lSetPortAttribute;
        VAR[i]->GetPortAttribute = V4lGetPortAttribute;
        VAR[i]->QueryBestSize = V4lQueryBestSize;

        VAR[i]->nEncodings = pPPriv->nenc;
        VAR[i]->pEncodings = pPPriv->enc;
        VAR[i]->nFormats = sizeof(InputVideoFormats) / sizeof(InputVideoFormats[0]);
        VAR[i]->pFormats = InputVideoFormats;

        if (fd != -1) {
            DEBUG(xf86Msg(X_INFO,  "v4l: %s init finished.\n",dev));
            close(fd);
        }
        i++;
    }

    xvEncoding   = MAKE_ATOM(XV_ENCODING);
    xvFreq       = MAKE_ATOM(XV_FREQ);

    DEBUG(xf86Msg(X_INFO, "v4l: init done, %d device(s) found\n",i));

    *adaptors = VAR;
    return i;
}

static Bool
V4LDetect(void)
{
    struct v4l2_capability cap;
    int fd, d, n = 0;
    char dev[18];

    for (d = 0; d < MAX_V4L_DEVICES; d++) {
        sprintf(dev, "/dev/video%d", d);
        fd = open(dev, O_RDWR, 0);
        if (fd == -1) {
            sprintf(dev, "/dev/v4l/video%d", d);
            fd = open(dev, O_RDWR, 0);
            if (fd == -1)
                break;
        }
        close (fd);
        memset(&cap, 0, sizeof(cap));
        if (!ioctl(fd, VIDIOC_QUERYCAP, &cap) &&
            (cap.capabilities & V4L2_CAP_VIDEO_OVERLAY))
            n++;
    }
    xf86Msg(X_INFO, "v4l: %d video adapters with overlay support detected\n", n);

    return (n > 0) ? TRUE : FALSE;
}

static Bool
V4LProbe(DriverPtr drv, int flags)
{
    /*
     * Don't try to register adapter at the detection phase, as Xv
     * extensions won't be there
     */
    if (flags == PROBE_DETECT)
        return V4LDetect();

    DEBUG(xf86Msg(X_INFO, "v4l: Initiating device probe\n"));

    xf86XVRegisterGenericAdaptorDriver(V4LInit);
    drv->refCount++;
    return TRUE;
}
