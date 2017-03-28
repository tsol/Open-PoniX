/* DO NOT EDIT - This file generated automatically by remap_helper.py (from Mesa) script */

/*
 * Copyright (C) 2009 Chia-I Wu <olv@0xlab.org>
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * Chia-I Wu,
 * AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "main/dispatch.h"
#include "main/remap.h"

/* this is internal to remap.c */
#ifndef need_MESA_remap_table
#error Only remap.c should include this file!
#endif /* need_MESA_remap_table */


static const char _mesa_function_pool[] =
   /* _mesa_function_pool[0]: MapGrid1d (offset 224) */
   "idd\0"
   "glMapGrid1d\0"
   "\0"
   /* _mesa_function_pool[17]: MapGrid1f (offset 225) */
   "iff\0"
   "glMapGrid1f\0"
   "\0"
   /* _mesa_function_pool[34]: ReplacementCodeuiVertex3fvSUN (dynamic) */
   "pp\0"
   "glReplacementCodeuiVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[70]: PolygonOffsetx (will be remapped) */
   "ii\0"
   "glPolygonOffsetxOES\0"
   "glPolygonOffsetx\0"
   "\0"
   /* _mesa_function_pool[111]: FramebufferTexture (will be remapped) */
   "iiii\0"
   "glFramebufferTextureARB\0"
   "glFramebufferTexture\0"
   "\0"
   /* _mesa_function_pool[162]: TexCoordP1ui (will be remapped) */
   "ii\0"
   "glTexCoordP1ui\0"
   "\0"
   /* _mesa_function_pool[181]: PolygonStipple (offset 175) */
   "p\0"
   "glPolygonStipple\0"
   "\0"
   /* _mesa_function_pool[201]: ListParameterfSGIX (dynamic) */
   "iif\0"
   "glListParameterfSGIX\0"
   "\0"
   /* _mesa_function_pool[227]: MultiTexCoord1dv (offset 377) */
   "ip\0"
   "glMultiTexCoord1dv\0"
   "glMultiTexCoord1dvARB\0"
   "\0"
   /* _mesa_function_pool[272]: IsEnabled (offset 286) */
   "i\0"
   "glIsEnabled\0"
   "\0"
   /* _mesa_function_pool[287]: GetTexFilterFuncSGIS (dynamic) */
   "iip\0"
   "glGetTexFilterFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[315]: AttachShader (will be remapped) */
   "ii\0"
   "glAttachShader\0"
   "\0"
   /* _mesa_function_pool[334]: VertexAttrib3fARB (will be remapped) */
   "ifff\0"
   "glVertexAttrib3f\0"
   "glVertexAttrib3fARB\0"
   "\0"
   /* _mesa_function_pool[377]: Indexubv (offset 316) */
   "p\0"
   "glIndexubv\0"
   "\0"
   /* _mesa_function_pool[391]: MultiTexCoordP3uiv (will be remapped) */
   "iip\0"
   "glMultiTexCoordP3uiv\0"
   "\0"
   /* _mesa_function_pool[417]: Color4fNormal3fVertex3fSUN (dynamic) */
   "ffffffffff\0"
   "glColor4fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[458]: Color3ubv (offset 20) */
   "p\0"
   "glColor3ubv\0"
   "\0"
   /* _mesa_function_pool[473]: GetCombinerOutputParameterfvNV (dynamic) */
   "iiip\0"
   "glGetCombinerOutputParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[512]: Binormal3ivEXT (dynamic) */
   "p\0"
   "glBinormal3ivEXT\0"
   "\0"
   /* _mesa_function_pool[532]: GetImageTransformParameterfvHP (dynamic) */
   "iip\0"
   "glGetImageTransformParameterfvHP\0"
   "\0"
   /* _mesa_function_pool[570]: GetClipPlanex (will be remapped) */
   "ip\0"
   "glGetClipPlanexOES\0"
   "glGetClipPlanex\0"
   "\0"
   /* _mesa_function_pool[609]: TexCoordP1uiv (will be remapped) */
   "ip\0"
   "glTexCoordP1uiv\0"
   "\0"
   /* _mesa_function_pool[629]: RenderbufferStorage (will be remapped) */
   "iiii\0"
   "glRenderbufferStorage\0"
   "glRenderbufferStorageEXT\0"
   "glRenderbufferStorageOES\0"
   "\0"
   /* _mesa_function_pool[707]: GetClipPlanef (will be remapped) */
   "ip\0"
   "glGetClipPlanefOES\0"
   "glGetClipPlanef\0"
   "\0"
   /* _mesa_function_pool[746]: GetPerfQueryDataINTEL (will be remapped) */
   "iiipp\0"
   "glGetPerfQueryDataINTEL\0"
   "\0"
   /* _mesa_function_pool[777]: DrawArraysIndirect (will be remapped) */
   "ip\0"
   "glDrawArraysIndirect\0"
   "\0"
   /* _mesa_function_pool[802]: Uniform3i (will be remapped) */
   "iiii\0"
   "glUniform3i\0"
   "glUniform3iARB\0"
   "\0"
   /* _mesa_function_pool[835]: VDPAUGetSurfaceivNV (will be remapped) */
   "iiipp\0"
   "glVDPAUGetSurfaceivNV\0"
   "\0"
   /* _mesa_function_pool[864]: ReadInstrumentsSGIX (dynamic) */
   "i\0"
   "glReadInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[889]: Uniform3f (will be remapped) */
   "ifff\0"
   "glUniform3f\0"
   "glUniform3fARB\0"
   "\0"
   /* _mesa_function_pool[922]: UniformMatrix2x4fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix2x4fv\0"
   "\0"
   /* _mesa_function_pool[949]: QueryMatrixxOES (will be remapped) */
   "pp\0"
   "glQueryMatrixxOES\0"
   "\0"
   /* _mesa_function_pool[971]: Normal3iv (offset 59) */
   "p\0"
   "glNormal3iv\0"
   "\0"
   /* _mesa_function_pool[986]: DrawTexiOES (will be remapped) */
   "iiiii\0"
   "glDrawTexiOES\0"
   "\0"
   /* _mesa_function_pool[1007]: Viewport (offset 305) */
   "iiii\0"
   "glViewport\0"
   "\0"
   /* _mesa_function_pool[1024]: ReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN (dynamic) */
   "pppp\0"
   "glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[1080]: WindowPos4svMESA (will be remapped) */
   "p\0"
   "glWindowPos4svMESA\0"
   "\0"
   /* _mesa_function_pool[1102]: FragmentLightModelivSGIX (dynamic) */
   "ip\0"
   "glFragmentLightModelivSGIX\0"
   "\0"
   /* _mesa_function_pool[1133]: DeleteVertexArrays (will be remapped) */
   "ip\0"
   "glDeleteVertexArrays\0"
   "glDeleteVertexArraysAPPLE\0"
   "glDeleteVertexArraysOES\0"
   "\0"
   /* _mesa_function_pool[1208]: ClearColorIuiEXT (will be remapped) */
   "iiii\0"
   "glClearColorIuiEXT\0"
   "\0"
   /* _mesa_function_pool[1233]: GetnConvolutionFilterARB (will be remapped) */
   "iiiip\0"
   "glGetnConvolutionFilterARB\0"
   "\0"
   /* _mesa_function_pool[1267]: GetLightxv (will be remapped) */
   "iip\0"
   "glGetLightxvOES\0"
   "glGetLightxv\0"
   "\0"
   /* _mesa_function_pool[1301]: GetConvolutionParameteriv (offset 358) */
   "iip\0"
   "glGetConvolutionParameteriv\0"
   "glGetConvolutionParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[1365]: VertexAttrib4usv (will be remapped) */
   "ip\0"
   "glVertexAttrib4usv\0"
   "glVertexAttrib4usvARB\0"
   "\0"
   /* _mesa_function_pool[1410]: TextureStorage1DEXT (will be remapped) */
   "iiiii\0"
   "glTextureStorage1DEXT\0"
   "\0"
   /* _mesa_function_pool[1439]: VertexAttrib4Nub (will be remapped) */
   "iiiii\0"
   "glVertexAttrib4Nub\0"
   "glVertexAttrib4NubARB\0"
   "\0"
   /* _mesa_function_pool[1487]: VertexAttribP3ui (will be remapped) */
   "iiii\0"
   "glVertexAttribP3ui\0"
   "\0"
   /* _mesa_function_pool[1512]: Color4ubVertex3fSUN (dynamic) */
   "iiiifff\0"
   "glColor4ubVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[1543]: PointSize (offset 173) */
   "f\0"
   "glPointSize\0"
   "\0"
   /* _mesa_function_pool[1558]: TexCoord2fVertex3fSUN (dynamic) */
   "fffff\0"
   "glTexCoord2fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[1589]: PopName (offset 200) */
   "\0"
   "glPopName\0"
   "\0"
   /* _mesa_function_pool[1601]: VertexAttrib4ubNV (will be remapped) */
   "iiiii\0"
   "glVertexAttrib4ubNV\0"
   "\0"
   /* _mesa_function_pool[1628]: ValidateProgramPipeline (will be remapped) */
   "i\0"
   "glValidateProgramPipeline\0"
   "glValidateProgramPipelineEXT\0"
   "\0"
   /* _mesa_function_pool[1686]: BindFragDataLocationIndexed (will be remapped) */
   "iiip\0"
   "glBindFragDataLocationIndexed\0"
   "\0"
   /* _mesa_function_pool[1722]: GetClipPlane (offset 259) */
   "ip\0"
   "glGetClipPlane\0"
   "\0"
   /* _mesa_function_pool[1741]: CombinerParameterfvNV (dynamic) */
   "ip\0"
   "glCombinerParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[1769]: TexCoordP4uiv (will be remapped) */
   "ip\0"
   "glTexCoordP4uiv\0"
   "\0"
   /* _mesa_function_pool[1789]: VertexAttribs3dvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs3dvNV\0"
   "\0"
   /* _mesa_function_pool[1815]: GenQueries (will be remapped) */
   "ip\0"
   "glGenQueries\0"
   "glGenQueriesARB\0"
   "\0"
   /* _mesa_function_pool[1848]: ProgramUniform4iv (will be remapped) */
   "iiip\0"
   "glProgramUniform4iv\0"
   "glProgramUniform4ivEXT\0"
   "\0"
   /* _mesa_function_pool[1897]: ObjectUnpurgeableAPPLE (will be remapped) */
   "iii\0"
   "glObjectUnpurgeableAPPLE\0"
   "\0"
   /* _mesa_function_pool[1927]: TexCoord2iv (offset 107) */
   "p\0"
   "glTexCoord2iv\0"
   "\0"
   /* _mesa_function_pool[1944]: TexImage2DMultisample (will be remapped) */
   "iiiiii\0"
   "glTexImage2DMultisample\0"
   "\0"
   /* _mesa_function_pool[1976]: TexParameterx (will be remapped) */
   "iii\0"
   "glTexParameterxOES\0"
   "glTexParameterx\0"
   "\0"
   /* _mesa_function_pool[2016]: Rotatef (offset 300) */
   "ffff\0"
   "glRotatef\0"
   "\0"
   /* _mesa_function_pool[2032]: TexParameterf (offset 178) */
   "iif\0"
   "glTexParameterf\0"
   "\0"
   /* _mesa_function_pool[2053]: TexParameteri (offset 180) */
   "iii\0"
   "glTexParameteri\0"
   "\0"
   /* _mesa_function_pool[2074]: GetUniformiv (will be remapped) */
   "iip\0"
   "glGetUniformiv\0"
   "glGetUniformivARB\0"
   "\0"
   /* _mesa_function_pool[2112]: ClearBufferSubData (will be remapped) */
   "iiiiiip\0"
   "glClearBufferSubData\0"
   "\0"
   /* _mesa_function_pool[2142]: VDPAUFiniNV (will be remapped) */
   "\0"
   "glVDPAUFiniNV\0"
   "\0"
   /* _mesa_function_pool[2158]: GlobalAlphaFactordSUN (dynamic) */
   "d\0"
   "glGlobalAlphaFactordSUN\0"
   "\0"
   /* _mesa_function_pool[2185]: ProgramUniformMatrix4x2fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix4x2fv\0"
   "glProgramUniformMatrix4x2fvEXT\0"
   "\0"
   /* _mesa_function_pool[2251]: ProgramUniform2f (will be remapped) */
   "iiff\0"
   "glProgramUniform2f\0"
   "glProgramUniform2fEXT\0"
   "\0"
   /* _mesa_function_pool[2298]: PixelTransformParameterfvEXT (dynamic) */
   "iip\0"
   "glPixelTransformParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[2334]: ProgramUniform2i (will be remapped) */
   "iiii\0"
   "glProgramUniform2i\0"
   "glProgramUniform2iEXT\0"
   "\0"
   /* _mesa_function_pool[2381]: Fogx (will be remapped) */
   "ii\0"
   "glFogxOES\0"
   "glFogx\0"
   "\0"
   /* _mesa_function_pool[2402]: Fogf (offset 153) */
   "if\0"
   "glFogf\0"
   "\0"
   /* _mesa_function_pool[2413]: TexSubImage1D (offset 332) */
   "iiiiiip\0"
   "glTexSubImage1D\0"
   "glTexSubImage1DEXT\0"
   "\0"
   /* _mesa_function_pool[2457]: Color4usv (offset 40) */
   "p\0"
   "glColor4usv\0"
   "\0"
   /* _mesa_function_pool[2472]: Fogi (offset 155) */
   "ii\0"
   "glFogi\0"
   "\0"
   /* _mesa_function_pool[2483]: FinalCombinerInputNV (dynamic) */
   "iiii\0"
   "glFinalCombinerInputNV\0"
   "\0"
   /* _mesa_function_pool[2512]: DepthFunc (offset 245) */
   "i\0"
   "glDepthFunc\0"
   "\0"
   /* _mesa_function_pool[2527]: GetSamplerParameterIiv (will be remapped) */
   "iip\0"
   "glGetSamplerParameterIiv\0"
   "\0"
   /* _mesa_function_pool[2557]: VertexAttribI4uiEXT (will be remapped) */
   "iiiii\0"
   "glVertexAttribI4uiEXT\0"
   "glVertexAttribI4ui\0"
   "\0"
   /* _mesa_function_pool[2605]: DrawElementsInstancedBaseVertexBaseInstance (will be remapped) */
   "iiipiii\0"
   "glDrawElementsInstancedBaseVertexBaseInstance\0"
   "\0"
   /* _mesa_function_pool[2660]: ProgramEnvParameter4dvARB (will be remapped) */
   "iip\0"
   "glProgramEnvParameter4dvARB\0"
   "glProgramParameter4dvNV\0"
   "\0"
   /* _mesa_function_pool[2717]: ColorTableParameteriv (offset 341) */
   "iip\0"
   "glColorTableParameteriv\0"
   "glColorTableParameterivSGI\0"
   "\0"
   /* _mesa_function_pool[2773]: BindSamplers (will be remapped) */
   "iip\0"
   "glBindSamplers\0"
   "\0"
   /* _mesa_function_pool[2793]: GetnCompressedTexImageARB (will be remapped) */
   "iiip\0"
   "glGetnCompressedTexImageARB\0"
   "\0"
   /* _mesa_function_pool[2827]: BindSampler (will be remapped) */
   "ii\0"
   "glBindSampler\0"
   "\0"
   /* _mesa_function_pool[2845]: MultiTexCoord2fARB (offset 386) */
   "iff\0"
   "glMultiTexCoord2f\0"
   "glMultiTexCoord2fARB\0"
   "\0"
   /* _mesa_function_pool[2889]: IndexPointer (offset 314) */
   "iip\0"
   "glIndexPointer\0"
   "\0"
   /* _mesa_function_pool[2909]: MultiTexCoord3iv (offset 397) */
   "ip\0"
   "glMultiTexCoord3iv\0"
   "glMultiTexCoord3ivARB\0"
   "\0"
   /* _mesa_function_pool[2954]: Finish (offset 216) */
   "\0"
   "glFinish\0"
   "\0"
   /* _mesa_function_pool[2965]: ClearStencil (offset 207) */
   "i\0"
   "glClearStencil\0"
   "\0"
   /* _mesa_function_pool[2983]: ClearColorIiEXT (will be remapped) */
   "iiii\0"
   "glClearColorIiEXT\0"
   "\0"
   /* _mesa_function_pool[3007]: LoadMatrixd (offset 292) */
   "p\0"
   "glLoadMatrixd\0"
   "\0"
   /* _mesa_function_pool[3024]: VDPAURegisterOutputSurfaceNV (will be remapped) */
   "piip\0"
   "glVDPAURegisterOutputSurfaceNV\0"
   "\0"
   /* _mesa_function_pool[3061]: VertexP4ui (will be remapped) */
   "ii\0"
   "glVertexP4ui\0"
   "\0"
   /* _mesa_function_pool[3078]: SpriteParameterfvSGIX (dynamic) */
   "ip\0"
   "glSpriteParameterfvSGIX\0"
   "\0"
   /* _mesa_function_pool[3106]: GetActiveUniformName (will be remapped) */
   "iiipp\0"
   "glGetActiveUniformName\0"
   "\0"
   /* _mesa_function_pool[3136]: GetnUniformivARB (will be remapped) */
   "iiip\0"
   "glGetnUniformivARB\0"
   "\0"
   /* _mesa_function_pool[3161]: ReleaseShaderCompiler (will be remapped) */
   "\0"
   "glReleaseShaderCompiler\0"
   "\0"
   /* _mesa_function_pool[3187]: BlendFuncSeparate (will be remapped) */
   "iiii\0"
   "glBlendFuncSeparate\0"
   "glBlendFuncSeparateEXT\0"
   "glBlendFuncSeparateINGR\0"
   "glBlendFuncSeparateOES\0"
   "\0"
   /* _mesa_function_pool[3283]: Color3us (offset 23) */
   "iii\0"
   "glColor3us\0"
   "\0"
   /* _mesa_function_pool[3299]: MultiDrawArraysIndirect (will be remapped) */
   "ipii\0"
   "glMultiDrawArraysIndirect\0"
   "\0"
   /* _mesa_function_pool[3331]: BufferStorage (will be remapped) */
   "iipi\0"
   "glBufferStorage\0"
   "\0"
   /* _mesa_function_pool[3353]: Color3ub (offset 19) */
   "iii\0"
   "glColor3ub\0"
   "\0"
   /* _mesa_function_pool[3369]: GetInstrumentsSGIX (dynamic) */
   "\0"
   "glGetInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[3392]: Color3ui (offset 21) */
   "iii\0"
   "glColor3ui\0"
   "\0"
   /* _mesa_function_pool[3408]: VertexAttrib4dvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib4dvNV\0"
   "\0"
   /* _mesa_function_pool[3432]: AlphaFragmentOp2ATI (will be remapped) */
   "iiiiiiiii\0"
   "glAlphaFragmentOp2ATI\0"
   "\0"
   /* _mesa_function_pool[3465]: RasterPos4dv (offset 79) */
   "p\0"
   "glRasterPos4dv\0"
   "\0"
   /* _mesa_function_pool[3483]: DeleteProgramPipelines (will be remapped) */
   "ip\0"
   "glDeleteProgramPipelines\0"
   "glDeleteProgramPipelinesEXT\0"
   "\0"
   /* _mesa_function_pool[3540]: LineWidthx (will be remapped) */
   "i\0"
   "glLineWidthxOES\0"
   "glLineWidthx\0"
   "\0"
   /* _mesa_function_pool[3572]: Indexdv (offset 45) */
   "p\0"
   "glIndexdv\0"
   "\0"
   /* _mesa_function_pool[3585]: GetnPixelMapfvARB (will be remapped) */
   "iip\0"
   "glGetnPixelMapfvARB\0"
   "\0"
   /* _mesa_function_pool[3610]: EGLImageTargetTexture2DOES (will be remapped) */
   "ip\0"
   "glEGLImageTargetTexture2DOES\0"
   "\0"
   /* _mesa_function_pool[3643]: DepthMask (offset 211) */
   "i\0"
   "glDepthMask\0"
   "\0"
   /* _mesa_function_pool[3658]: WindowPos4ivMESA (will be remapped) */
   "p\0"
   "glWindowPos4ivMESA\0"
   "\0"
   /* _mesa_function_pool[3680]: GetShaderInfoLog (will be remapped) */
   "iipp\0"
   "glGetShaderInfoLog\0"
   "\0"
   /* _mesa_function_pool[3705]: BindFragmentShaderATI (will be remapped) */
   "i\0"
   "glBindFragmentShaderATI\0"
   "\0"
   /* _mesa_function_pool[3732]: BlendFuncSeparateiARB (will be remapped) */
   "iiiii\0"
   "glBlendFuncSeparateiARB\0"
   "glBlendFuncSeparateIndexedAMD\0"
   "glBlendFuncSeparatei\0"
   "\0"
   /* _mesa_function_pool[3814]: PixelTexGenParameteriSGIS (dynamic) */
   "ii\0"
   "glPixelTexGenParameteriSGIS\0"
   "\0"
   /* _mesa_function_pool[3846]: EGLImageTargetRenderbufferStorageOES (will be remapped) */
   "ip\0"
   "glEGLImageTargetRenderbufferStorageOES\0"
   "\0"
   /* _mesa_function_pool[3889]: GenTransformFeedbacks (will be remapped) */
   "ip\0"
   "glGenTransformFeedbacks\0"
   "\0"
   /* _mesa_function_pool[3917]: VertexPointer (offset 321) */
   "iiip\0"
   "glVertexPointer\0"
   "\0"
   /* _mesa_function_pool[3939]: GetCompressedTexImage (will be remapped) */
   "iip\0"
   "glGetCompressedTexImage\0"
   "glGetCompressedTexImageARB\0"
   "\0"
   /* _mesa_function_pool[3995]: ProgramLocalParameter4dvARB (will be remapped) */
   "iip\0"
   "glProgramLocalParameter4dvARB\0"
   "\0"
   /* _mesa_function_pool[4030]: TexCoord4fv (offset 121) */
   "p\0"
   "glTexCoord4fv\0"
   "\0"
   /* _mesa_function_pool[4047]: GetQueryObjectui64v (will be remapped) */
   "iip\0"
   "glGetQueryObjectui64v\0"
   "glGetQueryObjectui64vEXT\0"
   "\0"
   /* _mesa_function_pool[4099]: VertexAttribP1uiv (will be remapped) */
   "iiip\0"
   "glVertexAttribP1uiv\0"
   "\0"
   /* _mesa_function_pool[4125]: IsProgram (will be remapped) */
   "i\0"
   "glIsProgram\0"
   "\0"
   /* _mesa_function_pool[4140]: TexCoordPointerListIBM (dynamic) */
   "iiipi\0"
   "glTexCoordPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[4172]: ResizeBuffersMESA (will be remapped) */
   "\0"
   "glResizeBuffersMESA\0"
   "\0"
   /* _mesa_function_pool[4194]: BindBuffersBase (will be remapped) */
   "iiip\0"
   "glBindBuffersBase\0"
   "\0"
   /* _mesa_function_pool[4218]: GenTextures (offset 328) */
   "ip\0"
   "glGenTextures\0"
   "glGenTexturesEXT\0"
   "\0"
   /* _mesa_function_pool[4253]: IndexPointerListIBM (dynamic) */
   "iipi\0"
   "glIndexPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[4281]: WindowPos4fMESA (will be remapped) */
   "ffff\0"
   "glWindowPos4fMESA\0"
   "\0"
   /* _mesa_function_pool[4305]: VertexAttribs2fvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs2fvNV\0"
   "\0"
   /* _mesa_function_pool[4331]: VertexAttribP4ui (will be remapped) */
   "iiii\0"
   "glVertexAttribP4ui\0"
   "\0"
   /* _mesa_function_pool[4356]: Uniform4i (will be remapped) */
   "iiiii\0"
   "glUniform4i\0"
   "glUniform4iARB\0"
   "\0"
   /* _mesa_function_pool[4390]: BufferParameteriAPPLE (will be remapped) */
   "iii\0"
   "glBufferParameteriAPPLE\0"
   "\0"
   /* _mesa_function_pool[4419]: Uniform4f (will be remapped) */
   "iffff\0"
   "glUniform4f\0"
   "glUniform4fARB\0"
   "\0"
   /* _mesa_function_pool[4453]: ProgramUniform3f (will be remapped) */
   "iifff\0"
   "glProgramUniform3f\0"
   "glProgramUniform3fEXT\0"
   "\0"
   /* _mesa_function_pool[4501]: ProgramUniform3i (will be remapped) */
   "iiiii\0"
   "glProgramUniform3i\0"
   "glProgramUniform3iEXT\0"
   "\0"
   /* _mesa_function_pool[4549]: PointParameterfv (will be remapped) */
   "ip\0"
   "glPointParameterfv\0"
   "glPointParameterfvARB\0"
   "glPointParameterfvEXT\0"
   "glPointParameterfvSGIS\0"
   "\0"
   /* _mesa_function_pool[4639]: GetHistogramParameterfv (offset 362) */
   "iip\0"
   "glGetHistogramParameterfv\0"
   "glGetHistogramParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[4699]: GetString (offset 275) */
   "i\0"
   "glGetString\0"
   "\0"
   /* _mesa_function_pool[4714]: ColorPointervINTEL (dynamic) */
   "iip\0"
   "glColorPointervINTEL\0"
   "\0"
   /* _mesa_function_pool[4740]: VDPAUUnmapSurfacesNV (will be remapped) */
   "ip\0"
   "glVDPAUUnmapSurfacesNV\0"
   "\0"
   /* _mesa_function_pool[4767]: GetnHistogramARB (will be remapped) */
   "iiiiip\0"
   "glGetnHistogramARB\0"
   "\0"
   /* _mesa_function_pool[4794]: ReplacementCodeuiColor4fNormal3fVertex3fvSUN (dynamic) */
   "pppp\0"
   "glReplacementCodeuiColor4fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[4847]: SecondaryColor3s (will be remapped) */
   "iii\0"
   "glSecondaryColor3s\0"
   "glSecondaryColor3sEXT\0"
   "\0"
   /* _mesa_function_pool[4893]: VertexAttribP2uiv (will be remapped) */
   "iiip\0"
   "glVertexAttribP2uiv\0"
   "\0"
   /* _mesa_function_pool[4919]: GetDetailTexFuncSGIS (dynamic) */
   "ip\0"
   "glGetDetailTexFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[4946]: VertexAttrib3fNV (will be remapped) */
   "ifff\0"
   "glVertexAttrib3fNV\0"
   "\0"
   /* _mesa_function_pool[4971]: SecondaryColor3b (will be remapped) */
   "iii\0"
   "glSecondaryColor3b\0"
   "glSecondaryColor3bEXT\0"
   "\0"
   /* _mesa_function_pool[5017]: EnableClientState (offset 313) */
   "i\0"
   "glEnableClientState\0"
   "\0"
   /* _mesa_function_pool[5040]: Color4ubVertex2fvSUN (dynamic) */
   "pp\0"
   "glColor4ubVertex2fvSUN\0"
   "\0"
   /* _mesa_function_pool[5067]: SecondaryColor3i (will be remapped) */
   "iii\0"
   "glSecondaryColor3i\0"
   "glSecondaryColor3iEXT\0"
   "\0"
   /* _mesa_function_pool[5113]: TexFilterFuncSGIS (dynamic) */
   "iiip\0"
   "glTexFilterFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[5139]: GetFragmentMaterialfvSGIX (dynamic) */
   "iip\0"
   "glGetFragmentMaterialfvSGIX\0"
   "\0"
   /* _mesa_function_pool[5172]: DetailTexFuncSGIS (dynamic) */
   "iip\0"
   "glDetailTexFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[5197]: FlushMappedBufferRange (will be remapped) */
   "iii\0"
   "glFlushMappedBufferRange\0"
   "glFlushMappedBufferRangeEXT\0"
   "\0"
   /* _mesa_function_pool[5255]: Lightfv (offset 160) */
   "iip\0"
   "glLightfv\0"
   "\0"
   /* _mesa_function_pool[5270]: GetFramebufferAttachmentParameteriv (will be remapped) */
   "iiip\0"
   "glGetFramebufferAttachmentParameteriv\0"
   "glGetFramebufferAttachmentParameterivEXT\0"
   "glGetFramebufferAttachmentParameterivOES\0"
   "\0"
   /* _mesa_function_pool[5396]: ColorSubTable (offset 346) */
   "iiiiip\0"
   "glColorSubTable\0"
   "glColorSubTableEXT\0"
   "\0"
   /* _mesa_function_pool[5439]: EndPerfMonitorAMD (will be remapped) */
   "i\0"
   "glEndPerfMonitorAMD\0"
   "\0"
   /* _mesa_function_pool[5462]: Uniform1ui (will be remapped) */
   "ii\0"
   "glUniform1uiEXT\0"
   "glUniform1ui\0"
   "\0"
   /* _mesa_function_pool[5495]: MapParameterivNV (dynamic) */
   "iip\0"
   "glMapParameterivNV\0"
   "\0"
   /* _mesa_function_pool[5519]: GetMultisamplefv (will be remapped) */
   "iip\0"
   "glGetMultisamplefv\0"
   "\0"
   /* _mesa_function_pool[5543]: WeightbvARB (dynamic) */
   "ip\0"
   "glWeightbvARB\0"
   "\0"
   /* _mesa_function_pool[5561]: Rectdv (offset 87) */
   "pp\0"
   "glRectdv\0"
   "\0"
   /* _mesa_function_pool[5574]: DrawArraysInstancedARB (will be remapped) */
   "iiii\0"
   "glDrawArraysInstancedARB\0"
   "glDrawArraysInstancedEXT\0"
   "glDrawArraysInstanced\0"
   "\0"
   /* _mesa_function_pool[5652]: ProgramEnvParameters4fvEXT (will be remapped) */
   "iiip\0"
   "glProgramEnvParameters4fvEXT\0"
   "\0"
   /* _mesa_function_pool[5687]: VertexAttrib2svNV (will be remapped) */
   "ip\0"
   "glVertexAttrib2svNV\0"
   "\0"
   /* _mesa_function_pool[5711]: SecondaryColorP3uiv (will be remapped) */
   "ip\0"
   "glSecondaryColorP3uiv\0"
   "\0"
   /* _mesa_function_pool[5737]: GetnPixelMapuivARB (will be remapped) */
   "iip\0"
   "glGetnPixelMapuivARB\0"
   "\0"
   /* _mesa_function_pool[5763]: GetSamplerParameterIuiv (will be remapped) */
   "iip\0"
   "glGetSamplerParameterIuiv\0"
   "\0"
   /* _mesa_function_pool[5794]: Disablei (will be remapped) */
   "ii\0"
   "glDisableIndexedEXT\0"
   "glDisablei\0"
   "\0"
   /* _mesa_function_pool[5829]: CompressedTexSubImage3D (will be remapped) */
   "iiiiiiiiiip\0"
   "glCompressedTexSubImage3D\0"
   "glCompressedTexSubImage3DARB\0"
   "glCompressedTexSubImage3DOES\0"
   "\0"
   /* _mesa_function_pool[5926]: BindFramebufferEXT (will be remapped) */
   "ii\0"
   "glBindFramebufferEXT\0"
   "\0"
   /* _mesa_function_pool[5951]: Color3dv (offset 12) */
   "p\0"
   "glColor3dv\0"
   "\0"
   /* _mesa_function_pool[5965]: BeginQuery (will be remapped) */
   "ii\0"
   "glBeginQuery\0"
   "glBeginQueryARB\0"
   "\0"
   /* _mesa_function_pool[5998]: VertexP3uiv (will be remapped) */
   "ip\0"
   "glVertexP3uiv\0"
   "\0"
   /* _mesa_function_pool[6016]: GetUniformLocation (will be remapped) */
   "ip\0"
   "glGetUniformLocation\0"
   "glGetUniformLocationARB\0"
   "\0"
   /* _mesa_function_pool[6065]: PixelStoref (offset 249) */
   "if\0"
   "glPixelStoref\0"
   "\0"
   /* _mesa_function_pool[6083]: WindowPos2iv (will be remapped) */
   "p\0"
   "glWindowPos2iv\0"
   "glWindowPos2ivARB\0"
   "glWindowPos2ivMESA\0"
   "\0"
   /* _mesa_function_pool[6138]: PixelStorei (offset 250) */
   "ii\0"
   "glPixelStorei\0"
   "\0"
   /* _mesa_function_pool[6156]: VertexAttribs1svNV (will be remapped) */
   "iip\0"
   "glVertexAttribs1svNV\0"
   "\0"
   /* _mesa_function_pool[6182]: RequestResidentProgramsNV (will be remapped) */
   "ip\0"
   "glRequestResidentProgramsNV\0"
   "\0"
   /* _mesa_function_pool[6214]: ListParameterivSGIX (dynamic) */
   "iip\0"
   "glListParameterivSGIX\0"
   "\0"
   /* _mesa_function_pool[6241]: TexCoord2fColor4fNormal3fVertex3fvSUN (dynamic) */
   "pppp\0"
   "glTexCoord2fColor4fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[6287]: CheckFramebufferStatus (will be remapped) */
   "i\0"
   "glCheckFramebufferStatus\0"
   "glCheckFramebufferStatusEXT\0"
   "glCheckFramebufferStatusOES\0"
   "\0"
   /* _mesa_function_pool[6371]: DispatchComputeIndirect (will be remapped) */
   "i\0"
   "glDispatchComputeIndirect\0"
   "\0"
   /* _mesa_function_pool[6400]: InvalidateBufferData (will be remapped) */
   "i\0"
   "glInvalidateBufferData\0"
   "\0"
   /* _mesa_function_pool[6426]: VDPAUMapSurfacesNV (will be remapped) */
   "ip\0"
   "glVDPAUMapSurfacesNV\0"
   "\0"
   /* _mesa_function_pool[6451]: IsFramebuffer (will be remapped) */
   "i\0"
   "glIsFramebuffer\0"
   "glIsFramebufferEXT\0"
   "glIsFramebufferOES\0"
   "\0"
   /* _mesa_function_pool[6508]: GetPixelTexGenParameterfvSGIS (dynamic) */
   "ip\0"
   "glGetPixelTexGenParameterfvSGIS\0"
   "\0"
   /* _mesa_function_pool[6544]: GetDoublev (offset 260) */
   "ip\0"
   "glGetDoublev\0"
   "\0"
   /* _mesa_function_pool[6561]: GetObjectLabel (will be remapped) */
   "iiipp\0"
   "glGetObjectLabel\0"
   "\0"
   /* _mesa_function_pool[6585]: TextureLightEXT (dynamic) */
   "i\0"
   "glTextureLightEXT\0"
   "\0"
   /* _mesa_function_pool[6606]: ColorP3uiv (will be remapped) */
   "ip\0"
   "glColorP3uiv\0"
   "\0"
   /* _mesa_function_pool[6623]: CombinerParameteriNV (dynamic) */
   "ii\0"
   "glCombinerParameteriNV\0"
   "\0"
   /* _mesa_function_pool[6650]: Normal3fVertex3fvSUN (dynamic) */
   "pp\0"
   "glNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[6677]: VertexAttribI4ivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI4ivEXT\0"
   "glVertexAttribI4iv\0"
   "\0"
   /* _mesa_function_pool[6722]: SecondaryColor3ubv (will be remapped) */
   "p\0"
   "glSecondaryColor3ubv\0"
   "glSecondaryColor3ubvEXT\0"
   "\0"
   /* _mesa_function_pool[6770]: GetDebugMessageLog (will be remapped) */
   "iipppppp\0"
   "glGetDebugMessageLogARB\0"
   "glGetDebugMessageLog\0"
   "\0"
   /* _mesa_function_pool[6825]: DeformationMap3fSGIX (dynamic) */
   "iffiiffiiffiip\0"
   "glDeformationMap3fSGIX\0"
   "\0"
   /* _mesa_function_pool[6864]: MatrixIndexubvARB (dynamic) */
   "ip\0"
   "glMatrixIndexubvARB\0"
   "\0"
   /* _mesa_function_pool[6888]: VertexAttribI4usv (will be remapped) */
   "ip\0"
   "glVertexAttribI4usvEXT\0"
   "glVertexAttribI4usv\0"
   "\0"
   /* _mesa_function_pool[6935]: PixelTexGenParameterfSGIS (dynamic) */
   "if\0"
   "glPixelTexGenParameterfSGIS\0"
   "\0"
   /* _mesa_function_pool[6967]: ProgramUniform2ui (will be remapped) */
   "iiii\0"
   "glProgramUniform2ui\0"
   "glProgramUniform2uiEXT\0"
   "\0"
   /* _mesa_function_pool[7016]: TexCoord2fVertex3fvSUN (dynamic) */
   "pp\0"
   "glTexCoord2fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[7045]: Color4ubVertex3fvSUN (dynamic) */
   "pp\0"
   "glColor4ubVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[7072]: GetShaderSource (will be remapped) */
   "iipp\0"
   "glGetShaderSource\0"
   "glGetShaderSourceARB\0"
   "\0"
   /* _mesa_function_pool[7117]: BindProgramARB (will be remapped) */
   "ii\0"
   "glBindProgramARB\0"
   "glBindProgramNV\0"
   "\0"
   /* _mesa_function_pool[7154]: VertexAttrib3sNV (will be remapped) */
   "iiii\0"
   "glVertexAttrib3sNV\0"
   "\0"
   /* _mesa_function_pool[7179]: ColorFragmentOp1ATI (will be remapped) */
   "iiiiiii\0"
   "glColorFragmentOp1ATI\0"
   "\0"
   /* _mesa_function_pool[7210]: ProgramUniformMatrix4x3fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix4x3fv\0"
   "glProgramUniformMatrix4x3fvEXT\0"
   "\0"
   /* _mesa_function_pool[7276]: PopClientAttrib (offset 334) */
   "\0"
   "glPopClientAttrib\0"
   "\0"
   /* _mesa_function_pool[7296]: DrawElementsInstancedARB (will be remapped) */
   "iiipi\0"
   "glDrawElementsInstancedARB\0"
   "glDrawElementsInstancedEXT\0"
   "glDrawElementsInstanced\0"
   "\0"
   /* _mesa_function_pool[7381]: GetQueryObjectuiv (will be remapped) */
   "iip\0"
   "glGetQueryObjectuiv\0"
   "glGetQueryObjectuivARB\0"
   "\0"
   /* _mesa_function_pool[7429]: VertexAttribI4bv (will be remapped) */
   "ip\0"
   "glVertexAttribI4bvEXT\0"
   "glVertexAttribI4bv\0"
   "\0"
   /* _mesa_function_pool[7474]: FogCoordPointerListIBM (dynamic) */
   "iipi\0"
   "glFogCoordPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[7505]: Binormal3sEXT (dynamic) */
   "iii\0"
   "glBinormal3sEXT\0"
   "\0"
   /* _mesa_function_pool[7526]: ListBase (offset 6) */
   "i\0"
   "glListBase\0"
   "\0"
   /* _mesa_function_pool[7540]: GenerateMipmap (will be remapped) */
   "i\0"
   "glGenerateMipmap\0"
   "glGenerateMipmapEXT\0"
   "glGenerateMipmapOES\0"
   "\0"
   /* _mesa_function_pool[7600]: BindBufferRange (will be remapped) */
   "iiiii\0"
   "glBindBufferRange\0"
   "glBindBufferRangeEXT\0"
   "\0"
   /* _mesa_function_pool[7646]: ProgramUniformMatrix2x4fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix2x4fv\0"
   "glProgramUniformMatrix2x4fvEXT\0"
   "\0"
   /* _mesa_function_pool[7712]: GetQueryObjectiv (will be remapped) */
   "iip\0"
   "glGetQueryObjectiv\0"
   "glGetQueryObjectivARB\0"
   "\0"
   /* _mesa_function_pool[7758]: VertexAttrib2s (will be remapped) */
   "iii\0"
   "glVertexAttrib2s\0"
   "glVertexAttrib2sARB\0"
   "\0"
   /* _mesa_function_pool[7800]: SecondaryColor3fvEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3fv\0"
   "glSecondaryColor3fvEXT\0"
   "\0"
   /* _mesa_function_pool[7846]: VertexAttrib2d (will be remapped) */
   "idd\0"
   "glVertexAttrib2d\0"
   "glVertexAttrib2dARB\0"
   "\0"
   /* _mesa_function_pool[7888]: Uniform1fv (will be remapped) */
   "iip\0"
   "glUniform1fv\0"
   "glUniform1fvARB\0"
   "\0"
   /* _mesa_function_pool[7922]: GetProgramPipelineInfoLog (will be remapped) */
   "iipp\0"
   "glGetProgramPipelineInfoLog\0"
   "glGetProgramPipelineInfoLogEXT\0"
   "\0"
   /* _mesa_function_pool[7987]: TextureMaterialEXT (dynamic) */
   "ii\0"
   "glTextureMaterialEXT\0"
   "\0"
   /* _mesa_function_pool[8012]: DepthBoundsEXT (will be remapped) */
   "dd\0"
   "glDepthBoundsEXT\0"
   "\0"
   /* _mesa_function_pool[8033]: WindowPos3fv (will be remapped) */
   "p\0"
   "glWindowPos3fv\0"
   "glWindowPos3fvARB\0"
   "glWindowPos3fvMESA\0"
   "\0"
   /* _mesa_function_pool[8088]: BindVertexArrayAPPLE (will be remapped) */
   "i\0"
   "glBindVertexArrayAPPLE\0"
   "\0"
   /* _mesa_function_pool[8114]: GetHistogramParameteriv (offset 363) */
   "iip\0"
   "glGetHistogramParameteriv\0"
   "glGetHistogramParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[8174]: PointParameteriv (will be remapped) */
   "ip\0"
   "glPointParameteriv\0"
   "glPointParameterivNV\0"
   "\0"
   /* _mesa_function_pool[8218]: GetProgramivARB (will be remapped) */
   "iip\0"
   "glGetProgramivARB\0"
   "\0"
   /* _mesa_function_pool[8241]: BindRenderbuffer (will be remapped) */
   "ii\0"
   "glBindRenderbuffer\0"
   "glBindRenderbufferOES\0"
   "\0"
   /* _mesa_function_pool[8286]: SecondaryColor3fEXT (will be remapped) */
   "fff\0"
   "glSecondaryColor3f\0"
   "glSecondaryColor3fEXT\0"
   "\0"
   /* _mesa_function_pool[8332]: PrimitiveRestartIndex (will be remapped) */
   "i\0"
   "glPrimitiveRestartIndex\0"
   "glPrimitiveRestartIndexNV\0"
   "\0"
   /* _mesa_function_pool[8385]: VertexAttribI4ubv (will be remapped) */
   "ip\0"
   "glVertexAttribI4ubvEXT\0"
   "glVertexAttribI4ubv\0"
   "\0"
   /* _mesa_function_pool[8432]: GetGraphicsResetStatusARB (will be remapped) */
   "\0"
   "glGetGraphicsResetStatusARB\0"
   "\0"
   /* _mesa_function_pool[8462]: ActiveStencilFaceEXT (will be remapped) */
   "i\0"
   "glActiveStencilFaceEXT\0"
   "\0"
   /* _mesa_function_pool[8488]: VertexAttrib4dNV (will be remapped) */
   "idddd\0"
   "glVertexAttrib4dNV\0"
   "\0"
   /* _mesa_function_pool[8514]: DepthRange (offset 288) */
   "dd\0"
   "glDepthRange\0"
   "\0"
   /* _mesa_function_pool[8531]: TexBumpParameterivATI (will be remapped) */
   "ip\0"
   "glTexBumpParameterivATI\0"
   "\0"
   /* _mesa_function_pool[8559]: VertexAttrib4fNV (will be remapped) */
   "iffff\0"
   "glVertexAttrib4fNV\0"
   "\0"
   /* _mesa_function_pool[8585]: Uniform4fv (will be remapped) */
   "iip\0"
   "glUniform4fv\0"
   "glUniform4fvARB\0"
   "\0"
   /* _mesa_function_pool[8619]: DrawMeshArraysSUN (dynamic) */
   "iiii\0"
   "glDrawMeshArraysSUN\0"
   "\0"
   /* _mesa_function_pool[8645]: SamplerParameterIiv (will be remapped) */
   "iip\0"
   "glSamplerParameterIiv\0"
   "\0"
   /* _mesa_function_pool[8672]: GetMapControlPointsNV (dynamic) */
   "iiiiiip\0"
   "glGetMapControlPointsNV\0"
   "\0"
   /* _mesa_function_pool[8705]: SpriteParameterivSGIX (dynamic) */
   "ip\0"
   "glSpriteParameterivSGIX\0"
   "\0"
   /* _mesa_function_pool[8733]: Frustumf (will be remapped) */
   "ffffff\0"
   "glFrustumfOES\0"
   "glFrustumf\0"
   "\0"
   /* _mesa_function_pool[8766]: ProgramUniform2uiv (will be remapped) */
   "iiip\0"
   "glProgramUniform2uiv\0"
   "glProgramUniform2uivEXT\0"
   "\0"
   /* _mesa_function_pool[8817]: Rectsv (offset 93) */
   "pp\0"
   "glRectsv\0"
   "\0"
   /* _mesa_function_pool[8830]: Frustumx (will be remapped) */
   "iiiiii\0"
   "glFrustumxOES\0"
   "glFrustumx\0"
   "\0"
   /* _mesa_function_pool[8863]: CullFace (offset 152) */
   "i\0"
   "glCullFace\0"
   "\0"
   /* _mesa_function_pool[8877]: BindTexture (offset 307) */
   "ii\0"
   "glBindTexture\0"
   "glBindTextureEXT\0"
   "\0"
   /* _mesa_function_pool[8912]: MultiTexCoord4fARB (offset 402) */
   "iffff\0"
   "glMultiTexCoord4f\0"
   "glMultiTexCoord4fARB\0"
   "\0"
   /* _mesa_function_pool[8958]: MultiTexCoordP2uiv (will be remapped) */
   "iip\0"
   "glMultiTexCoordP2uiv\0"
   "\0"
   /* _mesa_function_pool[8984]: NormalPointervINTEL (dynamic) */
   "ip\0"
   "glNormalPointervINTEL\0"
   "\0"
   /* _mesa_function_pool[9010]: NormalPointer (offset 318) */
   "iip\0"
   "glNormalPointer\0"
   "\0"
   /* _mesa_function_pool[9031]: TangentPointerEXT (dynamic) */
   "iip\0"
   "glTangentPointerEXT\0"
   "\0"
   /* _mesa_function_pool[9056]: WindowPos4iMESA (will be remapped) */
   "iiii\0"
   "glWindowPos4iMESA\0"
   "\0"
   /* _mesa_function_pool[9080]: ReferencePlaneSGIX (dynamic) */
   "p\0"
   "glReferencePlaneSGIX\0"
   "\0"
   /* _mesa_function_pool[9104]: VertexAttrib4bv (will be remapped) */
   "ip\0"
   "glVertexAttrib4bv\0"
   "glVertexAttrib4bvARB\0"
   "\0"
   /* _mesa_function_pool[9147]: ReplacementCodeuivSUN (dynamic) */
   "p\0"
   "glReplacementCodeuivSUN\0"
   "\0"
   /* _mesa_function_pool[9174]: SecondaryColor3usv (will be remapped) */
   "p\0"
   "glSecondaryColor3usv\0"
   "glSecondaryColor3usvEXT\0"
   "\0"
   /* _mesa_function_pool[9222]: GetPixelMapuiv (offset 272) */
   "ip\0"
   "glGetPixelMapuiv\0"
   "\0"
   /* _mesa_function_pool[9243]: Indexfv (offset 47) */
   "p\0"
   "glIndexfv\0"
   "\0"
   /* _mesa_function_pool[9256]: AlphaFragmentOp1ATI (will be remapped) */
   "iiiiii\0"
   "glAlphaFragmentOp1ATI\0"
   "\0"
   /* _mesa_function_pool[9286]: ListParameteriSGIX (dynamic) */
   "iii\0"
   "glListParameteriSGIX\0"
   "\0"
   /* _mesa_function_pool[9312]: GetFloatv (offset 262) */
   "ip\0"
   "glGetFloatv\0"
   "\0"
   /* _mesa_function_pool[9328]: MultiTexCoord3i (offset 396) */
   "iiii\0"
   "glMultiTexCoord3i\0"
   "glMultiTexCoord3iARB\0"
   "\0"
   /* _mesa_function_pool[9373]: ProgramUniform1fv (will be remapped) */
   "iiip\0"
   "glProgramUniform1fv\0"
   "glProgramUniform1fvEXT\0"
   "\0"
   /* _mesa_function_pool[9422]: MultiTexCoord3d (offset 392) */
   "iddd\0"
   "glMultiTexCoord3d\0"
   "glMultiTexCoord3dARB\0"
   "\0"
   /* _mesa_function_pool[9467]: TexCoord3sv (offset 117) */
   "p\0"
   "glTexCoord3sv\0"
   "\0"
   /* _mesa_function_pool[9484]: Fogfv (offset 154) */
   "ip\0"
   "glFogfv\0"
   "\0"
   /* _mesa_function_pool[9496]: Minmax (offset 368) */
   "iii\0"
   "glMinmax\0"
   "glMinmaxEXT\0"
   "\0"
   /* _mesa_function_pool[9522]: MultiTexCoord3s (offset 398) */
   "iiii\0"
   "glMultiTexCoord3s\0"
   "glMultiTexCoord3sARB\0"
   "\0"
   /* _mesa_function_pool[9567]: FinishTextureSUNX (dynamic) */
   "\0"
   "glFinishTextureSUNX\0"
   "\0"
   /* _mesa_function_pool[9589]: GetFinalCombinerInputParameterfvNV (dynamic) */
   "iip\0"
   "glGetFinalCombinerInputParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[9631]: PollInstrumentsSGIX (dynamic) */
   "p\0"
   "glPollInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[9656]: Vertex4iv (offset 147) */
   "p\0"
   "glVertex4iv\0"
   "\0"
   /* _mesa_function_pool[9671]: BufferSubData (will be remapped) */
   "iiip\0"
   "glBufferSubData\0"
   "glBufferSubDataARB\0"
   "\0"
   /* _mesa_function_pool[9712]: AlphaFragmentOp3ATI (will be remapped) */
   "iiiiiiiiiiii\0"
   "glAlphaFragmentOp3ATI\0"
   "\0"
   /* _mesa_function_pool[9748]: Begin (offset 7) */
   "i\0"
   "glBegin\0"
   "\0"
   /* _mesa_function_pool[9759]: LightModeli (offset 165) */
   "ii\0"
   "glLightModeli\0"
   "\0"
   /* _mesa_function_pool[9777]: UniformMatrix2fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix2fv\0"
   "glUniformMatrix2fvARB\0"
   "\0"
   /* _mesa_function_pool[9824]: LightModelf (offset 163) */
   "if\0"
   "glLightModelf\0"
   "\0"
   /* _mesa_function_pool[9842]: GetTexParameterfv (offset 282) */
   "iip\0"
   "glGetTexParameterfv\0"
   "\0"
   /* _mesa_function_pool[9867]: BinormalPointerEXT (dynamic) */
   "iip\0"
   "glBinormalPointerEXT\0"
   "\0"
   /* _mesa_function_pool[9893]: LightModelx (will be remapped) */
   "ii\0"
   "glLightModelxOES\0"
   "glLightModelx\0"
   "\0"
   /* _mesa_function_pool[9928]: DeleteAsyncMarkersSGIX (dynamic) */
   "ii\0"
   "glDeleteAsyncMarkersSGIX\0"
   "\0"
   /* _mesa_function_pool[9957]: MultiTexCoord2fvARB (offset 387) */
   "ip\0"
   "glMultiTexCoord2fv\0"
   "glMultiTexCoord2fvARB\0"
   "\0"
   /* _mesa_function_pool[10002]: VertexAttrib4ubv (will be remapped) */
   "ip\0"
   "glVertexAttrib4ubv\0"
   "glVertexAttrib4ubvARB\0"
   "\0"
   /* _mesa_function_pool[10047]: GetnTexImageARB (will be remapped) */
   "iiiiip\0"
   "glGetnTexImageARB\0"
   "\0"
   /* _mesa_function_pool[10073]: ColorMask (offset 210) */
   "iiii\0"
   "glColorMask\0"
   "\0"
   /* _mesa_function_pool[10091]: GenAsyncMarkersSGIX (dynamic) */
   "i\0"
   "glGenAsyncMarkersSGIX\0"
   "\0"
   /* _mesa_function_pool[10116]: ReplacementCodeuiVertex3fSUN (dynamic) */
   "ifff\0"
   "glReplacementCodeuiVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[10153]: VertexAttribI4sv (will be remapped) */
   "ip\0"
   "glVertexAttribI4svEXT\0"
   "glVertexAttribI4sv\0"
   "\0"
   /* _mesa_function_pool[10198]: DrawElementsInstancedBaseInstance (will be remapped) */
   "iiipii\0"
   "glDrawElementsInstancedBaseInstance\0"
   "\0"
   /* _mesa_function_pool[10242]: UniformMatrix4fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix4fv\0"
   "glUniformMatrix4fvARB\0"
   "\0"
   /* _mesa_function_pool[10289]: UniformMatrix3x2fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix3x2fv\0"
   "\0"
   /* _mesa_function_pool[10316]: VertexAttrib4Nuiv (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nuiv\0"
   "glVertexAttrib4NuivARB\0"
   "\0"
   /* _mesa_function_pool[10363]: ClientActiveTexture (offset 375) */
   "i\0"
   "glClientActiveTexture\0"
   "glClientActiveTextureARB\0"
   "\0"
   /* _mesa_function_pool[10413]: GetUniformIndices (will be remapped) */
   "iipp\0"
   "glGetUniformIndices\0"
   "\0"
   /* _mesa_function_pool[10439]: GetTexBumpParameterivATI (will be remapped) */
   "ip\0"
   "glGetTexBumpParameterivATI\0"
   "\0"
   /* _mesa_function_pool[10470]: Binormal3bEXT (dynamic) */
   "iii\0"
   "glBinormal3bEXT\0"
   "\0"
   /* _mesa_function_pool[10491]: CombinerParameterivNV (dynamic) */
   "ip\0"
   "glCombinerParameterivNV\0"
   "\0"
   /* _mesa_function_pool[10519]: MultiTexCoord2sv (offset 391) */
   "ip\0"
   "glMultiTexCoord2sv\0"
   "glMultiTexCoord2svARB\0"
   "\0"
   /* _mesa_function_pool[10564]: LoadIdentity (offset 290) */
   "\0"
   "glLoadIdentity\0"
   "\0"
   /* _mesa_function_pool[10581]: ActiveShaderProgram (will be remapped) */
   "ii\0"
   "glActiveShaderProgram\0"
   "glActiveShaderProgramEXT\0"
   "\0"
   /* _mesa_function_pool[10632]: BindImageTextures (will be remapped) */
   "iip\0"
   "glBindImageTextures\0"
   "\0"
   /* _mesa_function_pool[10657]: DeleteTransformFeedbacks (will be remapped) */
   "ip\0"
   "glDeleteTransformFeedbacks\0"
   "\0"
   /* _mesa_function_pool[10688]: VertexAttrib4ubvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib4ubvNV\0"
   "\0"
   /* _mesa_function_pool[10713]: FogCoordfEXT (will be remapped) */
   "f\0"
   "glFogCoordf\0"
   "glFogCoordfEXT\0"
   "\0"
   /* _mesa_function_pool[10743]: GetMapfv (offset 267) */
   "iip\0"
   "glGetMapfv\0"
   "\0"
   /* _mesa_function_pool[10759]: GetProgramInfoLog (will be remapped) */
   "iipp\0"
   "glGetProgramInfoLog\0"
   "\0"
   /* _mesa_function_pool[10785]: BindTransformFeedback (will be remapped) */
   "ii\0"
   "glBindTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[10813]: TexCoord4fColor4fNormal3fVertex4fvSUN (dynamic) */
   "pppp\0"
   "glTexCoord4fColor4fNormal3fVertex4fvSUN\0"
   "\0"
   /* _mesa_function_pool[10859]: GetPixelMapfv (offset 271) */
   "ip\0"
   "glGetPixelMapfv\0"
   "\0"
   /* _mesa_function_pool[10879]: WeightivARB (dynamic) */
   "ip\0"
   "glWeightivARB\0"
   "\0"
   /* _mesa_function_pool[10897]: VertexAttrib4svNV (will be remapped) */
   "ip\0"
   "glVertexAttrib4svNV\0"
   "\0"
   /* _mesa_function_pool[10921]: ReplacementCodeuiTexCoord2fVertex3fSUN (dynamic) */
   "ifffff\0"
   "glReplacementCodeuiTexCoord2fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[10970]: VDPAUSurfaceAccessNV (will be remapped) */
   "ii\0"
   "glVDPAUSurfaceAccessNV\0"
   "\0"
   /* _mesa_function_pool[10997]: EdgeFlagPointer (offset 312) */
   "ip\0"
   "glEdgeFlagPointer\0"
   "\0"
   /* _mesa_function_pool[11019]: WindowPos2f (will be remapped) */
   "ff\0"
   "glWindowPos2f\0"
   "glWindowPos2fARB\0"
   "glWindowPos2fMESA\0"
   "\0"
   /* _mesa_function_pool[11072]: WindowPos2d (will be remapped) */
   "dd\0"
   "glWindowPos2d\0"
   "glWindowPos2dARB\0"
   "glWindowPos2dMESA\0"
   "\0"
   /* _mesa_function_pool[11125]: WindowPos2i (will be remapped) */
   "ii\0"
   "glWindowPos2i\0"
   "glWindowPos2iARB\0"
   "glWindowPos2iMESA\0"
   "\0"
   /* _mesa_function_pool[11178]: WindowPos2s (will be remapped) */
   "ii\0"
   "glWindowPos2s\0"
   "glWindowPos2sARB\0"
   "glWindowPos2sMESA\0"
   "\0"
   /* _mesa_function_pool[11231]: VertexAttribI1uiEXT (will be remapped) */
   "ii\0"
   "glVertexAttribI1uiEXT\0"
   "glVertexAttribI1ui\0"
   "\0"
   /* _mesa_function_pool[11276]: DeleteSync (will be remapped) */
   "i\0"
   "glDeleteSync\0"
   "\0"
   /* _mesa_function_pool[11292]: WindowPos4fvMESA (will be remapped) */
   "p\0"
   "glWindowPos4fvMESA\0"
   "\0"
   /* _mesa_function_pool[11314]: CompressedTexImage3D (will be remapped) */
   "iiiiiiiip\0"
   "glCompressedTexImage3D\0"
   "glCompressedTexImage3DARB\0"
   "glCompressedTexImage3DOES\0"
   "\0"
   /* _mesa_function_pool[11400]: VertexAttribI1uiv (will be remapped) */
   "ip\0"
   "glVertexAttribI1uivEXT\0"
   "glVertexAttribI1uiv\0"
   "\0"
   /* _mesa_function_pool[11447]: SecondaryColor3dv (will be remapped) */
   "p\0"
   "glSecondaryColor3dv\0"
   "glSecondaryColor3dvEXT\0"
   "\0"
   /* _mesa_function_pool[11493]: GetListParameterivSGIX (dynamic) */
   "iip\0"
   "glGetListParameterivSGIX\0"
   "\0"
   /* _mesa_function_pool[11523]: GetnPixelMapusvARB (will be remapped) */
   "iip\0"
   "glGetnPixelMapusvARB\0"
   "\0"
   /* _mesa_function_pool[11549]: VertexAttrib3s (will be remapped) */
   "iiii\0"
   "glVertexAttrib3s\0"
   "glVertexAttrib3sARB\0"
   "\0"
   /* _mesa_function_pool[11592]: UniformMatrix4x3fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix4x3fv\0"
   "\0"
   /* _mesa_function_pool[11619]: Binormal3dEXT (dynamic) */
   "ddd\0"
   "glBinormal3dEXT\0"
   "\0"
   /* _mesa_function_pool[11640]: GetQueryiv (will be remapped) */
   "iip\0"
   "glGetQueryiv\0"
   "glGetQueryivARB\0"
   "\0"
   /* _mesa_function_pool[11674]: VertexAttrib3d (will be remapped) */
   "iddd\0"
   "glVertexAttrib3d\0"
   "glVertexAttrib3dARB\0"
   "\0"
   /* _mesa_function_pool[11717]: ImageTransformParameterfHP (dynamic) */
   "iif\0"
   "glImageTransformParameterfHP\0"
   "\0"
   /* _mesa_function_pool[11751]: MapBuffer (will be remapped) */
   "ii\0"
   "glMapBuffer\0"
   "glMapBufferARB\0"
   "glMapBufferOES\0"
   "\0"
   /* _mesa_function_pool[11797]: VertexAttrib4Nbv (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nbv\0"
   "glVertexAttrib4NbvARB\0"
   "\0"
   /* _mesa_function_pool[11842]: ProgramBinary (will be remapped) */
   "iipi\0"
   "glProgramBinary\0"
   "glProgramBinaryOES\0"
   "\0"
   /* _mesa_function_pool[11883]: InvalidateTexImage (will be remapped) */
   "ii\0"
   "glInvalidateTexImage\0"
   "\0"
   /* _mesa_function_pool[11908]: Uniform4ui (will be remapped) */
   "iiiii\0"
   "glUniform4uiEXT\0"
   "glUniform4ui\0"
   "\0"
   /* _mesa_function_pool[11944]: VertexAttrib1fARB (will be remapped) */
   "if\0"
   "glVertexAttrib1f\0"
   "glVertexAttrib1fARB\0"
   "\0"
   /* _mesa_function_pool[11985]: GetBooleani_v (will be remapped) */
   "iip\0"
   "glGetBooleanIndexedvEXT\0"
   "glGetBooleani_v\0"
   "\0"
   /* _mesa_function_pool[12030]: DrawTexsOES (will be remapped) */
   "iiiii\0"
   "glDrawTexsOES\0"
   "\0"
   /* _mesa_function_pool[12051]: GetObjectPtrLabel (will be remapped) */
   "pipp\0"
   "glGetObjectPtrLabel\0"
   "\0"
   /* _mesa_function_pool[12077]: ProgramParameteri (will be remapped) */
   "iii\0"
   "glProgramParameteriARB\0"
   "glProgramParameteri\0"
   "glProgramParameteriEXT\0"
   "\0"
   /* _mesa_function_pool[12148]: SecondaryColorPointerListIBM (dynamic) */
   "iiipi\0"
   "glSecondaryColorPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[12186]: Color3fv (offset 14) */
   "p\0"
   "glColor3fv\0"
   "\0"
   /* _mesa_function_pool[12200]: ReplacementCodeubSUN (dynamic) */
   "i\0"
   "glReplacementCodeubSUN\0"
   "\0"
   /* _mesa_function_pool[12226]: GetnMapfvARB (will be remapped) */
   "iiip\0"
   "glGetnMapfvARB\0"
   "\0"
   /* _mesa_function_pool[12247]: MultiTexCoord2i (offset 388) */
   "iii\0"
   "glMultiTexCoord2i\0"
   "glMultiTexCoord2iARB\0"
   "\0"
   /* _mesa_function_pool[12291]: MultiTexCoord2d (offset 384) */
   "idd\0"
   "glMultiTexCoord2d\0"
   "glMultiTexCoord2dARB\0"
   "\0"
   /* _mesa_function_pool[12335]: SamplerParameterIuiv (will be remapped) */
   "iip\0"
   "glSamplerParameterIuiv\0"
   "\0"
   /* _mesa_function_pool[12363]: MultiTexCoord2s (offset 390) */
   "iii\0"
   "glMultiTexCoord2s\0"
   "glMultiTexCoord2sARB\0"
   "\0"
   /* _mesa_function_pool[12407]: VDPAURegisterVideoSurfaceNV (will be remapped) */
   "piip\0"
   "glVDPAURegisterVideoSurfaceNV\0"
   "\0"
   /* _mesa_function_pool[12443]: TexCoord2fColor4fNormal3fVertex3fSUN (dynamic) */
   "ffffffffffff\0"
   "glTexCoord2fColor4fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[12496]: BindBufferBase (will be remapped) */
   "iii\0"
   "glBindBufferBase\0"
   "glBindBufferBaseEXT\0"
   "\0"
   /* _mesa_function_pool[12538]: GetPerfMonitorCounterDataAMD (will be remapped) */
   "iiipp\0"
   "glGetPerfMonitorCounterDataAMD\0"
   "\0"
   /* _mesa_function_pool[12576]: ReplacementCodeuiNormal3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glReplacementCodeuiNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[12621]: MultTransposeMatrixf (will be remapped) */
   "p\0"
   "glMultTransposeMatrixf\0"
   "glMultTransposeMatrixfARB\0"
   "\0"
   /* _mesa_function_pool[12673]: PolygonOffsetEXT (will be remapped) */
   "ff\0"
   "glPolygonOffsetEXT\0"
   "\0"
   /* _mesa_function_pool[12696]: Scalex (will be remapped) */
   "iii\0"
   "glScalexOES\0"
   "glScalex\0"
   "\0"
   /* _mesa_function_pool[12722]: Scaled (offset 301) */
   "ddd\0"
   "glScaled\0"
   "\0"
   /* _mesa_function_pool[12736]: Scalef (offset 302) */
   "fff\0"
   "glScalef\0"
   "\0"
   /* _mesa_function_pool[12750]: IndexPointerEXT (will be remapped) */
   "iiip\0"
   "glIndexPointerEXT\0"
   "\0"
   /* _mesa_function_pool[12774]: GetUniformfv (will be remapped) */
   "iip\0"
   "glGetUniformfv\0"
   "glGetUniformfvARB\0"
   "\0"
   /* _mesa_function_pool[12812]: ColorFragmentOp2ATI (will be remapped) */
   "iiiiiiiiii\0"
   "glColorFragmentOp2ATI\0"
   "\0"
   /* _mesa_function_pool[12846]: VertexAttrib2sNV (will be remapped) */
   "iii\0"
   "glVertexAttrib2sNV\0"
   "\0"
   /* _mesa_function_pool[12870]: ReadPixels (offset 256) */
   "iiiiiip\0"
   "glReadPixels\0"
   "\0"
   /* _mesa_function_pool[12892]: NormalPointerListIBM (dynamic) */
   "iipi\0"
   "glNormalPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[12921]: QueryCounter (will be remapped) */
   "ii\0"
   "glQueryCounter\0"
   "\0"
   /* _mesa_function_pool[12940]: NormalPointerEXT (will be remapped) */
   "iiip\0"
   "glNormalPointerEXT\0"
   "\0"
   /* _mesa_function_pool[12965]: ProgramUniform3iv (will be remapped) */
   "iiip\0"
   "glProgramUniform3iv\0"
   "glProgramUniform3ivEXT\0"
   "\0"
   /* _mesa_function_pool[13014]: ClearTexSubImage (will be remapped) */
   "iiiiiiiiiip\0"
   "glClearTexSubImage\0"
   "\0"
   /* _mesa_function_pool[13046]: GetActiveUniformBlockName (will be remapped) */
   "iiipp\0"
   "glGetActiveUniformBlockName\0"
   "\0"
   /* _mesa_function_pool[13081]: DrawElementsBaseVertex (will be remapped) */
   "iiipi\0"
   "glDrawElementsBaseVertex\0"
   "\0"
   /* _mesa_function_pool[13113]: RasterPos3iv (offset 75) */
   "p\0"
   "glRasterPos3iv\0"
   "\0"
   /* _mesa_function_pool[13131]: ColorMaski (will be remapped) */
   "iiiii\0"
   "glColorMaskIndexedEXT\0"
   "glColorMaski\0"
   "\0"
   /* _mesa_function_pool[13173]: Uniform2uiv (will be remapped) */
   "iip\0"
   "glUniform2uivEXT\0"
   "glUniform2uiv\0"
   "\0"
   /* _mesa_function_pool[13209]: RasterPos3s (offset 76) */
   "iii\0"
   "glRasterPos3s\0"
   "\0"
   /* _mesa_function_pool[13228]: RasterPos3d (offset 70) */
   "ddd\0"
   "glRasterPos3d\0"
   "\0"
   /* _mesa_function_pool[13247]: RasterPos3f (offset 72) */
   "fff\0"
   "glRasterPos3f\0"
   "\0"
   /* _mesa_function_pool[13266]: BindVertexArray (will be remapped) */
   "i\0"
   "glBindVertexArray\0"
   "glBindVertexArrayOES\0"
   "\0"
   /* _mesa_function_pool[13308]: RasterPos3i (offset 74) */
   "iii\0"
   "glRasterPos3i\0"
   "\0"
   /* _mesa_function_pool[13327]: GetTexParameterIuiv (will be remapped) */
   "iip\0"
   "glGetTexParameterIuivEXT\0"
   "glGetTexParameterIuiv\0"
   "\0"
   /* _mesa_function_pool[13379]: DrawTransformFeedbackStreamInstanced (will be remapped) */
   "iiii\0"
   "glDrawTransformFeedbackStreamInstanced\0"
   "\0"
   /* _mesa_function_pool[13424]: VertexAttrib2fvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib2fv\0"
   "glVertexAttrib2fvARB\0"
   "\0"
   /* _mesa_function_pool[13467]: VertexPointerListIBM (dynamic) */
   "iiipi\0"
   "glVertexPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[13497]: TexCoord2fNormal3fVertex3fSUN (dynamic) */
   "ffffffff\0"
   "glTexCoord2fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[13539]: IsFenceNV (dynamic) */
   "i\0"
   "glIsFenceNV\0"
   "\0"
   /* _mesa_function_pool[13554]: ColorTable (offset 339) */
   "iiiiip\0"
   "glColorTable\0"
   "glColorTableSGI\0"
   "glColorTableEXT\0"
   "\0"
   /* _mesa_function_pool[13607]: LoadName (offset 198) */
   "i\0"
   "glLoadName\0"
   "\0"
   /* _mesa_function_pool[13621]: Color3fVertex3fSUN (dynamic) */
   "ffffff\0"
   "glColor3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[13650]: GetnUniformuivARB (will be remapped) */
   "iiip\0"
   "glGetnUniformuivARB\0"
   "\0"
   /* _mesa_function_pool[13676]: ClearIndex (offset 205) */
   "f\0"
   "glClearIndex\0"
   "\0"
   /* _mesa_function_pool[13692]: ConvolutionParameterfv (offset 351) */
   "iip\0"
   "glConvolutionParameterfv\0"
   "glConvolutionParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[13750]: TbufferMask3DFX (dynamic) */
   "i\0"
   "glTbufferMask3DFX\0"
   "\0"
   /* _mesa_function_pool[13771]: GetTexGendv (offset 278) */
   "iip\0"
   "glGetTexGendv\0"
   "\0"
   /* _mesa_function_pool[13790]: MultiTexCoordP1ui (will be remapped) */
   "iii\0"
   "glMultiTexCoordP1ui\0"
   "\0"
   /* _mesa_function_pool[13815]: EvalMesh2 (offset 238) */
   "iiiii\0"
   "glEvalMesh2\0"
   "\0"
   /* _mesa_function_pool[13834]: Vertex4fv (offset 145) */
   "p\0"
   "glVertex4fv\0"
   "\0"
   /* _mesa_function_pool[13849]: SelectPerfMonitorCountersAMD (will be remapped) */
   "iiiip\0"
   "glSelectPerfMonitorCountersAMD\0"
   "\0"
   /* _mesa_function_pool[13887]: BindFramebuffer (will be remapped) */
   "ii\0"
   "glBindFramebuffer\0"
   "glBindFramebufferOES\0"
   "\0"
   /* _mesa_function_pool[13930]: CreateProgram (will be remapped) */
   "\0"
   "glCreateProgram\0"
   "\0"
   /* _mesa_function_pool[13948]: GetMinmax (offset 364) */
   "iiiip\0"
   "glGetMinmax\0"
   "glGetMinmaxEXT\0"
   "\0"
   /* _mesa_function_pool[13982]: Color3fVertex3fvSUN (dynamic) */
   "pp\0"
   "glColor3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[14008]: VertexAttribs3svNV (will be remapped) */
   "iip\0"
   "glVertexAttribs3svNV\0"
   "\0"
   /* _mesa_function_pool[14034]: GetActiveUniformsiv (will be remapped) */
   "iipip\0"
   "glGetActiveUniformsiv\0"
   "\0"
   /* _mesa_function_pool[14063]: VertexAttrib2sv (will be remapped) */
   "ip\0"
   "glVertexAttrib2sv\0"
   "glVertexAttrib2svARB\0"
   "\0"
   /* _mesa_function_pool[14106]: GetProgramEnvParameterdvARB (will be remapped) */
   "iip\0"
   "glGetProgramEnvParameterdvARB\0"
   "\0"
   /* _mesa_function_pool[14141]: VertexAttrib1dNV (will be remapped) */
   "id\0"
   "glVertexAttrib1dNV\0"
   "\0"
   /* _mesa_function_pool[14164]: ReplacementCodeuiNormal3fVertex3fSUN (dynamic) */
   "iffffff\0"
   "glReplacementCodeuiNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[14212]: GetPerfMonitorGroupStringAMD (will be remapped) */
   "iipp\0"
   "glGetPerfMonitorGroupStringAMD\0"
   "\0"
   /* _mesa_function_pool[14249]: GetError (offset 261) */
   "\0"
   "glGetError\0"
   "\0"
   /* _mesa_function_pool[14262]: PassThrough (offset 199) */
   "f\0"
   "glPassThrough\0"
   "\0"
   /* _mesa_function_pool[14279]: GetListParameterfvSGIX (dynamic) */
   "iip\0"
   "glGetListParameterfvSGIX\0"
   "\0"
   /* _mesa_function_pool[14309]: GetObjectParameterivAPPLE (will be remapped) */
   "iiip\0"
   "glGetObjectParameterivAPPLE\0"
   "\0"
   /* _mesa_function_pool[14343]: GlobalAlphaFactorubSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactorubSUN\0"
   "\0"
   /* _mesa_function_pool[14371]: BindBuffersRange (will be remapped) */
   "iiippp\0"
   "glBindBuffersRange\0"
   "\0"
   /* _mesa_function_pool[14398]: VertexAttrib4fvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4fv\0"
   "glVertexAttrib4fvARB\0"
   "\0"
   /* _mesa_function_pool[14441]: WindowPos3dv (will be remapped) */
   "p\0"
   "glWindowPos3dv\0"
   "glWindowPos3dvARB\0"
   "glWindowPos3dvMESA\0"
   "\0"
   /* _mesa_function_pool[14496]: TexGenxOES (will be remapped) */
   "iii\0"
   "glTexGenxOES\0"
   "\0"
   /* _mesa_function_pool[14514]: DeleteFencesNV (dynamic) */
   "ip\0"
   "glDeleteFencesNV\0"
   "\0"
   /* _mesa_function_pool[14535]: GetImageTransformParameterivHP (dynamic) */
   "iip\0"
   "glGetImageTransformParameterivHP\0"
   "\0"
   /* _mesa_function_pool[14573]: StencilOp (offset 244) */
   "iii\0"
   "glStencilOp\0"
   "\0"
   /* _mesa_function_pool[14590]: Binormal3fEXT (dynamic) */
   "fff\0"
   "glBinormal3fEXT\0"
   "\0"
   /* _mesa_function_pool[14611]: ProgramUniform1iv (will be remapped) */
   "iiip\0"
   "glProgramUniform1iv\0"
   "glProgramUniform1ivEXT\0"
   "\0"
   /* _mesa_function_pool[14660]: ProgramUniform3ui (will be remapped) */
   "iiiii\0"
   "glProgramUniform3ui\0"
   "glProgramUniform3uiEXT\0"
   "\0"
   /* _mesa_function_pool[14710]: SecondaryColor3sv (will be remapped) */
   "p\0"
   "glSecondaryColor3sv\0"
   "glSecondaryColor3svEXT\0"
   "\0"
   /* _mesa_function_pool[14756]: TexCoordP3ui (will be remapped) */
   "ii\0"
   "glTexCoordP3ui\0"
   "\0"
   /* _mesa_function_pool[14775]: Fogxv (will be remapped) */
   "ip\0"
   "glFogxvOES\0"
   "glFogxv\0"
   "\0"
   /* _mesa_function_pool[14798]: VertexPointervINTEL (dynamic) */
   "iip\0"
   "glVertexPointervINTEL\0"
   "\0"
   /* _mesa_function_pool[14825]: VertexAttribP1ui (will be remapped) */
   "iiii\0"
   "glVertexAttribP1ui\0"
   "\0"
   /* _mesa_function_pool[14850]: DeleteLists (offset 4) */
   "ii\0"
   "glDeleteLists\0"
   "\0"
   /* _mesa_function_pool[14868]: LogicOp (offset 242) */
   "i\0"
   "glLogicOp\0"
   "\0"
   /* _mesa_function_pool[14881]: RenderbufferStorageMultisample (will be remapped) */
   "iiiii\0"
   "glRenderbufferStorageMultisample\0"
   "glRenderbufferStorageMultisampleEXT\0"
   "\0"
   /* _mesa_function_pool[14957]: WindowPos3d (will be remapped) */
   "ddd\0"
   "glWindowPos3d\0"
   "glWindowPos3dARB\0"
   "glWindowPos3dMESA\0"
   "\0"
   /* _mesa_function_pool[15011]: Enablei (will be remapped) */
   "ii\0"
   "glEnableIndexedEXT\0"
   "glEnablei\0"
   "\0"
   /* _mesa_function_pool[15044]: WindowPos3f (will be remapped) */
   "fff\0"
   "glWindowPos3f\0"
   "glWindowPos3fARB\0"
   "glWindowPos3fMESA\0"
   "\0"
   /* _mesa_function_pool[15098]: GenProgramsARB (will be remapped) */
   "ip\0"
   "glGenProgramsARB\0"
   "glGenProgramsNV\0"
   "\0"
   /* _mesa_function_pool[15135]: RasterPos2sv (offset 69) */
   "p\0"
   "glRasterPos2sv\0"
   "\0"
   /* _mesa_function_pool[15153]: WindowPos3i (will be remapped) */
   "iii\0"
   "glWindowPos3i\0"
   "glWindowPos3iARB\0"
   "glWindowPos3iMESA\0"
   "\0"
   /* _mesa_function_pool[15207]: MultiTexCoord4iv (offset 405) */
   "ip\0"
   "glMultiTexCoord4iv\0"
   "glMultiTexCoord4ivARB\0"
   "\0"
   /* _mesa_function_pool[15252]: TexCoord1sv (offset 101) */
   "p\0"
   "glTexCoord1sv\0"
   "\0"
   /* _mesa_function_pool[15269]: WindowPos3s (will be remapped) */
   "iii\0"
   "glWindowPos3s\0"
   "glWindowPos3sARB\0"
   "glWindowPos3sMESA\0"
   "\0"
   /* _mesa_function_pool[15323]: PixelMapusv (offset 253) */
   "iip\0"
   "glPixelMapusv\0"
   "\0"
   /* _mesa_function_pool[15342]: DebugMessageInsert (will be remapped) */
   "iiiiip\0"
   "glDebugMessageInsertARB\0"
   "glDebugMessageInsert\0"
   "\0"
   /* _mesa_function_pool[15395]: Orthof (will be remapped) */
   "ffffff\0"
   "glOrthofOES\0"
   "glOrthof\0"
   "\0"
   /* _mesa_function_pool[15424]: CompressedTexImage2D (will be remapped) */
   "iiiiiiip\0"
   "glCompressedTexImage2D\0"
   "glCompressedTexImage2DARB\0"
   "\0"
   /* _mesa_function_pool[15483]: DeleteObjectARB (will be remapped) */
   "i\0"
   "glDeleteObjectARB\0"
   "\0"
   /* _mesa_function_pool[15504]: IsSync (will be remapped) */
   "i\0"
   "glIsSync\0"
   "\0"
   /* _mesa_function_pool[15516]: Color4uiv (offset 38) */
   "p\0"
   "glColor4uiv\0"
   "\0"
   /* _mesa_function_pool[15531]: MultiTexCoord1sv (offset 383) */
   "ip\0"
   "glMultiTexCoord1sv\0"
   "glMultiTexCoord1svARB\0"
   "\0"
   /* _mesa_function_pool[15576]: Orthox (will be remapped) */
   "iiiiii\0"
   "glOrthoxOES\0"
   "glOrthox\0"
   "\0"
   /* _mesa_function_pool[15605]: PushAttrib (offset 219) */
   "i\0"
   "glPushAttrib\0"
   "\0"
   /* _mesa_function_pool[15621]: RasterPos2i (offset 66) */
   "ii\0"
   "glRasterPos2i\0"
   "\0"
   /* _mesa_function_pool[15639]: ClipPlane (offset 150) */
   "ip\0"
   "glClipPlane\0"
   "\0"
   /* _mesa_function_pool[15655]: TexCoord2fColor3fVertex3fSUN (dynamic) */
   "ffffffff\0"
   "glTexCoord2fColor3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[15696]: GetProgramivNV (will be remapped) */
   "iip\0"
   "glGetProgramivNV\0"
   "\0"
   /* _mesa_function_pool[15718]: RasterPos2f (offset 64) */
   "ff\0"
   "glRasterPos2f\0"
   "\0"
   /* _mesa_function_pool[15736]: RasterPos2d (offset 62) */
   "dd\0"
   "glRasterPos2d\0"
   "\0"
   /* _mesa_function_pool[15754]: RasterPos3fv (offset 73) */
   "p\0"
   "glRasterPos3fv\0"
   "\0"
   /* _mesa_function_pool[15772]: InvalidateSubFramebuffer (will be remapped) */
   "iipiiii\0"
   "glInvalidateSubFramebuffer\0"
   "\0"
   /* _mesa_function_pool[15808]: Color4ub (offset 35) */
   "iiii\0"
   "glColor4ub\0"
   "\0"
   /* _mesa_function_pool[15825]: RasterPos2s (offset 68) */
   "ii\0"
   "glRasterPos2s\0"
   "\0"
   /* _mesa_function_pool[15843]: Color4fNormal3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glColor4fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[15878]: GetVertexAttribivNV (will be remapped) */
   "iip\0"
   "glGetVertexAttribivNV\0"
   "\0"
   /* _mesa_function_pool[15905]: TexSubImage4DSGIS (dynamic) */
   "iiiiiiiiiiiip\0"
   "glTexSubImage4DSGIS\0"
   "\0"
   /* _mesa_function_pool[15940]: MultiTexCoord3dv (offset 393) */
   "ip\0"
   "glMultiTexCoord3dv\0"
   "glMultiTexCoord3dvARB\0"
   "\0"
   /* _mesa_function_pool[15985]: BindProgramPipeline (will be remapped) */
   "i\0"
   "glBindProgramPipeline\0"
   "glBindProgramPipelineEXT\0"
   "\0"
   /* _mesa_function_pool[16035]: VertexAttribP4uiv (will be remapped) */
   "iiip\0"
   "glVertexAttribP4uiv\0"
   "\0"
   /* _mesa_function_pool[16061]: DebugMessageCallback (will be remapped) */
   "pp\0"
   "glDebugMessageCallbackARB\0"
   "glDebugMessageCallback\0"
   "\0"
   /* _mesa_function_pool[16114]: MultiTexCoord1i (offset 380) */
   "ii\0"
   "glMultiTexCoord1i\0"
   "glMultiTexCoord1iARB\0"
   "\0"
   /* _mesa_function_pool[16157]: WindowPos2dv (will be remapped) */
   "p\0"
   "glWindowPos2dv\0"
   "glWindowPos2dvARB\0"
   "glWindowPos2dvMESA\0"
   "\0"
   /* _mesa_function_pool[16212]: TexParameterIuiv (will be remapped) */
   "iip\0"
   "glTexParameterIuivEXT\0"
   "glTexParameterIuiv\0"
   "\0"
   /* _mesa_function_pool[16258]: DeletePerfQueryINTEL (will be remapped) */
   "i\0"
   "glDeletePerfQueryINTEL\0"
   "\0"
   /* _mesa_function_pool[16284]: MultiTexCoord1d (offset 376) */
   "id\0"
   "glMultiTexCoord1d\0"
   "glMultiTexCoord1dARB\0"
   "\0"
   /* _mesa_function_pool[16327]: GenVertexArraysAPPLE (will be remapped) */
   "ip\0"
   "glGenVertexArraysAPPLE\0"
   "\0"
   /* _mesa_function_pool[16354]: MultiTexCoord1s (offset 382) */
   "ii\0"
   "glMultiTexCoord1s\0"
   "glMultiTexCoord1sARB\0"
   "\0"
   /* _mesa_function_pool[16397]: BeginConditionalRender (will be remapped) */
   "ii\0"
   "glBeginConditionalRender\0"
   "glBeginConditionalRenderNV\0"
   "\0"
   /* _mesa_function_pool[16453]: LoadPaletteFromModelViewMatrixOES (dynamic) */
   "\0"
   "glLoadPaletteFromModelViewMatrixOES\0"
   "\0"
   /* _mesa_function_pool[16491]: GetShaderiv (will be remapped) */
   "iip\0"
   "glGetShaderiv\0"
   "\0"
   /* _mesa_function_pool[16510]: GetMapAttribParameterfvNV (dynamic) */
   "iiip\0"
   "glGetMapAttribParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[16544]: CopyConvolutionFilter1D (offset 354) */
   "iiiii\0"
   "glCopyConvolutionFilter1D\0"
   "glCopyConvolutionFilter1DEXT\0"
   "\0"
   /* _mesa_function_pool[16606]: ClearBufferfv (will be remapped) */
   "iip\0"
   "glClearBufferfv\0"
   "\0"
   /* _mesa_function_pool[16627]: InstrumentsBufferSGIX (dynamic) */
   "ip\0"
   "glInstrumentsBufferSGIX\0"
   "\0"
   /* _mesa_function_pool[16655]: CreateShaderObjectARB (will be remapped) */
   "i\0"
   "glCreateShaderObjectARB\0"
   "\0"
   /* _mesa_function_pool[16682]: GetTexParameterxv (will be remapped) */
   "iip\0"
   "glGetTexParameterxvOES\0"
   "glGetTexParameterxv\0"
   "\0"
   /* _mesa_function_pool[16730]: GetAttachedShaders (will be remapped) */
   "iipp\0"
   "glGetAttachedShaders\0"
   "\0"
   /* _mesa_function_pool[16757]: ClearBufferfi (will be remapped) */
   "iifi\0"
   "glClearBufferfi\0"
   "\0"
   /* _mesa_function_pool[16779]: Materialiv (offset 172) */
   "iip\0"
   "glMaterialiv\0"
   "\0"
   /* _mesa_function_pool[16797]: DeleteFragmentShaderATI (will be remapped) */
   "i\0"
   "glDeleteFragmentShaderATI\0"
   "\0"
   /* _mesa_function_pool[16826]: Tangent3dvEXT (dynamic) */
   "p\0"
   "glTangent3dvEXT\0"
   "\0"
   /* _mesa_function_pool[16845]: DrawElementsInstancedBaseVertex (will be remapped) */
   "iiipii\0"
   "glDrawElementsInstancedBaseVertex\0"
   "\0"
   /* _mesa_function_pool[16887]: DisableClientState (offset 309) */
   "i\0"
   "glDisableClientState\0"
   "\0"
   /* _mesa_function_pool[16911]: TexGeni (offset 192) */
   "iii\0"
   "glTexGeni\0"
   "glTexGeniOES\0"
   "\0"
   /* _mesa_function_pool[16939]: TexGenf (offset 190) */
   "iif\0"
   "glTexGenf\0"
   "glTexGenfOES\0"
   "\0"
   /* _mesa_function_pool[16967]: TexGend (offset 188) */
   "iid\0"
   "glTexGend\0"
   "\0"
   /* _mesa_function_pool[16982]: GetVertexAttribfvNV (will be remapped) */
   "iip\0"
   "glGetVertexAttribfvNV\0"
   "\0"
   /* _mesa_function_pool[17009]: ColorPointerListIBM (dynamic) */
   "iiipi\0"
   "glColorPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[17038]: Color4sv (offset 34) */
   "p\0"
   "glColor4sv\0"
   "\0"
   /* _mesa_function_pool[17052]: GetCombinerInputParameterfvNV (dynamic) */
   "iiiip\0"
   "glGetCombinerInputParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[17091]: LoadTransposeMatrixf (will be remapped) */
   "p\0"
   "glLoadTransposeMatrixf\0"
   "glLoadTransposeMatrixfARB\0"
   "\0"
   /* _mesa_function_pool[17143]: LoadTransposeMatrixd (will be remapped) */
   "p\0"
   "glLoadTransposeMatrixd\0"
   "glLoadTransposeMatrixdARB\0"
   "\0"
   /* _mesa_function_pool[17195]: PixelZoom (offset 246) */
   "ff\0"
   "glPixelZoom\0"
   "\0"
   /* _mesa_function_pool[17211]: ProgramEnvParameter4dARB (will be remapped) */
   "iidddd\0"
   "glProgramEnvParameter4dARB\0"
   "glProgramParameter4dNV\0"
   "\0"
   /* _mesa_function_pool[17269]: ColorTableParameterfv (offset 340) */
   "iip\0"
   "glColorTableParameterfv\0"
   "glColorTableParameterfvSGI\0"
   "\0"
   /* _mesa_function_pool[17325]: IsTexture (offset 330) */
   "i\0"
   "glIsTexture\0"
   "glIsTextureEXT\0"
   "\0"
   /* _mesa_function_pool[17355]: ProgramUniform3uiv (will be remapped) */
   "iiip\0"
   "glProgramUniform3uiv\0"
   "glProgramUniform3uivEXT\0"
   "\0"
   /* _mesa_function_pool[17406]: ProgramUniform2fv (will be remapped) */
   "iiip\0"
   "glProgramUniform2fv\0"
   "glProgramUniform2fvEXT\0"
   "\0"
   /* _mesa_function_pool[17455]: ImageTransformParameterivHP (dynamic) */
   "iip\0"
   "glImageTransformParameterivHP\0"
   "\0"
   /* _mesa_function_pool[17490]: VertexAttrib4sNV (will be remapped) */
   "iiiii\0"
   "glVertexAttrib4sNV\0"
   "\0"
   /* _mesa_function_pool[17516]: GetMapdv (offset 266) */
   "iip\0"
   "glGetMapdv\0"
   "\0"
   /* _mesa_function_pool[17532]: GenVertexArrays (will be remapped) */
   "ip\0"
   "glGenVertexArrays\0"
   "glGenVertexArraysOES\0"
   "\0"
   /* _mesa_function_pool[17575]: ReplacementCodeuiColor4ubVertex3fSUN (dynamic) */
   "iiiiifff\0"
   "glReplacementCodeuiColor4ubVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[17624]: IsBuffer (will be remapped) */
   "i\0"
   "glIsBuffer\0"
   "glIsBufferARB\0"
   "\0"
   /* _mesa_function_pool[17652]: ColorP4ui (will be remapped) */
   "ii\0"
   "glColorP4ui\0"
   "\0"
   /* _mesa_function_pool[17668]: SpriteParameteriSGIX (dynamic) */
   "ii\0"
   "glSpriteParameteriSGIX\0"
   "\0"
   /* _mesa_function_pool[17695]: TexCoordP3uiv (will be remapped) */
   "ip\0"
   "glTexCoordP3uiv\0"
   "\0"
   /* _mesa_function_pool[17715]: WeightusvARB (dynamic) */
   "ip\0"
   "glWeightusvARB\0"
   "\0"
   /* _mesa_function_pool[17734]: EvalMapsNV (dynamic) */
   "ii\0"
   "glEvalMapsNV\0"
   "\0"
   /* _mesa_function_pool[17751]: ReplacementCodeuiSUN (dynamic) */
   "i\0"
   "glReplacementCodeuiSUN\0"
   "\0"
   /* _mesa_function_pool[17777]: GlobalAlphaFactoruiSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactoruiSUN\0"
   "\0"
   /* _mesa_function_pool[17805]: Uniform1iv (will be remapped) */
   "iip\0"
   "glUniform1iv\0"
   "glUniform1ivARB\0"
   "\0"
   /* _mesa_function_pool[17839]: Uniform4uiv (will be remapped) */
   "iip\0"
   "glUniform4uivEXT\0"
   "glUniform4uiv\0"
   "\0"
   /* _mesa_function_pool[17875]: PopDebugGroup (will be remapped) */
   "\0"
   "glPopDebugGroup\0"
   "\0"
   /* _mesa_function_pool[17893]: VertexAttrib1d (will be remapped) */
   "id\0"
   "glVertexAttrib1d\0"
   "glVertexAttrib1dARB\0"
   "\0"
   /* _mesa_function_pool[17934]: CompressedTexImage1D (will be remapped) */
   "iiiiiip\0"
   "glCompressedTexImage1D\0"
   "glCompressedTexImage1DARB\0"
   "\0"
   /* _mesa_function_pool[17992]: TexBufferRange (will be remapped) */
   "iiiii\0"
   "glTexBufferRange\0"
   "\0"
   /* _mesa_function_pool[18016]: VertexAttrib1s (will be remapped) */
   "ii\0"
   "glVertexAttrib1s\0"
   "glVertexAttrib1sARB\0"
   "\0"
   /* _mesa_function_pool[18057]: MultiDrawElementsIndirect (will be remapped) */
   "iipii\0"
   "glMultiDrawElementsIndirect\0"
   "\0"
   /* _mesa_function_pool[18092]: FogCoordfvEXT (will be remapped) */
   "p\0"
   "glFogCoordfv\0"
   "glFogCoordfvEXT\0"
   "\0"
   /* _mesa_function_pool[18124]: BeginPerfMonitorAMD (will be remapped) */
   "i\0"
   "glBeginPerfMonitorAMD\0"
   "\0"
   /* _mesa_function_pool[18149]: GetColorTableParameterfv (offset 344) */
   "iip\0"
   "glGetColorTableParameterfv\0"
   "glGetColorTableParameterfvSGI\0"
   "glGetColorTableParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[18241]: MultiTexCoord3fARB (offset 394) */
   "ifff\0"
   "glMultiTexCoord3f\0"
   "glMultiTexCoord3fARB\0"
   "\0"
   /* _mesa_function_pool[18286]: GetTexLevelParameterfv (offset 284) */
   "iiip\0"
   "glGetTexLevelParameterfv\0"
   "\0"
   /* _mesa_function_pool[18317]: Vertex2sv (offset 133) */
   "p\0"
   "glVertex2sv\0"
   "\0"
   /* _mesa_function_pool[18332]: GetnMapdvARB (will be remapped) */
   "iiip\0"
   "glGetnMapdvARB\0"
   "\0"
   /* _mesa_function_pool[18353]: VertexAttrib2dNV (will be remapped) */
   "idd\0"
   "glVertexAttrib2dNV\0"
   "\0"
   /* _mesa_function_pool[18377]: GetTrackMatrixivNV (will be remapped) */
   "iiip\0"
   "glGetTrackMatrixivNV\0"
   "\0"
   /* _mesa_function_pool[18404]: VertexAttrib3svNV (will be remapped) */
   "ip\0"
   "glVertexAttrib3svNV\0"
   "\0"
   /* _mesa_function_pool[18428]: GetTexEnviv (offset 277) */
   "iip\0"
   "glGetTexEnviv\0"
   "\0"
   /* _mesa_function_pool[18447]: ViewportArrayv (will be remapped) */
   "iip\0"
   "glViewportArrayv\0"
   "\0"
   /* _mesa_function_pool[18469]: ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN (dynamic) */
   "iffffffffffff\0"
   "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[18540]: SeparableFilter2D (offset 360) */
   "iiiiiipp\0"
   "glSeparableFilter2D\0"
   "glSeparableFilter2DEXT\0"
   "\0"
   /* _mesa_function_pool[18593]: ReplacementCodeuiColor4ubVertex3fvSUN (dynamic) */
   "ppp\0"
   "glReplacementCodeuiColor4ubVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[18638]: ArrayElement (offset 306) */
   "i\0"
   "glArrayElement\0"
   "glArrayElementEXT\0"
   "\0"
   /* _mesa_function_pool[18674]: TexImage2D (offset 183) */
   "iiiiiiiip\0"
   "glTexImage2D\0"
   "\0"
   /* _mesa_function_pool[18698]: FragmentMaterialiSGIX (dynamic) */
   "iii\0"
   "glFragmentMaterialiSGIX\0"
   "\0"
   /* _mesa_function_pool[18727]: RasterPos2dv (offset 63) */
   "p\0"
   "glRasterPos2dv\0"
   "\0"
   /* _mesa_function_pool[18745]: Fogiv (offset 156) */
   "ip\0"
   "glFogiv\0"
   "\0"
   /* _mesa_function_pool[18757]: EndQuery (will be remapped) */
   "i\0"
   "glEndQuery\0"
   "glEndQueryARB\0"
   "\0"
   /* _mesa_function_pool[18785]: TexCoord1dv (offset 95) */
   "p\0"
   "glTexCoord1dv\0"
   "\0"
   /* _mesa_function_pool[18802]: TexCoord4dv (offset 119) */
   "p\0"
   "glTexCoord4dv\0"
   "\0"
   /* _mesa_function_pool[18819]: GetVertexAttribdvNV (will be remapped) */
   "iip\0"
   "glGetVertexAttribdvNV\0"
   "\0"
   /* _mesa_function_pool[18846]: Clear (offset 203) */
   "i\0"
   "glClear\0"
   "\0"
   /* _mesa_function_pool[18857]: VertexAttrib4sv (will be remapped) */
   "ip\0"
   "glVertexAttrib4sv\0"
   "glVertexAttrib4svARB\0"
   "\0"
   /* _mesa_function_pool[18900]: Ortho (offset 296) */
   "dddddd\0"
   "glOrtho\0"
   "\0"
   /* _mesa_function_pool[18916]: Uniform3uiv (will be remapped) */
   "iip\0"
   "glUniform3uivEXT\0"
   "glUniform3uiv\0"
   "\0"
   /* _mesa_function_pool[18952]: MatrixIndexPointerARB (dynamic) */
   "iiip\0"
   "glMatrixIndexPointerARB\0"
   "glMatrixIndexPointerOES\0"
   "\0"
   /* _mesa_function_pool[19006]: EndQueryIndexed (will be remapped) */
   "ii\0"
   "glEndQueryIndexed\0"
   "\0"
   /* _mesa_function_pool[19028]: TexParameterxv (will be remapped) */
   "iip\0"
   "glTexParameterxvOES\0"
   "glTexParameterxv\0"
   "\0"
   /* _mesa_function_pool[19070]: SampleMaskSGIS (will be remapped) */
   "fi\0"
   "glSampleMaskSGIS\0"
   "glSampleMaskEXT\0"
   "\0"
   /* _mesa_function_pool[19107]: FramebufferTextureFaceARB (will be remapped) */
   "iiiii\0"
   "glFramebufferTextureFaceARB\0"
   "\0"
   /* _mesa_function_pool[19142]: ProgramUniformMatrix2fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix2fv\0"
   "glProgramUniformMatrix2fvEXT\0"
   "\0"
   /* _mesa_function_pool[19204]: ProgramLocalParameter4fvARB (will be remapped) */
   "iip\0"
   "glProgramLocalParameter4fvARB\0"
   "\0"
   /* _mesa_function_pool[19239]: GetProgramStringNV (will be remapped) */
   "iip\0"
   "glGetProgramStringNV\0"
   "\0"
   /* _mesa_function_pool[19265]: Binormal3svEXT (dynamic) */
   "p\0"
   "glBinormal3svEXT\0"
   "\0"
   /* _mesa_function_pool[19285]: GetCombinerInputParameterivNV (dynamic) */
   "iiiip\0"
   "glGetCombinerInputParameterivNV\0"
   "\0"
   /* _mesa_function_pool[19324]: VertexAttribI3iEXT (will be remapped) */
   "iiii\0"
   "glVertexAttribI3iEXT\0"
   "glVertexAttribI3i\0"
   "\0"
   /* _mesa_function_pool[19369]: ClearColorx (will be remapped) */
   "iiii\0"
   "glClearColorxOES\0"
   "glClearColorx\0"
   "\0"
   /* _mesa_function_pool[19406]: EndTransformFeedback (will be remapped) */
   "\0"
   "glEndTransformFeedback\0"
   "glEndTransformFeedbackEXT\0"
   "\0"
   /* _mesa_function_pool[19457]: GetHandleARB (will be remapped) */
   "i\0"
   "glGetHandleARB\0"
   "\0"
   /* _mesa_function_pool[19475]: GetProgramBinary (will be remapped) */
   "iippp\0"
   "glGetProgramBinary\0"
   "glGetProgramBinaryOES\0"
   "\0"
   /* _mesa_function_pool[19523]: ViewportIndexedfv (will be remapped) */
   "ip\0"
   "glViewportIndexedfv\0"
   "\0"
   /* _mesa_function_pool[19547]: CallList (offset 2) */
   "i\0"
   "glCallList\0"
   "\0"
   /* _mesa_function_pool[19561]: Materialfv (offset 170) */
   "iip\0"
   "glMaterialfv\0"
   "\0"
   /* _mesa_function_pool[19579]: DeleteProgram (will be remapped) */
   "i\0"
   "glDeleteProgram\0"
   "\0"
   /* _mesa_function_pool[19598]: GetActiveAtomicCounterBufferiv (will be remapped) */
   "iiip\0"
   "glGetActiveAtomicCounterBufferiv\0"
   "\0"
   /* _mesa_function_pool[19637]: TexParameterIiv (will be remapped) */
   "iip\0"
   "glTexParameterIivEXT\0"
   "glTexParameterIiv\0"
   "\0"
   /* _mesa_function_pool[19681]: VertexWeightfEXT (dynamic) */
   "f\0"
   "glVertexWeightfEXT\0"
   "\0"
   /* _mesa_function_pool[19703]: FlushVertexArrayRangeNV (dynamic) */
   "\0"
   "glFlushVertexArrayRangeNV\0"
   "\0"
   /* _mesa_function_pool[19731]: GetConvolutionFilter (offset 356) */
   "iiip\0"
   "glGetConvolutionFilter\0"
   "glGetConvolutionFilterEXT\0"
   "\0"
   /* _mesa_function_pool[19786]: MultiModeDrawElementsIBM (will be remapped) */
   "ppipii\0"
   "glMultiModeDrawElementsIBM\0"
   "\0"
   /* _mesa_function_pool[19821]: Uniform2iv (will be remapped) */
   "iip\0"
   "glUniform2iv\0"
   "glUniform2ivARB\0"
   "\0"
   /* _mesa_function_pool[19855]: GetFixedv (will be remapped) */
   "ip\0"
   "glGetFixedvOES\0"
   "glGetFixedv\0"
   "\0"
   /* _mesa_function_pool[19886]: ProgramParameters4dvNV (will be remapped) */
   "iiip\0"
   "glProgramParameters4dvNV\0"
   "\0"
   /* _mesa_function_pool[19917]: Binormal3dvEXT (dynamic) */
   "p\0"
   "glBinormal3dvEXT\0"
   "\0"
   /* _mesa_function_pool[19937]: SampleCoveragex (will be remapped) */
   "ii\0"
   "glSampleCoveragexOES\0"
   "glSampleCoveragex\0"
   "\0"
   /* _mesa_function_pool[19980]: GetPerfQueryInfoINTEL (will be remapped) */
   "iippppp\0"
   "glGetPerfQueryInfoINTEL\0"
   "\0"
   /* _mesa_function_pool[20013]: DeleteFramebuffers (will be remapped) */
   "ip\0"
   "glDeleteFramebuffers\0"
   "glDeleteFramebuffersEXT\0"
   "glDeleteFramebuffersOES\0"
   "\0"
   /* _mesa_function_pool[20086]: CombinerInputNV (dynamic) */
   "iiiiii\0"
   "glCombinerInputNV\0"
   "\0"
   /* _mesa_function_pool[20112]: VertexAttrib4uiv (will be remapped) */
   "ip\0"
   "glVertexAttrib4uiv\0"
   "glVertexAttrib4uivARB\0"
   "\0"
   /* _mesa_function_pool[20157]: VertexAttrib4Nsv (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nsv\0"
   "glVertexAttrib4NsvARB\0"
   "\0"
   /* _mesa_function_pool[20202]: Vertex4s (offset 148) */
   "iiii\0"
   "glVertex4s\0"
   "\0"
   /* _mesa_function_pool[20219]: VertexAttribI2iEXT (will be remapped) */
   "iii\0"
   "glVertexAttribI2iEXT\0"
   "glVertexAttribI2i\0"
   "\0"
   /* _mesa_function_pool[20263]: Vertex4f (offset 144) */
   "ffff\0"
   "glVertex4f\0"
   "\0"
   /* _mesa_function_pool[20280]: Vertex4d (offset 142) */
   "dddd\0"
   "glVertex4d\0"
   "\0"
   /* _mesa_function_pool[20297]: GetTexGenfv (offset 279) */
   "iip\0"
   "glGetTexGenfv\0"
   "glGetTexGenfvOES\0"
   "\0"
   /* _mesa_function_pool[20333]: Vertex4i (offset 146) */
   "iiii\0"
   "glVertex4i\0"
   "\0"
   /* _mesa_function_pool[20350]: VertexWeightPointerEXT (dynamic) */
   "iiip\0"
   "glVertexWeightPointerEXT\0"
   "\0"
   /* _mesa_function_pool[20381]: StencilFuncSeparateATI (will be remapped) */
   "iiii\0"
   "glStencilFuncSeparateATI\0"
   "\0"
   /* _mesa_function_pool[20412]: GetVertexAttribIuiv (will be remapped) */
   "iip\0"
   "glGetVertexAttribIuivEXT\0"
   "glGetVertexAttribIuiv\0"
   "\0"
   /* _mesa_function_pool[20464]: LightModelfv (offset 164) */
   "ip\0"
   "glLightModelfv\0"
   "\0"
   /* _mesa_function_pool[20483]: Vertex4dv (offset 143) */
   "p\0"
   "glVertex4dv\0"
   "\0"
   /* _mesa_function_pool[20498]: ProgramParameters4fvNV (will be remapped) */
   "iiip\0"
   "glProgramParameters4fvNV\0"
   "\0"
   /* _mesa_function_pool[20529]: GetInfoLogARB (will be remapped) */
   "iipp\0"
   "glGetInfoLogARB\0"
   "\0"
   /* _mesa_function_pool[20551]: StencilMask (offset 209) */
   "i\0"
   "glStencilMask\0"
   "\0"
   /* _mesa_function_pool[20568]: IsList (offset 287) */
   "i\0"
   "glIsList\0"
   "\0"
   /* _mesa_function_pool[20580]: ClearBufferiv (will be remapped) */
   "iip\0"
   "glClearBufferiv\0"
   "\0"
   /* _mesa_function_pool[20601]: GetIntegeri_v (will be remapped) */
   "iip\0"
   "glGetIntegerIndexedvEXT\0"
   "glGetIntegeri_v\0"
   "\0"
   /* _mesa_function_pool[20646]: ProgramUniform2iv (will be remapped) */
   "iiip\0"
   "glProgramUniform2iv\0"
   "glProgramUniform2ivEXT\0"
   "\0"
   /* _mesa_function_pool[20695]: VertexAttribs4svNV (will be remapped) */
   "iip\0"
   "glVertexAttribs4svNV\0"
   "\0"
   /* _mesa_function_pool[20721]: FogCoordPointer (will be remapped) */
   "iip\0"
   "glFogCoordPointer\0"
   "glFogCoordPointerEXT\0"
   "\0"
   /* _mesa_function_pool[20765]: SecondaryColor3us (will be remapped) */
   "iii\0"
   "glSecondaryColor3us\0"
   "glSecondaryColor3usEXT\0"
   "\0"
   /* _mesa_function_pool[20813]: DeformationMap3dSGIX (dynamic) */
   "iddiiddiiddiip\0"
   "glDeformationMap3dSGIX\0"
   "\0"
   /* _mesa_function_pool[20852]: TextureNormalEXT (dynamic) */
   "i\0"
   "glTextureNormalEXT\0"
   "\0"
   /* _mesa_function_pool[20874]: SecondaryColor3ub (will be remapped) */
   "iii\0"
   "glSecondaryColor3ub\0"
   "glSecondaryColor3ubEXT\0"
   "\0"
   /* _mesa_function_pool[20922]: SecondaryColor3ui (will be remapped) */
   "iii\0"
   "glSecondaryColor3ui\0"
   "glSecondaryColor3uiEXT\0"
   "\0"
   /* _mesa_function_pool[20970]: VertexAttribI3uivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI3uivEXT\0"
   "glVertexAttribI3uiv\0"
   "\0"
   /* _mesa_function_pool[21017]: Binormal3fvEXT (dynamic) */
   "p\0"
   "glBinormal3fvEXT\0"
   "\0"
   /* _mesa_function_pool[21037]: TexCoordPointervINTEL (dynamic) */
   "iip\0"
   "glTexCoordPointervINTEL\0"
   "\0"
   /* _mesa_function_pool[21066]: VertexAttrib1sNV (will be remapped) */
   "ii\0"
   "glVertexAttrib1sNV\0"
   "\0"
   /* _mesa_function_pool[21089]: Tangent3bEXT (dynamic) */
   "iii\0"
   "glTangent3bEXT\0"
   "\0"
   /* _mesa_function_pool[21109]: FragmentLightModelfSGIX (dynamic) */
   "if\0"
   "glFragmentLightModelfSGIX\0"
   "\0"
   /* _mesa_function_pool[21139]: InitNames (offset 197) */
   "\0"
   "glInitNames\0"
   "\0"
   /* _mesa_function_pool[21153]: Normal3sv (offset 61) */
   "p\0"
   "glNormal3sv\0"
   "\0"
   /* _mesa_function_pool[21168]: DeleteQueries (will be remapped) */
   "ip\0"
   "glDeleteQueries\0"
   "glDeleteQueriesARB\0"
   "\0"
   /* _mesa_function_pool[21207]: InvalidateFramebuffer (will be remapped) */
   "iip\0"
   "glInvalidateFramebuffer\0"
   "\0"
   /* _mesa_function_pool[21236]: Hint (offset 158) */
   "ii\0"
   "glHint\0"
   "\0"
   /* _mesa_function_pool[21247]: MemoryBarrier (will be remapped) */
   "i\0"
   "glMemoryBarrier\0"
   "\0"
   /* _mesa_function_pool[21266]: CopyColorSubTable (offset 347) */
   "iiiii\0"
   "glCopyColorSubTable\0"
   "glCopyColorSubTableEXT\0"
   "\0"
   /* _mesa_function_pool[21316]: WeightdvARB (dynamic) */
   "ip\0"
   "glWeightdvARB\0"
   "\0"
   /* _mesa_function_pool[21334]: GetObjectParameterfvARB (will be remapped) */
   "iip\0"
   "glGetObjectParameterfvARB\0"
   "\0"
   /* _mesa_function_pool[21365]: GetTexEnvxv (will be remapped) */
   "iip\0"
   "glGetTexEnvxvOES\0"
   "glGetTexEnvxv\0"
   "\0"
   /* _mesa_function_pool[21401]: DrawTexsvOES (will be remapped) */
   "p\0"
   "glDrawTexsvOES\0"
   "\0"
   /* _mesa_function_pool[21419]: Disable (offset 214) */
   "i\0"
   "glDisable\0"
   "\0"
   /* _mesa_function_pool[21432]: ClearColor (offset 206) */
   "ffff\0"
   "glClearColor\0"
   "\0"
   /* _mesa_function_pool[21451]: WeightuivARB (dynamic) */
   "ip\0"
   "glWeightuivARB\0"
   "\0"
   /* _mesa_function_pool[21470]: RasterPos4iv (offset 83) */
   "p\0"
   "glRasterPos4iv\0"
   "\0"
   /* _mesa_function_pool[21488]: VDPAUIsSurfaceNV (will be remapped) */
   "i\0"
   "glVDPAUIsSurfaceNV\0"
   "\0"
   /* _mesa_function_pool[21510]: ProgramUniformMatrix2x3fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix2x3fv\0"
   "glProgramUniformMatrix2x3fvEXT\0"
   "\0"
   /* _mesa_function_pool[21576]: BindVertexBuffer (will be remapped) */
   "iiii\0"
   "glBindVertexBuffer\0"
   "\0"
   /* _mesa_function_pool[21601]: Binormal3iEXT (dynamic) */
   "iii\0"
   "glBinormal3iEXT\0"
   "\0"
   /* _mesa_function_pool[21622]: RasterPos4i (offset 82) */
   "iiii\0"
   "glRasterPos4i\0"
   "\0"
   /* _mesa_function_pool[21642]: RasterPos4d (offset 78) */
   "dddd\0"
   "glRasterPos4d\0"
   "\0"
   /* _mesa_function_pool[21662]: RasterPos4f (offset 80) */
   "ffff\0"
   "glRasterPos4f\0"
   "\0"
   /* _mesa_function_pool[21682]: GetQueryIndexediv (will be remapped) */
   "iiip\0"
   "glGetQueryIndexediv\0"
   "\0"
   /* _mesa_function_pool[21708]: RasterPos3dv (offset 71) */
   "p\0"
   "glRasterPos3dv\0"
   "\0"
   /* _mesa_function_pool[21726]: GetProgramiv (will be remapped) */
   "iip\0"
   "glGetProgramiv\0"
   "\0"
   /* _mesa_function_pool[21746]: TexCoord1iv (offset 99) */
   "p\0"
   "glTexCoord1iv\0"
   "\0"
   /* _mesa_function_pool[21763]: RasterPos4s (offset 84) */
   "iiii\0"
   "glRasterPos4s\0"
   "\0"
   /* _mesa_function_pool[21783]: PixelTexGenParameterfvSGIS (dynamic) */
   "ip\0"
   "glPixelTexGenParameterfvSGIS\0"
   "\0"
   /* _mesa_function_pool[21816]: VertexAttrib3dv (will be remapped) */
   "ip\0"
   "glVertexAttrib3dv\0"
   "glVertexAttrib3dvARB\0"
   "\0"
   /* _mesa_function_pool[21859]: Histogram (offset 367) */
   "iiii\0"
   "glHistogram\0"
   "glHistogramEXT\0"
   "\0"
   /* _mesa_function_pool[21892]: Uniform2fv (will be remapped) */
   "iip\0"
   "glUniform2fv\0"
   "glUniform2fvARB\0"
   "\0"
   /* _mesa_function_pool[21926]: TexImage4DSGIS (dynamic) */
   "iiiiiiiiiip\0"
   "glTexImage4DSGIS\0"
   "\0"
   /* _mesa_function_pool[21956]: Uniform1uiv (will be remapped) */
   "iip\0"
   "glUniform1uivEXT\0"
   "glUniform1uiv\0"
   "\0"
   /* _mesa_function_pool[21992]: GetnPolygonStippleARB (will be remapped) */
   "ip\0"
   "glGetnPolygonStippleARB\0"
   "\0"
   /* _mesa_function_pool[22020]: Color3uiv (offset 22) */
   "p\0"
   "glColor3uiv\0"
   "\0"
   /* _mesa_function_pool[22035]: EvalCoord2fv (offset 235) */
   "p\0"
   "glEvalCoord2fv\0"
   "\0"
   /* _mesa_function_pool[22053]: TextureStorage3DEXT (will be remapped) */
   "iiiiiii\0"
   "glTextureStorage3DEXT\0"
   "\0"
   /* _mesa_function_pool[22084]: VertexAttrib2fARB (will be remapped) */
   "iff\0"
   "glVertexAttrib2f\0"
   "glVertexAttrib2fARB\0"
   "\0"
   /* _mesa_function_pool[22126]: WindowPos2fv (will be remapped) */
   "p\0"
   "glWindowPos2fv\0"
   "glWindowPos2fvARB\0"
   "glWindowPos2fvMESA\0"
   "\0"
   /* _mesa_function_pool[22181]: Tangent3fEXT (dynamic) */
   "fff\0"
   "glTangent3fEXT\0"
   "\0"
   /* _mesa_function_pool[22201]: TexImage3D (offset 371) */
   "iiiiiiiiip\0"
   "glTexImage3D\0"
   "glTexImage3DEXT\0"
   "glTexImage3DOES\0"
   "\0"
   /* _mesa_function_pool[22258]: GetPerfQueryIdByNameINTEL (will be remapped) */
   "pp\0"
   "glGetPerfQueryIdByNameINTEL\0"
   "\0"
   /* _mesa_function_pool[22290]: BindFragDataLocation (will be remapped) */
   "iip\0"
   "glBindFragDataLocationEXT\0"
   "glBindFragDataLocation\0"
   "\0"
   /* _mesa_function_pool[22344]: LightModeliv (offset 166) */
   "ip\0"
   "glLightModeliv\0"
   "\0"
   /* _mesa_function_pool[22363]: Normal3bv (offset 53) */
   "p\0"
   "glNormal3bv\0"
   "\0"
   /* _mesa_function_pool[22378]: BeginQueryIndexed (will be remapped) */
   "iii\0"
   "glBeginQueryIndexed\0"
   "\0"
   /* _mesa_function_pool[22403]: Vertex3iv (offset 139) */
   "p\0"
   "glVertex3iv\0"
   "\0"
   /* _mesa_function_pool[22418]: TexCoord3dv (offset 111) */
   "p\0"
   "glTexCoord3dv\0"
   "\0"
   /* _mesa_function_pool[22435]: GetProgramStringARB (will be remapped) */
   "iip\0"
   "glGetProgramStringARB\0"
   "\0"
   /* _mesa_function_pool[22462]: VertexP3ui (will be remapped) */
   "ii\0"
   "glVertexP3ui\0"
   "\0"
   /* _mesa_function_pool[22479]: CreateProgramObjectARB (will be remapped) */
   "\0"
   "glCreateProgramObjectARB\0"
   "\0"
   /* _mesa_function_pool[22506]: UniformMatrix3fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix3fv\0"
   "glUniformMatrix3fvARB\0"
   "\0"
   /* _mesa_function_pool[22553]: PrioritizeTextures (offset 331) */
   "ipp\0"
   "glPrioritizeTextures\0"
   "glPrioritizeTexturesEXT\0"
   "\0"
   /* _mesa_function_pool[22603]: VertexAttribI3uiEXT (will be remapped) */
   "iiii\0"
   "glVertexAttribI3uiEXT\0"
   "glVertexAttribI3ui\0"
   "\0"
   /* _mesa_function_pool[22650]: AsyncMarkerSGIX (dynamic) */
   "i\0"
   "glAsyncMarkerSGIX\0"
   "\0"
   /* _mesa_function_pool[22671]: GetProgramNamedParameterfvNV (will be remapped) */
   "iipp\0"
   "glGetProgramNamedParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[22708]: GetMaterialxv (will be remapped) */
   "iip\0"
   "glGetMaterialxvOES\0"
   "glGetMaterialxv\0"
   "\0"
   /* _mesa_function_pool[22748]: MatrixIndexusvARB (dynamic) */
   "ip\0"
   "glMatrixIndexusvARB\0"
   "\0"
   /* _mesa_function_pool[22772]: SecondaryColor3uiv (will be remapped) */
   "p\0"
   "glSecondaryColor3uiv\0"
   "glSecondaryColor3uivEXT\0"
   "\0"
   /* _mesa_function_pool[22820]: EndConditionalRender (will be remapped) */
   "\0"
   "glEndConditionalRender\0"
   "glEndConditionalRenderNV\0"
   "\0"
   /* _mesa_function_pool[22870]: ProgramLocalParameter4dARB (will be remapped) */
   "iidddd\0"
   "glProgramLocalParameter4dARB\0"
   "\0"
   /* _mesa_function_pool[22907]: Color3sv (offset 18) */
   "p\0"
   "glColor3sv\0"
   "\0"
   /* _mesa_function_pool[22921]: GenFragmentShadersATI (will be remapped) */
   "i\0"
   "glGenFragmentShadersATI\0"
   "\0"
   /* _mesa_function_pool[22948]: BlendEquationSeparateiARB (will be remapped) */
   "iii\0"
   "glBlendEquationSeparateiARB\0"
   "glBlendEquationSeparateIndexedAMD\0"
   "glBlendEquationSeparatei\0"
   "\0"
   /* _mesa_function_pool[23040]: TestFenceNV (dynamic) */
   "i\0"
   "glTestFenceNV\0"
   "\0"
   /* _mesa_function_pool[23057]: MultiTexCoord1fvARB (offset 379) */
   "ip\0"
   "glMultiTexCoord1fv\0"
   "glMultiTexCoord1fvARB\0"
   "\0"
   /* _mesa_function_pool[23102]: TexStorage2D (will be remapped) */
   "iiiii\0"
   "glTexStorage2D\0"
   "\0"
   /* _mesa_function_pool[23124]: GetPixelTexGenParameterivSGIS (dynamic) */
   "ip\0"
   "glGetPixelTexGenParameterivSGIS\0"
   "\0"
   /* _mesa_function_pool[23160]: FramebufferTexture2D (will be remapped) */
   "iiiii\0"
   "glFramebufferTexture2D\0"
   "glFramebufferTexture2DEXT\0"
   "glFramebufferTexture2DOES\0"
   "\0"
   /* _mesa_function_pool[23242]: GetSamplerParameterfv (will be remapped) */
   "iip\0"
   "glGetSamplerParameterfv\0"
   "\0"
   /* _mesa_function_pool[23271]: VertexAttrib2dv (will be remapped) */
   "ip\0"
   "glVertexAttrib2dv\0"
   "glVertexAttrib2dvARB\0"
   "\0"
   /* _mesa_function_pool[23314]: Vertex4sv (offset 149) */
   "p\0"
   "glVertex4sv\0"
   "\0"
   /* _mesa_function_pool[23329]: GetQueryObjecti64v (will be remapped) */
   "iip\0"
   "glGetQueryObjecti64v\0"
   "glGetQueryObjecti64vEXT\0"
   "\0"
   /* _mesa_function_pool[23379]: ClampColor (will be remapped) */
   "ii\0"
   "glClampColorARB\0"
   "glClampColor\0"
   "\0"
   /* _mesa_function_pool[23412]: TextureRangeAPPLE (dynamic) */
   "iip\0"
   "glTextureRangeAPPLE\0"
   "\0"
   /* _mesa_function_pool[23437]: ConvolutionFilter1D (offset 348) */
   "iiiiip\0"
   "glConvolutionFilter1D\0"
   "glConvolutionFilter1DEXT\0"
   "\0"
   /* _mesa_function_pool[23492]: DrawElementsIndirect (will be remapped) */
   "iip\0"
   "glDrawElementsIndirect\0"
   "\0"
   /* _mesa_function_pool[23520]: WindowPos3sv (will be remapped) */
   "p\0"
   "glWindowPos3sv\0"
   "glWindowPos3svARB\0"
   "glWindowPos3svMESA\0"
   "\0"
   /* _mesa_function_pool[23575]: FragmentMaterialfvSGIX (dynamic) */
   "iip\0"
   "glFragmentMaterialfvSGIX\0"
   "\0"
   /* _mesa_function_pool[23605]: CallLists (offset 3) */
   "iip\0"
   "glCallLists\0"
   "\0"
   /* _mesa_function_pool[23622]: AlphaFunc (offset 240) */
   "if\0"
   "glAlphaFunc\0"
   "\0"
   /* _mesa_function_pool[23638]: EdgeFlag (offset 41) */
   "i\0"
   "glEdgeFlag\0"
   "\0"
   /* _mesa_function_pool[23652]: TexCoord2fNormal3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glTexCoord2fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[23690]: EdgeFlagv (offset 42) */
   "p\0"
   "glEdgeFlagv\0"
   "\0"
   /* _mesa_function_pool[23705]: DepthRangex (will be remapped) */
   "ii\0"
   "glDepthRangexOES\0"
   "glDepthRangex\0"
   "\0"
   /* _mesa_function_pool[23740]: ReplacementCodeubvSUN (dynamic) */
   "p\0"
   "glReplacementCodeubvSUN\0"
   "\0"
   /* _mesa_function_pool[23767]: VDPAUInitNV (will be remapped) */
   "pp\0"
   "glVDPAUInitNV\0"
   "\0"
   /* _mesa_function_pool[23785]: GetBufferParameteri64v (will be remapped) */
   "iip\0"
   "glGetBufferParameteri64v\0"
   "\0"
   /* _mesa_function_pool[23815]: LoadIdentityDeformationMapSGIX (dynamic) */
   "i\0"
   "glLoadIdentityDeformationMapSGIX\0"
   "\0"
   /* _mesa_function_pool[23851]: DepthRangef (will be remapped) */
   "ff\0"
   "glDepthRangef\0"
   "glDepthRangefOES\0"
   "\0"
   /* _mesa_function_pool[23886]: GetSharpenTexFuncSGIS (dynamic) */
   "ip\0"
   "glGetSharpenTexFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[23914]: ColorFragmentOp3ATI (will be remapped) */
   "iiiiiiiiiiiii\0"
   "glColorFragmentOp3ATI\0"
   "\0"
   /* _mesa_function_pool[23951]: ValidateProgram (will be remapped) */
   "i\0"
   "glValidateProgram\0"
   "glValidateProgramARB\0"
   "\0"
   /* _mesa_function_pool[23993]: VertexPointerEXT (will be remapped) */
   "iiiip\0"
   "glVertexPointerEXT\0"
   "\0"
   /* _mesa_function_pool[24019]: Scissor (offset 176) */
   "iiii\0"
   "glScissor\0"
   "\0"
   /* _mesa_function_pool[24035]: BeginTransformFeedback (will be remapped) */
   "i\0"
   "glBeginTransformFeedback\0"
   "glBeginTransformFeedbackEXT\0"
   "\0"
   /* _mesa_function_pool[24091]: TexCoord2i (offset 106) */
   "ii\0"
   "glTexCoord2i\0"
   "\0"
   /* _mesa_function_pool[24108]: Color4ui (offset 37) */
   "iiii\0"
   "glColor4ui\0"
   "\0"
   /* _mesa_function_pool[24125]: TexCoord2f (offset 104) */
   "ff\0"
   "glTexCoord2f\0"
   "\0"
   /* _mesa_function_pool[24142]: TexCoord2d (offset 102) */
   "dd\0"
   "glTexCoord2d\0"
   "\0"
   /* _mesa_function_pool[24159]: TexCoord2s (offset 108) */
   "ii\0"
   "glTexCoord2s\0"
   "\0"
   /* _mesa_function_pool[24176]: PointSizePointerOES (will be remapped) */
   "iip\0"
   "glPointSizePointerOES\0"
   "\0"
   /* _mesa_function_pool[24203]: Color4us (offset 39) */
   "iiii\0"
   "glColor4us\0"
   "\0"
   /* _mesa_function_pool[24220]: Color3bv (offset 10) */
   "p\0"
   "glColor3bv\0"
   "\0"
   /* _mesa_function_pool[24234]: PrimitiveRestartNV (will be remapped) */
   "\0"
   "glPrimitiveRestartNV\0"
   "\0"
   /* _mesa_function_pool[24257]: BindBufferOffsetEXT (will be remapped) */
   "iiii\0"
   "glBindBufferOffsetEXT\0"
   "\0"
   /* _mesa_function_pool[24285]: ProvokingVertex (will be remapped) */
   "i\0"
   "glProvokingVertexEXT\0"
   "glProvokingVertex\0"
   "\0"
   /* _mesa_function_pool[24327]: VertexAttribs4fvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs4fvNV\0"
   "\0"
   /* _mesa_function_pool[24353]: MapControlPointsNV (dynamic) */
   "iiiiiiiip\0"
   "glMapControlPointsNV\0"
   "\0"
   /* _mesa_function_pool[24385]: Vertex2i (offset 130) */
   "ii\0"
   "glVertex2i\0"
   "\0"
   /* _mesa_function_pool[24400]: HintPGI (dynamic) */
   "ii\0"
   "glHintPGI\0"
   "\0"
   /* _mesa_function_pool[24414]: InterleavedArrays (offset 317) */
   "iip\0"
   "glInterleavedArrays\0"
   "\0"
   /* _mesa_function_pool[24439]: RasterPos2fv (offset 65) */
   "p\0"
   "glRasterPos2fv\0"
   "\0"
   /* _mesa_function_pool[24457]: TexCoord1fv (offset 97) */
   "p\0"
   "glTexCoord1fv\0"
   "\0"
   /* _mesa_function_pool[24474]: PixelTransferf (offset 247) */
   "if\0"
   "glPixelTransferf\0"
   "\0"
   /* _mesa_function_pool[24495]: MultiTexCoord4dv (offset 401) */
   "ip\0"
   "glMultiTexCoord4dv\0"
   "glMultiTexCoord4dvARB\0"
   "\0"
   /* _mesa_function_pool[24540]: ProgramEnvParameter4fvARB (will be remapped) */
   "iip\0"
   "glProgramEnvParameter4fvARB\0"
   "glProgramParameter4fvNV\0"
   "\0"
   /* _mesa_function_pool[24597]: RasterPos4fv (offset 81) */
   "p\0"
   "glRasterPos4fv\0"
   "\0"
   /* _mesa_function_pool[24615]: FragmentLightModeliSGIX (dynamic) */
   "ii\0"
   "glFragmentLightModeliSGIX\0"
   "\0"
   /* _mesa_function_pool[24645]: PushMatrix (offset 298) */
   "\0"
   "glPushMatrix\0"
   "\0"
   /* _mesa_function_pool[24660]: EndList (offset 1) */
   "\0"
   "glEndList\0"
   "\0"
   /* _mesa_function_pool[24672]: DrawRangeElements (offset 338) */
   "iiiiip\0"
   "glDrawRangeElements\0"
   "glDrawRangeElementsEXT\0"
   "\0"
   /* _mesa_function_pool[24723]: GetTexGenxvOES (will be remapped) */
   "iip\0"
   "glGetTexGenxvOES\0"
   "\0"
   /* _mesa_function_pool[24745]: VertexAttribs4dvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs4dvNV\0"
   "\0"
   /* _mesa_function_pool[24771]: DrawTexfvOES (will be remapped) */
   "p\0"
   "glDrawTexfvOES\0"
   "\0"
   /* _mesa_function_pool[24789]: BlendFunciARB (will be remapped) */
   "iii\0"
   "glBlendFunciARB\0"
   "glBlendFuncIndexedAMD\0"
   "glBlendFunci\0"
   "\0"
   /* _mesa_function_pool[24845]: GlobalAlphaFactorbSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactorbSUN\0"
   "\0"
   /* _mesa_function_pool[24872]: Uniform2ui (will be remapped) */
   "iii\0"
   "glUniform2uiEXT\0"
   "glUniform2ui\0"
   "\0"
   /* _mesa_function_pool[24906]: ScissorIndexed (will be remapped) */
   "iiiii\0"
   "glScissorIndexed\0"
   "\0"
   /* _mesa_function_pool[24930]: End (offset 43) */
   "\0"
   "glEnd\0"
   "\0"
   /* _mesa_function_pool[24938]: BindVertexBuffers (will be remapped) */
   "iippp\0"
   "glBindVertexBuffers\0"
   "\0"
   /* _mesa_function_pool[24965]: GetSamplerParameteriv (will be remapped) */
   "iip\0"
   "glGetSamplerParameteriv\0"
   "\0"
   /* _mesa_function_pool[24994]: GenProgramPipelines (will be remapped) */
   "ip\0"
   "glGenProgramPipelines\0"
   "glGenProgramPipelinesEXT\0"
   "\0"
   /* _mesa_function_pool[25045]: Enable (offset 215) */
   "i\0"
   "glEnable\0"
   "\0"
   /* _mesa_function_pool[25057]: IsProgramPipeline (will be remapped) */
   "i\0"
   "glIsProgramPipeline\0"
   "glIsProgramPipelineEXT\0"
   "\0"
   /* _mesa_function_pool[25103]: ShaderBinary (will be remapped) */
   "ipipi\0"
   "glShaderBinary\0"
   "\0"
   /* _mesa_function_pool[25125]: GetFragmentMaterialivSGIX (dynamic) */
   "iip\0"
   "glGetFragmentMaterialivSGIX\0"
   "\0"
   /* _mesa_function_pool[25158]: WeightPointerARB (dynamic) */
   "iiip\0"
   "glWeightPointerARB\0"
   "glWeightPointerOES\0"
   "\0"
   /* _mesa_function_pool[25202]: Normal3x (will be remapped) */
   "iii\0"
   "glNormal3xOES\0"
   "glNormal3x\0"
   "\0"
   /* _mesa_function_pool[25232]: VertexAttrib4fARB (will be remapped) */
   "iffff\0"
   "glVertexAttrib4f\0"
   "glVertexAttrib4fARB\0"
   "\0"
   /* _mesa_function_pool[25276]: ReadnPixelsARB (will be remapped) */
   "iiiiiiip\0"
   "glReadnPixelsARB\0"
   "\0"
   /* _mesa_function_pool[25303]: InvalidateTexSubImage (will be remapped) */
   "iiiiiiii\0"
   "glInvalidateTexSubImage\0"
   "\0"
   /* _mesa_function_pool[25337]: Normal3s (offset 60) */
   "iii\0"
   "glNormal3s\0"
   "\0"
   /* _mesa_function_pool[25353]: Materialxv (will be remapped) */
   "iip\0"
   "glMaterialxvOES\0"
   "glMaterialxv\0"
   "\0"
   /* _mesa_function_pool[25387]: Normal3i (offset 58) */
   "iii\0"
   "glNormal3i\0"
   "\0"
   /* _mesa_function_pool[25403]: ProgramNamedParameter4fvNV (will be remapped) */
   "iipp\0"
   "glProgramNamedParameter4fvNV\0"
   "\0"
   /* _mesa_function_pool[25438]: Normal3b (offset 52) */
   "iii\0"
   "glNormal3b\0"
   "\0"
   /* _mesa_function_pool[25454]: Normal3d (offset 54) */
   "ddd\0"
   "glNormal3d\0"
   "\0"
   /* _mesa_function_pool[25470]: Normal3f (offset 56) */
   "fff\0"
   "glNormal3f\0"
   "\0"
   /* _mesa_function_pool[25486]: Indexi (offset 48) */
   "i\0"
   "glIndexi\0"
   "\0"
   /* _mesa_function_pool[25498]: VertexAttribI2uiEXT (will be remapped) */
   "iii\0"
   "glVertexAttribI2uiEXT\0"
   "glVertexAttribI2ui\0"
   "\0"
   /* _mesa_function_pool[25544]: IsRenderbuffer (will be remapped) */
   "i\0"
   "glIsRenderbuffer\0"
   "glIsRenderbufferEXT\0"
   "glIsRenderbufferOES\0"
   "\0"
   /* _mesa_function_pool[25604]: NormalP3uiv (will be remapped) */
   "ip\0"
   "glNormalP3uiv\0"
   "\0"
   /* _mesa_function_pool[25622]: Indexf (offset 46) */
   "f\0"
   "glIndexf\0"
   "\0"
   /* _mesa_function_pool[25634]: Indexd (offset 44) */
   "d\0"
   "glIndexd\0"
   "\0"
   /* _mesa_function_pool[25646]: GetMaterialiv (offset 270) */
   "iip\0"
   "glGetMaterialiv\0"
   "\0"
   /* _mesa_function_pool[25667]: Indexs (offset 50) */
   "i\0"
   "glIndexs\0"
   "\0"
   /* _mesa_function_pool[25679]: MultiTexCoordP1uiv (will be remapped) */
   "iip\0"
   "glMultiTexCoordP1uiv\0"
   "\0"
   /* _mesa_function_pool[25705]: ConvolutionFilter2D (offset 349) */
   "iiiiiip\0"
   "glConvolutionFilter2D\0"
   "glConvolutionFilter2DEXT\0"
   "\0"
   /* _mesa_function_pool[25761]: Vertex2d (offset 126) */
   "dd\0"
   "glVertex2d\0"
   "\0"
   /* _mesa_function_pool[25776]: Vertex2f (offset 128) */
   "ff\0"
   "glVertex2f\0"
   "\0"
   /* _mesa_function_pool[25791]: Color4bv (offset 26) */
   "p\0"
   "glColor4bv\0"
   "\0"
   /* _mesa_function_pool[25805]: VertexAttrib2fvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib2fvNV\0"
   "\0"
   /* _mesa_function_pool[25829]: Vertex2s (offset 132) */
   "ii\0"
   "glVertex2s\0"
   "\0"
   /* _mesa_function_pool[25844]: ActiveTexture (offset 374) */
   "i\0"
   "glActiveTexture\0"
   "glActiveTextureARB\0"
   "\0"
   /* _mesa_function_pool[25882]: GlobalAlphaFactorfSUN (dynamic) */
   "f\0"
   "glGlobalAlphaFactorfSUN\0"
   "\0"
   /* _mesa_function_pool[25909]: ColorP4uiv (will be remapped) */
   "ip\0"
   "glColorP4uiv\0"
   "\0"
   /* _mesa_function_pool[25926]: DrawTexxOES (will be remapped) */
   "iiiii\0"
   "glDrawTexxOES\0"
   "\0"
   /* _mesa_function_pool[25947]: SetFenceNV (dynamic) */
   "ii\0"
   "glSetFenceNV\0"
   "\0"
   /* _mesa_function_pool[25964]: PixelTexGenParameterivSGIS (dynamic) */
   "ip\0"
   "glPixelTexGenParameterivSGIS\0"
   "\0"
   /* _mesa_function_pool[25997]: MultiTexCoordP3ui (will be remapped) */
   "iii\0"
   "glMultiTexCoordP3ui\0"
   "\0"
   /* _mesa_function_pool[26022]: GetAttribLocation (will be remapped) */
   "ip\0"
   "glGetAttribLocation\0"
   "glGetAttribLocationARB\0"
   "\0"
   /* _mesa_function_pool[26069]: GetCombinerStageParameterfvNV (dynamic) */
   "iip\0"
   "glGetCombinerStageParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[26106]: DrawBuffer (offset 202) */
   "i\0"
   "glDrawBuffer\0"
   "\0"
   /* _mesa_function_pool[26122]: MultiTexCoord2dv (offset 385) */
   "ip\0"
   "glMultiTexCoord2dv\0"
   "glMultiTexCoord2dvARB\0"
   "\0"
   /* _mesa_function_pool[26167]: IsSampler (will be remapped) */
   "i\0"
   "glIsSampler\0"
   "\0"
   /* _mesa_function_pool[26182]: BlendFunc (offset 241) */
   "ii\0"
   "glBlendFunc\0"
   "\0"
   /* _mesa_function_pool[26198]: LineStipple (offset 167) */
   "ii\0"
   "glLineStipple\0"
   "\0"
   /* _mesa_function_pool[26216]: Tangent3fvEXT (dynamic) */
   "p\0"
   "glTangent3fvEXT\0"
   "\0"
   /* _mesa_function_pool[26235]: ColorMaterial (offset 151) */
   "ii\0"
   "glColorMaterial\0"
   "\0"
   /* _mesa_function_pool[26255]: RasterPos3sv (offset 77) */
   "p\0"
   "glRasterPos3sv\0"
   "\0"
   /* _mesa_function_pool[26273]: TexCoordP2ui (will be remapped) */
   "ii\0"
   "glTexCoordP2ui\0"
   "\0"
   /* _mesa_function_pool[26292]: TexParameteriv (offset 181) */
   "iip\0"
   "glTexParameteriv\0"
   "\0"
   /* _mesa_function_pool[26314]: VertexAttrib3fvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib3fv\0"
   "glVertexAttrib3fvARB\0"
   "\0"
   /* _mesa_function_pool[26357]: ProgramUniformMatrix3x4fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix3x4fv\0"
   "glProgramUniformMatrix3x4fvEXT\0"
   "\0"
   /* _mesa_function_pool[26423]: PixelTransformParameterfEXT (dynamic) */
   "iif\0"
   "glPixelTransformParameterfEXT\0"
   "\0"
   /* _mesa_function_pool[26458]: TextureColorMaskSGIS (dynamic) */
   "iiii\0"
   "glTextureColorMaskSGIS\0"
   "\0"
   /* _mesa_function_pool[26487]: GetColorTable (offset 343) */
   "iiip\0"
   "glGetColorTable\0"
   "glGetColorTableSGI\0"
   "glGetColorTableEXT\0"
   "\0"
   /* _mesa_function_pool[26547]: TexCoord3i (offset 114) */
   "iii\0"
   "glTexCoord3i\0"
   "\0"
   /* _mesa_function_pool[26565]: CopyColorTable (offset 342) */
   "iiiii\0"
   "glCopyColorTable\0"
   "glCopyColorTableSGI\0"
   "\0"
   /* _mesa_function_pool[26609]: Frustum (offset 289) */
   "dddddd\0"
   "glFrustum\0"
   "\0"
   /* _mesa_function_pool[26627]: TexCoord3d (offset 110) */
   "ddd\0"
   "glTexCoord3d\0"
   "\0"
   /* _mesa_function_pool[26645]: TexCoord3f (offset 112) */
   "fff\0"
   "glTexCoord3f\0"
   "\0"
   /* _mesa_function_pool[26663]: DepthRangeArrayv (will be remapped) */
   "iip\0"
   "glDepthRangeArrayv\0"
   "\0"
   /* _mesa_function_pool[26687]: DeleteTextures (offset 327) */
   "ip\0"
   "glDeleteTextures\0"
   "glDeleteTexturesEXT\0"
   "\0"
   /* _mesa_function_pool[26728]: TexCoordPointerEXT (will be remapped) */
   "iiiip\0"
   "glTexCoordPointerEXT\0"
   "\0"
   /* _mesa_function_pool[26756]: TexCoord3s (offset 116) */
   "iii\0"
   "glTexCoord3s\0"
   "\0"
   /* _mesa_function_pool[26774]: TexCoord4fVertex4fSUN (dynamic) */
   "ffffffff\0"
   "glTexCoord4fVertex4fSUN\0"
   "\0"
   /* _mesa_function_pool[26808]: CombinerStageParameterfvNV (dynamic) */
   "iip\0"
   "glCombinerStageParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[26842]: GenPerfMonitorsAMD (will be remapped) */
   "ip\0"
   "glGenPerfMonitorsAMD\0"
   "\0"
   /* _mesa_function_pool[26867]: ClearAccum (offset 204) */
   "ffff\0"
   "glClearAccum\0"
   "\0"
   /* _mesa_function_pool[26886]: DeformSGIX (dynamic) */
   "i\0"
   "glDeformSGIX\0"
   "\0"
   /* _mesa_function_pool[26902]: TexCoord4iv (offset 123) */
   "p\0"
   "glTexCoord4iv\0"
   "\0"
   /* _mesa_function_pool[26919]: TexStorage3D (will be remapped) */
   "iiiiii\0"
   "glTexStorage3D\0"
   "\0"
   /* _mesa_function_pool[26942]: FramebufferTexture3D (will be remapped) */
   "iiiiii\0"
   "glFramebufferTexture3D\0"
   "glFramebufferTexture3DEXT\0"
   "glFramebufferTexture3DOES\0"
   "\0"
   /* _mesa_function_pool[27025]: FragmentLightModelfvSGIX (dynamic) */
   "ip\0"
   "glFragmentLightModelfvSGIX\0"
   "\0"
   /* _mesa_function_pool[27056]: GetBufferParameteriv (will be remapped) */
   "iip\0"
   "glGetBufferParameteriv\0"
   "glGetBufferParameterivARB\0"
   "\0"
   /* _mesa_function_pool[27110]: VertexAttrib2fNV (will be remapped) */
   "iff\0"
   "glVertexAttrib2fNV\0"
   "\0"
   /* _mesa_function_pool[27134]: GetFragmentLightfvSGIX (dynamic) */
   "iip\0"
   "glGetFragmentLightfvSGIX\0"
   "\0"
   /* _mesa_function_pool[27164]: CopyTexImage2D (offset 324) */
   "iiiiiiii\0"
   "glCopyTexImage2D\0"
   "glCopyTexImage2DEXT\0"
   "\0"
   /* _mesa_function_pool[27211]: Vertex3fv (offset 137) */
   "p\0"
   "glVertex3fv\0"
   "\0"
   /* _mesa_function_pool[27226]: WindowPos4dvMESA (will be remapped) */
   "p\0"
   "glWindowPos4dvMESA\0"
   "\0"
   /* _mesa_function_pool[27248]: CreateShaderProgramEXT (will be remapped) */
   "ip\0"
   "glCreateShaderProgramEXT\0"
   "\0"
   /* _mesa_function_pool[27277]: VertexAttribs1dvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs1dvNV\0"
   "\0"
   /* _mesa_function_pool[27303]: IsQuery (will be remapped) */
   "i\0"
   "glIsQuery\0"
   "glIsQueryARB\0"
   "\0"
   /* _mesa_function_pool[27329]: EdgeFlagPointerEXT (will be remapped) */
   "iip\0"
   "glEdgeFlagPointerEXT\0"
   "\0"
   /* _mesa_function_pool[27355]: VertexAttribs2svNV (will be remapped) */
   "iip\0"
   "glVertexAttribs2svNV\0"
   "\0"
   /* _mesa_function_pool[27381]: CreateShaderProgramv (will be remapped) */
   "iip\0"
   "glCreateShaderProgramv\0"
   "glCreateShaderProgramvEXT\0"
   "\0"
   /* _mesa_function_pool[27435]: BlendEquationiARB (will be remapped) */
   "ii\0"
   "glBlendEquationiARB\0"
   "glBlendEquationIndexedAMD\0"
   "glBlendEquationi\0"
   "\0"
   /* _mesa_function_pool[27502]: VertexAttribI4uivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI4uivEXT\0"
   "glVertexAttribI4uiv\0"
   "\0"
   /* _mesa_function_pool[27549]: PointSizex (will be remapped) */
   "i\0"
   "glPointSizexOES\0"
   "glPointSizex\0"
   "\0"
   /* _mesa_function_pool[27581]: PolygonMode (offset 174) */
   "ii\0"
   "glPolygonMode\0"
   "\0"
   /* _mesa_function_pool[27599]: SecondaryColor3iv (will be remapped) */
   "p\0"
   "glSecondaryColor3iv\0"
   "glSecondaryColor3ivEXT\0"
   "\0"
   /* _mesa_function_pool[27645]: VertexAttribI1iEXT (will be remapped) */
   "ii\0"
   "glVertexAttribI1iEXT\0"
   "glVertexAttribI1i\0"
   "\0"
   /* _mesa_function_pool[27688]: VertexAttrib4Niv (will be remapped) */
   "ip\0"
   "glVertexAttrib4Niv\0"
   "glVertexAttrib4NivARB\0"
   "\0"
   /* _mesa_function_pool[27733]: GetMapAttribParameterivNV (dynamic) */
   "iiip\0"
   "glGetMapAttribParameterivNV\0"
   "\0"
   /* _mesa_function_pool[27767]: GetnUniformdvARB (will be remapped) */
   "iiip\0"
   "glGetnUniformdvARB\0"
   "\0"
   /* _mesa_function_pool[27792]: LinkProgram (will be remapped) */
   "i\0"
   "glLinkProgram\0"
   "glLinkProgramARB\0"
   "\0"
   /* _mesa_function_pool[27826]: ProgramUniform4f (will be remapped) */
   "iiffff\0"
   "glProgramUniform4f\0"
   "glProgramUniform4fEXT\0"
   "\0"
   /* _mesa_function_pool[27875]: ProgramUniform4i (will be remapped) */
   "iiiiii\0"
   "glProgramUniform4i\0"
   "glProgramUniform4iEXT\0"
   "\0"
   /* _mesa_function_pool[27924]: ListParameterfvSGIX (dynamic) */
   "iip\0"
   "glListParameterfvSGIX\0"
   "\0"
   /* _mesa_function_pool[27951]: VertexAttrib4d (will be remapped) */
   "idddd\0"
   "glVertexAttrib4d\0"
   "glVertexAttrib4dARB\0"
   "\0"
   /* _mesa_function_pool[27995]: WindowPos4sMESA (will be remapped) */
   "iiii\0"
   "glWindowPos4sMESA\0"
   "\0"
   /* _mesa_function_pool[28019]: VertexAttrib4s (will be remapped) */
   "iiiii\0"
   "glVertexAttrib4s\0"
   "glVertexAttrib4sARB\0"
   "\0"
   /* _mesa_function_pool[28063]: VertexAttrib1dvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib1dvNV\0"
   "\0"
   /* _mesa_function_pool[28087]: ReplacementCodePointerSUN (dynamic) */
   "iip\0"
   "glReplacementCodePointerSUN\0"
   "\0"
   /* _mesa_function_pool[28120]: TexStorage3DMultisample (will be remapped) */
   "iiiiiii\0"
   "glTexStorage3DMultisample\0"
   "\0"
   /* _mesa_function_pool[28155]: Binormal3bvEXT (dynamic) */
   "p\0"
   "glBinormal3bvEXT\0"
   "\0"
   /* _mesa_function_pool[28175]: SamplerParameteriv (will be remapped) */
   "iip\0"
   "glSamplerParameteriv\0"
   "\0"
   /* _mesa_function_pool[28201]: VertexAttribP3uiv (will be remapped) */
   "iiip\0"
   "glVertexAttribP3uiv\0"
   "\0"
   /* _mesa_function_pool[28227]: ScissorIndexedv (will be remapped) */
   "ip\0"
   "glScissorIndexedv\0"
   "\0"
   /* _mesa_function_pool[28249]: Color4ubVertex2fSUN (dynamic) */
   "iiiiff\0"
   "glColor4ubVertex2fSUN\0"
   "\0"
   /* _mesa_function_pool[28279]: FragmentColorMaterialSGIX (dynamic) */
   "ii\0"
   "glFragmentColorMaterialSGIX\0"
   "\0"
   /* _mesa_function_pool[28311]: GetStringi (will be remapped) */
   "ii\0"
   "glGetStringi\0"
   "\0"
   /* _mesa_function_pool[28328]: VertexAttrib4dv (will be remapped) */
   "ip\0"
   "glVertexAttrib4dv\0"
   "glVertexAttrib4dvARB\0"
   "\0"
   /* _mesa_function_pool[28371]: EvalCoord2dv (offset 233) */
   "p\0"
   "glEvalCoord2dv\0"
   "\0"
   /* _mesa_function_pool[28389]: VertexAttrib1fNV (will be remapped) */
   "if\0"
   "glVertexAttrib1fNV\0"
   "\0"
   /* _mesa_function_pool[28412]: CompressedTexSubImage1D (will be remapped) */
   "iiiiiip\0"
   "glCompressedTexSubImage1D\0"
   "glCompressedTexSubImage1DARB\0"
   "\0"
   /* _mesa_function_pool[28476]: GetSeparableFilter (offset 359) */
   "iiippp\0"
   "glGetSeparableFilter\0"
   "glGetSeparableFilterEXT\0"
   "\0"
   /* _mesa_function_pool[28529]: ReplacementCodeusSUN (dynamic) */
   "i\0"
   "glReplacementCodeusSUN\0"
   "\0"
   /* _mesa_function_pool[28555]: FeedbackBuffer (offset 194) */
   "iip\0"
   "glFeedbackBuffer\0"
   "\0"
   /* _mesa_function_pool[28577]: RasterPos2iv (offset 67) */
   "p\0"
   "glRasterPos2iv\0"
   "\0"
   /* _mesa_function_pool[28595]: TexImage1D (offset 182) */
   "iiiiiiip\0"
   "glTexImage1D\0"
   "\0"
   /* _mesa_function_pool[28618]: MultiDrawElementsEXT (will be remapped) */
   "ipipi\0"
   "glMultiDrawElements\0"
   "glMultiDrawElementsEXT\0"
   "\0"
   /* _mesa_function_pool[28668]: GetnSeparableFilterARB (will be remapped) */
   "iiiipipp\0"
   "glGetnSeparableFilterARB\0"
   "\0"
   /* _mesa_function_pool[28703]: FrontFace (offset 157) */
   "i\0"
   "glFrontFace\0"
   "\0"
   /* _mesa_function_pool[28718]: MultiModeDrawArraysIBM (will be remapped) */
   "pppii\0"
   "glMultiModeDrawArraysIBM\0"
   "\0"
   /* _mesa_function_pool[28750]: Tangent3ivEXT (dynamic) */
   "p\0"
   "glTangent3ivEXT\0"
   "\0"
   /* _mesa_function_pool[28769]: LightEnviSGIX (dynamic) */
   "ii\0"
   "glLightEnviSGIX\0"
   "\0"
   /* _mesa_function_pool[28789]: Normal3dv (offset 55) */
   "p\0"
   "glNormal3dv\0"
   "\0"
   /* _mesa_function_pool[28804]: Lightf (offset 159) */
   "iif\0"
   "glLightf\0"
   "\0"
   /* _mesa_function_pool[28818]: MatrixMode (offset 293) */
   "i\0"
   "glMatrixMode\0"
   "\0"
   /* _mesa_function_pool[28834]: GetPixelMapusv (offset 273) */
   "ip\0"
   "glGetPixelMapusv\0"
   "\0"
   /* _mesa_function_pool[28855]: Lighti (offset 161) */
   "iii\0"
   "glLighti\0"
   "\0"
   /* _mesa_function_pool[28869]: VertexAttribPointerNV (will be remapped) */
   "iiiip\0"
   "glVertexAttribPointerNV\0"
   "\0"
   /* _mesa_function_pool[28900]: GetFragDataIndex (will be remapped) */
   "ip\0"
   "glGetFragDataIndex\0"
   "\0"
   /* _mesa_function_pool[28923]: Lightx (will be remapped) */
   "iii\0"
   "glLightxOES\0"
   "glLightx\0"
   "\0"
   /* _mesa_function_pool[28949]: ProgramUniform3fv (will be remapped) */
   "iiip\0"
   "glProgramUniform3fv\0"
   "glProgramUniform3fvEXT\0"
   "\0"
   /* _mesa_function_pool[28998]: MultMatrixd (offset 295) */
   "p\0"
   "glMultMatrixd\0"
   "\0"
   /* _mesa_function_pool[29015]: MultMatrixf (offset 294) */
   "p\0"
   "glMultMatrixf\0"
   "\0"
   /* _mesa_function_pool[29032]: MultiTexCoord4fvARB (offset 403) */
   "ip\0"
   "glMultiTexCoord4fv\0"
   "glMultiTexCoord4fvARB\0"
   "\0"
   /* _mesa_function_pool[29077]: UniformMatrix2x3fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix2x3fv\0"
   "\0"
   /* _mesa_function_pool[29104]: TrackMatrixNV (will be remapped) */
   "iiii\0"
   "glTrackMatrixNV\0"
   "\0"
   /* _mesa_function_pool[29126]: MultMatrixx (will be remapped) */
   "p\0"
   "glMultMatrixxOES\0"
   "glMultMatrixx\0"
   "\0"
   /* _mesa_function_pool[29160]: PointParameterx (will be remapped) */
   "ii\0"
   "glPointParameterxOES\0"
   "glPointParameterx\0"
   "\0"
   /* _mesa_function_pool[29203]: DrawArrays (offset 310) */
   "iii\0"
   "glDrawArrays\0"
   "glDrawArraysEXT\0"
   "\0"
   /* _mesa_function_pool[29237]: PointParameteri (will be remapped) */
   "ii\0"
   "glPointParameteri\0"
   "glPointParameteriNV\0"
   "\0"
   /* _mesa_function_pool[29279]: PointParameterf (will be remapped) */
   "if\0"
   "glPointParameterf\0"
   "glPointParameterfARB\0"
   "glPointParameterfEXT\0"
   "glPointParameterfSGIS\0"
   "\0"
   /* _mesa_function_pool[29365]: GlobalAlphaFactorsSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactorsSUN\0"
   "\0"
   /* _mesa_function_pool[29392]: VertexAttribBinding (will be remapped) */
   "ii\0"
   "glVertexAttribBinding\0"
   "\0"
   /* _mesa_function_pool[29418]: ReplacementCodeuiTexCoord2fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glReplacementCodeuiTexCoord2fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[29465]: CreateShader (will be remapped) */
   "i\0"
   "glCreateShader\0"
   "\0"
   /* _mesa_function_pool[29483]: GetProgramParameterdvNV (will be remapped) */
   "iiip\0"
   "glGetProgramParameterdvNV\0"
   "\0"
   /* _mesa_function_pool[29515]: GetProgramEnvParameterfvARB (will be remapped) */
   "iip\0"
   "glGetProgramEnvParameterfvARB\0"
   "\0"
   /* _mesa_function_pool[29550]: DeleteBuffers (will be remapped) */
   "ip\0"
   "glDeleteBuffers\0"
   "glDeleteBuffersARB\0"
   "\0"
   /* _mesa_function_pool[29589]: GetBufferSubData (will be remapped) */
   "iiip\0"
   "glGetBufferSubData\0"
   "glGetBufferSubDataARB\0"
   "\0"
   /* _mesa_function_pool[29636]: GetPerfMonitorGroupsAMD (will be remapped) */
   "pip\0"
   "glGetPerfMonitorGroupsAMD\0"
   "\0"
   /* _mesa_function_pool[29667]: FlushRasterSGIX (dynamic) */
   "\0"
   "glFlushRasterSGIX\0"
   "\0"
   /* _mesa_function_pool[29687]: VertexAttribP2ui (will be remapped) */
   "iiii\0"
   "glVertexAttribP2ui\0"
   "\0"
   /* _mesa_function_pool[29712]: GetMinmaxParameteriv (offset 366) */
   "iip\0"
   "glGetMinmaxParameteriv\0"
   "glGetMinmaxParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[29766]: DrawTexivOES (will be remapped) */
   "p\0"
   "glDrawTexivOES\0"
   "\0"
   /* _mesa_function_pool[29784]: CopyTexImage1D (offset 323) */
   "iiiiiii\0"
   "glCopyTexImage1D\0"
   "glCopyTexImage1DEXT\0"
   "\0"
   /* _mesa_function_pool[29830]: GetnColorTableARB (will be remapped) */
   "iiiip\0"
   "glGetnColorTableARB\0"
   "\0"
   /* _mesa_function_pool[29857]: VertexAttribFormat (will be remapped) */
   "iiiii\0"
   "glVertexAttribFormat\0"
   "\0"
   /* _mesa_function_pool[29885]: Vertex3i (offset 138) */
   "iii\0"
   "glVertex3i\0"
   "\0"
   /* _mesa_function_pool[29901]: Vertex3f (offset 136) */
   "fff\0"
   "glVertex3f\0"
   "\0"
   /* _mesa_function_pool[29917]: Vertex3d (offset 134) */
   "ddd\0"
   "glVertex3d\0"
   "\0"
   /* _mesa_function_pool[29933]: GetProgramPipelineiv (will be remapped) */
   "iip\0"
   "glGetProgramPipelineiv\0"
   "glGetProgramPipelineivEXT\0"
   "\0"
   /* _mesa_function_pool[29987]: ReadBuffer (offset 254) */
   "i\0"
   "glReadBuffer\0"
   "glReadBufferNV\0"
   "\0"
   /* _mesa_function_pool[30018]: ConvolutionParameteri (offset 352) */
   "iii\0"
   "glConvolutionParameteri\0"
   "glConvolutionParameteriEXT\0"
   "\0"
   /* _mesa_function_pool[30074]: GetTexParameterIiv (will be remapped) */
   "iip\0"
   "glGetTexParameterIivEXT\0"
   "glGetTexParameterIiv\0"
   "\0"
   /* _mesa_function_pool[30124]: Vertex3s (offset 140) */
   "iii\0"
   "glVertex3s\0"
   "\0"
   /* _mesa_function_pool[30140]: ConvolutionParameterf (offset 350) */
   "iif\0"
   "glConvolutionParameterf\0"
   "glConvolutionParameterfEXT\0"
   "\0"
   /* _mesa_function_pool[30196]: GetColorTableParameteriv (offset 345) */
   "iip\0"
   "glGetColorTableParameteriv\0"
   "glGetColorTableParameterivSGI\0"
   "glGetColorTableParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[30288]: GetTransformFeedbackVarying (will be remapped) */
   "iiipppp\0"
   "glGetTransformFeedbackVarying\0"
   "glGetTransformFeedbackVaryingEXT\0"
   "\0"
   /* _mesa_function_pool[30360]: GetNextPerfQueryIdINTEL (will be remapped) */
   "ip\0"
   "glGetNextPerfQueryIdINTEL\0"
   "\0"
   /* _mesa_function_pool[30390]: TexCoord3fv (offset 113) */
   "p\0"
   "glTexCoord3fv\0"
   "\0"
   /* _mesa_function_pool[30407]: TextureBarrierNV (will be remapped) */
   "\0"
   "glTextureBarrier\0"
   "glTextureBarrierNV\0"
   "\0"
   /* _mesa_function_pool[30445]: ReplacementCodeuiColor4fNormal3fVertex3fSUN (dynamic) */
   "iffffffffff\0"
   "glReplacementCodeuiColor4fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[30504]: ProgramLocalParameter4fARB (will be remapped) */
   "iiffff\0"
   "glProgramLocalParameter4fARB\0"
   "\0"
   /* _mesa_function_pool[30541]: ObjectLabel (will be remapped) */
   "iiip\0"
   "glObjectLabel\0"
   "\0"
   /* _mesa_function_pool[30561]: PauseTransformFeedback (will be remapped) */
   "\0"
   "glPauseTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[30588]: DeleteShader (will be remapped) */
   "i\0"
   "glDeleteShader\0"
   "\0"
   /* _mesa_function_pool[30606]: CompileShader (will be remapped) */
   "i\0"
   "glCompileShader\0"
   "glCompileShaderARB\0"
   "\0"
   /* _mesa_function_pool[30644]: Vertex2iv (offset 131) */
   "p\0"
   "glVertex2iv\0"
   "\0"
   /* _mesa_function_pool[30659]: TexGendv (offset 189) */
   "iip\0"
   "glTexGendv\0"
   "\0"
   /* _mesa_function_pool[30675]: ProgramLocalParameters4fvEXT (will be remapped) */
   "iiip\0"
   "glProgramLocalParameters4fvEXT\0"
   "\0"
   /* _mesa_function_pool[30712]: ResetMinmax (offset 370) */
   "i\0"
   "glResetMinmax\0"
   "glResetMinmaxEXT\0"
   "\0"
   /* _mesa_function_pool[30746]: SpriteParameterfSGIX (dynamic) */
   "if\0"
   "glSpriteParameterfSGIX\0"
   "\0"
   /* _mesa_function_pool[30773]: DeleteProgramsARB (will be remapped) */
   "ip\0"
   "glDeleteProgramsARB\0"
   "glDeleteProgramsNV\0"
   "\0"
   /* _mesa_function_pool[30816]: ShadeModel (offset 177) */
   "i\0"
   "glShadeModel\0"
   "\0"
   /* _mesa_function_pool[30832]: VertexAttribs1fvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs1fvNV\0"
   "\0"
   /* _mesa_function_pool[30858]: FogFuncSGIS (dynamic) */
   "ip\0"
   "glFogFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[30876]: GetTexLevelParameteriv (offset 285) */
   "iiip\0"
   "glGetTexLevelParameteriv\0"
   "\0"
   /* _mesa_function_pool[30907]: MultiDrawArrays (will be remapped) */
   "ippi\0"
   "glMultiDrawArrays\0"
   "glMultiDrawArraysEXT\0"
   "\0"
   /* _mesa_function_pool[30952]: GetProgramLocalParameterdvARB (will be remapped) */
   "iip\0"
   "glGetProgramLocalParameterdvARB\0"
   "\0"
   /* _mesa_function_pool[30989]: MapBufferRange (will be remapped) */
   "iiii\0"
   "glMapBufferRange\0"
   "glMapBufferRangeEXT\0"
   "\0"
   /* _mesa_function_pool[31032]: DispatchCompute (will be remapped) */
   "iii\0"
   "glDispatchCompute\0"
   "\0"
   /* _mesa_function_pool[31055]: UseProgramStages (will be remapped) */
   "iii\0"
   "glUseProgramStages\0"
   "glUseProgramStagesEXT\0"
   "\0"
   /* _mesa_function_pool[31101]: ProgramUniformMatrix4fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix4fv\0"
   "glProgramUniformMatrix4fvEXT\0"
   "\0"
   /* _mesa_function_pool[31163]: FinishAsyncSGIX (dynamic) */
   "p\0"
   "glFinishAsyncSGIX\0"
   "\0"
   /* _mesa_function_pool[31184]: FramebufferRenderbuffer (will be remapped) */
   "iiii\0"
   "glFramebufferRenderbuffer\0"
   "glFramebufferRenderbufferEXT\0"
   "glFramebufferRenderbufferOES\0"
   "\0"
   /* _mesa_function_pool[31274]: IsProgramARB (will be remapped) */
   "i\0"
   "glIsProgramARB\0"
   "glIsProgramNV\0"
   "\0"
   /* _mesa_function_pool[31306]: Map2d (offset 222) */
   "iddiiddiip\0"
   "glMap2d\0"
   "\0"
   /* _mesa_function_pool[31326]: Map2f (offset 223) */
   "iffiiffiip\0"
   "glMap2f\0"
   "\0"
   /* _mesa_function_pool[31346]: ProgramStringARB (will be remapped) */
   "iiip\0"
   "glProgramStringARB\0"
   "\0"
   /* _mesa_function_pool[31371]: MultiTexCoord4x (will be remapped) */
   "iiiii\0"
   "glMultiTexCoord4xOES\0"
   "glMultiTexCoord4x\0"
   "\0"
   /* _mesa_function_pool[31417]: MultiTexCoord4s (offset 406) */
   "iiiii\0"
   "glMultiTexCoord4s\0"
   "glMultiTexCoord4sARB\0"
   "\0"
   /* _mesa_function_pool[31463]: ViewportIndexedf (will be remapped) */
   "iffff\0"
   "glViewportIndexedf\0"
   "\0"
   /* _mesa_function_pool[31489]: MultiTexCoord4i (offset 404) */
   "iiiii\0"
   "glMultiTexCoord4i\0"
   "glMultiTexCoord4iARB\0"
   "\0"
   /* _mesa_function_pool[31535]: ApplyTextureEXT (dynamic) */
   "i\0"
   "glApplyTextureEXT\0"
   "\0"
   /* _mesa_function_pool[31556]: DebugMessageControl (will be remapped) */
   "iiiipi\0"
   "glDebugMessageControlARB\0"
   "glDebugMessageControl\0"
   "\0"
   /* _mesa_function_pool[31611]: MultiTexCoord4d (offset 400) */
   "idddd\0"
   "glMultiTexCoord4d\0"
   "glMultiTexCoord4dARB\0"
   "\0"
   /* _mesa_function_pool[31657]: GetHistogram (offset 361) */
   "iiiip\0"
   "glGetHistogram\0"
   "glGetHistogramEXT\0"
   "\0"
   /* _mesa_function_pool[31697]: Translatex (will be remapped) */
   "iii\0"
   "glTranslatexOES\0"
   "glTranslatex\0"
   "\0"
   /* _mesa_function_pool[31731]: IglooInterfaceSGIX (dynamic) */
   "ip\0"
   "glIglooInterfaceSGIX\0"
   "\0"
   /* _mesa_function_pool[31756]: Indexsv (offset 51) */
   "p\0"
   "glIndexsv\0"
   "\0"
   /* _mesa_function_pool[31769]: VertexAttrib1fvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib1fv\0"
   "glVertexAttrib1fvARB\0"
   "\0"
   /* _mesa_function_pool[31812]: TexCoord2dv (offset 103) */
   "p\0"
   "glTexCoord2dv\0"
   "\0"
   /* _mesa_function_pool[31829]: Translated (offset 303) */
   "ddd\0"
   "glTranslated\0"
   "\0"
   /* _mesa_function_pool[31847]: Translatef (offset 304) */
   "fff\0"
   "glTranslatef\0"
   "\0"
   /* _mesa_function_pool[31865]: MultTransposeMatrixd (will be remapped) */
   "p\0"
   "glMultTransposeMatrixd\0"
   "glMultTransposeMatrixdARB\0"
   "\0"
   /* _mesa_function_pool[31917]: ProgramUniform4uiv (will be remapped) */
   "iiip\0"
   "glProgramUniform4uiv\0"
   "glProgramUniform4uivEXT\0"
   "\0"
   /* _mesa_function_pool[31968]: GetPerfCounterInfoINTEL (will be remapped) */
   "iiipipppppp\0"
   "glGetPerfCounterInfoINTEL\0"
   "\0"
   /* _mesa_function_pool[32007]: RenderMode (offset 196) */
   "i\0"
   "glRenderMode\0"
   "\0"
   /* _mesa_function_pool[32023]: MultiTexCoord1fARB (offset 378) */
   "if\0"
   "glMultiTexCoord1f\0"
   "glMultiTexCoord1fARB\0"
   "\0"
   /* _mesa_function_pool[32066]: SecondaryColor3d (will be remapped) */
   "ddd\0"
   "glSecondaryColor3d\0"
   "glSecondaryColor3dEXT\0"
   "\0"
   /* _mesa_function_pool[32112]: VertexAttribs4ubvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs4ubvNV\0"
   "\0"
   /* _mesa_function_pool[32139]: WeightsvARB (dynamic) */
   "ip\0"
   "glWeightsvARB\0"
   "\0"
   /* _mesa_function_pool[32157]: ProgramUniform4fv (will be remapped) */
   "iiip\0"
   "glProgramUniform4fv\0"
   "glProgramUniform4fvEXT\0"
   "\0"
   /* _mesa_function_pool[32206]: LightModelxv (will be remapped) */
   "ip\0"
   "glLightModelxvOES\0"
   "glLightModelxv\0"
   "\0"
   /* _mesa_function_pool[32243]: CopyTexSubImage1D (offset 325) */
   "iiiiii\0"
   "glCopyTexSubImage1D\0"
   "glCopyTexSubImage1DEXT\0"
   "\0"
   /* _mesa_function_pool[32294]: PushDebugGroup (will be remapped) */
   "iiip\0"
   "glPushDebugGroup\0"
   "\0"
   /* _mesa_function_pool[32317]: StencilFunc (offset 243) */
   "iii\0"
   "glStencilFunc\0"
   "\0"
   /* _mesa_function_pool[32336]: CopyPixels (offset 255) */
   "iiiii\0"
   "glCopyPixels\0"
   "\0"
   /* _mesa_function_pool[32356]: TexGenxvOES (will be remapped) */
   "iip\0"
   "glTexGenxvOES\0"
   "\0"
   /* _mesa_function_pool[32375]: VertexAttrib4Nubv (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nubv\0"
   "glVertexAttrib4NubvARB\0"
   "\0"
   /* _mesa_function_pool[32422]: GetFogFuncSGIS (dynamic) */
   "p\0"
   "glGetFogFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[32442]: VertexP2uiv (will be remapped) */
   "ip\0"
   "glVertexP2uiv\0"
   "\0"
   /* _mesa_function_pool[32460]: VertexAttribPointer (will be remapped) */
   "iiiiip\0"
   "glVertexAttribPointer\0"
   "glVertexAttribPointerARB\0"
   "\0"
   /* _mesa_function_pool[32515]: IndexMask (offset 212) */
   "i\0"
   "glIndexMask\0"
   "\0"
   /* _mesa_function_pool[32530]: SharpenTexFuncSGIS (dynamic) */
   "iip\0"
   "glSharpenTexFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[32556]: VertexAttribIFormat (will be remapped) */
   "iiii\0"
   "glVertexAttribIFormat\0"
   "\0"
   /* _mesa_function_pool[32584]: CombinerOutputNV (dynamic) */
   "iiiiiiiiii\0"
   "glCombinerOutputNV\0"
   "\0"
   /* _mesa_function_pool[32615]: DrawArraysInstancedBaseInstance (will be remapped) */
   "iiiii\0"
   "glDrawArraysInstancedBaseInstance\0"
   "\0"
   /* _mesa_function_pool[32656]: PopAttrib (offset 218) */
   "\0"
   "glPopAttrib\0"
   "\0"
   /* _mesa_function_pool[32670]: SamplePatternSGIS (will be remapped) */
   "i\0"
   "glSamplePatternSGIS\0"
   "glSamplePatternEXT\0"
   "\0"
   /* _mesa_function_pool[32712]: Uniform3ui (will be remapped) */
   "iiii\0"
   "glUniform3uiEXT\0"
   "glUniform3ui\0"
   "\0"
   /* _mesa_function_pool[32747]: DeletePerfMonitorsAMD (will be remapped) */
   "ip\0"
   "glDeletePerfMonitorsAMD\0"
   "\0"
   /* _mesa_function_pool[32775]: Color4dv (offset 28) */
   "p\0"
   "glColor4dv\0"
   "\0"
   /* _mesa_function_pool[32789]: AreProgramsResidentNV (will be remapped) */
   "ipp\0"
   "glAreProgramsResidentNV\0"
   "\0"
   /* _mesa_function_pool[32818]: DisableVertexAttribArray (will be remapped) */
   "i\0"
   "glDisableVertexAttribArray\0"
   "glDisableVertexAttribArrayARB\0"
   "\0"
   /* _mesa_function_pool[32878]: ProgramUniformMatrix3x2fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix3x2fv\0"
   "glProgramUniformMatrix3x2fvEXT\0"
   "\0"
   /* _mesa_function_pool[32944]: GetDoublei_v (will be remapped) */
   "iip\0"
   "glGetDoublei_v\0"
   "\0"
   /* _mesa_function_pool[32964]: IsTransformFeedback (will be remapped) */
   "i\0"
   "glIsTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[32989]: ClipPlanex (will be remapped) */
   "ip\0"
   "glClipPlanexOES\0"
   "glClipPlanex\0"
   "\0"
   /* _mesa_function_pool[33022]: ReplacementCodeuiColor3fVertex3fSUN (dynamic) */
   "iffffff\0"
   "glReplacementCodeuiColor3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[33069]: GetLightfv (offset 264) */
   "iip\0"
   "glGetLightfv\0"
   "\0"
   /* _mesa_function_pool[33087]: ClipPlanef (will be remapped) */
   "ip\0"
   "glClipPlanefOES\0"
   "glClipPlanef\0"
   "\0"
   /* _mesa_function_pool[33120]: ProgramUniform1ui (will be remapped) */
   "iii\0"
   "glProgramUniform1ui\0"
   "glProgramUniform1uiEXT\0"
   "\0"
   /* _mesa_function_pool[33168]: SecondaryColorPointer (will be remapped) */
   "iiip\0"
   "glSecondaryColorPointer\0"
   "glSecondaryColorPointerEXT\0"
   "\0"
   /* _mesa_function_pool[33225]: Tangent3svEXT (dynamic) */
   "p\0"
   "glTangent3svEXT\0"
   "\0"
   /* _mesa_function_pool[33244]: Tangent3iEXT (dynamic) */
   "iii\0"
   "glTangent3iEXT\0"
   "\0"
   /* _mesa_function_pool[33264]: Indexub (offset 315) */
   "i\0"
   "glIndexub\0"
   "\0"
   /* _mesa_function_pool[33277]: FragmentLightfSGIX (dynamic) */
   "iif\0"
   "glFragmentLightfSGIX\0"
   "\0"
   /* _mesa_function_pool[33303]: BeginFragmentShaderATI (will be remapped) */
   "\0"
   "glBeginFragmentShaderATI\0"
   "\0"
   /* _mesa_function_pool[33330]: GenRenderbuffers (will be remapped) */
   "ip\0"
   "glGenRenderbuffers\0"
   "glGenRenderbuffersEXT\0"
   "glGenRenderbuffersOES\0"
   "\0"
   /* _mesa_function_pool[33397]: GetMinmaxParameterfv (offset 365) */
   "iip\0"
   "glGetMinmaxParameterfv\0"
   "glGetMinmaxParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[33451]: IsEnabledi (will be remapped) */
   "ii\0"
   "glIsEnabledIndexedEXT\0"
   "glIsEnabledi\0"
   "\0"
   /* _mesa_function_pool[33490]: FragmentMaterialivSGIX (dynamic) */
   "iip\0"
   "glFragmentMaterialivSGIX\0"
   "\0"
   /* _mesa_function_pool[33520]: WaitSync (will be remapped) */
   "iii\0"
   "glWaitSync\0"
   "\0"
   /* _mesa_function_pool[33536]: GetVertexAttribPointerv (will be remapped) */
   "iip\0"
   "glGetVertexAttribPointerv\0"
   "glGetVertexAttribPointervARB\0"
   "glGetVertexAttribPointervNV\0"
   "\0"
   /* _mesa_function_pool[33624]: CreatePerfQueryINTEL (will be remapped) */
   "ip\0"
   "glCreatePerfQueryINTEL\0"
   "\0"
   /* _mesa_function_pool[33651]: NewList (dynamic) */
   "ii\0"
   "glNewList\0"
   "\0"
   /* _mesa_function_pool[33665]: TexBuffer (will be remapped) */
   "iii\0"
   "glTexBufferARB\0"
   "glTexBuffer\0"
   "\0"
   /* _mesa_function_pool[33697]: TexCoord4sv (offset 125) */
   "p\0"
   "glTexCoord4sv\0"
   "\0"
   /* _mesa_function_pool[33714]: TexCoord1f (offset 96) */
   "f\0"
   "glTexCoord1f\0"
   "\0"
   /* _mesa_function_pool[33730]: TexCoord1d (offset 94) */
   "d\0"
   "glTexCoord1d\0"
   "\0"
   /* _mesa_function_pool[33746]: TexCoord1i (offset 98) */
   "i\0"
   "glTexCoord1i\0"
   "\0"
   /* _mesa_function_pool[33762]: GetnUniformfvARB (will be remapped) */
   "iiip\0"
   "glGetnUniformfvARB\0"
   "\0"
   /* _mesa_function_pool[33787]: TexCoord1s (offset 100) */
   "i\0"
   "glTexCoord1s\0"
   "\0"
   /* _mesa_function_pool[33803]: GlobalAlphaFactoriSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactoriSUN\0"
   "\0"
   /* _mesa_function_pool[33830]: TexStorage1D (will be remapped) */
   "iiii\0"
   "glTexStorage1D\0"
   "\0"
   /* _mesa_function_pool[33851]: BlitFramebuffer (will be remapped) */
   "iiiiiiiiii\0"
   "glBlitFramebuffer\0"
   "glBlitFramebufferEXT\0"
   "\0"
   /* _mesa_function_pool[33902]: FramebufferTexture1D (will be remapped) */
   "iiiii\0"
   "glFramebufferTexture1D\0"
   "glFramebufferTexture1DEXT\0"
   "\0"
   /* _mesa_function_pool[33958]: GetMapiv (offset 268) */
   "iip\0"
   "glGetMapiv\0"
   "\0"
   /* _mesa_function_pool[33974]: TexCoordP4ui (will be remapped) */
   "ii\0"
   "glTexCoordP4ui\0"
   "\0"
   /* _mesa_function_pool[33993]: VertexAttrib1sv (will be remapped) */
   "ip\0"
   "glVertexAttrib1sv\0"
   "glVertexAttrib1svARB\0"
   "\0"
   /* _mesa_function_pool[34036]: WindowPos4dMESA (will be remapped) */
   "dddd\0"
   "glWindowPos4dMESA\0"
   "\0"
   /* _mesa_function_pool[34060]: Vertex3dv (offset 135) */
   "p\0"
   "glVertex3dv\0"
   "\0"
   /* _mesa_function_pool[34075]: MultiTexCoordP2ui (will be remapped) */
   "iii\0"
   "glMultiTexCoordP2ui\0"
   "\0"
   /* _mesa_function_pool[34100]: GetnMapivARB (will be remapped) */
   "iiip\0"
   "glGetnMapivARB\0"
   "\0"
   /* _mesa_function_pool[34121]: MapParameterfvNV (dynamic) */
   "iip\0"
   "glMapParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[34145]: GetVertexAttribfv (will be remapped) */
   "iip\0"
   "glGetVertexAttribfv\0"
   "glGetVertexAttribfvARB\0"
   "\0"
   /* _mesa_function_pool[34193]: MultiTexCoordP4uiv (will be remapped) */
   "iip\0"
   "glMultiTexCoordP4uiv\0"
   "\0"
   /* _mesa_function_pool[34219]: TexGeniv (offset 193) */
   "iip\0"
   "glTexGeniv\0"
   "glTexGenivOES\0"
   "\0"
   /* _mesa_function_pool[34249]: WeightubvARB (dynamic) */
   "ip\0"
   "glWeightubvARB\0"
   "\0"
   /* _mesa_function_pool[34268]: BlendColor (offset 336) */
   "ffff\0"
   "glBlendColor\0"
   "glBlendColorEXT\0"
   "\0"
   /* _mesa_function_pool[34303]: DrawBuffers (will be remapped) */
   "ip\0"
   "glDrawBuffers\0"
   "glDrawBuffersARB\0"
   "glDrawBuffersATI\0"
   "glDrawBuffersNV\0"
   "\0"
   /* _mesa_function_pool[34371]: Materiali (offset 171) */
   "iii\0"
   "glMateriali\0"
   "\0"
   /* _mesa_function_pool[34388]: VertexAttrib2dvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib2dvNV\0"
   "\0"
   /* _mesa_function_pool[34412]: ResetHistogram (offset 369) */
   "i\0"
   "glResetHistogram\0"
   "glResetHistogramEXT\0"
   "\0"
   /* _mesa_function_pool[34452]: CompressedTexSubImage2D (will be remapped) */
   "iiiiiiiip\0"
   "glCompressedTexSubImage2D\0"
   "glCompressedTexSubImage2DARB\0"
   "\0"
   /* _mesa_function_pool[34518]: TexCoord2sv (offset 109) */
   "p\0"
   "glTexCoord2sv\0"
   "\0"
   /* _mesa_function_pool[34535]: StencilMaskSeparate (will be remapped) */
   "ii\0"
   "glStencilMaskSeparate\0"
   "\0"
   /* _mesa_function_pool[34561]: MultiTexCoord3sv (offset 399) */
   "ip\0"
   "glMultiTexCoord3sv\0"
   "glMultiTexCoord3svARB\0"
   "\0"
   /* _mesa_function_pool[34606]: GetMapParameterfvNV (dynamic) */
   "iip\0"
   "glGetMapParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[34633]: TexCoord3iv (offset 115) */
   "p\0"
   "glTexCoord3iv\0"
   "\0"
   /* _mesa_function_pool[34650]: MultiTexCoord4sv (offset 407) */
   "ip\0"
   "glMultiTexCoord4sv\0"
   "glMultiTexCoord4svARB\0"
   "\0"
   /* _mesa_function_pool[34695]: VertexBindingDivisor (will be remapped) */
   "ii\0"
   "glVertexBindingDivisor\0"
   "\0"
   /* _mesa_function_pool[34722]: GetPerfMonitorCounterInfoAMD (will be remapped) */
   "iiip\0"
   "glGetPerfMonitorCounterInfoAMD\0"
   "\0"
   /* _mesa_function_pool[34759]: UniformBlockBinding (will be remapped) */
   "iii\0"
   "glUniformBlockBinding\0"
   "\0"
   /* _mesa_function_pool[34786]: FenceSync (will be remapped) */
   "ii\0"
   "glFenceSync\0"
   "\0"
   /* _mesa_function_pool[34802]: VertexAttrib4Nusv (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nusv\0"
   "glVertexAttrib4NusvARB\0"
   "\0"
   /* _mesa_function_pool[34849]: SetFragmentShaderConstantATI (will be remapped) */
   "ip\0"
   "glSetFragmentShaderConstantATI\0"
   "\0"
   /* _mesa_function_pool[34884]: VertexP2ui (will be remapped) */
   "ii\0"
   "glVertexP2ui\0"
   "\0"
   /* _mesa_function_pool[34901]: GetTexEnvfv (offset 276) */
   "iip\0"
   "glGetTexEnvfv\0"
   "\0"
   /* _mesa_function_pool[34920]: BindAttribLocation (will be remapped) */
   "iip\0"
   "glBindAttribLocation\0"
   "glBindAttribLocationARB\0"
   "\0"
   /* _mesa_function_pool[34970]: TextureStorage2DEXT (will be remapped) */
   "iiiiii\0"
   "glTextureStorage2DEXT\0"
   "\0"
   /* _mesa_function_pool[35000]: FragmentLightiSGIX (dynamic) */
   "iii\0"
   "glFragmentLightiSGIX\0"
   "\0"
   /* _mesa_function_pool[35026]: DrawTransformFeedbackInstanced (will be remapped) */
   "iii\0"
   "glDrawTransformFeedbackInstanced\0"
   "\0"
   /* _mesa_function_pool[35064]: PollAsyncSGIX (dynamic) */
   "p\0"
   "glPollAsyncSGIX\0"
   "\0"
   /* _mesa_function_pool[35083]: ResumeTransformFeedback (will be remapped) */
   "\0"
   "glResumeTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[35111]: GetProgramNamedParameterdvNV (will be remapped) */
   "iipp\0"
   "glGetProgramNamedParameterdvNV\0"
   "\0"
   /* _mesa_function_pool[35148]: VertexAttribI1iv (will be remapped) */
   "ip\0"
   "glVertexAttribI1ivEXT\0"
   "glVertexAttribI1iv\0"
   "\0"
   /* _mesa_function_pool[35193]: Vertex2dv (offset 127) */
   "p\0"
   "glVertex2dv\0"
   "\0"
   /* _mesa_function_pool[35208]: VertexAttribI2uivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI2uivEXT\0"
   "glVertexAttribI2uiv\0"
   "\0"
   /* _mesa_function_pool[35255]: SampleMaski (will be remapped) */
   "ii\0"
   "glSampleMaski\0"
   "\0"
   /* _mesa_function_pool[35273]: GetFloati_v (will be remapped) */
   "iip\0"
   "glGetFloati_v\0"
   "\0"
   /* _mesa_function_pool[35292]: MultiTexCoord2iv (offset 389) */
   "ip\0"
   "glMultiTexCoord2iv\0"
   "glMultiTexCoord2ivARB\0"
   "\0"
   /* _mesa_function_pool[35337]: DrawPixels (offset 257) */
   "iiiip\0"
   "glDrawPixels\0"
   "\0"
   /* _mesa_function_pool[35357]: ReplacementCodeuiTexCoord2fNormal3fVertex3fSUN (dynamic) */
   "iffffffff\0"
   "glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[35417]: DrawTransformFeedback (will be remapped) */
   "ii\0"
   "glDrawTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[35445]: VertexAttribs3fvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs3fvNV\0"
   "\0"
   /* _mesa_function_pool[35471]: GenLists (offset 5) */
   "i\0"
   "glGenLists\0"
   "\0"
   /* _mesa_function_pool[35485]: MapGrid2d (offset 226) */
   "iddidd\0"
   "glMapGrid2d\0"
   "\0"
   /* _mesa_function_pool[35505]: MapGrid2f (offset 227) */
   "iffiff\0"
   "glMapGrid2f\0"
   "\0"
   /* _mesa_function_pool[35525]: SampleMapATI (will be remapped) */
   "iii\0"
   "glSampleMapATI\0"
   "\0"
   /* _mesa_function_pool[35545]: TexBumpParameterfvATI (will be remapped) */
   "ip\0"
   "glTexBumpParameterfvATI\0"
   "\0"
   /* _mesa_function_pool[35573]: GetActiveAttrib (will be remapped) */
   "iiipppp\0"
   "glGetActiveAttrib\0"
   "glGetActiveAttribARB\0"
   "\0"
   /* _mesa_function_pool[35621]: TexCoord2fColor4ubVertex3fvSUN (dynamic) */
   "ppp\0"
   "glTexCoord2fColor4ubVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[35659]: PixelMapfv (offset 251) */
   "iip\0"
   "glPixelMapfv\0"
   "\0"
   /* _mesa_function_pool[35677]: ClearBufferData (will be remapped) */
   "iiiip\0"
   "glClearBufferData\0"
   "\0"
   /* _mesa_function_pool[35702]: Color3usv (offset 24) */
   "p\0"
   "glColor3usv\0"
   "\0"
   /* _mesa_function_pool[35717]: CopyImageSubData (will be remapped) */
   "iiiiiiiiiiiiiii\0"
   "glCopyImageSubData\0"
   "\0"
   /* _mesa_function_pool[35753]: StencilOpSeparate (will be remapped) */
   "iiii\0"
   "glStencilOpSeparate\0"
   "glStencilOpSeparateATI\0"
   "\0"
   /* _mesa_function_pool[35802]: GenSamplers (will be remapped) */
   "ip\0"
   "glGenSamplers\0"
   "\0"
   /* _mesa_function_pool[35820]: DrawTexfOES (will be remapped) */
   "fffff\0"
   "glDrawTexfOES\0"
   "\0"
   /* _mesa_function_pool[35841]: AttachObjectARB (will be remapped) */
   "ii\0"
   "glAttachObjectARB\0"
   "\0"
   /* _mesa_function_pool[35863]: GetFragmentLightivSGIX (dynamic) */
   "iip\0"
   "glGetFragmentLightivSGIX\0"
   "\0"
   /* _mesa_function_pool[35893]: Accum (offset 213) */
   "if\0"
   "glAccum\0"
   "\0"
   /* _mesa_function_pool[35905]: GetTexImage (offset 281) */
   "iiiip\0"
   "glGetTexImage\0"
   "\0"
   /* _mesa_function_pool[35926]: Color4x (will be remapped) */
   "iiii\0"
   "glColor4xOES\0"
   "glColor4x\0"
   "\0"
   /* _mesa_function_pool[35955]: ConvolutionParameteriv (offset 353) */
   "iip\0"
   "glConvolutionParameteriv\0"
   "glConvolutionParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[36013]: Color4s (offset 33) */
   "iiii\0"
   "glColor4s\0"
   "\0"
   /* _mesa_function_pool[36029]: CullParameterdvEXT (dynamic) */
   "ip\0"
   "glCullParameterdvEXT\0"
   "\0"
   /* _mesa_function_pool[36054]: GetUniformuiv (will be remapped) */
   "iip\0"
   "glGetUniformuivEXT\0"
   "glGetUniformuiv\0"
   "\0"
   /* _mesa_function_pool[36094]: Color4i (offset 31) */
   "iiii\0"
   "glColor4i\0"
   "\0"
   /* _mesa_function_pool[36110]: Color4f (offset 29) */
   "ffff\0"
   "glColor4f\0"
   "\0"
   /* _mesa_function_pool[36126]: Color4d (offset 27) */
   "dddd\0"
   "glColor4d\0"
   "\0"
   /* _mesa_function_pool[36142]: Color4b (offset 25) */
   "iiii\0"
   "glColor4b\0"
   "\0"
   /* _mesa_function_pool[36158]: LoadProgramNV (will be remapped) */
   "iiip\0"
   "glLoadProgramNV\0"
   "\0"
   /* _mesa_function_pool[36180]: GetAttachedObjectsARB (will be remapped) */
   "iipp\0"
   "glGetAttachedObjectsARB\0"
   "\0"
   /* _mesa_function_pool[36210]: EvalCoord1fv (offset 231) */
   "p\0"
   "glEvalCoord1fv\0"
   "\0"
   /* _mesa_function_pool[36228]: VertexAttribLFormat (will be remapped) */
   "iiii\0"
   "glVertexAttribLFormat\0"
   "\0"
   /* _mesa_function_pool[36256]: StencilFuncSeparate (will be remapped) */
   "iiii\0"
   "glStencilFuncSeparate\0"
   "\0"
   /* _mesa_function_pool[36284]: ShaderSource (will be remapped) */
   "iipp\0"
   "glShaderSource\0"
   "glShaderSourceARB\0"
   "\0"
   /* _mesa_function_pool[36323]: Normal3fv (offset 57) */
   "p\0"
   "glNormal3fv\0"
   "\0"
   /* _mesa_function_pool[36338]: ImageTransformParameterfvHP (dynamic) */
   "iip\0"
   "glImageTransformParameterfvHP\0"
   "\0"
   /* _mesa_function_pool[36373]: NormalP3ui (will be remapped) */
   "ii\0"
   "glNormalP3ui\0"
   "\0"
   /* _mesa_function_pool[36390]: MultiTexCoord3fvARB (offset 395) */
   "ip\0"
   "glMultiTexCoord3fv\0"
   "glMultiTexCoord3fvARB\0"
   "\0"
   /* _mesa_function_pool[36435]: GetProgramParameterfvNV (will be remapped) */
   "iiip\0"
   "glGetProgramParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[36467]: BufferData (will be remapped) */
   "iipi\0"
   "glBufferData\0"
   "glBufferDataARB\0"
   "\0"
   /* _mesa_function_pool[36502]: TexSubImage2D (offset 333) */
   "iiiiiiiip\0"
   "glTexSubImage2D\0"
   "glTexSubImage2DEXT\0"
   "\0"
   /* _mesa_function_pool[36548]: FragmentLightivSGIX (dynamic) */
   "iip\0"
   "glFragmentLightivSGIX\0"
   "\0"
   /* _mesa_function_pool[36575]: GetTexParameterPointervAPPLE (dynamic) */
   "iip\0"
   "glGetTexParameterPointervAPPLE\0"
   "\0"
   /* _mesa_function_pool[36611]: TexGenfv (offset 191) */
   "iip\0"
   "glTexGenfv\0"
   "glTexGenfvOES\0"
   "\0"
   /* _mesa_function_pool[36641]: GetVertexAttribiv (will be remapped) */
   "iip\0"
   "glGetVertexAttribiv\0"
   "glGetVertexAttribivARB\0"
   "\0"
   /* _mesa_function_pool[36689]: TexCoordP2uiv (will be remapped) */
   "ip\0"
   "glTexCoordP2uiv\0"
   "\0"
   /* _mesa_function_pool[36709]: ReplacementCodeuiColor3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glReplacementCodeuiColor3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[36753]: Uniform3fv (will be remapped) */
   "iip\0"
   "glUniform3fv\0"
   "glUniform3fvARB\0"
   "\0"
   /* _mesa_function_pool[36787]: BlendEquation (offset 337) */
   "i\0"
   "glBlendEquation\0"
   "glBlendEquationEXT\0"
   "glBlendEquationOES\0"
   "\0"
   /* _mesa_function_pool[36844]: VertexAttrib3dNV (will be remapped) */
   "iddd\0"
   "glVertexAttrib3dNV\0"
   "\0"
   /* _mesa_function_pool[36869]: ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN (dynamic) */
   "ppppp\0"
   "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[36933]: IndexFuncEXT (dynamic) */
   "if\0"
   "glIndexFuncEXT\0"
   "\0"
   /* _mesa_function_pool[36952]: UseShaderProgramEXT (will be remapped) */
   "ii\0"
   "glUseShaderProgramEXT\0"
   "\0"
   /* _mesa_function_pool[36978]: PushName (offset 201) */
   "i\0"
   "glPushName\0"
   "\0"
   /* _mesa_function_pool[36992]: GenFencesNV (dynamic) */
   "ip\0"
   "glGenFencesNV\0"
   "\0"
   /* _mesa_function_pool[37010]: CullParameterfvEXT (dynamic) */
   "ip\0"
   "glCullParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[37035]: DeleteRenderbuffers (will be remapped) */
   "ip\0"
   "glDeleteRenderbuffers\0"
   "glDeleteRenderbuffersEXT\0"
   "glDeleteRenderbuffersOES\0"
   "\0"
   /* _mesa_function_pool[37111]: VertexAttrib1dv (will be remapped) */
   "ip\0"
   "glVertexAttrib1dv\0"
   "glVertexAttrib1dvARB\0"
   "\0"
   /* _mesa_function_pool[37154]: ImageTransformParameteriHP (dynamic) */
   "iii\0"
   "glImageTransformParameteriHP\0"
   "\0"
   /* _mesa_function_pool[37188]: IsShader (will be remapped) */
   "i\0"
   "glIsShader\0"
   "\0"
   /* _mesa_function_pool[37202]: Rotated (offset 299) */
   "dddd\0"
   "glRotated\0"
   "\0"
   /* _mesa_function_pool[37218]: Color4iv (offset 32) */
   "p\0"
   "glColor4iv\0"
   "\0"
   /* _mesa_function_pool[37232]: PointParameterxv (will be remapped) */
   "ip\0"
   "glPointParameterxvOES\0"
   "glPointParameterxv\0"
   "\0"
   /* _mesa_function_pool[37277]: Rotatex (will be remapped) */
   "iiii\0"
   "glRotatexOES\0"
   "glRotatex\0"
   "\0"
   /* _mesa_function_pool[37306]: FramebufferTextureLayer (will be remapped) */
   "iiiii\0"
   "glFramebufferTextureLayer\0"
   "glFramebufferTextureLayerARB\0"
   "glFramebufferTextureLayerEXT\0"
   "\0"
   /* _mesa_function_pool[37397]: TexEnvfv (offset 185) */
   "iip\0"
   "glTexEnvfv\0"
   "\0"
   /* _mesa_function_pool[37413]: ProgramUniformMatrix3fv (will be remapped) */
   "iiiip\0"
   "glProgramUniformMatrix3fv\0"
   "glProgramUniformMatrix3fvEXT\0"
   "\0"
   /* _mesa_function_pool[37475]: LoadMatrixf (offset 291) */
   "p\0"
   "glLoadMatrixf\0"
   "\0"
   /* _mesa_function_pool[37492]: GetProgramLocalParameterfvARB (will be remapped) */
   "iip\0"
   "glGetProgramLocalParameterfvARB\0"
   "\0"
   /* _mesa_function_pool[37529]: LoadMatrixx (will be remapped) */
   "p\0"
   "glLoadMatrixxOES\0"
   "glLoadMatrixx\0"
   "\0"
   /* _mesa_function_pool[37563]: DrawRangeElementsBaseVertex (will be remapped) */
   "iiiiipi\0"
   "glDrawRangeElementsBaseVertex\0"
   "\0"
   /* _mesa_function_pool[37602]: MatrixIndexuivARB (dynamic) */
   "ip\0"
   "glMatrixIndexuivARB\0"
   "\0"
   /* _mesa_function_pool[37626]: Tangent3sEXT (dynamic) */
   "iii\0"
   "glTangent3sEXT\0"
   "\0"
   /* _mesa_function_pool[37646]: SecondaryColor3bv (will be remapped) */
   "p\0"
   "glSecondaryColor3bv\0"
   "glSecondaryColor3bvEXT\0"
   "\0"
   /* _mesa_function_pool[37692]: GlobalAlphaFactorusSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactorusSUN\0"
   "\0"
   /* _mesa_function_pool[37720]: GetCombinerOutputParameterivNV (dynamic) */
   "iiip\0"
   "glGetCombinerOutputParameterivNV\0"
   "\0"
   /* _mesa_function_pool[37759]: DrawTexxvOES (will be remapped) */
   "p\0"
   "glDrawTexxvOES\0"
   "\0"
   /* _mesa_function_pool[37777]: TexParameterfv (offset 179) */
   "iip\0"
   "glTexParameterfv\0"
   "\0"
   /* _mesa_function_pool[37799]: Color4ubv (offset 36) */
   "p\0"
   "glColor4ubv\0"
   "\0"
   /* _mesa_function_pool[37814]: TexCoord2fv (offset 105) */
   "p\0"
   "glTexCoord2fv\0"
   "\0"
   /* _mesa_function_pool[37831]: FogCoorddv (will be remapped) */
   "p\0"
   "glFogCoorddv\0"
   "glFogCoorddvEXT\0"
   "\0"
   /* _mesa_function_pool[37863]: VDPAUUnregisterSurfaceNV (will be remapped) */
   "i\0"
   "glVDPAUUnregisterSurfaceNV\0"
   "\0"
   /* _mesa_function_pool[37893]: ColorP3ui (will be remapped) */
   "ii\0"
   "glColorP3ui\0"
   "\0"
   /* _mesa_function_pool[37909]: ClearBufferuiv (will be remapped) */
   "iip\0"
   "glClearBufferuiv\0"
   "\0"
   /* _mesa_function_pool[37931]: GetShaderPrecisionFormat (will be remapped) */
   "iipp\0"
   "glGetShaderPrecisionFormat\0"
   "\0"
   /* _mesa_function_pool[37964]: ProgramNamedParameter4dvNV (will be remapped) */
   "iipp\0"
   "glProgramNamedParameter4dvNV\0"
   "\0"
   /* _mesa_function_pool[37999]: Flush (offset 217) */
   "\0"
   "glFlush\0"
   "\0"
   /* _mesa_function_pool[38009]: VertexAttribI4iEXT (will be remapped) */
   "iiiii\0"
   "glVertexAttribI4iEXT\0"
   "glVertexAttribI4i\0"
   "\0"
   /* _mesa_function_pool[38055]: FogCoordd (will be remapped) */
   "d\0"
   "glFogCoordd\0"
   "glFogCoorddEXT\0"
   "\0"
   /* _mesa_function_pool[38085]: Uniform3iv (will be remapped) */
   "iip\0"
   "glUniform3iv\0"
   "glUniform3ivARB\0"
   "\0"
   /* _mesa_function_pool[38119]: TexStorage2DMultisample (will be remapped) */
   "iiiiii\0"
   "glTexStorage2DMultisample\0"
   "\0"
   /* _mesa_function_pool[38153]: UnlockArraysEXT (will be remapped) */
   "\0"
   "glUnlockArraysEXT\0"
   "\0"
   /* _mesa_function_pool[38173]: VertexAttrib1svNV (will be remapped) */
   "ip\0"
   "glVertexAttrib1svNV\0"
   "\0"
   /* _mesa_function_pool[38197]: VertexAttrib4iv (will be remapped) */
   "ip\0"
   "glVertexAttrib4iv\0"
   "glVertexAttrib4ivARB\0"
   "\0"
   /* _mesa_function_pool[38240]: CopyTexSubImage3D (offset 373) */
   "iiiiiiiii\0"
   "glCopyTexSubImage3D\0"
   "glCopyTexSubImage3DEXT\0"
   "glCopyTexSubImage3DOES\0"
   "\0"
   /* _mesa_function_pool[38317]: GetInteger64v (will be remapped) */
   "ip\0"
   "glGetInteger64v\0"
   "\0"
   /* _mesa_function_pool[38337]: DetachObjectARB (will be remapped) */
   "ii\0"
   "glDetachObjectARB\0"
   "\0"
   /* _mesa_function_pool[38359]: Indexiv (offset 49) */
   "p\0"
   "glIndexiv\0"
   "\0"
   /* _mesa_function_pool[38372]: TexEnvi (offset 186) */
   "iii\0"
   "glTexEnvi\0"
   "\0"
   /* _mesa_function_pool[38387]: TexEnvf (offset 184) */
   "iif\0"
   "glTexEnvf\0"
   "\0"
   /* _mesa_function_pool[38402]: TexEnvx (will be remapped) */
   "iii\0"
   "glTexEnvxOES\0"
   "glTexEnvx\0"
   "\0"
   /* _mesa_function_pool[38430]: StopInstrumentsSGIX (dynamic) */
   "i\0"
   "glStopInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[38455]: TexCoord4fColor4fNormal3fVertex4fSUN (dynamic) */
   "fffffffffffffff\0"
   "glTexCoord4fColor4fNormal3fVertex4fSUN\0"
   "\0"
   /* _mesa_function_pool[38511]: InvalidateBufferSubData (will be remapped) */
   "iii\0"
   "glInvalidateBufferSubData\0"
   "\0"
   /* _mesa_function_pool[38542]: UniformMatrix4x2fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix4x2fv\0"
   "\0"
   /* _mesa_function_pool[38569]: ClearTexImage (will be remapped) */
   "iiiip\0"
   "glClearTexImage\0"
   "\0"
   /* _mesa_function_pool[38592]: PolygonOffset (offset 319) */
   "ff\0"
   "glPolygonOffset\0"
   "\0"
   /* _mesa_function_pool[38612]: BeginPerfQueryINTEL (will be remapped) */
   "i\0"
   "glBeginPerfQueryINTEL\0"
   "\0"
   /* _mesa_function_pool[38637]: SamplerParameterfv (will be remapped) */
   "iip\0"
   "glSamplerParameterfv\0"
   "\0"
   /* _mesa_function_pool[38663]: Normal3fVertex3fSUN (dynamic) */
   "ffffff\0"
   "glNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[38693]: ProgramEnvParameter4fARB (will be remapped) */
   "iiffff\0"
   "glProgramEnvParameter4fARB\0"
   "glProgramParameter4fNV\0"
   "\0"
   /* _mesa_function_pool[38751]: ClearDepth (offset 208) */
   "d\0"
   "glClearDepth\0"
   "\0"
   /* _mesa_function_pool[38767]: VertexAttrib3dvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib3dvNV\0"
   "\0"
   /* _mesa_function_pool[38791]: Color4fv (offset 30) */
   "p\0"
   "glColor4fv\0"
   "\0"
   /* _mesa_function_pool[38805]: GetnMinmaxARB (will be remapped) */
   "iiiiip\0"
   "glGetnMinmaxARB\0"
   "\0"
   /* _mesa_function_pool[38829]: ColorPointer (offset 308) */
   "iiip\0"
   "glColorPointer\0"
   "\0"
   /* _mesa_function_pool[38850]: GetPointerv (offset 329) */
   "ip\0"
   "glGetPointerv\0"
   "glGetPointervEXT\0"
   "\0"
   /* _mesa_function_pool[38885]: Lightiv (offset 162) */
   "iip\0"
   "glLightiv\0"
   "\0"
   /* _mesa_function_pool[38900]: GetTexParameteriv (offset 283) */
   "iip\0"
   "glGetTexParameteriv\0"
   "\0"
   /* _mesa_function_pool[38925]: TransformFeedbackVaryings (will be remapped) */
   "iipi\0"
   "glTransformFeedbackVaryings\0"
   "glTransformFeedbackVaryingsEXT\0"
   "\0"
   /* _mesa_function_pool[38990]: VertexAttrib3sv (will be remapped) */
   "ip\0"
   "glVertexAttrib3sv\0"
   "glVertexAttrib3svARB\0"
   "\0"
   /* _mesa_function_pool[39033]: IsVertexArray (will be remapped) */
   "i\0"
   "glIsVertexArray\0"
   "glIsVertexArrayAPPLE\0"
   "glIsVertexArrayOES\0"
   "\0"
   /* _mesa_function_pool[39092]: PushClientAttrib (offset 335) */
   "i\0"
   "glPushClientAttrib\0"
   "\0"
   /* _mesa_function_pool[39114]: ProgramUniform4ui (will be remapped) */
   "iiiiii\0"
   "glProgramUniform4ui\0"
   "glProgramUniform4uiEXT\0"
   "\0"
   /* _mesa_function_pool[39165]: Uniform1f (will be remapped) */
   "if\0"
   "glUniform1f\0"
   "glUniform1fARB\0"
   "\0"
   /* _mesa_function_pool[39196]: FragmentMaterialfSGIX (dynamic) */
   "iif\0"
   "glFragmentMaterialfSGIX\0"
   "\0"
   /* _mesa_function_pool[39225]: Uniform1i (will be remapped) */
   "ii\0"
   "glUniform1i\0"
   "glUniform1iARB\0"
   "\0"
   /* _mesa_function_pool[39256]: GetPolygonStipple (offset 274) */
   "p\0"
   "glGetPolygonStipple\0"
   "\0"
   /* _mesa_function_pool[39279]: PixelTexGenSGIX (dynamic) */
   "i\0"
   "glPixelTexGenSGIX\0"
   "\0"
   /* _mesa_function_pool[39300]: ReplacementCodeusvSUN (dynamic) */
   "p\0"
   "glReplacementCodeusvSUN\0"
   "\0"
   /* _mesa_function_pool[39327]: UseProgram (will be remapped) */
   "i\0"
   "glUseProgram\0"
   "glUseProgramObjectARB\0"
   "\0"
   /* _mesa_function_pool[39365]: StartInstrumentsSGIX (dynamic) */
   "\0"
   "glStartInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[39390]: FlushMappedBufferRangeAPPLE (will be remapped) */
   "iii\0"
   "glFlushMappedBufferRangeAPPLE\0"
   "\0"
   /* _mesa_function_pool[39425]: GetFragDataLocation (will be remapped) */
   "ip\0"
   "glGetFragDataLocationEXT\0"
   "glGetFragDataLocation\0"
   "\0"
   /* _mesa_function_pool[39476]: PixelMapuiv (offset 252) */
   "iip\0"
   "glPixelMapuiv\0"
   "\0"
   /* _mesa_function_pool[39495]: VertexWeightfvEXT (dynamic) */
   "p\0"
   "glVertexWeightfvEXT\0"
   "\0"
   /* _mesa_function_pool[39518]: GetFenceivNV (dynamic) */
   "iip\0"
   "glGetFenceivNV\0"
   "\0"
   /* _mesa_function_pool[39538]: CurrentPaletteMatrixARB (dynamic) */
   "i\0"
   "glCurrentPaletteMatrixARB\0"
   "glCurrentPaletteMatrixOES\0"
   "\0"
   /* _mesa_function_pool[39593]: GetInteger64i_v (will be remapped) */
   "iip\0"
   "glGetInteger64i_v\0"
   "\0"
   /* _mesa_function_pool[39616]: TexCoord2fColor4ubVertex3fSUN (dynamic) */
   "ffiiiifff\0"
   "glTexCoord2fColor4ubVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[39659]: TagSampleBufferSGIX (dynamic) */
   "\0"
   "glTagSampleBufferSGIX\0"
   "\0"
   /* _mesa_function_pool[39683]: Color3s (offset 17) */
   "iii\0"
   "glColor3s\0"
   "\0"
   /* _mesa_function_pool[39698]: TexCoordPointer (offset 320) */
   "iiip\0"
   "glTexCoordPointer\0"
   "\0"
   /* _mesa_function_pool[39722]: Color3i (offset 15) */
   "iii\0"
   "glColor3i\0"
   "\0"
   /* _mesa_function_pool[39737]: EvalCoord2d (offset 232) */
   "dd\0"
   "glEvalCoord2d\0"
   "\0"
   /* _mesa_function_pool[39755]: EvalCoord2f (offset 234) */
   "ff\0"
   "glEvalCoord2f\0"
   "\0"
   /* _mesa_function_pool[39773]: Color3b (offset 9) */
   "iii\0"
   "glColor3b\0"
   "\0"
   /* _mesa_function_pool[39788]: ExecuteProgramNV (will be remapped) */
   "iip\0"
   "glExecuteProgramNV\0"
   "\0"
   /* _mesa_function_pool[39812]: Color3f (offset 13) */
   "fff\0"
   "glColor3f\0"
   "\0"
   /* _mesa_function_pool[39827]: Color3d (offset 11) */
   "ddd\0"
   "glColor3d\0"
   "\0"
   /* _mesa_function_pool[39842]: GetVertexAttribdv (will be remapped) */
   "iip\0"
   "glGetVertexAttribdv\0"
   "glGetVertexAttribdvARB\0"
   "\0"
   /* _mesa_function_pool[39890]: GetBufferPointerv (will be remapped) */
   "iip\0"
   "glGetBufferPointerv\0"
   "glGetBufferPointervARB\0"
   "glGetBufferPointervOES\0"
   "\0"
   /* _mesa_function_pool[39961]: GenFramebuffers (will be remapped) */
   "ip\0"
   "glGenFramebuffers\0"
   "glGenFramebuffersEXT\0"
   "glGenFramebuffersOES\0"
   "\0"
   /* _mesa_function_pool[40025]: GenBuffers (will be remapped) */
   "ip\0"
   "glGenBuffers\0"
   "glGenBuffersARB\0"
   "\0"
   /* _mesa_function_pool[40058]: ClearDepthx (will be remapped) */
   "i\0"
   "glClearDepthxOES\0"
   "glClearDepthx\0"
   "\0"
   /* _mesa_function_pool[40092]: BlendEquationSeparate (will be remapped) */
   "ii\0"
   "glBlendEquationSeparate\0"
   "glBlendEquationSeparateEXT\0"
   "glBlendEquationSeparateATI\0"
   "glBlendEquationSeparateOES\0"
   "\0"
   /* _mesa_function_pool[40201]: PixelTransformParameteriEXT (dynamic) */
   "iii\0"
   "glPixelTransformParameteriEXT\0"
   "\0"
   /* _mesa_function_pool[40236]: MultiTexCoordP4ui (will be remapped) */
   "iii\0"
   "glMultiTexCoordP4ui\0"
   "\0"
   /* _mesa_function_pool[40261]: VertexAttribIPointer (will be remapped) */
   "iiiip\0"
   "glVertexAttribIPointerEXT\0"
   "glVertexAttribIPointer\0"
   "\0"
   /* _mesa_function_pool[40317]: EnableVertexAttribArray (will be remapped) */
   "i\0"
   "glEnableVertexAttribArray\0"
   "glEnableVertexAttribArrayARB\0"
   "\0"
   /* _mesa_function_pool[40375]: FrameZoomSGIX (dynamic) */
   "i\0"
   "glFrameZoomSGIX\0"
   "\0"
   /* _mesa_function_pool[40394]: RasterPos4sv (offset 85) */
   "p\0"
   "glRasterPos4sv\0"
   "\0"
   /* _mesa_function_pool[40412]: SelectBuffer (offset 195) */
   "ip\0"
   "glSelectBuffer\0"
   "\0"
   /* _mesa_function_pool[40431]: GetSynciv (will be remapped) */
   "iiipp\0"
   "glGetSynciv\0"
   "\0"
   /* _mesa_function_pool[40450]: TextureView (will be remapped) */
   "iiiiiiii\0"
   "glTextureView\0"
   "\0"
   /* _mesa_function_pool[40474]: TexEnviv (offset 187) */
   "iip\0"
   "glTexEnviv\0"
   "\0"
   /* _mesa_function_pool[40490]: TexSubImage3D (offset 372) */
   "iiiiiiiiiip\0"
   "glTexSubImage3D\0"
   "glTexSubImage3DEXT\0"
   "glTexSubImage3DOES\0"
   "\0"
   /* _mesa_function_pool[40557]: Bitmap (offset 8) */
   "iiffffp\0"
   "glBitmap\0"
   "\0"
   /* _mesa_function_pool[40575]: VertexAttribDivisor (will be remapped) */
   "ii\0"
   "glVertexAttribDivisorARB\0"
   "glVertexAttribDivisor\0"
   "\0"
   /* _mesa_function_pool[40626]: DrawTransformFeedbackStream (will be remapped) */
   "iii\0"
   "glDrawTransformFeedbackStream\0"
   "\0"
   /* _mesa_function_pool[40661]: GetIntegerv (offset 263) */
   "ip\0"
   "glGetIntegerv\0"
   "\0"
   /* _mesa_function_pool[40679]: EndPerfQueryINTEL (will be remapped) */
   "i\0"
   "glEndPerfQueryINTEL\0"
   "\0"
   /* _mesa_function_pool[40702]: FragmentLightfvSGIX (dynamic) */
   "iip\0"
   "glFragmentLightfvSGIX\0"
   "\0"
   /* _mesa_function_pool[40729]: TexCoord2fColor3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glTexCoord2fColor3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[40766]: GetActiveUniform (will be remapped) */
   "iiipppp\0"
   "glGetActiveUniform\0"
   "glGetActiveUniformARB\0"
   "\0"
   /* _mesa_function_pool[40816]: AlphaFuncx (will be remapped) */
   "ii\0"
   "glAlphaFuncxOES\0"
   "glAlphaFuncx\0"
   "\0"
   /* _mesa_function_pool[40849]: VertexAttribI2ivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI2ivEXT\0"
   "glVertexAttribI2iv\0"
   "\0"
   /* _mesa_function_pool[40894]: VertexBlendARB (dynamic) */
   "i\0"
   "glVertexBlendARB\0"
   "\0"
   /* _mesa_function_pool[40914]: Map1d (offset 220) */
   "iddiip\0"
   "glMap1d\0"
   "\0"
   /* _mesa_function_pool[40930]: Map1f (offset 221) */
   "iffiip\0"
   "glMap1f\0"
   "\0"
   /* _mesa_function_pool[40946]: AreTexturesResident (offset 322) */
   "ipp\0"
   "glAreTexturesResident\0"
   "glAreTexturesResidentEXT\0"
   "\0"
   /* _mesa_function_pool[40998]: ProgramNamedParameter4fNV (will be remapped) */
   "iipffff\0"
   "glProgramNamedParameter4fNV\0"
   "\0"
   /* _mesa_function_pool[41035]: PixelTransferi (offset 248) */
   "ii\0"
   "glPixelTransferi\0"
   "\0"
   /* _mesa_function_pool[41056]: VertexAttrib3fvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib3fvNV\0"
   "\0"
   /* _mesa_function_pool[41080]: GetFinalCombinerInputParameterivNV (dynamic) */
   "iip\0"
   "glGetFinalCombinerInputParameterivNV\0"
   "\0"
   /* _mesa_function_pool[41122]: SecondaryColorP3ui (will be remapped) */
   "ii\0"
   "glSecondaryColorP3ui\0"
   "\0"
   /* _mesa_function_pool[41147]: BindTextures (will be remapped) */
   "iip\0"
   "glBindTextures\0"
   "\0"
   /* _mesa_function_pool[41167]: GetMapParameterivNV (dynamic) */
   "iip\0"
   "glGetMapParameterivNV\0"
   "\0"
   /* _mesa_function_pool[41194]: VertexAttrib4fvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib4fvNV\0"
   "\0"
   /* _mesa_function_pool[41218]: Rectiv (offset 91) */
   "pp\0"
   "glRectiv\0"
   "\0"
   /* _mesa_function_pool[41231]: MultiTexCoord1iv (offset 381) */
   "ip\0"
   "glMultiTexCoord1iv\0"
   "glMultiTexCoord1ivARB\0"
   "\0"
   /* _mesa_function_pool[41276]: PassTexCoordATI (will be remapped) */
   "iii\0"
   "glPassTexCoordATI\0"
   "\0"
   /* _mesa_function_pool[41299]: Tangent3dEXT (dynamic) */
   "ddd\0"
   "glTangent3dEXT\0"
   "\0"
   /* _mesa_function_pool[41319]: Vertex2fv (offset 129) */
   "p\0"
   "glVertex2fv\0"
   "\0"
   /* _mesa_function_pool[41334]: BindRenderbufferEXT (will be remapped) */
   "ii\0"
   "glBindRenderbufferEXT\0"
   "\0"
   /* _mesa_function_pool[41360]: Vertex3sv (offset 141) */
   "p\0"
   "glVertex3sv\0"
   "\0"
   /* _mesa_function_pool[41375]: EvalMesh1 (offset 236) */
   "iii\0"
   "glEvalMesh1\0"
   "\0"
   /* _mesa_function_pool[41392]: DiscardFramebufferEXT (will be remapped) */
   "iip\0"
   "glDiscardFramebufferEXT\0"
   "\0"
   /* _mesa_function_pool[41421]: Uniform2f (will be remapped) */
   "iff\0"
   "glUniform2f\0"
   "glUniform2fARB\0"
   "\0"
   /* _mesa_function_pool[41453]: ColorPointerEXT (will be remapped) */
   "iiiip\0"
   "glColorPointerEXT\0"
   "\0"
   /* _mesa_function_pool[41478]: LineWidth (offset 168) */
   "f\0"
   "glLineWidth\0"
   "\0"
   /* _mesa_function_pool[41493]: Uniform2i (will be remapped) */
   "iii\0"
   "glUniform2i\0"
   "glUniform2iARB\0"
   "\0"
   /* _mesa_function_pool[41525]: MultiDrawElementsBaseVertex (will be remapped) */
   "ipipip\0"
   "glMultiDrawElementsBaseVertex\0"
   "\0"
   /* _mesa_function_pool[41563]: Lightxv (will be remapped) */
   "iip\0"
   "glLightxvOES\0"
   "glLightxv\0"
   "\0"
   /* _mesa_function_pool[41591]: DepthRangeIndexed (will be remapped) */
   "idd\0"
   "glDepthRangeIndexed\0"
   "\0"
   /* _mesa_function_pool[41616]: GetConvolutionParameterfv (offset 357) */
   "iip\0"
   "glGetConvolutionParameterfv\0"
   "glGetConvolutionParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[41680]: GetTexBumpParameterfvATI (will be remapped) */
   "ip\0"
   "glGetTexBumpParameterfvATI\0"
   "\0"
   /* _mesa_function_pool[41711]: ProgramNamedParameter4dNV (will be remapped) */
   "iipdddd\0"
   "glProgramNamedParameter4dNV\0"
   "\0"
   /* _mesa_function_pool[41748]: GetMaterialfv (offset 269) */
   "iip\0"
   "glGetMaterialfv\0"
   "\0"
   /* _mesa_function_pool[41769]: TexImage3DMultisample (will be remapped) */
   "iiiiiii\0"
   "glTexImage3DMultisample\0"
   "\0"
   /* _mesa_function_pool[41802]: VertexAttrib1fvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib1fvNV\0"
   "\0"
   /* _mesa_function_pool[41826]: GetUniformBlockIndex (will be remapped) */
   "ip\0"
   "glGetUniformBlockIndex\0"
   "\0"
   /* _mesa_function_pool[41853]: DetachShader (will be remapped) */
   "ii\0"
   "glDetachShader\0"
   "\0"
   /* _mesa_function_pool[41872]: CopyTexSubImage2D (offset 326) */
   "iiiiiiii\0"
   "glCopyTexSubImage2D\0"
   "glCopyTexSubImage2DEXT\0"
   "\0"
   /* _mesa_function_pool[41925]: SampleCoverage (will be remapped) */
   "fi\0"
   "glSampleCoverage\0"
   "glSampleCoverageARB\0"
   "\0"
   /* _mesa_function_pool[41966]: GetObjectParameterivARB (will be remapped) */
   "iip\0"
   "glGetObjectParameterivARB\0"
   "\0"
   /* _mesa_function_pool[41997]: Color3iv (offset 16) */
   "p\0"
   "glColor3iv\0"
   "\0"
   /* _mesa_function_pool[42011]: DrawElements (offset 311) */
   "iiip\0"
   "glDrawElements\0"
   "\0"
   /* _mesa_function_pool[42032]: ScissorArrayv (will be remapped) */
   "iip\0"
   "glScissorArrayv\0"
   "\0"
   /* _mesa_function_pool[42053]: GetInternalformativ (will be remapped) */
   "iiiip\0"
   "glGetInternalformativ\0"
   "\0"
   /* _mesa_function_pool[42082]: EvalPoint2 (offset 239) */
   "ii\0"
   "glEvalPoint2\0"
   "\0"
   /* _mesa_function_pool[42099]: EvalPoint1 (offset 237) */
   "i\0"
   "glEvalPoint1\0"
   "\0"
   /* _mesa_function_pool[42115]: PopMatrix (offset 297) */
   "\0"
   "glPopMatrix\0"
   "\0"
   /* _mesa_function_pool[42129]: FinishFenceNV (dynamic) */
   "i\0"
   "glFinishFenceNV\0"
   "\0"
   /* _mesa_function_pool[42148]: Tangent3bvEXT (dynamic) */
   "p\0"
   "glTangent3bvEXT\0"
   "\0"
   /* _mesa_function_pool[42167]: GetTexGeniv (offset 280) */
   "iip\0"
   "glGetTexGeniv\0"
   "glGetTexGenivOES\0"
   "\0"
   /* _mesa_function_pool[42203]: GetFirstPerfQueryIdINTEL (will be remapped) */
   "p\0"
   "glGetFirstPerfQueryIdINTEL\0"
   "\0"
   /* _mesa_function_pool[42233]: ActiveProgramEXT (will be remapped) */
   "i\0"
   "glActiveProgramEXT\0"
   "\0"
   /* _mesa_function_pool[42255]: PixelTransformParameterivEXT (dynamic) */
   "iip\0"
   "glPixelTransformParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[42291]: TexCoord4fVertex4fvSUN (dynamic) */
   "pp\0"
   "glTexCoord4fVertex4fvSUN\0"
   "\0"
   /* _mesa_function_pool[42320]: UnmapBuffer (will be remapped) */
   "i\0"
   "glUnmapBuffer\0"
   "glUnmapBufferARB\0"
   "glUnmapBufferOES\0"
   "\0"
   /* _mesa_function_pool[42371]: EvalCoord1d (offset 228) */
   "d\0"
   "glEvalCoord1d\0"
   "\0"
   /* _mesa_function_pool[42388]: EvalCoord1f (offset 230) */
   "f\0"
   "glEvalCoord1f\0"
   "\0"
   /* _mesa_function_pool[42405]: IndexMaterialEXT (dynamic) */
   "ii\0"
   "glIndexMaterialEXT\0"
   "\0"
   /* _mesa_function_pool[42428]: Materialf (offset 169) */
   "iif\0"
   "glMaterialf\0"
   "\0"
   /* _mesa_function_pool[42445]: VertexAttribs2dvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs2dvNV\0"
   "\0"
   /* _mesa_function_pool[42471]: ProgramUniform1uiv (will be remapped) */
   "iiip\0"
   "glProgramUniform1uiv\0"
   "glProgramUniform1uivEXT\0"
   "\0"
   /* _mesa_function_pool[42522]: EvalCoord1dv (offset 229) */
   "p\0"
   "glEvalCoord1dv\0"
   "\0"
   /* _mesa_function_pool[42540]: Materialx (will be remapped) */
   "iii\0"
   "glMaterialxOES\0"
   "glMaterialx\0"
   "\0"
   /* _mesa_function_pool[42572]: GetLightiv (offset 265) */
   "iip\0"
   "glGetLightiv\0"
   "\0"
   /* _mesa_function_pool[42590]: BindBuffer (will be remapped) */
   "ii\0"
   "glBindBuffer\0"
   "glBindBufferARB\0"
   "\0"
   /* _mesa_function_pool[42623]: ProgramUniform1i (will be remapped) */
   "iii\0"
   "glProgramUniform1i\0"
   "glProgramUniform1iEXT\0"
   "\0"
   /* _mesa_function_pool[42669]: ProgramUniform1f (will be remapped) */
   "iif\0"
   "glProgramUniform1f\0"
   "glProgramUniform1fEXT\0"
   "\0"
   /* _mesa_function_pool[42715]: WindowPos3iv (will be remapped) */
   "p\0"
   "glWindowPos3iv\0"
   "glWindowPos3ivARB\0"
   "glWindowPos3ivMESA\0"
   "\0"
   /* _mesa_function_pool[42770]: CopyConvolutionFilter2D (offset 355) */
   "iiiiii\0"
   "glCopyConvolutionFilter2D\0"
   "glCopyConvolutionFilter2DEXT\0"
   "\0"
   /* _mesa_function_pool[42833]: CopyBufferSubData (will be remapped) */
   "iiiii\0"
   "glCopyBufferSubData\0"
   "\0"
   /* _mesa_function_pool[42860]: WeightfvARB (dynamic) */
   "ip\0"
   "glWeightfvARB\0"
   "\0"
   /* _mesa_function_pool[42878]: UniformMatrix3x4fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix3x4fv\0"
   "\0"
   /* _mesa_function_pool[42905]: Recti (offset 90) */
   "iiii\0"
   "glRecti\0"
   "\0"
   /* _mesa_function_pool[42919]: VertexAttribI3ivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI3ivEXT\0"
   "glVertexAttribI3iv\0"
   "\0"
   /* _mesa_function_pool[42964]: DeleteSamplers (will be remapped) */
   "ip\0"
   "glDeleteSamplers\0"
   "\0"
   /* _mesa_function_pool[42985]: SamplerParameteri (will be remapped) */
   "iii\0"
   "glSamplerParameteri\0"
   "\0"
   /* _mesa_function_pool[43010]: Rectf (offset 88) */
   "ffff\0"
   "glRectf\0"
   "\0"
   /* _mesa_function_pool[43024]: Rectd (offset 86) */
   "dddd\0"
   "glRectd\0"
   "\0"
   /* _mesa_function_pool[43038]: SamplerParameterf (will be remapped) */
   "iif\0"
   "glSamplerParameterf\0"
   "\0"
   /* _mesa_function_pool[43063]: Rects (offset 92) */
   "iiii\0"
   "glRects\0"
   "\0"
   /* _mesa_function_pool[43077]: CombinerParameterfNV (dynamic) */
   "if\0"
   "glCombinerParameterfNV\0"
   "\0"
   /* _mesa_function_pool[43104]: GetVertexAttribIiv (will be remapped) */
   "iip\0"
   "glGetVertexAttribIivEXT\0"
   "glGetVertexAttribIiv\0"
   "\0"
   /* _mesa_function_pool[43154]: ClientWaitSync (will be remapped) */
   "iii\0"
   "glClientWaitSync\0"
   "\0"
   /* _mesa_function_pool[43176]: TexCoord4s (offset 124) */
   "iiii\0"
   "glTexCoord4s\0"
   "\0"
   /* _mesa_function_pool[43195]: TexEnvxv (will be remapped) */
   "iip\0"
   "glTexEnvxvOES\0"
   "glTexEnvxv\0"
   "\0"
   /* _mesa_function_pool[43225]: TexCoord4i (offset 122) */
   "iiii\0"
   "glTexCoord4i\0"
   "\0"
   /* _mesa_function_pool[43244]: ObjectPurgeableAPPLE (will be remapped) */
   "iii\0"
   "glObjectPurgeableAPPLE\0"
   "\0"
   /* _mesa_function_pool[43272]: TexCoord4d (offset 118) */
   "dddd\0"
   "glTexCoord4d\0"
   "\0"
   /* _mesa_function_pool[43291]: TexCoord4f (offset 120) */
   "ffff\0"
   "glTexCoord4f\0"
   "\0"
   /* _mesa_function_pool[43310]: GetBooleanv (offset 258) */
   "ip\0"
   "glGetBooleanv\0"
   "\0"
   /* _mesa_function_pool[43328]: IsAsyncMarkerSGIX (dynamic) */
   "i\0"
   "glIsAsyncMarkerSGIX\0"
   "\0"
   /* _mesa_function_pool[43351]: LockArraysEXT (will be remapped) */
   "ii\0"
   "glLockArraysEXT\0"
   "\0"
   /* _mesa_function_pool[43371]: GetActiveUniformBlockiv (will be remapped) */
   "iiip\0"
   "glGetActiveUniformBlockiv\0"
   "\0"
   /* _mesa_function_pool[43403]: GetPerfMonitorCountersAMD (will be remapped) */
   "ippip\0"
   "glGetPerfMonitorCountersAMD\0"
   "\0"
   /* _mesa_function_pool[43438]: ObjectPtrLabel (will be remapped) */
   "pip\0"
   "glObjectPtrLabel\0"
   "\0"
   /* _mesa_function_pool[43460]: Rectfv (offset 89) */
   "pp\0"
   "glRectfv\0"
   "\0"
   /* _mesa_function_pool[43473]: BindImageTexture (will be remapped) */
   "iiiiiii\0"
   "glBindImageTexture\0"
   "\0"
   /* _mesa_function_pool[43501]: ClearDepthf (will be remapped) */
   "f\0"
   "glClearDepthf\0"
   "glClearDepthfOES\0"
   "\0"
   /* _mesa_function_pool[43535]: VertexP4uiv (will be remapped) */
   "ip\0"
   "glVertexP4uiv\0"
   "\0"
   /* _mesa_function_pool[43553]: MinSampleShading (will be remapped) */
   "f\0"
   "glMinSampleShadingARB\0"
   "glMinSampleShading\0"
   "\0"
   /* _mesa_function_pool[43597]: GetRenderbufferParameteriv (will be remapped) */
   "iip\0"
   "glGetRenderbufferParameteriv\0"
   "glGetRenderbufferParameterivEXT\0"
   "glGetRenderbufferParameterivOES\0"
   "\0"
   /* _mesa_function_pool[43695]: EdgeFlagPointerListIBM (dynamic) */
   "ipi\0"
   "glEdgeFlagPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[43725]: WindowPos2sv (will be remapped) */
   "p\0"
   "glWindowPos2sv\0"
   "glWindowPos2svARB\0"
   "glWindowPos2svMESA\0"
   "\0"
   /* _mesa_function_pool[43780]: VertexArrayRangeNV (dynamic) */
   "ip\0"
   "glVertexArrayRangeNV\0"
   "\0"
   /* _mesa_function_pool[43805]: GetPerfMonitorCounterStringAMD (will be remapped) */
   "iiipp\0"
   "glGetPerfMonitorCounterStringAMD\0"
   "\0"
   /* _mesa_function_pool[43845]: EndFragmentShaderATI (will be remapped) */
   "\0"
   "glEndFragmentShaderATI\0"
   "\0"
   /* _mesa_function_pool[43870]: Uniform4iv (will be remapped) */
   "iip\0"
   "glUniform4iv\0"
   "glUniform4ivARB\0"
   "\0"
   ;

