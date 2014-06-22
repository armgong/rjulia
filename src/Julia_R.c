/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <stdbool.h>
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

SEXP Julia_R_Scalar_NA(jl_value_t* Var)
{
  return ScalarInteger(NA_INTEGER);
}

SEXP Julia_R_MD_NA(jl_value_t* Var)
{
 SEXP ans=R_NilValue; 
 char* strData="Varname0tmp.data";
 char* strNA="bitunpack(Varname0tmp.na)"; 
 jl_set_global(jl_main_module, jl_symbol("Varname0tmp"), (jl_value_t*)Var);
 jl_value_t* retData=jl_eval_string(strData); 
 jl_value_t* retNA=jl_eval_string(strNA); 
 jl_value_t* val;
 //Rprintf("Data Len %d, NA Len %d\n",jl_array_len(retData),jl_array_len(retNA));
 
 if (((jl_array_t*)retData)->ptrarray)
    val = jl_cellref(retData, 0);
 else
    val = jl_arrayref((jl_array_t*)retData,0);
 //get Julia dims and set R array Dims
 int len=jl_array_len(retData);
 if (len==0) 
   return ans;

 int ndims=jl_array_ndims(retData);
 //Rprintf("data dims is %d,na dims is %d\n",ndims,jl_array_ndims(retNA));
 SEXP dims;
 PROTECT(dims = allocVector(INTSXP, ndims));
  for (size_t i=0;i<ndims;i++) 
   { 
    INTEGER(dims)[i]=jl_array_dim(retData,i);
   } 
  UNPROTECT(1);

 if (jl_is_bool(val))
 {
  char* p=(char*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(LGLSXP, dims));
  for (size_t i=0;i<len;i++) 
    if(pNA[i])
     LOGICAL(ans)[i]=NA_LOGICAL; 
    else  
     LOGICAL(ans)[i]=p[i]; 
  UNPROTECT(1);
 }
 else if (jl_is_int32(val))
{
  int32_t* p=(int32_t*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(INTSXP, dims));
  //Rprintf("pNA len is %d\n",jl_array_len(retNA));
  for (size_t i=0;i<len;i++)
   {
    //Rprintf("enter int32 %d\n",i); 
    //Rprintf("bool value %d\n",pNA[i]);
    if(pNA[i])
     INTEGER(ans)[i]=NA_INTEGER; 
    else   
     INTEGER(ans)[i]=p[i];
    }  
  UNPROTECT(1);
}
//int64
else if (jl_is_int64(val))
{
  int64_t* p=(int64_t*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
   if(pNA[i])
    REAL(ans)[i]=NA_REAL; 
   else    
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//more integer type
else if (jl_is_int8(val))
{
  int8_t* p=(int8_t*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    if(pNA[i])
     INTEGER(ans)[i]=NA_INTEGER; 
    else   
     INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_int16(val))
{
  int16_t* p=(int16_t*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    if(pNA[i])
     INTEGER(ans)[i]=NA_INTEGER; 
    else   
     INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_uint8(val))
{
  uint8_t* p=(uint8_t*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    if(pNA[i])
     INTEGER(ans)[i]=NA_INTEGER; 
    else   
     INTEGER(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_uint16(val))
{
  uint16_t* p=(uint16_t*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(INTSXP, dims));
  for (size_t i=0;i<len;i++)
    if(pNA[i])
     INTEGER(ans)[i]=NA_INTEGER; 
    else   
     INTEGER(ans)[i]=p[i];  
  UNPROTECT(1);
}
else if (jl_is_uint32(val))
{
  uint32_t* p=(uint32_t*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
   if(pNA[i])
    REAL(ans)[i]=NA_REAL; 
   else    
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_uint64(val))
{
  uint64_t* p=(uint64_t*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
   if(pNA[i])
    REAL(ans)[i]=NA_REAL; 
   else    
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
//double
else if (jl_is_float64(val))
{
  double* p=(double*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
   if(pNA[i])
    REAL(ans)[i]=NA_REAL; 
   else    
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}
else if (jl_is_float32(val))
{
  float* p=(float*) jl_array_data(retData);
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(REALSXP, dims));
  for (size_t i=0;i<len;i++)
   if(pNA[i])
    REAL(ans)[i]=NA_REAL; 
   else    
    REAL(ans)[i]=p[i]; 
  UNPROTECT(1);
}      
//convert string array to STRSXP ,but not sure it is corret?
else if (jl_is_utf8_string(val))
{
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(STRSXP, dims));
  for (size_t i=0;i<len;i++)
     if(pNA[i])
      SET_STRING_ELT(ans,i,NA_STRING);
     else    
    SET_STRING_ELT(ans,i,mkCharCE(jl_string_data(jl_cellref(retData,i)),CE_UTF8));
  UNPROTECT(1);
}
else if (jl_is_ascii_string(val))
{
  char* pNA=(char*) jl_array_data(retNA);
  PROTECT(ans = allocArray(STRSXP, dims));
  for (size_t i=0;i<len;i++)
     if(pNA[i])
      SET_STRING_ELT(ans,i,NA_STRING);
     else
      SET_STRING_ELT(ans,i,mkChar(jl_string_data(jl_cellref(retData,i))));
  UNPROTECT(1);
}
return ans;
}