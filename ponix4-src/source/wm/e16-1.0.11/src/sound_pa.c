/*
 * Copyright (C) 2008-2009 Kim Woelders
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
#if defined(HAVE_SOUND) && defined(HAVE_SOUND_PA)
#include "sound.h"
#include <fcntl.h>
#include <pulse/pulseaudio.h>
#include <sys/stat.h>

#ifdef USE_MODULES
#define Estrdup strdup
#endif

#define DEBUG_PA 0
#if DEBUG_PA
#define D2printf(fmt...) if(EDebug(EDBUG_TYPE_SOUND)>1)Eprintf(fmt)
#define D3printf(fmt...) if(EDebug(EDBUG_TYPE_SOUND)>2)Eprintf(fmt)
#define D4printf(fmt...) if(EDebug(EDBUG_TYPE_SOUND)>3)Eprintf(fmt)
#else
#define D2printf(fmt...)
#define D3printf(fmt...)
#define D4printf(fmt...)
#endif

struct _sample {
   SoundSampleData     ssd;
   char               *name;
   unsigned int        written;
};

static pa_mainloop *pa_mloop = NULL;
static pa_mainloop_api *mainloop_api = NULL;
static pa_context  *pa_ctx = NULL;

static pa_stream   *sample_stream = NULL;
static size_t       sample_length = 0;
static int          pa_block = 0;

static void         _sound_pa_Exit(void);

static int
dispatch(int block)
{
   int                 err, rc;

   D3printf("%s: beg\n", __func__);
   rc = 1234;
   pa_block = block;
   for (;;)
     {
	err = pa_mainloop_iterate(pa_mloop, pa_block, &rc);
	D4printf("%s: run err=%d rc=%d\n", __func__, err, rc);
	if (err <= 0)
	   break;
     }

   if (err < 0)
      _sound_pa_Exit();

   D3printf("%s: end\n", __func__);
   return err;
}

static void
context_op_callback(pa_context * pac __UNUSED__, int success __UNUSED__,
		    void *userdata __UNUSED__)
{
   D2printf("%s: succ=%d %s\n", __func__, success,
	    (success) ? "" : pa_strerror(pa_context_errno(pac)));
   pa_block = 0;
}

static void
context_drain_complete(pa_context * pac __UNUSED__, void *userdata __UNUSED__)
{
   D2printf("%s\n", __func__);
}

static void
context_drain(pa_context * pac)
{
   pa_operation       *op;

   D2printf("%s\n", __func__);
   op = pa_context_drain(pac, context_drain_complete, NULL);
   if (op)
      pa_operation_unref(op);
   pa_block = 0;
}

static void
stream_state_callback(pa_stream * pas, void *userdata __UNUSED__)
{
   D2printf("%s: state=%d\n", __func__, pa_stream_get_state(pas));
   switch (pa_stream_get_state(pas))
     {
     case PA_STREAM_CREATING:	/* 1 */
     case PA_STREAM_READY:	/* 2 */
	break;

     case PA_STREAM_TERMINATED:	/* 4 */
	context_drain(pa_stream_get_context(pas));
	break;

     case PA_STREAM_FAILED:	/* 3 */
     default:
	Eprintf("PA failure: %s\n",
		pa_strerror(pa_context_errno(pa_stream_get_context(pas))));
	pa_block = 0;
	break;
     }
}

static void
stream_write_callback(pa_stream * pas, size_t length, void *userdata)
{
   D2printf("%s: state=%d length=%d\n", __func__, pa_stream_get_state(pas),
	    length);
   Sample             *s = userdata;
   unsigned int        left;

   left = s->ssd.size - s->written;
   length = (left > length) ? length : left;
   pa_stream_write(pas, s->ssd.data, length, NULL, 0, PA_SEEK_RELATIVE);
   s->written += length;

   D2printf("%s: size=%d written=%d\n", __func__, s->ssd.size, s->written);
   if (s->written >= s->ssd.size)
     {
	pa_stream_set_write_callback(pas, NULL, NULL);
	pa_stream_finish_upload(pas);
     }
}

static void
context_state_callback(pa_context * pac, void *userdata __UNUSED__)
{
   D2printf("%s: state=%d\n", __func__, pa_context_get_state(pac));
   switch (pa_context_get_state(pac))
     {
     case PA_CONTEXT_CONNECTING:	/* 1 */
     case PA_CONTEXT_AUTHORIZING:	/* 2 */
     case PA_CONTEXT_SETTING_NAME:	/* 3 */
	break;

     case PA_CONTEXT_READY:	/* 4 */
	pa_block = 0;
	break;

     case PA_CONTEXT_TERMINATED:	/* 6 */
	break;

     case PA_CONTEXT_FAILED:	/* 5 */
     default:
	Eprintf("PA failure: %s\n", pa_strerror(pa_context_errno(pac)));
	pa_mainloop_quit(pa_mloop, 1);
	break;
     }
}

