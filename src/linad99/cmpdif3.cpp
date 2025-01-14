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
#include <fvar.hpp>

#ifdef __TURBOC__
  #pragma hdrstop
  #include <iostream.h>
#endif

#ifdef __ZTC__
  #include <iostream.hpp>
#endif

#include <string.h>

#ifndef OPT_LIB
  #include <cassert>
  #include <climits>
  #define CHK_ID_STRING
#endif

//extern ofstream clogf;

/**
 * Description not yet available.
 * \param
 */
void report_gradstack_flag(void)
{
  verify_identifier_string("stack");
  // Back up the stream and read the number of bytes written in the
  // ``write function'' corresponding to this ``read function''
  int num_bytes;
  gradient_structure::fp->fread(&num_bytes,sizeof(int));
  char str1[100];
  str1[0]='\0';
#ifndef OPT_LIB
  assert(num_bytes > 0);
#endif
  gradient_structure::fp->fread(str1,(size_t)num_bytes);
  cout << "in report_gradstack_flag  " << str1 << endl;
}

#if defined(CHK_ID_STRING)
/**
 * Description not yet available.
 * \param
 */
static void report_gradstack_flag2(void)
{
  verify_identifier_string("stack");
  // Back up the stream and read the number of bytes written in the
  // ``write function'' corresponding to this ``read function''
  DF_FILE* fp = gradient_structure::fp;
  int num_bytes;
  fp->fread(&num_bytes,sizeof(int));
  char str1[100];
  str1[0]='\0';
#ifndef OPT_LIB
  assert(num_bytes >= 0);
#endif
  fp->fread(str1, (size_t)num_bytes);
  int i,j;
  fp->fread(&j,sizeof(int));
  fp->fread(&i,sizeof(int));
  cout << "in report_gradstack_flag  " << str1 << endl;
}
#endif

/**
 * Description not yet available.
 * \param
 */
void set_gradstack_flag(char* str)
{
#if defined(CHK_ID_STRING)
  //int wsize=sizeof(char);
#ifdef OPT_LIB
  int length=(int)strlen(str);
#else
  size_t _length = strlen(str);
  assert(_length <= INT_MAX);
  int length=(int)_length;
#endif
#ifndef OPT_LIB
  assert(length >= 0);
#endif
  gradient_structure* gs = gradient_structure::_instance;
  DF_FILE* fp = gradient_structure::fp;
  fp->fwrite(str, (size_t)length);
  fp->fwrite(&length,sizeof(int));
  gs->GRAD_STACK1->set_gradient_stack(report_gradstack_flag);
  save_identifier_string("stack");
#endif
}

/**
 * Description not yet available.
 * \param
 */
void set_gradstack_flag(char* _str,int i,int j)
{
#if defined(CHK_ID_STRING)
  adstring ads=_str;
  ads+=" ";
  ads+=str(i);
  ads+=" ";
  ads+=str(j);
  //int wsize=sizeof(char);
  char * str=(char*)(ads);
#ifdef OPT_LIB
  int length=(int)strlen(str);
#else
  size_t _length = strlen(str);
  assert(_length <= INT_MAX);
  int length=(int)_length;
#endif
  gradient_structure* gs = gradient_structure::_instance;
  DF_FILE* fp = gradient_structure::fp;
  fp->fwrite(&i,sizeof(int));
  fp->fwrite(&j,sizeof(int));
#ifndef OPT_LIB
  assert(length >= 0);
#endif
  fp->fwrite(str, (size_t)length);
  fp->fwrite(&length,sizeof(int));
  gs->GRAD_STACK1->set_gradient_stack(report_gradstack_flag2);
  save_identifier_string("stack");
#endif
}

/**
\ingroup DEVEL
Verifies gradient stack string.
Retrieves a character string from the adjoint code variable stack
and compares it to its argument. Reports and error and exits
if the two strings are not identical. Only available in the "safe"
library. Used primarily for debugging adjoint coide.
\param str1 Character sting to compare with value retrieved from stack.
*/
void verify_identifier_string(const char* str1)
{
#if defined(CHK_ID_STRING)
  // Back up the stream and read the number of bytes written in the
  // ``write function'' corresponding to this ``read function''
  size_t num_bytes=strlen(str1);
  char str[10];
  str[num_bytes]='\0';
  gradient_structure::fp->fread(str,num_bytes);
  //clogf << "in verify_id_string " << str1 << endl;
  if(strcmp(str1,str))
  {
    cerr << "Error reading stack identifer for " << str1 << endl;
    ad_exit(1);
  }
#endif
}

/**
 * Description not yet available.
 * \param
 */
adstring get_string_marker(void)
{
  adstring str1;
#if defined(CHK_ID_STRING)
  // Back up the stream and read the number of bytes written in the
  // ``write function'' corresponding to this ``read function''
  long int num_bytes=5;
  char str[10];
  str[num_bytes]='\0';
#ifndef OPT_LIB
  assert(num_bytes > 0);
#endif
  gradient_structure::fp->fread(str,(size_t)num_bytes);
  //clogf << "in verify_id_string " << str1 << endl;
  str1=str;
#endif
 return str1;
}

/**
 * Description not yet available.
 * \param
 */
void ivector::save_ivector_position() const
{
  gradient_structure::fp->save_ivector_position(*this);
}
void DF_FILE::save_ivector_position(const ivector& v)
{
  // saves the size and address information for a ivector
  constexpr size_t wsize=sizeof(ivector_position);
  ivector_position tmp(v);
  fwrite(&tmp, wsize);
}

/**
 * Description not yet available.
 * \param
 */
void dvar_vector::save_dvar_vector_position() const
{
  gradient_structure::fp->save_dvar_vector_position(*this);
}
void DF_FILE::save_dvar_vector_position(const dvar_vector& v)
{
  // saves the size and address information for a dvar_vector
  constexpr size_t wsize=sizeof(dvar_vector_position);
  dvar_vector_position tmp(v);
  fwrite(&tmp, wsize);
}

/**
 * Description not yet available.
 * \param
 */
void save_ad_pointer(void * p)
{
  // saves the size and address information for a dvar_vector
  size_t wsize=sizeof(void *);
  gradient_structure::fp->fwrite(&p, wsize);
}

/**
 * Description not yet available.
 * \param
 */
void * restore_ad_pointer(void)
{
  void * p=0;
  // saves the size and address information for a dvar_vector
  size_t wsize=sizeof(void *);
  gradient_structure::fp->fread(&p, wsize);
  return p;
}
