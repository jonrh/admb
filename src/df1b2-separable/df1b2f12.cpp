/**
 * Author: David Fournier
 * Copyright (c) 2008-2012 Regents of the University of California
 */
#include <df1b2fun.h>
#ifndef OPT_LIB
  #include <cassert>
  #include <climits>
#endif

void ad_read_pass2(void);

/**
 * Description not yet available.
 * \param
 */
 int df1b2_gradlist::write_pass1(const df1b2variable * _px,
   const df1b2variable * _py,df1b2variable * pz,df1b2function2 * pf)
 {
   ADUNCONST(df1b2variable*,px)
   ADUNCONST(df1b2variable*,py)
   ncount++;
#if defined(CHECK_COUNT)
  if (ncount >= ncount_check)
    ncount_checker(ncount,ncount_check);
#endif

  unsigned int nvar=df1b2variable::nvar;

   size_t total_bytes=3*sizeof(df1b2_header)+sizeof(char*)
     +2*(nvar+1)*sizeof(double);
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="BY";
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
   memcpy(list,&pf,sizeof(char *));
   //*(char**)(list.bptr)=(char*)pf;
   memcpy(list,px->get_u(),sizeof(double));
   memcpy(list,py->get_u(),sizeof(double));
   memcpy(list,px->get_u_dot(),nvar*sizeof(double));
   memcpy(list,py->get_u_dot(),nvar*sizeof(double));
   // ***** write  record size
   nlist.bptr->numbytes=adptr_diff(list.bptr,tmpptr);
   nlist.bptr->pf=(ADrfptr)(&ad_read_pass2);
   ++nlist;
   return 0;
 }


void read_pass2_1(void);
void read_pass2_2(void);
void read_pass2_3(void);

/**
 * Description not yet available.
 * \param
 */
