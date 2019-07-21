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

    $Id: _imsvq.c,v 1.16 2013/12/16 09:01:58 mataki Exp $

    Decoder of Multi Stage Vector Quantization

        imsvq(index, cb, l, cbsize, stage, x)

        int    *index  : index of codebook
        real   *cb     : codebook vector
        int    l       : order of vector
        int    *cbsize : size of codebook
        int    stage   : number of stage
        real   *x      : decoded vector

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include "../SPTK.h"
#endif

void imsvq(int *index, double *cb, const int l, int *cbsize, const int stage,
           double *x)
{
   int i, j;
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

   fillz(x, sizeof(*x), l);

   for (i = 0; i < stage; i++) {
      ivq(index[i], cb, l, xx);

      for (j = 0; j < l; j++)
         x[j] += xx[j];

      cb += cbsize[i] * l;
   }

   return;
}
