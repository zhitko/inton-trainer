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

    $Id: _gmm.c,v 1.20 2013/12/16 09:01:57 mataki Exp $

    GMM output prob calculation functions

*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#  include "SPTK.h"
#else
#  include "../SPTK.h"
#endif

int choleski(double **cov, double **S, const int L);

double cal_det(double **var, const int D)
{
   int i, j, l;
   double ldet = 0.0, **tri;

   tri = (double **) malloc(sizeof(double *) * D);
   for (l = 0; l < D; l++)
      tri[l] = dgetmem(D);

   for (i = 0; i < D; i++)
      for (j = 0; j < D; j++)
         tri[i][j] = 0.0;

   if (choleski(var, tri, D)) {
      for (l = 0; l < D; l++)
         ldet += log(tri[l][l]);

      for (l = 0; l < D; l++) {
         free(tri[l]);
      }
      free(tri);

      return (2.0 * ldet);
   } else {
      for (l = 0; l < D; l++) {
         free(tri[l]);
      }
      free(tri);

      return 0;
   }
}

double cal_gconst(double *var, const int D)
{
   int d;
   double gconst;

   gconst = D * log(M_2PI);
   for (d = 0; d < D; d++)
      gconst += log(var[d]);

   return (gconst);
}

double cal_gconstf(double **var, const int D)
{
   double gconst, tmp;

   tmp = cal_det(var, D);
   if (tmp == 0) {
      fprintf(stderr, "WARNING : det is 0!\n");
      return 0;
   }
   gconst = D * log(M_2PI);
   gconst += tmp;

   return (gconst);
}

void cal_tri_inv(double **S, double **S_inv, const int L)
{
   int i, j, k;

   for (i = 0; i < L; i++) {
      S_inv[i][i] = 1.0 / S[i][i];
   }
   for (i = 1; i < L; i++)
      for (j = i - 1; j >= 0; j--)
         for (k = j; k < i; k++) {
            S_inv[i][j] = S_inv[i][j] - S[i][k] * S_inv[k][j] / S[i][i];
         }
}

int choleski(double **cov, double **S, const int L)
{
   int i, j, k;
   double tmp;

   for (i = 0; i < L; i++) {
      for (j = 0; j < i; j++) {
         tmp = cov[i][j];
         for (k = 0; k < j; k++)
            tmp -= S[i][k] * S[j][k];
         S[i][j] = tmp / S[j][j];
      }
      tmp = cov[i][i];
      for (k = 0; k < i; k++)
         tmp -= S[i][k] * S[i][k];
      if (tmp <= 0) {
         return 0;
      }
      S[i][i] = sqrt(tmp);
   }
   return 1;
}

void cal_inv(double **cov, double **inv, const int L)
{
   int i, j, k;
   double **S, **S_inv;

   S = (double **) malloc(sizeof(double *) * L);
   S_inv = (double **) malloc(sizeof(double *) * L);

   for (i = 0; i < L; i++) {
      S[i] = dgetmem(L);
      S_inv[i] = dgetmem(L);
   }

   for (i = 0; i < L; i++) {
      for (j = 0; j < L; j++) {
         S[i][j] = 0.0;
         S_inv[i][j] = 0.0;
         inv[i][j] = 0.0;
      }
   }

   if (choleski(cov, S, L) == 0)
      return;
   cal_tri_inv(S, S_inv, L);

   for (i = 0; i < L; i++)
      for (j = 0; j < L; j++) {
         if (i > j)
            for (k = i; k < L; k++)
               inv[i][j] = inv[i][j] + S_inv[k][i] * S_inv[k][j];
         else
            for (k = j; k < L; k++)
               inv[i][j] = inv[i][j] + S_inv[k][i] * S_inv[k][j];
      }

   for (i = 0; i < L; i++) {
      free(S[i]);
      free(S_inv[i]);
   }
   free(S);
   free(S_inv);
}

void fillz_GMM(GMM * gmm)
{
   int m, l, ll;

   for (m = 0; m < gmm->nmix; m++) {
      gmm->weight[m] = 0.;
      if (gmm->full != TR) {
         for (l = 0; l < gmm->dim; l++) {
            gmm->gauss[m].mean[l] = 0.0;
            gmm->gauss[m].var[l] = 0.0;
         }
      } else {
         for (l = 0; l < gmm->dim; l++) {
            gmm->gauss[m].mean[l] = 0.0;
            for (ll = 0; ll < gmm->dim; ll++) {
               gmm->gauss[m].cov[l][ll] = 0.0;
               gmm->gauss[m].inv[l][ll] = 0.0;
            }
         }
      }
   }
}

