/* ----------------------------------------------------------------- */
/*             The Speech Signal Processing Toolkit (SPTK)           */
/*             developed by SPTK Working Group                       */
/*             http://sp-tk.sourceforge.net/                         */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 1984-2007  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/*                1996-2013  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the SPTK working group nor the names of its */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

/************************************************************************
*                                                                       *
*    Data Windowing                                                     *
*                                                                       *
*                                      1996.1  N.Miyazaki               *
*                                      1998.11 T.Masuko                 *
*                                                                       *
*       usage:                                                          *
*               window [ options ] [ infile ] > outfile                 *
*       options:                                                        *
*               -l l  :  frame length of input    [256]                 *
*               -L L  :  frane length of output   [l]                   *
*               -n n  :  type of normalization    [1]                   *
*                          0 none                                       *
*                          1 normalize by power                         *
*                          2 normalize by magnitude                     *
*               -w w  :  type of window           [0]                   *
*                          0 (Blackman)                                 *
*                          1 (Hamming)                                  *
*                          2 (Hanning)                                  *
*                          3 (Bartlett)                                 *
*                          4 (trapezoid)                                *
*                          5 (rectangul)                                *
*       infile:                                                         *
*               stdin for default                                       *
*               input is assumed to be double                           *
*                                                                       *
************************************************************************/

/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#if defined(WIN32)
#  include "SPTK.h"
#  include "window.h"
#else
#  include <SPTK.h>
#  include <window.h>
#endif

/*  Default Values */
#define NORMFLG  1

vector sptk_window(vector data, WINDOW_SETTINGS * settings)
{
   int fleng = settings->leng,
       outl = -1,
       normflg = NORMFLG,
       i,j;
   Window wintype = settings->window_type;
   double *x;

   if (outl < 0)
      outl = fleng;

   vector res = makev(data.x);
   x = dgetmem(fleng > outl ? fleng : outl);

   for(i=0;i<data.x;i+=fleng){
      for(j=0;j<fleng;j++) x[j] = data.v[j+i];
      window(wintype, x, fleng, normflg);
      for(j=0;j<fleng;j++) res.v[j+i] = x[j];
   }

   return res;
}
