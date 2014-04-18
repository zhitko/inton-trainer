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

/*************************************************************************
*                                                                        *
*    Data Type Transformation                                            *
*                                                                        *
*                                 1985.12 K.Tokuda                       *
*                                 1996.5  K.Koishida                     *
*                                 2010.3  A.Tamamori                     *
*                                 2010.10 T.Sawada                       *
*                                                                        *
*       usage:                                                           *
*               x2x [options] [infile] > stdout                          *
*       options:                                                         *
*               +type1   :  input data type                   [f]        *
*               +type2   :  output data type                  [type1]    *
*                           c (char)           C  (unsigned char)        *
*                           s (short)          S  (unsigned short)       *
*                           i (int)            I  (unsigned int)         *
*                           i3 (int, 3byte)    I3 (unsigned int, 3byte)  *
*                           l (long)           L  (unsigned long)        *
*                           le (long long)     LE (unsigned long long)   *
*                           f (float)          d  (double)               *
*                           de (long double)   a  (ascii)                *
*                           aN (ascii w/ column number N;N=1 by default) *
*               -r       :  specify rounding off when a real number      *
*                           is substituted for a integer      [FALSE]    *
*               -o       :  clip by minimum and maximum of               *
*                           output data type if input data is over       *
*                           the range of output data type     [FALSE]    *
*               %format  :  specify output format similar to  [%Lg]      *
*                           "printf()".                                  *
*                           if type2 is ascii.                           *
*                                                                        *
*************************************************************************/

/*  Standard C Libraries  */
#include <stdio.h>
#include <sys/stat.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#  ifndef HAVE_STRRCHR
#     define strrchr rindex
#  endif
#endif

#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#  include "SPTK.h"
#  include "x2x.h"

#include <limits.h>
#include <float.h>
/*  Default Values  */
#define ROUND      FA
#define CLIP       FA
#define COL        1
#define FORM_LONG   "%d"
#define FORM_ULONG  "%u"
#define FORM_LLONG  "%lld"
#define FORM_ULLONG "%llu"
#define FORM_FLOAT  "%g"
#define FORM_LDBL   "%Lg"
#define INT3_MAX   8388607
#define INT3_MIN   (-8388608)
#define UINT3_MAX  16777215
#ifndef LLONG_MAX
#  define LLONG_MAX __LONG_LONG_MAX__
#  define LLONG_MIN (-__LONG_LONG_MAX__-1)
#  define ULLONG_MAX (__LONG_LONG_MAX__*2ULL+1)
#endif

double r = 0.0;

vector sptk_x2x(const char *fname)
{
    char c1 = 's',
         c2 = 'd';
    long double x1;
    double x2;
    int i;
    size_t size1 = sizeof(short),
           size2 = sizeof(double);
    struct stat sb;
    stat(fname, &sb);
    int len = sb.st_size / size1;
    FILE *fp = getfp(fname, "rb");
    Boolean clip = TR; //CLIP;

    vector res;

    void x2x(void *x1, void *x2, char c1, char c2, int clip);

    res = makev(len);

    i = 0;
    while (freadx(&x1, size1, 1, fp) == 1) {
       x2x(&x1, &x2, c1, c2, clip);
       res.v[i] = x2;
//       printf(" %d %d \n", x1, x2);
       i++;
    }

    return res;
}

vector sptk_v2v(void *data, int byteSize, short int bitDepth)
{
    char c1 = 's',
         c2 = 'd';
    int i;
    int len = byteSize / (bitDepth/CHAR_BIT);
    long double x1;
    double x2;
    Boolean clip = TR; //CLIP;
    signed short int *vals = data;
    vector res;

    void x2x(void *x1, void *x2, char c1, char c2, int clip);

    res = makev(len);

    i = 0;
    while (len > i) {
       x2x(vals, &x2, c1, c2, clip);
       res.v[i] = x2;
//       printf(" %d %d \n", x1, x2);
       i++;
       vals++;
    }

    return res;
}

