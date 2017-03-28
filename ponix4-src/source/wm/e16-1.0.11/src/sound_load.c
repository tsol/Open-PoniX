/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#if HAVE_SOUND
#if USE_SOUND_LOADER_AUDIOFILE
#include "sound.h"
#include <audiofile.h>

int
SoundSampleGetData(const char *file, SoundSampleData * ssd)
{
   AFfilehandle        in_file;
   int                 in_format, in_width, bytes_per_frame, frame_count;
   int                 frames_read;

   in_file = afOpenFile(file, "r", NULL);
   if (!in_file)
      return -1;

   frame_count = afGetFrameCount(in_file, AF_DEFAULT_TRACK);
   ssd->channels = afGetChannels(in_file, AF_DEFAULT_TRACK);
   ssd->rate = (unsigned int)(afGetRate(in_file, AF_DEFAULT_TRACK) + .5);
   afGetSampleFormat(in_file, AF_DEFAULT_TRACK, &in_format, &in_width);
   ssd->bit_per_sample = in_width;
#ifdef WORDS_BIGENDIAN
   afSetVirtualByteOrder(in_file, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);
#else
   afSetVirtualByteOrder(in_file, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
#endif
   if (EDebug(EDBUG_TYPE_SOUND))
      Eprintf("SoundSampleGetData chan=%d width=%d rate=%d\n", ssd->channels,
	      ssd->bit_per_sample, ssd->rate);

   bytes_per_frame = (ssd->bit_per_sample * ssd->channels) / 8;
   ssd->size = frame_count * bytes_per_frame;
   ssd->data = EMALLOC(unsigned char, ssd->size);

   frames_read =
      afReadFrames(in_file, AF_DEFAULT_TRACK, ssd->data, frame_count);

   afCloseFile(in_file);

   if (frames_read <= 0)
     {
	ssd->size = 0;
	_EFREE(ssd->data);
	return -1;
     }

   return 0;
}

#endif /* USE_SOUND_LOADER_AUDIOFILE */

#endif /* HAVE_SOUND */
