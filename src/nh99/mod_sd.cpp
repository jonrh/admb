/*
 * $Id$
 *
 * Author: David Fournier
 * Copyright (c) 2008-2012 Regents of the University of California
 */
#ifndef _MSC_VER
  #include <unistd.h>
#endif
#if !defined(DOS386)
  #define DOS386
#endif

#include <chrono>
#include <df1b2fun.h>
#include <admodel.h>

dmatrix * GAUSS_varcovariance_matrix = NULL;

void  set_gauss_covariance_matrix(const dll_data_matrix& m)
{
  GAUSS_varcovariance_matrix = &((dmatrix&)(m) );
}

void  set_gauss_covariance_matrix(const dmatrix& m)
{
  GAUSS_varcovariance_matrix = &((dmatrix&)(m) );
}

void  set_covariance_matrix(const dll_data_matrix& m)
{
  GAUSS_varcovariance_matrix = &((dmatrix&)(m) );
}

void  set_covariance_matrix(const dmatrix& m)
{
  GAUSS_varcovariance_matrix = &((dmatrix&)(m) );
}

#include <string>
std::string get_elapsed_time(
  const std::chrono::time_point<std::chrono::system_clock>& from,
  const std::chrono::time_point<std::chrono::system_clock>& to)
{
  std::stringstream ss;

  /*
  using namespace std::chrono_literals;
  auto elapsed = 86400000ms * 2 + 3600000ms * 11 + 60000ms * 34 + 1500ms;
  //(2d 11h 34m 1.5s)
  */
  auto elapsed = to - from;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
  auto count = ms.count();

  if (count >= 86400000)
  {
    ss << (count / 86400000) << " d ";
    count %= 86400000;
  }
  if (count >= 3600000)
  {
    ss << (count / 3600000) << " h ";
    count %= 3600000;
  }
  if (count >= 60000)
  {
    ss << (count / 60000) << " m ";
    count %= 60000;
  }
  ss << setprecision(2) << (static_cast<double>(count) * 0.001) << " s";

/*
  double runtime = ( std::clock()-start)/(double) CLOCKS_PER_SEC;
  // Depending on how long it ran convert to sec/min/hour/days so
  // the outputs are interpretable
  std::string u; // units
  if(runtime<=60){
    u=" s";
  } else if(runtime > 60 && runtime <=60*60){
    runtime/=60; u=" mins";
  } else if(runtime > (60*60) && runtime <= (360*24)){
    runtime/=(60*60); u=" hours";
  } else {
    runtime/=(24*60*60); u=" days";
  }
  runtime=std::round(runtime * 10.0) / 10.0;
  cout << " done! (" << runtime  << u << ")" <<  endl;
*/

  return std::move(ss.str());
}

void print_elapsed_time(
  const std::chrono::time_point<std::chrono::system_clock>& from,
  const std::chrono::time_point<std::chrono::system_clock>& to)
{
  cout << " done (" << get_elapsed_time(from, to) << ") " << endl;
}

