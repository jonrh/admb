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
void read_pass1_plus_eq_1(void);
void read_pass1_plus_eq_2(void);
void read_pass1_plus_eq_3(void);
//#define ADDEBUG_PRINT
#if defined(ADDEBUG_PRINT)
  extern int addebug_count;
#endif
//#define PRINT_DERS


#if defined(PRINT_DERS)
/**
 * Description not yet available.
 * \param
 */
void print_derivatives(const adstring& s, double f, double df,
  double d2f,double d3f,int bflag)
{
  ostream * derout;
  derout=&cout;
  if (bflag)
  {
    *derout << "           ---------------------------------------- " << endl;
  }
  *derout << "Function: " << s << " " << "f = " << f
          << " df = " << df << " d2f = " << d2f << " d3f = " << d3f << endl;
}

/**
 * Description not yet available.
 * \param
 */
void print_derivatives(const adstring& s, double f, double df1,
  double df2,double df11,double df12, double df22,
  double df111, double df112, double df122, double df222,int bflag)
{
  ostream * derout;
  derout=&cout;
  if (bflag)
  {
    *derout << endl << "           --------------------------------- " << endl;
  }
  *derout << "Function: " << s << " " << "f = " << f
          << " df1 = " << df1
          << " df2 = " << df2
          << " df11 = " << df11
          << " df12 = " << df12
          << " df22 = " << df22
          << " df111 = " << df111
          << " df112 = " << df112
          << " df122 = " << df122
          << " df222 = " << df222 << endl;
}

/**
 * Description not yet available.
 * \param
 */
void print_derivatives(df1b2_header * px,const char * s,
  int bflag)
{
  ostream * derout;
  derout=&cout;
  //*derout << derprintcount << " " << endl;
  if (bflag)
  {
    *derout << endl << "           --------------------------------- " << endl;
  }
  *derout << "pass " << df1b2variable::passnumber;
  *derout << "  variable " << s << "  address "
          << int(px->u) << endl;
  *derout << "u\t\t = " << *px->u << " ";
  *derout << endl;

  *derout << "udot\t\t = ";
  for (unsigned int i=0;i<df1b2variable::nvar;i++)
    *derout <<  px->u_dot[i] << " ";
  *derout << endl;

  *derout << "u_bar\t\t = ";
  for (unsigned int i=0;i<df1b2variable::nvar;i++)
    *derout <<  px->u_bar[i] << " ";
  *derout << endl;

  *derout << "u_dot_bar\t = ";
  for (unsigned int i=0;i<df1b2variable::nvar;i++)
    *derout <<  px->u_dot_bar[i] << " ";
  *derout << endl;

  if (df1b2variable::passnumber>1)
  {
    *derout << "u_tilde\t\t = " << *px->u_tilde << " ";
    *derout << endl;

    *derout << "u_dot_tilde\t = ";
    for (unsigned int i=0;i<df1b2variable::nvar;i++)
      *derout <<  px->u_dot_tilde[i] << " ";
    *derout << endl;

    *derout << "u_bar_tilde\t = ";
    for (unsigned int i=0;i<df1b2variable::nvar;i++)
      *derout <<  px->u_bar_tilde[i] << " ";
    *derout << endl;

    *derout << "u_dot_bar_tilde\t = ";
    for (unsigned int i=0;i<df1b2variable::nvar;i++)
      *derout <<  px->u_dot_bar_tilde[i] << " ";
    *derout << endl;
  }
  *derout << endl;
}
#endif

/**
 * Description not yet available.
 * \param
 */
df1b2variable& df1b2variable::operator += (const df1b2variable& _x)
{
  ADUNCONST(df1b2variable,x)
  double * xd=x.get_u_dot();
  double * zd=get_u_dot();
  *get_u()+=*x.get_u();
  for (unsigned int i=0;i<df1b2variable::nvar;i++)
  {
    *zd++ += *xd++;
  }

  // WRITE WHATEVER ON TAPE
  //df1b2tape->set_tapeinfo_header(&x,&z,this,xd);
  // save stuff for first reverse pass
  // need &x, &z, this,
  if (!df1b2_gradlist::no_derivatives)
    f1b2gradlist->write_pass1_pluseq(&x,this);
  return *this;
}
void ad_read_pass1_plus_eq(void);

