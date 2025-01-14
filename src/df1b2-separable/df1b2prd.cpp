/*
 * $Id$
 *
 * Author: David Fournier
 * Copyright (c) 2008-2012 Regents of the University of California
 */
/**
 * \file
 * Description not yet available.
 */
#include <df1b2fun.h>
#ifndef OPT_LIB
  #include <cassert>
  #include <climits>
#endif

/**
 * Description not yet available.
 * \param
 */
  df1b2variable operator * (const df1b2variable& _x,const df1b2variable& _y)
  {
    ADUNCONST(df1b2variable,x)
    ADUNCONST(df1b2variable,y)
    df1b2variable z;
    double * xd=x.get_u_dot();
    double * yd=y.get_u_dot();
    double * zd=z.get_u_dot();
    double xu=*x.get_u();
    double yu=*y.get_u();

    *z.get_u()=xu*yu;

    for (unsigned int i=0;i<df1b2variable::nvar;i++)
    {
      *zd++ = yu * *xd++ + xu * *yd++;
    }

    // WRITE WHATEVER ON TAPE
    if (!df1b2_gradlist::no_derivatives)
      f1b2gradlist->write_pass1_prod(&x,&y,&z);
    return z;
  }

void ad_read_pass2_prod(void);

/**
 * Description not yet available.
 * \param
 */
 int df1b2_gradlist::write_pass1_prod(const df1b2variable * _px,
   const df1b2variable * _py,df1b2variable * pz)
 {
   ADUNCONST(df1b2variable*,px)
   ADUNCONST(df1b2variable*,py)
   ncount++;
#if defined(CHECK_COUNT)
  if (ncount >= ncount_check)
    cout << ncount << endl;
#endif
   size_t nvar=df1b2variable::nvar;

   //int total_bytes=3*sizeof(df1b2_header)+sizeof(char*)
   //  +2*(nvar+1)*sizeof(double);
   size_t total_bytes=3*sizeof(df1b2_header)
     +2*(nvar+1)*sizeof(double);
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="DL";
  size_t slen=strlen(ids);
  total_bytes+=slen;
#endif

  list.check_buffer_size(total_bytes);
  void * tmpptr=list.bptr;
#if defined(SAFE_ALL)
  memcpy(list,ids,slen);
#endif
// end of string identifier debug stuff

   memcpy(list,(df1b2_header*)(px),sizeof(df1b2_header));
   memcpy(list,(df1b2_header*)(py),sizeof(df1b2_header));
   memcpy(list,(df1b2_header*)(pz),sizeof(df1b2_header));
   //memcpy(list,&pf,sizeof(char *));
   //*(char**)(list.bptr)=(char*)pf;
   size_t sizeofdouble = sizeof(double);
   memcpy(list,px->get_u(),sizeofdouble);
   memcpy(list,py->get_u(),sizeofdouble);
   memcpy(list,px->get_u_dot(),nvar*sizeofdouble);
   memcpy(list,py->get_u_dot(),nvar*sizeofdouble);
   // ***** write  record size
   nlist.bptr->numbytes=adptr_diff(list.bptr,tmpptr);
   nlist.bptr->pf=(ADrfptr)(&ad_read_pass2_prod);
      ++nlist;
   return 0;
 }


void read_pass2_1_prod(void);
void read_pass2_2_prod(void);
void read_pass2_3_prod(void);

/**
 * Description not yet available.
 * \param
 */
void ad_read_pass2_prod(void)
{
  switch(df1b2variable::passnumber)
  {
  case 1:
    read_pass2_1_prod();
    break;
  case 2:
    read_pass2_2_prod();
    break;
  case 3:
    read_pass2_3_prod();
    break;
  default:
    cerr << "illegal value for df1b2variable::pass = "
         << df1b2variable::passnumber << endl;
    ad_exit(1);
  }
}

/**
 * Description not yet available.
 * \param
 */
