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
    $Id: _dfs.c,v 1.15 2013/12/16 09:01:55 mataki Exp $                        
        Standard Form Digital Filter               
        latice digital filter                  
            double dfs(x, a, m, b, n, buf, bufp)         
                           
            double x             : filter input         
            double a[0]          : gain
            double a[1],...,a[m] : AR coefficints
            int  m               : order of AR part
            double b[0],...,b[n] : MA coefficents
            int  n               : order of MA part
            double buf[]         : buffer
            int *bufp            : pointer for buffer
            return               : filter output   
                1987.11  K.Tokuda               
                1995.12  N.Isshiki               
************************************************************************/

#include <stdio.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include <SPTK.h>
#endif

double dfs(double x, double *a, int m, double *b, int n, double *buf, int *bufp)
{
   double y = 0.0;
   int i, p;
   int max;

   n++;
   m++;

   (m < n) ? (max = n) : (max = m);

   x = x * a[0];
   for (i = 1; i < m; i++) {
      if ((p = *bufp + i) >= max)
         p -= max;
      x -= buf[p] * a[i];
   }
   buf[*bufp] = x;
   for (i = 0; i < n; i++) {
      if ((p = *bufp + i) >= max)
         p -= max;
      y += buf[p] * b[i];
   }

   if (--*bufp < 0)
      *bufp += max;

   return (y);
}
