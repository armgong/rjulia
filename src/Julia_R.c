/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include "Julia_R.h"
#define pkgdebug
#ifndef jl_is_int8
#define jl_is_int8(v)        jl_typeis(v,jl_int8_type)
#define jl_is_int16(v)       jl_typeis(v,jl_int16_type)
#define jl_is_uint8(v)       jl_typeis(v,jl_uint8_type)
#define jl_is_uint16(v)      jl_typeis(v,jl_uint16_type)
#endif
SEXP Julia_R_Scalar(jl_value_t* Var) 
{
  SEXP ans=R_NilValue;
  //most common type is here
  if (jl_is_int32(Var))
  {
    ans=ScalarInteger(jl_unbox_int32(Var)); 
  }
  else if (jl_is_int64(Var))
  {
    ans=ScalarReal((double)jl_unbox_int64(Var)); 
  }
  //more integer type
  if (jl_is_uint32(Var))
  {
    ans=ScalarReal((double)jl_unbox_uint32(Var)); 
  }
  else if (jl_is_uint64(Var))
  {
     ans=ScalarReal((double)jl_unbox_uint64(Var));
  }
  else if (jl_is_float64(Var))
  {
    ans=ScalarReal(jl_unbox_float64(Var)); 
  }
  else if (jl_is_float32(Var))
  {
    ans=ScalarReal(jl_unbox_float32(Var)); 
  }
  else if (jl_is_bool(Var))
  {
    ans=ScalarLogical(jl_unbox_bool(Var)); 
  }
  else if (jl_is_int8(Var))
  {
    ans=ScalarInteger(jl_unbox_int8(Var)); 
  }
  else if (jl_is_uint8(Var))
  {
    ans=ScalarInteger(jl_unbox_uint8(Var)); 
  }
  else if (jl_is_int16(Var))
  {
    ans=ScalarInteger(jl_unbox_int16(Var)); 
  }
  else if (jl_is_uint16(Var))
  {
    ans=ScalarInteger(jl_unbox_uint16(Var)); 
  }
  else if (jl_is_utf8_string(Var))
  {
    PROTECT(ans = allocVector(STRSXP, 1));
    SET_STRING_ELT(ans,0,mkCharCE(jl_string_data(Var),CE_UTF8));
    UNPROTECT(1);
  } 
  else if (jl_is_ascii_string(Var))
  {
    ans=ScalarString(mkChar(jl_string_data(Var)));
  } 
  return ans;
}

SEXP Julia_R_MD(jl_value_t* Var)
{
 SEXP ans=R_NilValue; 
 jl_value_t* val; 
 if (((jl_array_t*)Var)->ptrarray)
    val = jl_cellref(Var, 0);
 else
    val = jl_arrayref((jl_array_t*)Var,0);
 //get Julia dims and set R array Dims
 int len=jl_array_len(Var);
 if (len==0) 
   return ans;

 int ndims=jl_array_ndims(Var);
 SEXP dims;
 PROTECT(dims = allocVector(INTSXP, ndims));
  for (size_t i=0;i<ndims;i++) 
   { 
    INTEGER(dims)[i]=jl_array_dim(Var,i);
   } 
  UNPROTECT(1);

 if (jl_is_bool(val))
 {
  char* p=(char*) jl_array_data(Var);
  PROTECT(ans = allocArray(LGLSXP, dims));
  for (size_t i=0;i<len;i++) 
    LOGICAL(ans)[i]=p[i]; 
  UNPROTECT(1);
 }
 else if (jl_is_int32(val))
{
  int32_t* p=(int32_t*) jl_array_data(Var);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//int64
else if (jl_is_int64(val))
{
  int64_t* p=(int64_t*) jl_array_data(Var);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//more integer type
else if (jl_is_int8(val))
{
  int8_t* p=(int8_t*) jl_array_data(Var);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_int16(val))
{
  int16_t* p=(int16_t*) jl_array_data(Var);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_uint8(val))
{
  uint8_t* p=(uint8_t*) jl_array_data(Var);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_uint16(val))
{
  uint16_t* p=(uint16_t*) jl_array_data(Var);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_uint32(val))
{
  uint32_t* p=(uint32_t*) jl_array_data(Var);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_uint64(val))
{
  uint64_t* p=(uint64_t*) jl_array_data(Var);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//double
else if (jl_is_float64(val))
{
  double* p=(double*) jl_array_data(Var);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_float32(val))
{
  float* p=(float*) jl_array_data(Var);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}      
//convert string array to STRSXP ,but not sure it is corret?
else if (jl_is_utf8_string(val))
{
  PROTECT(ans = allocArray(STRSXP, dims));
  for (size_t i=0;i<len;i++)
    SET_STRING_ELT(ans,i,mkCharCE(jl_string_data(jl_cellref(Var,i)),CE_UTF8));
  UNPROTECT(1);
}
else if (jl_is_ascii_string(val))
{
  PROTECT(ans = allocArray(STRSXP, dims));
  for (size_t i=0;i<len;i++)
   SET_STRING_ELT(ans,i,mkChar(jl_string_data(jl_cellref(Var,i))));
  UNPROTECT(1);
}
return ans;
}