void read_pass2_1_prod(void)
{
  // We are going backword for bptr and nbptr
  // and  forward for bptr2 and nbptr2
  // the current entry+2 in bptr is the size of the record i.e
  // points to the next record
  //char * bptr=f1b2gradlist->bptr;
  //char * bptr2=f1b2gradlist2->bptr;
  size_t nvar=df1b2variable::nvar;
  test_smartlist& list=f1b2gradlist->list;
  //f1b2gradlist->nlist-=sizeof(int);
  int num_bytes=f1b2gradlist->nlist.bptr->numbytes;
  list-=num_bytes;
  list.saveposition(); // save pointer to beginning of record;
  double xu,yu;
  //ad_dstar xdot,ydot;
  //df1b2function2 * pf;

  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("DL",f1b2gradlist->list);
#endif
  char * bptr=f1b2gradlist->list.bptr;
  df1b2_header * px=(df1b2_header *) bptr;
  bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) bptr;
  bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) bptr;
  bptr+=sizeof(df1b2_header);
  //pf=*(df1b2function2 **) bptr;
  //bptr+=sizeof(char*);
  constexpr size_t sizeofdouble = sizeof(double);
  size_t total_bytes=2*nvar*sizeofdouble;
  memcpy(&xu,bptr,sizeofdouble);
  bptr+=sizeofdouble;
  memcpy(&yu,bptr,sizeofdouble);
  bptr+=sizeofdouble;
  double * xdot=(double*)bptr;
  bptr+=nvar*sizeofdouble;
  double * ydot=(double*)bptr;

  list.restoreposition(); // save pointer to beginning of record;

  // ****************************************************************
  // turn this off if no third derivatives are calculated
  // if (!no_third_derivatives)
  // {
  // save for second reverse pass
  // save identifier 1
     test_smartlist & list2 = f1b2gradlist->list2;

// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="QK";
  size_t slen=strlen(ids);
  total_bytes+=slen;
#endif

  list2.check_buffer_size(total_bytes);

  void * tmpptr=list2.bptr;
#if defined(SAFE_ALL)
  memcpy(list2,ids,slen);
#endif

  fixed_smartlist2 & nlist2 = f1b2gradlist->nlist2;
  memcpy(list2,pz->get_u_bar(),nvar*sizeofdouble);
  memcpy(list2,pz->get_u_dot_bar(),nvar*sizeofdouble);
  *nlist2.bptr=adptr_diff(list2.bptr,tmpptr);
  ++nlist2;

  // Do first reverse pass calculations
  double* px_u_bari = px->u_bar;
  double* pz_u_bari = pz->u_bar;
  for (size_t i=0;i<nvar;i++)
  {
   //px->u_bar[i]+=(pf->df1)(xu,yu)*pz->u_bar[i];
    *px_u_bari += yu * *pz_u_bari;
    ++px_u_bari;
    ++pz_u_bari;
  }
  double* py_u_bari = py->u_bar;
  pz_u_bari = pz->u_bar;
  for (size_t i=0;i<nvar;i++)
  {
    //py->u_bar[i]+=(pf->df2)(xu,yu)*pz->u_bar[i];
    *py_u_bari += xu * *pz_u_bari;
    ++py_u_bari;
    ++pz_u_bari;
  }

  px_u_bari = px->u_bar;
  double* pz_u_dot_bari = pz->u_dot_bar;
  double* ydoti = ydot;
  for (size_t i=0;i<nvar;i++)
  {
    //px->u_bar[i]+=(pf->d2f11)(xu,yu)*xdot[i]*pz->u_dot_bar[i];
    //px->u_bar[i]+=(pf->d2f12)(xu,yu)*ydot[i]*pz->u_dot_bar[i];
    *px_u_bari += *ydoti * *pz_u_dot_bari;
    ++px_u_bari;
    ++pz_u_dot_bari;
    ++ydoti;
  }

  py_u_bari = py->u_bar;
  double* xdoti = xdot;
  pz_u_dot_bari = pz->u_dot_bar;
  for (size_t i=0;i<nvar;i++)
  {
    //py->u_bar[i]+=(pf->d2f22)(xu,yu)*ydot[i]*pz->u_dot_bar[i];
    //py->u_bar[i]+=(pf->d2f12)(xu,yu)*xdot[i]*pz->u_dot_bar[i];
    *py_u_bari += *xdoti * *pz_u_dot_bari;

    ++py_u_bari;
    ++xdoti;
    ++pz_u_dot_bari;
  }
  double* px_u_dot_bari = px->u_dot_bar;
  pz_u_dot_bari = pz->u_dot_bar;
  for (size_t i=0;i<nvar;i++)
  {
    //px->u_dot_bar[i]+=(pf->df1)(xu,yu)*pz->u_dot_bar[i];
    *px_u_dot_bari += yu * *pz_u_dot_bari;

    ++px_u_dot_bari;
    ++pz_u_dot_bari;
  }
  double* py_u_dot_bari = py->u_dot_bar;
  pz_u_dot_bari = pz->u_dot_bar;
  for (size_t i=0;i<nvar;i++)
  {
    //py->u_dot_bar[i]+=(pf->df2)(xu,yu)*pz->u_dot_bar[i];
    *py_u_dot_bari += xu * *pz_u_dot_bari;
    ++py_u_dot_bari;
    ++pz_u_dot_bari;
  }

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  memset(pz->u_bar, 0, nvar * sizeofdouble);
  memset(pz->u_dot_bar, 0, nvar * sizeofdouble);
}

