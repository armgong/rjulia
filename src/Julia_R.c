#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include "julia_R.h"
#define pkgdebug

SEXP Julia_R_Basic_Element(jl_value_t* Var) 
{
  SEXP ans=R_NilValue;
  if (jl_is_int32(Var))
  {
    PROTECT(ans = allocVector(INTSXP, 1));
    INTEGER(ans)[0]=jl_unbox_int32(Var); 
    UNPROTECT(1);
  }
  if (jl_is_int64(Var))
  {
    PROTECT(ans = allocVector(REALSXP, 1));
    REAL(ans)[0]=(double)jl_unbox_int64(Var); 
    UNPROTECT(1);
  }
  if (jl_is_float64(Var))
  {
    PROTECT(ans = allocVector(REALSXP, 1));
    REAL(ans)[0]=jl_unbox_int64(Var); 
    UNPROTECT(1);

  }
  if (jl_is_bool(Var))
  {
    PROTECT(ans = allocVector(LGLSXP, 1));
    LOGICAL(ans)[0]=jl_unbox_bool(Var); 
    UNPROTECT(1);
  }

  if (jl_is_utf8_string(Var))
  {
    PROTECT(ans = allocVector(STRSXP, 1));
    SET_STRING_ELT(ans,0,mkCharCE(jl_string_data(Var),CE_UTF8));
    UNPROTECT(1);
  } 
  if (jl_is_ascii_string(Var))
  {
    PROTECT(ans = allocVector(STRSXP, 1));
    SET_STRING_ELT(ans,0,mkChar(jl_string_data(Var)));
    UNPROTECT(1);
  } 
  return ans;
}

SEXP Julia_R_1D(jl_value_t* Var)
{
 SEXP ans=R_NilValue;

 jl_value_t* val; 
 if (((jl_array_t*)Var)->ptrarray)
    val = jl_cellref(Var, 0);
 else
    val = jl_arrayref((jl_array_t*)Var,0);

 int len=jl_array_dim(Var,0);
 if (jl_is_bool(val))
 {
  char* p=(char*) jl_array_data(Var);
  PROTECT(ans = allocVector(LGLSXP, len));
  for (size_t i=0;i<len;i++)
    LOGICAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
if (jl_is_int32(val))
{
  int* p=(int*) jl_array_data(Var);
  PROTECT(ans = allocVector(INTSXP, len));
  for (size_t i=0;i<len;i++)
    INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//int64
if (jl_is_int64(val))
{
  long long* p=(long long*) jl_array_data(Var);
  PROTECT(ans = allocVector(REALSXP, len));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//double
if (jl_is_float64(val))
{
  double* p=(double*) jl_array_data(Var);
  PROTECT(ans = allocVector(REALSXP, len));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}   
//convert string array to STRSXP ,but not sure it is corret?
if (jl_is_utf8_string(val))
{
  PROTECT(ans = allocVector(STRSXP, len));
  for (size_t i=0;i<len;i++)
    SET_STRING_ELT(ans,i,mkCharCE(jl_string_data(jl_cellref(Var,i)),CE_UTF8));
  UNPROTECT(1);
}
if (jl_is_utf8_string(val))
{
  PROTECT(ans = allocVector(STRSXP, len));
  for (size_t i=0;i<len;i++)
    SET_STRING_ELT(ans,i,mkCharCE(jl_string_data(jl_cellref(Var,i)),CE_UTF8));
  UNPROTECT(1);
}
if (jl_is_ascii_string(val))
{
  PROTECT(ans = allocVector(STRSXP, len));
  for (size_t i=0;i<len;i++)
    SET_STRING_ELT(ans,i,mkChar(jl_string_data(jl_cellref(Var,i))));
  UNPROTECT(1);
}

return ans;
}

SEXP Julia_R_2D(jl_value_t* Var)
{
 SEXP ans=R_NilValue; 
 jl_value_t* val; 
 if (((jl_array_t*)Var)->ptrarray)
    val = jl_cellref(Var, 0);
 else
    val = jl_arrayref((jl_array_t*)Var,0);
 int len=jl_array_len(Var);
 int dim0=jl_array_dim(Var,0);
 int dim1=jl_array_dim(Var,1);
 if (jl_is_bool(val))
 {
  char* p=(char*) jl_array_data(Var);
  PROTECT(ans = allocMatrix(LGLSXP, dim0,dim1));
  for (size_t i=0;i<len;i++) 
    LOGICAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
if (jl_is_int32(val))
{
  int* p=(int*) jl_array_data(Var);
  PROTECT(ans = allocMatrix(INTSXP, dim0,dim1));
  for (size_t i=0;i<len;i++)
    INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//int64
if (jl_is_int64(val))
{
  long long* p=(long long*) jl_array_data(Var);
  PROTECT(ans = allocMatrix(REALSXP, dim0,dim1));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//double
if (jl_is_float64(val))
{
  double* p=(double*) jl_array_data(Var);
  PROTECT(ans = allocMatrix(REALSXP, dim0,dim1));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}   
//convert string array to STRSXP ,but not sure it is corret?
if (jl_is_utf8_string(val))
{
  PROTECT(ans = allocVector(STRSXP, len));
  for (size_t i=0;i<len;i++)
    SET_STRING_ELT(ans,i,mkCharCE(jl_string_data(jl_cellref(Var,i)),CE_UTF8));
  UNPROTECT(1);
}
if (jl_is_ascii_string(val))
{
  PROTECT(ans = allocMatrix(STRSXP, dim0,dim1));
  for (size_t i=0;i<len;i++)
   SET_STRING_ELT(ans,i,mkChar(jl_string_data(jl_cellref(Var,i))));
  UNPROTECT(1);
}
return ans;
}