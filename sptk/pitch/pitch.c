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
*    Pitch Extraction                                                   *
*                                                                       *
*                                      1998.7  M.Tamura                 *
*                                      2000.3  T.Tanaka                 *
*                                      2011.10 A.Tamamori               *
*                                      2011.11 T.Sawada                 *
*                                                                       *
*       usage:                                                          *
*               pitch [ options ] [ infile ] > stdout                   *
*       options:                                                        *
*               -a  a     :  algorithm used for pitch      [0]          *
*                            estimation                                 *
*                              0 (RAPT)                                 *
*                              1 (SWIPE')                               *
*               -s  s     :  sampling frequency (Hz)       [16]         *
*               -p  p     :  frame shift                   [80]         *
*               -T  T     :  voiced/unvoiced threshold     [0.0]        *
*                            (used only for RAPT algorithm)             *
*               -t  t     :  voiced/unvoiced threshold     [0.3]        *
*                            (used only for SWIPE' algorithm)           *
*               -L  L     :  minimum fundamental frequency [60]         *
*                            to search for (Hz)                         *
*               -H  H     :  maximum fundamental frequency [240]        *
*                            to search for (Hz)                         *
*               -o  o     :  output format                 [0]          *
*                              0 (pitch)                                *
*                              1 (f0)                                   *
*                              2 (log(f0))                              *
*       infile:                                                         *
*               data sequence                                           *
*                       x(0), x(1), ..., x(n-1), ...                    *
*       stdout:                                                         *
*               pitch, f0, or log(f0)                                   *
*                                                                       *
************************************************************************/

/*  Standard C Libraries  */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#  include "SPTK.h"
#  include "pitch.h"

typedef struct _float_list {
   float f;
   struct _float_list *next;
} float_list;

vector sptk_pitch(vector data, PITCH_SETTINGS * settings)
{
    return sptk_pitch_spec(data, settings, -1);
}

vector sptk_pitch_spec(vector data, PITCH_SETTINGS * settings, int count)
{
   if(!settings) settings = initPitchSettings();
   int length, i,
       frame_shift = settings->FRAME_SHIFT,
       atype = settings->ATYPE,
       otype = settings->OTYPE;
   double *x,
       thresh_rapt = settings->THRESH_RAPT,
       thresh_swipe = settings->THRESH_SWIPE,
       sample_freq = settings->SAMPLE_FREQ * 1000.0,
       L = settings->MIN_FREQ,
       H = settings->MAX_FREQ;
   float_list *top, *cur, *prev;

   vector rapt(float_list * flist, int length, double sample_freq,
             int frame_shift, double min, double max, double threshold,
             int otype);
   vector swipe(float_list * input, int length, double sample_freq,
              int frame_shift, double min, double max, double threshold,
              int otype);

   x = dgetmem(1);
   top = prev = (float_list *) malloc(sizeof(float_list));
   length = 0;
   prev->next = NULL;
   for(i=0;i<data.x;i++){
       cur = (float_list *) malloc(sizeof(float_list));
       cur->f = (float) getv(data, i);
       length++;
       prev->next = cur;
       cur->next = NULL;
       prev = cur;
   }

   if(count != -1)
      frame_shift = (int) (ceil((double) length / (double) count));
   if (atype == 0) {
      return(rapt(top->next, length, sample_freq, frame_shift, L, H, thresh_rapt, otype));
   } else {
      return(swipe(top->next, length, sample_freq, frame_shift, L, H, thresh_swipe, otype));
   }
}
