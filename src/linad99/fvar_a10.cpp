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
#include "fvar.hpp"

#ifdef __ZTC__
  #include <iostream.hpp>
  #if (__ZTC__ < 0x310)
    #include <sstream.hpp>
  #else
    #include <strstream.h>
  #endif
#endif

#if defined(__TURBOC__)
   #include <iostream.h>
   #include <strstrea.h>
#endif

#ifdef __SUN__
   #include <iostream.h>
  #include <strstream.h>
  #define __USE_IOSTREAM__
#endif

#ifdef __NDPX__
   #include <iostream.h>
   #include <sstream.h>
#endif

#include <string.h>
#include <ctype.h>

#ifndef OPT_LIB
  #include <cassert>
  #include <climits>
#endif

const int MAX_FIELD_LENGTH = 500;

/**
Fill variable vector from values in string s.
\param s should be in {v1, ..., vn} format.
*/
void dvar_vector::fill(const char * s)
{
  const size_t len = strlen(s);
#ifndef OPT_LIB
  assert(len <= INT_MAX);
#endif
  char *t = new char[len];
  const int n = static_cast<int>(len);
  int lbraces = 0;
  int rbraces = 0;
  unsigned int commas  = 0;

  for (int k = 0; k < n; k++)
  {
    if (s[k] == '{')
    {
      lbraces ++;
      t[k] = ' ';
    }
    else if (s[k] == '}')
    {
      rbraces ++;
      t[k] = ' ';
    }
    else if (s[k] == ',')
    {
      commas ++;
      t[k] = ' ';
    }
    else
    {
      t[k] = s[k];
    }
  }

  if (lbraces == 1 && rbraces == 1)
  {
    unsigned int nch = commas + 1;

    if (nch != size())
    {
      if (nch < size())
      {
        cerr << "Not enough elements to fill vector in "
        "dvar_vector::fill(const char * s)\n";
        cerr << s << "\n";
        ad_exit(1);
      }
      else
      {
        cerr << "Too many elements for size of vector in "
        "dvar_vector::fill(const char * s)\n";
        cerr << s << "\n";
        ad_exit(1);
      }
    }
//   char * field = (char *) new[size_t(MAX_FIELD_LENGTH+1)];
   char* field = new char[size_t(MAX_FIELD_LENGTH+1)];
   char* err_ptr = NULL;

   size_t index = 0;
   size_t length = strlen(t);
   for (int i=indexmin();i<=indexmax();i++)
   {
     char c = t[index];
     while (c == ' ')
     {
       ++index;
       if (index >= length) break;

       c = t[index];
     }
     int field_index = 0;
     while (c != ' ')
     {
       field[field_index] = c;
       ++field_index;

       ++index;
       if (index >= length) break;

       c = t[index];
     }
     field[field_index] = '\0';
     elem(i)=strtod(field,&err_ptr); // increment column counter

     if (isalpha((unsigned char)err_ptr[0]))
     {
       cerr << "Error decoding field "
         << " in dmatrix::dmatrix(char * filename) " << "\n";
       cerr << "Error occurred at element " << i << "\n";
       cerr << "Offending characters start with "
           << err_ptr[0]
           << err_ptr[1]
           << err_ptr[2]
           << err_ptr[3] << "\n";
       ad_exit(1);
     }
     if (elem(i) == HUGE_VAL || elem(i) == -HUGE_VAL)
     {
       cerr << "Overflow Error decoding field "
           " in dmatrix::dmatrix(char * ) " << "\n";
       cerr << "Error occurred at element " << i << "\n";
       ad_exit(1);
     }
   }
   delete [] field;
   field = NULL;
  }
  else
  {
    delete [] t;
    t = NULL;

    if (lbraces != rbraces)
    {
      cerr << "Unbalanced braces in dvar_vector::fill(const char * s)\n";
      cerr << s << "\n";
      ad_exit(1);
    }
    if (lbraces > 1)
    {
      cerr << "Only one level of braces allowed in "
      "dvar_vector::fill(const char * s)\n";
      cerr << s << "\n";
      ad_exit(1);
    }
    if (lbraces == 0)
    {
      cerr << "Missing braces { ... } in dvar_vector::fill(const char * s)\n";
      cerr << s << "\n";
      ad_exit(1);
    }
  }
  delete [] t;
  t = NULL;
}