void ad_read_pass2(void)
{
  switch(df1b2variable::passnumber)
  {
  case 1:
    read_pass2_1();
    break;
  case 2:
    read_pass2_2();
    break;
  case 3:
    read_pass2_3();
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
void read_pass2_1(void)
{
  // We are going backword for bptr and nbptr
  // and  forward for bptr2 and nbptr2
  // the current entry+2 in bptr is the size of the record i.e
  // points to the next record
  //char * bptr=f1b2gradlist->bptr;
  //char * bptr2=f1b2gradlist2->bptr;

  unsigned int nvar=df1b2variable::nvar;

  test_smartlist& list=f1b2gradlist->list;
  //f1b2gradlist->nlist-=sizeof(int);
  int num_bytes=f1b2gradlist->nlist.bptr->numbytes;
  list-=num_bytes;
  list.saveposition(); // save pointer to beginning of record;
  double xu,yu;
  //ad_dstar xdot,ydot;
  df1b2function2 * pf;

  // get info from tape1
#if defined(SAFE_ARRAYS)
  checkidentiferstring("BY",f1b2gradlist->list);
#endif
  char * bptr=f1b2gradlist->list.bptr;
  df1b2_header * px=(df1b2_header *) bptr;
  bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) bptr;
  bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) bptr;
  bptr+=sizeof(df1b2_header);
  pf=*(df1b2function2 **) bptr;
  bptr+=sizeof(char*);
  memcpy(&xu,bptr,sizeof(double));
  bptr+=sizeof(double);
  memcpy(&yu,bptr,sizeof(double));
  bptr+=sizeof(double);
  double * xdot=(double*)bptr;
  bptr+=nvar*sizeof(double);
  double * ydot=(double*)bptr;

  list.restoreposition(); // save pointer to beginning of record;

  // ****************************************************************
  // turn this off if no third derivatives are calculated
  // if (!no_third_derivatives)
  // {
  // save for second reverse pass
  // save identifier 1
     test_smartlist & list2 = f1b2gradlist->list2;


   size_t total_bytes=2*nvar*sizeof(double);
// string identifier debug stuff
#if defined(SAFE_ALL)
  char ids[]="FW";
  int slen=strlen(ids);
  total_bytes+=slen;
#endif
  list2.check_buffer_size(total_bytes);
  void * tmpptr=list2.bptr;
#if defined(SAFE_ALL)
  memcpy(list2,ids,slen);
#endif

     fixed_smartlist2 & nlist2 = f1b2gradlist->nlist2;
     memcpy(list2,pz->get_u_bar(),nvar*sizeof(double));
     memcpy(list2,pz->get_u_dot_bar(),nvar*sizeof(double));
     *nlist2.bptr=adptr_diff(list2.bptr,tmpptr);
     ++nlist2;
  // }
  //
  // ****************************************************************
#if defined(PRINT_DERS)
 print_derivatives(pf->funname,(pf->f)(xu,yu),(pf->df1)(xu,yu),
  (pf->df2)(xu,yu),(pf->d2f11)(xu,yu),(pf->d2f12)(xu,yu),(pf->d2f22)(xu,yu),
  (pf->d3f111)(xu,yu),(pf->d3f112)(xu,yu),(pf->d3f122)(xu,yu),
  (pf->d3f222)(xu,yu),1);
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
 print_derivatives(py,"y");
#endif
#if defined(__DERCHECK__)
  if (derchecker)
  if (derchecker->node_number)
  {
    if (derchecker->counter == derchecker->node_number)
    {
      switch (derchecker->pass_number) // increment the variable of interest
      {
      case 2:
        switch(derchecker->vartype)
        {
        case 1:
          if (!derchecker->dotflag)
            px->u_bar[derchecker->index-1]+=derchecker->delta;
          else
            px->u_dot_bar[derchecker->index-1]+=derchecker->delta;
          break;
        case 2:
          if (!derchecker->dotflag)
            py->u_bar[derchecker->index-1]+=derchecker->delta;
          else
            py->u_dot_bar[derchecker->index-1]+=derchecker->delta;
          break;
        case 3:
          if (!derchecker->dotflag)
            pz->u_bar[derchecker->index-1]+=derchecker->delta;
          else
            pz->u_dot_bar[derchecker->index-1]+=derchecker->delta;
          break;
        default:
          cerr << "Invalid index value for dercheck_index was "
               << derchecker->index << endl;
          break;
        }
        break;
      case 3:
        switch(derchecker->vartype)
        {
        case 1:
          if (!derchecker->dotflag)
            px->u_bar[derchecker->index-1]-=derchecker->delta;
          else
            px->u_dot_bar[derchecker->index-1]-=derchecker->delta;
          break;
        case 2:
          if (!derchecker->dotflag)
            py->u_bar[derchecker->index-1]-=derchecker->delta;
          else
            py->u_dot_bar[derchecker->index-1]-=derchecker->delta;
          break;
        case 3:
          if (!derchecker->dotflag)
            pz->u_bar[derchecker->index-1]-=derchecker->delta;
          else
            pz->u_dot_bar[derchecker->index-1]-=derchecker->delta;
          break;
        default:
          cerr << "Invalid index value for dercheck_index was "
               << derchecker->index << endl;
          break;
        }
        break;
      }
    }
  }
#endif

  // Do first reverse pass calculations
  for (size_t i=0;i<nvar;i++)
  {
    //double df1=(pf->df1)(xu,yu);
    px->u_bar[i]+=(pf->df1)(xu,yu)*pz->u_bar[i];
  }
  for (size_t i=0;i<nvar;i++)
  {
    py->u_bar[i]+=(pf->df2)(xu,yu)*pz->u_bar[i];
  }
  for (size_t i=0;i<nvar;i++)
  {
    px->u_bar[i]+=(pf->d2f11)(xu,yu)*xdot[i]*pz->u_dot_bar[i];
    px->u_bar[i]+=(pf->d2f12)(xu,yu)*ydot[i]*pz->u_dot_bar[i];
#if defined(ADDEBUG_PRINT)
    cout << px->u_bar[i] << " " << pz->u_dot_bar[i] << " " << addebug_count
         << endl;
#endif
  }
  for (size_t i=0;i<nvar;i++)
  {
    //py->u_bar[i]+=(pf->d2f22)(*(px->u),*(py->u))*ydot[i]*pz->u_dot_bar[i];
    //py->u_bar[i]+=(pf->d2f12)(*(px->u),*(py->u))*xdot[i]*pz->u_dot_bar[i];
    py->u_bar[i]+=(pf->d2f22)(xu,yu)*ydot[i]*pz->u_dot_bar[i];
    py->u_bar[i]+=(pf->d2f12)(xu,yu)*xdot[i]*pz->u_dot_bar[i];
#if defined(ADDEBUG_PRINT)
    cout << py->u_bar[i] << " " << pz->u_dot_bar[i] << " " << addebug_count
         << endl;
#endif
  }
  for (size_t i=0;i<nvar;i++)
  {
    //px->u_dot_bar[i]+=(pf->df1)(*(px->u),*(py->u))*pz->u_dot_bar[i];
    px->u_dot_bar[i]+=(pf->df1)(xu,yu)*pz->u_dot_bar[i];
#if defined(ADDEBUG_PRINT)
    cout << px->u_dot_bar[i] << " " << addebug_count << endl;
    cout << pz->u_dot_bar[i] << " " << addebug_count << endl;
#endif
  }
  for (size_t i=0;i<nvar;i++)
  {
    py->u_dot_bar[i]+=(pf->df2)(xu,yu)*pz->u_dot_bar[i];
#if defined(ADDEBUG_PRINT)
    cout << py->u_dot_bar[i] << " " << addebug_count << endl;
    cout << pz->u_dot_bar[i] << " " << addebug_count << endl;
#endif
  }

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  for (size_t i=0;i<nvar;i++)
  {
    pz->u_bar[i]=0;
  }
  for (size_t i=0;i<nvar;i++)
  {
    pz->u_dot_bar[i]=0;
  }

#if defined(PRINT_DERS)
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
 print_derivatives(py,"y");
#endif
}

