/**
 * Author: David Fournier
 * Copyright (c) 2008-2012 Regents of the University of California
 */
#include <df1b2fun.h>
#ifndef OPT_LIB
  #include <cassert>
  #include <climits>
#endif

#if defined(CHECK_COUNT)
void ncount_checker(int ncount,int ncount_check)
{
  //if (ncount ==6599)
  //  cout << ncount << endl;
}
#endif

void ad_read_pass1(void);
void ad_read_pass2(void);
void read_pass1_3(void);
// should inline this

/**
 * Description not yet available.
 * \param
 */
int df1b2_gradlist::write_pass1(const df1b2variable * _px,
  df1b2variable * pz, df1b2function1 * pf)
{
  ADUNCONST(df1b2variable*,px)
  ncount++;
#if defined(CHECK_COUNT)
  if (ncount >= ncount_check)
    ncount_checker(ncount,ncount_check);
#endif

  unsigned int nvar=df1b2variable::nvar;

  size_t total_bytes=sizeof(df1b2_header)+sizeof(df1b2_header)+sizeof(char*)
    +sizeof(double)+nvar*sizeof(double);
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="CX";
  int slen=strlen(ids);
  total_bytes+=slen;
#endif

  list.check_buffer_size(total_bytes);
  void * tmpptr=list.bptr;
#if defined(SAFE_ALL)
  memcpy(list,ids,slen);
#endif
// end of string identifier debug stuff

  memcpy(list,(df1b2_header*)(px),sizeof(df1b2_header));
  memcpy(list,(df1b2_header*)(pz),sizeof(df1b2_header));
  //list.bptr+=sizeof(df1b2_header);
  memcpy(list,&pf,sizeof(char*));
  //*(char**)(list.bptr)=(char*)pf;
  //list.bptr+=sizeof(char*);
  memcpy(list,px->get_u(),sizeof(double));
  //list.bptr+=sizeof(double);
  memcpy(list,px->get_u_dot(),nvar*(int)sizeof(double));
  //list.bptr+=nvar*sizeof(double);
  // ***** write  record size
  nlist.bptr->numbytes=adptr_diff(list.bptr,tmpptr);
  nlist.bptr->pf=(ADrfptr)(&ad_read_pass1);
  ++nlist;
  return 0;
}

/**
 * Description not yet available.
 * \param
 */