/**
 * Description not yet available.
 * \param
 */
int df1b2_gradlist::write_pass1_pluseq(const df1b2variable * _px,
  df1b2variable * pz)
{
  ncount++;
#if defined(CHECK_COUNT)
  if (ncount >= ncount_check)
    ncount_checker(ncount,ncount_check);
#endif
  //int nvar=df1b2variable::nvar;
  ADUNCONST(df1b2variable*,px)
  fixed_smartlist & nlist=f1b2gradlist->nlist;
  test_smartlist& list=f1b2gradlist->list;

  size_t total_bytes=sizeof(df1b2_header)+sizeof(df1b2_header);
#if defined(SAFE_ALL)
  char ids[]="JK";
  int slen=strlen(ids);
  total_bytes+=slen;
#endif
  list.check_buffer_size(total_bytes);
  void * tmpptr=list.bptr;
#if defined(SAFE_ALL)
  memcpy(list,ids,slen);
#endif

  memcpy(list,(df1b2_header*)(px),sizeof(df1b2_header));
  memcpy(list,(df1b2_header*)(pz),sizeof(df1b2_header));

  // ***** write  record size
  nlist.bptr->numbytes=adptr_diff(list.bptr,tmpptr);
  nlist.bptr->pf=(ADrfptr)(&ad_read_pass1_plus_eq);
  ++nlist;
  return 0;
}

/**
 * Description not yet available.
 * \param
 */
