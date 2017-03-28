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
   /* _mesa_function_pool[17]: UniformMatrix3fvARB (will be remapped) */
   "iiip\0"
   "glUniformMatrix3fv\0"
   "glUniformMatrix3fvARB\0"
   "\0"
   /* _mesa_function_pool[64]: MapGrid1f (offset 225) */
   "iff\0"
   "glMapGrid1f\0"
   "\0"
   /* _mesa_function_pool[81]: VertexAttribI2iEXT (will be remapped) */
   "iii\0"
   "glVertexAttribI2iEXT\0"
   "glVertexAttribI2i\0"
   "\0"
   /* _mesa_function_pool[125]: RasterPos4i (offset 82) */
   "iiii\0"
   "glRasterPos4i\0"
   "\0"
   /* _mesa_function_pool[145]: VertexAttribI4uiEXT (will be remapped) */
   "iiiii\0"
   "glVertexAttribI4uiEXT\0"
   "glVertexAttribI4ui\0"
   "\0"
   /* _mesa_function_pool[193]: RasterPos4d (offset 78) */
   "dddd\0"
   "glRasterPos4d\0"
   "\0"
   /* _mesa_function_pool[213]: NewList (dynamic) */
   "ii\0"
   "glNewList\0"
   "\0"
   /* _mesa_function_pool[227]: RasterPos4f (offset 80) */
   "ffff\0"
   "glRasterPos4f\0"
   "\0"
   /* _mesa_function_pool[247]: LoadIdentity (offset 290) */
   "\0"
   "glLoadIdentity\0"
   "\0"
   /* _mesa_function_pool[264]: SampleCoverageARB (will be remapped) */
   "fi\0"
   "glSampleCoverage\0"
   "glSampleCoverageARB\0"
   "\0"
   /* _mesa_function_pool[305]: ConvolutionFilter1D (offset 348) */
   "iiiiip\0"
   "glConvolutionFilter1D\0"
   "glConvolutionFilter1DEXT\0"
   "\0"
   /* _mesa_function_pool[360]: BeginQueryARB (will be remapped) */
   "ii\0"
   "glBeginQuery\0"
   "glBeginQueryARB\0"
   "\0"
   /* _mesa_function_pool[393]: RasterPos3dv (offset 71) */
   "p\0"
   "glRasterPos3dv\0"
   "\0"
   /* _mesa_function_pool[411]: PointParameteriNV (will be remapped) */
   "ii\0"
   "glPointParameteri\0"
   "glPointParameteriNV\0"
   "\0"
   /* _mesa_function_pool[453]: GetProgramiv (will be remapped) */
   "iip\0"
   "glGetProgramiv\0"
   "\0"
   /* _mesa_function_pool[473]: MultiTexCoord3sARB (offset 398) */
   "iiii\0"
   "glMultiTexCoord3s\0"
   "glMultiTexCoord3sARB\0"
   "\0"
   /* _mesa_function_pool[518]: SecondaryColor3iEXT (will be remapped) */
   "iii\0"
   "glSecondaryColor3i\0"
   "glSecondaryColor3iEXT\0"
   "\0"
   /* _mesa_function_pool[564]: WindowPos3fMESA (will be remapped) */
   "fff\0"
   "glWindowPos3f\0"
   "glWindowPos3fARB\0"
   "glWindowPos3fMESA\0"
   "\0"
   /* _mesa_function_pool[618]: TexCoord1iv (offset 99) */
   "p\0"
   "glTexCoord1iv\0"
   "\0"
   /* _mesa_function_pool[635]: TexCoord4sv (offset 125) */
   "p\0"
   "glTexCoord4sv\0"
   "\0"
   /* _mesa_function_pool[652]: RasterPos4s (offset 84) */
   "iiii\0"
   "glRasterPos4s\0"
   "\0"
   /* _mesa_function_pool[672]: PixelTexGenParameterfvSGIS (dynamic) */
   "ip\0"
   "glPixelTexGenParameterfvSGIS\0"
   "\0"
   /* _mesa_function_pool[705]: ActiveTextureARB (offset 374) */
   "i\0"
   "glActiveTexture\0"
   "glActiveTextureARB\0"
   "\0"
   /* _mesa_function_pool[743]: BlitFramebufferEXT (will be remapped) */
   "iiiiiiiiii\0"
   "glBlitFramebuffer\0"
   "glBlitFramebufferEXT\0"
   "\0"
   /* _mesa_function_pool[794]: TexCoord1f (offset 96) */
   "f\0"
   "glTexCoord1f\0"
   "\0"
   /* _mesa_function_pool[810]: TexCoord1d (offset 94) */
   "d\0"
   "glTexCoord1d\0"
   "\0"
   /* _mesa_function_pool[826]: VertexAttrib4ubvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib4ubvNV\0"
   "\0"
   /* _mesa_function_pool[851]: TexCoord1i (offset 98) */
   "i\0"
   "glTexCoord1i\0"
   "\0"
   /* _mesa_function_pool[867]: GetProgramNamedParameterdvNV (will be remapped) */
   "iipp\0"
   "glGetProgramNamedParameterdvNV\0"
   "\0"
   /* _mesa_function_pool[904]: Histogram (offset 367) */
   "iiii\0"
   "glHistogram\0"
   "glHistogramEXT\0"
   "\0"
   /* _mesa_function_pool[937]: TexCoord1s (offset 100) */
   "i\0"
   "glTexCoord1s\0"
   "\0"
   /* _mesa_function_pool[953]: GetMapfv (offset 267) */
   "iip\0"
   "glGetMapfv\0"
   "\0"
   /* _mesa_function_pool[969]: EvalCoord1f (offset 230) */
   "f\0"
   "glEvalCoord1f\0"
   "\0"
   /* _mesa_function_pool[986]: FramebufferTexture (will be remapped) */
   "iiii\0"
   "glFramebufferTexture\0"
   "\0"
   /* _mesa_function_pool[1013]: TexCoordP1ui (will be remapped) */
   "ii\0"
   "glTexCoordP1ui\0"
   "\0"
   /* _mesa_function_pool[1032]: TexImage4DSGIS (dynamic) */
   "iiiiiiiiiip\0"
   "glTexImage4DSGIS\0"
   "\0"
   /* _mesa_function_pool[1062]: PolygonStipple (offset 175) */
   "p\0"
   "glPolygonStipple\0"
   "\0"
   /* _mesa_function_pool[1082]: WindowPos2dvMESA (will be remapped) */
   "p\0"
   "glWindowPos2dv\0"
   "glWindowPos2dvARB\0"
   "glWindowPos2dvMESA\0"
   "\0"
   /* _mesa_function_pool[1137]: ReplacementCodeuiColor3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glReplacementCodeuiColor3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[1181]: BlendEquationSeparateEXT (will be remapped) */
   "ii\0"
   "glBlendEquationSeparate\0"
   "glBlendEquationSeparateEXT\0"
   "glBlendEquationSeparateATI\0"
   "\0"
   /* _mesa_function_pool[1263]: ListParameterfSGIX (dynamic) */
   "iif\0"
   "glListParameterfSGIX\0"
   "\0"
   /* _mesa_function_pool[1289]: TexStorage1D (will be remapped) */
   "iiii\0"
   "glTexStorage1D\0"
   "\0"
   /* _mesa_function_pool[1310]: SecondaryColor3bEXT (will be remapped) */
   "iii\0"
   "glSecondaryColor3b\0"
   "glSecondaryColor3bEXT\0"
   "\0"
   /* _mesa_function_pool[1356]: TexCoord4fColor4fNormal3fVertex4fvSUN (dynamic) */
   "pppp\0"
   "glTexCoord4fColor4fNormal3fVertex4fvSUN\0"
   "\0"
   /* _mesa_function_pool[1402]: GetnPolygonStippleARB (will be remapped) */
   "ip\0"
   "glGetnPolygonStippleARB\0"
   "\0"
   /* _mesa_function_pool[1430]: GetPixelMapfv (offset 271) */
   "ip\0"
   "glGetPixelMapfv\0"
   "\0"
   /* _mesa_function_pool[1450]: Color3uiv (offset 22) */
   "p\0"
   "glColor3uiv\0"
   "\0"
   /* _mesa_function_pool[1465]: IsEnabled (offset 286) */
   "i\0"
   "glIsEnabled\0"
   "\0"
   /* _mesa_function_pool[1480]: DebugMessageCallbackARB (will be remapped) */
   "pp\0"
   "glDebugMessageCallbackARB\0"
   "\0"
   /* _mesa_function_pool[1510]: VertexAttrib4svNV (will be remapped) */
   "ip\0"
   "glVertexAttrib4svNV\0"
   "\0"
   /* _mesa_function_pool[1534]: EvalCoord2fv (offset 235) */
   "p\0"
   "glEvalCoord2fv\0"
   "\0"
   /* _mesa_function_pool[1552]: GetBufferSubDataARB (will be remapped) */
   "iiip\0"
   "glGetBufferSubData\0"
   "glGetBufferSubDataARB\0"
   "\0"
   /* _mesa_function_pool[1599]: BufferSubDataARB (will be remapped) */
   "iiip\0"
   "glBufferSubData\0"
   "glBufferSubDataARB\0"
   "\0"
   /* _mesa_function_pool[1640]: TexCoord2fColor4ubVertex3fvSUN (dynamic) */
   "ppp\0"
   "glTexCoord2fColor4ubVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[1678]: TexCoord2fColor4fNormal3fVertex3fSUN (dynamic) */
   "ffffffffffff\0"
   "glTexCoord2fColor4fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[1731]: AttachShader (will be remapped) */
   "ii\0"
   "glAttachShader\0"
   "\0"
   /* _mesa_function_pool[1750]: TextureStorage3DEXT (will be remapped) */
   "iiiiiii\0"
   "glTextureStorage3DEXT\0"
   "\0"
   /* _mesa_function_pool[1781]: VertexAttrib2fARB (will be remapped) */
   "iff\0"
   "glVertexAttrib2f\0"
   "glVertexAttrib2fARB\0"
   "\0"
   /* _mesa_function_pool[1823]: GetDebugLogLengthMESA (dynamic) */
   "iii\0"
   "glGetDebugLogLengthMESA\0"
   "\0"
   /* _mesa_function_pool[1852]: GetMapiv (offset 268) */
   "iip\0"
   "glGetMapiv\0"
   "\0"
   /* _mesa_function_pool[1868]: VertexAttrib3fARB (will be remapped) */
   "ifff\0"
   "glVertexAttrib3f\0"
   "glVertexAttrib3fARB\0"
   "\0"
   /* _mesa_function_pool[1911]: VertexAttribI4ivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI4ivEXT\0"
   "glVertexAttribI4iv\0"
   "\0"
   /* _mesa_function_pool[1956]: Indexubv (offset 316) */
   "p\0"
   "glIndexubv\0"
   "\0"
   /* _mesa_function_pool[1970]: GetQueryivARB (will be remapped) */
   "iip\0"
   "glGetQueryiv\0"
   "glGetQueryivARB\0"
   "\0"
   /* _mesa_function_pool[2004]: TexImage3D (offset 371) */
   "iiiiiiiiip\0"
   "glTexImage3D\0"
   "glTexImage3DEXT\0"
   "\0"
   /* _mesa_function_pool[2045]: BindFragDataLocationEXT (will be remapped) */
   "iip\0"
   "glBindFragDataLocationEXT\0"
   "glBindFragDataLocation\0"
   "\0"
   /* _mesa_function_pool[2099]: TexCoordP4ui (will be remapped) */
   "ii\0"
   "glTexCoordP4ui\0"
   "\0"
   /* _mesa_function_pool[2118]: ReplacementCodeuiVertex3fvSUN (dynamic) */
   "pp\0"
   "glReplacementCodeuiVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[2154]: EdgeFlagPointer (offset 312) */
   "ip\0"
   "glEdgeFlagPointer\0"
   "\0"
   /* _mesa_function_pool[2176]: Color3ubv (offset 20) */
   "p\0"
   "glColor3ubv\0"
   "\0"
   /* _mesa_function_pool[2191]: GetQueryObjectivARB (will be remapped) */
   "iip\0"
   "glGetQueryObjectiv\0"
   "glGetQueryObjectivARB\0"
   "\0"
   /* _mesa_function_pool[2237]: Vertex3dv (offset 135) */
   "p\0"
   "glVertex3dv\0"
   "\0"
   /* _mesa_function_pool[2252]: ReplacementCodeuiTexCoord2fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glReplacementCodeuiTexCoord2fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[2299]: CompressedTexSubImage2DARB (will be remapped) */
   "iiiiiiiip\0"
   "glCompressedTexSubImage2D\0"
   "glCompressedTexSubImage2DARB\0"
   "\0"
   /* _mesa_function_pool[2365]: CombinerOutputNV (dynamic) */
   "iiiiiiiiii\0"
   "glCombinerOutputNV\0"
   "\0"
   /* _mesa_function_pool[2396]: VertexAttribs3fvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs3fvNV\0"
   "\0"
   /* _mesa_function_pool[2422]: GetnMapivARB (will be remapped) */
   "iiip\0"
   "glGetnMapivARB\0"
   "\0"
   /* _mesa_function_pool[2443]: Uniform2fARB (will be remapped) */
   "iff\0"
   "glUniform2f\0"
   "glUniform2fARB\0"
   "\0"
   /* _mesa_function_pool[2475]: LightModeliv (offset 166) */
   "ip\0"
   "glLightModeliv\0"
   "\0"
   /* _mesa_function_pool[2494]: VertexAttrib1svARB (will be remapped) */
   "ip\0"
   "glVertexAttrib1sv\0"
   "glVertexAttrib1svARB\0"
   "\0"
   /* _mesa_function_pool[2537]: VertexAttribs1dvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs1dvNV\0"
   "\0"
   /* _mesa_function_pool[2563]: Uniform2ivARB (will be remapped) */
   "iip\0"
   "glUniform2iv\0"
   "glUniform2ivARB\0"
   "\0"
   /* _mesa_function_pool[2597]: GetImageTransformParameterfvHP (dynamic) */
   "iip\0"
   "glGetImageTransformParameterfvHP\0"
   "\0"
   /* _mesa_function_pool[2635]: Normal3bv (offset 53) */
   "p\0"
   "glNormal3bv\0"
   "\0"
   /* _mesa_function_pool[2650]: TexGeniv (offset 193) */
   "iip\0"
   "glTexGeniv\0"
   "\0"
   /* _mesa_function_pool[2666]: TexCoordP1uiv (will be remapped) */
   "ip\0"
   "glTexCoordP1uiv\0"
   "\0"
   /* _mesa_function_pool[2686]: BeginQueryIndexed (will be remapped) */
   "iii\0"
   "glBeginQueryIndexed\0"
   "\0"
   /* _mesa_function_pool[2711]: WeightubvARB (dynamic) */
   "ip\0"
   "glWeightubvARB\0"
   "\0"
   /* _mesa_function_pool[2730]: VertexAttrib1fvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib1fvNV\0"
   "\0"
   /* _mesa_function_pool[2754]: Vertex3iv (offset 139) */
   "p\0"
   "glVertex3iv\0"
   "\0"
   /* _mesa_function_pool[2769]: CopyConvolutionFilter1D (offset 354) */
   "iiiii\0"
   "glCopyConvolutionFilter1D\0"
   "glCopyConvolutionFilter1DEXT\0"
   "\0"
   /* _mesa_function_pool[2831]: VertexAttribI1uiEXT (will be remapped) */
   "ii\0"
   "glVertexAttribI1uiEXT\0"
   "glVertexAttribI1ui\0"
   "\0"
   /* _mesa_function_pool[2876]: ReplacementCodeuiNormal3fVertex3fSUN (dynamic) */
   "iffffff\0"
   "glReplacementCodeuiNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[2924]: DeleteSync (will be remapped) */
   "i\0"
   "glDeleteSync\0"
   "\0"
   /* _mesa_function_pool[2940]: FragmentMaterialfvSGIX (dynamic) */
   "iip\0"
   "glFragmentMaterialfvSGIX\0"
   "\0"
   /* _mesa_function_pool[2970]: BlendColor (offset 336) */
   "ffff\0"
   "glBlendColor\0"
   "glBlendColorEXT\0"
   "\0"
   /* _mesa_function_pool[3005]: UniformMatrix4fvARB (will be remapped) */
   "iiip\0"
   "glUniformMatrix4fv\0"
   "glUniformMatrix4fvARB\0"
   "\0"
   /* _mesa_function_pool[3052]: DeleteVertexArraysAPPLE (will be remapped) */
   "ip\0"
   "glDeleteVertexArrays\0"
   "glDeleteVertexArraysAPPLE\0"
   "\0"
   /* _mesa_function_pool[3103]: GetDebugMessageLogARB (will be remapped) */
   "iipppppp\0"
   "glGetDebugMessageLogARB\0"
   "\0"
   /* _mesa_function_pool[3137]: ReadInstrumentsSGIX (dynamic) */
   "i\0"
   "glReadInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[3162]: CallLists (offset 3) */
   "iip\0"
   "glCallLists\0"
   "\0"
   /* _mesa_function_pool[3179]: DeformationMap3dSGIX (dynamic) */
   "iddiiddiiddiip\0"
   "glDeformationMap3dSGIX\0"
   "\0"
   /* _mesa_function_pool[3218]: UniformMatrix2x4fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix2x4fv\0"
   "\0"
   /* _mesa_function_pool[3245]: ReadnPixelsARB (will be remapped) */
   "iiiiiiip\0"
   "glReadnPixelsARB\0"
   "\0"
   /* _mesa_function_pool[3272]: Color4ubVertex3fvSUN (dynamic) */
   "pp\0"
   "glColor4ubVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[3299]: Normal3iv (offset 59) */
   "p\0"
   "glNormal3iv\0"
   "\0"
   /* _mesa_function_pool[3314]: PassThrough (offset 199) */
   "f\0"
   "glPassThrough\0"
   "\0"
   /* _mesa_function_pool[3331]: VertexP3ui (will be remapped) */
   "ii\0"
   "glVertexP3ui\0"
   "\0"
   /* _mesa_function_pool[3348]: TexParameterIivEXT (will be remapped) */
   "iip\0"
   "glTexParameterIivEXT\0"
   "glTexParameterIiv\0"
   "\0"
   /* _mesa_function_pool[3392]: FramebufferTextureLayerEXT (will be remapped) */
   "iiiii\0"
   "glFramebufferTextureLayer\0"
   "glFramebufferTextureLayerARB\0"
   "glFramebufferTextureLayerEXT\0"
   "\0"
   /* _mesa_function_pool[3483]: InvalidateTexSubImage (will be remapped) */
   "iiiiiiii\0"
   "glInvalidateTexSubImage\0"
   "\0"
   /* _mesa_function_pool[3517]: Viewport (offset 305) */
   "iiii\0"
   "glViewport\0"
   "\0"
   /* _mesa_function_pool[3534]: VertexAttrib4NusvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nusv\0"
   "glVertexAttrib4NusvARB\0"
   "\0"
   /* _mesa_function_pool[3581]: GetQueryIndexediv (will be remapped) */
   "iiip\0"
   "glGetQueryIndexediv\0"
   "\0"
   /* _mesa_function_pool[3607]: WindowPos4svMESA (will be remapped) */
   "p\0"
   "glWindowPos4svMESA\0"
   "\0"
   /* _mesa_function_pool[3629]: CreateProgramObjectARB (will be remapped) */
   "\0"
   "glCreateProgramObjectARB\0"
   "\0"
   /* _mesa_function_pool[3656]: FragmentLightModelivSGIX (dynamic) */
   "ip\0"
   "glFragmentLightModelivSGIX\0"
   "\0"
   /* _mesa_function_pool[3687]: UniformMatrix4x3fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix4x3fv\0"
   "\0"
   /* _mesa_function_pool[3714]: PrioritizeTextures (offset 331) */
   "ipp\0"
   "glPrioritizeTextures\0"
   "glPrioritizeTexturesEXT\0"
   "\0"
   /* _mesa_function_pool[3764]: VertexAttribI3uiEXT (will be remapped) */
   "iiii\0"
   "glVertexAttribI3uiEXT\0"
   "glVertexAttribI3ui\0"
   "\0"
   /* _mesa_function_pool[3811]: AsyncMarkerSGIX (dynamic) */
   "i\0"
   "glAsyncMarkerSGIX\0"
   "\0"
   /* _mesa_function_pool[3832]: GlobalAlphaFactorubSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactorubSUN\0"
   "\0"
   /* _mesa_function_pool[3860]: ClearColorIuiEXT (will be remapped) */
   "iiii\0"
   "glClearColorIuiEXT\0"
   "\0"
   /* _mesa_function_pool[3885]: GetProgramParameterdvNV (will be remapped) */
   "iiip\0"
   "glGetProgramParameterdvNV\0"
   "\0"
   /* _mesa_function_pool[3917]: Uniform4uiEXT (will be remapped) */
   "iiiii\0"
   "glUniform4uiEXT\0"
   "glUniform4ui\0"
   "\0"
   /* _mesa_function_pool[3953]: ResetHistogram (offset 369) */
   "i\0"
   "glResetHistogram\0"
   "glResetHistogramEXT\0"
   "\0"
   /* _mesa_function_pool[3993]: GetProgramNamedParameterfvNV (will be remapped) */
   "iipp\0"
   "glGetProgramNamedParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[4030]: PointParameterfEXT (will be remapped) */
   "if\0"
   "glPointParameterf\0"
   "glPointParameterfARB\0"
   "glPointParameterfEXT\0"
   "glPointParameterfSGIS\0"
   "\0"
   /* _mesa_function_pool[4116]: LoadIdentityDeformationMapSGIX (dynamic) */
   "i\0"
   "glLoadIdentityDeformationMapSGIX\0"
   "\0"
   /* _mesa_function_pool[4152]: GenFencesNV (dynamic) */
   "ip\0"
   "glGenFencesNV\0"
   "\0"
   /* _mesa_function_pool[4170]: ImageTransformParameterfHP (dynamic) */
   "iif\0"
   "glImageTransformParameterfHP\0"
   "\0"
   /* _mesa_function_pool[4204]: MatrixIndexusvARB (dynamic) */
   "ip\0"
   "glMatrixIndexusvARB\0"
   "\0"
   /* _mesa_function_pool[4228]: DrawElementsBaseVertex (will be remapped) */
   "iiipi\0"
   "glDrawElementsBaseVertex\0"
   "\0"
   /* _mesa_function_pool[4260]: DisableVertexAttribArrayARB (will be remapped) */
   "i\0"
   "glDisableVertexAttribArray\0"
   "glDisableVertexAttribArrayARB\0"
   "\0"
   /* _mesa_function_pool[4320]: GetnConvolutionFilterARB (will be remapped) */
   "iiiip\0"
   "glGetnConvolutionFilterARB\0"
   "\0"
   /* _mesa_function_pool[4354]: TexCoord2sv (offset 109) */
   "p\0"
   "glTexCoord2sv\0"
   "\0"
   /* _mesa_function_pool[4371]: Vertex4dv (offset 143) */
   "p\0"
   "glVertex4dv\0"
   "\0"
   /* _mesa_function_pool[4386]: StencilMaskSeparate (will be remapped) */
   "ii\0"
   "glStencilMaskSeparate\0"
   "\0"
   /* _mesa_function_pool[4412]: ProgramLocalParameter4dARB (will be remapped) */
   "iidddd\0"
   "glProgramLocalParameter4dARB\0"
   "\0"
   /* _mesa_function_pool[4449]: InvalidateTexImage (will be remapped) */
   "ii\0"
   "glInvalidateTexImage\0"
   "\0"
   /* _mesa_function_pool[4474]: CompressedTexImage3DARB (will be remapped) */
   "iiiiiiiip\0"
   "glCompressedTexImage3D\0"
   "glCompressedTexImage3DARB\0"
   "\0"
   /* _mesa_function_pool[4534]: Color3sv (offset 18) */
   "p\0"
   "glColor3sv\0"
   "\0"
   /* _mesa_function_pool[4548]: GetConvolutionParameteriv (offset 358) */
   "iip\0"
   "glGetConvolutionParameteriv\0"
   "glGetConvolutionParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[4612]: DeleteSamplers (will be remapped) */
   "ip\0"
   "glDeleteSamplers\0"
   "\0"
   /* _mesa_function_pool[4633]: VertexAttrib1fARB (will be remapped) */
   "if\0"
   "glVertexAttrib1f\0"
   "glVertexAttrib1fARB\0"
   "\0"
   /* _mesa_function_pool[4674]: Vertex2dv (offset 127) */
   "p\0"
   "glVertex2dv\0"
   "\0"
   /* _mesa_function_pool[4689]: TestFenceNV (dynamic) */
   "i\0"
   "glTestFenceNV\0"
   "\0"
   /* _mesa_function_pool[4706]: CreateShaderProgramEXT (will be remapped) */
   "ip\0"
   "glCreateShaderProgramEXT\0"
   "\0"
   /* _mesa_function_pool[4735]: MultiTexCoord1fvARB (offset 379) */
   "ip\0"
   "glMultiTexCoord1fv\0"
   "glMultiTexCoord1fvARB\0"
   "\0"
   /* _mesa_function_pool[4780]: TexCoord3iv (offset 115) */
   "p\0"
   "glTexCoord3iv\0"
   "\0"
   /* _mesa_function_pool[4797]: Uniform2uivEXT (will be remapped) */
   "iip\0"
   "glUniform2uivEXT\0"
   "glUniform2uiv\0"
   "\0"
   /* _mesa_function_pool[4833]: ColorFragmentOp2ATI (will be remapped) */
   "iiiiiiiiii\0"
   "glColorFragmentOp2ATI\0"
   "\0"
   /* _mesa_function_pool[4867]: TexStorage2D (will be remapped) */
   "iiiii\0"
   "glTexStorage2D\0"
   "\0"
   /* _mesa_function_pool[4889]: SecondaryColorPointerListIBM (dynamic) */
   "iiipi\0"
   "glSecondaryColorPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[4927]: GetPixelTexGenParameterivSGIS (dynamic) */
   "ip\0"
   "glGetPixelTexGenParameterivSGIS\0"
   "\0"
   /* _mesa_function_pool[4963]: Color3fv (offset 14) */
   "p\0"
   "glColor3fv\0"
   "\0"
   /* _mesa_function_pool[4977]: GetnPixelMapfvARB (will be remapped) */
   "iip\0"
   "glGetnPixelMapfvARB\0"
   "\0"
   /* _mesa_function_pool[5002]: GetnMapdvARB (will be remapped) */
   "iiip\0"
   "glGetnMapdvARB\0"
   "\0"
   /* _mesa_function_pool[5023]: TextureStorage1DEXT (will be remapped) */
   "iiiii\0"
   "glTextureStorage1DEXT\0"
   "\0"
   /* _mesa_function_pool[5052]: ReplacementCodeubSUN (dynamic) */
   "i\0"
   "glReplacementCodeubSUN\0"
   "\0"
   /* _mesa_function_pool[5078]: FinishAsyncSGIX (dynamic) */
   "p\0"
   "glFinishAsyncSGIX\0"
   "\0"
   /* _mesa_function_pool[5099]: VertexAttribP3ui (will be remapped) */
   "iiii\0"
   "glVertexAttribP3ui\0"
   "\0"
   /* _mesa_function_pool[5124]: GetDebugLogMESA (dynamic) */
   "iiiipp\0"
   "glGetDebugLogMESA\0"
   "\0"
   /* _mesa_function_pool[5150]: FogCoorddEXT (will be remapped) */
   "d\0"
   "glFogCoordd\0"
   "glFogCoorddEXT\0"
   "\0"
   /* _mesa_function_pool[5180]: BeginConditionalRenderNV (will be remapped) */
   "ii\0"
   "glBeginConditionalRender\0"
   "glBeginConditionalRenderNV\0"
   "\0"
   /* _mesa_function_pool[5236]: Color4ubVertex3fSUN (dynamic) */
   "iiiifff\0"
   "glColor4ubVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[5267]: FogCoordfEXT (will be remapped) */
   "f\0"
   "glFogCoordf\0"
   "glFogCoordfEXT\0"
   "\0"
   /* _mesa_function_pool[5297]: PointSize (offset 173) */
   "f\0"
   "glPointSize\0"
   "\0"
   /* _mesa_function_pool[5312]: VertexAttribI2uivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI2uivEXT\0"
   "glVertexAttribI2uiv\0"
   "\0"
   /* _mesa_function_pool[5359]: TexCoord2fVertex3fSUN (dynamic) */
   "fffff\0"
   "glTexCoord2fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[5390]: UniformBlockBinding (will be remapped) */
   "iii\0"
   "glUniformBlockBinding\0"
   "\0"
   /* _mesa_function_pool[5417]: PopName (offset 200) */
   "\0"
   "glPopName\0"
   "\0"
   /* _mesa_function_pool[5429]: GetSamplerParameterfv (will be remapped) */
   "iip\0"
   "glGetSamplerParameterfv\0"
   "\0"
   /* _mesa_function_pool[5458]: GlobalAlphaFactoriSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactoriSUN\0"
   "\0"
   /* _mesa_function_pool[5485]: VertexAttrib2dNV (will be remapped) */
   "idd\0"
   "glVertexAttrib2dNV\0"
   "\0"
   /* _mesa_function_pool[5509]: GetProgramInfoLog (will be remapped) */
   "iipp\0"
   "glGetProgramInfoLog\0"
   "\0"
   /* _mesa_function_pool[5535]: VertexP2ui (will be remapped) */
   "ii\0"
   "glVertexP2ui\0"
   "\0"
   /* _mesa_function_pool[5552]: VertexAttrib4NbvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nbv\0"
   "glVertexAttrib4NbvARB\0"
   "\0"
   /* _mesa_function_pool[5597]: GetActiveAttribARB (will be remapped) */
   "iiipppp\0"
   "glGetActiveAttrib\0"
   "glGetActiveAttribARB\0"
   "\0"
   /* _mesa_function_pool[5645]: Vertex4sv (offset 149) */
   "p\0"
   "glVertex4sv\0"
   "\0"
   /* _mesa_function_pool[5660]: GetActiveUniformBlockiv (will be remapped) */
   "iiip\0"
   "glGetActiveUniformBlockiv\0"
   "\0"
   /* _mesa_function_pool[5692]: VertexAttrib4ubNV (will be remapped) */
   "iiiii\0"
   "glVertexAttrib4ubNV\0"
   "\0"
   /* _mesa_function_pool[5719]: VertexAttribI1ivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI1ivEXT\0"
   "glVertexAttribI1iv\0"
   "\0"
   /* _mesa_function_pool[5764]: ClampColor (will be remapped) */
   "ii\0"
   "glClampColor\0"
   "\0"
   /* _mesa_function_pool[5781]: TextureRangeAPPLE (dynamic) */
   "iip\0"
   "glTextureRangeAPPLE\0"
   "\0"
   /* _mesa_function_pool[5806]: GetTexEnvfv (offset 276) */
   "iip\0"
   "glGetTexEnvfv\0"
   "\0"
   /* _mesa_function_pool[5825]: BindFragDataLocationIndexed (will be remapped) */
   "iiip\0"
   "glBindFragDataLocationIndexed\0"
   "\0"
   /* _mesa_function_pool[5861]: BindTransformFeedback (will be remapped) */
   "ii\0"
   "glBindTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[5889]: TextureStorage2DEXT (will be remapped) */
   "iiiiii\0"
   "glTextureStorage2DEXT\0"
   "\0"
   /* _mesa_function_pool[5919]: Indexub (offset 315) */
   "i\0"
   "glIndexub\0"
   "\0"
   /* _mesa_function_pool[5932]: VertexAttrib4fNV (will be remapped) */
   "iffff\0"
   "glVertexAttrib4fNV\0"
   "\0"
   /* _mesa_function_pool[5958]: ReplacementCodeuiSUN (dynamic) */
   "i\0"
   "glReplacementCodeuiSUN\0"
   "\0"
   /* _mesa_function_pool[5984]: GetTransformFeedbackVaryingEXT (will be remapped) */
   "iiipppp\0"
   "glGetTransformFeedbackVarying\0"
   "glGetTransformFeedbackVaryingEXT\0"
   "\0"
   /* _mesa_function_pool[6056]: TexEnvi (offset 186) */
   "iii\0"
   "glTexEnvi\0"
   "\0"
   /* _mesa_function_pool[6071]: GetClipPlane (offset 259) */
   "ip\0"
   "glGetClipPlane\0"
   "\0"
   /* _mesa_function_pool[6090]: CombinerParameterfvNV (dynamic) */
   "ip\0"
   "glCombinerParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[6118]: TexCoordP4uiv (will be remapped) */
   "ip\0"
   "glTexCoordP4uiv\0"
   "\0"
   /* _mesa_function_pool[6138]: VertexAttribs3dvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs3dvNV\0"
   "\0"
   /* _mesa_function_pool[6164]: VertexAttribI2uiEXT (will be remapped) */
   "iii\0"
   "glVertexAttribI2uiEXT\0"
   "glVertexAttribI2ui\0"
   "\0"
   /* _mesa_function_pool[6210]: VertexAttribs4fvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs4fvNV\0"
   "\0"
   /* _mesa_function_pool[6236]: VertexArrayRangeNV (dynamic) */
   "ip\0"
   "glVertexArrayRangeNV\0"
   "\0"
   /* _mesa_function_pool[6261]: FragmentLightiSGIX (dynamic) */
   "iii\0"
   "glFragmentLightiSGIX\0"
   "\0"
   /* _mesa_function_pool[6287]: DrawTransformFeedbackInstanced (will be remapped) */
   "iii\0"
   "glDrawTransformFeedbackInstanced\0"
   "\0"
   /* _mesa_function_pool[6325]: PolygonOffsetEXT (will be remapped) */
   "ff\0"
   "glPolygonOffsetEXT\0"
   "\0"
   /* _mesa_function_pool[6348]: VertexAttribI4uivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI4uivEXT\0"
   "glVertexAttribI4uiv\0"
   "\0"
   /* _mesa_function_pool[6395]: PollAsyncSGIX (dynamic) */
   "p\0"
   "glPollAsyncSGIX\0"
   "\0"
   /* _mesa_function_pool[6414]: DeleteFragmentShaderATI (will be remapped) */
   "i\0"
   "glDeleteFragmentShaderATI\0"
   "\0"
   /* _mesa_function_pool[6443]: Scaled (offset 301) */
   "ddd\0"
   "glScaled\0"
   "\0"
   /* _mesa_function_pool[6457]: TangentPointerEXT (dynamic) */
   "iip\0"
   "glTangentPointerEXT\0"
   "\0"
   /* _mesa_function_pool[6482]: Scalef (offset 302) */
   "fff\0"
   "glScalef\0"
   "\0"
   /* _mesa_function_pool[6496]: TexCoord2fNormal3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glTexCoord2fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[6534]: ProgramEnvParameters4fvEXT (will be remapped) */
   "iiip\0"
   "glProgramEnvParameters4fvEXT\0"
   "\0"
   /* _mesa_function_pool[6569]: MultTransposeMatrixdARB (will be remapped) */
   "p\0"
   "glMultTransposeMatrixd\0"
   "glMultTransposeMatrixdARB\0"
   "\0"
   /* _mesa_function_pool[6621]: ObjectUnpurgeableAPPLE (will be remapped) */
   "iii\0"
   "glObjectUnpurgeableAPPLE\0"
   "\0"
   /* _mesa_function_pool[6651]: AlphaFunc (offset 240) */
   "if\0"
   "glAlphaFunc\0"
   "\0"
   /* _mesa_function_pool[6667]: WindowPos2svMESA (will be remapped) */
   "p\0"
   "glWindowPos2sv\0"
   "glWindowPos2svARB\0"
   "glWindowPos2svMESA\0"
   "\0"
   /* _mesa_function_pool[6722]: EdgeFlag (offset 41) */
   "i\0"
   "glEdgeFlag\0"
   "\0"
   /* _mesa_function_pool[6736]: TexCoord2iv (offset 107) */
   "p\0"
   "glTexCoord2iv\0"
   "\0"
   /* _mesa_function_pool[6753]: CompressedTexImage1DARB (will be remapped) */
   "iiiiiip\0"
   "glCompressedTexImage1D\0"
   "glCompressedTexImage1DARB\0"
   "\0"
   /* _mesa_function_pool[6811]: Rotated (offset 299) */
   "dddd\0"
   "glRotated\0"
   "\0"
   /* _mesa_function_pool[6827]: GetTexParameterIuivEXT (will be remapped) */
   "iip\0"
   "glGetTexParameterIuivEXT\0"
   "glGetTexParameterIuiv\0"
   "\0"
   /* _mesa_function_pool[6879]: VertexAttrib2sNV (will be remapped) */
   "iii\0"
   "glVertexAttrib2sNV\0"
   "\0"
   /* _mesa_function_pool[6903]: BindBufferBaseEXT (will be remapped) */
   "iii\0"
   "glBindBufferBase\0"
   "glBindBufferBaseEXT\0"
   "\0"
   /* _mesa_function_pool[6945]: ReadPixels (offset 256) */
   "iiiiiip\0"
   "glReadPixels\0"
   "\0"
   /* _mesa_function_pool[6967]: EdgeFlagv (offset 42) */
   "p\0"
   "glEdgeFlagv\0"
   "\0"
   /* _mesa_function_pool[6982]: NormalPointerListIBM (dynamic) */
   "iipi\0"
   "glNormalPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[7011]: IndexPointerEXT (will be remapped) */
   "iiip\0"
   "glIndexPointerEXT\0"
   "\0"
   /* _mesa_function_pool[7035]: QueryCounter (will be remapped) */
   "ii\0"
   "glQueryCounter\0"
   "\0"
   /* _mesa_function_pool[7054]: Color4iv (offset 32) */
   "p\0"
   "glColor4iv\0"
   "\0"
   /* _mesa_function_pool[7068]: TexParameterf (offset 178) */
   "iif\0"
   "glTexParameterf\0"
   "\0"
   /* _mesa_function_pool[7089]: TexParameteri (offset 180) */
   "iii\0"
   "glTexParameteri\0"
   "\0"
   /* _mesa_function_pool[7110]: NormalPointerEXT (will be remapped) */
   "iiip\0"
   "glNormalPointerEXT\0"
   "\0"
   /* _mesa_function_pool[7135]: MultiTexCoord3dARB (offset 392) */
   "iddd\0"
   "glMultiTexCoord3d\0"
   "glMultiTexCoord3dARB\0"
   "\0"
   /* _mesa_function_pool[7180]: MultiTexCoord2iARB (offset 388) */
   "iii\0"
   "glMultiTexCoord2i\0"
   "glMultiTexCoord2iARB\0"
   "\0"
   /* _mesa_function_pool[7224]: DrawPixels (offset 257) */
   "iiiip\0"
   "glDrawPixels\0"
   "\0"
   /* _mesa_function_pool[7244]: ReplacementCodeuiTexCoord2fNormal3fVertex3fSUN (dynamic) */
   "iffffffff\0"
   "glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[7304]: MultiTexCoord2svARB (offset 391) */
   "ip\0"
   "glMultiTexCoord2sv\0"
   "glMultiTexCoord2svARB\0"
   "\0"
   /* _mesa_function_pool[7349]: ReplacementCodeubvSUN (dynamic) */
   "p\0"
   "glReplacementCodeubvSUN\0"
   "\0"
   /* _mesa_function_pool[7376]: Uniform3iARB (will be remapped) */
   "iiii\0"
   "glUniform3i\0"
   "glUniform3iARB\0"
   "\0"
   /* _mesa_function_pool[7409]: DrawTransformFeedback (will be remapped) */
   "ii\0"
   "glDrawTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[7437]: DrawElementsInstancedARB (will be remapped) */
   "iiipi\0"
   "glDrawElementsInstancedARB\0"
   "glDrawElementsInstancedEXT\0"
   "glDrawElementsInstanced\0"
   "\0"
   /* _mesa_function_pool[7522]: VertexAttribI1iEXT (will be remapped) */
   "ii\0"
   "glVertexAttribI1iEXT\0"
   "glVertexAttribI1i\0"
   "\0"
   /* _mesa_function_pool[7565]: GetShaderInfoLog (will be remapped) */
   "iipp\0"
   "glGetShaderInfoLog\0"
   "\0"
   /* _mesa_function_pool[7590]: WeightivARB (dynamic) */
   "ip\0"
   "glWeightivARB\0"
   "\0"
   /* _mesa_function_pool[7608]: PollInstrumentsSGIX (dynamic) */
   "p\0"
   "glPollInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[7633]: GlobalAlphaFactordSUN (dynamic) */
   "d\0"
   "glGlobalAlphaFactordSUN\0"
   "\0"
   /* _mesa_function_pool[7660]: GetFinalCombinerInputParameterfvNV (dynamic) */
   "iip\0"
   "glGetFinalCombinerInputParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[7702]: GenerateMipmapEXT (will be remapped) */
   "i\0"
   "glGenerateMipmap\0"
   "glGenerateMipmapEXT\0"
   "\0"
   /* _mesa_function_pool[7742]: GenLists (offset 5) */
   "i\0"
   "glGenLists\0"
   "\0"
   /* _mesa_function_pool[7756]: DepthRangef (will be remapped) */
   "ff\0"
   "glDepthRangef\0"
   "\0"
   /* _mesa_function_pool[7774]: GetMapAttribParameterivNV (dynamic) */
   "iiip\0"
   "glGetMapAttribParameterivNV\0"
   "\0"
   /* _mesa_function_pool[7808]: CreateShaderObjectARB (will be remapped) */
   "i\0"
   "glCreateShaderObjectARB\0"
   "\0"
   /* _mesa_function_pool[7835]: GetSharpenTexFuncSGIS (dynamic) */
   "ip\0"
   "glGetSharpenTexFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[7863]: BufferDataARB (will be remapped) */
   "iipi\0"
   "glBufferData\0"
   "glBufferDataARB\0"
   "\0"
   /* _mesa_function_pool[7898]: FlushVertexArrayRangeNV (dynamic) */
   "\0"
   "glFlushVertexArrayRangeNV\0"
   "\0"
   /* _mesa_function_pool[7926]: MapGrid2d (offset 226) */
   "iddidd\0"
   "glMapGrid2d\0"
   "\0"
   /* _mesa_function_pool[7946]: MapGrid2f (offset 227) */
   "iffiff\0"
   "glMapGrid2f\0"
   "\0"
   /* _mesa_function_pool[7966]: SampleMapATI (will be remapped) */
   "iii\0"
   "glSampleMapATI\0"
   "\0"
   /* _mesa_function_pool[7986]: GetActiveUniformBlockName (will be remapped) */
   "iiipp\0"
   "glGetActiveUniformBlockName\0"
   "\0"
   /* _mesa_function_pool[8021]: VertexPointerEXT (will be remapped) */
   "iiiip\0"
   "glVertexPointerEXT\0"
   "\0"
   /* _mesa_function_pool[8047]: GetTexFilterFuncSGIS (dynamic) */
   "iip\0"
   "glGetTexFilterFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[8075]: Scissor (offset 176) */
   "iiii\0"
   "glScissor\0"
   "\0"
   /* _mesa_function_pool[8091]: Fogf (offset 153) */
   "if\0"
   "glFogf\0"
   "\0"
   /* _mesa_function_pool[8102]: ReplacementCodeuiColor4ubVertex3fvSUN (dynamic) */
   "ppp\0"
   "glReplacementCodeuiColor4ubVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[8147]: TexSubImage1D (offset 332) */
   "iiiiiip\0"
   "glTexSubImage1D\0"
   "glTexSubImage1DEXT\0"
   "\0"
   /* _mesa_function_pool[8191]: VertexAttrib1sARB (will be remapped) */
   "ii\0"
   "glVertexAttrib1s\0"
   "glVertexAttrib1sARB\0"
   "\0"
   /* _mesa_function_pool[8232]: FenceSync (will be remapped) */
   "ii\0"
   "glFenceSync\0"
   "\0"
   /* _mesa_function_pool[8248]: Color4usv (offset 40) */
   "p\0"
   "glColor4usv\0"
   "\0"
   /* _mesa_function_pool[8263]: GetnUniformfvARB (will be remapped) */
   "iiip\0"
   "glGetnUniformfvARB\0"
   "\0"
   /* _mesa_function_pool[8288]: Fogi (offset 155) */
   "ii\0"
   "glFogi\0"
   "\0"
   /* _mesa_function_pool[8299]: DepthRange (offset 288) */
   "dd\0"
   "glDepthRange\0"
   "\0"
   /* _mesa_function_pool[8316]: RasterPos3iv (offset 75) */
   "p\0"
   "glRasterPos3iv\0"
   "\0"
   /* _mesa_function_pool[8334]: FinalCombinerInputNV (dynamic) */
   "iiii\0"
   "glFinalCombinerInputNV\0"
   "\0"
   /* _mesa_function_pool[8363]: TexCoord2i (offset 106) */
   "ii\0"
   "glTexCoord2i\0"
   "\0"
   /* _mesa_function_pool[8380]: PixelMapfv (offset 251) */
   "iip\0"
   "glPixelMapfv\0"
   "\0"
   /* _mesa_function_pool[8398]: Color4ui (offset 37) */
   "iiii\0"
   "glColor4ui\0"
   "\0"
   /* _mesa_function_pool[8415]: RasterPos3s (offset 76) */
   "iii\0"
   "glRasterPos3s\0"
   "\0"
   /* _mesa_function_pool[8434]: Color3usv (offset 24) */
   "p\0"
   "glColor3usv\0"
   "\0"
   /* _mesa_function_pool[8449]: FlushRasterSGIX (dynamic) */
   "\0"
   "glFlushRasterSGIX\0"
   "\0"
   /* _mesa_function_pool[8469]: TexCoord2f (offset 104) */
   "ff\0"
   "glTexCoord2f\0"
   "\0"
   /* _mesa_function_pool[8486]: ReplacementCodeuiTexCoord2fVertex3fSUN (dynamic) */
   "ifffff\0"
   "glReplacementCodeuiTexCoord2fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[8535]: TexCoord2d (offset 102) */
   "dd\0"
   "glTexCoord2d\0"
   "\0"
   /* _mesa_function_pool[8552]: RasterPos3d (offset 70) */
   "ddd\0"
   "glRasterPos3d\0"
   "\0"
   /* _mesa_function_pool[8571]: RasterPos3f (offset 72) */
   "fff\0"
   "glRasterPos3f\0"
   "\0"
   /* _mesa_function_pool[8590]: Uniform1fARB (will be remapped) */
   "if\0"
   "glUniform1f\0"
   "glUniform1fARB\0"
   "\0"
   /* _mesa_function_pool[8621]: AreTexturesResident (offset 322) */
   "ipp\0"
   "glAreTexturesResident\0"
   "glAreTexturesResidentEXT\0"
   "\0"
   /* _mesa_function_pool[8673]: DrawElementsInstancedBaseVertexBaseInstance (will be remapped) */
   "iiipiii\0"
   "glDrawElementsInstancedBaseVertexBaseInstance\0"
   "\0"
   /* _mesa_function_pool[8728]: TexCoord2s (offset 108) */
   "ii\0"
   "glTexCoord2s\0"
   "\0"
   /* _mesa_function_pool[8745]: StencilOpSeparate (will be remapped) */
   "iiii\0"
   "glStencilOpSeparate\0"
   "glStencilOpSeparateATI\0"
   "\0"
   /* _mesa_function_pool[8794]: ColorTableParameteriv (offset 341) */
   "iip\0"
   "glColorTableParameteriv\0"
   "glColorTableParameterivSGI\0"
   "\0"
   /* _mesa_function_pool[8850]: VertexAttribP2ui (will be remapped) */
   "iiii\0"
   "glVertexAttribP2ui\0"
   "\0"
   /* _mesa_function_pool[8875]: WindowPos3dMESA (will be remapped) */
   "ddd\0"
   "glWindowPos3d\0"
   "glWindowPos3dARB\0"
   "glWindowPos3dMESA\0"
   "\0"
   /* _mesa_function_pool[8929]: Color4us (offset 39) */
   "iiii\0"
   "glColor4us\0"
   "\0"
   /* _mesa_function_pool[8946]: PointParameterfvEXT (will be remapped) */
   "ip\0"
   "glPointParameterfv\0"
   "glPointParameterfvARB\0"
   "glPointParameterfvEXT\0"
   "glPointParameterfvSGIS\0"
   "\0"
   /* _mesa_function_pool[9036]: Color3bv (offset 10) */
   "p\0"
   "glColor3bv\0"
   "\0"
   /* _mesa_function_pool[9050]: GetnCompressedTexImageARB (will be remapped) */
   "iiip\0"
   "glGetnCompressedTexImageARB\0"
   "\0"
   /* _mesa_function_pool[9084]: WindowPos2fvMESA (will be remapped) */
   "p\0"
   "glWindowPos2fv\0"
   "glWindowPos2fvARB\0"
   "glWindowPos2fvMESA\0"
   "\0"
   /* _mesa_function_pool[9139]: SecondaryColor3bvEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3bv\0"
   "glSecondaryColor3bvEXT\0"
   "\0"
   /* _mesa_function_pool[9185]: GetListParameterfvSGIX (dynamic) */
   "iip\0"
   "glGetListParameterfvSGIX\0"
   "\0"
   /* _mesa_function_pool[9215]: VertexPointerListIBM (dynamic) */
   "iiipi\0"
   "glVertexPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[9245]: GetProgramLocalParameterfvARB (will be remapped) */
   "iip\0"
   "glGetProgramLocalParameterfvARB\0"
   "\0"
   /* _mesa_function_pool[9282]: FragmentMaterialfSGIX (dynamic) */
   "iif\0"
   "glFragmentMaterialfSGIX\0"
   "\0"
   /* _mesa_function_pool[9311]: BindSampler (will be remapped) */
   "ii\0"
   "glBindSampler\0"
   "\0"
   /* _mesa_function_pool[9329]: RenderbufferStorageEXT (will be remapped) */
   "iiii\0"
   "glRenderbufferStorage\0"
   "glRenderbufferStorageEXT\0"
   "\0"
   /* _mesa_function_pool[9382]: IsFenceNV (dynamic) */
   "i\0"
   "glIsFenceNV\0"
   "\0"
   /* _mesa_function_pool[9397]: AlphaFragmentOp2ATI (will be remapped) */
   "iiiiiiiii\0"
   "glAlphaFragmentOp2ATI\0"
   "\0"
   /* _mesa_function_pool[9430]: AttachObjectARB (will be remapped) */
   "ii\0"
   "glAttachObjectARB\0"
   "\0"
   /* _mesa_function_pool[9452]: GetFragmentLightivSGIX (dynamic) */
   "iip\0"
   "glGetFragmentLightivSGIX\0"
   "\0"
   /* _mesa_function_pool[9482]: UniformMatrix2fvARB (will be remapped) */
   "iiip\0"
   "glUniformMatrix2fv\0"
   "glUniformMatrix2fvARB\0"
   "\0"
   /* _mesa_function_pool[9529]: MultiTexCoord2fARB (offset 386) */
   "iff\0"
   "glMultiTexCoord2f\0"
   "glMultiTexCoord2fARB\0"
   "\0"
   /* _mesa_function_pool[9573]: ColorTable (offset 339) */
   "iiiiip\0"
   "glColorTable\0"
   "glColorTableSGI\0"
   "glColorTableEXT\0"
   "\0"
   /* _mesa_function_pool[9626]: IndexPointer (offset 314) */
   "iip\0"
   "glIndexPointer\0"
   "\0"
   /* _mesa_function_pool[9646]: Accum (offset 213) */
   "if\0"
   "glAccum\0"
   "\0"
   /* _mesa_function_pool[9658]: GetTexImage (offset 281) */
   "iiiip\0"
   "glGetTexImage\0"
   "\0"
   /* _mesa_function_pool[9679]: MapControlPointsNV (dynamic) */
   "iiiiiiiip\0"
   "glMapControlPointsNV\0"
   "\0"
   /* _mesa_function_pool[9711]: ConvolutionFilter2D (offset 349) */
   "iiiiiip\0"
   "glConvolutionFilter2D\0"
   "glConvolutionFilter2DEXT\0"
   "\0"
   /* _mesa_function_pool[9767]: Finish (offset 216) */
   "\0"
   "glFinish\0"
   "\0"
   /* _mesa_function_pool[9778]: MapParameterfvNV (dynamic) */
   "iip\0"
   "glMapParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[9802]: ClearStencil (offset 207) */
   "i\0"
   "glClearStencil\0"
   "\0"
   /* _mesa_function_pool[9820]: VertexAttrib3dvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib3dv\0"
   "glVertexAttrib3dvARB\0"
   "\0"
   /* _mesa_function_pool[9863]: Uniform4uivEXT (will be remapped) */
   "iip\0"
   "glUniform4uivEXT\0"
   "glUniform4uiv\0"
   "\0"
   /* _mesa_function_pool[9899]: HintPGI (dynamic) */
   "ii\0"
   "glHintPGI\0"
   "\0"
   /* _mesa_function_pool[9913]: ConvolutionParameteriv (offset 353) */
   "iip\0"
   "glConvolutionParameteriv\0"
   "glConvolutionParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[9971]: Color4s (offset 33) */
   "iiii\0"
   "glColor4s\0"
   "\0"
   /* _mesa_function_pool[9987]: InterleavedArrays (offset 317) */
   "iip\0"
   "glInterleavedArrays\0"
   "\0"
   /* _mesa_function_pool[10012]: RasterPos2fv (offset 65) */
   "p\0"
   "glRasterPos2fv\0"
   "\0"
   /* _mesa_function_pool[10030]: TexCoord1fv (offset 97) */
   "p\0"
   "glTexCoord1fv\0"
   "\0"
   /* _mesa_function_pool[10047]: Vertex2d (offset 126) */
   "dd\0"
   "glVertex2d\0"
   "\0"
   /* _mesa_function_pool[10062]: CullParameterdvEXT (dynamic) */
   "ip\0"
   "glCullParameterdvEXT\0"
   "\0"
   /* _mesa_function_pool[10087]: ProgramNamedParameter4fNV (will be remapped) */
   "iipffff\0"
   "glProgramNamedParameter4fNV\0"
   "\0"
   /* _mesa_function_pool[10124]: Color3fVertex3fSUN (dynamic) */
   "ffffff\0"
   "glColor3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[10153]: ProgramEnvParameter4fvARB (will be remapped) */
   "iip\0"
   "glProgramEnvParameter4fvARB\0"
   "glProgramParameter4fvNV\0"
   "\0"
   /* _mesa_function_pool[10210]: Color4i (offset 31) */
   "iiii\0"
   "glColor4i\0"
   "\0"
   /* _mesa_function_pool[10226]: Color4f (offset 29) */
   "ffff\0"
   "glColor4f\0"
   "\0"
   /* _mesa_function_pool[10242]: RasterPos4fv (offset 81) */
   "p\0"
   "glRasterPos4fv\0"
   "\0"
   /* _mesa_function_pool[10260]: Color4d (offset 27) */
   "dddd\0"
   "glColor4d\0"
   "\0"
   /* _mesa_function_pool[10276]: ClearIndex (offset 205) */
   "f\0"
   "glClearIndex\0"
   "\0"
   /* _mesa_function_pool[10292]: Color4b (offset 25) */
   "iiii\0"
   "glColor4b\0"
   "\0"
   /* _mesa_function_pool[10308]: LoadMatrixd (offset 292) */
   "p\0"
   "glLoadMatrixd\0"
   "\0"
   /* _mesa_function_pool[10325]: FragmentLightModeliSGIX (dynamic) */
   "ii\0"
   "glFragmentLightModeliSGIX\0"
   "\0"
   /* _mesa_function_pool[10355]: RasterPos2dv (offset 63) */
   "p\0"
   "glRasterPos2dv\0"
   "\0"
   /* _mesa_function_pool[10373]: ConvolutionParameterfv (offset 351) */
   "iip\0"
   "glConvolutionParameterfv\0"
   "glConvolutionParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[10431]: TbufferMask3DFX (dynamic) */
   "i\0"
   "glTbufferMask3DFX\0"
   "\0"
   /* _mesa_function_pool[10452]: GetTexGendv (offset 278) */
   "iip\0"
   "glGetTexGendv\0"
   "\0"
   /* _mesa_function_pool[10471]: ColorMaskIndexedEXT (will be remapped) */
   "iiiii\0"
   "glColorMaskIndexedEXT\0"
   "glColorMaski\0"
   "\0"
   /* _mesa_function_pool[10513]: GetFragDataLocationEXT (will be remapped) */
   "ip\0"
   "glGetFragDataLocationEXT\0"
   "glGetFragDataLocation\0"
   "\0"
   /* _mesa_function_pool[10564]: LoadProgramNV (will be remapped) */
   "iiip\0"
   "glLoadProgramNV\0"
   "\0"
   /* _mesa_function_pool[10586]: WaitSync (will be remapped) */
   "iii\0"
   "glWaitSync\0"
   "\0"
   /* _mesa_function_pool[10602]: EndList (offset 1) */
   "\0"
   "glEndList\0"
   "\0"
   /* _mesa_function_pool[10614]: VertexP4ui (will be remapped) */
   "ii\0"
   "glVertexP4ui\0"
   "\0"
   /* _mesa_function_pool[10631]: MultiTexCoordP1ui (will be remapped) */
   "iii\0"
   "glMultiTexCoordP1ui\0"
   "\0"
   /* _mesa_function_pool[10656]: GetAttachedObjectsARB (will be remapped) */
   "iipp\0"
   "glGetAttachedObjectsARB\0"
   "\0"
   /* _mesa_function_pool[10686]: Uniform3fvARB (will be remapped) */
   "iip\0"
   "glUniform3fv\0"
   "glUniform3fvARB\0"
   "\0"
   /* _mesa_function_pool[10720]: EvalCoord1fv (offset 231) */
   "p\0"
   "glEvalCoord1fv\0"
   "\0"
   /* _mesa_function_pool[10738]: DrawRangeElements (offset 338) */
   "iiiiip\0"
   "glDrawRangeElements\0"
   "glDrawRangeElementsEXT\0"
   "\0"
   /* _mesa_function_pool[10789]: EvalMesh2 (offset 238) */
   "iiiii\0"
   "glEvalMesh2\0"
   "\0"
   /* _mesa_function_pool[10808]: Vertex4fv (offset 145) */
   "p\0"
   "glVertex4fv\0"
   "\0"
   /* _mesa_function_pool[10823]: GenTransformFeedbacks (will be remapped) */
   "ip\0"
   "glGenTransformFeedbacks\0"
   "\0"
   /* _mesa_function_pool[10851]: SpriteParameterfvSGIX (dynamic) */
   "ip\0"
   "glSpriteParameterfvSGIX\0"
   "\0"
   /* _mesa_function_pool[10879]: CheckFramebufferStatusEXT (will be remapped) */
   "i\0"
   "glCheckFramebufferStatus\0"
   "glCheckFramebufferStatusEXT\0"
   "\0"
   /* _mesa_function_pool[10935]: GlobalAlphaFactoruiSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactoruiSUN\0"
   "\0"
   /* _mesa_function_pool[10963]: GetHandleARB (will be remapped) */
   "i\0"
   "glGetHandleARB\0"
   "\0"
   /* _mesa_function_pool[10981]: DebugMessageControlARB (will be remapped) */
   "iiiipi\0"
   "glDebugMessageControlARB\0"
   "\0"
   /* _mesa_function_pool[11014]: GetVertexAttribivARB (will be remapped) */
   "iip\0"
   "glGetVertexAttribiv\0"
   "glGetVertexAttribivARB\0"
   "\0"
   /* _mesa_function_pool[11062]: BlendFunciARB (will be remapped) */
   "iii\0"
   "glBlendFunciARB\0"
   "glBlendFuncIndexedAMD\0"
   "\0"
   /* _mesa_function_pool[11105]: VertexAttribP1ui (will be remapped) */
   "iiii\0"
   "glVertexAttribP1ui\0"
   "\0"
   /* _mesa_function_pool[11130]: GetnUniformivARB (will be remapped) */
   "iiip\0"
   "glGetnUniformivARB\0"
   "\0"
   /* _mesa_function_pool[11155]: GetTexParameterIivEXT (will be remapped) */
   "iip\0"
   "glGetTexParameterIivEXT\0"
   "glGetTexParameterIiv\0"
   "\0"
   /* _mesa_function_pool[11205]: CreateProgram (will be remapped) */
   "\0"
   "glCreateProgram\0"
   "\0"
   /* _mesa_function_pool[11223]: LoadTransposeMatrixdARB (will be remapped) */
   "p\0"
   "glLoadTransposeMatrixd\0"
   "glLoadTransposeMatrixdARB\0"
   "\0"
   /* _mesa_function_pool[11275]: ReleaseShaderCompiler (will be remapped) */
   "\0"
   "glReleaseShaderCompiler\0"
   "\0"
   /* _mesa_function_pool[11301]: GetMinmax (offset 364) */
   "iiiip\0"
   "glGetMinmax\0"
   "glGetMinmaxEXT\0"
   "\0"
   /* _mesa_function_pool[11335]: StencilFuncSeparate (will be remapped) */
   "iiii\0"
   "glStencilFuncSeparate\0"
   "\0"
   /* _mesa_function_pool[11363]: SecondaryColor3sEXT (will be remapped) */
   "iii\0"
   "glSecondaryColor3s\0"
   "glSecondaryColor3sEXT\0"
   "\0"
   /* _mesa_function_pool[11409]: Color3fVertex3fvSUN (dynamic) */
   "pp\0"
   "glColor3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[11435]: GetInteger64i_v (will be remapped) */
   "iip\0"
   "glGetInteger64i_v\0"
   "\0"
   /* _mesa_function_pool[11458]: GetVertexAttribdvNV (will be remapped) */
   "iip\0"
   "glGetVertexAttribdvNV\0"
   "\0"
   /* _mesa_function_pool[11485]: DeleteTransformFeedbacks (will be remapped) */
   "ip\0"
   "glDeleteTransformFeedbacks\0"
   "\0"
   /* _mesa_function_pool[11516]: Normal3fv (offset 57) */
   "p\0"
   "glNormal3fv\0"
   "\0"
   /* _mesa_function_pool[11531]: GlobalAlphaFactorbSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactorbSUN\0"
   "\0"
   /* _mesa_function_pool[11558]: Color3us (offset 23) */
   "iii\0"
   "glColor3us\0"
   "\0"
   /* _mesa_function_pool[11574]: ImageTransformParameterfvHP (dynamic) */
   "iip\0"
   "glImageTransformParameterfvHP\0"
   "\0"
   /* _mesa_function_pool[11609]: VertexAttrib4ivARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4iv\0"
   "glVertexAttrib4ivARB\0"
   "\0"
   /* _mesa_function_pool[11652]: End (offset 43) */
   "\0"
   "glEnd\0"
   "\0"
   /* _mesa_function_pool[11660]: VertexAttrib3fNV (will be remapped) */
   "ifff\0"
   "glVertexAttrib3fNV\0"
   "\0"
   /* _mesa_function_pool[11685]: MultiTexCoordP3uiv (will be remapped) */
   "iip\0"
   "glMultiTexCoordP3uiv\0"
   "\0"
   /* _mesa_function_pool[11711]: VertexAttribs2dvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs2dvNV\0"
   "\0"
   /* _mesa_function_pool[11737]: GetQueryObjectui64vEXT (will be remapped) */
   "iip\0"
   "glGetQueryObjectui64v\0"
   "glGetQueryObjectui64vEXT\0"
   "\0"
   /* _mesa_function_pool[11789]: MultiTexCoord3fvARB (offset 395) */
   "ip\0"
   "glMultiTexCoord3fv\0"
   "glMultiTexCoord3fvARB\0"
   "\0"
   /* _mesa_function_pool[11834]: SecondaryColor3dEXT (will be remapped) */
   "ddd\0"
   "glSecondaryColor3d\0"
   "glSecondaryColor3dEXT\0"
   "\0"
   /* _mesa_function_pool[11880]: Color3ub (offset 19) */
   "iii\0"
   "glColor3ub\0"
   "\0"
   /* _mesa_function_pool[11896]: GetProgramParameterfvNV (will be remapped) */
   "iiip\0"
   "glGetProgramParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[11928]: GetActiveUniformsiv (will be remapped) */
   "iipip\0"
   "glGetActiveUniformsiv\0"
   "\0"
   /* _mesa_function_pool[11957]: Color4fNormal3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glColor4fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[11992]: GetInstrumentsSGIX (dynamic) */
   "\0"
   "glGetInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[12015]: GetUniformuivEXT (will be remapped) */
   "iip\0"
   "glGetUniformuivEXT\0"
   "glGetUniformuiv\0"
   "\0"
   /* _mesa_function_pool[12055]: Color3ui (offset 21) */
   "iii\0"
   "glColor3ui\0"
   "\0"
   /* _mesa_function_pool[12071]: EvalMapsNV (dynamic) */
   "ii\0"
   "glEvalMapsNV\0"
   "\0"
   /* _mesa_function_pool[12088]: TexSubImage2D (offset 333) */
   "iiiiiiiip\0"
   "glTexSubImage2D\0"
   "glTexSubImage2DEXT\0"
   "\0"
   /* _mesa_function_pool[12134]: FragmentLightivSGIX (dynamic) */
   "iip\0"
   "glFragmentLightivSGIX\0"
   "\0"
   /* _mesa_function_pool[12161]: GetTexParameterPointervAPPLE (dynamic) */
   "iip\0"
   "glGetTexParameterPointervAPPLE\0"
   "\0"
   /* _mesa_function_pool[12197]: TexGenfv (offset 191) */
   "iip\0"
   "glTexGenfv\0"
   "\0"
   /* _mesa_function_pool[12213]: PixelTransformParameterfvEXT (dynamic) */
   "iip\0"
   "glPixelTransformParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[12249]: VertexAttrib4bvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4bv\0"
   "glVertexAttrib4bvARB\0"
   "\0"
   /* _mesa_function_pool[12292]: ShaderBinary (will be remapped) */
   "ipipi\0"
   "glShaderBinary\0"
   "\0"
   /* _mesa_function_pool[12314]: TexCoordP2uiv (will be remapped) */
   "ip\0"
   "glTexCoordP2uiv\0"
   "\0"
   /* _mesa_function_pool[12334]: MultiTexCoord4sARB (offset 406) */
   "iiiii\0"
   "glMultiTexCoord4s\0"
   "glMultiTexCoord4sARB\0"
   "\0"
   /* _mesa_function_pool[12380]: GetFragmentMaterialivSGIX (dynamic) */
   "iip\0"
   "glGetFragmentMaterialivSGIX\0"
   "\0"
   /* _mesa_function_pool[12413]: WindowPos4dMESA (will be remapped) */
   "dddd\0"
   "glWindowPos4dMESA\0"
   "\0"
   /* _mesa_function_pool[12437]: WeightPointerARB (dynamic) */
   "iiip\0"
   "glWeightPointerARB\0"
   "\0"
   /* _mesa_function_pool[12462]: WindowPos2dMESA (will be remapped) */
   "dd\0"
   "glWindowPos2d\0"
   "glWindowPos2dARB\0"
   "glWindowPos2dMESA\0"
   "\0"
   /* _mesa_function_pool[12515]: FramebufferTexture3DEXT (will be remapped) */
   "iiiiii\0"
   "glFramebufferTexture3D\0"
   "glFramebufferTexture3DEXT\0"
   "\0"
   /* _mesa_function_pool[12572]: BlendEquation (offset 337) */
   "i\0"
   "glBlendEquation\0"
   "glBlendEquationEXT\0"
   "\0"
   /* _mesa_function_pool[12610]: VertexAttrib3dNV (will be remapped) */
   "iddd\0"
   "glVertexAttrib3dNV\0"
   "\0"
   /* _mesa_function_pool[12635]: VertexAttrib3dARB (will be remapped) */
   "iddd\0"
   "glVertexAttrib3d\0"
   "glVertexAttrib3dARB\0"
   "\0"
   /* _mesa_function_pool[12678]: VertexAttribI4usvEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI4usvEXT\0"
   "glVertexAttribI4usv\0"
   "\0"
   /* _mesa_function_pool[12725]: ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN (dynamic) */
   "ppppp\0"
   "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[12789]: VertexAttrib4fARB (will be remapped) */
   "iffff\0"
   "glVertexAttrib4f\0"
   "glVertexAttrib4fARB\0"
   "\0"
   /* _mesa_function_pool[12833]: GetError (offset 261) */
   "\0"
   "glGetError\0"
   "\0"
   /* _mesa_function_pool[12846]: IndexFuncEXT (dynamic) */
   "if\0"
   "glIndexFuncEXT\0"
   "\0"
   /* _mesa_function_pool[12865]: TexCoord3dv (offset 111) */
   "p\0"
   "glTexCoord3dv\0"
   "\0"
   /* _mesa_function_pool[12882]: Indexdv (offset 45) */
   "p\0"
   "glIndexdv\0"
   "\0"
   /* _mesa_function_pool[12895]: FramebufferTexture2DEXT (will be remapped) */
   "iiiii\0"
   "glFramebufferTexture2D\0"
   "glFramebufferTexture2DEXT\0"
   "\0"
   /* _mesa_function_pool[12951]: Normal3s (offset 60) */
   "iii\0"
   "glNormal3s\0"
   "\0"
   /* _mesa_function_pool[12967]: GetObjectParameterivAPPLE (will be remapped) */
   "iiip\0"
   "glGetObjectParameterivAPPLE\0"
   "\0"
   /* _mesa_function_pool[13001]: PushName (offset 201) */
   "i\0"
   "glPushName\0"
   "\0"
   /* _mesa_function_pool[13015]: MultiTexCoord2dvARB (offset 385) */
   "ip\0"
   "glMultiTexCoord2dv\0"
   "glMultiTexCoord2dvARB\0"
   "\0"
   /* _mesa_function_pool[13060]: CullParameterfvEXT (dynamic) */
   "ip\0"
   "glCullParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[13085]: Normal3i (offset 58) */
   "iii\0"
   "glNormal3i\0"
   "\0"
   /* _mesa_function_pool[13101]: ProgramNamedParameter4fvNV (will be remapped) */
   "iipp\0"
   "glProgramNamedParameter4fvNV\0"
   "\0"
   /* _mesa_function_pool[13136]: SecondaryColorPointerEXT (will be remapped) */
   "iiip\0"
   "glSecondaryColorPointer\0"
   "glSecondaryColorPointerEXT\0"
   "\0"
   /* _mesa_function_pool[13193]: VertexAttrib4fvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4fv\0"
   "glVertexAttrib4fvARB\0"
   "\0"
   /* _mesa_function_pool[13236]: PixelTexGenSGIX (dynamic) */
   "i\0"
   "glPixelTexGenSGIX\0"
   "\0"
   /* _mesa_function_pool[13257]: GetActiveUniformARB (will be remapped) */
   "iiipppp\0"
   "glGetActiveUniform\0"
   "glGetActiveUniformARB\0"
   "\0"
   /* _mesa_function_pool[13307]: VertexAttribP1uiv (will be remapped) */
   "iiip\0"
   "glVertexAttribP1uiv\0"
   "\0"
   /* _mesa_function_pool[13333]: Normal3b (offset 52) */
   "iii\0"
   "glNormal3b\0"
   "\0"
   /* _mesa_function_pool[13349]: Normal3d (offset 54) */
   "ddd\0"
   "glNormal3d\0"
   "\0"
   /* _mesa_function_pool[13365]: Uniform1uiEXT (will be remapped) */
   "ii\0"
   "glUniform1uiEXT\0"
   "glUniform1ui\0"
   "\0"
   /* _mesa_function_pool[13398]: Normal3f (offset 56) */
   "fff\0"
   "glNormal3f\0"
   "\0"
   /* _mesa_function_pool[13414]: MultiTexCoord1svARB (offset 383) */
   "ip\0"
   "glMultiTexCoord1sv\0"
   "glMultiTexCoord1svARB\0"
   "\0"
   /* _mesa_function_pool[13459]: Indexi (offset 48) */
   "i\0"
   "glIndexi\0"
   "\0"
   /* _mesa_function_pool[13471]: EGLImageTargetTexture2DOES (will be remapped) */
   "ip\0"
   "glEGLImageTargetTexture2DOES\0"
   "\0"
   /* _mesa_function_pool[13504]: EndQueryARB (will be remapped) */
   "i\0"
   "glEndQuery\0"
   "glEndQueryARB\0"
   "\0"
   /* _mesa_function_pool[13532]: DeleteFencesNV (dynamic) */
   "ip\0"
   "glDeleteFencesNV\0"
   "\0"
   /* _mesa_function_pool[13553]: ColorPointerListIBM (dynamic) */
   "iiipi\0"
   "glColorPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[13582]: BindBufferRangeEXT (will be remapped) */
   "iiiii\0"
   "glBindBufferRange\0"
   "glBindBufferRangeEXT\0"
   "\0"
   /* _mesa_function_pool[13628]: DepthMask (offset 211) */
   "i\0"
   "glDepthMask\0"
   "\0"
   /* _mesa_function_pool[13643]: IsShader (will be remapped) */
   "i\0"
   "glIsShader\0"
   "\0"
   /* _mesa_function_pool[13657]: Indexf (offset 46) */
   "f\0"
   "glIndexf\0"
   "\0"
   /* _mesa_function_pool[13669]: GetImageTransformParameterivHP (dynamic) */
   "iip\0"
   "glGetImageTransformParameterivHP\0"
   "\0"
   /* _mesa_function_pool[13707]: Indexd (offset 44) */
   "d\0"
   "glIndexd\0"
   "\0"
   /* _mesa_function_pool[13719]: GetMaterialiv (offset 270) */
   "iip\0"
   "glGetMaterialiv\0"
   "\0"
   /* _mesa_function_pool[13740]: StencilOp (offset 244) */
   "iii\0"
   "glStencilOp\0"
   "\0"
   /* _mesa_function_pool[13757]: WindowPos4ivMESA (will be remapped) */
   "p\0"
   "glWindowPos4ivMESA\0"
   "\0"
   /* _mesa_function_pool[13779]: MultiTexCoord3svARB (offset 399) */
   "ip\0"
   "glMultiTexCoord3sv\0"
   "glMultiTexCoord3svARB\0"
   "\0"
   /* _mesa_function_pool[13824]: TexEnvfv (offset 185) */
   "iip\0"
   "glTexEnvfv\0"
   "\0"
   /* _mesa_function_pool[13840]: MultiTexCoord4iARB (offset 404) */
   "iiiii\0"
   "glMultiTexCoord4i\0"
   "glMultiTexCoord4iARB\0"
   "\0"
   /* _mesa_function_pool[13886]: Indexs (offset 50) */
   "i\0"
   "glIndexs\0"
   "\0"
   /* _mesa_function_pool[13898]: TexCoordP3ui (will be remapped) */
   "ii\0"
   "glTexCoordP3ui\0"
   "\0"
   /* _mesa_function_pool[13917]: ResizeBuffersMESA (will be remapped) */
   "\0"
   "glResizeBuffersMESA\0"
   "\0"
   /* _mesa_function_pool[13939]: MultiTexCoordP1uiv (will be remapped) */
   "iip\0"
   "glMultiTexCoordP1uiv\0"
   "\0"
   /* _mesa_function_pool[13965]: BlendFuncSeparateiARB (will be remapped) */
   "iiiii\0"
   "glBlendFuncSeparateiARB\0"
   "glBlendFuncSeparateIndexedAMD\0"
   "\0"
   /* _mesa_function_pool[14026]: GetUniformivARB (will be remapped) */
   "iip\0"
   "glGetUniformiv\0"
   "glGetUniformivARB\0"
   "\0"
   /* _mesa_function_pool[14064]: PixelTexGenParameteriSGIS (dynamic) */
   "ii\0"
   "glPixelTexGenParameteriSGIS\0"
   "\0"
   /* _mesa_function_pool[14096]: VertexPointervINTEL (dynamic) */
   "iip\0"
   "glVertexPointervINTEL\0"
   "\0"
   /* _mesa_function_pool[14123]: Vertex2i (offset 130) */
   "ii\0"
   "glVertex2i\0"
   "\0"
   /* _mesa_function_pool[14138]: LoadMatrixf (offset 291) */
   "p\0"
   "glLoadMatrixf\0"
   "\0"
   /* _mesa_function_pool[14155]: VertexAttribI1uivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI1uivEXT\0"
   "glVertexAttribI1uiv\0"
   "\0"
   /* _mesa_function_pool[14202]: Vertex2f (offset 128) */
   "ff\0"
   "glVertex2f\0"
   "\0"
   /* _mesa_function_pool[14217]: ReplacementCodeuiColor4fNormal3fVertex3fvSUN (dynamic) */
   "pppp\0"
   "glReplacementCodeuiColor4fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[14270]: Color4bv (offset 26) */
   "p\0"
   "glColor4bv\0"
   "\0"
   /* _mesa_function_pool[14284]: VertexPointer (offset 321) */
   "iiip\0"
   "glVertexPointer\0"
   "\0"
   /* _mesa_function_pool[14306]: SecondaryColor3uiEXT (will be remapped) */
   "iii\0"
   "glSecondaryColor3ui\0"
   "glSecondaryColor3uiEXT\0"
   "\0"
   /* _mesa_function_pool[14354]: StartInstrumentsSGIX (dynamic) */
   "\0"
   "glStartInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[14379]: SecondaryColor3usvEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3usv\0"
   "glSecondaryColor3usvEXT\0"
   "\0"
   /* _mesa_function_pool[14427]: VertexAttrib2fvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib2fvNV\0"
   "\0"
   /* _mesa_function_pool[14451]: ProgramLocalParameter4dvARB (will be remapped) */
   "iip\0"
   "glProgramLocalParameter4dvARB\0"
   "\0"
   /* _mesa_function_pool[14486]: DeleteLists (offset 4) */
   "ii\0"
   "glDeleteLists\0"
   "\0"
   /* _mesa_function_pool[14504]: LogicOp (offset 242) */
   "i\0"
   "glLogicOp\0"
   "\0"
   /* _mesa_function_pool[14517]: MatrixIndexuivARB (dynamic) */
   "ip\0"
   "glMatrixIndexuivARB\0"
   "\0"
   /* _mesa_function_pool[14541]: Vertex2s (offset 132) */
   "ii\0"
   "glVertex2s\0"
   "\0"
   /* _mesa_function_pool[14556]: RenderbufferStorageMultisample (will be remapped) */
   "iiiii\0"
   "glRenderbufferStorageMultisample\0"
   "glRenderbufferStorageMultisampleEXT\0"
   "\0"
   /* _mesa_function_pool[14632]: TexCoord4fv (offset 121) */
   "p\0"
   "glTexCoord4fv\0"
   "\0"
   /* _mesa_function_pool[14649]: Tangent3sEXT (dynamic) */
   "iii\0"
   "glTangent3sEXT\0"
   "\0"
   /* _mesa_function_pool[14669]: GlobalAlphaFactorfSUN (dynamic) */
   "f\0"
   "glGlobalAlphaFactorfSUN\0"
   "\0"
   /* _mesa_function_pool[14696]: MultiTexCoord3iARB (offset 396) */
   "iiii\0"
   "glMultiTexCoord3i\0"
   "glMultiTexCoord3iARB\0"
   "\0"
   /* _mesa_function_pool[14741]: IsProgram (will be remapped) */
   "i\0"
   "glIsProgram\0"
   "\0"
   /* _mesa_function_pool[14756]: TexCoordPointerListIBM (dynamic) */
   "iiipi\0"
   "glTexCoordPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[14788]: VertexAttribI4svEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI4svEXT\0"
   "glVertexAttribI4sv\0"
   "\0"
   /* _mesa_function_pool[14833]: GlobalAlphaFactorusSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactorusSUN\0"
   "\0"
   /* _mesa_function_pool[14861]: ColorP4uiv (will be remapped) */
   "ip\0"
   "glColorP4uiv\0"
   "\0"
   /* _mesa_function_pool[14878]: FramebufferRenderbufferEXT (will be remapped) */
   "iiii\0"
   "glFramebufferRenderbuffer\0"
   "glFramebufferRenderbufferEXT\0"
   "\0"
   /* _mesa_function_pool[14939]: ClearBufferuiv (will be remapped) */
   "iip\0"
   "glClearBufferuiv\0"
   "\0"
   /* _mesa_function_pool[14961]: VertexAttrib1dvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib1dvNV\0"
   "\0"
   /* _mesa_function_pool[14985]: GenTextures (offset 328) */
   "ip\0"
   "glGenTextures\0"
   "glGenTexturesEXT\0"
   "\0"
   /* _mesa_function_pool[15020]: FramebufferTextureARB (will be remapped) */
   "iiii\0"
   "glFramebufferTextureARB\0"
   "\0"
   /* _mesa_function_pool[15050]: MultiTexCoordP4uiv (will be remapped) */
   "iip\0"
   "glMultiTexCoordP4uiv\0"
   "\0"
   /* _mesa_function_pool[15076]: FramebufferTexture1DEXT (will be remapped) */
   "iiiii\0"
   "glFramebufferTexture1D\0"
   "glFramebufferTexture1DEXT\0"
   "\0"
   /* _mesa_function_pool[15132]: GetCombinerOutputParameterivNV (dynamic) */
   "iiip\0"
   "glGetCombinerOutputParameterivNV\0"
   "\0"
   /* _mesa_function_pool[15171]: PixelTexGenParameterivSGIS (dynamic) */
   "ip\0"
   "glPixelTexGenParameterivSGIS\0"
   "\0"
   /* _mesa_function_pool[15204]: TextureNormalEXT (dynamic) */
   "i\0"
   "glTextureNormalEXT\0"
   "\0"
   /* _mesa_function_pool[15226]: IndexPointerListIBM (dynamic) */
   "iipi\0"
   "glIndexPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[15254]: WeightfvARB (dynamic) */
   "ip\0"
   "glWeightfvARB\0"
   "\0"
   /* _mesa_function_pool[15272]: MultiTexCoordP3ui (will be remapped) */
   "iii\0"
   "glMultiTexCoordP3ui\0"
   "\0"
   /* _mesa_function_pool[15297]: ClearDebugLogMESA (dynamic) */
   "iii\0"
   "glClearDebugLogMESA\0"
   "\0"
   /* _mesa_function_pool[15322]: RasterPos2sv (offset 69) */
   "p\0"
   "glRasterPos2sv\0"
   "\0"
   /* _mesa_function_pool[15340]: Color4ubv (offset 36) */
   "p\0"
   "glColor4ubv\0"
   "\0"
   /* _mesa_function_pool[15355]: DrawBuffer (offset 202) */
   "i\0"
   "glDrawBuffer\0"
   "\0"
   /* _mesa_function_pool[15371]: TexCoord2fv (offset 105) */
   "p\0"
   "glTexCoord2fv\0"
   "\0"
   /* _mesa_function_pool[15388]: BeginFragmentShaderATI (will be remapped) */
   "\0"
   "glBeginFragmentShaderATI\0"
   "\0"
   /* _mesa_function_pool[15415]: WindowPos4fMESA (will be remapped) */
   "ffff\0"
   "glWindowPos4fMESA\0"
   "\0"
   /* _mesa_function_pool[15439]: TexCoord1sv (offset 101) */
   "p\0"
   "glTexCoord1sv\0"
   "\0"
   /* _mesa_function_pool[15456]: WindowPos3dvMESA (will be remapped) */
   "p\0"
   "glWindowPos3dv\0"
   "glWindowPos3dvARB\0"
   "glWindowPos3dvMESA\0"
   "\0"
   /* _mesa_function_pool[15511]: VertexAttribP4ui (will be remapped) */
   "iiii\0"
   "glVertexAttribP4ui\0"
   "\0"
   /* _mesa_function_pool[15536]: DepthFunc (offset 245) */
   "i\0"
   "glDepthFunc\0"
   "\0"
   /* _mesa_function_pool[15551]: PixelMapusv (offset 253) */
   "iip\0"
   "glPixelMapusv\0"
   "\0"
   /* _mesa_function_pool[15570]: GetQueryObjecti64vEXT (will be remapped) */
   "iip\0"
   "glGetQueryObjecti64v\0"
   "glGetQueryObjecti64vEXT\0"
   "\0"
   /* _mesa_function_pool[15620]: MultiTexCoord1dARB (offset 376) */
   "id\0"
   "glMultiTexCoord1d\0"
   "glMultiTexCoord1dARB\0"
   "\0"
   /* _mesa_function_pool[15663]: PointParameterivNV (will be remapped) */
   "ip\0"
   "glPointParameteriv\0"
   "glPointParameterivNV\0"
   "\0"
   /* _mesa_function_pool[15707]: IsSampler (will be remapped) */
   "i\0"
   "glIsSampler\0"
   "\0"
   /* _mesa_function_pool[15722]: BlendFunc (offset 241) */
   "ii\0"
   "glBlendFunc\0"
   "\0"
   /* _mesa_function_pool[15738]: ColorP3ui (will be remapped) */
   "ii\0"
   "glColorP3ui\0"
   "\0"
   /* _mesa_function_pool[15754]: Uniform2fvARB (will be remapped) */
   "iip\0"
   "glUniform2fv\0"
   "glUniform2fvARB\0"
   "\0"
   /* _mesa_function_pool[15788]: BufferParameteriAPPLE (will be remapped) */
   "iii\0"
   "glBufferParameteriAPPLE\0"
   "\0"
   /* _mesa_function_pool[15817]: MultiTexCoord3dvARB (offset 393) */
   "ip\0"
   "glMultiTexCoord3dv\0"
   "glMultiTexCoord3dvARB\0"
   "\0"
   /* _mesa_function_pool[15862]: ReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN (dynamic) */
   "pppp\0"
   "glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[15918]: DeleteObjectARB (will be remapped) */
   "i\0"
   "glDeleteObjectARB\0"
   "\0"
   /* _mesa_function_pool[15939]: GetShaderPrecisionFormat (will be remapped) */
   "iipp\0"
   "glGetShaderPrecisionFormat\0"
   "\0"
   /* _mesa_function_pool[15972]: MatrixIndexPointerARB (dynamic) */
   "iiip\0"
   "glMatrixIndexPointerARB\0"
   "\0"
   /* _mesa_function_pool[16002]: ProgramNamedParameter4dvNV (will be remapped) */
   "iipp\0"
   "glProgramNamedParameter4dvNV\0"
   "\0"
   /* _mesa_function_pool[16037]: Tangent3fvEXT (dynamic) */
   "p\0"
   "glTangent3fvEXT\0"
   "\0"
   /* _mesa_function_pool[16056]: Flush (offset 217) */
   "\0"
   "glFlush\0"
   "\0"
   /* _mesa_function_pool[16066]: Color4uiv (offset 38) */
   "p\0"
   "glColor4uiv\0"
   "\0"
   /* _mesa_function_pool[16081]: VertexAttribI4iEXT (will be remapped) */
   "iiiii\0"
   "glVertexAttribI4iEXT\0"
   "glVertexAttribI4i\0"
   "\0"
   /* _mesa_function_pool[16127]: GenVertexArrays (will be remapped) */
   "ip\0"
   "glGenVertexArrays\0"
   "\0"
   /* _mesa_function_pool[16149]: Uniform3uivEXT (will be remapped) */
   "iip\0"
   "glUniform3uivEXT\0"
   "glUniform3uiv\0"
   "\0"
   /* _mesa_function_pool[16185]: RasterPos3sv (offset 77) */
   "p\0"
   "glRasterPos3sv\0"
   "\0"
   /* _mesa_function_pool[16203]: TexCoordP2ui (will be remapped) */
   "ii\0"
   "glTexCoordP2ui\0"
   "\0"
   /* _mesa_function_pool[16222]: BindFramebufferEXT (will be remapped) */
   "ii\0"
   "glBindFramebuffer\0"
   "glBindFramebufferEXT\0"
   "\0"
   /* _mesa_function_pool[16265]: ReferencePlaneSGIX (dynamic) */
   "p\0"
   "glReferencePlaneSGIX\0"
   "\0"
   /* _mesa_function_pool[16289]: PushAttrib (offset 219) */
   "i\0"
   "glPushAttrib\0"
   "\0"
   /* _mesa_function_pool[16305]: RasterPos2i (offset 66) */
   "ii\0"
   "glRasterPos2i\0"
   "\0"
   /* _mesa_function_pool[16323]: ValidateProgramARB (will be remapped) */
   "i\0"
   "glValidateProgram\0"
   "glValidateProgramARB\0"
   "\0"
   /* _mesa_function_pool[16365]: TexParameteriv (offset 181) */
   "iip\0"
   "glTexParameteriv\0"
   "\0"
   /* _mesa_function_pool[16387]: UnlockArraysEXT (will be remapped) */
   "\0"
   "glUnlockArraysEXT\0"
   "\0"
   /* _mesa_function_pool[16407]: TexCoord2fColor3fVertex3fSUN (dynamic) */
   "ffffffff\0"
   "glTexCoord2fColor3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[16448]: WindowPos3fvMESA (will be remapped) */
   "p\0"
   "glWindowPos3fv\0"
   "glWindowPos3fvARB\0"
   "glWindowPos3fvMESA\0"
   "\0"
   /* _mesa_function_pool[16503]: RasterPos2f (offset 64) */
   "ff\0"
   "glRasterPos2f\0"
   "\0"
   /* _mesa_function_pool[16521]: VertexAttrib1svNV (will be remapped) */
   "ip\0"
   "glVertexAttrib1svNV\0"
   "\0"
   /* _mesa_function_pool[16545]: RasterPos2d (offset 62) */
   "dd\0"
   "glRasterPos2d\0"
   "\0"
   /* _mesa_function_pool[16563]: RasterPos3fv (offset 73) */
   "p\0"
   "glRasterPos3fv\0"
   "\0"
   /* _mesa_function_pool[16581]: CopyTexSubImage3D (offset 373) */
   "iiiiiiiii\0"
   "glCopyTexSubImage3D\0"
   "glCopyTexSubImage3DEXT\0"
   "\0"
   /* _mesa_function_pool[16635]: VertexAttrib2dARB (will be remapped) */
   "idd\0"
   "glVertexAttrib2d\0"
   "glVertexAttrib2dARB\0"
   "\0"
   /* _mesa_function_pool[16677]: Color4ub (offset 35) */
   "iiii\0"
   "glColor4ub\0"
   "\0"
   /* _mesa_function_pool[16694]: GetInteger64v (will be remapped) */
   "ip\0"
   "glGetInteger64v\0"
   "\0"
   /* _mesa_function_pool[16714]: TextureColorMaskSGIS (dynamic) */
   "iiii\0"
   "glTextureColorMaskSGIS\0"
   "\0"
   /* _mesa_function_pool[16743]: RasterPos2s (offset 68) */
   "ii\0"
   "glRasterPos2s\0"
   "\0"
   /* _mesa_function_pool[16761]: GetColorTable (offset 343) */
   "iiip\0"
   "glGetColorTable\0"
   "glGetColorTableSGI\0"
   "glGetColorTableEXT\0"
   "\0"
   /* _mesa_function_pool[16821]: EndQueryIndexed (will be remapped) */
   "ii\0"
   "glEndQueryIndexed\0"
   "\0"
   /* _mesa_function_pool[16843]: SelectBuffer (offset 195) */
   "ip\0"
   "glSelectBuffer\0"
   "\0"
   /* _mesa_function_pool[16862]: Indexiv (offset 49) */
   "p\0"
   "glIndexiv\0"
   "\0"
   /* _mesa_function_pool[16875]: TexCoord3i (offset 114) */
   "iii\0"
   "glTexCoord3i\0"
   "\0"
   /* _mesa_function_pool[16893]: CopyColorTable (offset 342) */
   "iiiii\0"
   "glCopyColorTable\0"
   "glCopyColorTableSGI\0"
   "\0"
   /* _mesa_function_pool[16937]: GetHistogramParameterfv (offset 362) */
   "iip\0"
   "glGetHistogramParameterfv\0"
   "glGetHistogramParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[16997]: Frustum (offset 289) */
   "dddddd\0"
   "glFrustum\0"
   "\0"
   /* _mesa_function_pool[17015]: GetString (offset 275) */
   "i\0"
   "glGetString\0"
   "\0"
   /* _mesa_function_pool[17030]: ColorPointervINTEL (dynamic) */
   "iip\0"
   "glColorPointervINTEL\0"
   "\0"
   /* _mesa_function_pool[17056]: TexEnvf (offset 184) */
   "iif\0"
   "glTexEnvf\0"
   "\0"
   /* _mesa_function_pool[17071]: VertexP2uiv (will be remapped) */
   "ip\0"
   "glVertexP2uiv\0"
   "\0"
   /* _mesa_function_pool[17089]: TexCoord3d (offset 110) */
   "ddd\0"
   "glTexCoord3d\0"
   "\0"
   /* _mesa_function_pool[17107]: AlphaFragmentOp1ATI (will be remapped) */
   "iiiiii\0"
   "glAlphaFragmentOp1ATI\0"
   "\0"
   /* _mesa_function_pool[17137]: TexCoord3f (offset 112) */
   "fff\0"
   "glTexCoord3f\0"
   "\0"
   /* _mesa_function_pool[17155]: MultiTexCoord3ivARB (offset 397) */
   "ip\0"
   "glMultiTexCoord3iv\0"
   "glMultiTexCoord3ivARB\0"
   "\0"
   /* _mesa_function_pool[17200]: MultiTexCoord2sARB (offset 390) */
   "iii\0"
   "glMultiTexCoord2s\0"
   "glMultiTexCoord2sARB\0"
   "\0"
   /* _mesa_function_pool[17244]: VertexAttrib1dvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib1dv\0"
   "glVertexAttrib1dvARB\0"
   "\0"
   /* _mesa_function_pool[17287]: GetnHistogramARB (will be remapped) */
   "iiiiip\0"
   "glGetnHistogramARB\0"
   "\0"
   /* _mesa_function_pool[17314]: DeleteTextures (offset 327) */
   "ip\0"
   "glDeleteTextures\0"
   "glDeleteTexturesEXT\0"
   "\0"
   /* _mesa_function_pool[17355]: TexCoordPointerEXT (will be remapped) */
   "iiiip\0"
   "glTexCoordPointerEXT\0"
   "\0"
   /* _mesa_function_pool[17383]: TexSubImage4DSGIS (dynamic) */
   "iiiiiiiiiiiip\0"
   "glTexSubImage4DSGIS\0"
   "\0"
   /* _mesa_function_pool[17418]: TexCoord3s (offset 116) */
   "iii\0"
   "glTexCoord3s\0"
   "\0"
   /* _mesa_function_pool[17436]: GetTexLevelParameteriv (offset 285) */
   "iiip\0"
   "glGetTexLevelParameteriv\0"
   "\0"
   /* _mesa_function_pool[17467]: CombinerStageParameterfvNV (dynamic) */
   "iip\0"
   "glCombinerStageParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[17501]: VertexAttribP4uiv (will be remapped) */
   "iiip\0"
   "glVertexAttribP4uiv\0"
   "\0"
   /* _mesa_function_pool[17527]: StopInstrumentsSGIX (dynamic) */
   "i\0"
   "glStopInstrumentsSGIX\0"
   "\0"
   /* _mesa_function_pool[17552]: TexCoord4fColor4fNormal3fVertex4fSUN (dynamic) */
   "fffffffffffffff\0"
   "glTexCoord4fColor4fNormal3fVertex4fSUN\0"
   "\0"
   /* _mesa_function_pool[17608]: ClearAccum (offset 204) */
   "ffff\0"
   "glClearAccum\0"
   "\0"
   /* _mesa_function_pool[17627]: DeformSGIX (dynamic) */
   "i\0"
   "glDeformSGIX\0"
   "\0"
   /* _mesa_function_pool[17643]: InvalidateBufferSubData (will be remapped) */
   "iii\0"
   "glInvalidateBufferSubData\0"
   "\0"
   /* _mesa_function_pool[17674]: GetVertexAttribfvARB (will be remapped) */
   "iip\0"
   "glGetVertexAttribfv\0"
   "glGetVertexAttribfvARB\0"
   "\0"
   /* _mesa_function_pool[17722]: SecondaryColor3ivEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3iv\0"
   "glSecondaryColor3ivEXT\0"
   "\0"
   /* _mesa_function_pool[17768]: TexCoord4iv (offset 123) */
   "p\0"
   "glTexCoord4iv\0"
   "\0"
   /* _mesa_function_pool[17785]: TexStorage3D (will be remapped) */
   "iiiiii\0"
   "glTexStorage3D\0"
   "\0"
   /* _mesa_function_pool[17808]: GetFragmentMaterialfvSGIX (dynamic) */
   "iip\0"
   "glGetFragmentMaterialfvSGIX\0"
   "\0"
   /* _mesa_function_pool[17841]: UniformMatrix4x2fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix4x2fv\0"
   "\0"
   /* _mesa_function_pool[17868]: GetDetailTexFuncSGIS (dynamic) */
   "ip\0"
   "glGetDetailTexFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[17895]: GetCombinerStageParameterfvNV (dynamic) */
   "iip\0"
   "glGetCombinerStageParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[17932]: SamplerParameterIiv (will be remapped) */
   "iip\0"
   "glSamplerParameterIiv\0"
   "\0"
   /* _mesa_function_pool[17959]: PolygonOffset (offset 319) */
   "ff\0"
   "glPolygonOffset\0"
   "\0"
   /* _mesa_function_pool[17979]: BindVertexArray (will be remapped) */
   "i\0"
   "glBindVertexArray\0"
   "\0"
   /* _mesa_function_pool[18000]: Color4ubVertex2fvSUN (dynamic) */
   "pp\0"
   "glColor4ubVertex2fvSUN\0"
   "\0"
   /* _mesa_function_pool[18027]: ImageTransformParameterivHP (dynamic) */
   "iip\0"
   "glImageTransformParameterivHP\0"
   "\0"
   /* _mesa_function_pool[18062]: Rectd (offset 86) */
   "dddd\0"
   "glRectd\0"
   "\0"
   /* _mesa_function_pool[18076]: TexFilterFuncSGIS (dynamic) */
   "iiip\0"
   "glTexFilterFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[18102]: SampleMaskSGIS (will be remapped) */
   "fi\0"
   "glSampleMaskSGIS\0"
   "glSampleMaskEXT\0"
   "\0"
   /* _mesa_function_pool[18139]: Binormal3ivEXT (dynamic) */
   "p\0"
   "glBinormal3ivEXT\0"
   "\0"
   /* _mesa_function_pool[18159]: SamplerParameterfv (will be remapped) */
   "iip\0"
   "glSamplerParameterfv\0"
   "\0"
   /* _mesa_function_pool[18185]: VertexAttribI4ubvEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI4ubvEXT\0"
   "glVertexAttribI4ubv\0"
   "\0"
   /* _mesa_function_pool[18232]: GetAttribLocationARB (will be remapped) */
   "ip\0"
   "glGetAttribLocation\0"
   "glGetAttribLocationARB\0"
   "\0"
   /* _mesa_function_pool[18279]: RasterPos3i (offset 74) */
   "iii\0"
   "glRasterPos3i\0"
   "\0"
   /* _mesa_function_pool[18298]: VertexAttrib4ubvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4ubv\0"
   "glVertexAttrib4ubvARB\0"
   "\0"
   /* _mesa_function_pool[18343]: DetailTexFuncSGIS (dynamic) */
   "iip\0"
   "glDetailTexFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[18368]: Normal3fVertex3fSUN (dynamic) */
   "ffffff\0"
   "glNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[18398]: CopyTexImage2D (offset 324) */
   "iiiiiiii\0"
   "glCopyTexImage2D\0"
   "glCopyTexImage2DEXT\0"
   "\0"
   /* _mesa_function_pool[18445]: GetBufferPointervARB (will be remapped) */
   "iip\0"
   "glGetBufferPointerv\0"
   "glGetBufferPointervARB\0"
   "\0"
   /* _mesa_function_pool[18493]: ProgramEnvParameter4fARB (will be remapped) */
   "iiffff\0"
   "glProgramEnvParameter4fARB\0"
   "glProgramParameter4fNV\0"
   "\0"
   /* _mesa_function_pool[18551]: Uniform3ivARB (will be remapped) */
   "iip\0"
   "glUniform3iv\0"
   "glUniform3ivARB\0"
   "\0"
   /* _mesa_function_pool[18585]: Lightfv (offset 160) */
   "iip\0"
   "glLightfv\0"
   "\0"
   /* _mesa_function_pool[18600]: PrimitiveRestartIndexNV (will be remapped) */
   "i\0"
   "glPrimitiveRestartIndex\0"
   "glPrimitiveRestartIndexNV\0"
   "\0"
   /* _mesa_function_pool[18653]: ClearDepth (offset 208) */
   "d\0"
   "glClearDepth\0"
   "\0"
   /* _mesa_function_pool[18669]: GetFenceivNV (dynamic) */
   "iip\0"
   "glGetFenceivNV\0"
   "\0"
   /* _mesa_function_pool[18689]: WindowPos4dvMESA (will be remapped) */
   "p\0"
   "glWindowPos4dvMESA\0"
   "\0"
   /* _mesa_function_pool[18711]: ColorSubTable (offset 346) */
   "iiiiip\0"
   "glColorSubTable\0"
   "glColorSubTableEXT\0"
   "\0"
   /* _mesa_function_pool[18754]: Color4fv (offset 30) */
   "p\0"
   "glColor4fv\0"
   "\0"
   /* _mesa_function_pool[18768]: MultiTexCoord4ivARB (offset 405) */
   "ip\0"
   "glMultiTexCoord4iv\0"
   "glMultiTexCoord4ivARB\0"
   "\0"
   /* _mesa_function_pool[18813]: GetnMinmaxARB (will be remapped) */
   "iiiiip\0"
   "glGetnMinmaxARB\0"
   "\0"
   /* _mesa_function_pool[18837]: ProgramLocalParameters4fvEXT (will be remapped) */
   "iiip\0"
   "glProgramLocalParameters4fvEXT\0"
   "\0"
   /* _mesa_function_pool[18874]: DrawTransformFeedbackStreamInstanced (will be remapped) */
   "iiii\0"
   "glDrawTransformFeedbackStreamInstanced\0"
   "\0"
   /* _mesa_function_pool[18919]: ResumeTransformFeedback (will be remapped) */
   "\0"
   "glResumeTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[18947]: ColorPointer (offset 308) */
   "iiip\0"
   "glColorPointer\0"
   "\0"
   /* _mesa_function_pool[18968]: Rects (offset 92) */
   "iiii\0"
   "glRects\0"
   "\0"
   /* _mesa_function_pool[18982]: GetMapAttribParameterfvNV (dynamic) */
   "iiip\0"
   "glGetMapAttribParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[19016]: MultiTexCoordP2ui (will be remapped) */
   "iii\0"
   "glMultiTexCoordP2ui\0"
   "\0"
   /* _mesa_function_pool[19041]: ActiveProgramEXT (will be remapped) */
   "i\0"
   "glActiveProgramEXT\0"
   "\0"
   /* _mesa_function_pool[19063]: Lightiv (offset 162) */
   "iip\0"
   "glLightiv\0"
   "\0"
   /* _mesa_function_pool[19078]: VertexAttrib4sARB (will be remapped) */
   "iiiii\0"
   "glVertexAttrib4s\0"
   "glVertexAttrib4sARB\0"
   "\0"
   /* _mesa_function_pool[19122]: GetQueryObjectuivARB (will be remapped) */
   "iip\0"
   "glGetQueryObjectuiv\0"
   "glGetQueryObjectuivARB\0"
   "\0"
   /* _mesa_function_pool[19170]: GetTexParameteriv (offset 283) */
   "iip\0"
   "glGetTexParameteriv\0"
   "\0"
   /* _mesa_function_pool[19195]: MapParameterivNV (dynamic) */
   "iip\0"
   "glMapParameterivNV\0"
   "\0"
   /* _mesa_function_pool[19219]: GenRenderbuffersEXT (will be remapped) */
   "ip\0"
   "glGenRenderbuffers\0"
   "glGenRenderbuffersEXT\0"
   "\0"
   /* _mesa_function_pool[19264]: VertexAttribs4dvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs4dvNV\0"
   "\0"
   /* _mesa_function_pool[19290]: ClearBufferfv (will be remapped) */
   "iip\0"
   "glClearBufferfv\0"
   "\0"
   /* _mesa_function_pool[19311]: VertexAttrib2dvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib2dv\0"
   "glVertexAttrib2dvARB\0"
   "\0"
   /* _mesa_function_pool[19354]: EdgeFlagPointerEXT (will be remapped) */
   "iip\0"
   "glEdgeFlagPointerEXT\0"
   "\0"
   /* _mesa_function_pool[19380]: VertexAttribs2svNV (will be remapped) */
   "iip\0"
   "glVertexAttribs2svNV\0"
   "\0"
   /* _mesa_function_pool[19406]: WeightbvARB (dynamic) */
   "ip\0"
   "glWeightbvARB\0"
   "\0"
   /* _mesa_function_pool[19424]: VertexAttrib2fvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib2fv\0"
   "glVertexAttrib2fvARB\0"
   "\0"
   /* _mesa_function_pool[19467]: GetBufferParameterivARB (will be remapped) */
   "iip\0"
   "glGetBufferParameteriv\0"
   "glGetBufferParameterivARB\0"
   "\0"
   /* _mesa_function_pool[19521]: Rectdv (offset 87) */
   "pp\0"
   "glRectdv\0"
   "\0"
   /* _mesa_function_pool[19534]: ListParameteriSGIX (dynamic) */
   "iii\0"
   "glListParameteriSGIX\0"
   "\0"
   /* _mesa_function_pool[19560]: BlendEquationiARB (will be remapped) */
   "ii\0"
   "glBlendEquationiARB\0"
   "glBlendEquationIndexedAMD\0"
   "\0"
   /* _mesa_function_pool[19610]: ReplacementCodeuiColor4fNormal3fVertex3fSUN (dynamic) */
   "iffffffffff\0"
   "glReplacementCodeuiColor4fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[19669]: InstrumentsBufferSGIX (dynamic) */
   "ip\0"
   "glInstrumentsBufferSGIX\0"
   "\0"
   /* _mesa_function_pool[19697]: VertexAttrib4NivARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4Niv\0"
   "glVertexAttrib4NivARB\0"
   "\0"
   /* _mesa_function_pool[19742]: DrawArraysInstancedARB (will be remapped) */
   "iiii\0"
   "glDrawArraysInstancedARB\0"
   "glDrawArraysInstancedEXT\0"
   "glDrawArraysInstanced\0"
   "\0"
   /* _mesa_function_pool[19820]: GetAttachedShaders (will be remapped) */
   "iipp\0"
   "glGetAttachedShaders\0"
   "\0"
   /* _mesa_function_pool[19847]: GenVertexArraysAPPLE (will be remapped) */
   "ip\0"
   "glGenVertexArraysAPPLE\0"
   "\0"
   /* _mesa_function_pool[19874]: ClearBufferfi (will be remapped) */
   "iifi\0"
   "glClearBufferfi\0"
   "\0"
   /* _mesa_function_pool[19896]: Materialiv (offset 172) */
   "iip\0"
   "glMaterialiv\0"
   "\0"
   /* _mesa_function_pool[19914]: PushClientAttrib (offset 335) */
   "i\0"
   "glPushClientAttrib\0"
   "\0"
   /* _mesa_function_pool[19936]: SamplerParameteriv (will be remapped) */
   "iip\0"
   "glSamplerParameteriv\0"
   "\0"
   /* _mesa_function_pool[19962]: TexCoord2fColor4fNormal3fVertex3fvSUN (dynamic) */
   "pppp\0"
   "glTexCoord2fColor4fNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[20008]: WindowPos2iMESA (will be remapped) */
   "ii\0"
   "glWindowPos2i\0"
   "glWindowPos2iARB\0"
   "glWindowPos2iMESA\0"
   "\0"
   /* _mesa_function_pool[20061]: SecondaryColor3fvEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3fv\0"
   "glSecondaryColor3fvEXT\0"
   "\0"
   /* _mesa_function_pool[20107]: PolygonMode (offset 174) */
   "ii\0"
   "glPolygonMode\0"
   "\0"
   /* _mesa_function_pool[20125]: CompressedTexSubImage1DARB (will be remapped) */
   "iiiiiip\0"
   "glCompressedTexSubImage1D\0"
   "glCompressedTexSubImage1DARB\0"
   "\0"
   /* _mesa_function_pool[20189]: VertexAttrib2dvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib2dvNV\0"
   "\0"
   /* _mesa_function_pool[20213]: TexCoord2fNormal3fVertex3fSUN (dynamic) */
   "ffffffff\0"
   "glTexCoord2fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[20255]: GetVertexAttribivNV (will be remapped) */
   "iip\0"
   "glGetVertexAttribivNV\0"
   "\0"
   /* _mesa_function_pool[20282]: GetProgramStringARB (will be remapped) */
   "iip\0"
   "glGetProgramStringARB\0"
   "\0"
   /* _mesa_function_pool[20309]: GetnUniformdvARB (will be remapped) */
   "iiip\0"
   "glGetnUniformdvARB\0"
   "\0"
   /* _mesa_function_pool[20334]: DrawElementsInstancedBaseVertex (will be remapped) */
   "iiipii\0"
   "glDrawElementsInstancedBaseVertex\0"
   "\0"
   /* _mesa_function_pool[20376]: PauseTransformFeedback (will be remapped) */
   "\0"
   "glPauseTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[20403]: VertexAttribIPointerEXT (will be remapped) */
   "iiiip\0"
   "glVertexAttribIPointerEXT\0"
   "glVertexAttribIPointer\0"
   "\0"
   /* _mesa_function_pool[20459]: TexBumpParameterfvATI (will be remapped) */
   "ip\0"
   "glTexBumpParameterfvATI\0"
   "\0"
   /* _mesa_function_pool[20487]: Tangent3ivEXT (dynamic) */
   "p\0"
   "glTangent3ivEXT\0"
   "\0"
   /* _mesa_function_pool[20506]: CompileShaderARB (will be remapped) */
   "i\0"
   "glCompileShader\0"
   "glCompileShaderARB\0"
   "\0"
   /* _mesa_function_pool[20544]: DeleteShader (will be remapped) */
   "i\0"
   "glDeleteShader\0"
   "\0"
   /* _mesa_function_pool[20562]: DisableClientState (offset 309) */
   "i\0"
   "glDisableClientState\0"
   "\0"
   /* _mesa_function_pool[20586]: TexGeni (offset 192) */
   "iii\0"
   "glTexGeni\0"
   "\0"
   /* _mesa_function_pool[20601]: TexGenf (offset 190) */
   "iif\0"
   "glTexGenf\0"
   "\0"
   /* _mesa_function_pool[20616]: Uniform3fARB (will be remapped) */
   "ifff\0"
   "glUniform3f\0"
   "glUniform3fARB\0"
   "\0"
   /* _mesa_function_pool[20649]: TexGend (offset 188) */
   "iid\0"
   "glTexGend\0"
   "\0"
   /* _mesa_function_pool[20664]: ListParameterfvSGIX (dynamic) */
   "iip\0"
   "glListParameterfvSGIX\0"
   "\0"
   /* _mesa_function_pool[20691]: GetPolygonStipple (offset 274) */
   "p\0"
   "glGetPolygonStipple\0"
   "\0"
   /* _mesa_function_pool[20714]: Tangent3dvEXT (dynamic) */
   "p\0"
   "glTangent3dvEXT\0"
   "\0"
   /* _mesa_function_pool[20733]: GetVertexAttribfvNV (will be remapped) */
   "iip\0"
   "glGetVertexAttribfvNV\0"
   "\0"
   /* _mesa_function_pool[20760]: WindowPos3sMESA (will be remapped) */
   "iii\0"
   "glWindowPos3s\0"
   "glWindowPos3sARB\0"
   "glWindowPos3sMESA\0"
   "\0"
   /* _mesa_function_pool[20814]: VertexAttrib2svNV (will be remapped) */
   "ip\0"
   "glVertexAttrib2svNV\0"
   "\0"
   /* _mesa_function_pool[20838]: NormalP3uiv (will be remapped) */
   "ip\0"
   "glNormalP3uiv\0"
   "\0"
   /* _mesa_function_pool[20856]: SecondaryColorP3uiv (will be remapped) */
   "ip\0"
   "glSecondaryColorP3uiv\0"
   "\0"
   /* _mesa_function_pool[20882]: VertexAttribs1fvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs1fvNV\0"
   "\0"
   /* _mesa_function_pool[20908]: TexCoord2fVertex3fvSUN (dynamic) */
   "pp\0"
   "glTexCoord2fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[20937]: WindowPos4sMESA (will be remapped) */
   "iiii\0"
   "glWindowPos4sMESA\0"
   "\0"
   /* _mesa_function_pool[20961]: GetnPixelMapuivARB (will be remapped) */
   "iip\0"
   "glGetnPixelMapuivARB\0"
   "\0"
   /* _mesa_function_pool[20987]: VertexAttrib4NuivARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nuiv\0"
   "glVertexAttrib4NuivARB\0"
   "\0"
   /* _mesa_function_pool[21034]: ClientActiveTextureARB (offset 375) */
   "i\0"
   "glClientActiveTexture\0"
   "glClientActiveTextureARB\0"
   "\0"
   /* _mesa_function_pool[21084]: GetSamplerParameterIuiv (will be remapped) */
   "iip\0"
   "glGetSamplerParameterIuiv\0"
   "\0"
   /* _mesa_function_pool[21115]: ReplacementCodeusvSUN (dynamic) */
   "p\0"
   "glReplacementCodeusvSUN\0"
   "\0"
   /* _mesa_function_pool[21142]: Uniform4fARB (will be remapped) */
   "iffff\0"
   "glUniform4f\0"
   "glUniform4fARB\0"
   "\0"
   /* _mesa_function_pool[21176]: Color4sv (offset 34) */
   "p\0"
   "glColor4sv\0"
   "\0"
   /* _mesa_function_pool[21190]: FlushMappedBufferRange (will be remapped) */
   "iii\0"
   "glFlushMappedBufferRange\0"
   "\0"
   /* _mesa_function_pool[21220]: IsProgramNV (will be remapped) */
   "i\0"
   "glIsProgramARB\0"
   "glIsProgramNV\0"
   "\0"
   /* _mesa_function_pool[21252]: FlushMappedBufferRangeAPPLE (will be remapped) */
   "iii\0"
   "glFlushMappedBufferRangeAPPLE\0"
   "\0"
   /* _mesa_function_pool[21287]: PixelZoom (offset 246) */
   "ff\0"
   "glPixelZoom\0"
   "\0"
   /* _mesa_function_pool[21303]: ReplacementCodePointerSUN (dynamic) */
   "iip\0"
   "glReplacementCodePointerSUN\0"
   "\0"
   /* _mesa_function_pool[21336]: ProgramEnvParameter4dARB (will be remapped) */
   "iidddd\0"
   "glProgramEnvParameter4dARB\0"
   "glProgramParameter4dNV\0"
   "\0"
   /* _mesa_function_pool[21394]: ColorTableParameterfv (offset 340) */
   "iip\0"
   "glColorTableParameterfv\0"
   "glColorTableParameterfvSGI\0"
   "\0"
   /* _mesa_function_pool[21450]: FragmentLightModelfSGIX (dynamic) */
   "if\0"
   "glFragmentLightModelfSGIX\0"
   "\0"
   /* _mesa_function_pool[21480]: Binormal3bvEXT (dynamic) */
   "p\0"
   "glBinormal3bvEXT\0"
   "\0"
   /* _mesa_function_pool[21500]: PixelMapuiv (offset 252) */
   "iip\0"
   "glPixelMapuiv\0"
   "\0"
   /* _mesa_function_pool[21519]: Color3dv (offset 12) */
   "p\0"
   "glColor3dv\0"
   "\0"
   /* _mesa_function_pool[21533]: IsTexture (offset 330) */
   "i\0"
   "glIsTexture\0"
   "glIsTextureEXT\0"
   "\0"
   /* _mesa_function_pool[21563]: GenSamplers (will be remapped) */
   "ip\0"
   "glGenSamplers\0"
   "\0"
   /* _mesa_function_pool[21581]: ColorPointerEXT (will be remapped) */
   "iiiip\0"
   "glColorPointerEXT\0"
   "\0"
   /* _mesa_function_pool[21606]: VertexWeightfvEXT (dynamic) */
   "p\0"
   "glVertexWeightfvEXT\0"
   "\0"
   /* _mesa_function_pool[21629]: VertexAttrib1dARB (will be remapped) */
   "id\0"
   "glVertexAttrib1d\0"
   "glVertexAttrib1dARB\0"
   "\0"
   /* _mesa_function_pool[21670]: VertexP3uiv (will be remapped) */
   "ip\0"
   "glVertexP3uiv\0"
   "\0"
   /* _mesa_function_pool[21688]: TexCoord4i (offset 122) */
   "iiii\0"
   "glTexCoord4i\0"
   "\0"
   /* _mesa_function_pool[21707]: DeleteQueriesARB (will be remapped) */
   "ip\0"
   "glDeleteQueries\0"
   "glDeleteQueriesARB\0"
   "\0"
   /* _mesa_function_pool[21746]: Color4ubVertex2fSUN (dynamic) */
   "iiiiff\0"
   "glColor4ubVertex2fSUN\0"
   "\0"
   /* _mesa_function_pool[21776]: FragmentColorMaterialSGIX (dynamic) */
   "ii\0"
   "glFragmentColorMaterialSGIX\0"
   "\0"
   /* _mesa_function_pool[21808]: CurrentPaletteMatrixARB (dynamic) */
   "i\0"
   "glCurrentPaletteMatrixARB\0"
   "\0"
   /* _mesa_function_pool[21837]: GetMapdv (offset 266) */
   "iip\0"
   "glGetMapdv\0"
   "\0"
   /* _mesa_function_pool[21853]: ObjectPurgeableAPPLE (will be remapped) */
   "iii\0"
   "glObjectPurgeableAPPLE\0"
   "\0"
   /* _mesa_function_pool[21881]: GetStringi (will be remapped) */
   "ii\0"
   "glGetStringi\0"
   "\0"
   /* _mesa_function_pool[21898]: SamplePatternSGIS (will be remapped) */
   "i\0"
   "glSamplePatternSGIS\0"
   "glSamplePatternEXT\0"
   "\0"
   /* _mesa_function_pool[21940]: PixelStoref (offset 249) */
   "if\0"
   "glPixelStoref\0"
   "\0"
   /* _mesa_function_pool[21958]: IsQueryARB (will be remapped) */
   "i\0"
   "glIsQuery\0"
   "glIsQueryARB\0"
   "\0"
   /* _mesa_function_pool[21984]: ReplacementCodeuiColor4ubVertex3fSUN (dynamic) */
   "iiiiifff\0"
   "glReplacementCodeuiColor4ubVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[22033]: PixelStorei (offset 250) */
   "ii\0"
   "glPixelStorei\0"
   "\0"
   /* _mesa_function_pool[22051]: VertexAttrib4usvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4usv\0"
   "glVertexAttrib4usvARB\0"
   "\0"
   /* _mesa_function_pool[22096]: LinkProgramARB (will be remapped) */
   "i\0"
   "glLinkProgram\0"
   "glLinkProgramARB\0"
   "\0"
   /* _mesa_function_pool[22130]: VertexAttrib2fNV (will be remapped) */
   "iff\0"
   "glVertexAttrib2fNV\0"
   "\0"
   /* _mesa_function_pool[22154]: ShaderSourceARB (will be remapped) */
   "iipp\0"
   "glShaderSource\0"
   "glShaderSourceARB\0"
   "\0"
   /* _mesa_function_pool[22193]: FragmentMaterialiSGIX (dynamic) */
   "iii\0"
   "glFragmentMaterialiSGIX\0"
   "\0"
   /* _mesa_function_pool[22222]: EvalCoord2dv (offset 233) */
   "p\0"
   "glEvalCoord2dv\0"
   "\0"
   /* _mesa_function_pool[22240]: VertexAttrib3svARB (will be remapped) */
   "ip\0"
   "glVertexAttrib3sv\0"
   "glVertexAttrib3svARB\0"
   "\0"
   /* _mesa_function_pool[22283]: ColorMaterial (offset 151) */
   "ii\0"
   "glColorMaterial\0"
   "\0"
   /* _mesa_function_pool[22303]: InvalidateSubFramebuffer (will be remapped) */
   "iipiiii\0"
   "glInvalidateSubFramebuffer\0"
   "\0"
   /* _mesa_function_pool[22339]: CompressedTexSubImage3DARB (will be remapped) */
   "iiiiiiiiiip\0"
   "glCompressedTexSubImage3D\0"
   "glCompressedTexSubImage3DARB\0"
   "\0"
   /* _mesa_function_pool[22407]: WindowPos2ivMESA (will be remapped) */
   "p\0"
   "glWindowPos2iv\0"
   "glWindowPos2ivARB\0"
   "glWindowPos2ivMESA\0"
   "\0"
   /* _mesa_function_pool[22462]: IsFramebufferEXT (will be remapped) */
   "i\0"
   "glIsFramebuffer\0"
   "glIsFramebufferEXT\0"
   "\0"
   /* _mesa_function_pool[22500]: Uniform4ivARB (will be remapped) */
   "iip\0"
   "glUniform4iv\0"
   "glUniform4ivARB\0"
   "\0"
   /* _mesa_function_pool[22534]: GetVertexAttribdvARB (will be remapped) */
   "iip\0"
   "glGetVertexAttribdv\0"
   "glGetVertexAttribdvARB\0"
   "\0"
   /* _mesa_function_pool[22582]: TexBumpParameterivATI (will be remapped) */
   "ip\0"
   "glTexBumpParameterivATI\0"
   "\0"
   /* _mesa_function_pool[22610]: GetSeparableFilter (offset 359) */
   "iiippp\0"
   "glGetSeparableFilter\0"
   "glGetSeparableFilterEXT\0"
   "\0"
   /* _mesa_function_pool[22663]: Binormal3dEXT (dynamic) */
   "ddd\0"
   "glBinormal3dEXT\0"
   "\0"
   /* _mesa_function_pool[22684]: SpriteParameteriSGIX (dynamic) */
   "ii\0"
   "glSpriteParameteriSGIX\0"
   "\0"
   /* _mesa_function_pool[22711]: RequestResidentProgramsNV (will be remapped) */
   "ip\0"
   "glRequestResidentProgramsNV\0"
   "\0"
   /* _mesa_function_pool[22743]: TexCoordP3uiv (will be remapped) */
   "ip\0"
   "glTexCoordP3uiv\0"
   "\0"
   /* _mesa_function_pool[22763]: ReplacementCodeusSUN (dynamic) */
   "i\0"
   "glReplacementCodeusSUN\0"
   "\0"
   /* _mesa_function_pool[22789]: FeedbackBuffer (offset 194) */
   "iip\0"
   "glFeedbackBuffer\0"
   "\0"
   /* _mesa_function_pool[22811]: RasterPos2iv (offset 67) */
   "p\0"
   "glRasterPos2iv\0"
   "\0"
   /* _mesa_function_pool[22829]: TexImage1D (offset 182) */
   "iiiiiiip\0"
   "glTexImage1D\0"
   "\0"
   /* _mesa_function_pool[22852]: ListParameterivSGIX (dynamic) */
   "iip\0"
   "glListParameterivSGIX\0"
   "\0"
   /* _mesa_function_pool[22879]: MultiDrawElementsEXT (will be remapped) */
   "ipipi\0"
   "glMultiDrawElements\0"
   "glMultiDrawElementsEXT\0"
   "\0"
   /* _mesa_function_pool[22929]: Color3s (offset 17) */
   "iii\0"
   "glColor3s\0"
   "\0"
   /* _mesa_function_pool[22944]: Uniform1ivARB (will be remapped) */
   "iip\0"
   "glUniform1iv\0"
   "glUniform1ivARB\0"
   "\0"
   /* _mesa_function_pool[22978]: WindowPos2sMESA (will be remapped) */
   "ii\0"
   "glWindowPos2s\0"
   "glWindowPos2sARB\0"
   "glWindowPos2sMESA\0"
   "\0"
   /* _mesa_function_pool[23031]: WeightusvARB (dynamic) */
   "ip\0"
   "glWeightusvARB\0"
   "\0"
   /* _mesa_function_pool[23050]: ColorP4ui (will be remapped) */
   "ii\0"
   "glColorP4ui\0"
   "\0"
   /* _mesa_function_pool[23066]: TexCoordPointer (offset 320) */
   "iiip\0"
   "glTexCoordPointer\0"
   "\0"
   /* _mesa_function_pool[23090]: FogCoordPointerEXT (will be remapped) */
   "iip\0"
   "glFogCoordPointer\0"
   "glFogCoordPointerEXT\0"
   "\0"
   /* _mesa_function_pool[23134]: GetnSeparableFilterARB (will be remapped) */
   "iiiipipp\0"
   "glGetnSeparableFilterARB\0"
   "\0"
   /* _mesa_function_pool[23169]: InvalidateBufferData (will be remapped) */
   "i\0"
   "glInvalidateBufferData\0"
   "\0"
   /* _mesa_function_pool[23195]: Color3i (offset 15) */
   "iii\0"
   "glColor3i\0"
   "\0"
   /* _mesa_function_pool[23210]: FrontFace (offset 157) */
   "i\0"
   "glFrontFace\0"
   "\0"
   /* _mesa_function_pool[23225]: EvalCoord2d (offset 232) */
   "dd\0"
   "glEvalCoord2d\0"
   "\0"
   /* _mesa_function_pool[23243]: SecondaryColor3ubvEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3ubv\0"
   "glSecondaryColor3ubvEXT\0"
   "\0"
   /* _mesa_function_pool[23291]: EvalCoord2f (offset 234) */
   "ff\0"
   "glEvalCoord2f\0"
   "\0"
   /* _mesa_function_pool[23309]: VertexAttrib4dvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4dv\0"
   "glVertexAttrib4dvARB\0"
   "\0"
   /* _mesa_function_pool[23352]: BindAttribLocationARB (will be remapped) */
   "iip\0"
   "glBindAttribLocation\0"
   "glBindAttribLocationARB\0"
   "\0"
   /* _mesa_function_pool[23402]: Color3b (offset 9) */
   "iii\0"
   "glColor3b\0"
   "\0"
   /* _mesa_function_pool[23417]: MultiTexCoord2dARB (offset 384) */
   "idd\0"
   "glMultiTexCoord2d\0"
   "glMultiTexCoord2dARB\0"
   "\0"
   /* _mesa_function_pool[23461]: ExecuteProgramNV (will be remapped) */
   "iip\0"
   "glExecuteProgramNV\0"
   "\0"
   /* _mesa_function_pool[23485]: Color3f (offset 13) */
   "fff\0"
   "glColor3f\0"
   "\0"
   /* _mesa_function_pool[23500]: LightEnviSGIX (dynamic) */
   "ii\0"
   "glLightEnviSGIX\0"
   "\0"
   /* _mesa_function_pool[23520]: Color3d (offset 11) */
   "ddd\0"
   "glColor3d\0"
   "\0"
   /* _mesa_function_pool[23535]: TextureBarrierNV (will be remapped) */
   "\0"
   "glTextureBarrierNV\0"
   "\0"
   /* _mesa_function_pool[23556]: Normal3dv (offset 55) */
   "p\0"
   "glNormal3dv\0"
   "\0"
   /* _mesa_function_pool[23571]: Lightf (offset 159) */
   "iif\0"
   "glLightf\0"
   "\0"
   /* _mesa_function_pool[23585]: FinishTextureSUNX (dynamic) */
   "\0"
   "glFinishTextureSUNX\0"
   "\0"
   /* _mesa_function_pool[23607]: MatrixMode (offset 293) */
   "i\0"
   "glMatrixMode\0"
   "\0"
   /* _mesa_function_pool[23623]: GetPixelMapusv (offset 273) */
   "ip\0"
   "glGetPixelMapusv\0"
   "\0"
   /* _mesa_function_pool[23644]: Lighti (offset 161) */
   "iii\0"
   "glLighti\0"
   "\0"
   /* _mesa_function_pool[23658]: VertexAttribPointerNV (will be remapped) */
   "iiiip\0"
   "glVertexAttribPointerNV\0"
   "\0"
   /* _mesa_function_pool[23689]: ClearDepthf (will be remapped) */
   "f\0"
   "glClearDepthf\0"
   "\0"
   /* _mesa_function_pool[23706]: GetCombinerInputParameterfvNV (dynamic) */
   "iiiip\0"
   "glGetCombinerInputParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[23745]: GetFragDataIndex (will be remapped) */
   "ip\0"
   "glGetFragDataIndex\0"
   "\0"
   /* _mesa_function_pool[23768]: GetBooleanIndexedvEXT (will be remapped) */
   "iip\0"
   "glGetBooleanIndexedvEXT\0"
   "glGetBooleani_v\0"
   "\0"
   /* _mesa_function_pool[23813]: GetFramebufferAttachmentParameterivEXT (will be remapped) */
   "iiip\0"
   "glGetFramebufferAttachmentParameteriv\0"
   "glGetFramebufferAttachmentParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[23898]: PixelTransformParameterfEXT (dynamic) */
   "iif\0"
   "glPixelTransformParameterfEXT\0"
   "\0"
   /* _mesa_function_pool[23933]: MultiTexCoord4dvARB (offset 401) */
   "ip\0"
   "glMultiTexCoord4dv\0"
   "glMultiTexCoord4dvARB\0"
   "\0"
   /* _mesa_function_pool[23978]: PixelTransformParameteriEXT (dynamic) */
   "iii\0"
   "glPixelTransformParameteriEXT\0"
   "\0"
   /* _mesa_function_pool[24013]: GetDoublev (offset 260) */
   "ip\0"
   "glGetDoublev\0"
   "\0"
   /* _mesa_function_pool[24030]: MultiTexCoordP4ui (will be remapped) */
   "iii\0"
   "glMultiTexCoordP4ui\0"
   "\0"
   /* _mesa_function_pool[24055]: MultMatrixd (offset 295) */
   "p\0"
   "glMultMatrixd\0"
   "\0"
   /* _mesa_function_pool[24072]: MultMatrixf (offset 294) */
   "p\0"
   "glMultMatrixf\0"
   "\0"
   /* _mesa_function_pool[24089]: VertexAttribI4bvEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI4bvEXT\0"
   "glVertexAttribI4bv\0"
   "\0"
   /* _mesa_function_pool[24134]: TexCoord2fColor4ubVertex3fSUN (dynamic) */
   "ffiiiifff\0"
   "glTexCoord2fColor4ubVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[24177]: Uniform1iARB (will be remapped) */
   "ii\0"
   "glUniform1i\0"
   "glUniform1iARB\0"
   "\0"
   /* _mesa_function_pool[24208]: GetnMapfvARB (will be remapped) */
   "iiip\0"
   "glGetnMapfvARB\0"
   "\0"
   /* _mesa_function_pool[24229]: IndexMaterialEXT (dynamic) */
   "ii\0"
   "glIndexMaterialEXT\0"
   "\0"
   /* _mesa_function_pool[24252]: VertexAttribPointerARB (will be remapped) */
   "iiiiip\0"
   "glVertexAttribPointer\0"
   "glVertexAttribPointerARB\0"
   "\0"
   /* _mesa_function_pool[24307]: SharpenTexFuncSGIS (dynamic) */
   "iip\0"
   "glSharpenTexFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[24333]: MultiTexCoord4fvARB (offset 403) */
   "ip\0"
   "glMultiTexCoord4fv\0"
   "glMultiTexCoord4fvARB\0"
   "\0"
   /* _mesa_function_pool[24378]: Uniform2uiEXT (will be remapped) */
   "iii\0"
   "glUniform2uiEXT\0"
   "glUniform2ui\0"
   "\0"
   /* _mesa_function_pool[24412]: TagSampleBufferSGIX (dynamic) */
   "\0"
   "glTagSampleBufferSGIX\0"
   "\0"
   /* _mesa_function_pool[24436]: UniformMatrix2x3fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix2x3fv\0"
   "\0"
   /* _mesa_function_pool[24463]: SamplerParameteri (will be remapped) */
   "iii\0"
   "glSamplerParameteri\0"
   "\0"
   /* _mesa_function_pool[24488]: SamplerParameterf (will be remapped) */
   "iif\0"
   "glSamplerParameterf\0"
   "\0"
   /* _mesa_function_pool[24513]: CombinerParameteriNV (dynamic) */
   "ii\0"
   "glCombinerParameteriNV\0"
   "\0"
   /* _mesa_function_pool[24540]: DeleteAsyncMarkersSGIX (dynamic) */
   "ii\0"
   "glDeleteAsyncMarkersSGIX\0"
   "\0"
   /* _mesa_function_pool[24569]: TransformFeedbackVaryingsEXT (will be remapped) */
   "iipi\0"
   "glTransformFeedbackVaryings\0"
   "glTransformFeedbackVaryingsEXT\0"
   "\0"
   /* _mesa_function_pool[24634]: IsAsyncMarkerSGIX (dynamic) */
   "i\0"
   "glIsAsyncMarkerSGIX\0"
   "\0"
   /* _mesa_function_pool[24657]: FrameZoomSGIX (dynamic) */
   "i\0"
   "glFrameZoomSGIX\0"
   "\0"
   /* _mesa_function_pool[24676]: Normal3fVertex3fvSUN (dynamic) */
   "pp\0"
   "glNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[24703]: GetnUniformuivARB (will be remapped) */
   "iiip\0"
   "glGetnUniformuivARB\0"
   "\0"
   /* _mesa_function_pool[24729]: RasterPos4sv (offset 85) */
   "p\0"
   "glRasterPos4sv\0"
   "\0"
   /* _mesa_function_pool[24747]: VertexAttrib4NsvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nsv\0"
   "glVertexAttrib4NsvARB\0"
   "\0"
   /* _mesa_function_pool[24792]: VertexAttrib3fvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib3fv\0"
   "glVertexAttrib3fvARB\0"
   "\0"
   /* _mesa_function_pool[24835]: ClearColor (offset 206) */
   "ffff\0"
   "glClearColor\0"
   "\0"
   /* _mesa_function_pool[24854]: GetSynciv (will be remapped) */
   "iiipp\0"
   "glGetSynciv\0"
   "\0"
   /* _mesa_function_pool[24873]: ClearColorIiEXT (will be remapped) */
   "iiii\0"
   "glClearColorIiEXT\0"
   "\0"
   /* _mesa_function_pool[24897]: DeleteFramebuffersEXT (will be remapped) */
   "ip\0"
   "glDeleteFramebuffers\0"
   "glDeleteFramebuffersEXT\0"
   "\0"
   /* _mesa_function_pool[24946]: GlobalAlphaFactorsSUN (dynamic) */
   "i\0"
   "glGlobalAlphaFactorsSUN\0"
   "\0"
   /* _mesa_function_pool[24973]: IsEnabledIndexedEXT (will be remapped) */
   "ii\0"
   "glIsEnabledIndexedEXT\0"
   "glIsEnabledi\0"
   "\0"
   /* _mesa_function_pool[25012]: TexEnviv (offset 187) */
   "iip\0"
   "glTexEnviv\0"
   "\0"
   /* _mesa_function_pool[25028]: TexSubImage3D (offset 372) */
   "iiiiiiiiiip\0"
   "glTexSubImage3D\0"
   "glTexSubImage3DEXT\0"
   "\0"
   /* _mesa_function_pool[25076]: Tangent3fEXT (dynamic) */
   "fff\0"
   "glTangent3fEXT\0"
   "\0"
   /* _mesa_function_pool[25096]: SecondaryColor3uivEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3uiv\0"
   "glSecondaryColor3uivEXT\0"
   "\0"
   /* _mesa_function_pool[25144]: MatrixIndexubvARB (dynamic) */
   "ip\0"
   "glMatrixIndexubvARB\0"
   "\0"
   /* _mesa_function_pool[25168]: Color4fNormal3fVertex3fSUN (dynamic) */
   "ffffffffff\0"
   "glColor4fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[25209]: PixelTexGenParameterfSGIS (dynamic) */
   "if\0"
   "glPixelTexGenParameterfSGIS\0"
   "\0"
   /* _mesa_function_pool[25241]: CreateShader (will be remapped) */
   "i\0"
   "glCreateShader\0"
   "\0"
   /* _mesa_function_pool[25259]: GetColorTableParameterfv (offset 344) */
   "iip\0"
   "glGetColorTableParameterfv\0"
   "glGetColorTableParameterfvSGI\0"
   "glGetColorTableParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[25351]: FragmentLightModelfvSGIX (dynamic) */
   "ip\0"
   "glFragmentLightModelfvSGIX\0"
   "\0"
   /* _mesa_function_pool[25382]: Bitmap (offset 8) */
   "iiffffp\0"
   "glBitmap\0"
   "\0"
   /* _mesa_function_pool[25400]: MultiTexCoord3fARB (offset 394) */
   "ifff\0"
   "glMultiTexCoord3f\0"
   "glMultiTexCoord3fARB\0"
   "\0"
   /* _mesa_function_pool[25445]: GetTexLevelParameterfv (offset 284) */
   "iiip\0"
   "glGetTexLevelParameterfv\0"
   "\0"
   /* _mesa_function_pool[25476]: GetPixelTexGenParameterfvSGIS (dynamic) */
   "ip\0"
   "glGetPixelTexGenParameterfvSGIS\0"
   "\0"
   /* _mesa_function_pool[25512]: GenFramebuffersEXT (will be remapped) */
   "ip\0"
   "glGenFramebuffers\0"
   "glGenFramebuffersEXT\0"
   "\0"
   /* _mesa_function_pool[25555]: BindBufferOffsetEXT (will be remapped) */
   "iiii\0"
   "glBindBufferOffsetEXT\0"
   "\0"
   /* _mesa_function_pool[25583]: VertexAttribDivisor (will be remapped) */
   "ii\0"
   "glVertexAttribDivisor\0"
   "\0"
   /* _mesa_function_pool[25609]: DrawTransformFeedbackStream (will be remapped) */
   "iii\0"
   "glDrawTransformFeedbackStream\0"
   "\0"
   /* _mesa_function_pool[25644]: Vertex2sv (offset 133) */
   "p\0"
   "glVertex2sv\0"
   "\0"
   /* _mesa_function_pool[25659]: GetIntegerv (offset 263) */
   "ip\0"
   "glGetIntegerv\0"
   "\0"
   /* _mesa_function_pool[25677]: IsVertexArrayAPPLE (will be remapped) */
   "i\0"
   "glIsVertexArray\0"
   "glIsVertexArrayAPPLE\0"
   "\0"
   /* _mesa_function_pool[25717]: FragmentLightfvSGIX (dynamic) */
   "iip\0"
   "glFragmentLightfvSGIX\0"
   "\0"
   /* _mesa_function_pool[25744]: VertexAttribDivisorARB (will be remapped) */
   "ii\0"
   "glVertexAttribDivisorARB\0"
   "\0"
   /* _mesa_function_pool[25773]: DetachShader (will be remapped) */
   "ii\0"
   "glDetachShader\0"
   "\0"
   /* _mesa_function_pool[25792]: VertexAttrib4NubARB (will be remapped) */
   "iiiii\0"
   "glVertexAttrib4Nub\0"
   "glVertexAttrib4NubARB\0"
   "\0"
   /* _mesa_function_pool[25840]: GetProgramEnvParameterfvARB (will be remapped) */
   "iip\0"
   "glGetProgramEnvParameterfvARB\0"
   "\0"
   /* _mesa_function_pool[25875]: GetTrackMatrixivNV (will be remapped) */
   "iiip\0"
   "glGetTrackMatrixivNV\0"
   "\0"
   /* _mesa_function_pool[25902]: VertexAttrib3svNV (will be remapped) */
   "ip\0"
   "glVertexAttrib3svNV\0"
   "\0"
   /* _mesa_function_pool[25926]: Uniform4fvARB (will be remapped) */
   "iip\0"
   "glUniform4fv\0"
   "glUniform4fvARB\0"
   "\0"
   /* _mesa_function_pool[25960]: MultTransposeMatrixfARB (will be remapped) */
   "p\0"
   "glMultTransposeMatrixf\0"
   "glMultTransposeMatrixfARB\0"
   "\0"
   /* _mesa_function_pool[26012]: GetTexEnviv (offset 277) */
   "iip\0"
   "glGetTexEnviv\0"
   "\0"
   /* _mesa_function_pool[26031]: ColorFragmentOp1ATI (will be remapped) */
   "iiiiiii\0"
   "glColorFragmentOp1ATI\0"
   "\0"
   /* _mesa_function_pool[26062]: GetUniformfvARB (will be remapped) */
   "iip\0"
   "glGetUniformfv\0"
   "glGetUniformfvARB\0"
   "\0"
   /* _mesa_function_pool[26100]: EGLImageTargetRenderbufferStorageOES (will be remapped) */
   "ip\0"
   "glEGLImageTargetRenderbufferStorageOES\0"
   "\0"
   /* _mesa_function_pool[26143]: VertexAttribI2ivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI2ivEXT\0"
   "glVertexAttribI2iv\0"
   "\0"
   /* _mesa_function_pool[26188]: PopClientAttrib (offset 334) */
   "\0"
   "glPopClientAttrib\0"
   "\0"
   /* _mesa_function_pool[26208]: ReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN (dynamic) */
   "iffffffffffff\0"
   "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[26279]: DetachObjectARB (will be remapped) */
   "ii\0"
   "glDetachObjectARB\0"
   "\0"
   /* _mesa_function_pool[26301]: VertexBlendARB (dynamic) */
   "i\0"
   "glVertexBlendARB\0"
   "\0"
   /* _mesa_function_pool[26321]: WindowPos3iMESA (will be remapped) */
   "iii\0"
   "glWindowPos3i\0"
   "glWindowPos3iARB\0"
   "glWindowPos3iMESA\0"
   "\0"
   /* _mesa_function_pool[26375]: SeparableFilter2D (offset 360) */
   "iiiiiipp\0"
   "glSeparableFilter2D\0"
   "glSeparableFilter2DEXT\0"
   "\0"
   /* _mesa_function_pool[26428]: ProgramParameteriARB (will be remapped) */
   "iii\0"
   "glProgramParameteriARB\0"
   "\0"
   /* _mesa_function_pool[26456]: Map1d (offset 220) */
   "iddiip\0"
   "glMap1d\0"
   "\0"
   /* _mesa_function_pool[26472]: Map1f (offset 221) */
   "iffiip\0"
   "glMap1f\0"
   "\0"
   /* _mesa_function_pool[26488]: CompressedTexImage2DARB (will be remapped) */
   "iiiiiiip\0"
   "glCompressedTexImage2D\0"
   "glCompressedTexImage2DARB\0"
   "\0"
   /* _mesa_function_pool[26547]: ArrayElement (offset 306) */
   "i\0"
   "glArrayElement\0"
   "glArrayElementEXT\0"
   "\0"
   /* _mesa_function_pool[26583]: TexImage2D (offset 183) */
   "iiiiiiiip\0"
   "glTexImage2D\0"
   "\0"
   /* _mesa_function_pool[26607]: DepthBoundsEXT (will be remapped) */
   "dd\0"
   "glDepthBoundsEXT\0"
   "\0"
   /* _mesa_function_pool[26628]: ProgramParameters4fvNV (will be remapped) */
   "iiip\0"
   "glProgramParameters4fvNV\0"
   "\0"
   /* _mesa_function_pool[26659]: DeformationMap3fSGIX (dynamic) */
   "iffiiffiiffiip\0"
   "glDeformationMap3fSGIX\0"
   "\0"
   /* _mesa_function_pool[26698]: GetProgramivNV (will be remapped) */
   "iip\0"
   "glGetProgramivNV\0"
   "\0"
   /* _mesa_function_pool[26720]: GetIntegerIndexedvEXT (will be remapped) */
   "iip\0"
   "glGetIntegerIndexedvEXT\0"
   "glGetIntegeri_v\0"
   "\0"
   /* _mesa_function_pool[26765]: BeginTransformFeedbackEXT (will be remapped) */
   "i\0"
   "glBeginTransformFeedback\0"
   "glBeginTransformFeedbackEXT\0"
   "\0"
   /* _mesa_function_pool[26821]: GetMinmaxParameteriv (offset 366) */
   "iip\0"
   "glGetMinmaxParameteriv\0"
   "glGetMinmaxParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[26875]: PixelTransferf (offset 247) */
   "if\0"
   "glPixelTransferf\0"
   "\0"
   /* _mesa_function_pool[26896]: CopyTexImage1D (offset 323) */
   "iiiiiii\0"
   "glCopyTexImage1D\0"
   "glCopyTexImage1DEXT\0"
   "\0"
   /* _mesa_function_pool[26942]: PushMatrix (offset 298) */
   "\0"
   "glPushMatrix\0"
   "\0"
   /* _mesa_function_pool[26957]: Fogiv (offset 156) */
   "ip\0"
   "glFogiv\0"
   "\0"
   /* _mesa_function_pool[26969]: TexCoord1dv (offset 95) */
   "p\0"
   "glTexCoord1dv\0"
   "\0"
   /* _mesa_function_pool[26986]: AlphaFragmentOp3ATI (will be remapped) */
   "iiiiiiiiiiii\0"
   "glAlphaFragmentOp3ATI\0"
   "\0"
   /* _mesa_function_pool[27022]: PixelTransferi (offset 248) */
   "ii\0"
   "glPixelTransferi\0"
   "\0"
   /* _mesa_function_pool[27043]: GetnColorTableARB (will be remapped) */
   "iiiip\0"
   "glGetnColorTableARB\0"
   "\0"
   /* _mesa_function_pool[27070]: VertexAttrib3fvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib3fvNV\0"
   "\0"
   /* _mesa_function_pool[27094]: Rotatef (offset 300) */
   "ffff\0"
   "glRotatef\0"
   "\0"
   /* _mesa_function_pool[27110]: GetFinalCombinerInputParameterivNV (dynamic) */
   "iip\0"
   "glGetFinalCombinerInputParameterivNV\0"
   "\0"
   /* _mesa_function_pool[27152]: Vertex3i (offset 138) */
   "iii\0"
   "glVertex3i\0"
   "\0"
   /* _mesa_function_pool[27168]: SecondaryColorP3ui (will be remapped) */
   "ii\0"
   "glSecondaryColorP3ui\0"
   "\0"
   /* _mesa_function_pool[27193]: Vertex3f (offset 136) */
   "fff\0"
   "glVertex3f\0"
   "\0"
   /* _mesa_function_pool[27209]: Clear (offset 203) */
   "i\0"
   "glClear\0"
   "\0"
   /* _mesa_function_pool[27220]: Vertex3d (offset 134) */
   "ddd\0"
   "glVertex3d\0"
   "\0"
   /* _mesa_function_pool[27236]: GetMapParameterivNV (dynamic) */
   "iip\0"
   "glGetMapParameterivNV\0"
   "\0"
   /* _mesa_function_pool[27263]: Uniform4iARB (will be remapped) */
   "iiiii\0"
   "glUniform4i\0"
   "glUniform4iARB\0"
   "\0"
   /* _mesa_function_pool[27297]: ReadBuffer (offset 254) */
   "i\0"
   "glReadBuffer\0"
   "\0"
   /* _mesa_function_pool[27313]: ConvolutionParameteri (offset 352) */
   "iii\0"
   "glConvolutionParameteri\0"
   "glConvolutionParameteriEXT\0"
   "\0"
   /* _mesa_function_pool[27369]: Ortho (offset 296) */
   "dddddd\0"
   "glOrtho\0"
   "\0"
   /* _mesa_function_pool[27385]: Binormal3sEXT (dynamic) */
   "iii\0"
   "glBinormal3sEXT\0"
   "\0"
   /* _mesa_function_pool[27406]: ListBase (offset 6) */
   "i\0"
   "glListBase\0"
   "\0"
   /* _mesa_function_pool[27420]: VertexAttribI3ivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI3ivEXT\0"
   "glVertexAttribI3iv\0"
   "\0"
   /* _mesa_function_pool[27465]: VertexAttrib4fvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib4fvNV\0"
   "\0"
   /* _mesa_function_pool[27489]: Vertex3s (offset 140) */
   "iii\0"
   "glVertex3s\0"
   "\0"
   /* _mesa_function_pool[27505]: ConvolutionParameterf (offset 350) */
   "iif\0"
   "glConvolutionParameterf\0"
   "glConvolutionParameterfEXT\0"
   "\0"
   /* _mesa_function_pool[27561]: GetColorTableParameteriv (offset 345) */
   "iip\0"
   "glGetColorTableParameteriv\0"
   "glGetColorTableParameterivSGI\0"
   "glGetColorTableParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[27653]: ProgramEnvParameter4dvARB (will be remapped) */
   "iip\0"
   "glProgramEnvParameter4dvARB\0"
   "glProgramParameter4dvNV\0"
   "\0"
   /* _mesa_function_pool[27710]: ShadeModel (offset 177) */
   "i\0"
   "glShadeModel\0"
   "\0"
   /* _mesa_function_pool[27726]: BlendEquationSeparateiARB (will be remapped) */
   "iii\0"
   "glBlendEquationSeparateiARB\0"
   "glBlendEquationSeparateIndexedAMD\0"
   "\0"
   /* _mesa_function_pool[27793]: Rectiv (offset 91) */
   "pp\0"
   "glRectiv\0"
   "\0"
   /* _mesa_function_pool[27806]: UseProgramObjectARB (will be remapped) */
   "i\0"
   "glUseProgram\0"
   "glUseProgramObjectARB\0"
   "\0"
   /* _mesa_function_pool[27844]: GetMapParameterfvNV (dynamic) */
   "iip\0"
   "glGetMapParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[27871]: EndConditionalRenderNV (will be remapped) */
   "\0"
   "glEndConditionalRender\0"
   "glEndConditionalRenderNV\0"
   "\0"
   /* _mesa_function_pool[27921]: PassTexCoordATI (will be remapped) */
   "iii\0"
   "glPassTexCoordATI\0"
   "\0"
   /* _mesa_function_pool[27944]: DeleteProgram (will be remapped) */
   "i\0"
   "glDeleteProgram\0"
   "\0"
   /* _mesa_function_pool[27963]: GetSamplerParameteriv (will be remapped) */
   "iip\0"
   "glGetSamplerParameteriv\0"
   "\0"
   /* _mesa_function_pool[27992]: Tangent3dEXT (dynamic) */
   "ddd\0"
   "glTangent3dEXT\0"
   "\0"
   /* _mesa_function_pool[28012]: SecondaryColor3dvEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3dv\0"
   "glSecondaryColor3dvEXT\0"
   "\0"
   /* _mesa_function_pool[28058]: VertexAttrib1dNV (will be remapped) */
   "id\0"
   "glVertexAttrib1dNV\0"
   "\0"
   /* _mesa_function_pool[28081]: Vertex2fv (offset 129) */
   "p\0"
   "glVertex2fv\0"
   "\0"
   /* _mesa_function_pool[28096]: MultiDrawArraysEXT (will be remapped) */
   "ippi\0"
   "glMultiDrawArrays\0"
   "glMultiDrawArraysEXT\0"
   "\0"
   /* _mesa_function_pool[28141]: BindRenderbufferEXT (will be remapped) */
   "ii\0"
   "glBindRenderbuffer\0"
   "glBindRenderbufferEXT\0"
   "\0"
   /* _mesa_function_pool[28186]: MultiTexCoord4dARB (offset 400) */
   "idddd\0"
   "glMultiTexCoord4d\0"
   "glMultiTexCoord4dARB\0"
   "\0"
   /* _mesa_function_pool[28232]: FramebufferTextureFaceARB (will be remapped) */
   "iiiii\0"
   "glFramebufferTextureFaceARB\0"
   "\0"
   /* _mesa_function_pool[28267]: Vertex3sv (offset 141) */
   "p\0"
   "glVertex3sv\0"
   "\0"
   /* _mesa_function_pool[28282]: SecondaryColor3usEXT (will be remapped) */
   "iii\0"
   "glSecondaryColor3us\0"
   "glSecondaryColor3usEXT\0"
   "\0"
   /* _mesa_function_pool[28330]: ProgramLocalParameter4fvARB (will be remapped) */
   "iip\0"
   "glProgramLocalParameter4fvARB\0"
   "\0"
   /* _mesa_function_pool[28365]: DeleteProgramsNV (will be remapped) */
   "ip\0"
   "glDeleteProgramsARB\0"
   "glDeleteProgramsNV\0"
   "\0"
   /* _mesa_function_pool[28408]: EvalMesh1 (offset 236) */
   "iii\0"
   "glEvalMesh1\0"
   "\0"
   /* _mesa_function_pool[28425]: GetCombinerOutputParameterfvNV (dynamic) */
   "iiip\0"
   "glGetCombinerOutputParameterfvNV\0"
   "\0"
   /* _mesa_function_pool[28464]: MultiTexCoord1sARB (offset 382) */
   "ii\0"
   "glMultiTexCoord1s\0"
   "glMultiTexCoord1sARB\0"
   "\0"
   /* _mesa_function_pool[28507]: ReplacementCodeuiColor3fVertex3fSUN (dynamic) */
   "iffffff\0"
   "glReplacementCodeuiColor3fVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[28554]: GetVertexAttribPointervNV (will be remapped) */
   "iip\0"
   "glGetVertexAttribPointerv\0"
   "glGetVertexAttribPointervARB\0"
   "glGetVertexAttribPointervNV\0"
   "\0"
   /* _mesa_function_pool[28642]: DisableIndexedEXT (will be remapped) */
   "ii\0"
   "glDisableIndexedEXT\0"
   "glDisablei\0"
   "\0"
   /* _mesa_function_pool[28677]: MultiTexCoord1dvARB (offset 377) */
   "ip\0"
   "glMultiTexCoord1dv\0"
   "glMultiTexCoord1dvARB\0"
   "\0"
   /* _mesa_function_pool[28722]: Uniform2iARB (will be remapped) */
   "iii\0"
   "glUniform2i\0"
   "glUniform2iARB\0"
   "\0"
   /* _mesa_function_pool[28754]: Vertex2iv (offset 131) */
   "p\0"
   "glVertex2iv\0"
   "\0"
   /* _mesa_function_pool[28769]: GetProgramStringNV (will be remapped) */
   "iip\0"
   "glGetProgramStringNV\0"
   "\0"
   /* _mesa_function_pool[28795]: VertexAttribP3uiv (will be remapped) */
   "iiip\0"
   "glVertexAttribP3uiv\0"
   "\0"
   /* _mesa_function_pool[28821]: LineWidth (offset 168) */
   "f\0"
   "glLineWidth\0"
   "\0"
   /* _mesa_function_pool[28836]: Tangent3svEXT (dynamic) */
   "p\0"
   "glTangent3svEXT\0"
   "\0"
   /* _mesa_function_pool[28855]: MapBufferARB (will be remapped) */
   "ii\0"
   "glMapBuffer\0"
   "glMapBufferARB\0"
   "\0"
   /* _mesa_function_pool[28886]: MultiDrawElementsBaseVertex (will be remapped) */
   "ipipip\0"
   "glMultiDrawElementsBaseVertex\0"
   "\0"
   /* _mesa_function_pool[28924]: TexParameterIuivEXT (will be remapped) */
   "iip\0"
   "glTexParameterIuivEXT\0"
   "glTexParameterIuiv\0"
   "\0"
   /* _mesa_function_pool[28970]: Binormal3svEXT (dynamic) */
   "p\0"
   "glBinormal3svEXT\0"
   "\0"
   /* _mesa_function_pool[28990]: ApplyTextureEXT (dynamic) */
   "i\0"
   "glApplyTextureEXT\0"
   "\0"
   /* _mesa_function_pool[29011]: GetBufferParameteri64v (will be remapped) */
   "iip\0"
   "glGetBufferParameteri64v\0"
   "\0"
   /* _mesa_function_pool[29041]: TexGendv (offset 189) */
   "iip\0"
   "glTexGendv\0"
   "\0"
   /* _mesa_function_pool[29057]: VertexAttribI3iEXT (will be remapped) */
   "iiii\0"
   "glVertexAttribI3iEXT\0"
   "glVertexAttribI3i\0"
   "\0"
   /* _mesa_function_pool[29102]: EnableIndexedEXT (will be remapped) */
   "ii\0"
   "glEnableIndexedEXT\0"
   "glEnablei\0"
   "\0"
   /* _mesa_function_pool[29135]: TextureMaterialEXT (dynamic) */
   "ii\0"
   "glTextureMaterialEXT\0"
   "\0"
   /* _mesa_function_pool[29160]: TextureLightEXT (dynamic) */
   "i\0"
   "glTextureLightEXT\0"
   "\0"
   /* _mesa_function_pool[29181]: ResetMinmax (offset 370) */
   "i\0"
   "glResetMinmax\0"
   "glResetMinmaxEXT\0"
   "\0"
   /* _mesa_function_pool[29215]: SpriteParameterfSGIX (dynamic) */
   "if\0"
   "glSpriteParameterfSGIX\0"
   "\0"
   /* _mesa_function_pool[29242]: EnableClientState (offset 313) */
   "i\0"
   "glEnableClientState\0"
   "\0"
   /* _mesa_function_pool[29265]: VertexAttrib4sNV (will be remapped) */
   "iiiii\0"
   "glVertexAttrib4sNV\0"
   "\0"
   /* _mesa_function_pool[29291]: GetConvolutionParameterfv (offset 357) */
   "iip\0"
   "glGetConvolutionParameterfv\0"
   "glGetConvolutionParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[29355]: GetActiveUniformName (will be remapped) */
   "iiipp\0"
   "glGetActiveUniformName\0"
   "\0"
   /* _mesa_function_pool[29385]: MultiModeDrawArraysIBM (will be remapped) */
   "pppii\0"
   "glMultiModeDrawArraysIBM\0"
   "\0"
   /* _mesa_function_pool[29417]: VertexAttrib4dARB (will be remapped) */
   "idddd\0"
   "glVertexAttrib4d\0"
   "glVertexAttrib4dARB\0"
   "\0"
   /* _mesa_function_pool[29461]: GetTexBumpParameterfvATI (will be remapped) */
   "ip\0"
   "glGetTexBumpParameterfvATI\0"
   "\0"
   /* _mesa_function_pool[29492]: ProgramNamedParameter4dNV (will be remapped) */
   "iipdddd\0"
   "glProgramNamedParameter4dNV\0"
   "\0"
   /* _mesa_function_pool[29529]: GetMaterialfv (offset 269) */
   "iip\0"
   "glGetMaterialfv\0"
   "\0"
   /* _mesa_function_pool[29550]: VertexWeightfEXT (dynamic) */
   "f\0"
   "glVertexWeightfEXT\0"
   "\0"
   /* _mesa_function_pool[29572]: EndTransformFeedbackEXT (will be remapped) */
   "\0"
   "glEndTransformFeedback\0"
   "glEndTransformFeedbackEXT\0"
   "\0"
   /* _mesa_function_pool[29623]: SetFragmentShaderConstantATI (will be remapped) */
   "ip\0"
   "glSetFragmentShaderConstantATI\0"
   "\0"
   /* _mesa_function_pool[29658]: Binormal3fEXT (dynamic) */
   "fff\0"
   "glBinormal3fEXT\0"
   "\0"
   /* _mesa_function_pool[29679]: CallList (offset 2) */
   "i\0"
   "glCallList\0"
   "\0"
   /* _mesa_function_pool[29693]: Materialfv (offset 170) */
   "iip\0"
   "glMaterialfv\0"
   "\0"
   /* _mesa_function_pool[29711]: TexCoord3fv (offset 113) */
   "p\0"
   "glTexCoord3fv\0"
   "\0"
   /* _mesa_function_pool[29728]: GetUniformBlockIndex (will be remapped) */
   "ip\0"
   "glGetUniformBlockIndex\0"
   "\0"
   /* _mesa_function_pool[29755]: FogCoordfvEXT (will be remapped) */
   "p\0"
   "glFogCoordfv\0"
   "glFogCoordfvEXT\0"
   "\0"
   /* _mesa_function_pool[29787]: MultiTexCoord1ivARB (offset 381) */
   "ip\0"
   "glMultiTexCoord1iv\0"
   "glMultiTexCoord1ivARB\0"
   "\0"
   /* _mesa_function_pool[29832]: SecondaryColor3ubEXT (will be remapped) */
   "iii\0"
   "glSecondaryColor3ub\0"
   "glSecondaryColor3ubEXT\0"
   "\0"
   /* _mesa_function_pool[29880]: MultiTexCoord2ivARB (offset 389) */
   "ip\0"
   "glMultiTexCoord2iv\0"
   "glMultiTexCoord2ivARB\0"
   "\0"
   /* _mesa_function_pool[29925]: FogFuncSGIS (dynamic) */
   "ip\0"
   "glFogFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[29943]: CopyTexSubImage2D (offset 326) */
   "iiiiiiii\0"
   "glCopyTexSubImage2D\0"
   "glCopyTexSubImage2DEXT\0"
   "\0"
   /* _mesa_function_pool[29996]: GetObjectParameterivARB (will be remapped) */
   "iip\0"
   "glGetObjectParameterivARB\0"
   "\0"
   /* _mesa_function_pool[30027]: Color3iv (offset 16) */
   "p\0"
   "glColor3iv\0"
   "\0"
   /* _mesa_function_pool[30041]: TexCoord4fVertex4fSUN (dynamic) */
   "ffffffff\0"
   "glTexCoord4fVertex4fSUN\0"
   "\0"
   /* _mesa_function_pool[30075]: DrawElements (offset 311) */
   "iiip\0"
   "glDrawElements\0"
   "\0"
   /* _mesa_function_pool[30096]: BindVertexArrayAPPLE (will be remapped) */
   "i\0"
   "glBindVertexArrayAPPLE\0"
   "\0"
   /* _mesa_function_pool[30122]: GetProgramLocalParameterdvARB (will be remapped) */
   "iip\0"
   "glGetProgramLocalParameterdvARB\0"
   "\0"
   /* _mesa_function_pool[30159]: GetHistogramParameteriv (offset 363) */
   "iip\0"
   "glGetHistogramParameteriv\0"
   "glGetHistogramParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[30219]: MultiTexCoord1iARB (offset 380) */
   "ii\0"
   "glMultiTexCoord1i\0"
   "glMultiTexCoord1iARB\0"
   "\0"
   /* _mesa_function_pool[30262]: GetConvolutionFilter (offset 356) */
   "iiip\0"
   "glGetConvolutionFilter\0"
   "glGetConvolutionFilterEXT\0"
   "\0"
   /* _mesa_function_pool[30317]: GetProgramivARB (will be remapped) */
   "iip\0"
   "glGetProgramivARB\0"
   "\0"
   /* _mesa_function_pool[30340]: TexBufferARB (will be remapped) */
   "iii\0"
   "glTexBufferARB\0"
   "glTexBuffer\0"
   "\0"
   /* _mesa_function_pool[30372]: BlendFuncSeparateEXT (will be remapped) */
   "iiii\0"
   "glBlendFuncSeparate\0"
   "glBlendFuncSeparateEXT\0"
   "glBlendFuncSeparateINGR\0"
   "\0"
   /* _mesa_function_pool[30445]: MapBufferRange (will be remapped) */
   "iiii\0"
   "glMapBufferRange\0"
   "\0"
   /* _mesa_function_pool[30468]: ProgramParameters4dvNV (will be remapped) */
   "iiip\0"
   "glProgramParameters4dvNV\0"
   "\0"
   /* _mesa_function_pool[30499]: TexCoord2fColor3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glTexCoord2fColor3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[30536]: EvalPoint2 (offset 239) */
   "ii\0"
   "glEvalPoint2\0"
   "\0"
   /* _mesa_function_pool[30553]: Uniform1uivEXT (will be remapped) */
   "iip\0"
   "glUniform1uivEXT\0"
   "glUniform1uiv\0"
   "\0"
   /* _mesa_function_pool[30589]: EvalPoint1 (offset 237) */
   "i\0"
   "glEvalPoint1\0"
   "\0"
   /* _mesa_function_pool[30605]: Binormal3dvEXT (dynamic) */
   "p\0"
   "glBinormal3dvEXT\0"
   "\0"
   /* _mesa_function_pool[30625]: PopMatrix (offset 297) */
   "\0"
   "glPopMatrix\0"
   "\0"
   /* _mesa_function_pool[30639]: GetVertexAttribIuivEXT (will be remapped) */
   "iip\0"
   "glGetVertexAttribIuivEXT\0"
   "glGetVertexAttribIuiv\0"
   "\0"
   /* _mesa_function_pool[30691]: FinishFenceNV (dynamic) */
   "i\0"
   "glFinishFenceNV\0"
   "\0"
   /* _mesa_function_pool[30710]: GetFogFuncSGIS (dynamic) */
   "p\0"
   "glGetFogFuncSGIS\0"
   "\0"
   /* _mesa_function_pool[30730]: GetUniformLocationARB (will be remapped) */
   "ip\0"
   "glGetUniformLocation\0"
   "glGetUniformLocationARB\0"
   "\0"
   /* _mesa_function_pool[30779]: SecondaryColor3fEXT (will be remapped) */
   "fff\0"
   "glSecondaryColor3f\0"
   "glSecondaryColor3fEXT\0"
   "\0"
   /* _mesa_function_pool[30825]: GetTexGeniv (offset 280) */
   "iip\0"
   "glGetTexGeniv\0"
   "\0"
   /* _mesa_function_pool[30844]: CombinerInputNV (dynamic) */
   "iiiiii\0"
   "glCombinerInputNV\0"
   "\0"
   /* _mesa_function_pool[30870]: VertexAttrib3sARB (will be remapped) */
   "iiii\0"
   "glVertexAttrib3s\0"
   "glVertexAttrib3sARB\0"
   "\0"
   /* _mesa_function_pool[30913]: IsTransformFeedback (will be remapped) */
   "i\0"
   "glIsTransformFeedback\0"
   "\0"
   /* _mesa_function_pool[30938]: ReplacementCodeuiNormal3fVertex3fvSUN (dynamic) */
   "ppp\0"
   "glReplacementCodeuiNormal3fVertex3fvSUN\0"
   "\0"
   /* _mesa_function_pool[30983]: Map2d (offset 222) */
   "iddiiddiip\0"
   "glMap2d\0"
   "\0"
   /* _mesa_function_pool[31003]: Map2f (offset 223) */
   "iffiiffiip\0"
   "glMap2f\0"
   "\0"
   /* _mesa_function_pool[31023]: ProgramStringARB (will be remapped) */
   "iiip\0"
   "glProgramStringARB\0"
   "\0"
   /* _mesa_function_pool[31048]: Vertex4s (offset 148) */
   "iiii\0"
   "glVertex4s\0"
   "\0"
   /* _mesa_function_pool[31065]: TexCoord4fVertex4fvSUN (dynamic) */
   "pp\0"
   "glTexCoord4fVertex4fvSUN\0"
   "\0"
   /* _mesa_function_pool[31094]: VertexAttrib3sNV (will be remapped) */
   "iiii\0"
   "glVertexAttrib3sNV\0"
   "\0"
   /* _mesa_function_pool[31119]: VertexAttrib1fNV (will be remapped) */
   "if\0"
   "glVertexAttrib1fNV\0"
   "\0"
   /* _mesa_function_pool[31142]: Vertex4f (offset 144) */
   "ffff\0"
   "glVertex4f\0"
   "\0"
   /* _mesa_function_pool[31159]: EvalCoord1d (offset 228) */
   "d\0"
   "glEvalCoord1d\0"
   "\0"
   /* _mesa_function_pool[31176]: Vertex4d (offset 142) */
   "dddd\0"
   "glVertex4d\0"
   "\0"
   /* _mesa_function_pool[31193]: RasterPos4dv (offset 79) */
   "p\0"
   "glRasterPos4dv\0"
   "\0"
   /* _mesa_function_pool[31211]: GetGraphicsResetStatusARB (will be remapped) */
   "\0"
   "glGetGraphicsResetStatusARB\0"
   "\0"
   /* _mesa_function_pool[31241]: UseShaderProgramEXT (will be remapped) */
   "ii\0"
   "glUseShaderProgramEXT\0"
   "\0"
   /* _mesa_function_pool[31267]: VertexAttribP2uiv (will be remapped) */
   "iiip\0"
   "glVertexAttribP2uiv\0"
   "\0"
   /* _mesa_function_pool[31293]: PrimitiveRestartNV (will be remapped) */
   "\0"
   "glPrimitiveRestartNV\0"
   "\0"
   /* _mesa_function_pool[31316]: GetCompressedTexImageARB (will be remapped) */
   "iip\0"
   "glGetCompressedTexImage\0"
   "glGetCompressedTexImageARB\0"
   "\0"
   /* _mesa_function_pool[31372]: GetTexGenfv (offset 279) */
   "iip\0"
   "glGetTexGenfv\0"
   "\0"
   /* _mesa_function_pool[31391]: Vertex4i (offset 146) */
   "iiii\0"
   "glVertex4i\0"
   "\0"
   /* _mesa_function_pool[31408]: VertexWeightPointerEXT (dynamic) */
   "iiip\0"
   "glVertexWeightPointerEXT\0"
   "\0"
   /* _mesa_function_pool[31439]: GetHistogram (offset 361) */
   "iiiip\0"
   "glGetHistogram\0"
   "glGetHistogramEXT\0"
   "\0"
   /* _mesa_function_pool[31479]: ActiveStencilFaceEXT (will be remapped) */
   "i\0"
   "glActiveStencilFaceEXT\0"
   "\0"
   /* _mesa_function_pool[31505]: StencilFuncSeparateATI (will be remapped) */
   "iiii\0"
   "glStencilFuncSeparateATI\0"
   "\0"
   /* _mesa_function_pool[31536]: Materialf (offset 169) */
   "iif\0"
   "glMaterialf\0"
   "\0"
   /* _mesa_function_pool[31553]: GetShaderSourceARB (will be remapped) */
   "iipp\0"
   "glGetShaderSource\0"
   "glGetShaderSourceARB\0"
   "\0"
   /* _mesa_function_pool[31598]: IglooInterfaceSGIX (dynamic) */
   "ip\0"
   "glIglooInterfaceSGIX\0"
   "\0"
   /* _mesa_function_pool[31623]: Materiali (offset 171) */
   "iii\0"
   "glMateriali\0"
   "\0"
   /* _mesa_function_pool[31640]: VertexAttrib4dNV (will be remapped) */
   "idddd\0"
   "glVertexAttrib4dNV\0"
   "\0"
   /* _mesa_function_pool[31666]: MultiModeDrawElementsIBM (will be remapped) */
   "ppipii\0"
   "glMultiModeDrawElementsIBM\0"
   "\0"
   /* _mesa_function_pool[31701]: Indexsv (offset 51) */
   "p\0"
   "glIndexsv\0"
   "\0"
   /* _mesa_function_pool[31714]: MultiTexCoord4svARB (offset 407) */
   "ip\0"
   "glMultiTexCoord4sv\0"
   "glMultiTexCoord4svARB\0"
   "\0"
   /* _mesa_function_pool[31759]: LightModelfv (offset 164) */
   "ip\0"
   "glLightModelfv\0"
   "\0"
   /* _mesa_function_pool[31778]: TexCoord2dv (offset 103) */
   "p\0"
   "glTexCoord2dv\0"
   "\0"
   /* _mesa_function_pool[31795]: GenQueriesARB (will be remapped) */
   "ip\0"
   "glGenQueries\0"
   "glGenQueriesARB\0"
   "\0"
   /* _mesa_function_pool[31828]: EvalCoord1dv (offset 229) */
   "p\0"
   "glEvalCoord1dv\0"
   "\0"
   /* _mesa_function_pool[31846]: ReplacementCodeuiVertex3fSUN (dynamic) */
   "ifff\0"
   "glReplacementCodeuiVertex3fSUN\0"
   "\0"
   /* _mesa_function_pool[31883]: Translated (offset 303) */
   "ddd\0"
   "glTranslated\0"
   "\0"
   /* _mesa_function_pool[31901]: Translatef (offset 304) */
   "fff\0"
   "glTranslatef\0"
   "\0"
   /* _mesa_function_pool[31919]: Uniform3uiEXT (will be remapped) */
   "iiii\0"
   "glUniform3uiEXT\0"
   "glUniform3ui\0"
   "\0"
   /* _mesa_function_pool[31954]: StencilMask (offset 209) */
   "i\0"
   "glStencilMask\0"
   "\0"
   /* _mesa_function_pool[31971]: Tangent3iEXT (dynamic) */
   "iii\0"
   "glTangent3iEXT\0"
   "\0"
   /* _mesa_function_pool[31991]: ClampColorARB (will be remapped) */
   "ii\0"
   "glClampColorARB\0"
   "\0"
   /* _mesa_function_pool[32011]: GetLightiv (offset 265) */
   "iip\0"
   "glGetLightiv\0"
   "\0"
   /* _mesa_function_pool[32029]: GetSamplerParameterIiv (will be remapped) */
   "iip\0"
   "glGetSamplerParameterIiv\0"
   "\0"
   /* _mesa_function_pool[32059]: DrawMeshArraysSUN (dynamic) */
   "iiii\0"
   "glDrawMeshArraysSUN\0"
   "\0"
   /* _mesa_function_pool[32085]: IsList (offset 287) */
   "i\0"
   "glIsList\0"
   "\0"
   /* _mesa_function_pool[32097]: GetnPixelMapusvARB (will be remapped) */
   "iip\0"
   "glGetnPixelMapusvARB\0"
   "\0"
   /* _mesa_function_pool[32123]: IsSync (will be remapped) */
   "i\0"
   "glIsSync\0"
   "\0"
   /* _mesa_function_pool[32135]: RenderMode (offset 196) */
   "i\0"
   "glRenderMode\0"
   "\0"
   /* _mesa_function_pool[32151]: GetMapControlPointsNV (dynamic) */
   "iiiiiip\0"
   "glGetMapControlPointsNV\0"
   "\0"
   /* _mesa_function_pool[32184]: DrawBuffersARB (will be remapped) */
   "ip\0"
   "glDrawBuffers\0"
   "glDrawBuffersARB\0"
   "glDrawBuffersATI\0"
   "\0"
   /* _mesa_function_pool[32236]: ClearBufferiv (will be remapped) */
   "iip\0"
   "glClearBufferiv\0"
   "\0"
   /* _mesa_function_pool[32257]: ProgramLocalParameter4fARB (will be remapped) */
   "iiffff\0"
   "glProgramLocalParameter4fARB\0"
   "\0"
   /* _mesa_function_pool[32294]: SpriteParameterivSGIX (dynamic) */
   "ip\0"
   "glSpriteParameterivSGIX\0"
   "\0"
   /* _mesa_function_pool[32322]: ProvokingVertexEXT (will be remapped) */
   "i\0"
   "glProvokingVertexEXT\0"
   "glProvokingVertex\0"
   "\0"
   /* _mesa_function_pool[32364]: MultiTexCoord1fARB (offset 378) */
   "if\0"
   "glMultiTexCoord1f\0"
   "glMultiTexCoord1fARB\0"
   "\0"
   /* _mesa_function_pool[32407]: LoadName (offset 198) */
   "i\0"
   "glLoadName\0"
   "\0"
   /* _mesa_function_pool[32421]: VertexAttribs4ubvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs4ubvNV\0"
   "\0"
   /* _mesa_function_pool[32448]: WeightsvARB (dynamic) */
   "ip\0"
   "glWeightsvARB\0"
   "\0"
   /* _mesa_function_pool[32466]: NormalP3ui (will be remapped) */
   "ii\0"
   "glNormalP3ui\0"
   "\0"
   /* _mesa_function_pool[32483]: Uniform1fvARB (will be remapped) */
   "iip\0"
   "glUniform1fv\0"
   "glUniform1fvARB\0"
   "\0"
   /* _mesa_function_pool[32517]: CopyTexSubImage1D (offset 325) */
   "iiiiii\0"
   "glCopyTexSubImage1D\0"
   "glCopyTexSubImage1DEXT\0"
   "\0"
   /* _mesa_function_pool[32568]: CullFace (offset 152) */
   "i\0"
   "glCullFace\0"
   "\0"
   /* _mesa_function_pool[32582]: BindTexture (offset 307) */
   "ii\0"
   "glBindTexture\0"
   "glBindTextureEXT\0"
   "\0"
   /* _mesa_function_pool[32617]: VertexAttribs2fvNV (will be remapped) */
   "iip\0"
   "glVertexAttribs2fvNV\0"
   "\0"
   /* _mesa_function_pool[32643]: MultiTexCoord4fARB (offset 402) */
   "iffff\0"
   "glMultiTexCoord4f\0"
   "glMultiTexCoord4fARB\0"
   "\0"
   /* _mesa_function_pool[32689]: VertexAttribs3svNV (will be remapped) */
   "iip\0"
   "glVertexAttribs3svNV\0"
   "\0"
   /* _mesa_function_pool[32715]: StencilFunc (offset 243) */
   "iii\0"
   "glStencilFunc\0"
   "\0"
   /* _mesa_function_pool[32734]: CopyPixels (offset 255) */
   "iiiii\0"
   "glCopyPixels\0"
   "\0"
   /* _mesa_function_pool[32754]: Rectsv (offset 93) */
   "pp\0"
   "glRectsv\0"
   "\0"
   /* _mesa_function_pool[32767]: ReplacementCodeuivSUN (dynamic) */
   "p\0"
   "glReplacementCodeuivSUN\0"
   "\0"
   /* _mesa_function_pool[32794]: MultiTexCoordP2uiv (will be remapped) */
   "iip\0"
   "glMultiTexCoordP2uiv\0"
   "\0"
   /* _mesa_function_pool[32820]: EnableVertexAttribArrayARB (will be remapped) */
   "i\0"
   "glEnableVertexAttribArray\0"
   "glEnableVertexAttribArrayARB\0"
   "\0"
   /* _mesa_function_pool[32878]: NormalPointervINTEL (dynamic) */
   "ip\0"
   "glNormalPointervINTEL\0"
   "\0"
   /* _mesa_function_pool[32904]: CopyConvolutionFilter2D (offset 355) */
   "iiiiii\0"
   "glCopyConvolutionFilter2D\0"
   "glCopyConvolutionFilter2DEXT\0"
   "\0"
   /* _mesa_function_pool[32967]: WindowPos3ivMESA (will be remapped) */
   "p\0"
   "glWindowPos3iv\0"
   "glWindowPos3ivARB\0"
   "glWindowPos3ivMESA\0"
   "\0"
   /* _mesa_function_pool[33022]: CopyBufferSubData (will be remapped) */
   "iiiii\0"
   "glCopyBufferSubData\0"
   "\0"
   /* _mesa_function_pool[33049]: NormalPointer (offset 318) */
   "iip\0"
   "glNormalPointer\0"
   "\0"
   /* _mesa_function_pool[33070]: TexParameterfv (offset 179) */
   "iip\0"
   "glTexParameterfv\0"
   "\0"
   /* _mesa_function_pool[33092]: IsBufferARB (will be remapped) */
   "i\0"
   "glIsBuffer\0"
   "glIsBufferARB\0"
   "\0"
   /* _mesa_function_pool[33120]: WindowPos4iMESA (will be remapped) */
   "iiii\0"
   "glWindowPos4iMESA\0"
   "\0"
   /* _mesa_function_pool[33144]: VertexAttrib4uivARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4uiv\0"
   "glVertexAttrib4uivARB\0"
   "\0"
   /* _mesa_function_pool[33189]: Tangent3bvEXT (dynamic) */
   "p\0"
   "glTangent3bvEXT\0"
   "\0"
   /* _mesa_function_pool[33208]: VertexAttribI3uivEXT (will be remapped) */
   "ip\0"
   "glVertexAttribI3uivEXT\0"
   "glVertexAttribI3uiv\0"
   "\0"
   /* _mesa_function_pool[33255]: UniformMatrix3x4fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix3x4fv\0"
   "\0"
   /* _mesa_function_pool[33282]: ClipPlane (offset 150) */
   "ip\0"
   "glClipPlane\0"
   "\0"
   /* _mesa_function_pool[33298]: Recti (offset 90) */
   "iiii\0"
   "glRecti\0"
   "\0"
   /* _mesa_function_pool[33312]: TrackMatrixNV (will be remapped) */
   "iiii\0"
   "glTrackMatrixNV\0"
   "\0"
   /* _mesa_function_pool[33334]: DrawRangeElementsBaseVertex (will be remapped) */
   "iiiiipi\0"
   "glDrawRangeElementsBaseVertex\0"
   "\0"
   /* _mesa_function_pool[33373]: SamplerParameterIuiv (will be remapped) */
   "iip\0"
   "glSamplerParameterIuiv\0"
   "\0"
   /* _mesa_function_pool[33401]: TexCoordPointervINTEL (dynamic) */
   "iip\0"
   "glTexCoordPointervINTEL\0"
   "\0"
   /* _mesa_function_pool[33430]: DeleteBuffersARB (will be remapped) */
   "ip\0"
   "glDeleteBuffers\0"
   "glDeleteBuffersARB\0"
   "\0"
   /* _mesa_function_pool[33469]: ImageTransformParameteriHP (dynamic) */
   "iii\0"
   "glImageTransformParameteriHP\0"
   "\0"
   /* _mesa_function_pool[33503]: WindowPos4fvMESA (will be remapped) */
   "p\0"
   "glWindowPos4fvMESA\0"
   "\0"
   /* _mesa_function_pool[33525]: GetPixelMapuiv (offset 272) */
   "ip\0"
   "glGetPixelMapuiv\0"
   "\0"
   /* _mesa_function_pool[33546]: Rectf (offset 88) */
   "ffff\0"
   "glRectf\0"
   "\0"
   /* _mesa_function_pool[33560]: VertexAttrib1sNV (will be remapped) */
   "ii\0"
   "glVertexAttrib1sNV\0"
   "\0"
   /* _mesa_function_pool[33583]: Indexfv (offset 47) */
   "p\0"
   "glIndexfv\0"
   "\0"
   /* _mesa_function_pool[33596]: ColorP3uiv (will be remapped) */
   "ip\0"
   "glColorP3uiv\0"
   "\0"
   /* _mesa_function_pool[33613]: SecondaryColor3svEXT (will be remapped) */
   "p\0"
   "glSecondaryColor3sv\0"
   "glSecondaryColor3svEXT\0"
   "\0"
   /* _mesa_function_pool[33659]: LoadTransposeMatrixfARB (will be remapped) */
   "p\0"
   "glLoadTransposeMatrixf\0"
   "glLoadTransposeMatrixfARB\0"
   "\0"
   /* _mesa_function_pool[33711]: GetPointerv (offset 329) */
   "ip\0"
   "glGetPointerv\0"
   "glGetPointervEXT\0"
   "\0"
   /* _mesa_function_pool[33746]: Tangent3bEXT (dynamic) */
   "iii\0"
   "glTangent3bEXT\0"
   "\0"
   /* _mesa_function_pool[33766]: CombinerParameterfNV (dynamic) */
   "if\0"
   "glCombinerParameterfNV\0"
   "\0"
   /* _mesa_function_pool[33793]: IndexMask (offset 212) */
   "i\0"
   "glIndexMask\0"
   "\0"
   /* _mesa_function_pool[33808]: BindProgramNV (will be remapped) */
   "ii\0"
   "glBindProgramARB\0"
   "glBindProgramNV\0"
   "\0"
   /* _mesa_function_pool[33845]: VertexAttrib4svARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4sv\0"
   "glVertexAttrib4svARB\0"
   "\0"
   /* _mesa_function_pool[33888]: GetFloatv (offset 262) */
   "ip\0"
   "glGetFloatv\0"
   "\0"
   /* _mesa_function_pool[33904]: CreateDebugObjectMESA (dynamic) */
   "\0"
   "glCreateDebugObjectMESA\0"
   "\0"
   /* _mesa_function_pool[33930]: GetShaderiv (will be remapped) */
   "iip\0"
   "glGetShaderiv\0"
   "\0"
   /* _mesa_function_pool[33949]: DrawArraysInstancedBaseInstance (will be remapped) */
   "iiiii\0"
   "glDrawArraysInstancedBaseInstance\0"
   "\0"
   /* _mesa_function_pool[33990]: ClientWaitSync (will be remapped) */
   "iii\0"
   "glClientWaitSync\0"
   "\0"
   /* _mesa_function_pool[34012]: TexCoord4s (offset 124) */
   "iiii\0"
   "glTexCoord4s\0"
   "\0"
   /* _mesa_function_pool[34031]: TexCoord3sv (offset 117) */
   "p\0"
   "glTexCoord3sv\0"
   "\0"
   /* _mesa_function_pool[34048]: BindFragmentShaderATI (will be remapped) */
   "i\0"
   "glBindFragmentShaderATI\0"
   "\0"
   /* _mesa_function_pool[34075]: PopAttrib (offset 218) */
   "\0"
   "glPopAttrib\0"
   "\0"
   /* _mesa_function_pool[34089]: Fogfv (offset 154) */
   "ip\0"
   "glFogfv\0"
   "\0"
   /* _mesa_function_pool[34101]: UnmapBufferARB (will be remapped) */
   "i\0"
   "glUnmapBuffer\0"
   "glUnmapBufferARB\0"
   "\0"
   /* _mesa_function_pool[34135]: InitNames (offset 197) */
   "\0"
   "glInitNames\0"
   "\0"
   /* _mesa_function_pool[34149]: Normal3sv (offset 61) */
   "p\0"
   "glNormal3sv\0"
   "\0"
   /* _mesa_function_pool[34164]: Minmax (offset 368) */
   "iii\0"
   "glMinmax\0"
   "glMinmaxEXT\0"
   "\0"
   /* _mesa_function_pool[34190]: TexCoord4d (offset 118) */
   "dddd\0"
   "glTexCoord4d\0"
   "\0"
   /* _mesa_function_pool[34209]: TexCoord4f (offset 120) */
   "ffff\0"
   "glTexCoord4f\0"
   "\0"
   /* _mesa_function_pool[34228]: FogCoorddvEXT (will be remapped) */
   "p\0"
   "glFogCoorddv\0"
   "glFogCoorddvEXT\0"
   "\0"
   /* _mesa_function_pool[34260]: InvalidateFramebuffer (will be remapped) */
   "iip\0"
   "glInvalidateFramebuffer\0"
   "\0"
   /* _mesa_function_pool[34289]: GetFragmentLightfvSGIX (dynamic) */
   "iip\0"
   "glGetFragmentLightfvSGIX\0"
   "\0"
   /* _mesa_function_pool[34319]: Binormal3fvEXT (dynamic) */
   "p\0"
   "glBinormal3fvEXT\0"
   "\0"
   /* _mesa_function_pool[34339]: GetBooleanv (offset 258) */
   "ip\0"
   "glGetBooleanv\0"
   "\0"
   /* _mesa_function_pool[34357]: ColorFragmentOp3ATI (will be remapped) */
   "iiiiiiiiiiiii\0"
   "glColorFragmentOp3ATI\0"
   "\0"
   /* _mesa_function_pool[34394]: Hint (offset 158) */
   "ii\0"
   "glHint\0"
   "\0"
   /* _mesa_function_pool[34405]: Color4dv (offset 28) */
   "p\0"
   "glColor4dv\0"
   "\0"
   /* _mesa_function_pool[34419]: VertexAttrib2svARB (will be remapped) */
   "ip\0"
   "glVertexAttrib2sv\0"
   "glVertexAttrib2svARB\0"
   "\0"
   /* _mesa_function_pool[34462]: AreProgramsResidentNV (will be remapped) */
   "ipp\0"
   "glAreProgramsResidentNV\0"
   "\0"
   /* _mesa_function_pool[34491]: WindowPos3svMESA (will be remapped) */
   "p\0"
   "glWindowPos3sv\0"
   "glWindowPos3svARB\0"
   "glWindowPos3svMESA\0"
   "\0"
   /* _mesa_function_pool[34546]: CopyColorSubTable (offset 347) */
   "iiiii\0"
   "glCopyColorSubTable\0"
   "glCopyColorSubTableEXT\0"
   "\0"
   /* _mesa_function_pool[34596]: WeightdvARB (dynamic) */
   "ip\0"
   "glWeightdvARB\0"
   "\0"
   /* _mesa_function_pool[34614]: DeleteRenderbuffersEXT (will be remapped) */
   "ip\0"
   "glDeleteRenderbuffers\0"
   "glDeleteRenderbuffersEXT\0"
   "\0"
   /* _mesa_function_pool[34665]: VertexAttrib4NubvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib4Nubv\0"
   "glVertexAttrib4NubvARB\0"
   "\0"
   /* _mesa_function_pool[34712]: VertexAttrib3dvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib3dvNV\0"
   "\0"
   /* _mesa_function_pool[34736]: GetObjectParameterfvARB (will be remapped) */
   "iip\0"
   "glGetObjectParameterfvARB\0"
   "\0"
   /* _mesa_function_pool[34767]: Vertex4iv (offset 147) */
   "p\0"
   "glVertex4iv\0"
   "\0"
   /* _mesa_function_pool[34782]: GetProgramEnvParameterdvARB (will be remapped) */
   "iip\0"
   "glGetProgramEnvParameterdvARB\0"
   "\0"
   /* _mesa_function_pool[34817]: TexCoord4dv (offset 119) */
   "p\0"
   "glTexCoord4dv\0"
   "\0"
   /* _mesa_function_pool[34834]: LockArraysEXT (will be remapped) */
   "ii\0"
   "glLockArraysEXT\0"
   "\0"
   /* _mesa_function_pool[34854]: Begin (offset 7) */
   "i\0"
   "glBegin\0"
   "\0"
   /* _mesa_function_pool[34865]: LightModeli (offset 165) */
   "ii\0"
   "glLightModeli\0"
   "\0"
   /* _mesa_function_pool[34883]: FogCoordPointerListIBM (dynamic) */
   "iipi\0"
   "glFogCoordPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[34914]: Rectfv (offset 89) */
   "pp\0"
   "glRectfv\0"
   "\0"
   /* _mesa_function_pool[34927]: LightModelf (offset 163) */
   "if\0"
   "glLightModelf\0"
   "\0"
   /* _mesa_function_pool[34945]: GetTexParameterfv (offset 282) */
   "iip\0"
   "glGetTexParameterfv\0"
   "\0"
   /* _mesa_function_pool[34970]: GetLightfv (offset 264) */
   "iip\0"
   "glGetLightfv\0"
   "\0"
   /* _mesa_function_pool[34988]: PixelTransformParameterivEXT (dynamic) */
   "iip\0"
   "glPixelTransformParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[35024]: BinormalPointerEXT (dynamic) */
   "iip\0"
   "glBinormalPointerEXT\0"
   "\0"
   /* _mesa_function_pool[35050]: VertexP4uiv (will be remapped) */
   "ip\0"
   "glVertexP4uiv\0"
   "\0"
   /* _mesa_function_pool[35068]: GetCombinerInputParameterivNV (dynamic) */
   "iiiip\0"
   "glGetCombinerInputParameterivNV\0"
   "\0"
   /* _mesa_function_pool[35107]: Disable (offset 214) */
   "i\0"
   "glDisable\0"
   "\0"
   /* _mesa_function_pool[35120]: MultiTexCoord2fvARB (offset 387) */
   "ip\0"
   "glMultiTexCoord2fv\0"
   "glMultiTexCoord2fvARB\0"
   "\0"
   /* _mesa_function_pool[35165]: GetRenderbufferParameterivEXT (will be remapped) */
   "iip\0"
   "glGetRenderbufferParameteriv\0"
   "glGetRenderbufferParameterivEXT\0"
   "\0"
   /* _mesa_function_pool[35231]: CombinerParameterivNV (dynamic) */
   "ip\0"
   "glCombinerParameterivNV\0"
   "\0"
   /* _mesa_function_pool[35259]: GenFragmentShadersATI (will be remapped) */
   "i\0"
   "glGenFragmentShadersATI\0"
   "\0"
   /* _mesa_function_pool[35286]: DrawArrays (offset 310) */
   "iii\0"
   "glDrawArrays\0"
   "glDrawArraysEXT\0"
   "\0"
   /* _mesa_function_pool[35320]: WeightuivARB (dynamic) */
   "ip\0"
   "glWeightuivARB\0"
   "\0"
   /* _mesa_function_pool[35339]: GetVertexAttribIivEXT (will be remapped) */
   "iip\0"
   "glGetVertexAttribIivEXT\0"
   "glGetVertexAttribIiv\0"
   "\0"
   /* _mesa_function_pool[35389]: VertexAttrib2sARB (will be remapped) */
   "iii\0"
   "glVertexAttrib2s\0"
   "glVertexAttrib2sARB\0"
   "\0"
   /* _mesa_function_pool[35431]: GetnTexImageARB (will be remapped) */
   "iiiiip\0"
   "glGetnTexImageARB\0"
   "\0"
   /* _mesa_function_pool[35457]: ColorMask (offset 210) */
   "iiii\0"
   "glColorMask\0"
   "\0"
   /* _mesa_function_pool[35475]: GenAsyncMarkersSGIX (dynamic) */
   "i\0"
   "glGenAsyncMarkersSGIX\0"
   "\0"
   /* _mesa_function_pool[35500]: DebugMessageInsertARB (will be remapped) */
   "iiiiip\0"
   "glDebugMessageInsertARB\0"
   "\0"
   /* _mesa_function_pool[35532]: GetListParameterivSGIX (dynamic) */
   "iip\0"
   "glGetListParameterivSGIX\0"
   "\0"
   /* _mesa_function_pool[35562]: BindBufferARB (will be remapped) */
   "ii\0"
   "glBindBuffer\0"
   "glBindBufferARB\0"
   "\0"
   /* _mesa_function_pool[35595]: GetInfoLogARB (will be remapped) */
   "iipp\0"
   "glGetInfoLogARB\0"
   "\0"
   /* _mesa_function_pool[35617]: RasterPos4iv (offset 83) */
   "p\0"
   "glRasterPos4iv\0"
   "\0"
   /* _mesa_function_pool[35635]: Enable (offset 215) */
   "i\0"
   "glEnable\0"
   "\0"
   /* _mesa_function_pool[35647]: LineStipple (offset 167) */
   "ii\0"
   "glLineStipple\0"
   "\0"
   /* _mesa_function_pool[35665]: FragmentLightfSGIX (dynamic) */
   "iif\0"
   "glFragmentLightfSGIX\0"
   "\0"
   /* _mesa_function_pool[35691]: SetFenceNV (dynamic) */
   "ii\0"
   "glSetFenceNV\0"
   "\0"
   /* _mesa_function_pool[35708]: VertexAttribs4svNV (will be remapped) */
   "iip\0"
   "glVertexAttribs4svNV\0"
   "\0"
   /* _mesa_function_pool[35734]: DrawElementsInstancedBaseInstance (will be remapped) */
   "iiipii\0"
   "glDrawElementsInstancedBaseInstance\0"
   "\0"
   /* _mesa_function_pool[35778]: EdgeFlagPointerListIBM (dynamic) */
   "ipi\0"
   "glEdgeFlagPointerListIBM\0"
   "\0"
   /* _mesa_function_pool[35808]: UniformMatrix3x2fv (will be remapped) */
   "iiip\0"
   "glUniformMatrix3x2fv\0"
   "\0"
   /* _mesa_function_pool[35835]: GetMinmaxParameterfv (offset 365) */
   "iip\0"
   "glGetMinmaxParameterfv\0"
   "glGetMinmaxParameterfvEXT\0"
   "\0"
   /* _mesa_function_pool[35889]: VertexAttrib1fvARB (will be remapped) */
   "ip\0"
   "glVertexAttrib1fv\0"
   "glVertexAttrib1fvARB\0"
   "\0"
   /* _mesa_function_pool[35932]: GenBuffersARB (will be remapped) */
   "ip\0"
   "glGenBuffers\0"
   "glGenBuffersARB\0"
   "\0"
   /* _mesa_function_pool[35965]: VertexAttribs1svNV (will be remapped) */
   "iip\0"
   "glVertexAttribs1svNV\0"
   "\0"
   /* _mesa_function_pool[35991]: Vertex3fv (offset 137) */
   "p\0"
   "glVertex3fv\0"
   "\0"
   /* _mesa_function_pool[36006]: GetUniformIndices (will be remapped) */
   "iipp\0"
   "glGetUniformIndices\0"
   "\0"
   /* _mesa_function_pool[36032]: GetTexBumpParameterivATI (will be remapped) */
   "ip\0"
   "glGetTexBumpParameterivATI\0"
   "\0"
   /* _mesa_function_pool[36063]: Binormal3bEXT (dynamic) */
   "iii\0"
   "glBinormal3bEXT\0"
   "\0"
   /* _mesa_function_pool[36084]: FragmentMaterialivSGIX (dynamic) */
   "iip\0"
   "glFragmentMaterialivSGIX\0"
   "\0"
   /* _mesa_function_pool[36114]: IsRenderbufferEXT (will be remapped) */
   "i\0"
   "glIsRenderbuffer\0"
   "glIsRenderbufferEXT\0"
   "\0"
   /* _mesa_function_pool[36154]: GenProgramsNV (will be remapped) */
   "ip\0"
   "glGenProgramsARB\0"
   "glGenProgramsNV\0"
   "\0"
   /* _mesa_function_pool[36191]: VertexAttrib4dvNV (will be remapped) */
   "ip\0"
   "glVertexAttrib4dvNV\0"
   "\0"
   /* _mesa_function_pool[36215]: EndFragmentShaderATI (will be remapped) */
   "\0"
   "glEndFragmentShaderATI\0"
   "\0"
   /* _mesa_function_pool[36240]: Binormal3iEXT (dynamic) */
   "iii\0"
   "glBinormal3iEXT\0"
   "\0"
   /* _mesa_function_pool[36261]: WindowPos2fMESA (will be remapped) */
   "ff\0"
   "glWindowPos2f\0"
   "glWindowPos2fARB\0"
   "glWindowPos2fMESA\0"
   "\0"
   ;