/**
 * Description not yet available.
 * \param
 */
void read_pass2_2(void)
{
  //const int nlist_record_size=sizeof(int)+sizeof(char*);
  // We are going forward for bptr and backword for bptr2
  //
  // list 1
  //

  unsigned int nvar=df1b2variable::nvar;

  test_smartlist & list=f1b2gradlist->list;

  size_t total_bytes=3*sizeof(df1b2_header)+sizeof(char*)
    +2*(nvar+1)*sizeof(double);
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
  list2.saveposition(); // save pointer to beginning of record;
  // save the pointer to the beginning of the record
  // bptr and bptr2 now both point to the beginning of their records

  double xu,yu;
  double * xdot;
  double * ydot;
  //df1b2_header x,z;
  df1b2function2 * pf;

  // get info from tape1
  // get info from tape1
#if defined(SAFE_ARRAYS)
  checkidentiferstring("BY",list);
  checkidentiferstring("FW",list2);
#endif
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  pf=*(df1b2function2 **) list.bptr;
  list.bptr+=sizeof(char*);
  memcpy(&xu,list.bptr,sizeof(double));
  list.bptr+=sizeof(double);
  memcpy(&yu,list.bptr,sizeof(double));
  list.bptr+=sizeof(double);
  xdot=(double*)list.bptr;
  list.bptr+=nvar*sizeof(double);
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
#if defined(PRINT_DERS)
 print_derivatives(pf->funname,(pf->f)(xu,yu),(pf->df1)(xu,yu),
  (pf->df2)(xu,yu),(pf->d2f11)(xu,yu),(pf->d2f12)(xu,yu),(pf->d2f22)(xu,yu),
  (pf->d3f111)(xu,yu),(pf->d3f112)(xu,yu),(pf->d3f122)(xu,yu),
  (pf->d3f222)(xu,yu),1);
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
 print_derivatives(py,"y");
#endif

  for (size_t i=0;i<nvar;i++)
  {
    z_bar_tilde[i]=0;
    z_dot_bar_tilde[i]=0;
  }

  // start with x and add y
  for (size_t i=0;i<nvar;i++)
  {
    *x_tilde+=(pf->d2f11)(xu,yu)*zbar[i]*x_bar_tilde[i];
    z_bar_tilde[i]+=(pf->df1)(xu,yu)*x_bar_tilde[i];
    *y_tilde+=(pf->d2f12)(xu,yu)*zbar[i]*x_bar_tilde[i];
  }

  for (size_t i=0;i<nvar;i++)
  {
    *x_tilde+=(pf->d2f11)(xu,yu)*zdotbar[i]*x_dot_bar_tilde[i];
    *y_tilde+=(pf->d2f12)(xu,yu)*zdotbar[i]*x_dot_bar_tilde[i];
    z_dot_bar_tilde[i]+=(pf->df1)(xu,yu)*x_dot_bar_tilde[i];
  }

  for (size_t i=0;i<nvar;i++)
  {
    x_dot_tilde[i]+=(pf->d2f11)(xu,yu)*zdotbar[i]*x_bar_tilde[i];
    z_dot_bar_tilde[i]+=(pf->d2f11)(xu,yu)*xdot[i]*x_bar_tilde[i];
    *x_tilde+=(pf->d3f111)(xu,yu)*xdot[i]*zdotbar[i]*x_bar_tilde[i];
    *y_tilde+=(pf->d3f112)(xu,yu)*xdot[i]*zdotbar[i]*x_bar_tilde[i];
  }
  // start with y and add x
  for (size_t i=0;i<nvar;i++)
  {
    *y_tilde+=(pf->d2f22)(xu,yu)*zbar[i]*y_bar_tilde[i];
    *x_tilde+=(pf->d2f12)(xu,yu)*zbar[i]*y_bar_tilde[i];
    z_bar_tilde[i]+=(pf->df2)(xu,yu)*y_bar_tilde[i];
  }

  for (size_t i=0;i<nvar;i++)
  {
    *y_tilde+=(pf->d2f22)(xu,yu)*zdotbar[i]*y_dot_bar_tilde[i];
    *x_tilde+=(pf->d2f12)(xu,yu)*zdotbar[i]*y_dot_bar_tilde[i];
    z_dot_bar_tilde[i]+=(pf->df2)(xu,yu)*y_dot_bar_tilde[i];
  }

  for (size_t i=0;i<nvar;i++)
  {
    y_dot_tilde[i]+=(pf->d2f22)(xu,yu)*zdotbar[i]*y_bar_tilde[i];
    z_dot_bar_tilde[i]+=(pf->d2f22)(xu,yu)*ydot[i]*y_bar_tilde[i];
    *y_tilde+=(pf->d3f222)(xu,yu)*ydot[i]*zdotbar[i]*y_bar_tilde[i];
    *x_tilde+=(pf->d3f122)(xu,yu)*ydot[i]*zdotbar[i]*y_bar_tilde[i];
  }
  for (size_t i=0;i<nvar;i++)
  {
    *x_tilde+=(pf->d3f112)(xu,yu)*ydot[i]*zdotbar[i]*x_bar_tilde[i];
    *y_tilde+=(pf->d3f122)(xu,yu)*ydot[i]*zdotbar[i]*x_bar_tilde[i];
    y_dot_tilde[i]+=(pf->d2f12)(xu,yu)*zdotbar[i]*x_bar_tilde[i];
    z_dot_bar_tilde[i]+=(pf->d2f12)(xu,yu)*ydot[i]*x_bar_tilde[i];
  }
  for (size_t i=0;i<nvar;i++)
  {
    *x_tilde+=(pf->d3f112)(xu,yu)*xdot[i]*zdotbar[i]*y_bar_tilde[i];
    *y_tilde+=(pf->d3f122)(xu,yu)*xdot[i]*zdotbar[i]*y_bar_tilde[i];
    x_dot_tilde[i]+=(pf->d2f12)(xu,yu)*zdotbar[i]*y_bar_tilde[i];
    z_dot_bar_tilde[i]+=(pf->d2f12)(xu,yu)*xdot[i]*y_bar_tilde[i];
  }
#if defined(__DERCHECK__)
  if (derchecker->node_number)
  {
    if (derchecker->counter == derchecker->node_number)
    {
      if (derchecker->pass_number==1) // increment the variable of interest
      {
        switch(derchecker->vartype)
        {
        case 1:
          if (!derchecker->dotflag)
            derchecker->der_value=
              px->u_bar_tilde[derchecker->index-1];
          else
            derchecker->der_value=
              px->u_dot_bar_tilde[derchecker->index-1];
          break;
        case 2:
          if (!derchecker->dotflag)
            derchecker->der_value=
              py->u_bar_tilde[derchecker->index-1];
          else
            derchecker->der_value=
              py->u_dot_bar_tilde[derchecker->index-1];
          break;
        case 3:
          if (!derchecker->dotflag)
            derchecker->der_value=
              pz->u_bar_tilde[derchecker->index-1];
          else
            derchecker->der_value=
              pz->u_dot_bar_tilde[derchecker->index-1];
          break;
        default:
          cerr << "Invalid index value for dercheck_index was "
               << derchecker->index << endl;
        }
      }
    }
  }
#endif
#if defined(PRINT_DERS)
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
 print_derivatives(py,"y");
#endif
}