void function_minimizer::sd_routine(void)
{
  std::chrono::time_point<std::chrono::system_clock> from_start;
  if (function_minimizer::output_flag == 1)
  {
    from_start = std::chrono::system_clock::now();
    cout << "Starting standard error calculations... " ;
    cout.flush();
  }

  int nvar=initial_params::nvarcalc(); // get the number of active parameters
  dvector x(1,nvar);
  initial_params::xinit(x); // get the number of active parameters

  initial_params::restore_start_phase();
  int nvar1=initial_params::nvarcalc(); // get the number of active parameters
  int num_sdrep_types=stddev_params::num_stddev_params +
    initial_params::num_active_calc();

  param_labels.allocate(1,num_sdrep_types);
  param_size.allocate(1,num_sdrep_types);

  int ii=1;
  size_t max_name_length = 0;
  for (int i=0;i<initial_params::num_initial_params;i++)
  {
    //if ((initial_params::varsptr[i])->phase_start
     // <= initial_params::current_phase)
    if (withinbound(0,(initial_params::varsptr[i])->phase_start,
      initial_params::current_phase))
    {
      param_labels[ii]=
       (initial_params::varsptr[i])->label();
      param_size[ii]=
       (initial_params::varsptr[i])->size_count();
      if (max_name_length<param_labels[ii].size())
      {
        max_name_length=param_labels[ii].size();
      }
      ii++;
    }
  }

  int start_stdlabels=ii;
  for (int i=0;i< stddev_params::num_stddev_params;i++)
  {
    param_labels[ii]=
      stddev_params::stddevptr[i]->label();
    param_size[ii]=
      stddev_params::stddevptr[i]->size_count();
    if (max_name_length<param_labels[ii].size())
    {
      max_name_length=param_labels[ii].size();
    }
    ii++;
  }
  int end_stdlabels=ii-1;

  int ndvar=stddev_params::num_stddev_calc();
  dvector scale(1,nvar1);   // need to get scale from somewhere
  dvector v(1,nvar);  // need to read in v from model.rep
  dmatrix S(1,nvar,1,nvar);
  {
    uistream cif("admodel.cov");
    int tmp_nvar = 0;
    cif >> tmp_nvar;
    if (nvar !=tmp_nvar)
    {
      cerr << "Incorrect number of independent variables in file"
        " model.cov" << endl;
      ad_exit(1);
    }
    cif >> S;
    if (!cif)
    {
      cerr << "error reading covariance matrix from model.cov" << endl;
      ad_exit(1);
    }
  }
  int sgn;
  initial_params::stddev_scale(scale,x);
  _ln_det_value = -ln_det(S,sgn)-2.0*sum(log(scale));
  initial_params::set_active_random_effects();
  //int nvar1=initial_params::nvarcalc();
  dvector diag(1,nvar1+ndvar);
  dvector tmp(1,nvar1+ndvar);

  {
    ofstream ofs("admodel.tmp");

    #if defined(__GNU__) || defined(DOS386)  || defined(__GNUDOS__)
    // *******************************************************
    // *******************************************************
    {
      if (nvar==nvar1)  // no random effects
      {
        for (int i=1;i<=nvar;i++)
        {
          for (int j=1;j<=i;j++)
          {
            tmp(j)=S(i,j)*scale(i)*scale(j);
            ofs << tmp(j) << " ";
          }
          ofs << endl;
          diag(i)=tmp(i);
        }
        dmatrix tv(1,ndvar,1,nvar1);
        adstring tmpstring="admodel.dep";
        if (ad_comm::wd_flag)
           tmpstring = ad_comm::adprogram_name + ".dep";
        cifstream cif((char*)tmpstring);

        int tmp_nvar = 0, tmp_ndvar = 0;
        cif >> tmp_nvar >> tmp_ndvar;
        if (tmp_nvar!=nvar1)
        {
          cerr << " tmp_nvar != nvar1 in file " << tmpstring
                 << endl;
          ad_exit(1);
        }
        if (ndvar>0)
        {
          cif >> tv;
          dvector tmpsub(1,nvar);
	  bool bad_vars=false;
          for (int i=1;i<=ndvar;i++)
          {
            for (int j=1;j<=nvar;j++)
            {
              tmpsub(j)=(tv(i)*S(j))*scale(j);
            }
            ofs << tmpsub << "  ";
            tmpsub=tv(i)*S;
            for (int j=1;j<=i;j++)
            {
              tmp(nvar+j)=tmpsub*tv(j);
              ofs << tmp(nvar+j) << " ";
            }
            diag(i+nvar)=tmp(i+nvar);

            if (diag(i + nvar) <= 0.0)
            {
              std::ostream& output_stream = get_output_stream();
              output_stream << "Estimated covariance matrix may not be positive definite.\n"
	                    << std::scientific << setprecision(10) << sort(eigenvalues(S)) << endl;

	      if(function_minimizer::output_flag==1)
              {
                // If first variable print message, otherwise tack it on
                if(!bad_vars)
                  cout << "\n Warning: Non-positive variance of sdreport variables: ";
                else
                  cout << ", ";

                cout << i + nvar;
                bad_vars=true;
              }
            }
            ofs << endl;
          }
	  if (bad_vars) cout << endl;
        }
      }
      else  // have random effects
      {
        dmatrix tv(1,ndvar,1,nvar1);
        adstring tmpstring="admodel.dep";
        if (ad_comm::wd_flag)
           tmpstring = ad_comm::adprogram_name + ".dep";
        cifstream cif((char*)tmpstring);

        int tmp_nvar = 0, tmp_ndvar = 0;
        cif >> tmp_nvar >> tmp_ndvar;
        if (tmp_nvar!=nvar1)
        {
          cerr << " tmp_nvar != nvar1 in file " << tmpstring
                 << endl;
          ad_exit(1);
        }

        dmatrix BS(1,nvar1,1,nvar1);
        BS.initialize();
        get_bigS(ndvar,nvar1,nvar,S,BS,scale);

        {
          tmpstring = ad_comm::adprogram_name + ".bgs";
          uostream uos((char*)(tmpstring));
          if (!uos)
          {
            cerr << "error opening file " << tmpstring << endl;
            ad_exit(1);
          }
          uos << nvar1;
          uos << BS;
          if (!uos)
          {
            cerr << "error writing to file " << tmpstring << endl;
            ad_exit(1);
          }
        }

	dvector* pBSi = &BS(1);
	double* pscalei = scale.get_v() + 1;
	double* ptmpi = tmp.get_v() + 1;
	double* pdiagi = diag.get_v() + 1;
        for (int i=1;i<=nvar1;i++)
        {
	  double* pBSij = pBSi->get_v() + 1;
	  double* ptmpj = tmp.get_v() + 1;
	  double* pscalej = scale.get_v() + 1;
          for (int j=1;j<=i;j++)
          {
            *ptmpj = *pBSij * *pscalei * *pscalej;
            ofs << *ptmpj << " ";

	    ++ptmpj;
	    ++pBSij;
	    ++pscalej;
          }
          ofs << endl;
          *pdiagi = *ptmpi;

	  ++pBSi;
	  ++ptmpi;
	  ++pscalei;
	  ++pdiagi;
        }

        if (ndvar>0)
        {
          cif >> tv;
          dvector tmpsub(1,nvar1);
          for (int i=1;i<=ndvar;i++)
          {
            for (int j=1;j<=nvar1;j++)
            {
              tmpsub(j)=(tv(i)*BS(j))*scale(j);
            }
            ofs << tmpsub << "  ";
            tmpsub=tv(i)*BS;
            for (int j=1;j<=i;j++)
            {
              tmp(nvar1+j)=tmpsub*tv(j);
              ofs << tmp(nvar1+j) << " ";
            }
            diag(i+nvar1)=tmp(i+nvar1);

            if (diag(i+nvar1)<=0.0)
            {
              if (norm(tv(i))>1.e-100)
              {
                cerr << "Estimated covariance matrix may not"
                 " be positive definite" << endl;
                cerr << sort(eigenvalues(BS)) << endl;
              }
            }
            ofs << endl;
          }
        }
      }
    }
    // *******************************************************
    #else
    // *******************************************************
    {
      for (int i=1;i<=nvar;i++)
      {
        for (int j=1;j<=i;j++)
        {
          tmp(j)=S(i,j)*scale(i)*scale(j);
          ofs << tmp(j) << " ";
        }
        ofs << endl;
        diag(i)=tmp(i);
      }
      dvector tv(1,nvar);
      adstring tmpstring="admodel.dep";
      if (ad_comm::wd_flag)
         tmpstring = ad_comm::adprogram_name + ".dep";
      cifstream cif((char*)tmpstring);
      int tmp_nvar,tmp_ndvar;
      cif >> tmp_nvar >> tmp_ndvar;
      dvector tmpsub(1,nvar);
      for (int i=1;i<=ndvar;i++)
      {
        cif >> tv;  // v(i)
        for (int j=1;j<=nvar;j++)
        {
          tmpsub(j)=(tv*S(j))*scale(j);
        }
        ofs << tmpsub << "  ";
        tmpsub=tv*S;
        cif.seekg(0,ios::beg);
        cif >> tmp_nvar >> tmp_ndvar;
        for (int j=1;j<=i;j++)
        {
          cif >> v;
          tmp(nvar+j)=tmpsub*v;
          ofs << tmp(nvar+j) << " ";
        }
        diag(i+nvar)=tmp(i+nvar);

        if (diag(i+nvar)<=0.0)
        {
          cerr << "Estimated covariance matrix may not be positive definite"
             << endl;
        }
        ofs << endl;
      }
    }
    // *******************************************************
    // *******************************************************
   #endif
  }


  {
    cifstream cif("admodel.tmp");
    //ofstream ofs("admodel.cor");
    ofstream ofs((char*)(ad_comm::adprogram_name + adstring(".cor")));
    ofstream ofsd((char*)(ad_comm::adprogram_name + adstring(".std")));

    int offset=1;
    dvector param_values(1,nvar1+ndvar);
    initial_params::copy_all_values(param_values,offset);
    stddev_params::copy_all_values(param_values,offset);

    for (int i=1;i<=nvar1;i++)
    {
      if (diag(i)<=0.0)
      {
        cerr << "Estimated covariance matrix may not be positive definite"
       << endl;
        ad_exit(1);
      }
      else
      {
        diag(i)=sqrt(diag(i));
      }
    }
    for (int i=nvar1+1;i<=nvar1+ndvar;i++)
    {
      if (diag(i)<0.0)
      {
        cerr << "Estimated covariance matrix may not be positive definite"
       << endl;
        ad_exit(1);
      }
      else if (diag(i)==0.0)
      {
        diag(i)=0.0;
      }
      else
      {
        diag(i)=sqrt(diag(i));
      }
    }

    {
      dvector dd=diag(nvar1+1,nvar1+ndvar);
      dd.shift(1);
      ii=0;
      stddev_params::get_all_sd_values(dd,ii);
    }

    int lc=1;
    int ic=1;
    ofs << " The logarithm of the determinant of the hessian = "
        << _ln_det_value << endl;
    ofs << " index  ";
    ofsd << " index  ";
    ofs << " name  ";
    ofsd << " name ";
    size_t inmax = max_name_length > 5 ? max_name_length - 5 : 0;
    for (size_t in = 1;in <= inmax; in++)
    {
      ofs << " ";
      ofsd << " ";
    }
    ofs << "  value      std.dev   ";
    ofsd << "  value      std.dev";
    for (int i=1;i<=nvar+ndvar;i++)
    {
      ofs << " " << setw(4) << i << "   ";
    }
    ofs << endl;
    ofsd << endl;

    if (GAUSS_varcovariance_matrix) (*GAUSS_varcovariance_matrix).initialize();

    double* pdiagi = diag.get_v() + 1;
    for (int i=1;i<=nvar1+ndvar;i++)
    {
      double* ptmpj = tmp.get_v() + 1;
      for (int j=1;j<=i;j++)
      {
        cif >> *ptmpj;

	++ptmpj;
      }
      ptmpj = tmp.get_v() + 1;
      double* pdiagj = diag.get_v() + 1;
      for (int j=1;j<=i;j++)
      {
        if (*pdiagi == 0.0 || *pdiagj == 0.0)
        {
          *ptmpj = 0.0;
        }
        else
        {
          if (i!=j)
          {
            *ptmpj /= (*pdiagi * *pdiagj);
          }
          else
          {
            *ptmpj = 1;
          }
        }
	++ptmpj;
	++pdiagj;
      }
      ofs << "  " << setw(4) << i << "   ";
      ofsd << "  " << setw(4) << i << "   ";
      ofs << param_labels[lc];
      ofsd << param_labels[lc];
// get the std dev of profiles likelihood variables into the right slots
      if (start_stdlabels <= lc && end_stdlabels >= lc)
      {
        for (int ix=0;ix<likeprof_params::num_likeprof_params;ix++)
        {
          if (param_labels[lc]==likeprof_params::likeprofptr[ix]->label())
          {
            likeprof_params::likeprofptr[ix]->get_sigma()= *pdiagi;
          }
        }
      }

      inmax = max_name_length + 1 > param_labels[lc].size()
              ? max_name_length + 1 - param_labels[lc].size() : 0;
      for (size_t in = 1; in <= inmax; in++)
      {
        ofs << " ";
        ofsd << " ";
      }
      if (++ic> param_size[lc])
      {
        lc++;
        ic=1;
      }
      ofs << setscientific() << setw(11) << setprecision(4) << param_values(i)
          << " ";
      ofs << setscientific() << setw(10) << setprecision(4) << *pdiagi << " ";

      if (GAUSS_varcovariance_matrix)
      {
        if (GAUSS_varcovariance_matrix->indexmax()>=i)
          (*GAUSS_varcovariance_matrix) (i,1)= *pdiagi;
      }

      ofsd << setscientific() << setw(11) << setprecision(4) << param_values(i)
           << " ";
      ofsd << setscientific() << setw(10) << setprecision(4) << *pdiagi;
      ptmpj = tmp.get_v() + 1;
      for (int j=1;j<=i;j++)
      {
        ofs << " " << setfixed() << setprecision(4) << setw(7) << *ptmpj;
        if (GAUSS_varcovariance_matrix)
        {
          if (GAUSS_varcovariance_matrix->indexmax()>=i  &&
            (*GAUSS_varcovariance_matrix)(i).indexmax()>j)
          {
            (*GAUSS_varcovariance_matrix) (i,j+1) = *ptmpj;
          }
        }
	++ptmpj;
      }
      ofs << endl;
      ofsd << endl;

      ++pdiagi;
    }
  }
#if defined(_MSC_VER)
  if (system("del admodel.tmp") == -1)
#else
  if (unlink("admodel.tmp") == -1)
#endif
  {
    char msg[40] = {"Error trying to delete temporary file "};
    cerr << msg << "admodel.tmp" << endl;
  }

  if (function_minimizer::output_flag == 1)
  {
    print_elapsed_time(from_start, std::chrono::system_clock::now());
  }
}
