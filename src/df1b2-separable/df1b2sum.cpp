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

/**
 * Description not yet available.
 * \param
 */
  df1b2variable operator + (const df1b2variable& _x, const df1b2variable& _y)
  {
    ADUNCONST(df1b2variable,x)
    ADUNCONST(df1b2variable,y)
    df1b2variable z;
    double * xd=x.get_u_dot();
    double * yd=y.get_u_dot();
    double * zd=z.get_u_dot();
    *z.get_u()=*x.get_u()+*y.get_u();
    for (unsigned int i=0;i<df1b2variable::nvar;i++)
    {
      *zd++ = *xd++ + *yd++;
    }

    // WRITE WHATEVER ON TAPE
    if (!df1b2_gradlist::no_derivatives)
      f1b2gradlist->write_pass1_sum(&x,&y,&z);
    return z;
  }

void ad_read_pass2_sum(void);

/**
 * Description not yet available.
 * \param
 */
 int df1b2_gradlist::write_pass1_sum(const df1b2variable * _px,
   const df1b2variable * _py,df1b2variable * pz)
 {
   ADUNCONST(df1b2variable*,px)
   ADUNCONST(df1b2variable*,py)
   ncount++;
#if defined(CHECK_COUNT)
  if (ncount >= ncount_check)
    cout << ncount << endl;
#endif
   //int nvar=df1b2variable::nvar;

   size_t total_bytes=3*sizeof(df1b2_header);
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="TU";
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
   memcpy(list,(df1b2_header*)(py),sizeof(df1b2_header));
   memcpy(list,(df1b2_header*)(pz),sizeof(df1b2_header));
   // ***** write  record size
   nlist.bptr->numbytes=adptr_diff(list.bptr,tmpptr);
   nlist.bptr->pf=(ADrfptr)(&ad_read_pass2_sum);
      ++nlist;
  return 0;
 }


void read_pass2_1_sum(void);
void read_pass2_2_sum(void);
void read_pass2_3_sum(void);

/**
 * Description not yet available.
 * \param
 */
void ad_read_pass2_sum(void)
{
  switch(df1b2variable::passnumber)
  {
  case 1:
    read_pass2_1_sum();
    break;
  case 2:
    read_pass2_2_sum();
    break;
  case 3:
    read_pass2_3_sum();
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
void read_pass2_1_sum(void)
{
  // We are going backword for bptr and nbptr
  // and  forward for bptr2 and nbptr2
  // the current entry+2 in bptr is the size of the record i.e
  // points to the next record
  unsigned int nvar=df1b2variable::nvar;
  test_smartlist& list=f1b2gradlist->list;
  int num_bytes=f1b2gradlist->nlist.bptr->numbytes;
  list-=num_bytes;
  list.saveposition(); // save pointer to beginning of record;

  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("TU",f1b2gradlist->list);
#endif
  char * bptr=f1b2gradlist->list.bptr;
  df1b2_header * px=(df1b2_header *) bptr;
  bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) bptr;
  bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) bptr;

  list.restoreposition(); // save pointer to beginning of record;

  // ****************************************************************
  // turn this off if no third derivatives are calculated
  // if (!no_third_derivatives)
  // {
  // save for second reverse pass
  // save identifier 1

  double* px_u_bar = px->u_bar;
  double* pz_u_bar = pz->u_bar;
  for (unsigned int i=0;i<nvar;i++)
  {
    *px_u_bar += *pz_u_bar;

    ++px_u_bar;
    ++pz_u_bar;
  }
  double* py_u_bar = py->u_bar;
  pz_u_bar = pz->u_bar;
  for (unsigned int i=0;i<nvar;i++)
  {
    *py_u_bar += *pz_u_bar;

    ++py_u_bar;
    ++pz_u_bar;
  }
  double* px_u_dot_bar = px->u_dot_bar;
  double* pz_u_dot_bar = pz->u_dot_bar;
  for (unsigned int i=0;i<nvar;i++)
  {
    *px_u_dot_bar += *pz_u_dot_bar;

    ++px_u_dot_bar;
    ++pz_u_dot_bar;
  }
  double* py_u_dot_bar = py->u_dot_bar;
  pz_u_dot_bar = pz->u_dot_bar;
  for (unsigned int i=0;i<nvar;i++)
  {
    *py_u_dot_bar += *pz_u_dot_bar;

    ++py_u_dot_bar;
    ++pz_u_dot_bar;
  }

  constexpr size_t sizeofdouble = sizeof(double);
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  memset(pz->u_bar, 0, nvar * sizeofdouble);
  memset(pz->u_dot_bar, 0, nvar * sizeofdouble);
}

/**
 * Description not yet available.
 * \param
 */
void read_pass2_2_sum(void)
{
  //const int nlist_record_size=sizeof(int)+sizeof(char*);
  // We are going forward for bptr and backword for bptr2
  //
  // list 1
  //
  unsigned int nvar=df1b2variable::nvar;
  test_smartlist & list=f1b2gradlist->list;

  size_t total_bytes=3*sizeof(df1b2_header);
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="BY";
  int slen=strlen(ids);
  total_bytes+=slen;
#endif
  list.check_buffer_size(total_bytes);
// end of string identifier debug stuff

  list.saveposition(); // save pointer to beginning of record;
  fixed_smartlist & nlist=f1b2gradlist->nlist;
  // get record size
  int num_bytes=nlist.bptr->numbytes;
  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("TU",list);
#endif
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.restoreposition(num_bytes); // save pointer to beginning of record;

  double * x_bar_tilde=px->get_u_bar_tilde();
  double * x_dot_bar_tilde=px->get_u_dot_bar_tilde();
  double * y_bar_tilde=py->get_u_bar_tilde();
  double * y_dot_bar_tilde=py->get_u_dot_bar_tilde();
  double * z_bar_tilde=pz->get_u_bar_tilde();
  double * z_dot_bar_tilde=pz->get_u_dot_bar_tilde();
  // Do second "reverse-reverse" pass calculations
  constexpr size_t sizeofdouble = sizeof(double);
  memset(z_bar_tilde, 0, nvar * sizeofdouble);
  memset(z_dot_bar_tilde, 0, nvar * sizeofdouble);

  // start with x and add y
  for (unsigned int i=0;i<nvar;i++)
  {
    z_bar_tilde[i]+=x_bar_tilde[i];
  }

  for (unsigned int i=0;i<nvar;i++)
  {
    z_dot_bar_tilde[i]+=x_dot_bar_tilde[i];
  }

  // start with y and add x
  for (unsigned int i=0;i<nvar;i++)
  {
    z_bar_tilde[i]+=y_bar_tilde[i];
  }

  for (unsigned int i=0;i<nvar;i++)
  {
    z_dot_bar_tilde[i]+=y_dot_bar_tilde[i];
  }
}

/**
 * Description not yet available.
 * \param
 */
void read_pass2_3_sum(void)
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
  //df1b2_header x,z;

  // get info from tape1
  // get info from tape1
#if defined(SAFE_ALL)
  checkidentiferstring("TU",list);
#endif
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);

  list.restoreposition(); // save pointer to beginning of record;

  *(px->u_tilde)+=*(pz->u_tilde);
  *(py->u_tilde)+=*(pz->u_tilde);
  for (unsigned int i=0;i<nvar;i++)
  {
    px->u_dot_tilde[i]+=pz->u_dot_tilde[i];
    py->u_dot_tilde[i]+=pz->u_dot_tilde[i];
  }
  *(pz->u_tilde)=0;
  constexpr size_t sizeofdouble = sizeof(double);
  memset(pz->u_dot_tilde, 0, nvar * sizeofdouble);
}
