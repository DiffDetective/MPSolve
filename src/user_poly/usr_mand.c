/***********************************************************
**       Multiprecision Polynomial Solver (MPSolve)       **
**              Version 2.1, september 1999               **
**                                                        **
**                      Written by                        **
**       Dario Andrea Bini and Giuseppe Fiorentino        **
**       (bini@dm.unipi.it)  (fiorent@dm.unipi.it)        **
**                                                        **
** (C) 1999, Dipartimento di Matematica, FRISCO LTR 21024 **
***********************************************************/

/********************************************************
This file contains a sample of a user-defined polynomial.
In this sample the (Mandelbrot) polynomial is defined by
the relation 
    p_{i+1}(x)=x*p_i(x)^2+1 where p_0(x)=1,        (1)
and its derivative by 
    p'_{i+1}(x)=p_i(x)^2+x*2*p_i(x)*p'_i(x).
The degree is 1, 3, 7, 15, 31,... (2^i-1).
The three programs below compute the values of p_i(x) and
p'_i(x), by means of the above formulae in the float version,
dpe version and mp version. 
The programs compute also the auxiliary variable apeps needed
for testing the stop condition apeps>|p|,  and for computing
the error bound rad (radius of the inclusion disk).
The formulae for the computation of apeps and rad are
obtained by means of a rounding error analysis of (1).
**********************************************************/

#include <mps.h/core.h>

/******************************************************
*              SUBROUTINE FNEWTON_USR                 *   
******************************************************* 
* user-defined program for the computation of p, p',  *
* and ap corr = p/p1, rad = n*(ABS(p)+ap)/ABS(p'),    *
* again = ABS(p)>ap.                                  *
* This sample computes the 'Mandelbrot polynomial by  *
* means of the relation: p=1+x*p**2, starting with p=1*
******************************************************/
void
fnewton_usr (cplx_t x, double *rad, cplx_t corr, mps_boolean * again)
{
  cplx_t p, pp, pt, tmp;
  double ap, ax, eps;
  int i, m;

  m = (int) (log (n + 1.0) / LOG2);
  if ((1 << m) <= n)
    m++;
  eps = (DBL_EPSILON * 4.0) * n;
  ax = cplx_mod (x);

  cplx_set (p, cplx_one);
  cplx_set (pp, cplx_zero);
  ap = 1.0;
  for (i = 1; i <= m; i++)
    {
      cplx_sqr (tmp, p);
      cplx_mul (pt, x, tmp);
      cplx_add_eq (pt, cplx_one);
      cplx_mul_eq (pp, x);
      cplx_mul_eq (pp, p);
      cplx_mul_eq_d (pp, 2.0);
      cplx_add_eq (pp, tmp);
      cplx_set (p, pt);
      ap = ap * ax + cplx_mod (p);
    }
  ap = ap * ax;
  cplx_div (corr, p, pp);

  *again = cplx_mod (p) > eps * ap * 3;

  *rad = n * (cplx_mod (p) + 3 * ap * eps) / cplx_mod (pp);
}

/******************************************************
*              SUBROUTINE DNEWTON_USR                 *   
******************************************************* 
 DPE computation
******************************************************/
void
dnewton_usr (cdpe_t x, rdpe_t rad, cdpe_t corr, mps_boolean * again)
{
  cdpe_t p, pp, pt, tmp;
  rdpe_t ap, ax, eps, temp, apeps, atmp;
  int i, m;

  m = (int) (log (n + 1.0) / LOG2);
  if ((1 << m) <= n)
    m++;
  rdpe_set_d (eps, DBL_EPSILON);
  rdpe_mul_eq_d (eps, (double) 4 * n);
  cdpe_mod (ax, x);

  cdpe_set (p, cdpe_one);
  cdpe_set (pp, cdpe_zero);
  rdpe_set (ap, rdpe_one);
  for (i = 1; i <= m; i++)
    {
      cdpe_sqr (tmp, p);
      cdpe_mul (pt, x, tmp);
      cdpe_add_eq (pt, cdpe_one);
      cdpe_mul_eq (pp, x);
      cdpe_mul_eq (pp, p);
      cdpe_mul_eq_d (pp, 2.0);
      cdpe_add_eq (pp, tmp);
      cdpe_set (p, pt);
      rdpe_mul_eq (ap, ax);
      cdpe_mod (atmp, p);
      rdpe_add_eq (ap, atmp);
    }
  rdpe_mul_eq (ap, ax);
  cdpe_div (corr, p, pp);

  cdpe_mod (temp, p);
  rdpe_mul (apeps, ap, eps);
  rdpe_mul_eq_d (apeps, 3.0);
  *again = rdpe_gt (temp, apeps);

  rdpe_add (rad, temp, apeps);
  rdpe_mul_eq_d (rad, (double) n);
  cdpe_mod (temp, pp);
  rdpe_div_eq (rad, temp);
  if (rdpe_eq (rad, rdpe_zero))
    rdpe_mul (rad, ax, eps);
}

/******************************************************
*              SUBROUTINE MNEWTON_USR                 *   
******************************************************* 
 multiprecision computation
******************************************************/
void
mnewton_usr (mpcf_t x, rdpe_t rad, mpcf_t corr, mps_boolean * again)
{
  int i, m;
  rdpe_t ap, ax, eps, temp, apeps, atmp;
  cdpe_t ctmp;
  tmpcf_t p, pp, pt, tmp;

  tmpcf_init2 (p, mpwp);
  tmpcf_init2 (pp, mpwp);
  tmpcf_init2 (pt, mpwp);
  tmpcf_init2 (tmp, mpwp);

  m = (int) (log (n + 1.0) / LOG2);
  if ((1 << m) <= n)
    m++;
  rdpe_set (eps, mp_epsilon);
  rdpe_mul_eq_d (eps, (double) 4 * n);
  mpcf_get_cdpe (ctmp, x);
  cdpe_mod (ax, ctmp);

  mpcf_set_ui (p, 1, 0);
  mpcf_set_ui (pp, 0, 0);
  rdpe_set (ap, rdpe_one);
  for (i = 1; i <= m; i++)
    {
      mpcf_sqr (tmp, p);
      mpcf_mul (pt, x, tmp);
      mpcf_add_eq_ui (pt, 1, 0);
      mpcf_mul_eq (pp, x);
      mpcf_mul_eq (pp, p);
      mpcf_mul_eq_ui (pp, 2);
      mpcf_add_eq (pp, tmp);
      mpcf_set (p, pt);
      rdpe_mul_eq (ap, ax);
      mpcf_get_cdpe (ctmp, p);
      cdpe_mod (atmp, ctmp);
      rdpe_add_eq (ap, atmp);
    }
  rdpe_mul_eq (ap, ax);
  mpcf_div (corr, p, pp);

  mpcf_get_cdpe (ctmp, p);
  cdpe_mod (temp, ctmp);
  rdpe_mul (apeps, ap, eps);
  rdpe_mul_eq_d (apeps, 3.0);
  *again = rdpe_gt (temp, apeps);

  rdpe_add (rad, temp, apeps);
  rdpe_mul_eq_d (rad, (double) n);
  mpcf_get_cdpe (ctmp, pp);
  cdpe_mod (temp, ctmp);
  rdpe_div_eq (rad, temp);
  if (rdpe_eq (rad, rdpe_zero))
    rdpe_mul (rad, ax, eps);

  tmpcf_clear (tmp);
  tmpcf_clear (pt);
  tmpcf_clear (pp);
  tmpcf_clear (p);
}
