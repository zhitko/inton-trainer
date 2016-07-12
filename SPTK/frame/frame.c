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
*    Extract Frame from Data Sequence                                   *
*                                                                       *
*                                       1985.11 K.Tokuda                *
*                                       1996.4  K.Koishida              *
*                                                                       *
*       usage:                                                          *
*               frame [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -l l     :  frame length                [256]           *
*               -p p     :  frame period                [100]           *
*               -n       :  no center start point       [FALSE]         *
*       infile:                                                         *
*               data sequence                                           *
*                   , x(0), x(1), ...,                                  *
*       stdout:                                                         *
*               frame sequence                                          *
*                   0, 0, ..., 0, x(0), x(1), ..., x(l/2),              *
*                   , x(t), x(t+1),       ...,       x(t+l-1),          *
*                   , x(2t), x(2t+1),     ....                          *
*              if -n specified                                          *
*                   x(0), x(1),           ...,       x(l),              *
*                   , x(t), x(t+1),       ...,       x(t+l-1),          *
*                   , x(2t), x(2t+1),     ....                          *
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

#if defined(__linux__)
#  include "SPTK.h"
#  include "frame.h"
#else
#  include "SPTK.h"
#  include "frame.h"
#endif

/*  Default Values  */
#define NOCTR TR //FA

vector sptk_frame(vector data, FRAME_SETTINGS * settings)
{
   int len = settings->leng,
       fprd = settings->shift,
       ns, i, j, rnum, ts, cs, dpos, rpos;
   Boolean noctr = NOCTR;
   double *x, *xx, *p1, *p2, *p;
   char *s, c;
   int rLen = (data.x/fprd + 1)*len;
   vector res = makev(rLen);

   x = dgetmem(len);
   dpos = rpos = 0;
   if (!noctr) {
      i = (int) ((len + 1) / 2);
//      rnum = freadf(&x[(int) (len / 2)], sizeof(*x), i, fp);
      for(j=0;j<i&&j<data.x;j++) x[((int) (len / 2)) + j ] = data.v[j];
      dpos = rnum = j;
   } else {
//      rnum = freadf(x, sizeof(*x), len, fp);
      for(j=0;j<len&&j<data.x;j++) x[j] = data.v[j];
      dpos = rnum = j;
   }
   if (rnum == 0) return res;
   cs = rnum;
//   fwritef(x, sizeof(*x), len, stdout);
   for(j=0;j<len;j++) res.v[j] = x[j];
   rpos = j;

   if ((ns = (len - fprd)) > 0) {
      p = &x[fprd];
      for (;;) {
         p1 = x;
         p2 = p;
         i = ns;
         while (i--) {
            *p1++ = *p2++;
         }
//         rnum = freadf(p1, sizeof(*p1), fprd, fp);
         for(j=0;j<fprd&&(fprd+dpos)<data.x;j++) p1[j] = data.v[j+dpos];
         dpos += j;
         rnum = j;
//         fprintf(stderr, "data %i - %i \n", dpos, data.x);
         if (rnum < fprd) {
            ts = fprd - rnum;
            cs -= ts;
            while (rnum--)
               p1++;
            while (ts--)
               *p1++ = 0.0;
         }
         if (cs <= 0)
            break;
//         fwritef(x, sizeof(*x), len, stdout);
         for(j=0;j<len&&(j+rpos)<rLen;j++) res.v[j+rpos] = x[j];
//         for(j=0;j<len;j++) res.v[j+rpos] = x[j];
         rpos += j;
//         fprintf(stderr, "res %i - %i \n", rpos, rLen);
      }
   } else {
      i = -ns;
      xx = dgetmem(i);
      for (;;) {
//         if (freadf(xx, sizeof(*xx), i, fp) != i)
         for(j=0;j<i&&(j+dpos)<data.x;j++) xx[j] = data.v[j+dpos];
         rnum = j;
         dpos += j;
         if(rnum != i)
            break;
//         rnum = freadf(x, sizeof(*x), len, fp);
         for(j=0;j<len&&(j+dpos)<data.x;j++) x[j] = data.v[j+dpos];
         rnum = j;
         dpos += j;
         if (rnum < len) {
            if (rnum == 0)
               break;
            ts = len - rnum;
            p1 = x;
            while (rnum--)
               p1++;
            while (ts--)
               *p1++ = 0.0;
         }
//         fwritef(x, sizeof(*x), len, stdout);
         for(j=0;j<len;j++) res.v[j+rpos] = x[j];
         rpos += j;
      }
   }

   return res;
}