void ad_read_pass1_plus_eq(void)
{
  switch(df1b2variable::passnumber)
  {
  case 1:
    read_pass1_plus_eq_1();
    break;
  case 2:
    read_pass1_plus_eq_2();
    break;
  case 3:
    read_pass1_plus_eq_3();
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
void read_pass1_plus_eq_1(void)
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
#if defined(SAFE_ALL)
  checkidentiferstring("JK",list);
#endif

  // get info from tape1
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;

  list.restoreposition(); // save pointer to beginning of record;

  // Do first reverse paSS calculations
  // ****************************************************************
  // turn this off if no third derivatives are calculated
  // if (!no_third_derivatives)
  // {
  // save for second reverse pass
  // save identifier 1
  //   fixed_smartlist2& nlist2=f1b2gradlist->nlist2;
  //   test_smartlist& list2=f1b2gradlist->list2;
  //int total_bytes=2*nvar*sizeof(double);
// string identifier debug stuff
#if defined(SAFE_ALL)
  //char ids[]="IL";
  //int slen=strlen(ids);
  //total_bytes+=slen;
#endif
  //list2.check_buffer_size(total_bytes);
  //void * tmpptr2=list2.bptr;
#if defined(SAFE_ALL)
  //memcpy(list2,ids,slen);
#endif
     //memcpy(list2,pz->get_u_bar(),nvar*sizeof(double));
     //memcpy(list2,pz->get_u_dot_bar(),nvar*sizeof(double));
     //*nlist2.bptr=adptr_diff(list2.bptr,tmpptr2);
     //nlist2++;
  // }
  //
  // ****************************************************************
#if defined(PRINT_DERS)
 print_derivatives(px,"x");
 print_derivatives(pz,"z");
#endif

  double* pz_u_bar = pz->u_bar;
  double* px_u_bar = px->u_bar;
  for (unsigned int i = 0; i < nvar; ++i)
  {
    *px_u_bar += *pz_u_bar;
    ++pz_u_bar;
    ++px_u_bar;
  }
  double* pz_u_dot_bar = pz->u_dot_bar;
  double* px_u_dot_bar = px->u_dot_bar;
  for (unsigned int i = 0; i < nvar; ++i)
  {
    *px_u_dot_bar += *pz_u_dot_bar;
    ++pz_u_dot_bar;
    ++px_u_dot_bar;
  }
#if defined(PRINT_DERS)
 print_derivatives(px,"x");
 print_derivatives(pz,"z");
#endif
}

/**
 * Description not yet available.
 * \param
 */
void read_pass1_plus_eq_2(void)
{
  //const int nlist_record_size=sizeof(int)+sizeof(char*);
  // We are going forward for bptr and backword for bptr2
  //
  // list 1
  //
  unsigned int nvar=df1b2variable::nvar;
  test_smartlist & list=f1b2gradlist->list;

  size_t total_bytes=sizeof(df1b2_header)+sizeof(df1b2_header);
#if defined(SAFE_ALL)
  char ids[]="JK";
  int slen=strlen(ids);
  total_bytes+=slen;
#endif
  list.check_buffer_size(total_bytes);

  list.saveposition(); // save pointer to beginning of record;
  fixed_smartlist & nlist=f1b2gradlist->nlist;
   // nlist-=sizeof(int);
  // get record size
  int num_bytes=nlist.bptr->numbytes;
    // nlist+=nlist_record_size;
  //
  // list 2
  //
  //test_smartlist & list2=f1b2gradlist->list2;
  //fixed_smartlist2 & nlist2=f1b2gradlist->nlist2;
  // get record size
  //int num_bytes2=*nlist2.bptr;
  //nlist2--;
  // backup the size of the record
  //list2-=num_bytes2;
  //list2.saveposition(); // save pointer to beginning of record;
  // save the pointer to the beginning of the record
  // bptr and bptr2 now both point to the beginning of their records
#if defined(SAFE_ALL)
  checkidentiferstring("JK",list);
  //checkidentiferstring("IL",list2);
#endif

  // get info from tape1
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;

  list.restoreposition(num_bytes); // save pointer to beginning of record;

  //double * zbar=(double*)list2.bptr;
  //double * zdotbar=(double*)(list2.bptr+nvar*sizeof(double));

  double * x_bar_tilde=px->get_u_bar_tilde();
  double * x_dot_bar_tilde=px->get_u_dot_bar_tilde();
  double * z_bar_tilde=pz->get_u_bar_tilde();
  double * z_dot_bar_tilde=pz->get_u_dot_bar_tilde();
  // Do second "reverse-reverse" pass calculations
  for (unsigned int i = 0; i < nvar; ++i)
  {
    *z_bar_tilde += *x_bar_tilde;
    ++z_bar_tilde;
    ++x_bar_tilde;
  }
  for (unsigned int i = 0; i < nvar; ++i)
  {
    *z_dot_bar_tilde += *x_dot_bar_tilde;
    ++x_dot_bar_tilde;
    ++z_dot_bar_tilde;
  }
  //list2.restoreposition(); // save pointer to beginning of record;
#if defined(PRINT_DERS)
 print_derivatives(px,"x");
 print_derivatives(pz,"z");
#endif
}

/**
 * Description not yet available.
 * \param
 */
void read_pass1_plus_eq_3(void)
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

#if defined(SAFE_ALL)
  checkidentiferstring("JK",list);
#endif
  // get info from tape1
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;

  list.restoreposition(); // save pointer to beginning of record;

  *(px->u_tilde)+=*pz->u_tilde;

  double* px_u_dot_tilde = px->u_dot_tilde;
  double* pz_u_dot_tilde = pz->u_dot_tilde;
  for (unsigned int i = 0; i < nvar; ++i)
  {
    *px_u_dot_tilde += *pz_u_dot_tilde;
    ++pz_u_dot_tilde;
    ++px_u_dot_tilde;
  }
#if defined(PRINT_DERS)
 print_derivatives(px,"x");
 print_derivatives(pz,"z");
#endif
}

/**
 * Description not yet available.
 * \param
 */
df1b2variable fabs(const df1b2variable& x)
{
  if (value(x)>=0.0)
    return x;
  else
    return -x;
}

/**
 * Description not yet available.
 * \param
 */
df1b2vector fabs(const df1b2vector& t1)
{
   df1b2vector tmp(t1.indexmin(),t1.indexmax());
   for (int i=t1.indexmin(); i<=t1.indexmax(); i++)
   {
     tmp(i)=fabs(t1(i));
   }

   return(tmp);
}

/**
 * Description not yet available.
 * \param
 */
df1b2variable max(const df1b2vector& t1)
{
   df1b2variable tmp;
   int mmin=t1.indexmin();
   int mmax=t1.indexmax();
   tmp=t1(mmin);
   for (int i=mmin+1; i<=mmax; i++)
   {
     if (value(tmp)<value(t1(i))) tmp=t1(i);
   }
   return(tmp);
}