/* these functions need to be remapped */
static const struct gl_function_pool_remap MESA_remap_table_functions[] = {
   {  1731, AttachShader_remap_index },
   { 11205, CreateProgram_remap_index },
   { 25241, CreateShader_remap_index },
   { 27944, DeleteProgram_remap_index },
   { 20544, DeleteShader_remap_index },
   { 25773, DetachShader_remap_index },
   { 19820, GetAttachedShaders_remap_index },
   {  5509, GetProgramInfoLog_remap_index },
   {   453, GetProgramiv_remap_index },
   {  7565, GetShaderInfoLog_remap_index },
   { 33930, GetShaderiv_remap_index },
   { 14741, IsProgram_remap_index },
   { 13643, IsShader_remap_index },
   { 11335, StencilFuncSeparate_remap_index },
   {  4386, StencilMaskSeparate_remap_index },
   {  8745, StencilOpSeparate_remap_index },
   { 24436, UniformMatrix2x3fv_remap_index },
   {  3218, UniformMatrix2x4fv_remap_index },
   { 35808, UniformMatrix3x2fv_remap_index },
   { 33255, UniformMatrix3x4fv_remap_index },
   { 17841, UniformMatrix4x2fv_remap_index },
   {  3687, UniformMatrix4x3fv_remap_index },
   {  5764, ClampColor_remap_index },
   { 19874, ClearBufferfi_remap_index },
   { 19290, ClearBufferfv_remap_index },
   { 32236, ClearBufferiv_remap_index },
   { 14939, ClearBufferuiv_remap_index },
   { 21881, GetStringi_remap_index },
   {   986, FramebufferTexture_remap_index },
   { 29011, GetBufferParameteri64v_remap_index },
   { 11435, GetInteger64i_v_remap_index },
   { 25583, VertexAttribDivisor_remap_index },
   { 11223, LoadTransposeMatrixdARB_remap_index },
   { 33659, LoadTransposeMatrixfARB_remap_index },
   {  6569, MultTransposeMatrixdARB_remap_index },
   { 25960, MultTransposeMatrixfARB_remap_index },
   {   264, SampleCoverageARB_remap_index },
   {  6753, CompressedTexImage1DARB_remap_index },
   { 26488, CompressedTexImage2DARB_remap_index },
   {  4474, CompressedTexImage3DARB_remap_index },
   { 20125, CompressedTexSubImage1DARB_remap_index },
   {  2299, CompressedTexSubImage2DARB_remap_index },
   { 22339, CompressedTexSubImage3DARB_remap_index },
   { 31316, GetCompressedTexImageARB_remap_index },
   {  4260, DisableVertexAttribArrayARB_remap_index },
   { 32820, EnableVertexAttribArrayARB_remap_index },
   { 34782, GetProgramEnvParameterdvARB_remap_index },
   { 25840, GetProgramEnvParameterfvARB_remap_index },
   { 30122, GetProgramLocalParameterdvARB_remap_index },
   {  9245, GetProgramLocalParameterfvARB_remap_index },
   { 20282, GetProgramStringARB_remap_index },
   { 30317, GetProgramivARB_remap_index },
   { 22534, GetVertexAttribdvARB_remap_index },
   { 17674, GetVertexAttribfvARB_remap_index },
   { 11014, GetVertexAttribivARB_remap_index },
   { 21336, ProgramEnvParameter4dARB_remap_index },
   { 27653, ProgramEnvParameter4dvARB_remap_index },
   { 18493, ProgramEnvParameter4fARB_remap_index },
   { 10153, ProgramEnvParameter4fvARB_remap_index },
   {  4412, ProgramLocalParameter4dARB_remap_index },
   { 14451, ProgramLocalParameter4dvARB_remap_index },
   { 32257, ProgramLocalParameter4fARB_remap_index },
   { 28330, ProgramLocalParameter4fvARB_remap_index },
   { 31023, ProgramStringARB_remap_index },
   { 21629, VertexAttrib1dARB_remap_index },
   { 17244, VertexAttrib1dvARB_remap_index },
   {  4633, VertexAttrib1fARB_remap_index },
   { 35889, VertexAttrib1fvARB_remap_index },
   {  8191, VertexAttrib1sARB_remap_index },
   {  2494, VertexAttrib1svARB_remap_index },
   { 16635, VertexAttrib2dARB_remap_index },
   { 19311, VertexAttrib2dvARB_remap_index },
   {  1781, VertexAttrib2fARB_remap_index },
   { 19424, VertexAttrib2fvARB_remap_index },
   { 35389, VertexAttrib2sARB_remap_index },
   { 34419, VertexAttrib2svARB_remap_index },
   { 12635, VertexAttrib3dARB_remap_index },
   {  9820, VertexAttrib3dvARB_remap_index },
   {  1868, VertexAttrib3fARB_remap_index },
   { 24792, VertexAttrib3fvARB_remap_index },
   { 30870, VertexAttrib3sARB_remap_index },
   { 22240, VertexAttrib3svARB_remap_index },
   {  5552, VertexAttrib4NbvARB_remap_index },
   { 19697, VertexAttrib4NivARB_remap_index },
   { 24747, VertexAttrib4NsvARB_remap_index },
   { 25792, VertexAttrib4NubARB_remap_index },
   { 34665, VertexAttrib4NubvARB_remap_index },
   { 20987, VertexAttrib4NuivARB_remap_index },
   {  3534, VertexAttrib4NusvARB_remap_index },
   { 12249, VertexAttrib4bvARB_remap_index },
   { 29417, VertexAttrib4dARB_remap_index },
   { 23309, VertexAttrib4dvARB_remap_index },
   { 12789, VertexAttrib4fARB_remap_index },
   { 13193, VertexAttrib4fvARB_remap_index },
   { 11609, VertexAttrib4ivARB_remap_index },
   { 19078, VertexAttrib4sARB_remap_index },
   { 33845, VertexAttrib4svARB_remap_index },
   { 18298, VertexAttrib4ubvARB_remap_index },
   { 33144, VertexAttrib4uivARB_remap_index },
   { 22051, VertexAttrib4usvARB_remap_index },
   { 24252, VertexAttribPointerARB_remap_index },
   { 35562, BindBufferARB_remap_index },
   {  7863, BufferDataARB_remap_index },
   {  1599, BufferSubDataARB_remap_index },
   { 33430, DeleteBuffersARB_remap_index },
   { 35932, GenBuffersARB_remap_index },
   { 19467, GetBufferParameterivARB_remap_index },
   { 18445, GetBufferPointervARB_remap_index },
   {  1552, GetBufferSubDataARB_remap_index },
   { 33092, IsBufferARB_remap_index },
   { 28855, MapBufferARB_remap_index },
   { 34101, UnmapBufferARB_remap_index },
   {   360, BeginQueryARB_remap_index },
   { 21707, DeleteQueriesARB_remap_index },
   { 13504, EndQueryARB_remap_index },
   { 31795, GenQueriesARB_remap_index },
   {  2191, GetQueryObjectivARB_remap_index },
   { 19122, GetQueryObjectuivARB_remap_index },
   {  1970, GetQueryivARB_remap_index },
   { 21958, IsQueryARB_remap_index },
   {  9430, AttachObjectARB_remap_index },
   { 20506, CompileShaderARB_remap_index },
   {  3629, CreateProgramObjectARB_remap_index },
   {  7808, CreateShaderObjectARB_remap_index },
   { 15918, DeleteObjectARB_remap_index },
   { 26279, DetachObjectARB_remap_index },
   { 13257, GetActiveUniformARB_remap_index },
   { 10656, GetAttachedObjectsARB_remap_index },
   { 10963, GetHandleARB_remap_index },
   { 35595, GetInfoLogARB_remap_index },
   { 34736, GetObjectParameterfvARB_remap_index },
   { 29996, GetObjectParameterivARB_remap_index },
   { 31553, GetShaderSourceARB_remap_index },
   { 30730, GetUniformLocationARB_remap_index },
   { 26062, GetUniformfvARB_remap_index },
   { 14026, GetUniformivARB_remap_index },
   { 22096, LinkProgramARB_remap_index },
   { 22154, ShaderSourceARB_remap_index },
   {  8590, Uniform1fARB_remap_index },
   { 32483, Uniform1fvARB_remap_index },
   { 24177, Uniform1iARB_remap_index },
   { 22944, Uniform1ivARB_remap_index },
   {  2443, Uniform2fARB_remap_index },
   { 15754, Uniform2fvARB_remap_index },
   { 28722, Uniform2iARB_remap_index },
   {  2563, Uniform2ivARB_remap_index },
   { 20616, Uniform3fARB_remap_index },
   { 10686, Uniform3fvARB_remap_index },
   {  7376, Uniform3iARB_remap_index },
   { 18551, Uniform3ivARB_remap_index },
   { 21142, Uniform4fARB_remap_index },
   { 25926, Uniform4fvARB_remap_index },
   { 27263, Uniform4iARB_remap_index },
   { 22500, Uniform4ivARB_remap_index },
   {  9482, UniformMatrix2fvARB_remap_index },
   {    17, UniformMatrix3fvARB_remap_index },
   {  3005, UniformMatrix4fvARB_remap_index },
   { 27806, UseProgramObjectARB_remap_index },
   { 16323, ValidateProgramARB_remap_index },
   { 23352, BindAttribLocationARB_remap_index },
   {  5597, GetActiveAttribARB_remap_index },
   { 18232, GetAttribLocationARB_remap_index },
   { 32184, DrawBuffersARB_remap_index },
   { 31991, ClampColorARB_remap_index },
   { 19742, DrawArraysInstancedARB_remap_index },
   {  7437, DrawElementsInstancedARB_remap_index },
   { 14556, RenderbufferStorageMultisample_remap_index },
   { 15020, FramebufferTextureARB_remap_index },
   { 28232, FramebufferTextureFaceARB_remap_index },
   { 26428, ProgramParameteriARB_remap_index },
   { 25744, VertexAttribDivisorARB_remap_index },
   { 21190, FlushMappedBufferRange_remap_index },
   { 30445, MapBufferRange_remap_index },
   { 30340, TexBufferARB_remap_index },
   { 17979, BindVertexArray_remap_index },
   { 16127, GenVertexArrays_remap_index },
   {  7986, GetActiveUniformBlockName_remap_index },
   {  5660, GetActiveUniformBlockiv_remap_index },
   { 29355, GetActiveUniformName_remap_index },
   { 11928, GetActiveUniformsiv_remap_index },
   { 29728, GetUniformBlockIndex_remap_index },
   { 36006, GetUniformIndices_remap_index },
   {  5390, UniformBlockBinding_remap_index },
   { 33022, CopyBufferSubData_remap_index },
   { 33990, ClientWaitSync_remap_index },
   {  2924, DeleteSync_remap_index },
   {  8232, FenceSync_remap_index },
   { 16694, GetInteger64v_remap_index },
   { 24854, GetSynciv_remap_index },
   { 32123, IsSync_remap_index },
   { 10586, WaitSync_remap_index },
   {  4228, DrawElementsBaseVertex_remap_index },
   { 20334, DrawElementsInstancedBaseVertex_remap_index },
   { 33334, DrawRangeElementsBaseVertex_remap_index },
   { 28886, MultiDrawElementsBaseVertex_remap_index },
   { 27726, BlendEquationSeparateiARB_remap_index },
   { 19560, BlendEquationiARB_remap_index },
   { 13965, BlendFuncSeparateiARB_remap_index },
   { 11062, BlendFunciARB_remap_index },
   {  5825, BindFragDataLocationIndexed_remap_index },
   { 23745, GetFragDataIndex_remap_index },
   {  9311, BindSampler_remap_index },
   {  4612, DeleteSamplers_remap_index },
   { 21563, GenSamplers_remap_index },
   { 32029, GetSamplerParameterIiv_remap_index },
   { 21084, GetSamplerParameterIuiv_remap_index },
   {  5429, GetSamplerParameterfv_remap_index },
   { 27963, GetSamplerParameteriv_remap_index },
   { 15707, IsSampler_remap_index },
   { 17932, SamplerParameterIiv_remap_index },
   { 33373, SamplerParameterIuiv_remap_index },
   { 24488, SamplerParameterf_remap_index },
   { 18159, SamplerParameterfv_remap_index },
   { 24463, SamplerParameteri_remap_index },
   { 19936, SamplerParameteriv_remap_index },
   {  7035, QueryCounter_remap_index },
   { 15738, ColorP3ui_remap_index },
   { 33596, ColorP3uiv_remap_index },
   { 23050, ColorP4ui_remap_index },
   { 14861, ColorP4uiv_remap_index },
   { 10631, MultiTexCoordP1ui_remap_index },
   { 13939, MultiTexCoordP1uiv_remap_index },
   { 19016, MultiTexCoordP2ui_remap_index },
   { 32794, MultiTexCoordP2uiv_remap_index },
   { 15272, MultiTexCoordP3ui_remap_index },
   { 11685, MultiTexCoordP3uiv_remap_index },
   { 24030, MultiTexCoordP4ui_remap_index },
   { 15050, MultiTexCoordP4uiv_remap_index },
   { 32466, NormalP3ui_remap_index },
   { 20838, NormalP3uiv_remap_index },
   { 27168, SecondaryColorP3ui_remap_index },
   { 20856, SecondaryColorP3uiv_remap_index },
   {  1013, TexCoordP1ui_remap_index },
   {  2666, TexCoordP1uiv_remap_index },
   { 16203, TexCoordP2ui_remap_index },
   { 12314, TexCoordP2uiv_remap_index },
   { 13898, TexCoordP3ui_remap_index },
   { 22743, TexCoordP3uiv_remap_index },
   {  2099, TexCoordP4ui_remap_index },
   {  6118, TexCoordP4uiv_remap_index },
   { 11105, VertexAttribP1ui_remap_index },
   { 13307, VertexAttribP1uiv_remap_index },
   {  8850, VertexAttribP2ui_remap_index },
   { 31267, VertexAttribP2uiv_remap_index },
   {  5099, VertexAttribP3ui_remap_index },
   { 28795, VertexAttribP3uiv_remap_index },
   { 15511, VertexAttribP4ui_remap_index },
   { 17501, VertexAttribP4uiv_remap_index },
   {  5535, VertexP2ui_remap_index },
   { 17071, VertexP2uiv_remap_index },
   {  3331, VertexP3ui_remap_index },
   { 21670, VertexP3uiv_remap_index },
   { 10614, VertexP4ui_remap_index },
   { 35050, VertexP4uiv_remap_index },
   {  5861, BindTransformFeedback_remap_index },
   { 11485, DeleteTransformFeedbacks_remap_index },
   {  7409, DrawTransformFeedback_remap_index },
   { 10823, GenTransformFeedbacks_remap_index },
   { 30913, IsTransformFeedback_remap_index },
   { 20376, PauseTransformFeedback_remap_index },
   { 18919, ResumeTransformFeedback_remap_index },
   {  2686, BeginQueryIndexed_remap_index },
   { 25609, DrawTransformFeedbackStream_remap_index },
   { 16821, EndQueryIndexed_remap_index },
   {  3581, GetQueryIndexediv_remap_index },
   { 23689, ClearDepthf_remap_index },
   {  7756, DepthRangef_remap_index },
   { 15939, GetShaderPrecisionFormat_remap_index },
   { 11275, ReleaseShaderCompiler_remap_index },
   { 12292, ShaderBinary_remap_index },
   {  1480, DebugMessageCallbackARB_remap_index },
   { 10981, DebugMessageControlARB_remap_index },
   { 35500, DebugMessageInsertARB_remap_index },
   {  3103, GetDebugMessageLogARB_remap_index },
   { 31211, GetGraphicsResetStatusARB_remap_index },
   { 27043, GetnColorTableARB_remap_index },
   {  9050, GetnCompressedTexImageARB_remap_index },
   {  4320, GetnConvolutionFilterARB_remap_index },
   { 17287, GetnHistogramARB_remap_index },
   {  5002, GetnMapdvARB_remap_index },
   { 24208, GetnMapfvARB_remap_index },
   {  2422, GetnMapivARB_remap_index },
   { 18813, GetnMinmaxARB_remap_index },
   {  4977, GetnPixelMapfvARB_remap_index },
   { 20961, GetnPixelMapuivARB_remap_index },
   { 32097, GetnPixelMapusvARB_remap_index },
   {  1402, GetnPolygonStippleARB_remap_index },
   { 23134, GetnSeparableFilterARB_remap_index },
   { 35431, GetnTexImageARB_remap_index },
   { 20309, GetnUniformdvARB_remap_index },
   {  8263, GetnUniformfvARB_remap_index },
   { 11130, GetnUniformivARB_remap_index },
   { 24703, GetnUniformuivARB_remap_index },
   {  3245, ReadnPixelsARB_remap_index },
   { 33949, DrawArraysInstancedBaseInstance_remap_index },
   { 35734, DrawElementsInstancedBaseInstance_remap_index },
   {  8673, DrawElementsInstancedBaseVertexBaseInstance_remap_index },
   {  6287, DrawTransformFeedbackInstanced_remap_index },
   { 18874, DrawTransformFeedbackStreamInstanced_remap_index },
   {  1289, TexStorage1D_remap_index },
   {  4867, TexStorage2D_remap_index },
   { 17785, TexStorage3D_remap_index },
   {  5023, TextureStorage1DEXT_remap_index },
   {  5889, TextureStorage2DEXT_remap_index },
   {  1750, TextureStorage3DEXT_remap_index },
   { 23169, InvalidateBufferData_remap_index },
   { 17643, InvalidateBufferSubData_remap_index },
   { 34260, InvalidateFramebuffer_remap_index },
   { 22303, InvalidateSubFramebuffer_remap_index },
   {  4449, InvalidateTexImage_remap_index },
   {  3483, InvalidateTexSubImage_remap_index },
   {  6325, PolygonOffsetEXT_remap_index },
   { 18102, SampleMaskSGIS_remap_index },
   { 21898, SamplePatternSGIS_remap_index },
   { 21581, ColorPointerEXT_remap_index },
   { 19354, EdgeFlagPointerEXT_remap_index },
   {  7011, IndexPointerEXT_remap_index },
   {  7110, NormalPointerEXT_remap_index },
   { 17355, TexCoordPointerEXT_remap_index },
   {  8021, VertexPointerEXT_remap_index },
   {  4030, PointParameterfEXT_remap_index },
   {  8946, PointParameterfvEXT_remap_index },
   { 34834, LockArraysEXT_remap_index },
   { 16387, UnlockArraysEXT_remap_index },
   {  1310, SecondaryColor3bEXT_remap_index },
   {  9139, SecondaryColor3bvEXT_remap_index },
   { 11834, SecondaryColor3dEXT_remap_index },
   { 28012, SecondaryColor3dvEXT_remap_index },
   { 30779, SecondaryColor3fEXT_remap_index },
   { 20061, SecondaryColor3fvEXT_remap_index },
   {   518, SecondaryColor3iEXT_remap_index },
   { 17722, SecondaryColor3ivEXT_remap_index },
   { 11363, SecondaryColor3sEXT_remap_index },
   { 33613, SecondaryColor3svEXT_remap_index },
   { 29832, SecondaryColor3ubEXT_remap_index },
   { 23243, SecondaryColor3ubvEXT_remap_index },
   { 14306, SecondaryColor3uiEXT_remap_index },
   { 25096, SecondaryColor3uivEXT_remap_index },
   { 28282, SecondaryColor3usEXT_remap_index },
   { 14379, SecondaryColor3usvEXT_remap_index },
   { 13136, SecondaryColorPointerEXT_remap_index },
   { 28096, MultiDrawArraysEXT_remap_index },
   { 22879, MultiDrawElementsEXT_remap_index },
   { 23090, FogCoordPointerEXT_remap_index },
   {  5150, FogCoorddEXT_remap_index },
   { 34228, FogCoorddvEXT_remap_index },
   {  5267, FogCoordfEXT_remap_index },
   { 29755, FogCoordfvEXT_remap_index },
   { 30372, BlendFuncSeparateEXT_remap_index },
   { 13917, ResizeBuffersMESA_remap_index },
   { 12462, WindowPos2dMESA_remap_index },
   {  1082, WindowPos2dvMESA_remap_index },
   { 36261, WindowPos2fMESA_remap_index },
   {  9084, WindowPos2fvMESA_remap_index },
   { 20008, WindowPos2iMESA_remap_index },
   { 22407, WindowPos2ivMESA_remap_index },
   { 22978, WindowPos2sMESA_remap_index },
   {  6667, WindowPos2svMESA_remap_index },
   {  8875, WindowPos3dMESA_remap_index },
   { 15456, WindowPos3dvMESA_remap_index },
   {   564, WindowPos3fMESA_remap_index },
   { 16448, WindowPos3fvMESA_remap_index },
   { 26321, WindowPos3iMESA_remap_index },
   { 32967, WindowPos3ivMESA_remap_index },
   { 20760, WindowPos3sMESA_remap_index },
   { 34491, WindowPos3svMESA_remap_index },
   { 12413, WindowPos4dMESA_remap_index },
   { 18689, WindowPos4dvMESA_remap_index },
   { 15415, WindowPos4fMESA_remap_index },
   { 33503, WindowPos4fvMESA_remap_index },
   { 33120, WindowPos4iMESA_remap_index },
   { 13757, WindowPos4ivMESA_remap_index },
   { 20937, WindowPos4sMESA_remap_index },
   {  3607, WindowPos4svMESA_remap_index },
   { 29385, MultiModeDrawArraysIBM_remap_index },
   { 31666, MultiModeDrawElementsIBM_remap_index },
   { 34462, AreProgramsResidentNV_remap_index },
   { 33808, BindProgramNV_remap_index },
   { 28365, DeleteProgramsNV_remap_index },
   { 23461, ExecuteProgramNV_remap_index },
   { 36154, GenProgramsNV_remap_index },
   {  3885, GetProgramParameterdvNV_remap_index },
   { 11896, GetProgramParameterfvNV_remap_index },
   { 28769, GetProgramStringNV_remap_index },
   { 26698, GetProgramivNV_remap_index },
   { 25875, GetTrackMatrixivNV_remap_index },
   { 28554, GetVertexAttribPointervNV_remap_index },
   { 11458, GetVertexAttribdvNV_remap_index },
   { 20733, GetVertexAttribfvNV_remap_index },
   { 20255, GetVertexAttribivNV_remap_index },
   { 21220, IsProgramNV_remap_index },
   { 10564, LoadProgramNV_remap_index },
   { 30468, ProgramParameters4dvNV_remap_index },
   { 26628, ProgramParameters4fvNV_remap_index },
   { 22711, RequestResidentProgramsNV_remap_index },
   { 33312, TrackMatrixNV_remap_index },
   { 28058, VertexAttrib1dNV_remap_index },
   { 14961, VertexAttrib1dvNV_remap_index },
   { 31119, VertexAttrib1fNV_remap_index },
   {  2730, VertexAttrib1fvNV_remap_index },
   { 33560, VertexAttrib1sNV_remap_index },
   { 16521, VertexAttrib1svNV_remap_index },
   {  5485, VertexAttrib2dNV_remap_index },
   { 20189, VertexAttrib2dvNV_remap_index },
   { 22130, VertexAttrib2fNV_remap_index },
   { 14427, VertexAttrib2fvNV_remap_index },
   {  6879, VertexAttrib2sNV_remap_index },
   { 20814, VertexAttrib2svNV_remap_index },
   { 12610, VertexAttrib3dNV_remap_index },
   { 34712, VertexAttrib3dvNV_remap_index },
   { 11660, VertexAttrib3fNV_remap_index },
   { 27070, VertexAttrib3fvNV_remap_index },
   { 31094, VertexAttrib3sNV_remap_index },
   { 25902, VertexAttrib3svNV_remap_index },
   { 31640, VertexAttrib4dNV_remap_index },
   { 36191, VertexAttrib4dvNV_remap_index },
   {  5932, VertexAttrib4fNV_remap_index },
   { 27465, VertexAttrib4fvNV_remap_index },
   { 29265, VertexAttrib4sNV_remap_index },
   {  1510, VertexAttrib4svNV_remap_index },
   {  5692, VertexAttrib4ubNV_remap_index },
   {   826, VertexAttrib4ubvNV_remap_index },
   { 23658, VertexAttribPointerNV_remap_index },
   {  2537, VertexAttribs1dvNV_remap_index },
   { 20882, VertexAttribs1fvNV_remap_index },
   { 35965, VertexAttribs1svNV_remap_index },
   { 11711, VertexAttribs2dvNV_remap_index },
   { 32617, VertexAttribs2fvNV_remap_index },
   { 19380, VertexAttribs2svNV_remap_index },
   {  6138, VertexAttribs3dvNV_remap_index },
   {  2396, VertexAttribs3fvNV_remap_index },
   { 32689, VertexAttribs3svNV_remap_index },
   { 19264, VertexAttribs4dvNV_remap_index },
   {  6210, VertexAttribs4fvNV_remap_index },
   { 35708, VertexAttribs4svNV_remap_index },
   { 32421, VertexAttribs4ubvNV_remap_index },
   { 29461, GetTexBumpParameterfvATI_remap_index },
   { 36032, GetTexBumpParameterivATI_remap_index },
   { 20459, TexBumpParameterfvATI_remap_index },
   { 22582, TexBumpParameterivATI_remap_index },
   { 17107, AlphaFragmentOp1ATI_remap_index },
   {  9397, AlphaFragmentOp2ATI_remap_index },
   { 26986, AlphaFragmentOp3ATI_remap_index },
   { 15388, BeginFragmentShaderATI_remap_index },
   { 34048, BindFragmentShaderATI_remap_index },
   { 26031, ColorFragmentOp1ATI_remap_index },
   {  4833, ColorFragmentOp2ATI_remap_index },
   { 34357, ColorFragmentOp3ATI_remap_index },
   {  6414, DeleteFragmentShaderATI_remap_index },
   { 36215, EndFragmentShaderATI_remap_index },
   { 35259, GenFragmentShadersATI_remap_index },
   { 27921, PassTexCoordATI_remap_index },
   {  7966, SampleMapATI_remap_index },
   { 29623, SetFragmentShaderConstantATI_remap_index },
   {   411, PointParameteriNV_remap_index },
   { 15663, PointParameterivNV_remap_index },
   { 31479, ActiveStencilFaceEXT_remap_index },
   { 30096, BindVertexArrayAPPLE_remap_index },
   {  3052, DeleteVertexArraysAPPLE_remap_index },
   { 19847, GenVertexArraysAPPLE_remap_index },
   { 25677, IsVertexArrayAPPLE_remap_index },
   {   867, GetProgramNamedParameterdvNV_remap_index },
   {  3993, GetProgramNamedParameterfvNV_remap_index },
   { 29492, ProgramNamedParameter4dNV_remap_index },
   { 16002, ProgramNamedParameter4dvNV_remap_index },
   { 10087, ProgramNamedParameter4fNV_remap_index },
   { 13101, ProgramNamedParameter4fvNV_remap_index },
   { 18600, PrimitiveRestartIndexNV_remap_index },
   { 31293, PrimitiveRestartNV_remap_index },
   { 26607, DepthBoundsEXT_remap_index },
   {  1181, BlendEquationSeparateEXT_remap_index },
   { 16222, BindFramebufferEXT_remap_index },
   { 28141, BindRenderbufferEXT_remap_index },
   { 10879, CheckFramebufferStatusEXT_remap_index },
   { 24897, DeleteFramebuffersEXT_remap_index },
   { 34614, DeleteRenderbuffersEXT_remap_index },
   { 14878, FramebufferRenderbufferEXT_remap_index },
   { 15076, FramebufferTexture1DEXT_remap_index },
   { 12895, FramebufferTexture2DEXT_remap_index },
   { 12515, FramebufferTexture3DEXT_remap_index },
   { 25512, GenFramebuffersEXT_remap_index },
   { 19219, GenRenderbuffersEXT_remap_index },
   {  7702, GenerateMipmapEXT_remap_index },
   { 23813, GetFramebufferAttachmentParameterivEXT_remap_index },
   { 35165, GetRenderbufferParameterivEXT_remap_index },
   { 22462, IsFramebufferEXT_remap_index },
   { 36114, IsRenderbufferEXT_remap_index },
   {  9329, RenderbufferStorageEXT_remap_index },
   {   743, BlitFramebufferEXT_remap_index },
   { 15788, BufferParameteriAPPLE_remap_index },
   { 21252, FlushMappedBufferRangeAPPLE_remap_index },
   {  2045, BindFragDataLocationEXT_remap_index },
   { 10513, GetFragDataLocationEXT_remap_index },
   { 12015, GetUniformuivEXT_remap_index },
   { 35339, GetVertexAttribIivEXT_remap_index },
   { 30639, GetVertexAttribIuivEXT_remap_index },
   { 13365, Uniform1uiEXT_remap_index },
   { 30553, Uniform1uivEXT_remap_index },
   { 24378, Uniform2uiEXT_remap_index },
   {  4797, Uniform2uivEXT_remap_index },
   { 31919, Uniform3uiEXT_remap_index },
   { 16149, Uniform3uivEXT_remap_index },
   {  3917, Uniform4uiEXT_remap_index },
   {  9863, Uniform4uivEXT_remap_index },
   {  7522, VertexAttribI1iEXT_remap_index },
   {  5719, VertexAttribI1ivEXT_remap_index },
   {  2831, VertexAttribI1uiEXT_remap_index },
   { 14155, VertexAttribI1uivEXT_remap_index },
   {    81, VertexAttribI2iEXT_remap_index },
   { 26143, VertexAttribI2ivEXT_remap_index },
   {  6164, VertexAttribI2uiEXT_remap_index },
   {  5312, VertexAttribI2uivEXT_remap_index },
   { 29057, VertexAttribI3iEXT_remap_index },
   { 27420, VertexAttribI3ivEXT_remap_index },
   {  3764, VertexAttribI3uiEXT_remap_index },
   { 33208, VertexAttribI3uivEXT_remap_index },
   { 24089, VertexAttribI4bvEXT_remap_index },
   { 16081, VertexAttribI4iEXT_remap_index },
   {  1911, VertexAttribI4ivEXT_remap_index },
   { 14788, VertexAttribI4svEXT_remap_index },
   { 18185, VertexAttribI4ubvEXT_remap_index },
   {   145, VertexAttribI4uiEXT_remap_index },
   {  6348, VertexAttribI4uivEXT_remap_index },
   { 12678, VertexAttribI4usvEXT_remap_index },
   { 20403, VertexAttribIPointerEXT_remap_index },
   {  3392, FramebufferTextureLayerEXT_remap_index },
   { 10471, ColorMaskIndexedEXT_remap_index },
   { 28642, DisableIndexedEXT_remap_index },
   { 29102, EnableIndexedEXT_remap_index },
   { 23768, GetBooleanIndexedvEXT_remap_index },
   { 26720, GetIntegerIndexedvEXT_remap_index },
   { 24973, IsEnabledIndexedEXT_remap_index },
   { 24873, ClearColorIiEXT_remap_index },
   {  3860, ClearColorIuiEXT_remap_index },
   { 11155, GetTexParameterIivEXT_remap_index },
   {  6827, GetTexParameterIuivEXT_remap_index },
   {  3348, TexParameterIivEXT_remap_index },
   { 28924, TexParameterIuivEXT_remap_index },
   {  5180, BeginConditionalRenderNV_remap_index },
   { 27871, EndConditionalRenderNV_remap_index },
   { 26765, BeginTransformFeedbackEXT_remap_index },
   {  6903, BindBufferBaseEXT_remap_index },
   { 25555, BindBufferOffsetEXT_remap_index },
   { 13582, BindBufferRangeEXT_remap_index },
   { 29572, EndTransformFeedbackEXT_remap_index },
   {  5984, GetTransformFeedbackVaryingEXT_remap_index },
   { 24569, TransformFeedbackVaryingsEXT_remap_index },
   { 32322, ProvokingVertexEXT_remap_index },
   { 12967, GetObjectParameterivAPPLE_remap_index },
   { 21853, ObjectPurgeableAPPLE_remap_index },
   {  6621, ObjectUnpurgeableAPPLE_remap_index },
   { 19041, ActiveProgramEXT_remap_index },
   {  4706, CreateShaderProgramEXT_remap_index },
   { 31241, UseShaderProgramEXT_remap_index },
   { 23535, TextureBarrierNV_remap_index },
   { 31505, StencilFuncSeparateATI_remap_index },
   {  6534, ProgramEnvParameters4fvEXT_remap_index },
   { 18837, ProgramLocalParameters4fvEXT_remap_index },
   { 15570, GetQueryObjecti64vEXT_remap_index },
   { 11737, GetQueryObjectui64vEXT_remap_index },
   { 26100, EGLImageTargetRenderbufferStorageOES_remap_index },
   { 13471, EGLImageTargetTexture2DOES_remap_index },
   {    -1, -1 }
};

