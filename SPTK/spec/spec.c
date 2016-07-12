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
*    spec -l 512 -n 20                                                  *
*    Transform Real Sequence to Spectrum                                *
*                                                                       *
*                                  1985.11  K.Tokuda                    *
*                                  1996. 5  K.Koishida                  *
*                                  2010. 5  A.Tamamori                  *
*                                                                       *
*       usage:                                                          *
*               spec [options] [infile] > stdout                        *
*       options:                                                        *
*               -l l     :  frame length                      [256]     *
*               -m m     :  order of MA part                  [0]       *
*               -n n     :  order of AR part                  [0]       *
*               -z z     :  MA coefficients filename          [NULL]    *
*               -p p     :  AR coefficients filename          [NULL]    *
*               -e e     :  small value for calculating log() [0]       *
*               -E E     :  floor in db calculated per frame  [N/A]     *
*               -o o     :  output format                     [0]       *
*                             0 (20 * log|H(z)|)                        *
*                             1 (ln|H(z)|)                              *
*                             2 (|H(z)|)                                *
*                             3 (|H(z)|^2)                              *
*       infile:                                                         *
*               real sequence                                           *
*                       , c(0), c(1), ..., c(l-1),                      *
*       stdout:                                                         *
*               spectrum                                                *
*                       , s(0), s(1), ..., s(L/2),                      *
*       notice:                                                         *
*               value of e must be e>=0                                 *
*               value of E must be E<0                                  *
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

#include <math.h>

#  include "SPTK.h"
#  include "spec.h"

/*  Default Values  */
#define LENG    512 //256
#define ORDERMA 0
#define ORDERAR 20
#define ETYPE   0
#define EPS     0.0
#define OTYPE   0

vector sptk_spec(vector data, SPEC_SETTINGS * settings)
{
   int leng = settings->leng,
       otype = OTYPE,
       etype = ETYPE,
       orderma = ORDERMA,
       orderar = settings->order,
       no, i,
       rnum, posR, posMA, posAR;
   vector *dataAR = NULL,
          *dataMA = NULL,
          res;
   double eps = EPS,
          eps2 = 1,
          k, *x, *y, *mag;

   if (etype == 1 && eps < 0.0) {
      fprintf(stderr, "value of e must be e >= 0!\n");
   }

   if (etype == 2 && eps2 >= 0.0) {
      fprintf(stderr, "value of E must be E < 0!\n");
   }

   rnum = posR = posMA = posAR = 0;
   no = leng / 2 + 1;

   x = dgetmem(leng + leng + no);
   y = x + leng;
   mag = y + leng;

   if(orderar > 0) dataAR = &data;
   else dataMA = &data;

   if (orderma <= 0) orderma = leng - 1;

   int lenR = no*(data.x/(dataAR != NULL ? (orderar + 1) : orderma + 1)+1);
   res = makev(lenR);

   for (;;){
      if ((dataMA == NULL) && (dataAR != NULL))
         for (i = 0; i < no; i++) mag[i] = 1.0;
      else {
         fillz(x, sizeof(*x), leng);
         for(i=0;i<(orderma + 1)&&(i+posMA)<dataMA->x;i++) x[i] = dataMA->v[i+posMA];
         rnum = i;
         posMA += i;
         if(rnum == 0) return res;

         fftr(x, y, leng);
         for (i = 0; i < no; i++)
            mag[i] = x[i] * x[i] + y[i] * y[i];
      }

      if (dataAR != NULL) {
         fillz(x, sizeof(*x), leng);
         for(i=0;i<(orderar + 1)&&(i+posAR)<dataAR->x;i++) x[i] = dataAR->v[i+posAR];
         rnum = i;
         posAR += i;
         if(rnum == 0) return res;

         k = x[0] * x[0];
         x[0] = 1.0;
         fftr(x, y, leng);
         for (i = 0; i < no; i++)
            mag[i] *= k / (x[i] * x[i] + y[i] * y[i]);
      }

      if (otype == 0 || otype == 1) {
         double max, min;
         if (etype == 1 && eps >= 0.0) {
            for (i = 0; i < no; i++) {
               mag[i] = mag[i] + eps;
            }
         } else if (etype == 2 && eps2 < 0) {
            max = mag[0];
            for (i = 1; i < no; i++) {
               if (max < mag[i])
                  max = mag[i];
            }
            max = sqrt(max);
            min = max * pow(10.0, eps2 / 20.0); /* floor is 20*log10(min/max) */
            min = min * min;
            for (i = 0; i < no; i++) {
               if (mag[i] < min)
                  mag[i] = min;
            }
         }
      }

      switch (otype) {
          case 1:
             for (i = 0; i < no; i++)
                x[i] = 0.5 * log(mag[i]);
             for(i=0;i<no;i++) res.v[i+posR] = x[i];
             posR += i;
             break;
          case 2:
             for (i = 0; i < no; i++)
                x[i] = sqrt(mag[i]);
             for(i=0;i<no;i++) res.v[i+posR] = x[i];
             posR += i;
             break;
          case 3:
             for (i = 0; i < no; i++)
                x[i] = mag[i];
             for(i=0;i<no;i++) res.v[i+posR] = x[i];
             posR += i;
             break;
          default:
             for (i = 0; i < no; i++)
                x[i] = 10 * log10(mag[i]);
             for(i=0;i<no;i++) res.v[i+posR] = x[i];
             posR += i;
             break;
          }
   }

   return res;
}
