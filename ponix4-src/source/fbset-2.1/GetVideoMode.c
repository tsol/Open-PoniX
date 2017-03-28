
/*
 *    Under AmigaOS: gcc -noixemul GetVideoMode.c -o GetVideoMode
 *
 *    Usage:
 *
 *       GetVideoMode
 *       GetVideoMode NONAMEOK
 *       GetVideoMode <hexmodeid> ...
 */


#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <graphics/displayinfo.h>
#include <exec/execbase.h>
#include <string.h>
#include <stdio.h>


#define LOW       0
#define HIGH      1


struct VideoMode {
   STRPTR name;
   struct {
      ULONG xres;
      ULONG yres;
      ULONG vxres;
      ULONG vyres;
      ULONG depth;
   } geometry;
   struct {
      ULONG pixclock;
      ULONG left;
      ULONG right;
      ULONG upper;
      ULONG lower;
      ULONG hslen;
      ULONG vslen;
   } timings;
   struct {
      ULONG hsync    : 1;
      ULONG vsync    : 1;
      ULONG csync    : 1;
      ULONG gsync    : 1;
      ULONG extsync  : 1;
      ULONG bcast    : 1;
      ULONG laced    : 1;
      ULONG dblscan  : 1;
   } options;
};


struct DisplayInfo DisplayInfo;
struct DimensionInfo DimensionInfo;
struct MonitorInfo MonitorInfo;
struct NameInfo NameInfo;
struct VideoMode VideoMode;