/**
 * Description not yet available.
 * \param
 */
void read_pass2_3(void)
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
  df1b2function2 * pf;

  // get info from tape1
  // get info from tape1
#if defined(SAFE_ARRAYS)
  checkidentiferstring("BY",list);
#endif
  df1b2_header * px=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * py=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  df1b2_header * pz=(df1b2_header *) list.bptr;
  list.bptr+=sizeof(df1b2_header);
  pf=*(df1b2function2 **) list.bptr;
  list.bptr+=sizeof(char*);
  memcpy(&xu,list.bptr,sizeof(double));
  list.bptr+=sizeof(double);
  memcpy(&yu,list.bptr,sizeof(double));
  list.bptr+=sizeof(double);
  xdot=(double*)list.bptr;
  list.bptr+=nvar*sizeof(double);
  ydot=(double*)list.bptr;
  list.restoreposition(); // save pointer to beginning of record;
#if defined(PRINT_DERS)
 print_derivatives(pf->funname,(pf->f)(xu,yu),(pf->df1)(xu,yu),
  (pf->df2)(xu,yu),(pf->d2f11)(xu,yu),(pf->d2f12)(xu,yu),(pf->d2f22)(xu,yu),
  (pf->d3f111)(xu,yu),(pf->d3f112)(xu,yu),(pf->d3f122)(xu,yu),
  (pf->d3f222)(xu,yu),1);
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
 print_derivatives(py,"y");
