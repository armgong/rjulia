/*
Copyright (C) 2014, 2015 by Yu Gong
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

// Macros for julia type to r, for shorter code

#define jlint_to_r              PROTECT(ans = allocArray(INTSXP, dims));\
    for (size_t i = 0; i < len; i++)\
      INTEGER(ans)[i] = p[i];\
    UNPROTECT(1);

#define jlfloat_to_r          PROTECT(ans = allocArray(REALSXP, dims));\
    for (size_t i = 0; i < len; i++)\
      REAL(ans)[i] = p[i];\
    UNPROTECT(1);

#define jlbigint_to_r       bool isInt32=true;\
    for (size_t ii=0;ii<len;ii++)\
    {\
      if (p[ii]>INT32_MAX || p[ii]<INT32_MIN)\
      {\
        isInt32=false;\
        break;\
      }\
    }\
    if (isInt32)\
    {\
      PROTECT(ans = allocArray(INTSXP, dims));\
      for (size_t i = 0; i < len; i++)\
      INTEGER(ans)[i] = p[i];\
      UNPROTECT(1);\
    }\
    else\
    {\
      PROTECT(ans = allocArray(REALSXP, dims));\
      for (size_t i = 0; i < len; i++)\
      REAL(ans)[i] = p[i];\
      UNPROTECT(1);\
    }

// Macros for julia type which includes NA to R, for shorter code
#define jlint_to_r_na           PROTECT(ans = allocArray(INTSXP, dims));\
    for (size_t i = 0; i < len; i++)\
    {\
      if (pNA[i])\
        INTEGER(ans)[i] = NA_INTEGER;\
      else\
        INTEGER(ans)[i] = p[i];\
    }\
    UNPROTECT(1);

#define jlfloat_to_r_na        PROTECT(ans = allocArray(REALSXP, dims));\
    for (size_t i = 0; i < len; i++)\
    {\
      if (pNA[i])\
        REAL(ans)[i] = NA_REAL;\
      else\
        REAL(ans)[i] = p[i];\
    }\
    UNPROTECT(1);

#define jlbigint_to_r_na    bool isInt32=true;\
  for (size_t ii=0;ii<len;ii++)\
  {\
    if (pNA[ii]) continue;\
    if (p[ii]>INT32_MAX || p[ii]<INT32_MIN)\
    {\
      isInt32=false;\
      break;\
    } \
  }\
  if (isInt32)\
  {\
    PROTECT(ans = allocArray(INTSXP, dims));\
    for (size_t i = 0; i < len; i++)\
     {\
      if (pNA[i])\
       INTEGER(ans)[i] = NA_INTEGER;\
      else\
       INTEGER(ans)[i] = p[i];\
     }\
    UNPROTECT(1); \
  } \
  else\
  {\
    PROTECT(ans = allocArray(REALSXP, dims));\
    for (size_t i = 0; i < len; i++)\
    {\
      if (pNA[i])\
        REAL(ans)[i] = NA_REAL;\
      else\
        REAL(ans)[i] = p[i];\
    }\
    UNPROTECT(1);\
  }

// macro for julia type which includes factor to r, for shorter code
#define jlint_to_r_md          PROTECT(ans = allocVector(INTSXP, len));\
    for (size_t i = 0; i < len; i++)\
    {\
      if (p[i] == 0)\
        INTEGER(ans)[i] = NA_INTEGER;\
      else\
        INTEGER(ans)[i] = p[i];\
    }\
    UNPROTECT(1);

static bool biginttodouble=false;
SEXP Julia_BigintToDouble(SEXP Var)
{
 biginttodouble=LOGICAL(Var)[0];
 return R_NilValue;
}

static bool inInt32Range(double val)
{
  return (val<=INT32_MAX && val>=INT32_MIN)?true:false;
}
static bool jl_is_NAtype(jl_value_t *Var)
{
  if (strcmp(jl_typeof_str(Var), "NAtype") == 0)
    return true;
  else
    return false;
}
static bool jl_is_DataArray(jl_value_t *Var)
{
  if (strcmp(jl_typeof_str(Var), "DataArray") == 0 ||
      strcmp(jl_typeof_str(Var), "DataVector") == 0 ||
      strcmp(jl_typeof_str(Var), "DataMatrix") == 0)
    return true;
  else
    return false;
}
static bool jl_is_PooledDataArray(jl_value_t *Var)
{
  if (strcmp(jl_typeof_str(Var), "PooledDataArray") == 0 ||
      strcmp(jl_typeof_str(Var), "PooledDataVector") == 0 ||
      strcmp(jl_typeof_str(Var), "PooledDataMatrix") == 0)
    return true;
  else
    return false;
}
static bool jl_is_DataFrame(jl_value_t *Var)
{
  if (strcmp(jl_typeof_str(Var), "DataFrame") == 0)
    return true;
  else
    return false;
}

static bool jl_is_DataArrayFrame(jl_value_t *Var)
{
  if (strcmp(jl_typeof_str(Var), "DataArray") == 0 ||
      strcmp(jl_typeof_str(Var), "DataVector") == 0 ||
      strcmp(jl_typeof_str(Var), "DataMatrix") == 0 ||
      strcmp(jl_typeof_str(Var), "PooledDataArray") == 0 ||
      strcmp(jl_typeof_str(Var), "PooledDataVector") == 0 ||
      strcmp(jl_typeof_str(Var), "PooledDataMatrix") == 0 ||
      strcmp(jl_typeof_str(Var), "DataFrame") == 0 ||
      strcmp(jl_typeof_str(Var), "NAtype") == 0)
    return true;
  else
    return false;
}

static SEXP Julia_R_Scalar(jl_value_t *Var)
{
  SEXP ans = R_NilValue;
  double tmpfloat;
  //most common type is here
  if (jl_is_int32(Var))
  {
    PROTECT(ans = ScalarInteger(jl_unbox_int32(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_int64(Var))
  {
   tmpfloat=(double)jl_unbox_int64(Var);
   if (biginttodouble || !inInt32Range(tmpfloat))
   {
    PROTECT(ans = ScalarReal(tmpfloat));
    UNPROTECT(1);
   }
   else
   {
     PROTECT(ans = ScalarInteger((int32_t)jl_unbox_int64(Var)));
     UNPROTECT(1);
    }
  }
  //more integer type
  if (jl_is_uint32(Var))
  {
   tmpfloat=(double)jl_unbox_uint32(Var);
   if (biginttodouble || !inInt32Range(tmpfloat))
   {
    PROTECT(ans = ScalarReal(tmpfloat));
    UNPROTECT(1);
   }
   else
   {
     PROTECT(ans = ScalarInteger((int32_t)jl_unbox_uint32(Var)));
     UNPROTECT(1);
   }
  }
  else if (jl_is_uint64(Var))
  {
   tmpfloat=(double)jl_unbox_uint64(Var);
  if (biginttodouble || !inInt32Range(tmpfloat))
   {
    PROTECT(ans = ScalarReal(tmpfloat));
    UNPROTECT(1);
   }
   else
   {
     PROTECT(ans = ScalarInteger((int32_t)jl_unbox_uint64(Var)));
     UNPROTECT(1);
   }
  }
  else if (jl_is_float64(Var))
  {
    PROTECT(ans = ScalarReal(jl_unbox_float64(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_float32(Var))
  {
    PROTECT(ans = ScalarReal(jl_unbox_float32(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_bool(Var))
  {
    PROTECT(ans = ScalarLogical(jl_unbox_bool(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_int8(Var))
  {
    PROTECT(ans = ScalarInteger(jl_unbox_int8(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_uint8(Var))
  {
    PROTECT(ans = ScalarInteger(jl_unbox_uint8(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_int16(Var))
  {
    PROTECT(ans = ScalarInteger(jl_unbox_int16(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_uint16(Var))
  {
    PROTECT(ans = ScalarInteger(jl_unbox_uint16(Var)));
    UNPROTECT(1);
  }
  else if (jl_is_utf8_string(Var))
  {
    PROTECT(ans = allocVector(STRSXP, 1));
    SET_STRING_ELT(ans, 0, mkCharCE(jl_string_data(Var), CE_UTF8));
    UNPROTECT(1);
  }
  else if (jl_is_ascii_string(Var))
  {
    PROTECT(ans = ScalarString(mkChar(jl_string_data(Var))));
    UNPROTECT(1);
  }
  return ans;
}

static SEXP Julia_R_MD(jl_value_t *Var)
{
  SEXP ans = R_NilValue;
  //get Julia dims and set R array Dims
  int len = jl_array_len(Var);
  if (len == 0)
    return ans;

  jl_datatype_t *vartype=jl_array_eltype(Var);
  int ndims = jl_array_ndims(Var);
  SEXP dims;
  PROTECT(dims = allocVector(INTSXP, ndims));
  for (size_t i = 0; i < ndims; i++)
  {
    INTEGER(dims)[i] = jl_array_dim(Var, i);
  }
  UNPROTECT(1);

  if (jl_bool_type==vartype)
  {
    char *p = (char *) jl_array_data(Var);
    PROTECT(ans = allocArray(LGLSXP, dims));
    for (size_t i = 0; i < len; i++)
      LOGICAL(ans)[i] = p[i];
    UNPROTECT(1);
  }
  else if (jl_int32_type==vartype)
  {
    int32_t *p = (int32_t *) jl_array_data(Var);
    jlint_to_r;
  }
  //int64
  else if (jl_int64_type==vartype)
  {
    int64_t *p = (int64_t *) jl_array_data(Var);
    if (biginttodouble)
     {jlfloat_to_r;}
    else
     {jlbigint_to_r;}
  }
  //more integer type
  else if (jl_int8_type==vartype)
  {
    int8_t *p = (int8_t *) jl_array_data(Var);
    jlint_to_r;
  }
  else if (jl_int16_type==vartype)
  {
    int16_t *p = (int16_t *) jl_array_data(Var);
    jlint_to_r;
  }
  else if (jl_uint8_type==vartype)
  {
    uint8_t *p = (uint8_t *) jl_array_data(Var);
    jlint_to_r;
  }
  else if (jl_uint16_type==vartype)
  {
    uint16_t *p = (uint16_t *) jl_array_data(Var);
    jlint_to_r;
  }
  else if (jl_uint32_type==vartype)
  {
    uint32_t *p = (uint32_t *) jl_array_data(Var);
    if (biginttodouble)
     {jlfloat_to_r;}
    else
     {jlbigint_to_r;}
  }
  else if (jl_uint64_type==vartype)
  {
    uint64_t *p = (uint64_t *) jl_array_data(Var);
    if (biginttodouble)
     {jlfloat_to_r;}
    else
     {jlbigint_to_r;}
  }
  //double
  else if (jl_float64_type==vartype)
  {
    double *p = (double *) jl_array_data(Var);
    jlfloat_to_r;
  }
  else if (jl_float32_type==vartype)
  {
    float *p = (float *) jl_array_data(Var);
    jlfloat_to_r;
  }
  //convert string array to STRSXP ,but not sure it is corret?
  else if (jl_utf8_string_type==vartype)
  {
    PROTECT(ans = allocArray(STRSXP, dims));
    for (size_t i = 0; i < len; i++)
      SET_STRING_ELT(ans, i, mkCharCE(jl_string_data(jl_cellref(Var, i)), CE_UTF8));
    UNPROTECT(1);
  }
  else if (jl_ascii_string_type==vartype)
  {
    PROTECT(ans = allocArray(STRSXP, dims));
    for (size_t i = 0; i < len; i++)
      SET_STRING_ELT(ans, i, mkChar(jl_string_data(jl_cellref(Var, i))));
    UNPROTECT(1);
  }
  return ans;
}

static SEXP Julia_R_Scalar_NA(jl_value_t *Var)
{
  SEXP ans;
  PROTECT(ans = ScalarInteger(NA_INTEGER));
  UNPROTECT(1);
  return ans;
}

static SEXP Julia_R_MD_NA(jl_value_t *Var)
{
  SEXP ans = R_NilValue;
  char *strData = "Varname0tmp.data";
  char *strNA = "bitunpack(Varname0tmp.na)";
  jl_set_global(jl_main_module, jl_symbol("Varname0tmp"), (jl_value_t *)Var);
  jl_value_t *retData = jl_eval_string(strData);
  jl_value_t *retNA = jl_eval_string(strNA);
  JL_GC_PUSH2(&retData,&retNA);


  int len = jl_array_len(retData);
  if (len == 0)
  {
    JL_GC_POP();
    return ans;
  }

  jl_datatype_t *vartype=jl_array_eltype(retData);
  int ndims = jl_array_ndims(retData);
  SEXP dims;
  PROTECT(dims = allocVector(INTSXP, ndims));
  for (size_t i = 0; i < ndims; i++)
    INTEGER(dims)[i] = jl_array_dim(retData, i);
  UNPROTECT(1);

  //bool array
  char *pNA = (char *) jl_array_data(retNA);

  if (jl_bool_type==vartype)
  {
    char *p = (char *) jl_array_data(retData);
    PROTECT(ans = allocArray(LGLSXP, dims));
    for (size_t i = 0; i < len; i++)
      if (pNA[i])
        LOGICAL(ans)[i] = NA_LOGICAL;
      else
        LOGICAL(ans)[i] = p[i];
    UNPROTECT(1);
  }
  else if (jl_int32_type==vartype)
  {
    int32_t *p = (int32_t *) jl_array_data(retData);
    jlint_to_r_na;
  }
  //int64
  else if (jl_int64_type==vartype)
  {
    int64_t *p = (int64_t *) jl_array_data(retData);
    if (biginttodouble)
     {jlfloat_to_r_na;}
    else
     {jlbigint_to_r_na;}
  }
  //more integer type
  else if (jl_int8_type==vartype)
  {
    int8_t *p = (int8_t *) jl_array_data(retData);
    jlint_to_r_na;
  }
  else if (jl_int16_type==vartype)
  {
    int16_t *p = (int16_t *) jl_array_data(retData);
    jlint_to_r_na;
  }
  else if (jl_uint8_type==vartype)
  {
    uint8_t *p = (uint8_t *) jl_array_data(retData);
    jlint_to_r_na;
  }
  else if (jl_uint16_type==vartype)
  {
    uint16_t *p = (uint16_t *) jl_array_data(retData);
    jlint_to_r_na;
  }
  else if (jl_uint32_type==vartype)
  {
    uint32_t *p = (uint32_t *) jl_array_data(retData);
    if (biginttodouble)
     {jlfloat_to_r_na;}
    else
    {jlbigint_to_r_na;}
  }
  else if (jl_uint64_type==vartype)
  {
    uint64_t *p = (uint64_t *) jl_array_data(retData);
    if (biginttodouble)
     {jlfloat_to_r_na;}
    else
     {jlbigint_to_r_na;}
  }
  //double
  else if (jl_float64_type==vartype)
  {
    double *p = (double *) jl_array_data(retData);
    jlfloat_to_r_na;
  }
  else if (jl_float32_type==vartype)
  {
    float *p = (float *) jl_array_data(retData);
    jlfloat_to_r_na;
  }
  //convert string array to STRSXP
  else if (jl_utf8_string_type==vartype)
  {
    PROTECT(ans = allocArray(STRSXP, dims));
    for (size_t i = 0; i < len; i++)
      if (pNA[i])
        SET_STRING_ELT(ans, i, NA_STRING);
      else
        SET_STRING_ELT(ans, i, mkCharCE(jl_string_data(jl_cellref(retData, i)), CE_UTF8));
    UNPROTECT(1);
  }
  else if (jl_ascii_string_type==vartype)
  {
    PROTECT(ans = allocArray(STRSXP, dims));
    for (size_t i = 0; i < len; i++)
      if (pNA[i])
        SET_STRING_ELT(ans, i, NA_STRING);
      else
        SET_STRING_ELT(ans, i, mkChar(jl_string_data(jl_cellref(retData, i))));
    UNPROTECT(1);
  }
  JL_GC_POP();
  jl_eval_string("Varname0tmp=0;");
  return ans;
}

//this function is for factor convert it maybe not safe
//because PooledDataArray.refs is Uint32 or bigger
//but in pratice it should be ok
static SEXP Julia_R_MD_INT(jl_value_t *Var)
{
  SEXP ans = R_NilValue;

  int len = jl_array_len(Var);
  if (len == 0)
  {
   return ans;
  }

  jl_datatype_t *vartype=jl_array_eltype(Var);
  if (jl_int32_type==vartype)
  {
    int32_t *p = (int32_t *) jl_array_data(Var);
    jlint_to_r_md;
  }
  else if (jl_int64_type==vartype)
  {
    int64_t *p = (int64_t *) jl_array_data(Var);
    jlint_to_r_md;
  }
  else if (jl_int8_type==vartype)
  {
    int8_t *p = (int8_t *) jl_array_data(Var);
    jlint_to_r_md;
  }
  else if (jl_int16_type==vartype)
  {
    int16_t *p = (int16_t *) jl_array_data(Var);
    jlint_to_r_md;
  }
  else if (jl_uint8_type==vartype)
  {
    uint8_t *p = (uint8_t *) jl_array_data(Var);
    jlint_to_r_md;
  }
  else if (jl_uint16_type==vartype)
  {
    uint16_t *p = (uint16_t *) jl_array_data(Var);
    jlint_to_r_md;
  }
  else if (jl_uint32_type==vartype)
  {
    uint32_t *p = (uint32_t *) jl_array_data(Var);
    jlint_to_r_md;
  }
  else if (jl_uint64_type==vartype)
  {
    uint64_t *p = (uint64_t *) jl_array_data(Var);
    jlint_to_r_md;
  }
  return ans;
}

static SEXP Julia_R_MD_NA_Factor(jl_value_t *Var)
{
  SEXP ans = R_NilValue;
  char *strData = "Varname0tmp.refs";
  char *strlevels = "Varname0tmp.pool";
  jl_set_global(jl_main_module, jl_symbol("Varname0tmp"), (jl_value_t *)Var);
  jl_value_t *retData = jl_eval_string(strData);
  jl_value_t *retlevels = jl_eval_string(strlevels);
  JL_GC_PUSH2(&retData,&retlevels);
  //first get refs data,dims=n
  //caution this convert to int32 SEXP,it should be ok in reality,
  //but if have a lot factor may be cause int32 overrun.
  ans = Julia_R_MD_INT(retData);
  PROTECT(ans);
  //second setAttrib R levels and class
  SEXP levels = Julia_R_MD(retlevels);
  JL_GC_POP();
  jl_eval_string("Varname0tmp=0");
  setAttrib(ans, R_LevelsSymbol, levels);
  setAttrib(ans, R_ClassSymbol, mkString("factor"));
  UNPROTECT(1);
  return ans;
}

static SEXP Julia_R_MD_NA_DataFrame(jl_value_t *Var)
{
  SEXP ans, names, rownames;
  char evalcmd[evalsize];
  int i;
  const char *dfname = "DataFrameName0tmp";
  jl_set_global(jl_main_module, jl_symbol(dfname), (jl_value_t *)Var);
  //Get Frame cols
  snprintf(evalcmd, evalsize, "size(%s,2)", dfname);
  //gc args
  jl_value_t *eachcolvector=NULL;
  jl_value_t *ret=NULL;
  JL_GC_PUSH2(&eachcolvector,&ret);

  int collen = jl_unbox_long(jl_eval_string(evalcmd));
  //Create SEXP for Each Column and assign
  PROTECT(ans = allocVector(VECSXP, collen));
  for (i = 0; i < collen; i++)
  {
    snprintf(evalcmd, evalsize, "%s[%d]", dfname, i + 1);
    eachcolvector = jl_eval_string(evalcmd);
    snprintf(evalcmd, evalsize, "isa(%s[%d],PooledDataArray)", dfname, i + 1);
    if (jl_unbox_bool(jl_eval_string(evalcmd)))
      SET_VECTOR_ELT(ans, i, Julia_R_MD_NA_Factor(eachcolvector));
    else
      SET_VECTOR_ELT(ans, i, Julia_R_MD_NA(eachcolvector));
  }
  //set names attribute
  snprintf(evalcmd, evalsize, "names(%s)", dfname);
  ret = jl_eval_string(evalcmd);
  if (jl_is_array(ret))
  {
    PROTECT(names = allocVector(STRSXP, collen));
    for (i = 0; i < jl_array_len(ret); i++)
    {
      if (jl_is_symbol(jl_arrayref((jl_array_t *)ret, i)))
        SET_STRING_ELT(names, i, mkChar(((jl_sym_t *)jl_arrayref((jl_array_t *)ret, i))->name));
    }
    setAttrib(ans, R_NamesSymbol, names);
    UNPROTECT(1);
  }
  JL_GC_POP();
  //set row names
  snprintf(evalcmd, evalsize, "size(%s,1)", dfname);
  int rowlen=jl_unbox_long(jl_eval_string(evalcmd));
  PROTECT(rownames = allocVector(INTSXP, rowlen));
  for (i = 0; i < rowlen; i++)
    INTEGER(rownames)[i] = i + 1;
  setAttrib(ans, R_RowNamesSymbol, rownames);
  UNPROTECT(1);
  //set class as data frame
  setAttrib(ans, R_ClassSymbol, mkString("data.frame"));
  UNPROTECT(1);
  jl_eval_string("DataFrameName0tmp=0;");
  return ans;
}

//Convert Julia Type To R,Real interface
SEXP Julia_R(jl_value_t *Var)
{
  SEXP ans = R_NilValue;
  if (jl_is_nothing(Var))
    return ans;

  //Array To Vector
  JL_GC_PUSH1(&Var);
  if (jl_is_array(Var))
  {
    ans = Julia_R_MD(Var);
  }
  else if (jl_is_DataArrayFrame(Var))
  {
    //try to load DataArrays DataFrames package
    if (!LoadDF())
    {
      JL_GC_POP();
      return R_NilValue;
    }
    if (jl_is_NAtype(Var))
      ans = Julia_R_Scalar_NA(Var);
    else if (jl_is_DataFrame(Var))
      ans = Julia_R_MD_NA_DataFrame(Var);
    else if (jl_is_DataArray(Var))
      ans = Julia_R_MD_NA(Var);
    else if (jl_is_PooledDataArray(Var))
      ans = Julia_R_MD_NA_Factor(Var);
  }
  else if (jl_is_svec(Var))
  {
      PROTECT(ans = allocVector(VECSXP, jl_svec_len(Var)));
      for (int i = 0; i < jl_svec_len(Var); i++)
        SET_ELEMENT(ans, i, Julia_R(jl_svecref(Var, i)));
      UNPROTECT(1);
  }
  else
    ans = Julia_R_Scalar(Var);
  JL_GC_POP();
  return ans;
}
