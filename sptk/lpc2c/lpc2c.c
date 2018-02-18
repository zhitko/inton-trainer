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
*    Transform LPC to Cepstrum                                          * 
*                                                                       *
*                                      1988.7  T.Kobayashi              *
*                                      1996.1  K.Koishida               *
*                                                                       *
*       usage:                                                          *
*               lpc2c [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -m m     :  order of LPC              [25]              *
*               -M M     :  order of cepstrum         [25]              *
*       infile:                                                         *
*               LP Coefficients                                         *
*                       , K, a(1), ..., a(M),                           *
*       stdout:                                                         *
*               cepstral coefficients                                   *
*                       , c(0), c(1), ..., c(N),                        *
*       require:                                                        *
*               lpc2c()                                                 *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: lpc2c.c,v 1.23 2013/12/16 09:01:59 mataki Exp $";


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
#else
#  include <SPTK.h>
#endif

#  include "lpc2c.h"

/*  Default Values  */
#define ORDERC  25
#define ORDERA  25

vector sptk_lpc2c(vector data, LPC_SETTINGS * settings)
{
   int m = ORDERA, n = ORDERC, i, dPos, rPos;

   double *c, *a;

   m = settings->order;
   n = settings->cepstrum_order;

   a = dgetmem(m + n + 2);
   c = a + m + 1;

   dPos = rPos = 0;

   int rLen = data.x/(m + 1)*(n + 1);
   vector res = zerov(rLen);

   //while (freadf(a, sizeof(*a), m + 1, fp) == m + 1) {
   while(1) {

      for(i=0;i<(m + 1)&&(i+dPos)<data.x;i++) a[i] = getv(data, i+dPos);
      if(i != (m + 1)) break;
      dPos += i;

      lpc2c(a, m, c, n);
//      fwritef(c, sizeof(*c), n + 1, stdout);

      for(i=0;i<(n + 1);i++)
          setv(res, i+rPos, c[i]);
      rPos += i;
   }

   return res;
}