/**
 * Description not yet available.
 * \param
 */
void read_pass2_2_prod(void)
{
  //const int nlist_record_size=sizeof(int)+sizeof(char*);
  // We are going forward for bptr and backword for bptr2
  //
  // list 1
  //
  unsigned int nvar=df1b2variable::nvar;
  test_smartlist & list=f1b2gradlist->list;

  constexpr size_t sizeofdouble = sizeof(double);
  //int total_bytes=3*sizeof(df1b2_header)+sizeof(char*)
  //  +2*(nvar+1)*sizeof(double);
  size_t total_bytes=3*sizeof(df1b2_header)
    +2*(nvar+1)*sizeofdouble;
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="DL";
  size_t slen=strlen(ids);
  total_bytes+=slen;
#endif

  list.check_buffer_size(total_bytes);
// end of string identifier debug stuff

  list.saveposition(); // save pointer to beginning of record;
  fixed_smartlist & nlist=f1b2gradlist->nlist;
   // nlist-=sizeof(int);
  // get record size
  int num_bytes=nlist.bptr->numbytes;
  //
  // list 2
  //
  test_smartlist & list2=f1b2gradlist->list2;
  fixed_smartlist2 & nlist2=f1b2gradlist->nlist2;
  // get record size
  int num_bytes2=*nlist2.bptr;
  --nlist2;
  // backup the size of the record
  list2-=num_bytes2;
  list2.saveposition(); // save pointer to beginning of record;
  // save the pointer to the beginning of the record
  // bptr and bptr2 now both point to the beginning of their records

  double xu,yu;
  double * xdot;
  double * ydot;
  //df1b2_header x,z;
  //df1b2function2 * pf;

  // get info from tape1
  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("DL",list);
  checkidentiferstring("QK",list2);
#endif
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  //pf=*(df1b2function2 **) list.bptr;
  //list.bptr+=sizeof(char*);
  memcpy(&xu,list.bptr,sizeofdouble);
  list.bptr+=sizeof(double);
  memcpy(&yu,list.bptr,sizeofdouble);
  list.bptr+=sizeof(double);
  xdot=(double*)list.bptr;
  list.bptr+=nvar*sizeofdouble;
  ydot=(double*)list.bptr;
  list.restoreposition(num_bytes); // save pointer to beginning of record;

  double * zbar;
  double * zdotbar;

  zbar=(double*)list2.bptr;
  zdotbar=(double*)(list2.bptr+nvar*sizeof(double));
  list2.restoreposition(); // save pointer to beginning of record;

  double * x_tilde=px->get_u_tilde();
  double * x_dot_tilde=px->get_u_dot_tilde();
  double * x_bar_tilde=px->get_u_bar_tilde();
  double * x_dot_bar_tilde=px->get_u_dot_bar_tilde();
  double * y_tilde=py->get_u_tilde();
  double * y_dot_tilde=py->get_u_dot_tilde();
  double * y_bar_tilde=py->get_u_bar_tilde();
  double * y_dot_bar_tilde=py->get_u_dot_bar_tilde();
  double * z_bar_tilde=pz->get_u_bar_tilde();
  double * z_dot_bar_tilde=pz->get_u_dot_bar_tilde();
  // Do second "reverse-reverse" pass calculations

  memset(z_bar_tilde, 0, nvar * sizeofdouble);
  memset(z_dot_bar_tilde, 0, nvar * sizeofdouble);

  // start with x and add y
  double* x_bar_tildei = x_bar_tilde;
  double* zbari = zbar;
  double* z_bar_tildei = z_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //*x_tilde+=(pf->d2f11)(xu,yu)*zbar[i]*x_bar_tilde[i];
    //z_bar_tilde[i]+=(pf->df1)(xu,yu)*x_bar_tilde[i];
    //*y_tilde+=(pf->d2f12)(xu,yu)*zbar[i]*x_bar_tilde[i];
    *z_bar_tildei += yu * *x_bar_tildei;
    *y_tilde += *zbari * *x_bar_tildei;

    ++x_bar_tildei;
    ++zbari;
    ++z_bar_tildei;
  }
  double* x_dot_bar_tildei = x_dot_bar_tilde;
  double* zdotbari = zdotbar;
  double* z_dot_bar_tildei = z_dot_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //*x_tilde+=(pf->d2f11)(xu,yu)*zdotbar[i]*x_dot_bar_tilde[i];
    //*y_tilde+=(pf->d2f12)(xu,yu)*zdotbar[i]*x_dot_bar_tilde[i];
    //z_dot_bar_tilde[i]+=(pf->df1)(xu,yu)*x_dot_bar_tilde[i];
    *y_tilde += *zdotbari * *x_dot_bar_tildei;
    *z_dot_bar_tildei += yu * *x_dot_bar_tildei;

    ++x_dot_bar_tildei;
    ++zdotbari;
    ++z_dot_bar_tildei;
  }

  /*
  for (i=0;i<nvar;i++)
  {
    x_dot_tilde[i]+=(pf->d2f11)(xu,yu)*zdotbar[i]*x_bar_tilde[i];
    z_dot_bar_tilde[i]+=(pf->d2f11)(xu,yu)*xdot[i]*x_bar_tilde[i];
    *x_tilde+=(pf->d3f111)(xu,yu)*xdot[i]*zdotbar[i]*x_bar_tilde[i];
    *y_tilde+=(pf->d3f112)(xu,yu)*xdot[i]*zdotbar[i]*x_bar_tilde[i];
  }
  */
  // start with y and add x
  double* y_bar_tildei = y_bar_tilde;
  zbari = zbar;
  z_bar_tildei = z_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //*y_tilde+=(pf->d2f22)(xu,yu)*zbar[i]*y_bar_tilde[i];
    //*x_tilde+=(pf->d2f12)(xu,yu)*zbar[i]*y_bar_tilde[i];
    //z_bar_tilde[i]+=(pf->df2)(xu,yu)*y_bar_tilde[i];
    *x_tilde += *zbari * *y_bar_tildei;
    *z_bar_tildei += xu * *y_bar_tildei;

    ++y_bar_tildei;
    ++zbari;
    ++z_bar_tildei;
  }
  double* y_dot_bar_tildei = y_dot_bar_tilde;
  zdotbari = zdotbar;
  z_dot_bar_tildei = z_dot_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //*y_tilde+=(pf->d2f22)(xu,yu)*zdotbar[i]*y_dot_bar_tilde[i];
    //*x_tilde+=(pf->d2f12)(xu,yu)*zdotbar[i]*y_dot_bar_tilde[i];
    //z_dot_bar_tilde[i]+=(pf->df2)(xu,yu)*y_dot_bar_tilde[i];
    *x_tilde += *zdotbari * *y_dot_bar_tildei;
    *z_dot_bar_tildei += xu * *y_dot_bar_tildei;

    ++y_dot_bar_tildei;
    ++zdotbari;
    ++z_dot_bar_tildei;
  }
  /*
  for (i=0;i<nvar;i++)
  {
    y_dot_tilde[i]+=(pf->d2f22)(xu,yu)*zdotbar[i]*y_bar_tilde[i];
    z_dot_bar_tilde[i]+=(pf->d2f22)(xu,yu)*ydot[i]*y_bar_tilde[i];
    *y_tilde+=(pf->d3f222)(xu,yu)*ydot[i]*zdotbar[i]*y_bar_tilde[i];
    *x_tilde+=(pf->d3f122)(xu,yu)*ydot[i]*zdotbar[i]*y_bar_tilde[i];
  }
  */
  x_bar_tildei = x_bar_tilde;
  zdotbari = zdotbar;
  double* ydoti = ydot;
  double* y_dot_tildei = y_dot_tilde;
  z_dot_bar_tildei = z_dot_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //*x_tilde+=(pf->d3f112)(xu,yu)*ydot[i]*zdotbar[i]*x_bar_tilde[i];
    //*y_tilde+=(pf->d3f122)(xu,yu)*ydot[i]*zdotbar[i]*x_bar_tilde[i];
    //y_dot_tilde[i]+=(pf->d2f12)(xu,yu)*zdotbar[i]*x_bar_tilde[i];
    //z_dot_bar_tilde[i]+=(pf->d2f12)(xu,yu)*ydot[i]*x_bar_tilde[i];
    *y_dot_tildei += *zdotbari * *x_bar_tildei;
    *z_dot_bar_tildei += *ydoti * *x_bar_tildei;

    ++x_bar_tildei;
    ++zdotbari;
    ++ydoti;
    ++y_dot_tildei;
    ++z_dot_bar_tildei;
  }
  y_bar_tildei = y_bar_tilde;
  double* xdoti = xdot;
  zdotbari = zdotbar;
  double* x_dot_tildei = x_dot_tilde;
  z_dot_bar_tildei = z_dot_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //*x_tilde+=(pf->d3f112)(xu,yu)*xdot[i]*zdotbar[i]*y_bar_tilde[i];
    //*y_tilde+=(pf->d3f122)(xu,yu)*xdot[i]*zdotbar[i]*y_bar_tilde[i];
    //x_dot_tilde[i]+=(pf->d2f12)(xu,yu)*zdotbar[i]*y_bar_tilde[i];
    //z_dot_bar_tilde[i]+=(pf->d2f12)(xu,yu)*xdot[i]*y_bar_tilde[i];
    *x_dot_tildei += *zdotbari * *y_bar_tildei;
    *z_dot_bar_tildei += *xdoti * *y_bar_tildei;

    ++y_bar_tildei;
    ++xdoti;
    ++zdotbari;
    ++x_dot_tildei;
    ++z_dot_bar_tildei;
  }
}

