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

#ifndef OPT_LIB
  #include <cassert>
  #include <climits>
#endif

void dv_subassign(void);

/**
 * Description not yet available.
 * \param
 */
dvar_vector dvar_vector::operator()(const ivector& u)
 {
   dvar_vector tmp(u.indexmin(),u.indexmax());

   for (int i=u.indexmin();i<=u.indexmax();i++)
   {
     tmp.elem_value(i)=elem_value(u(i));
   }

   grad_stack* GRAD_STACK1 = gradient_structure::GRAD_STACK1;
   DF_FILE* fp = gradient_structure::fp;
   save_identifier_string("by");
   fp->save_dvar_vector_position(*this);
   fp->save_dvar_vector_position(tmp);
   fp->save_ivector_value(u);
   fp->save_ivector_position(u);
   save_identifier_string("ay");
   GRAD_STACK1->set_gradient_stack(dv_subassign);

   return tmp;
 }

/**
 * Description not yet available.
 * \param
 */
void dv_subassign()
{
  DF_FILE* fp = gradient_structure::fp;

  // int ierr=fsetpos(gradient_structure::get_fp(),&filepos);
  verify_identifier_string("ay");
  ivector_position u_pos=fp->restore_ivector_position();
  ivector u=restore_ivector_value(u_pos);
  dvar_vector_position tmp_pos=fp->restore_dvar_vector_position();
  dvector dftmp=restore_dvar_vector_derivatives(tmp_pos);
  dvar_vector_position t_pos=fp->restore_dvar_vector_position();
  dvector dft(t_pos.indexmin(),t_pos.indexmax());
  verify_identifier_string("by");
  dft.initialize();
  int mmin=dftmp.indexmin();
  int mmax=dftmp.indexmax();
  for (int i=mmin;i<=mmax;i++)
  {
     //tmp.elem_value(i)=this->elem_value(u(i));
     dft.elem(u(i))+=dftmp.elem(i);
  }
  dft.save_dvector_derivatives(t_pos);
}

/**
 * Description not yet available.
 * \param
 */
dvar_vector dvar_vector::operator()(const lvector& u)
 {
   dvar_vector tmp(u.indexmin(),u.indexmax());
   for ( int i=u.indexmin(); i<=u.indexmax(); i++)
   {
#ifdef OPT_LIB
     tmp(i)=(*this)((int)u(i));
#else
     const AD_LONG_INT ui = u(i);
     assert(ui <= INT_MAX);
     tmp(i)=(*this)((int)ui);
#endif
   }
   return tmp;
 }
