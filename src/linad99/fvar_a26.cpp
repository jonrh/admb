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

void dv_minuseq(void);

/**
 * Description not yet available.
 * \param
 */
 dvar_vector& dvar_vector::operator-=(const dvar_vector& v1)
 {
   if (indexmin() != v1.indexmin() || indexmax() != v1.indexmax())
   {
     cerr << " Incompatible array bounds in "
     "dvector& operator += (const dvar_vector&)\n";
     ad_exit(21);
   }

   {
     for (int i=indexmin();i<=indexmax();i++)
     {
       elem_value(i) -= v1.elem_value(i);
     }
   }
   grad_stack* GRAD_STACK1 = gradient_structure::GRAD_STACK1;
   DF_FILE* fp = gradient_structure::fp;
   save_identifier_string("uuvv");
   fp->save_dvar_vector_position(*this);  // for this->
   fp->save_dvar_vector_position(v1);
   save_identifier_string("wwxx");
   GRAD_STACK1->set_gradient_stack(dv_minuseq);

   return *this;
 }

/**
 * Description not yet available.
 * \param
 */
void dv_minuseq(void)
{
  DF_FILE* fp = gradient_structure::fp;

  // int ierr=fsetpos(gradient_structure::get_fp(),&filepos);
  verify_identifier_string("wwxx");
  dvar_vector_position v1_pos=fp->restore_dvar_vector_position();
  dvar_vector_position this_pos=fp->restore_dvar_vector_position();
  verify_identifier_string("uuvv");
  dvector dfthis=-1.*restore_dvar_vector_der_nozero(this_pos);
  dfthis.save_dvector_derivatives(v1_pos);
}

/**
 * Description not yet available.
 * \param
 */
dvar_vector& dvar_vector::operator-=(const dvector& v1)
 {
   if (indexmin() != v1.indexmin() || indexmax() != v1.indexmax())
   {
     cerr << " Incompatible array bounds in "
     "dvector& operator += (const dvar_vector&)\n";
     ad_exit(21);
   }

   {
     for (int i=indexmin();i<=indexmax();i++)
     {
       elem_value(i) -= v1.elem(i);
     }
   }
   return(*this);
 }