double log_wgd(const GMM * gmm, const int m, const int L, const double *dat)
{
   int l, ll;
   double sum, *diff = NULL, tmp, lwgd;

   sum = gmm->gauss[m].gconst;

   if (gmm->full != TR) {
      for (l = 0; l < L; l++) {
         tmp = dat[l] - gmm->gauss[m].mean[l];
         sum += (tmp * tmp) / gmm->gauss[m].var[l];
      }
   } else {
      diff = dgetmem(L);
      for (l = 0; l < L; l++) {
         diff[l] = dat[l] - gmm->gauss[m].mean[l];
      }
      for (l = 0; l < L; l++) {
         for (ll = 0, tmp = 0.0; ll < L; ll++) {
            tmp += diff[ll] * gmm->gauss[m].inv[ll][l];
         }
         sum += tmp * diff[l];
      }
      free(diff);
   }

   lwgd = log(gmm->weight[m]) - 0.5 * sum;
   return (lwgd);
}

double log_add(double logx, double logy)
{
   double swap, diff, minLogExp, z;

   if (logx < logy) {
      swap = logx;
      logx = logy;
      logy = swap;
   }

   diff = logy - logx;
   minLogExp = -log(-LZERO);

   if (diff < minLogExp)
      return ((logx < LSMALL) ? LZERO : logx);
   else {
      z = exp(diff);
      return (logx + log(1.0 + z));
   }
}

double log_outp(const GMM * gmm, const int L, const double *dat)
{
   int m;
   double logwgd, logb;

   for (m = 0, logb = LZERO; m < gmm->nmix; m++) {
      logwgd = log_wgd(gmm, m, L, dat);
      logb = log_add(logb, logwgd);
   }
   return (logb);
}

int alloc_GMM(GMM * gmm, const int M, const int L, const Boolean full)
{
   int m;
   gmm->nmix = M;
   gmm->dim = L;
   gmm->full = full;
   gmm->weight = dgetmem(M);
   gmm->gauss = (Gauss *) getmem(sizeof(Gauss), M);
   for (m = 0; m < M; m++) {
      gmm->gauss[m].mean = dgetmem(L);

      if (full != TR) {
         gmm->gauss[m].var = dgetmem(L);
      } else {
         gmm->gauss[m].cov = ddgetmem(L, L);
         gmm->gauss[m].inv = ddgetmem(L, L);
      }
   }

   return (0);
}

int load_GMM(GMM * gmm, FILE * fp)
{
   int m, l;

   freadf(gmm->weight, sizeof(*(gmm->weight)), gmm->nmix, fp);
   for (m = 0; m < gmm->nmix; m++) {
      freadf(gmm->gauss[m].mean, sizeof(*(gmm->gauss[m].mean)), gmm->dim, fp);

      if (gmm->full != TR) {
         freadf(gmm->gauss[m].var, sizeof(*(gmm->gauss[m].var)), gmm->dim, fp);
         gmm->gauss[m].gconst = cal_gconst(gmm->gauss[m].var, gmm->dim);
      } else {
         for (l = 0; l < gmm->dim; l++) {
            freadf(gmm->gauss[m].cov[l],
                   sizeof(*(gmm->gauss[m].cov[l])), gmm->dim, fp);
         }
         invert(gmm->gauss[m].cov, gmm->gauss[m].inv, gmm->dim);
         gmm->gauss[m].gconst = cal_gconstf(gmm->gauss[m].cov, gmm->dim);
      }
   }

   return (0);
}

int save_GMM(const GMM * gmm, FILE * fp)
{
   int m, i, j;

   fwritef(gmm->weight, sizeof(*(gmm->weight)), gmm->nmix, fp);
   for (m = 0; m < gmm->nmix; m++) {
      if (gmm->full != TR) {
         fwritef(gmm->gauss[m].mean, sizeof(*(gmm->gauss[m].mean)), gmm->dim,
                 fp);
         fwritef(gmm->gauss[m].var, sizeof(*(gmm->gauss[m].var)), gmm->dim, fp);
      } else {
         fwritef(gmm->gauss[m].mean, sizeof(*(gmm->gauss[m].mean)), gmm->dim,
                 fp);
         for (i = 0; i < gmm->dim; i++) {
            for (j = 0; j < i; j++) {
               gmm->gauss[m].cov[j][i] = gmm->gauss[m].cov[i][j];
            }
         }
         for (i = 0; i < gmm->dim; i++) {
            fwritef(gmm->gauss[m].cov[i],
                    sizeof(*(gmm->gauss[m].cov[i])), gmm->dim, fp);
         }
      }
   }

   return (0);
}

int free_GMM(GMM * gmm)
{
   int m;

   for (m = 0; m < gmm->nmix; m++) {
      free(gmm->gauss[m].mean);

      if (gmm->full != TR) {
         free(gmm->gauss[m].var);
      } else {
         free(gmm->gauss[m].cov[0]);
         free(gmm->gauss[m].inv[0]);
         free(gmm->gauss[m].cov);
         free(gmm->gauss[m].inv);
      }
   }
   free(gmm->gauss);
   free(gmm->weight);
   gmm->nmix = 0;
   gmm->dim = 0;
   gmm->full = FA;
   gmm->weight = NULL;
   gmm->gauss = NULL;

   return (0);
}
