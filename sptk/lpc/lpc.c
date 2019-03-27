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
*    LPC Analysis Using Levinson-Durbin Method                          *
*                                                                       *
*                                    1996.1  K.Koishida                 *
*                                                                       *
*       usage:                                                          *
*               lpc [ options ] [ infile ] > stdout                     *
*       options:                                                        *
*               -l l  :  frame length                      [256]        *
*               -m m  :  order of LPC                      [25]         *
*               -f f  :  mimimum value of the determinant               *
*                        of the normal matrix              [0.000001]   *
*       infile:                                                         *
*               data sequence                                           *
*                       , x(0), x(1), ..., x(l-1),                      *
*       stdout:                                                         *
*               LP Coefficeints                                         *
*                       , K, a(1), ..., a(m),                           *
*       require:                                                        *
*               lpc()                                                   *
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

#  include "../SPTK.h"
#  include "lpc.h"

/*  Default Values */
#define MINDET 0.000001

vector sptk_lpc(vector data, LPC_SETTINGS * settings)
{
   int m = settings->order,
       len = settings->leng,
       flag,
       t = 0,
       i, dPos, rPos;
   double *x, *a,
       f = MINDET;

   x = dgetmem(len + m + 1);
   a = x + len;
   int rLen = data.x/len*(m + 1);
   dPos = rPos = 0;
   vector res = zerov(rLen);

   while(1){
      for(i=0;i<len&&(i+dPos)<data.x;i++)
          x[i] = getv(data, i+dPos);
      if(i != len) break;
      dPos += i;

      flag = lpc(x, len, a, m, f);
      switch (flag) {
          case -1:
             //fprintf(stderr, "The coefficient matrix of the normal equation is singular at %dth frame!\n", t);
             break;
          case -2:
             //fprintf(stderr, "Extracted LPC coefficients become unstable at %dth frame!\n", t);
             break;
      }

      for(i=0;i<(m+1);i++)
          setv(res, i+rPos, a[i]);
      rPos += i;
      t++;
   }

   return res;
}