void ad_read_pass1(void)
{
  switch(df1b2variable::passnumber)
  {
  case 1:
    read_pass1_1();
    break;
  case 2:
    read_pass1_2();
    break;
  case 3:
    read_pass1_3();
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
void read_pass1_1(void)
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
  //list.bptr-=num_bytes;
  list.saveposition(); // save pointer to beginning of record;
  // save the pointer to the beginning of the record
  double xu;
  //df1b2_header x,z;
  df1b2function1 * pf;

  // get info from tape1
  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("CX",list);
#endif
  constexpr size_t sizeofdouble = sizeof(double);
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  pf=*(df1b2function1 **) list.bptr;
  list.bptr+=sizeof(char*);
  memcpy(&xu,list.bptr,sizeofdouble);
  list.bptr+=sizeofdouble;
  double* xdot=(double*)list.bptr;
  list.restoreposition(); // save pointer to beginning of record;

  // Do first reverse paSS calculations
  // ****************************************************************
  // turn this off if no third derivatives are calculated
  // if (!no_third_derivatives)
  // {
  // save for second reverse pass
  // save identifier 1
     fixed_smartlist2& nlist2=f1b2gradlist->nlist2;
     test_smartlist& list2=f1b2gradlist->list2;


  size_t total_bytes=2*nvar*sizeofdouble;
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="DU";
  size_t slen=strlen(ids);
  total_bytes+=slen;
#endif

  list2.check_buffer_size(total_bytes);
  void * tmpptr2=list2.bptr;

#if defined(SAFE_ALL)
  memcpy(list2,ids,slen);
#endif

   memcpy(list2,pz->get_u_bar(),nvar*sizeofdouble);
   memcpy(list2,pz->get_u_dot_bar(),nvar*sizeofdouble);
   *nlist2.bptr=adptr_diff(list2.bptr,tmpptr2);
   ++nlist2;
  // }
  //
  // ****************************************************************
#if defined(PRINT_DERS)
 print_derivatives(pf->funname,(pf->df)(xu),(pf->df)(xu),(pf->d2f)(xu),
  (pf->d3f)(xu),1);
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
#endif

  double df=(pf->df)(xu);
  double d2f=(pf->d2f)(xu);
  //double d3f=(pf->d3f)(xu);

  double* px_u_bari = px->u_bar;
  double* pz_u_bari = pz->u_bar;
  for (size_t i=0;i<nvar;i++)
  {
    //px->u_bar[i]+=(pf->df)(xu)* pz->u_bar[i];
    *px_u_bari += df * *pz_u_bari;

    ++px_u_bari;
    ++pz_u_bari;
  }
  px_u_bari = px->u_bar;
  double* xdoti = xdot;
  double* pz_u_dot_bari = pz->u_dot_bar;
  for (size_t i=0;i<nvar;i++)
  {
    //px->u_bar[i]+=(pf->d2f)(xu)*xdot[i]*pz->u_dot_bar[i];
    *px_u_bari += d2f * *xdoti * *pz_u_dot_bari;

    ++px_u_bari;
    ++xdoti;
    ++pz_u_dot_bari;
  }
  double* px_u_dot_bari = px->u_dot_bar;
  pz_u_dot_bari = pz->u_dot_bar;
  for (size_t i=0;i<nvar;i++)
  {
    //px->u_dot_bar[i]+=(pf->df)(xu)*pz->u_dot_bar[i];
    *px_u_dot_bari += df * *pz_u_dot_bari;

    ++px_u_dot_bari;
    ++pz_u_dot_bari;
  }

  // !!!!!!!!!!!!!!!!!!!!!!
  memset(pz->u_bar, 0, nvar * sizeofdouble);
  memset(pz->u_dot_bar, 0, nvar * sizeofdouble);

#if defined(PRINT_DERS)
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
#endif
}

/**
 * Description not yet available.
 * \param
 */
void read_pass1_2(void)
{
  //const int nlist_record_size=sizeof(int)+sizeof(char*);
  // We are going forward for bptr and backword for bptr2
  //
  // list 1
  //
  unsigned int nvar=df1b2variable::nvar;
  test_smartlist & list=f1b2gradlist->list;

  size_t total_bytes=sizeof(df1b2_header)+sizeof(df1b2_header)+sizeof(char*)
    +sizeof(double)+nvar*sizeof(double);
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="CX";
  int slen=strlen(ids);
  total_bytes+=slen;
#endif

  list.check_buffer_size(total_bytes);
// end of string identifier debug stuff

  list.saveposition(); // save pointer to beginning of record;
  fixed_smartlist & nlist=f1b2gradlist->nlist;
   // nlist-=sizeof(int);
  // get record size
  int num_bytes=nlist.bptr->numbytes;
    // nlist+=nlist_record_size;
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
  list2.saveposition();
  // save the pointer to the beginning of the record
  // bptr and bptr2 now both point to the beginning of their records

  double xu;
  //df1b2_header x,z;
  df1b2function1 * pf;

  // get info from tape1
  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("CX",list);
  checkidentiferstring("DU",list2);
#endif
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  pf=*(df1b2function1 **) list.bptr;
  list.bptr+=sizeof(char*);
  memcpy(&xu,list.bptr,sizeof(double));
  list.bptr+=sizeof(double);
  double* xdot=(double*)list.bptr;
  list.restoreposition(num_bytes); // save pointer to beginning of record;

  double* zbar=(double*)list2.bptr;
  double* zdotbar=(double*)(list2.bptr+nvar*sizeof(double));

  double * x_tilde=px->get_u_tilde();
  double * x_dot_tilde=px->get_u_dot_tilde();
  double * x_bar_tilde=px->get_u_bar_tilde();
  double * x_dot_bar_tilde=px->get_u_dot_bar_tilde();
  double * z_bar_tilde=pz->get_u_bar_tilde();
  double * z_dot_bar_tilde=pz->get_u_dot_bar_tilde();
#if defined(PRINT_DERS)
 print_derivatives(pf->funname,(pf->df)(xu),(pf->df)(xu),(pf->d2f)(xu),
  (pf->d3f)(xu),1);
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
#endif
  // Do second "reverse-reverse" pass calculations

  constexpr size_t sizeofdouble = sizeof(double);
  memset(z_bar_tilde, 0, nvar * sizeofdouble);
  memset(z_dot_bar_tilde, 0, nvar * sizeofdouble);

  double df=(pf->df)(xu);
  double d2f=(pf->d2f)(xu);
  double d3f=(pf->d3f)(xu);

  double* px_bar_tildei = x_bar_tilde;
  double* pzbari = zbar;
  double* pz_bar_tildei = z_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //*x_tilde+=(pf->d2f)(xu)*zbar[i]*x_bar_tilde[i];
    *x_tilde += d2f * *pzbari * *px_bar_tildei;
    //z_bar_tilde[i]+=(pf->df)(xu)*x_bar_tilde[i];
    *pz_bar_tildei += df * *px_bar_tildei;

    ++px_bar_tildei;
    ++pzbari;
    ++pz_bar_tildei;
  }

  double* pzdotbari = zdotbar;
  double* pz_dot_bar_tildei = z_dot_bar_tilde;
  double* px_dot_bar_tildei = x_dot_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //*x_tilde+=(pf->d2f)(xu)*zdotbar[i]*x_dot_bar_tilde[i];
    *x_tilde+=d2f * *pzdotbari * *px_dot_bar_tildei;
    //z_dot_bar_tilde[i]+=(pf->df)(xu)*x_dot_bar_tilde[i];
    *pz_dot_bar_tildei += df * *px_dot_bar_tildei;

    ++pzdotbari;
    ++pz_dot_bar_tildei;
    ++px_dot_bar_tildei;
  }

  px_bar_tildei = x_bar_tilde;
  pzdotbari = zdotbar;
  double* pxdoti = xdot;
  double* px_dot_tildei = x_dot_tilde;
  pz_dot_bar_tildei = z_dot_bar_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //x_dot_tilde[i]+=(pf->d2f)(xu)*zdotbar[i]*x_bar_tilde[i];
    //z_dot_bar_tilde[i]+=(pf->d2f)(xu)*xdot[i]*x_bar_tilde[i];
    //*x_tilde+=(pf->d3f)(xu)*xdot[i]*zdotbar[i]*x_bar_tilde[i];
    *px_dot_tildei += d2f * *pzdotbari * *px_bar_tildei;
    *pz_dot_bar_tildei += d2f * *pxdoti * *px_bar_tildei;
    *x_tilde += d3f * *pxdoti * *pzdotbari * *px_bar_tildei;

    ++px_bar_tildei;
    ++pzdotbari;
    ++pxdoti;
    ++px_dot_tildei;
    ++pz_dot_bar_tildei;
  }
  list2.restoreposition();
