/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <julia.h>
#include "dataframe.h"
#include "Julia_R.h"
#define pkgdebug
#ifndef jl_is_int8
#define jl_is_int8(v)        jl_typeis(v,jl_int8_type)
#define jl_is_int16(v)       jl_typeis(v,jl_int16_type)
#define jl_is_uint8(v)       jl_typeis(v,jl_uint8_type)
#define jl_is_uint16(v)      jl_typeis(v,jl_uint16_type)
#endif
bool jl_is_NAtype(jl_value_t* Var)
{
  if (strcmp(jl_typeof_str(Var),"NAtype")==0)
    return true;
  else
    return false;
}
bool jl_is_DataArray(jl_value_t* Var)
{
  if (strcmp(jl_typeof_str(Var),"DataArray")==0||
      strcmp(jl_typeof_str(Var),"DataVector")==0||
      strcmp(jl_typeof_str(Var),"DataMatrix")==0)
    return true;
  else
    return false;
}  
bool jl_is_PooledDataArray(jl_value_t* Var)
{
  if (strcmp(jl_typeof_str(Var),"PooledDataArray")==0||
      strcmp(jl_typeof_str(Var),"PooledDataVector")==0||
      strcmp(jl_typeof_str(Var),"PooledDataMatrix")==0)
    return true;
  else
    return false;
}  
bool jl_is_DataFrame(jl_value_t* Var)
{
  if (strcmp(jl_typeof_str(Var),"DataFrame")==0)
    return true;
  else
    return false;
} 

bool jl_is_DataArrayFrame(jl_value_t* Var)
{
  if (strcmp(jl_typeof_str(Var),"DataArray")==0||
           strcmp(jl_typeof_str(Var),"DataVector")==0||
           strcmp(jl_typeof_str(Var),"DataMatrix")==0||
           strcmp(jl_typeof_str(Var),"PooledDataArray")==0||
           strcmp(jl_typeof_str(Var),"PooledDataVector")==0||
           strcmp(jl_typeof_str(Var),"PooledDataMatrix")==0||
           strcmp(jl_typeof_str(Var),"DataFrame")==0||
           strcmp(jl_typeof_str(Var),"NAtype")==0)
    return true;
  else
    return false;
} 

