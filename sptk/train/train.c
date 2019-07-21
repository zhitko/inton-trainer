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
*    Generate Pulse Sequence                                            *
*                                                                       *
*                                            1989.6  K.Tokuda           *
*                                            1996.4  K.Koishida         *
*                                            2010.4  T.Sawada           *
*                                                                       *
*       usage:                                                          *
*               train [options] > stdout                                *
*       options:                                                        *
*               -l l     :  sequence length             [256]           *
*               -p p     :  frame period                [0.0]           *
*               -n n     :  type of normalization       [1]             *
*                             0 none                                    *
*                             1 normalize by power                      *
*                             2 normalize by magnitude                  *
*       stdout:                                                         *
*               p >= 1.0 pulse sequence with pitch p                    *
*               p =  0.0 M-sequence                                     *
*                                                                       *
************************************************************************/

static char *rcs_id = "$Id: train.c,v 1.24 2013/12/16 09:02:04 mataki Exp $";


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

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include "../SPTK.h"
#endif

/*  Default Values  */
#define LENG   256
#define PERIOD   0.0
#define NORMAL  1

/*  Command Name  */
char *cmnd;


void usage(int status)
{
   fprintf(stderr, "\n");
   fprintf(stderr, " %s - generate pulse sequence\n", cmnd);
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       %s [ options ] > stdout\n", cmnd);
   fprintf(stderr, "  options:\n");
   fprintf(stderr, "       -l l  : sequence length       [%d]\n", LENG);
   fprintf(stderr, "       -p p  : frame period          [%.1lf]\n", PERIOD);
   fprintf(stderr, "       -n n  : type of normalization [%d]\n", NORMAL);
   fprintf(stderr, "                 0 none\n");
   fprintf(stderr, "                 1 normalize by power\n");
   fprintf(stderr, "                 2 normalize by magnitude\n");
   fprintf(stderr, "       -h    : print this message\n");
   fprintf(stderr, "  stdout:\n");
   fprintf(stderr, "       pulse sequence (%s)\n", FORMAT);
#ifdef PACKAGE_VERSION
   fprintf(stderr, "\n");
   fprintf(stderr, " SPTK: version %s\n", PACKAGE_VERSION);
   fprintf(stderr, " CVS Info: %s", rcs_id);
#endif
   fprintf(stderr, "\n");
   exit(status);
}


int main(int argc, char **argv)
{
   int l = LENG, normal = NORMAL;
   double zero, pulse, period = PERIOD, i;

   if ((cmnd = strrchr(argv[0], '/')) == NULL)
      cmnd = argv[0];
   else
      cmnd++;
   while (--argc)
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 'l':
            l = atoi(*++argv);
            --argc;
            break;
         case 'p':
            period = atof(*++argv);
            --argc;
            break;
         case 'n':
            normal = atoi(*++argv);
            --argc;
            break;
         case 'h':
            usage(0);
         default:
            fprintf(stderr, "%s : Invalid option '%c'!\n", cmnd, *(*argv + 1));
            usage(1);
         }
      }

   switch (normal) {
   case 0:
      pulse = 1;
      break;
   case 1:
      pulse = sqrt(period);
      break;
   case 2:
      pulse = period;
      break;
   }
   zero = 0.0;
   i = 0.5;

   if (period >= 1.0)
      while (l--)
         if (--i >= 0.0)
            fwritef(&zero, sizeof(zero), 1, stdout);
         else {
            i += period;
            fwritef(&pulse, sizeof(pulse), 1, stdout);
   } else if (period != 0.0) {
      fprintf(stderr, "-p : frame period must be 0.0 or more than 1.0!\n");
      usage(1);
   } else
      while (l--) {
         pulse = mseq();
         fwritef(&pulse, sizeof(pulse), 1, stdout);
      }

   return (0);
}
