/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2009 Kim Woelders
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
#if defined(HAVE_SOUND) && defined(HAVE_SOUND_ESD)
#include "sound.h"
#include <esd.h>

#ifdef USE_MODULES
#define Estrdup strdup
#endif

struct _sample {
   SoundSampleData     ssd;
   int                 id;
};

static int          sound_fd = -1;

static Sample      *
_sound_esd_Load(const char *file)
{
   Sample             *s;
   int                 err, format;

   if (sound_fd < 0)
      return NULL;

   s = ECALLOC(Sample, 1);
   if (!s)
      return NULL;

   err = SoundSampleGetData(file, &s->ssd);
   if (err)
     {
	Efree(s);
	return NULL;
     }

   format = ESD_STREAM | ESD_PLAY;
   if (s->ssd.bit_per_sample == 8)
      format |= ESD_BITS8;
   else if (s->ssd.bit_per_sample == 16)
      format |= ESD_BITS16;
   if (s->ssd.channels == 1)
      format |= ESD_MONO;
   else if (s->ssd.channels == 2)
      format |= ESD_STEREO;

   s->id = esd_sample_getid(sound_fd, file);
   if (s->id < 0)
     {
	int                 confirm;

	s->id = esd_sample_cache(sound_fd, format, s->ssd.rate, s->ssd.size,
				 file);
	if (write(sound_fd, s->ssd.data, s->ssd.size) != (ssize_t) s->ssd.size)
	  {
	     s->id = 0;
	     goto done;
	  }
	confirm = esd_confirm_sample_cache(sound_fd);
	if (confirm != s->id)
	   s->id = 0;
     }

 done:
   _EFREE(s->ssd.data);
   if (s->id <= 0)
      _EFREE(s);

   return s;
}

static void
_sound_esd_Destroy(Sample * s)
{
   if (!s)
      return;

   if (s->id && sound_fd >= 0)
     {
/*      Why the hell is this symbol not in esd? */
/*      it's in esd.h - evil evil evil */
/*      esd_sample_kill(sound_fd,s->id); */
	esd_sample_free(sound_fd, s->id);
     }
   _EFREE(s->ssd.data);
   Efree(s);
}

static void
_sound_esd_Play(Sample * s)
{
   if (sound_fd < 0 || !s)
      return;

   if (s->id > 0)
      esd_sample_play(sound_fd, s->id);
}

static int
_sound_esd_Init(void)
{
   if (sound_fd >= 0)
      return 0;

   sound_fd = esd_open_sound(NULL);

   return sound_fd < 0;
}

static void
_sound_esd_Exit(void)
{
   if (sound_fd < 0)
      return;

   close(sound_fd);
   sound_fd = -1;
}

__EXPORT__ extern const SoundOps SoundOps_esd;

const SoundOps      SoundOps_esd = {
   _sound_esd_Init, _sound_esd_Exit, _sound_esd_Load, _sound_esd_Destroy,
   _sound_esd_Play,
};

#endif /* HAVE_SOUND && HAVE_SOUND_ESD */