/* these functions need to be remapped */
static const struct gl_function_pool_remap MESA_remap_table_functions[] = {
   { 17934, CompressedTexImage1D_remap_index },
   { 15424, CompressedTexImage2D_remap_index },
   { 11314, CompressedTexImage3D_remap_index },
   { 28412, CompressedTexSubImage1D_remap_index },
   { 34452, CompressedTexSubImage2D_remap_index },
   {  5829, CompressedTexSubImage3D_remap_index },
   {  3939, GetCompressedTexImage_remap_index },
   { 17143, LoadTransposeMatrixd_remap_index },
   { 17091, LoadTransposeMatrixf_remap_index },
   { 31865, MultTransposeMatrixd_remap_index },
   { 12621, MultTransposeMatrixf_remap_index },
   { 41925, SampleCoverage_remap_index },
   {  3187, BlendFuncSeparate_remap_index },
   { 20721, FogCoordPointer_remap_index },
   { 38055, FogCoordd_remap_index },
   { 37831, FogCoorddv_remap_index },
   { 30907, MultiDrawArrays_remap_index },
   { 29279, PointParameterf_remap_index },
   {  4549, PointParameterfv_remap_index },
   { 29237, PointParameteri_remap_index },
   {  8174, PointParameteriv_remap_index },
   {  4971, SecondaryColor3b_remap_index },
   { 37646, SecondaryColor3bv_remap_index },
   { 32066, SecondaryColor3d_remap_index },
   { 11447, SecondaryColor3dv_remap_index },
   {  5067, SecondaryColor3i_remap_index },
   { 27599, SecondaryColor3iv_remap_index },
   {  4847, SecondaryColor3s_remap_index },
   { 14710, SecondaryColor3sv_remap_index },
   { 20874, SecondaryColor3ub_remap_index },
   {  6722, SecondaryColor3ubv_remap_index },
   { 20922, SecondaryColor3ui_remap_index },
   { 22772, SecondaryColor3uiv_remap_index },
   { 20765, SecondaryColor3us_remap_index },
   {  9174, SecondaryColor3usv_remap_index },
   { 33168, SecondaryColorPointer_remap_index },
   { 11072, WindowPos2d_remap_index },
   { 16157, WindowPos2dv_remap_index },
   { 11019, WindowPos2f_remap_index },
   { 22126, WindowPos2fv_remap_index },
   { 11125, WindowPos2i_remap_index },
   {  6083, WindowPos2iv_remap_index },
   { 11178, WindowPos2s_remap_index },
   { 43725, WindowPos2sv_remap_index },
   { 14957, WindowPos3d_remap_index },
   { 14441, WindowPos3dv_remap_index },
   { 15044, WindowPos3f_remap_index },
   {  8033, WindowPos3fv_remap_index },
   { 15153, WindowPos3i_remap_index },
   { 42715, WindowPos3iv_remap_index },
   { 15269, WindowPos3s_remap_index },
   { 23520, WindowPos3sv_remap_index },
   {  5965, BeginQuery_remap_index },
   { 42590, BindBuffer_remap_index },
   { 36467, BufferData_remap_index },
   {  9671, BufferSubData_remap_index },
   { 29550, DeleteBuffers_remap_index },
   { 21168, DeleteQueries_remap_index },
   { 18757, EndQuery_remap_index },
   { 40025, GenBuffers_remap_index },
   {  1815, GenQueries_remap_index },
   { 27056, GetBufferParameteriv_remap_index },
   { 39890, GetBufferPointerv_remap_index },
   { 29589, GetBufferSubData_remap_index },
   {  7712, GetQueryObjectiv_remap_index },
   {  7381, GetQueryObjectuiv_remap_index },
   { 11640, GetQueryiv_remap_index },
   { 17624, IsBuffer_remap_index },
   { 27303, IsQuery_remap_index },
   { 11751, MapBuffer_remap_index },
   { 42320, UnmapBuffer_remap_index },
   {   315, AttachShader_remap_index },
   { 34920, BindAttribLocation_remap_index },
   { 40092, BlendEquationSeparate_remap_index },
   { 30606, CompileShader_remap_index },
   { 13930, CreateProgram_remap_index },
   { 29465, CreateShader_remap_index },
   { 19579, DeleteProgram_remap_index },
   { 30588, DeleteShader_remap_index },
   { 41853, DetachShader_remap_index },
   { 32818, DisableVertexAttribArray_remap_index },
   { 34303, DrawBuffers_remap_index },
   { 40317, EnableVertexAttribArray_remap_index },
   { 35573, GetActiveAttrib_remap_index },
   { 40766, GetActiveUniform_remap_index },
   { 16730, GetAttachedShaders_remap_index },
   { 26022, GetAttribLocation_remap_index },
   { 10759, GetProgramInfoLog_remap_index },
   { 21726, GetProgramiv_remap_index },
   {  3680, GetShaderInfoLog_remap_index },
   {  7072, GetShaderSource_remap_index },
   { 16491, GetShaderiv_remap_index },
   {  6016, GetUniformLocation_remap_index },
   { 12774, GetUniformfv_remap_index },
   {  2074, GetUniformiv_remap_index },
   { 33536, GetVertexAttribPointerv_remap_index },
   { 39842, GetVertexAttribdv_remap_index },
   { 34145, GetVertexAttribfv_remap_index },
   { 36641, GetVertexAttribiv_remap_index },
   {  4125, IsProgram_remap_index },
   { 37188, IsShader_remap_index },
   { 27792, LinkProgram_remap_index },
   { 36284, ShaderSource_remap_index },
   { 36256, StencilFuncSeparate_remap_index },
   { 34535, StencilMaskSeparate_remap_index },
   { 35753, StencilOpSeparate_remap_index },
   { 39165, Uniform1f_remap_index },
   {  7888, Uniform1fv_remap_index },
   { 39225, Uniform1i_remap_index },
   { 17805, Uniform1iv_remap_index },
   { 41421, Uniform2f_remap_index },
   { 21892, Uniform2fv_remap_index },
   { 41493, Uniform2i_remap_index },
   { 19821, Uniform2iv_remap_index },
   {   889, Uniform3f_remap_index },
   { 36753, Uniform3fv_remap_index },
   {   802, Uniform3i_remap_index },
   { 38085, Uniform3iv_remap_index },
   {  4419, Uniform4f_remap_index },
   {  8585, Uniform4fv_remap_index },
   {  4356, Uniform4i_remap_index },
   { 43870, Uniform4iv_remap_index },
   {  9777, UniformMatrix2fv_remap_index },
   { 22506, UniformMatrix3fv_remap_index },
   { 10242, UniformMatrix4fv_remap_index },
   { 39327, UseProgram_remap_index },
   { 23951, ValidateProgram_remap_index },
   { 17893, VertexAttrib1d_remap_index },
   { 37111, VertexAttrib1dv_remap_index },
   { 18016, VertexAttrib1s_remap_index },
   { 33993, VertexAttrib1sv_remap_index },
   {  7846, VertexAttrib2d_remap_index },
   { 23271, VertexAttrib2dv_remap_index },
   {  7758, VertexAttrib2s_remap_index },
   { 14063, VertexAttrib2sv_remap_index },
   { 11674, VertexAttrib3d_remap_index },
   { 21816, VertexAttrib3dv_remap_index },
   { 11549, VertexAttrib3s_remap_index },
   { 38990, VertexAttrib3sv_remap_index },
   { 11797, VertexAttrib4Nbv_remap_index },
   { 27688, VertexAttrib4Niv_remap_index },
   { 20157, VertexAttrib4Nsv_remap_index },
   {  1439, VertexAttrib4Nub_remap_index },
   { 32375, VertexAttrib4Nubv_remap_index },
   { 10316, VertexAttrib4Nuiv_remap_index },
   { 34802, VertexAttrib4Nusv_remap_index },
   {  9104, VertexAttrib4bv_remap_index },
   { 27951, VertexAttrib4d_remap_index },
   { 28328, VertexAttrib4dv_remap_index },
   { 38197, VertexAttrib4iv_remap_index },
   { 28019, VertexAttrib4s_remap_index },
   { 18857, VertexAttrib4sv_remap_index },
   { 10002, VertexAttrib4ubv_remap_index },
   { 20112, VertexAttrib4uiv_remap_index },
   {  1365, VertexAttrib4usv_remap_index },
   { 32460, VertexAttribPointer_remap_index },
   { 29077, UniformMatrix2x3fv_remap_index },
   {   922, UniformMatrix2x4fv_remap_index },
   { 10289, UniformMatrix3x2fv_remap_index },
   { 42878, UniformMatrix3x4fv_remap_index },
   { 38542, UniformMatrix4x2fv_remap_index },
   { 11592, UniformMatrix4x3fv_remap_index },
   { 16397, BeginConditionalRender_remap_index },
   { 24035, BeginTransformFeedback_remap_index },
   { 12496, BindBufferBase_remap_index },
   {  7600, BindBufferRange_remap_index },
   { 22290, BindFragDataLocation_remap_index },
   { 23379, ClampColor_remap_index },
   { 16757, ClearBufferfi_remap_index },
   { 16606, ClearBufferfv_remap_index },
   { 20580, ClearBufferiv_remap_index },
   { 37909, ClearBufferuiv_remap_index },
   { 13131, ColorMaski_remap_index },
   {  5794, Disablei_remap_index },
   { 15011, Enablei_remap_index },
   { 22820, EndConditionalRender_remap_index },
   { 19406, EndTransformFeedback_remap_index },
   { 11985, GetBooleani_v_remap_index },
   { 39425, GetFragDataLocation_remap_index },
   { 20601, GetIntegeri_v_remap_index },
   { 28311, GetStringi_remap_index },
   { 30074, GetTexParameterIiv_remap_index },
   { 13327, GetTexParameterIuiv_remap_index },
   { 30288, GetTransformFeedbackVarying_remap_index },
   { 36054, GetUniformuiv_remap_index },
   { 43104, GetVertexAttribIiv_remap_index },
   { 20412, GetVertexAttribIuiv_remap_index },
   { 33451, IsEnabledi_remap_index },
   { 19637, TexParameterIiv_remap_index },
   { 16212, TexParameterIuiv_remap_index },
   { 38925, TransformFeedbackVaryings_remap_index },
   {  5462, Uniform1ui_remap_index },
   { 21956, Uniform1uiv_remap_index },
   { 24872, Uniform2ui_remap_index },
   { 13173, Uniform2uiv_remap_index },
   { 32712, Uniform3ui_remap_index },
   { 18916, Uniform3uiv_remap_index },
   { 11908, Uniform4ui_remap_index },
   { 17839, Uniform4uiv_remap_index },
   { 35148, VertexAttribI1iv_remap_index },
   { 11400, VertexAttribI1uiv_remap_index },
   {  7429, VertexAttribI4bv_remap_index },
   { 10153, VertexAttribI4sv_remap_index },
   {  8385, VertexAttribI4ubv_remap_index },
   {  6888, VertexAttribI4usv_remap_index },
   { 40261, VertexAttribIPointer_remap_index },
   {  8332, PrimitiveRestartIndex_remap_index },
   { 33665, TexBuffer_remap_index },
   {   111, FramebufferTexture_remap_index },
   { 23785, GetBufferParameteri64v_remap_index },
   { 39593, GetInteger64i_v_remap_index },
   { 40575, VertexAttribDivisor_remap_index },
   { 43553, MinSampleShading_remap_index },
   {  7117, BindProgramARB_remap_index },
   { 30773, DeleteProgramsARB_remap_index },
   { 15098, GenProgramsARB_remap_index },
   { 14106, GetProgramEnvParameterdvARB_remap_index },
   { 29515, GetProgramEnvParameterfvARB_remap_index },
   { 30952, GetProgramLocalParameterdvARB_remap_index },
   { 37492, GetProgramLocalParameterfvARB_remap_index },
   { 22435, GetProgramStringARB_remap_index },
   {  8218, GetProgramivARB_remap_index },
   { 31274, IsProgramARB_remap_index },
   { 17211, ProgramEnvParameter4dARB_remap_index },
   {  2660, ProgramEnvParameter4dvARB_remap_index },
   { 38693, ProgramEnvParameter4fARB_remap_index },
   { 24540, ProgramEnvParameter4fvARB_remap_index },
   { 22870, ProgramLocalParameter4dARB_remap_index },
   {  3995, ProgramLocalParameter4dvARB_remap_index },
   { 30504, ProgramLocalParameter4fARB_remap_index },
   { 19204, ProgramLocalParameter4fvARB_remap_index },
   { 31346, ProgramStringARB_remap_index },
   { 11944, VertexAttrib1fARB_remap_index },
   { 31769, VertexAttrib1fvARB_remap_index },
   { 22084, VertexAttrib2fARB_remap_index },
   { 13424, VertexAttrib2fvARB_remap_index },
   {   334, VertexAttrib3fARB_remap_index },
   { 26314, VertexAttrib3fvARB_remap_index },
   { 25232, VertexAttrib4fARB_remap_index },
   { 14398, VertexAttrib4fvARB_remap_index },
   { 35841, AttachObjectARB_remap_index },
   { 22479, CreateProgramObjectARB_remap_index },
   { 16655, CreateShaderObjectARB_remap_index },
   { 15483, DeleteObjectARB_remap_index },
   { 38337, DetachObjectARB_remap_index },
   { 36180, GetAttachedObjectsARB_remap_index },
   { 19457, GetHandleARB_remap_index },
   { 20529, GetInfoLogARB_remap_index },
   { 21334, GetObjectParameterfvARB_remap_index },
   { 41966, GetObjectParameterivARB_remap_index },
   {  5574, DrawArraysInstancedARB_remap_index },
   {  7296, DrawElementsInstancedARB_remap_index },
   { 13887, BindFramebuffer_remap_index },
   {  8241, BindRenderbuffer_remap_index },
   { 33851, BlitFramebuffer_remap_index },
   {  6287, CheckFramebufferStatus_remap_index },
   { 20013, DeleteFramebuffers_remap_index },
   { 37035, DeleteRenderbuffers_remap_index },
   { 31184, FramebufferRenderbuffer_remap_index },
   { 33902, FramebufferTexture1D_remap_index },
   { 23160, FramebufferTexture2D_remap_index },
   { 26942, FramebufferTexture3D_remap_index },
   { 37306, FramebufferTextureLayer_remap_index },
   { 39961, GenFramebuffers_remap_index },
   { 33330, GenRenderbuffers_remap_index },
   {  7540, GenerateMipmap_remap_index },
   {  5270, GetFramebufferAttachmentParameteriv_remap_index },
   { 43597, GetRenderbufferParameteriv_remap_index },
   {  6451, IsFramebuffer_remap_index },
   { 25544, IsRenderbuffer_remap_index },
   {   629, RenderbufferStorage_remap_index },
   { 14881, RenderbufferStorageMultisample_remap_index },
   { 19107, FramebufferTextureFaceARB_remap_index },
   {  5197, FlushMappedBufferRange_remap_index },
   { 30989, MapBufferRange_remap_index },
   { 13266, BindVertexArray_remap_index },
   {  1133, DeleteVertexArrays_remap_index },
   { 17532, GenVertexArrays_remap_index },
   { 39033, IsVertexArray_remap_index },
   { 13046, GetActiveUniformBlockName_remap_index },
   { 43371, GetActiveUniformBlockiv_remap_index },
   {  3106, GetActiveUniformName_remap_index },
   { 14034, GetActiveUniformsiv_remap_index },
   { 41826, GetUniformBlockIndex_remap_index },
   { 10413, GetUniformIndices_remap_index },
   { 34759, UniformBlockBinding_remap_index },
   { 42833, CopyBufferSubData_remap_index },
   { 43154, ClientWaitSync_remap_index },
   { 11276, DeleteSync_remap_index },
   { 34786, FenceSync_remap_index },
   { 38317, GetInteger64v_remap_index },
   { 40431, GetSynciv_remap_index },
   { 15504, IsSync_remap_index },
   { 33520, WaitSync_remap_index },
   { 13081, DrawElementsBaseVertex_remap_index },
   { 16845, DrawElementsInstancedBaseVertex_remap_index },
   { 37563, DrawRangeElementsBaseVertex_remap_index },
   { 41525, MultiDrawElementsBaseVertex_remap_index },
   { 24285, ProvokingVertex_remap_index },
   {  5519, GetMultisamplefv_remap_index },
   { 35255, SampleMaski_remap_index },
   {  1944, TexImage2DMultisample_remap_index },
   { 41769, TexImage3DMultisample_remap_index },
   { 22948, BlendEquationSeparateiARB_remap_index },
   { 27435, BlendEquationiARB_remap_index },
   {  3732, BlendFuncSeparateiARB_remap_index },
   { 24789, BlendFunciARB_remap_index },
   {  1686, BindFragDataLocationIndexed_remap_index },
   { 28900, GetFragDataIndex_remap_index },
   {  2827, BindSampler_remap_index },
   { 42964, DeleteSamplers_remap_index },
   { 35802, GenSamplers_remap_index },
   {  2527, GetSamplerParameterIiv_remap_index },
   {  5763, GetSamplerParameterIuiv_remap_index },
   { 23242, GetSamplerParameterfv_remap_index },
   { 24965, GetSamplerParameteriv_remap_index },
   { 26167, IsSampler_remap_index },
   {  8645, SamplerParameterIiv_remap_index },
   { 12335, SamplerParameterIuiv_remap_index },
   { 43038, SamplerParameterf_remap_index },
   { 38637, SamplerParameterfv_remap_index },
   { 42985, SamplerParameteri_remap_index },
   { 28175, SamplerParameteriv_remap_index },
   { 23329, GetQueryObjecti64v_remap_index },
   {  4047, GetQueryObjectui64v_remap_index },
   { 12921, QueryCounter_remap_index },
   { 37893, ColorP3ui_remap_index },
   {  6606, ColorP3uiv_remap_index },
   { 17652, ColorP4ui_remap_index },
   { 25909, ColorP4uiv_remap_index },
   { 13790, MultiTexCoordP1ui_remap_index },
   { 25679, MultiTexCoordP1uiv_remap_index },
   { 34075, MultiTexCoordP2ui_remap_index },
   {  8958, MultiTexCoordP2uiv_remap_index },
   { 25997, MultiTexCoordP3ui_remap_index },
   {   391, MultiTexCoordP3uiv_remap_index },
   { 40236, MultiTexCoordP4ui_remap_index },
   { 34193, MultiTexCoordP4uiv_remap_index },
   { 36373, NormalP3ui_remap_index },
   { 25604, NormalP3uiv_remap_index },
   { 41122, SecondaryColorP3ui_remap_index },
   {  5711, SecondaryColorP3uiv_remap_index },
   {   162, TexCoordP1ui_remap_index },
   {   609, TexCoordP1uiv_remap_index },
   { 26273, TexCoordP2ui_remap_index },
   { 36689, TexCoordP2uiv_remap_index },
   { 14756, TexCoordP3ui_remap_index },
   { 17695, TexCoordP3uiv_remap_index },
   { 33974, TexCoordP4ui_remap_index },
   {  1769, TexCoordP4uiv_remap_index },
   { 14825, VertexAttribP1ui_remap_index },
   {  4099, VertexAttribP1uiv_remap_index },
   { 29687, VertexAttribP2ui_remap_index },
   {  4893, VertexAttribP2uiv_remap_index },
   {  1487, VertexAttribP3ui_remap_index },
   { 28201, VertexAttribP3uiv_remap_index },
   {  4331, VertexAttribP4ui_remap_index },
   { 16035, VertexAttribP4uiv_remap_index },
   { 34884, VertexP2ui_remap_index },
   { 32442, VertexP2uiv_remap_index },
   { 22462, VertexP3ui_remap_index },
   {  5998, VertexP3uiv_remap_index },
   {  3061, VertexP4ui_remap_index },
   { 43535, VertexP4uiv_remap_index },
   {   777, DrawArraysIndirect_remap_index },
   { 23492, DrawElementsIndirect_remap_index },
   { 10785, BindTransformFeedback_remap_index },
   { 10657, DeleteTransformFeedbacks_remap_index },
   { 35417, DrawTransformFeedback_remap_index },
   {  3889, GenTransformFeedbacks_remap_index },
   { 32964, IsTransformFeedback_remap_index },
   { 30561, PauseTransformFeedback_remap_index },
   { 35083, ResumeTransformFeedback_remap_index },
   { 22378, BeginQueryIndexed_remap_index },
   { 40626, DrawTransformFeedbackStream_remap_index },
   { 19006, EndQueryIndexed_remap_index },
   { 21682, GetQueryIndexediv_remap_index },
   { 43501, ClearDepthf_remap_index },
   { 23851, DepthRangef_remap_index },
   { 37931, GetShaderPrecisionFormat_remap_index },
   {  3161, ReleaseShaderCompiler_remap_index },
   { 25103, ShaderBinary_remap_index },
   { 19475, GetProgramBinary_remap_index },
   { 11842, ProgramBinary_remap_index },
   { 12077, ProgramParameteri_remap_index },
   { 26663, DepthRangeArrayv_remap_index },
   { 41591, DepthRangeIndexed_remap_index },
   { 32944, GetDoublei_v_remap_index },
   { 35273, GetFloati_v_remap_index },
   { 42032, ScissorArrayv_remap_index },
   { 24906, ScissorIndexed_remap_index },
   { 28227, ScissorIndexedv_remap_index },
   { 18447, ViewportArrayv_remap_index },
   { 31463, ViewportIndexedf_remap_index },
   { 19523, ViewportIndexedfv_remap_index },
   {  8432, GetGraphicsResetStatusARB_remap_index },
   { 29830, GetnColorTableARB_remap_index },
   {  2793, GetnCompressedTexImageARB_remap_index },
   {  1233, GetnConvolutionFilterARB_remap_index },
   {  4767, GetnHistogramARB_remap_index },
   { 18332, GetnMapdvARB_remap_index },
   { 12226, GetnMapfvARB_remap_index },
   { 34100, GetnMapivARB_remap_index },
   { 38805, GetnMinmaxARB_remap_index },
   {  3585, GetnPixelMapfvARB_remap_index },
   {  5737, GetnPixelMapuivARB_remap_index },
   { 11523, GetnPixelMapusvARB_remap_index },
   { 21992, GetnPolygonStippleARB_remap_index },
   { 28668, GetnSeparableFilterARB_remap_index },
   { 10047, GetnTexImageARB_remap_index },
   { 27767, GetnUniformdvARB_remap_index },
   { 33762, GetnUniformfvARB_remap_index },
   {  3136, GetnUniformivARB_remap_index },
   { 13650, GetnUniformuivARB_remap_index },
   { 25276, ReadnPixelsARB_remap_index },
   { 32615, DrawArraysInstancedBaseInstance_remap_index },
   { 10198, DrawElementsInstancedBaseInstance_remap_index },
   {  2605, DrawElementsInstancedBaseVertexBaseInstance_remap_index },
   { 35026, DrawTransformFeedbackInstanced_remap_index },
   { 13379, DrawTransformFeedbackStreamInstanced_remap_index },
   { 42053, GetInternalformativ_remap_index },
   { 19598, GetActiveAtomicCounterBufferiv_remap_index },
   { 43473, BindImageTexture_remap_index },
   { 21247, MemoryBarrier_remap_index },
   { 33830, TexStorage1D_remap_index },
   { 23102, TexStorage2D_remap_index },
   { 26919, TexStorage3D_remap_index },
   {  1410, TextureStorage1DEXT_remap_index },
   { 34970, TextureStorage2DEXT_remap_index },
   { 22053, TextureStorage3DEXT_remap_index },
   { 35677, ClearBufferData_remap_index },
   {  2112, ClearBufferSubData_remap_index },
   { 31032, DispatchCompute_remap_index },
   {  6371, DispatchComputeIndirect_remap_index },
   { 35717, CopyImageSubData_remap_index },
   { 40450, TextureView_remap_index },
   { 21576, BindVertexBuffer_remap_index },
   { 29392, VertexAttribBinding_remap_index },
   { 29857, VertexAttribFormat_remap_index },
   { 32556, VertexAttribIFormat_remap_index },
   { 36228, VertexAttribLFormat_remap_index },
   { 34695, VertexBindingDivisor_remap_index },
   {  3299, MultiDrawArraysIndirect_remap_index },
   { 18057, MultiDrawElementsIndirect_remap_index },
   { 17992, TexBufferRange_remap_index },
   { 38119, TexStorage2DMultisample_remap_index },
   { 28120, TexStorage3DMultisample_remap_index },
   {  3331, BufferStorage_remap_index },
   { 38569, ClearTexImage_remap_index },
   { 13014, ClearTexSubImage_remap_index },
   {  4194, BindBuffersBase_remap_index },
   { 14371, BindBuffersRange_remap_index },
   { 10632, BindImageTextures_remap_index },
   {  2773, BindSamplers_remap_index },
   { 41147, BindTextures_remap_index },
   { 24938, BindVertexBuffers_remap_index },
   {  6400, InvalidateBufferData_remap_index },
   { 38511, InvalidateBufferSubData_remap_index },
   { 21207, InvalidateFramebuffer_remap_index },
   { 15772, InvalidateSubFramebuffer_remap_index },
   { 11883, InvalidateTexImage_remap_index },
   { 25303, InvalidateTexSubImage_remap_index },
   { 12673, PolygonOffsetEXT_remap_index },
   { 35820, DrawTexfOES_remap_index },
   { 24771, DrawTexfvOES_remap_index },
   {   986, DrawTexiOES_remap_index },
   { 29766, DrawTexivOES_remap_index },
   { 12030, DrawTexsOES_remap_index },
   { 21401, DrawTexsvOES_remap_index },
   { 25926, DrawTexxOES_remap_index },
   { 37759, DrawTexxvOES_remap_index },
   { 24176, PointSizePointerOES_remap_index },
   {   949, QueryMatrixxOES_remap_index },
   { 19070, SampleMaskSGIS_remap_index },
   { 32670, SamplePatternSGIS_remap_index },
   { 41453, ColorPointerEXT_remap_index },
   { 27329, EdgeFlagPointerEXT_remap_index },
   { 12750, IndexPointerEXT_remap_index },
   { 12940, NormalPointerEXT_remap_index },
   { 26728, TexCoordPointerEXT_remap_index },
   { 23993, VertexPointerEXT_remap_index },
   { 41392, DiscardFramebufferEXT_remap_index },
   { 10581, ActiveShaderProgram_remap_index },
   { 15985, BindProgramPipeline_remap_index },
   { 27381, CreateShaderProgramv_remap_index },
   {  3483, DeleteProgramPipelines_remap_index },
   { 24994, GenProgramPipelines_remap_index },
   {  7922, GetProgramPipelineInfoLog_remap_index },
   { 29933, GetProgramPipelineiv_remap_index },
   { 25057, IsProgramPipeline_remap_index },
   { 43351, LockArraysEXT_remap_index },
   { 42669, ProgramUniform1f_remap_index },
   {  9373, ProgramUniform1fv_remap_index },
   { 42623, ProgramUniform1i_remap_index },
   { 14611, ProgramUniform1iv_remap_index },
   { 33120, ProgramUniform1ui_remap_index },
   { 42471, ProgramUniform1uiv_remap_index },
   {  2251, ProgramUniform2f_remap_index },
   { 17406, ProgramUniform2fv_remap_index },
   {  2334, ProgramUniform2i_remap_index },
   { 20646, ProgramUniform2iv_remap_index },
   {  6967, ProgramUniform2ui_remap_index },
   {  8766, ProgramUniform2uiv_remap_index },
   {  4453, ProgramUniform3f_remap_index },
   { 28949, ProgramUniform3fv_remap_index },
   {  4501, ProgramUniform3i_remap_index },
   { 12965, ProgramUniform3iv_remap_index },
   { 14660, ProgramUniform3ui_remap_index },
   { 17355, ProgramUniform3uiv_remap_index },
   { 27826, ProgramUniform4f_remap_index },
   { 32157, ProgramUniform4fv_remap_index },
   { 27875, ProgramUniform4i_remap_index },
   {  1848, ProgramUniform4iv_remap_index },
   { 39114, ProgramUniform4ui_remap_index },
   { 31917, ProgramUniform4uiv_remap_index },
   { 19142, ProgramUniformMatrix2fv_remap_index },
   { 21510, ProgramUniformMatrix2x3fv_remap_index },
   {  7646, ProgramUniformMatrix2x4fv_remap_index },
   { 37413, ProgramUniformMatrix3fv_remap_index },
   { 32878, ProgramUniformMatrix3x2fv_remap_index },
   { 26357, ProgramUniformMatrix3x4fv_remap_index },
   { 31101, ProgramUniformMatrix4fv_remap_index },
   {  2185, ProgramUniformMatrix4x2fv_remap_index },
   {  7210, ProgramUniformMatrix4x3fv_remap_index },
   { 38153, UnlockArraysEXT_remap_index },
   { 31055, UseProgramStages_remap_index },
   {  1628, ValidateProgramPipeline_remap_index },
   { 16061, DebugMessageCallback_remap_index },
   { 31556, DebugMessageControl_remap_index },
   { 15342, DebugMessageInsert_remap_index },
   {  6770, GetDebugMessageLog_remap_index },
   {  6561, GetObjectLabel_remap_index },
   { 12051, GetObjectPtrLabel_remap_index },
   { 30541, ObjectLabel_remap_index },
   { 43438, ObjectPtrLabel_remap_index },
   { 17875, PopDebugGroup_remap_index },
   { 32294, PushDebugGroup_remap_index },
   {  8286, SecondaryColor3fEXT_remap_index },
   {  7800, SecondaryColor3fvEXT_remap_index },
   { 28618, MultiDrawElementsEXT_remap_index },
   { 10713, FogCoordfEXT_remap_index },
   { 18092, FogCoordfvEXT_remap_index },
   {  4172, ResizeBuffersMESA_remap_index },
   { 34036, WindowPos4dMESA_remap_index },
   { 27226, WindowPos4dvMESA_remap_index },
   {  4281, WindowPos4fMESA_remap_index },
   { 11292, WindowPos4fvMESA_remap_index },
   {  9056, WindowPos4iMESA_remap_index },
   {  3658, WindowPos4ivMESA_remap_index },
   { 27995, WindowPos4sMESA_remap_index },
   {  1080, WindowPos4svMESA_remap_index },
   { 28718, MultiModeDrawArraysIBM_remap_index },
   { 19786, MultiModeDrawElementsIBM_remap_index },
   { 32789, AreProgramsResidentNV_remap_index },
   { 39788, ExecuteProgramNV_remap_index },
   { 29483, GetProgramParameterdvNV_remap_index },
   { 36435, GetProgramParameterfvNV_remap_index },
   { 19239, GetProgramStringNV_remap_index },
   { 15696, GetProgramivNV_remap_index },
   { 18377, GetTrackMatrixivNV_remap_index },
   { 18819, GetVertexAttribdvNV_remap_index },
   { 16982, GetVertexAttribfvNV_remap_index },
   { 15878, GetVertexAttribivNV_remap_index },
   { 36158, LoadProgramNV_remap_index },
   { 19886, ProgramParameters4dvNV_remap_index },
   { 20498, ProgramParameters4fvNV_remap_index },
   {  6182, RequestResidentProgramsNV_remap_index },
   { 29104, TrackMatrixNV_remap_index },
   { 14141, VertexAttrib1dNV_remap_index },
   { 28063, VertexAttrib1dvNV_remap_index },
   { 28389, VertexAttrib1fNV_remap_index },
   { 41802, VertexAttrib1fvNV_remap_index },
   { 21066, VertexAttrib1sNV_remap_index },
   { 38173, VertexAttrib1svNV_remap_index },
   { 18353, VertexAttrib2dNV_remap_index },
   { 34388, VertexAttrib2dvNV_remap_index },
   { 27110, VertexAttrib2fNV_remap_index },
   { 25805, VertexAttrib2fvNV_remap_index },
   { 12846, VertexAttrib2sNV_remap_index },
   {  5687, VertexAttrib2svNV_remap_index },
   { 36844, VertexAttrib3dNV_remap_index },
   { 38767, VertexAttrib3dvNV_remap_index },
   {  4946, VertexAttrib3fNV_remap_index },
   { 41056, VertexAttrib3fvNV_remap_index },
   {  7154, VertexAttrib3sNV_remap_index },
   { 18404, VertexAttrib3svNV_remap_index },
   {  8488, VertexAttrib4dNV_remap_index },
   {  3408, VertexAttrib4dvNV_remap_index },
   {  8559, VertexAttrib4fNV_remap_index },
   { 41194, VertexAttrib4fvNV_remap_index },
   { 17490, VertexAttrib4sNV_remap_index },
   { 10897, VertexAttrib4svNV_remap_index },
   {  1601, VertexAttrib4ubNV_remap_index },
   { 10688, VertexAttrib4ubvNV_remap_index },
   { 28869, VertexAttribPointerNV_remap_index },
   { 27277, VertexAttribs1dvNV_remap_index },
   { 30832, VertexAttribs1fvNV_remap_index },
   {  6156, VertexAttribs1svNV_remap_index },
   { 42445, VertexAttribs2dvNV_remap_index },
   {  4305, VertexAttribs2fvNV_remap_index },
   { 27355, VertexAttribs2svNV_remap_index },
   {  1789, VertexAttribs3dvNV_remap_index },
   { 35445, VertexAttribs3fvNV_remap_index },
   { 14008, VertexAttribs3svNV_remap_index },
   { 24745, VertexAttribs4dvNV_remap_index },
   { 24327, VertexAttribs4fvNV_remap_index },
   { 20695, VertexAttribs4svNV_remap_index },
   { 32112, VertexAttribs4ubvNV_remap_index },
   { 41680, GetTexBumpParameterfvATI_remap_index },
   { 10439, GetTexBumpParameterivATI_remap_index },
   { 35545, TexBumpParameterfvATI_remap_index },
   {  8531, TexBumpParameterivATI_remap_index },
   {  9256, AlphaFragmentOp1ATI_remap_index },
   {  3432, AlphaFragmentOp2ATI_remap_index },
   {  9712, AlphaFragmentOp3ATI_remap_index },
   { 33303, BeginFragmentShaderATI_remap_index },
   {  3705, BindFragmentShaderATI_remap_index },
   {  7179, ColorFragmentOp1ATI_remap_index },
   { 12812, ColorFragmentOp2ATI_remap_index },
   { 23914, ColorFragmentOp3ATI_remap_index },
   { 16797, DeleteFragmentShaderATI_remap_index },
   { 43845, EndFragmentShaderATI_remap_index },
   { 22921, GenFragmentShadersATI_remap_index },
   { 41276, PassTexCoordATI_remap_index },
   { 35525, SampleMapATI_remap_index },
   { 34849, SetFragmentShaderConstantATI_remap_index },
   {  8462, ActiveStencilFaceEXT_remap_index },
   {  8088, BindVertexArrayAPPLE_remap_index },
   { 16327, GenVertexArraysAPPLE_remap_index },
   { 35111, GetProgramNamedParameterdvNV_remap_index },
   { 22671, GetProgramNamedParameterfvNV_remap_index },
   { 41711, ProgramNamedParameter4dNV_remap_index },
   { 37964, ProgramNamedParameter4dvNV_remap_index },
   { 40998, ProgramNamedParameter4fNV_remap_index },
   { 25403, ProgramNamedParameter4fvNV_remap_index },
   { 24234, PrimitiveRestartNV_remap_index },
   { 24723, GetTexGenxvOES_remap_index },
   { 14496, TexGenxOES_remap_index },
   { 32356, TexGenxvOES_remap_index },
   {  8012, DepthBoundsEXT_remap_index },
   {  5926, BindFramebufferEXT_remap_index },
   { 41334, BindRenderbufferEXT_remap_index },
   {  4390, BufferParameteriAPPLE_remap_index },
   { 39390, FlushMappedBufferRangeAPPLE_remap_index },
   { 27645, VertexAttribI1iEXT_remap_index },
   { 11231, VertexAttribI1uiEXT_remap_index },
   { 20219, VertexAttribI2iEXT_remap_index },
   { 40849, VertexAttribI2ivEXT_remap_index },
   { 25498, VertexAttribI2uiEXT_remap_index },
   { 35208, VertexAttribI2uivEXT_remap_index },
   { 19324, VertexAttribI3iEXT_remap_index },
   { 42919, VertexAttribI3ivEXT_remap_index },
   { 22603, VertexAttribI3uiEXT_remap_index },
   { 20970, VertexAttribI3uivEXT_remap_index },
   { 38009, VertexAttribI4iEXT_remap_index },
   {  6677, VertexAttribI4ivEXT_remap_index },
   {  2557, VertexAttribI4uiEXT_remap_index },
   { 27502, VertexAttribI4uivEXT_remap_index },
   {  2983, ClearColorIiEXT_remap_index },
   {  1208, ClearColorIuiEXT_remap_index },
   { 24257, BindBufferOffsetEXT_remap_index },
   { 18124, BeginPerfMonitorAMD_remap_index },
   { 32747, DeletePerfMonitorsAMD_remap_index },
   {  5439, EndPerfMonitorAMD_remap_index },
   { 26842, GenPerfMonitorsAMD_remap_index },
   { 12538, GetPerfMonitorCounterDataAMD_remap_index },
   { 34722, GetPerfMonitorCounterInfoAMD_remap_index },
   { 43805, GetPerfMonitorCounterStringAMD_remap_index },
   { 43403, GetPerfMonitorCountersAMD_remap_index },
   { 14212, GetPerfMonitorGroupStringAMD_remap_index },
   { 29636, GetPerfMonitorGroupsAMD_remap_index },
   { 13849, SelectPerfMonitorCountersAMD_remap_index },
   { 14309, GetObjectParameterivAPPLE_remap_index },
   { 43244, ObjectPurgeableAPPLE_remap_index },
   {  1897, ObjectUnpurgeableAPPLE_remap_index },
   { 42233, ActiveProgramEXT_remap_index },
   { 27248, CreateShaderProgramEXT_remap_index },
   { 36952, UseShaderProgramEXT_remap_index },
   { 30407, TextureBarrierNV_remap_index },
   {  2142, VDPAUFiniNV_remap_index },
   {   835, VDPAUGetSurfaceivNV_remap_index },
   { 23767, VDPAUInitNV_remap_index },
   { 21488, VDPAUIsSurfaceNV_remap_index },
   {  6426, VDPAUMapSurfacesNV_remap_index },
   {  3024, VDPAURegisterOutputSurfaceNV_remap_index },
   { 12407, VDPAURegisterVideoSurfaceNV_remap_index },
   { 10970, VDPAUSurfaceAccessNV_remap_index },
   {  4740, VDPAUUnmapSurfacesNV_remap_index },
   { 37863, VDPAUUnregisterSurfaceNV_remap_index },
   { 38612, BeginPerfQueryINTEL_remap_index },
   { 33624, CreatePerfQueryINTEL_remap_index },
   { 16258, DeletePerfQueryINTEL_remap_index },
   { 40679, EndPerfQueryINTEL_remap_index },
   { 42203, GetFirstPerfQueryIdINTEL_remap_index },
   { 30360, GetNextPerfQueryIdINTEL_remap_index },
   { 31968, GetPerfCounterInfoINTEL_remap_index },
   {   746, GetPerfQueryDataINTEL_remap_index },
   { 22258, GetPerfQueryIdByNameINTEL_remap_index },
   { 19980, GetPerfQueryInfoINTEL_remap_index },
   { 20381, StencilFuncSeparateATI_remap_index },
   {  5652, ProgramEnvParameters4fvEXT_remap_index },
   { 30675, ProgramLocalParameters4fvEXT_remap_index },
   {  3846, EGLImageTargetRenderbufferStorageOES_remap_index },
   {  3610, EGLImageTargetTexture2DOES_remap_index },
   { 40816, AlphaFuncx_remap_index },
   { 19369, ClearColorx_remap_index },
   { 40058, ClearDepthx_remap_index },
   { 35926, Color4x_remap_index },
   { 23705, DepthRangex_remap_index },
   {  2381, Fogx_remap_index },
   { 14775, Fogxv_remap_index },
   {  8733, Frustumf_remap_index },
   {  8830, Frustumx_remap_index },
   {  9893, LightModelx_remap_index },
   { 32206, LightModelxv_remap_index },
   { 28923, Lightx_remap_index },
   { 41563, Lightxv_remap_index },
   {  3540, LineWidthx_remap_index },
   { 37529, LoadMatrixx_remap_index },
   { 42540, Materialx_remap_index },
   { 25353, Materialxv_remap_index },
   { 29126, MultMatrixx_remap_index },
   { 31371, MultiTexCoord4x_remap_index },
   { 25202, Normal3x_remap_index },
   { 15395, Orthof_remap_index },
   { 15576, Orthox_remap_index },
   { 27549, PointSizex_remap_index },
   {    70, PolygonOffsetx_remap_index },
   { 37277, Rotatex_remap_index },
   { 19937, SampleCoveragex_remap_index },
   { 12696, Scalex_remap_index },
   { 38402, TexEnvx_remap_index },
   { 43195, TexEnvxv_remap_index },
   {  1976, TexParameterx_remap_index },
   { 31697, Translatex_remap_index },
   { 33087, ClipPlanef_remap_index },
   { 32989, ClipPlanex_remap_index },
   {   707, GetClipPlanef_remap_index },
   {   570, GetClipPlanex_remap_index },
   { 19855, GetFixedv_remap_index },
   {  1267, GetLightxv_remap_index },
   { 22708, GetMaterialxv_remap_index },
   { 21365, GetTexEnvxv_remap_index },
   { 16682, GetTexParameterxv_remap_index },
   { 29160, PointParameterx_remap_index },
   { 37232, PointParameterxv_remap_index },
   { 19028, TexParameterxv_remap_index },
   {    -1, -1 }
};