SEXP Julia_R_Scalar(jl_value_t* Var) 
{
  SEXP ans=R_NilValue;
  //most common type is here
  if (jl_is_int32(Var))
  {
    PROTECT(ans=ScalarInteger(jl_unbox_int32(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_int64(Var))
  {
    PROTECT(ans=ScalarReal((double)jl_unbox_int64(Var)));
    UNPROTECT(1);
  }
  //more integer type
  if (jl_is_uint32(Var))
  {
    PROTECT(ans=ScalarReal((double)jl_unbox_uint32(Var)));
    UNPROTECT(1); 
  }
  else if (jl_is_uint64(Var))
  {
     PROTECT(ans=ScalarReal((double)jl_unbox_uint64(Var)));
     UNPROTECT(1);
  }
  else if (jl_is_float64(Var))
  {
    PROTECT(ans=ScalarReal(jl_unbox_float64(Var)));
    UNPROTECT(1); 
  }
  else if (jl_is_float32(Var))
  {
    PROTECT(ans=ScalarReal(jl_unbox_float32(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_bool(Var))
  {
    PROTECT(ans=ScalarLogical(jl_unbox_bool(Var))); 
    UNPROTECT(1);
  }
  else if (jl_is_int8(Var))
  {
    PROTECT(ans=ScalarInteger(jl_unbox_int8(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_uint8(Var))
  {
    PROTECT(ans=ScalarInteger(jl_unbox_uint8(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_int16(Var))
  {
    PROTECT(ans=ScalarInteger(jl_unbox_int16(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_uint16(Var))
  {
    PROTECT(ans=ScalarInteger(jl_unbox_uint16(Var)));
    UNPROTECT(1); 
  }
  else if (jl_is_utf8_string(Var))
  {
    PROTECT(ans = allocVector(STRSXP, 1));
    SET_STRING_ELT(ans,0,mkCharCE(jl_string_data(Var),CE_UTF8));
    UNPROTECT(1);
  } 
  else if (jl_is_ascii_string(Var))
  {
    PROTECT(ans=ScalarString(mkChar(jl_string_data(Var))));
    UNPROTECT(1);
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
  SEXP ans;
  PROTECT(ans=ScalarInteger(NA_INTEGER));
  UNPROTECT(1);
  return ans;
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
 
 if (((jl_array_t*)retData)->ptrarray)
    val = jl_cellref(retData, 0);
 else
    val = jl_arrayref((jl_array_t*)retData,0);
 int len=jl_array_len(retData);
 if (len==0) 
   return ans;

 int ndims=jl_array_ndims(retData);
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

//this function is for factor convert it maybe not safe
//because PooledDataArray.refs is Uint32 or bigger
//but in pratice it should be ok
SEXP Julia_R_MD_INT(jl_value_t* Var)
{
 SEXP ans=R_NilValue; 
 jl_value_t* val; 
 if (((jl_array_t*)Var)->ptrarray)
    val = jl_cellref(Var, 0);
 else
    val = jl_arrayref((jl_array_t*)Var,0);
 int len=jl_array_len(Var);
 if (len==0) return ans;

 
 if (jl_is_int32(val))
 {
  PROTECT(ans = allocVector(INTSXP, len));
  int32_t* p=(int32_t*) jl_array_data(Var);
  for (size_t i=0;i<len;i++)
  {
  if (p[i]==0)
   INTEGER(ans)[i]=NA_INTEGER;
  else
   INTEGER(ans)[i]=p[i]; 
  } 
 }
 else if (jl_is_int64(val))
 {
  PROTECT(ans = allocVector(INTSXP, len));
  int64_t* p=(int64_t*) jl_array_data(Var);
  for (size_t i=0;i<len;i++)
  {
  if (p[i]==0)
   INTEGER(ans)[i]=NA_INTEGER;
  else
   INTEGER(ans)[i]=p[i]; 
  } 
 }
 else if (jl_is_int8(val))
 {
  PROTECT(ans = allocVector(INTSXP, len));
  int8_t* p=(int8_t*) jl_array_data(Var);
  for (size_t i=0;i<len;i++)
  {
  if (p[i]==0)
   INTEGER(ans)[i]=NA_INTEGER;
  else
   INTEGER(ans)[i]=p[i]; 
  } 
 }
 else if (jl_is_int16(val))
   {
  PROTECT(ans = allocVector(INTSXP, len));
   int16_t* p=(int16_t*) jl_array_data(Var);
  for (size_t i=0;i<len;i++)
  {
  if (p[i]==0)
   INTEGER(ans)[i]=NA_INTEGER;
  else
   INTEGER(ans)[i]=p[i]; 
  } 
 }
 else if (jl_is_uint8(val))
   {
  PROTECT(ans = allocVector(INTSXP, len));
  uint8_t* p=(uint8_t*) jl_array_data(Var);
  for (size_t i=0;i<len;i++)
  {
  if (p[i]==0)
   INTEGER(ans)[i]=NA_INTEGER;
  else
   INTEGER(ans)[i]=p[i]; 
  } 
 }
 else if (jl_is_uint16(val))
   {
  PROTECT(ans = allocVector(INTSXP, len));
  uint16_t* p=(uint16_t*) jl_array_data(Var);
  for (size_t i=0;i<len;i++)
  {
  if (p[i]==0)
   INTEGER(ans)[i]=NA_INTEGER;
  else
   INTEGER(ans)[i]=p[i]; 
  } 
 }
 else if (jl_is_uint32(val))
  {
  PROTECT(ans = allocVector(INTSXP, len));
  uint32_t* p=(uint32_t*) jl_array_data(Var);
  for (size_t i=0;i<len;i++)
  {
  if (p[i]==0)
   INTEGER(ans)[i]=NA_INTEGER;
  else
   INTEGER(ans)[i]=p[i]; 
  } 
 }
 else if (jl_is_uint64(val))
 {
  PROTECT(ans = allocVector(INTSXP, len));
  uint64_t* p=(uint64_t*) jl_array_data(Var);
  for (size_t i=0;i<len;i++)
  {
  if (p[i]==0)
   INTEGER(ans)[i]=NA_INTEGER;
  else
   INTEGER(ans)[i]=p[i]; 
  } 
 }
UNPROTECT(1);
return ans;
}

SEXP Julia_R_MD_NA_Factor(jl_value_t* Var)
{
 SEXP ans=R_NilValue; 
 char* strData="Varname0tmp.refs";
 char* strlevels="VarPools=Array(ASCIIString,length(Varname0tmp.pool))\r\n" 
                 "for i in 1:length(Varname0tmp.pool)\r\n"
                 "VarPools[i]=string(Varname0tmp.pool[i])\r\n"
                 "end\r\n"
                 "VarPools\r\n"; 
 jl_set_global(jl_main_module, jl_symbol("Varname0tmp"), (jl_value_t*)Var);
 jl_value_t* retData=jl_eval_string(strData); 
 jl_value_t* retlevels=jl_eval_string(strlevels); 
 //first get refs data,dims=n
 //caution this convert to int32 SEXP,it should be ok in reality,
 //but if have a lot factor may be cause int32 overrun.
 ans=Julia_R_MD_INT(retData);
 PROTECT(ans);
 //second setAttrib R levels and class
 SEXP levels=Julia_R_MD(retlevels);
 setAttrib(ans,R_LevelsSymbol,levels);
 setAttrib(ans,R_ClassSymbol,mkString("factor"));
 UNPROTECT(1);
return ans;
}

SEXP Julia_R_MD_NA_DataFrame(jl_value_t* Var)
{
 SEXP ans,names,rownames;
 char evalcmd[evalsize];
 int i;
 const char* dfname="DataFrameName0tmp";
 jl_set_global(jl_main_module, jl_symbol(dfname), (jl_value_t*)Var);
 //Get Frame cols 
 snprintf(evalcmd,evalsize,"size(%s,2)",dfname);
 jl_value_t* cols=jl_eval_string(evalcmd);
 int collen=jl_unbox_long(cols);
 jl_value_t* eachcolvector;
 jl_value_t* coltype;
 //Create VECSXP

 //Create SEXP for Each Column and assign
 PROTECT(ans=allocVector(VECSXP,collen));
 for (i=0;i<collen;i++)
 {
  
  snprintf(evalcmd,evalsize,"%s[%d]",dfname,i+1);
  eachcolvector=jl_eval_string(evalcmd);
  snprintf(evalcmd,evalsize,"isa(%s[%d],PooledDataArray)",dfname,i+1);
  coltype=jl_eval_string(evalcmd);
  if (jl_unbox_bool(coltype))
   SET_VECTOR_ELT(ans,i,Julia_R_MD_NA_Factor(eachcolvector));
  else
   SET_VECTOR_ELT(ans,i,Julia_R_MD_NA(eachcolvector));
 }
 //set names attribute
 snprintf(evalcmd,evalsize,"names(%s)",dfname);
 jl_value_t* ret=jl_eval_string(evalcmd);
 jl_value_t* onesymbol;
 if (jl_is_array(ret))
 {
  PROTECT(names=allocVector(STRSXP,collen));
  for (i=0;i<jl_array_len(ret);i++)
  { 
   onesymbol=jl_arrayref((jl_array_t*)ret,i);
   if (jl_is_symbol(onesymbol))
    SET_STRING_ELT(names,i,mkChar(((jl_sym_t*)onesymbol)->name));
  }
  setAttrib(ans,R_NamesSymbol,names);
  UNPROTECT(1);
 } 
 //set row names
 snprintf(evalcmd,evalsize,"size(%s,1)",dfname);
 jl_value_t* rows=jl_eval_string(evalcmd);
 int rowlen=jl_unbox_long(rows);
 PROTECT(rownames=allocVector(INTSXP,rowlen));
 for (i=0;i<rowlen;i++)
  INTEGER(rownames)[i]=i+1;
 setAttrib(ans,R_RowNamesSymbol,rownames);
 UNPROTECT(1);
 //set class as data frame
 setAttrib(ans,R_ClassSymbol,mkString("data.frame"));
 //SET_OBJECT(ans, 1) ; 
 UNPROTECT(1);
 return ans;
}

//Convert Julia Type To R,Real interface
SEXP Julia_R(jl_value_t* Var)
{  
  SEXP ans=R_NilValue;
  if (jl_is_nothing(Var)||jl_is_null(Var))
    return ans;

  //Array To Vector
  JL_GC_PUSH1(&Var);
  if (jl_is_array(Var))
  {
   ans=Julia_R_MD(Var); 
  }
  else if (jl_is_DataArrayFrame(Var))
   {
    //try to load DataArrays DataFrames package
    if (!LoadDF())
    {
     JL_GC_POP();
     return R_NilValue;
    }
    if(jl_is_NAtype(Var))
        ans=Julia_R_Scalar_NA(Var); 
    else if (jl_is_DataFrame(Var))
        ans=Julia_R_MD_NA_DataFrame(Var);
    else if (jl_is_DataArray(Var))
     ans=Julia_R_MD_NA(Var);
    else if (jl_is_PooledDataArray(Var))
     ans=Julia_R_MD_NA_Factor(Var);
    else if (jl_is_tuple(Var))
    {
     PROTECT(ans=allocVector(VECSXP,jl_tuple_len(Var)));
     for(int i=0;i<jl_tuple_len(Var);i++)
      SET_ELEMENT(ans,i,Julia_R(jl_tupleref(Var,i)));
     UNPROTECT(1);
    } 
   }  
  else 
    ans=Julia_R_Scalar(Var); 
 JL_GC_POP();
 return ans;
}