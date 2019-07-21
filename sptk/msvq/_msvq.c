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

/****************************************************************

    $Id: _msvq.c,v 1.15 2013/12/16 09:02:01 mataki Exp $

    Multi Stage Vector Quantization

       void msvq(x, cb, l, cbsize, stage, index)

       double *x      : input vector
       double *cb     : codebook vector
       int     l      : order of vector
       int    *cbsize : size of codebook
       int     stage  : number of stage
       int    *index  : index of codebook

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include "../SPTK.h"
#endif

void msvq(double *x, double *cb, const int l, int *cbsize, const int stage,
          int *index)
{
   int i, j;
   double *p;
   static double *xx = NULL;
   static int size;

   if (xx == NULL) {
      xx = dgetmem(l);
      size = l;
   }
   if (size > l) {
      free(xx);
      xx = dgetmem(l);
      size = l;
   }

   movem(x, xx, sizeof(*x), l);

   for (i = 0; i < stage; i++) {
      index[i] = vq(xx, cb, l, cbsize[i]);

      p = cb + index[i] * l;
      for (j = 0; j < l; j++)
         xx[j] -= p[j];

      cb += cbsize[i] * l;
   }
}