#endif

  *(px->u_tilde)+=(pf->df1)(xu,yu)* *(pz->u_tilde);
  *(py->u_tilde)+=(pf->df2)(xu,yu)* *(pz->u_tilde);
  for (size_t i=0;i<nvar;i++)
  {
    *(px->u_tilde)+=(pf->d2f11)(xu,yu)*xdot[i]*pz->u_dot_tilde[i];
    *(py->u_tilde)+=(pf->d2f12)(xu,yu)*xdot[i]*pz->u_dot_tilde[i];
    *(py->u_tilde)+=(pf->d2f22)(xu,yu)*ydot[i]*pz->u_dot_tilde[i];
    *(px->u_tilde)+=(pf->d2f12)(xu,yu)*ydot[i]*pz->u_dot_tilde[i];
  }
  for (size_t i=0;i<nvar;i++)
  {
    px->u_dot_tilde[i]+=(pf->df1)(xu,yu)*pz->u_dot_tilde[i];
    py->u_dot_tilde[i]+=(pf->df2)(xu,yu)*pz->u_dot_tilde[i];
  }
  *(pz->u_tilde)=0;
  for (size_t i=0;i<nvar;i++)
  {
    pz->u_dot_tilde[i]=0;
  }
#if defined(PRINT_DERS)
 print_derivatives(pz,"z");
 print_derivatives(px,"x");
 print_derivatives(py,"y");
#endif
}