/* these functions are in the ABI, but have alternative names */
static const struct gl_function_remap MESA_alt_functions[] = {
   /* from GL_EXT_blend_color */
   { 34268, _gloffset_BlendColor },
   /* from GL_EXT_blend_minmax */
   { 36787, _gloffset_BlendEquation },
   /* from GL_EXT_color_subtable */
   {  5396, _gloffset_ColorSubTable },
   { 21266, _gloffset_CopyColorSubTable },
   /* from GL_EXT_convolution */
   {  1301, _gloffset_GetConvolutionParameteriv },
   { 13692, _gloffset_ConvolutionParameterfv },
   { 16544, _gloffset_CopyConvolutionFilter1D },
   { 18540, _gloffset_SeparableFilter2D },
   { 19731, _gloffset_GetConvolutionFilter },
   { 23437, _gloffset_ConvolutionFilter1D },
   { 25705, _gloffset_ConvolutionFilter2D },
   { 28476, _gloffset_GetSeparableFilter },
   { 30018, _gloffset_ConvolutionParameteri },
   { 30140, _gloffset_ConvolutionParameterf },
   { 35955, _gloffset_ConvolutionParameteriv },
   { 41616, _gloffset_GetConvolutionParameterfv },
   { 42770, _gloffset_CopyConvolutionFilter2D },
   /* from GL_EXT_copy_texture */
   { 27164, _gloffset_CopyTexImage2D },
   { 29784, _gloffset_CopyTexImage1D },
   { 32243, _gloffset_CopyTexSubImage1D },
   { 38240, _gloffset_CopyTexSubImage3D },
   { 41872, _gloffset_CopyTexSubImage2D },
   /* from GL_EXT_draw_range_elements */
   { 24672, _gloffset_DrawRangeElements },
   /* from GL_EXT_histogram */
   {  4639, _gloffset_GetHistogramParameterfv },
   {  8114, _gloffset_GetHistogramParameteriv },
   {  9496, _gloffset_Minmax },
   { 13948, _gloffset_GetMinmax },
   { 21859, _gloffset_Histogram },
   { 29712, _gloffset_GetMinmaxParameteriv },
   { 30712, _gloffset_ResetMinmax },
   { 31657, _gloffset_GetHistogram },
   { 33397, _gloffset_GetMinmaxParameterfv },
   { 34412, _gloffset_ResetHistogram },
   /* from GL_EXT_paletted_texture */
   { 13554, _gloffset_ColorTable },
   { 18149, _gloffset_GetColorTableParameterfv },
   { 26487, _gloffset_GetColorTable },
   { 30196, _gloffset_GetColorTableParameteriv },
   /* from GL_EXT_subtexture */
   {  2413, _gloffset_TexSubImage1D },
   { 36502, _gloffset_TexSubImage2D },
   /* from GL_EXT_texture3D */
   { 22201, _gloffset_TexImage3D },
   { 40490, _gloffset_TexSubImage3D },
   /* from GL_EXT_texture_object */
   {  4218, _gloffset_GenTextures },
   {  8877, _gloffset_BindTexture },
   { 17325, _gloffset_IsTexture },
   { 22553, _gloffset_PrioritizeTextures },
   { 26687, _gloffset_DeleteTextures },
   { 40946, _gloffset_AreTexturesResident },
   /* from GL_EXT_vertex_array */
   { 18638, _gloffset_ArrayElement },
   { 29203, _gloffset_DrawArrays },
   { 38850, _gloffset_GetPointerv },
   /* from GL_NV_read_buffer */
   { 29987, _gloffset_ReadBuffer },
   /* from GL_OES_blend_subtract */
   { 36787, _gloffset_BlendEquation },
   /* from GL_OES_texture_3D */
   { 22201, _gloffset_TexImage3D },
   { 38240, _gloffset_CopyTexSubImage3D },
   { 40490, _gloffset_TexSubImage3D },
   /* from GL_OES_texture_cube_map */
   { 16911, _gloffset_TexGeni },
   { 16939, _gloffset_TexGenf },
   { 20297, _gloffset_GetTexGenfv },
   { 34219, _gloffset_TexGeniv },
   { 36611, _gloffset_TexGenfv },
   { 42167, _gloffset_GetTexGeniv },
   /* from GL_SGI_color_table */
   {  2717, _gloffset_ColorTableParameteriv },
   { 13554, _gloffset_ColorTable },
   { 17269, _gloffset_ColorTableParameterfv },
   { 18149, _gloffset_GetColorTableParameterfv },
   { 26487, _gloffset_GetColorTable },
   { 26565, _gloffset_CopyColorTable },
   { 30196, _gloffset_GetColorTableParameteriv },
   {    -1, -1 }
};