static void
_sound_pa_Destroy(Sample * s)
{
   pa_operation       *op;

   D2printf("%s beg: %s\n", __func__, s ? s->name : "?");
   if (!s)
      return;

   if (pa_ctx && s->name)
     {
	op =
	   pa_context_remove_sample(pa_ctx, s->name, context_op_callback, NULL);
	if (op)
	   pa_operation_unref(op);
	dispatch(-1);
     }
   D2printf("%s end\n", __func__);

   _EFREE(s->name);
   _EFREE(s->ssd.data);
   Efree(s);
}

static Sample      *
_sound_pa_Load(const char *file)
{
   Sample             *s;
   pa_sample_spec      sample_spec;
   int                 err;
   char               *p;

   if (!pa_ctx)
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
   s->name = Estrdup(file);
   if (!s->name)
      goto bail_out;
   for (p = s->name; *p != '\0'; p++)
      if (*p == '/')
	 *p = '_';

   switch (s->ssd.bit_per_sample)
     {
     case 8:
	sample_spec.format = PA_SAMPLE_U8;
	break;
     default:
	sample_spec.format = PA_SAMPLE_S16NE;
	break;
     }
   sample_spec.rate = s->ssd.rate;
   sample_spec.channels = s->ssd.channels;
   sample_length = s->ssd.size;

   sample_stream = pa_stream_new(pa_ctx, s->name, &sample_spec, NULL);
   if (!sample_stream)
      goto bail_out;
   pa_stream_set_state_callback(sample_stream, stream_state_callback, NULL);
   pa_stream_set_write_callback(sample_stream, stream_write_callback, s);
   pa_stream_connect_upload(sample_stream, sample_length);

   err = dispatch(-1);
   if (err)
      goto bail_out;

   _EFREE(s->ssd.data);

   return s;

 bail_out:
   _sound_pa_Destroy(s);
   return NULL;
}

static void
_sound_pa_Play(Sample * s)
{
   pa_operation       *op;

   D2printf("%s beg: %s\n", __func__, s ? s->name : "?");
   if (!pa_ctx || !s)
      return;

   op = pa_context_play_sample(pa_ctx, s->name, NULL, PA_VOLUME_NORM,
			       context_op_callback, NULL);
   if (op)
      pa_operation_unref(op);
   dispatch(-1);
   D2printf("%s end\n", __func__);
}

static void
_sound_pa_Exit(void)
{
   D2printf("%s\n", __func__);
#if 0
   if (stream)
      pa_stream_unref(stream);
#endif

   if (pa_ctx)
     {
	pa_context_disconnect(pa_ctx);
	pa_context_unref(pa_ctx);
	pa_ctx = NULL;
     }

   if (pa_mloop)
     {
	pa_mainloop_quit(pa_mloop, 0);
	pa_mainloop_free(pa_mloop);
	pa_mloop = NULL;
     }
}

static int
_sound_pa_Init(void)
{
   int                 err;

   /* Set up a new main loop */
   pa_mloop = pa_mainloop_new();
   if (!pa_mloop)
     {
	Eprintf("pa_mainloop_new() failed.\n");
	goto quit;
     }

   mainloop_api = pa_mainloop_get_api(pa_mloop);

   /* Create a new connection context */
   pa_ctx = pa_context_new(mainloop_api, "e16");
   if (!pa_ctx)
     {
	Eprintf("pa_context_new() failed.\n");
	goto quit;
     }

   pa_context_set_state_callback(pa_ctx, context_state_callback, NULL);

   /* Connect the context */
   err = pa_context_connect(pa_ctx, NULL, 0, NULL);
   if (err)
      Eprintf("pa_context_connect(): %s\n", pa_strerror(err));

   err = dispatch(-1);
   if (err)
      goto quit;

 done:
   return !pa_ctx;
 quit:
   _sound_pa_Exit();
   goto done;
}

__EXPORT__ extern const SoundOps SoundOps_pa;

const SoundOps      SoundOps_pa = {
   _sound_pa_Init, _sound_pa_Exit, _sound_pa_Load, _sound_pa_Destroy,
   _sound_pa_Play,
};

#endif /* HAVE_SOUND && HAVE_SOUND_PA */