/* these functions are in the ABI, but have alternative names */
static const struct gl_function_remap MESA_alt_functions[] = {
   /* from GL_EXT_blend_color */
   {  2970, _gloffset_BlendColor },
   /* from GL_EXT_blend_minmax */
   { 12572, _gloffset_BlendEquation },
   /* from GL_EXT_color_subtable */
   { 18711, _gloffset_ColorSubTable },
   { 34546, _gloffset_CopyColorSubTable },
   /* from GL_EXT_convolution */
   {   305, _gloffset_ConvolutionFilter1D },
   {  2769, _gloffset_CopyConvolutionFilter1D },
   {  4548, _gloffset_GetConvolutionParameteriv },
   {  9711, _gloffset_ConvolutionFilter2D },
   {  9913, _gloffset_ConvolutionParameteriv },
   { 10373, _gloffset_ConvolutionParameterfv },
   { 22610, _gloffset_GetSeparableFilter },
   { 26375, _gloffset_SeparableFilter2D },
   { 27313, _gloffset_ConvolutionParameteri },
   { 27505, _gloffset_ConvolutionParameterf },
   { 29291, _gloffset_GetConvolutionParameterfv },
   { 30262, _gloffset_GetConvolutionFilter },
   { 32904, _gloffset_CopyConvolutionFilter2D },
   /* from GL_EXT_copy_texture */
   { 16581, _gloffset_CopyTexSubImage3D },
   { 18398, _gloffset_CopyTexImage2D },
   { 26896, _gloffset_CopyTexImage1D },
   { 29943, _gloffset_CopyTexSubImage2D },
   { 32517, _gloffset_CopyTexSubImage1D },
   /* from GL_EXT_draw_range_elements */
   { 10738, _gloffset_DrawRangeElements },
   /* from GL_EXT_histogram */
   {   904, _gloffset_Histogram },
   {  3953, _gloffset_ResetHistogram },
   { 11301, _gloffset_GetMinmax },
   { 16937, _gloffset_GetHistogramParameterfv },
   { 26821, _gloffset_GetMinmaxParameteriv },
   { 29181, _gloffset_ResetMinmax },
   { 30159, _gloffset_GetHistogramParameteriv },
   { 31439, _gloffset_GetHistogram },
   { 34164, _gloffset_Minmax },
   { 35835, _gloffset_GetMinmaxParameterfv },
   /* from GL_EXT_paletted_texture */
   {  9573, _gloffset_ColorTable },
   { 16761, _gloffset_GetColorTable },
   { 25259, _gloffset_GetColorTableParameterfv },
   { 27561, _gloffset_GetColorTableParameteriv },
   /* from GL_EXT_subtexture */
   {  8147, _gloffset_TexSubImage1D },
   { 12088, _gloffset_TexSubImage2D },
   /* from GL_EXT_texture3D */
   {  2004, _gloffset_TexImage3D },
   { 25028, _gloffset_TexSubImage3D },
   /* from GL_EXT_texture_object */
   {  3714, _gloffset_PrioritizeTextures },
   {  8621, _gloffset_AreTexturesResident },
   { 14985, _gloffset_GenTextures },
   { 17314, _gloffset_DeleteTextures },
   { 21533, _gloffset_IsTexture },
   { 32582, _gloffset_BindTexture },
   /* from GL_EXT_vertex_array */
   { 26547, _gloffset_ArrayElement },
   { 33711, _gloffset_GetPointerv },
   { 35286, _gloffset_DrawArrays },
   /* from GL_SGI_color_table */
   {  8794, _gloffset_ColorTableParameteriv },
   {  9573, _gloffset_ColorTable },
   { 16761, _gloffset_GetColorTable },
   { 16893, _gloffset_CopyColorTable },
   { 21394, _gloffset_ColorTableParameterfv },
   { 25259, _gloffset_GetColorTableParameterfv },
   { 27561, _gloffset_GetColorTableParameteriv },
   /* from GL_VERSION_1_3 */
   {   473, _gloffset_MultiTexCoord3sARB },
   {   705, _gloffset_ActiveTextureARB },
   {  4735, _gloffset_MultiTexCoord1fvARB },
   {  7135, _gloffset_MultiTexCoord3dARB },
   {  7180, _gloffset_MultiTexCoord2iARB },
   {  7304, _gloffset_MultiTexCoord2svARB },
   {  9529, _gloffset_MultiTexCoord2fARB },
   { 11789, _gloffset_MultiTexCoord3fvARB },
   { 12334, _gloffset_MultiTexCoord4sARB },
   { 13015, _gloffset_MultiTexCoord2dvARB },
   { 13414, _gloffset_MultiTexCoord1svARB },
   { 13779, _gloffset_MultiTexCoord3svARB },
   { 13840, _gloffset_MultiTexCoord4iARB },
   { 14696, _gloffset_MultiTexCoord3iARB },
   { 15620, _gloffset_MultiTexCoord1dARB },
   { 15817, _gloffset_MultiTexCoord3dvARB },
   { 17155, _gloffset_MultiTexCoord3ivARB },
   { 17200, _gloffset_MultiTexCoord2sARB },
   { 18768, _gloffset_MultiTexCoord4ivARB },
   { 21034, _gloffset_ClientActiveTextureARB },
   { 23417, _gloffset_MultiTexCoord2dARB },
   { 23933, _gloffset_MultiTexCoord4dvARB },
   { 24333, _gloffset_MultiTexCoord4fvARB },
   { 25400, _gloffset_MultiTexCoord3fARB },
   { 28186, _gloffset_MultiTexCoord4dARB },
   { 28464, _gloffset_MultiTexCoord1sARB },
   { 28677, _gloffset_MultiTexCoord1dvARB },
   { 29787, _gloffset_MultiTexCoord1ivARB },
   { 29880, _gloffset_MultiTexCoord2ivARB },
   { 30219, _gloffset_MultiTexCoord1iARB },
   { 31714, _gloffset_MultiTexCoord4svARB },
   { 32364, _gloffset_MultiTexCoord1fARB },
   { 32643, _gloffset_MultiTexCoord4fARB },
   { 35120, _gloffset_MultiTexCoord2fvARB },
   {    -1, -1 }
};

