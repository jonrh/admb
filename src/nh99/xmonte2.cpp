/*
 * $Id$
 *
 * Author: David Fournier
 * Copyright (c) 2008-2012 Regents of the University of California
 */
#include <admodel.h>

double inv_cumd_norm(const double& x);
double cumd_norm(const double& x);
double myran1(long int&);
//double better_rand(long int&);

void bounded_multivariate_normal_mcmc(int nvar, const dvector& a1,
  const dvector& b1, dmatrix& ch, const double& _wght, const dvector& y,
  const random_number_generator& rng)
{
  double & wght=(double &) _wght;
  //cout << y << endl;
  const double sqrt_tpi = sqrt(2*PI);
  dvector a(1,nvar);
  dvector b(1,nvar);
  dvector alpha(1,nvar);
  dvector beta(1,nvar);
  a=a1;
  b=b1;
  wght=0;
  int expflag;
  int in=0;
  int ie=0;
  double* pyi = y.get_v() + 1;
  double* pai = a.get_v() + 1;
  double* pbi = b.get_v() + 1;
  dvector* pchi = &ch(1);
  for (int i=1;i<=nvar;++i)
  {
    double chii = *(pchi->get_v() + i);
    double ah = *pai / chii;
    double bl = *pbi / chii;
    double upper=cumd_norm(bl);
    double lower=cumd_norm(ah);
    double diff=upper-lower;
    if (diff>1.e-5)
    {
      wght-=log(diff);
      expflag=0;
    }
    else
    {
      upper=cumd_cauchy(bl);
      lower=cumd_cauchy(ah);
      diff=upper-lower;
      wght-=log(diff);
      expflag=1;
    }

    if (!expflag)
    {
      wght -= 0.5 * *pyi * *pyi;
      ++in;
    }
    else
    {
      ++ie;
      wght += log_density_cauchy(*pyi);
    }

    double* paj = a.get_v() + i;
    double* pbj = b.get_v() + i;
    dvector* pchj = &ch(i);
    for (int j=i;j<=nvar;++j)
    {
      double tmp = *pyi * *(pchj->get_v() + i);
      *paj -= tmp;
      *pbj -= tmp;

      ++pchj;
    }
    ++pyi;
    ++pai;
    ++pbi;
    ++pchi;
  }
  wght +=  in*log(1./sqrt_tpi);
}

void probing_bounded_multivariate_normal_mcmc(int nvar, const dvector& a1,
  const dvector& b1, dmatrix& ch, const double& _wght, const dvector& y,
  double pprobe, const random_number_generator& rng)
{
  double & wght=(double &) _wght;
  //cout << y << endl;
  const double sqrt_tpi =sqrt(2*PI);
  dvector a(1,nvar);
  dvector b(1,nvar);
  dvector alpha(1,nvar);
  dvector beta(1,nvar);
  a=a1;
  b=b1;
  wght=0;
  double ah;
  double bl;
  double upper;
  double lower;
  double diff;
  double diff1;
  //int in=0;
  //int ie=0;
  for (int i=1;i<=nvar;i++)
  {
    ah=a(i)/ch(i,i);
    bl=b(i)/ch(i,i);
    upper=cumd_norm(bl);
    lower=cumd_norm(ah);
    diff=upper-lower;
    upper=cumd_cauchy(bl);
    lower=cumd_cauchy(ah);
    diff1=upper-lower;
    if (diff>1.e-5)
    {
      wght+=log((1.0-pprobe)*exp(-.5*y(i)*y(i))/(sqrt_tpi*diff)
         +pprobe*density_cauchy(y(i))/diff1);
    }
    else
    {
      wght += log_density_cauchy(y(i))-log(diff1);
    }

    for (int j=i;j<=nvar;j++)
    {
      double tmp=y(i)*ch(j,i);
      a(j)-=tmp;
      b(j)-=tmp;
    }
  }
}

void bounded_multivariate_uniform_mcmc(int nvar, const dvector& a1,
  const dvector& b1, dmatrix& ch, const double& _wght, const dvector& y,
  const random_number_generator& rng)
{
  double& wght=(double&) _wght;
  dvector a(1,nvar);
  dvector b(1,nvar);
  a=a1;
  b=b1;
  wght=0;
  double ah;
  double bl;
  double upper;
  double lower;
  double diff;
  for (int i=1;i<=nvar;i++)
  {
    ah=a(i)/ch(i,i);
    bl=b(i)/ch(i,i);
    lower=ffmax(-1.0,ah);
    upper=ffmin(1.0,bl);
    diff=upper-lower;
    wght-=log(diff);
    for (int j=i;j<=nvar;j++)
    {
      double tmp=y(i)*ch(j,i);
      a(j)-=tmp;
      b(j)-=tmp;
    }
  }
}