void GetVideoMode(ULONG id, BOOL nonameok)
{
   ULONG colorclock_to_dotclock;
   ULONG diwstrt_h, diwstop_h, diwstrt_v, diwstop_v;
   ULONG hsstrt, hsstop, htotal, vsstrt, vsstop, vtotal;

   if (!GetDisplayInfoData(NULL, (UBYTE *)&DisplayInfo, sizeof(DisplayInfo),
                           DTAG_DISP, id) ||
       !GetDisplayInfoData(NULL, (UBYTE *)&DimensionInfo,
                           sizeof(DimensionInfo), DTAG_DIMS, id) ||
       !GetDisplayInfoData(NULL, (UBYTE *)&MonitorInfo, sizeof(MonitorInfo),
                           DTAG_MNTR, id))
      return;

   if (GetDisplayInfoData(NULL, (UBYTE *)&NameInfo, sizeof(NameInfo), DTAG_NAME,
                          id))
      VideoMode.name = NameInfo.Name;
   else if (nonameok)
      VideoMode.name = NULL;
   else
      return;

   VideoMode.geometry.xres = DimensionInfo.Nominal.MaxX+1;
   VideoMode.geometry.yres = DimensionInfo.Nominal.MaxY+1;
   VideoMode.geometry.vxres = VideoMode.geometry.xres;
   VideoMode.geometry.vyres = VideoMode.geometry.yres;
   VideoMode.geometry.depth = DimensionInfo.MaxDepth < 2 ? 1 :
                              DimensionInfo.MaxDepth>>1;
   VideoMode.options.hsync = MonitorInfo.Mspc->BeamCon0 & HSYNCTRUE ? HIGH : LOW;
   VideoMode.options.vsync = MonitorInfo.Mspc->BeamCon0 & VSYNCTRUE ? HIGH : LOW;
   VideoMode.options.csync = MonitorInfo.Mspc->BeamCon0 & CSYNCTRUE ? HIGH : LOW;
   VideoMode.options.gsync = MonitorInfo.Mspc->BeamCon0 & GSYNCTRUE ? TRUE : FALSE;
   VideoMode.options.extsync = FALSE;
   VideoMode.options.laced = DisplayInfo.PropertyFlags & DIPF_IS_LACE ? TRUE : FALSE;
   VideoMode.options.dblscan = DisplayInfo.PropertyFlags & DIPF_IS_SCANDBL ? TRUE : FALSE;
   VideoMode.timings.pixclock = (double)DisplayInfo.PixelSpeed/35*25E9/
                                SysBase->ex_EClockFrequency;
   switch (DisplayInfo.PixelSpeed) {
      case 35:
         VideoMode.timings.pixclock = 25E9/SysBase->ex_EClockFrequency;
         colorclock_to_dotclock = 8;
         break;

      case 70:
         VideoMode.timings.pixclock = 50E9/SysBase->ex_EClockFrequency;
         colorclock_to_dotclock = 4;
         break;

      case 140:
         VideoMode.timings.pixclock = 100E9/SysBase->ex_EClockFrequency;
         colorclock_to_dotclock = 2;
         break;

      default:
         printf("Unexpected pixel clock of %d ns\n", DisplayInfo.PixelSpeed);
         return;
   }

   diwstrt_h = MonitorInfo.DefaultViewPosition.x*colorclock_to_dotclock>>1;
   diwstrt_v = MonitorInfo.DefaultViewPosition.y;
   hsstrt = MonitorInfo.Mspc->ms_Special->hsync.asi_Start*colorclock_to_dotclock;
   hsstop = MonitorInfo.Mspc->ms_Special->hsync.asi_Stop*colorclock_to_dotclock;
   htotal = (MonitorInfo.TotalColorClocks+1)*colorclock_to_dotclock;
   vsstrt = MonitorInfo.Mspc->ms_Special->vsync.asi_Start/MonitorInfo.TotalColorClocks;
   vsstop = MonitorInfo.Mspc->ms_Special->vsync.asi_Stop/MonitorInfo.TotalColorClocks;
   vtotal = MonitorInfo.TotalRows+1;
   if (VideoMode.options.laced) {
      diwstrt_v <<= 1;
      vsstrt <<= 1;
      vsstop <<= 1;
      vtotal <<= 1;
   }
   if (VideoMode.options.dblscan) {
      diwstrt_v >>= 1;
      vsstrt >>= 1;
      vsstop >>= 1;
      vtotal >>= 1;
   }
   diwstop_h = diwstrt_h+VideoMode.geometry.xres;
   diwstop_v = diwstrt_v+VideoMode.geometry.yres;

   if (MonitorInfo.Mspc->ms_Flags & MSF_REQUEST_SPECIAL) {
      VideoMode.timings.left = diwstrt_h-hsstop;
      VideoMode.timings.right = htotal+hsstrt-diwstop_h;
      VideoMode.timings.upper = diwstrt_v-vsstop;
      VideoMode.timings.lower = vtotal+vsstrt-diwstop_v;
      VideoMode.timings.hslen = hsstop-hsstrt;
      VideoMode.timings.vslen = vsstop-vsstrt;
      VideoMode.options.bcast = FALSE;
   } else {

      /*
       *    Not yet implemented
       */

      VideoMode.timings.left = 0;
      VideoMode.timings.right = 0;
      VideoMode.timings.upper = 0;
      VideoMode.timings.lower = 0;
      VideoMode.timings.hslen = 0;
      VideoMode.timings.vslen = 0;
      VideoMode.options.bcast = TRUE;
   }

   if (VideoMode.name)
      printf("mode \"%.32s\"\n", VideoMode.name);
   else
      printf("mode \"0x%08x\"\n", id);
   printf("\tgeometry %d %d %d %d %d\n", VideoMode.geometry.xres,
          VideoMode.geometry.yres, VideoMode.geometry.vxres,
          VideoMode.geometry.vyres, VideoMode.geometry.depth);
   printf("\ttimings %d %d %d %d %d %d %d\n", VideoMode.timings.pixclock,
          VideoMode.timings.left, VideoMode.timings.right,
          VideoMode.timings.upper, VideoMode.timings.lower,
          VideoMode.timings.hslen, VideoMode.timings.vslen);
   if (VideoMode.options.hsync)
      puts("\thsync high");
   if (VideoMode.options.vsync)
      puts("\tvsync high");
   if (VideoMode.options.csync)
      puts("\tcsync high");
   if (VideoMode.options.extsync)
      puts("\textsync true");
   if (VideoMode.options.bcast)
      puts("\tbcast true");
   if (VideoMode.options.laced)
      puts("\tlaced true");
   if (VideoMode.options.dblscan)
      puts("\tdouble true");
   puts("endmode\n");
}


int main(void)
{
   struct RDArgs *rdargs;
   LONG args[] = {
      NULL,
      NULL
   };
   STRPTR *ids;
   ULONG id;

   if (!(rdargs = ReadArgs("ID/M,NONAMEOK/S", args, NULL)))
      PrintFault(IoErr(), NULL);
   else if (ids = (STRPTR *)args[0])
      do {
         sscanf(*ids++, "%x", &id);
         GetVideoMode(id, 1);
      } while (*ids);
   else
      for (id = INVALID_ID; (id = NextDisplayInfo(id)) != INVALID_ID;)
         GetVideoMode(id, args[1]);

   return 0;
}