/**
 * Description not yet available.
 * \param
 */
void read_pass2_3_prod(void)
{
  // We are going backword for bptr and forward for bptr2
  // the current entry+2 in bptr is the size of the record i.e
  // points to the next record
  unsigned int nvar=df1b2variable::nvar;
  fixed_smartlist & nlist=f1b2gradlist->nlist;
  test_smartlist& list=f1b2gradlist->list;
   // nlist-=sizeof(int);
  // get record size
  int num_bytes=nlist.bptr->numbytes;
  // backup the size of the record
  list-=num_bytes;
  list.saveposition(); // save pointer to beginning of record;
  // save the pointer to the beginning of the record
  double xu;
  double yu;
  double * xdot;
  double * ydot;
  //df1b2_header x,z;
  //df1b2function2 * pf;

  // get info from tape1
  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("DL",list);
#endif
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  //pf=*(df1b2function2 **) list.bptr;
  //list.bptr+=sizeof(char*);
  constexpr size_t sizeofdouble = sizeof(double);
  memcpy(&xu,list.bptr,sizeofdouble);
  list.bptr+=sizeofdouble;
  memcpy(&yu,list.bptr,sizeofdouble);
  list.bptr+=sizeofdouble;
  xdot=(double*)list.bptr;
  list.bptr+=nvar*sizeofdouble;
  ydot=(double*)list.bptr;
  list.restoreposition(); // save pointer to beginning of record;

  //*(px->u_tilde)+=(pf->df1)(xu,yu)* *(pz->u_tilde);
  //*(py->u_tilde)+=(pf->df2)(xu,yu)* *(pz->u_tilde);
  *(px->u_tilde)+=yu* *(pz->u_tilde);
  *(py->u_tilde)+=xu* *(pz->u_tilde);

  double* pz_u_dot_tildei = pz->u_dot_tilde;
  double* xdoti = xdot;
  double* ydoti = ydot;
  for (size_t i=0;i<nvar;i++)
  {
    //*(px->u_tilde)+=(pf->d2f11)(xu,yu)*xdot[i]*pz->u_dot_tilde[i];
    //*(py->u_tilde)+=(pf->d2f12)(xu,yu)*xdot[i]*pz->u_dot_tilde[i];
    *(py->u_tilde) += *xdoti * *pz_u_dot_tildei;
    //*(py->u_tilde)+=(pf->d2f22)(xu,yu)*ydot[i]*pz->u_dot_tilde[i];
    //*(px->u_tilde)+=(pf->d2f12)(xu,yu)*ydot[i]*pz->u_dot_tilde[i];
    *(px->u_tilde) += *ydoti * *pz_u_dot_tildei;

    ++pz_u_dot_tildei;
    ++xdoti;
    ++ydoti;
  }
  double* px_u_dot_tildei = px->u_dot_tilde;
  double* py_u_dot_tildei = py->u_dot_tilde;
  pz_u_dot_tildei = pz->u_dot_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //px->u_dot_tilde[i]+=(pf->df1)(xu,yu)*pz->u_dot_tilde[i];
    //py->u_dot_tilde[i]+=(pf->df2)(xu,yu)*pz->u_dot_tilde[i];
    *px_u_dot_tildei += yu * *pz_u_dot_tildei;
    *py_u_dot_tildei += xu * *pz_u_dot_tildei;

    ++px_u_dot_tildei;
    ++py_u_dot_tildei;
    ++pz_u_dot_tildei;
  }
  *(pz->u_tilde)=0;
  memset(pz->u_dot_tilde, 0, nvar * sizeofdouble);
}