#if defined(PRINT_DERS)
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
#endif
}

/**
 * Description not yet available.
 * \param
 */
void read_pass1_3(void)
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
  //df1b2_header x,z;
  df1b2function1 * pf;

  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("CX",list);
#endif
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  pf=*(df1b2function1 **) list.bptr;
  list.bptr+=sizeof(char*);
  memcpy(&xu,list.bptr,sizeof(double));
  list.bptr+=sizeof(double);
  double* xdot=(double*)list.bptr;
  list.restoreposition(); // save pointer to beginning of record;

#if defined(PRINT_DERS)
 print_derivatives(pf->funname,(pf->df)(xu),(pf->df)(xu),(pf->d2f)(xu),
  (pf->d3f)(xu),1);
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
#endif

  double df=(pf->df)(xu);
  double d2f=(pf->d2f)(xu);
  //*(px->u_tilde)+=(pf->df)(xu)* *(pz->u_tilde);
  *(px->u_tilde)+=df * *(pz->u_tilde);

  double* pz_u_dot_tildei = pz->u_dot_tilde;
  double* pxdoti = xdot;
  for (size_t i=0;i<nvar;i++)
  {
    //*(px->u_tilde)+=(pf->d2f)(xu)*xdot[i]*pz->u_dot_tilde[i];
    *(px->u_tilde) += d2f * *pxdoti * *pz_u_dot_tildei;

    ++pxdoti;
    ++pz_u_dot_tildei;
  }

  double* px_u_dot_tildei = px->u_dot_tilde;
  pz_u_dot_tildei = pz->u_dot_tilde;
  for (size_t i=0;i<nvar;i++)
  {
    //px->u_dot_tilde[i]+=(pf->df)(xu)*pz->u_dot_tilde[i];
    *px_u_dot_tildei += df * *pz_u_dot_tildei;

    ++px_u_dot_tildei;
    ++pz_u_dot_tildei;
  }
  *(pz->u_tilde)=0;
  constexpr size_t sizeofdouble = sizeof(double);
  memset(pz->u_dot_tilde, 0, nvar * sizeofdouble);
#if defined(PRINT_DERS)
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
#endif
}