void x2x(void *x1, void *x2, char c1, char c2, int clip)
{
   long long xl = 0;
   unsigned long long xul = 0;
   long double xd = 0.0;
   int y = 0;
   switch (c1) {
   case 's':
      xl = *(short *) x1;
      break;
   case 'i':
      xl = *(int *) x1;
      break;
   case 'l':
      xl = *(long *) x1;
      break;
   case 'u':
      xl = *(long long *) x1;
      break;
   case 'S':
      xul = *(unsigned short *) x1;
      break;
   case 'I':
      xul = *(unsigned int *) x1;
      break;
   case 'L':
      xul = *(unsigned long *) x1;
      break;
   case 'U':
      xul = *(unsigned long long *) x1;
      break;
   case 'f':
      xd = *(float *) x1;
      break;
   case 'd':
      xd = *(double *) x1;
      break;
   case 'v':
      xd = *(long double *) x1;
      break;
   case 'c':
      xl = *(char *) x1;
      break;
   case 'C':
      xul = *(unsigned char *) x1;
      break;
   case 't':                   /* 3byte, signed */
      y = *(int *) x1 & 0x00FFFFFF;
      if (y >> 23 == 1)
         y = y | 0xFF000000;
      xl = y;
      break;
   case 'T':                   /* 3byte, unsigned */
      xul = *(unsigned int *) x1 & 0x00FFFFFF;
      break;
   }

   if (clip) {
      switch (c2) {
      case 's':
         if (xl > SHRT_MAX || xl < SHRT_MIN || xul > SHRT_MAX || xd > SHRT_MAX || xd < SHRT_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'short'!\n");
         xl = ((xl < SHRT_MIN) ? SHRT_MIN : ((xl > SHRT_MAX) ? SHRT_MAX : xl));
         xul = ((xul > SHRT_MAX) ? SHRT_MAX : xul);
         xd = ((xd < SHRT_MIN) ? SHRT_MIN : ((xd > SHRT_MAX) ? SHRT_MAX : xd));
         break;
      case 'i':
         if (xl > INT_MAX || xl < INT_MIN || xul > INT_MAX || xd > INT_MAX || xd < INT_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'int'!\n");
         xl = ((xl < INT_MIN) ? INT_MIN : ((xl > INT_MAX) ? INT_MAX : xl));
         xul = ((xul > INT_MAX) ? INT_MAX : xul);
         xd = ((xd < INT_MIN) ? INT_MIN : ((xd > INT_MAX) ? INT_MAX : xd));
         break;
      case 'l':
         if (xl > LONG_MAX || xl < LONG_MIN || xul > LONG_MAX || xd > LONG_MAX || xd < LONG_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'long'!\n");
         xl = ((xl < LONG_MIN) ? LONG_MIN : ((xl > LONG_MAX) ? LONG_MAX : xl));
         xul = ((xul > LONG_MAX) ? LONG_MAX : xul);
         xd = ((xd < LONG_MIN) ? LONG_MIN : ((xd > LONG_MAX) ? LONG_MAX : xd));
         break;
      case 'u':
         if (xul > LLONG_MAX || xd > LLONG_MAX || xd < LLONG_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'long long'!\n");
         xul = ((xul > LLONG_MAX) ? LLONG_MAX : xul);
         xd = ((xd <
                LLONG_MIN) ? LLONG_MIN : ((xd > LLONG_MAX) ? LLONG_MAX : xd));
         break;
      case 'S':
         if (xl > USHRT_MAX || xl < 0 || xul > USHRT_MAX || xd > USHRT_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned short'!\n");
         xl = ((xl < 0) ? 0 : ((xl > USHRT_MAX) ? USHRT_MAX : xl));
         xul = ((xul > USHRT_MAX) ? USHRT_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > USHRT_MAX) ? USHRT_MAX : xd));
         break;
      case 'I':
         if (xl > UINT_MAX || xl < 0 || xul > UINT_MAX || xd > UINT_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned int'!\n");
         xl = ((xl < 0) ? 0 : ((xl > UINT_MAX) ? UINT_MAX : xl));
         xul = ((xul > UINT_MAX) ? UINT_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > UINT_MAX) ? UINT_MAX : xd));
         break;
      case 'L':
         if (xl > ULONG_MAX || xl < 0 || xul > ULONG_MAX || xd > ULONG_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned long'!\n");
         xl = ((xl < 0) ? 0 : ((xl > ULONG_MAX) ? ULONG_MAX : xl));
         xul = ((xul > ULONG_MAX) ? ULONG_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > ULONG_MAX) ? ULONG_MAX : xd));
         break;
      case 'U':
         if (xl < 0 || xd > ULLONG_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned long long'!\n");
         xl = ((xl < 0) ? 0 : xl);
         xd = ((xd < 0.0) ? 0.0 : ((xd > ULLONG_MAX) ? ULLONG_MAX : xd));
         break;
      case 'f':
         if (xd > FLT_MAX || xd < (FLT_MAX * (-1)))
            fprintf(stderr, "warning: input data is over the range of type 'float'!\n");
         xd = ((xd <
                (FLT_MAX * (-1))) ? (FLT_MAX * (-1)) : ((xd >
                                                         FLT_MAX) ? FLT_MAX :
                                                        xd));
         break;
      case 'd':
         if (xd > DBL_MAX || xd < (DBL_MAX * (-1)))
            fprintf(stderr, "warning: input data is over the range of type 'double'!\n");
         xd = ((xd <
                (DBL_MAX * (-1))) ? (DBL_MAX * (-1)) : ((xd >
                                                         DBL_MAX) ? DBL_MAX :
                                                        xd));
         break;
      case 'v':
         if (xd > LDBL_MAX || xd < (LDBL_MAX * (-1)))
            fprintf(stderr, "warning: input data is over the range of type 'long double'!\n");
         break;
      case 'c':
         if (xl > CHAR_MAX || xl < CHAR_MIN || xul > CHAR_MAX || xd > CHAR_MAX
             || xd < CHAR_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'char'!\n");
         xl = ((xl < CHAR_MIN) ? CHAR_MIN : ((xl > CHAR_MAX) ? CHAR_MAX : xl));
         xul = ((xul > CHAR_MAX) ? CHAR_MAX : xul);
         xd = ((xd < CHAR_MIN) ? CHAR_MIN : ((xd > CHAR_MAX) ? CHAR_MAX : xd));
         break;
      case 'C':
         if (xl > UCHAR_MAX || xl < 0.0 || xul > UCHAR_MAX || xd > UCHAR_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned char'!\n");
         xl = ((xl < 0) ? 0 : ((xl > UCHAR_MAX) ? UCHAR_MAX : xl));
         xul = ((xul > UCHAR_MAX) ? UCHAR_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > UCHAR_MAX) ? UCHAR_MAX : xd));
         break;
      case 't':
         if (xl > INT3_MAX || xl < INT3_MIN || xul > INT3_MAX || xd > INT3_MAX || xd < INT3_MIN)
            fprintf(stderr, "warning: input data is over the range of type 'int(3byte)'!\n");
         xl = ((xl < INT3_MIN) ? INT3_MIN : ((xl > INT3_MAX) ? INT3_MAX : xl));
         xul = ((xul > INT3_MAX) ? INT3_MAX : xul);
         xd = ((xd < INT3_MIN) ? INT3_MIN : ((xd > INT3_MAX) ? INT3_MAX : xd));
         break;
      case 'T':
         if (xl > UINT3_MAX || xl < 0 || xul > UINT3_MAX || xd > UINT3_MAX || xd < 0.0)
            fprintf(stderr, "warning: input data is over the range of type 'unsigned int(3byte)'!\n");
         xl = ((xl < 0) ? 0 : ((xl > UINT3_MAX) ? UINT3_MAX : xl));
         xul = ((xul > UINT3_MAX) ? UINT3_MAX : xul);
         xd = ((xd < 0.0) ? 0.0 : ((xd > UINT3_MAX) ? UINT3_MAX : xd));
         break;
      }
   } else {
      switch (c2) {
      case 's':
         if (xl > SHRT_MAX || xl < SHRT_MIN || xul > SHRT_MAX || xd > SHRT_MAX || xd < SHRT_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'short'!\n");
            return;
         }
         break;
      case 'i':
         if (xl > INT_MAX || xl < INT_MIN || xul > INT_MAX || xd > INT_MAX || xd < INT_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'int'!\n");
            return;
         }
         break;
      case 'l':
         if (xl > LONG_MAX || xl < LONG_MIN || xul > LONG_MAX || xd > LONG_MAX || xd < LONG_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'long'!\n");
            return;
         }
         break;
      case 'u':
         if (xul > LLONG_MAX || xd > LLONG_MAX || xd < LLONG_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'long long'!\n");
            return;
         }
         break;
      case 'S':
         if (xl > USHRT_MAX || xl < 0 || xul > USHRT_MAX || xd > USHRT_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned short'!\n");
            return;
         }
         break;
      case 'I':
         if (xl > UINT_MAX || xl < 0 || xul > UINT_MAX || xd > UINT_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned int'!\n");
            return;
         }
         break;
      case 'L':
         if (xl > ULONG_MAX || xl < 0 || xul > ULONG_MAX || xd > ULONG_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned long'!\n");
            return;
         }
         break;
      case 'U':
         if (xl < 0 || xd > ULLONG_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned long long'!\n");
            return;
         }
         break;
      case 'f':
         if (xd > FLT_MAX || xd < (FLT_MAX * (-1))) {
            fprintf(stderr, "error: input data is over the range of type 'float'!\n");
            return;
         }
         break;
      case 'd':
         if (xd > DBL_MAX || xd < (DBL_MAX * (-1))) {
            fprintf(stderr, "error: input data is over the range of type 'double'!\n");
            return;
         }
         break;
      case 'v':
         if (xd > LDBL_MAX || xd < (LDBL_MAX * (-1))) {
            fprintf(stderr, "error: input data is over the range of type 'long double'!\n");
            return;
         }
         break;
      case 'c':
         if (xl > CHAR_MAX || xl < CHAR_MIN || xul > CHAR_MAX || xd > CHAR_MAX || xd < CHAR_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'char'!\n");
            return;
         }
         break;
      case 'C':
         if (xl > UCHAR_MAX || xl < 0.0 || xul > UCHAR_MAX || xd > UCHAR_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned char'!\n");
            return;
         }
         break;
      case 't':
         if (xl > INT3_MAX || xl < INT3_MIN || xul > INT3_MAX || xd > INT3_MAX || xd < INT3_MIN) {
            fprintf(stderr, "error: input data is over the range of type 'int(3byte)'!\n");
            return;
         }
         break;
      case 'T':
         if (xl > UINT3_MAX || xl < 0 || xul > UINT3_MAX || xd > UINT3_MAX || xd < 0.0) {
            fprintf(stderr, "error: input data is over the range of type 'unsigned int(3byte)'!\n");
            return;
         }
         break;
      }
   }

   switch (c2) {
   case 's':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(short *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(short *) x2 = xul;
      else {
         if (xd > 0)
            *(short *) x2 = xd + r;
         else
            *(short *) x2 = xd - r;
      }
      break;
   case 'i':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(int *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(int *) x2 = xul;
      else {
         if (xd > 0)
            *(int *) x2 = xd + r;
         else
            *(int *) x2 = xd - r;
      }
      break;
   case 'l':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(long *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(long *) x2 = xul;
      else {
         if (xd > 0)
            *(long *) x2 = xd + r;
         else
            *(long *) x2 = xd - r;
      }
      break;
   case 'u':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(long long *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(long long *) x2 = xul;
      else {
         if (xd > 0)
            *(long long *) x2 = xd + r;
         else
            *(long long *) x2 = xd - r;
      }
      break;
   case 'S':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned short *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned short *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned short *) x2 = xd + r;
         else
            *(unsigned short *) x2 = xd - r;
      }
      break;
   case 'I':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned int *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned int *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned int *) x2 = xd + r;
         else
            *(unsigned int *) x2 = xd - r;
      }
      break;
   case 'L':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned long *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned long *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned long *) x2 = xd + r;
         else
            *(unsigned long *) x2 = xd - r;
      }
      break;
   case 'U':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned long long *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned long long *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned long long *) x2 = xd + r;
         else
            *(unsigned long long *) x2 = xd - r;
      }
      break;
   case 'f':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(float *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(float *) x2 = xul;
      else {
         if (xd > 0)
            *(float *) x2 = xd;
         else
            *(float *) x2 = xd;
      }
      break;
   case 'd':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(double *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(double *) x2 = xul;
      else {
         if (xd > 0)
            *(double *) x2 = xd;
         else
            *(double *) x2 = xd;
      }
      break;
   case 'v':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(long double *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(long double *) x2 = xul;
      else {
         if (xd > 0)
            *(long double *) x2 = xd;
         else
            *(long double *) x2 = xd;
      }
      break;
   case 'c':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(char *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(char *) x2 = xul;
      else {
         if (xd > 0)
            *(char *) x2 = xd + r;
         else
            *(char *) x2 = xd - r;
      }
      break;
   case 'C':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned char *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned char *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned char *) x2 = xd + r;
         else
            *(unsigned char *) x2 = xd - r;
      }
      break;
   case 't':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(int *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(int *) x2 = xul;
      else {
         if (xd > 0)
            *(int *) x2 = xd + r;
         else
            *(int *) x2 = xd - r;
      }
      break;
   case 'T':
      if (c1 == 's' || c1 == 'i' || c1 == 'l' || c1 == 'u' || c1 == 'c'
          || c1 == 't')
         *(unsigned int *) x2 = xl;
      else if (c1 == 'S' || c1 == 'I' || c1 == 'L' || c1 == 'U' || c1 == 'C'
               || c1 == 'T')
         *(unsigned int *) x2 = xul;
      else {
         if (xd > 0)
            *(unsigned int *) x2 = xd + r;
         else
            *(unsigned int *) x2 = xd - r;
      }
      break;
   }
   return;
}
