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
void dvdv_sub(void);

/**
 * Description not yet available.
 * \param
 */
dvar_vector operator-(const dvar_vector& v1, const dvar_vector& v2)
{
  if (v1.indexmin()!=v2.indexmin()||v1.indexmax()!=v2.indexmax())
  {
    cerr << "Incompatible bounds in "
    "prevariable operator-(const dvar_vector& v1,const dvar_vector& v2)"
    << endl;
    ad_exit(1);
  }
  //dvector cv1=value(v1);
  //dvector cv2=value(v2);
  kkludge_object kkk;
  dvar_vector vtmp(v1.indexmin(),v1.indexmax(),kkk);
  for (int i=v1.indexmin();i<=v1.indexmax();i++)
  {
    vtmp.elem_value(i)=v1.elem_value(i)-v2.elem_value(i);
  }

  //dvar_vector vtmp=nograd_assign(tmp);

  grad_stack* GRAD_STACK1 = gradient_structure::GRAD_STACK1;
  DF_FILE* fp = gradient_structure::fp;
  // The derivative list considerations
  save_identifier_string("bbbb");
  fp->save_dvar_vector_position(v1);
  fp->save_dvar_vector_position(v2);
  fp->save_dvar_vector_position(vtmp);
  save_identifier_string("aaaa");
  GRAD_STACK1->set_gradient_stack(dvdv_sub);
  return vtmp;
}

/**
 * Description not yet available.
 * \param
 */
void dvdv_sub(void)
{
  DF_FILE* fp = gradient_structure::fp;

  // int ierr=fsetpos(gradient_structure::get_fp(),&filepos);
  verify_identifier_string("aaaa");
  dvar_vector_position tmp_pos=fp->restore_dvar_vector_position();
  dvector dftmp=restore_dvar_vector_derivatives(tmp_pos);
  dvar_vector_position v2pos=fp->restore_dvar_vector_position();
  dvar_vector_position v1pos=fp->restore_dvar_vector_position();
  verify_identifier_string("bbbb");
  dvector dfv1(dftmp.indexmin(),dftmp.indexmax());
  dvector dfv2(dftmp.indexmin(),dftmp.indexmax());
  for (int i=dftmp.indexmin();i<=dftmp.indexmax();i++)
  {
    //vtmp.elem(i)=value(v1.elem(i))+value(v2.elem(i));
    dfv1.elem(i)=dftmp.elem(i);
    dfv2.elem(i)=-dftmp.elem(i);
  }
  dfv1.save_dvector_derivatives(v1pos);
  dfv2.save_dvector_derivatives(v2pos);
  //ierr=fsetpos(gradient_structure::get_fp(),&filepos);
}
