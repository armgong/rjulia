/*
Copyright (C) 2014, 2015 by Yu Gong
*/

//this file is for conver R object to julia
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <R.h>
#define USE_RINTERNALS
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include "dataframe.h"
#include "R_Julia.h"
#define pkgdebug

//convert R SEXP dims to julia tuple
static jl_value_t *RDims_JuliaTuple(SEXP Var)
{
  /*
 jl_value_t *d = NULL;
 JL_GC_PUSH1(&d);
 SEXP dims = getAttrib(Var, R_DimSymbol);
  //array or matrix
  int ndims = LENGTH(dims);
  d = jl_alloc_svec(ndims);
  for (size_t i = 0; i < ndims; i++)
  {
  jl_svecset(d, i, jl_box_long(INTEGER(dims)[i]));
  }
  return d;
  */

  SEXP dims = getAttrib(Var, R_DimSymbol);
  char evalcmd[evalsize];
  char eltcmd[eltsize];
  snprintf(evalcmd, evalsize, "%s","(");
  int ndims = LENGTH(dims);
  for (size_t i = 0; i < ndims; i++)
    {
      snprintf(eltcmd,eltsize,"%d,",INTEGER(dims)[i]);
      strcat(evalcmd,eltcmd);
    }
  strcat(evalcmd,")");
  return jl_eval_string(evalcmd);
}

//create an Array in julia
static jl_array_t* CreateArray(jl_datatype_t *type, size_t ndim, jl_value_t *dims)
{
  return jl_new_array(jl_apply_array_type(type, ndim), dims);;
}

// Pick type for array element
static jl_datatype_t* ElementType(SEXP Var) {
  jl_datatype_t *eltype = jl_any_type;
   switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      eltype = jl_bool_type;
      break;
    };
    case INTSXP:
    {
      eltype = jl_int32_type;
      break;
    }
    case REALSXP:
    {
      eltype = jl_float64_type;
      break;
    }
    case STRSXP:
    {
      eltype = jl_string_type;
    }
   }
  return(eltype);
}
  
// Alternate array creator starting from R SEXP
static jl_array_t* NewArray(SEXP Var) {
  jl_datatype_t *eltype = ElementType(Var);
  jl_array_t *ret = NULL;
  if (isMatrix(Var)) {
    jl_value_t* array_type = jl_apply_array_type(eltype, 2);
    ret = jl_alloc_array_2d(array_type, nrows(Var), ncols(Var));
  } else if (isArray(Var)) {
    jl_value_t *dims = RDims_JuliaTuple(Var);
    ret = CreateArray(eltype, jl_nfields(dims), dims);
  } else { // isVector and isVectorAtomic do not mean what one would expect
     jl_value_t* array_type = jl_apply_array_type(eltype, 1);
     ret = jl_alloc_array_1d(array_type, LENGTH(Var));
  }
  return(ret);
}

//convert R object to julia object
//Var is R object
static jl_value_t *R_Julia_MD(SEXP Var)
{
   jl_array_t *ret = NewArray(Var);
   JL_GC_PUSH1(&ret);
   switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      char *retData = (char *)jl_array_data(ret);
      int *var_p = LOGICAL(Var);
      for (size_t i = 0; i < jl_array_len(ret); i++) // Can not be memcpy because we want the implicit cast from int32 to int8
        retData[i] = var_p[i];
      break;
    };
    case INTSXP:
    {
      int *retData = (int *)jl_array_data(ret);
      memcpy(retData, INTEGER(Var), jl_array_len(ret) * sizeof(int));
      break;
    }
    case REALSXP:
    {
      double *retData = (double *)jl_array_data(ret);
      memcpy(retData, REAL(Var), jl_array_len(ret) * sizeof(double));
      break;
    }
    case STRSXP:
    {
      jl_value_t **retData = jl_array_data(ret);
	for (size_t i = 0; i < jl_array_len(ret); i++)
          retData[i] = jl_cstr_to_string(translateCharUTF8(STRING_ELT(Var, i)));
      break;
    }
    case VECSXP:
    {
      jl_value_t **retData = jl_array_data(ret);
      // Does putting these pointers in this Vector{Any} require a GC write barrier?
      for (int i = 0; i < jl_array_len(ret); i++)
      {
	     retData[i] = R_Julia_MD(VECTOR_ELT(Var,i));
      }
      break;
    }
    default:
    {
      JL_GC_POP();
      return (jl_value_t *)jl_nothing;
    }
   }
  JL_GC_POP();
  return (jl_value_t *)ret;
}

//convert R object contain NA value to Julia DataArrays
static jl_value_t *R_Julia_MD_NA(SEXP Var, SEXP na)
{
  jl_function_t *func = jl_get_function(jl_base_module, "DataArray");
  jl_value_t *ret1  = NULL;
  jl_value_t *ret2 = NULL;
  jl_value_t *ans  = NULL;
  JL_GC_PUSH3(&ret1, &ret2, &ans);
  ret1  = (jl_value_t *)NewArray(Var);
  ret2 = (jl_value_t *)NewArray(na);
  ans = jl_call2(func, ret1, ret2);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    ans = (jl_value_t *) jl_nothing;
  }
  JL_GC_POP();
  return ans;
 }

//convert R factor to Julia PooledDataArray
static jl_value_t *R_Julia_MD_NA_Factor(SEXP Var, SEXP na)
{
  SEXP levels = getAttrib(Var, R_LevelsSymbol);
  if ((LENGTH(Var))== 0 || TYPEOF(Var) != INTSXP || levels == R_NilValue)
    return (jl_value_t *) jl_nothing;

  //create string array for levels in julia
  jl_value_t *ans=(jl_value_t *) jl_nothing;
  jl_array_t *ret=NULL;
  jl_array_t *ret1=NULL;
  ret1 = jl_alloc_array_1d(jl_apply_array_type(jl_string_type,1), LENGTH(levels));
  jl_value_t **retData1 = jl_array_data(ret1);
  JL_GC_PUSH3(&ret, &ret1,&ans);

  for (size_t i = 0; i < jl_array_len(ret1); i++)
     retData1[i] = jl_cstr_to_string(translateCharUTF8(STRING_ELT(levels, i)));

  ret = jl_alloc_array_1d(jl_apply_array_type(jl_uint32_type, 1), LENGTH(Var));
  int *retData = (int *)jl_array_data(ret);
  for (size_t i = 0; i < jl_array_len(ret); i++)
    {
      if (INTEGER(Var)[i] == NA_INTEGER)
	retData[i] = 0; //NA in poolarray is 0
      else
	retData[i] = INTEGER(Var)[i];
    }
  jl_function_t *func = jl_get_function(jl_base_module, "PooledDataArray");
  ans = jl_call2(func, (jl_value_t *)ret, (jl_value_t *)ret1);  

  if (jl_exception_occurred())
    {
      jl_show(jl_stderr_obj(), jl_exception_occurred());
      Rprintf("\n");
      jl_exception_clear();
      ans =  (jl_value_t *) jl_nothing;
    }

  JL_GC_POP();
  return ans;
}

//convert R DataFrame to Julia DataFrame in DataFrames package
static jl_value_t *R_Julia_MD_NA_DataFrame(SEXP Var, SEXP na)
{
  SEXP names = getAttrib(Var, R_NamesSymbol);
  size_t len = LENGTH(Var);
  if (TYPEOF(Var) != VECSXP || len == 0 || names == R_NilValue)
    return (jl_value_t *) jl_nothing;

  jl_array_t *col_names;
  jl_array_t *col_list;
  JL_GC_PUSH2(&col_list, &col_names);
  col_list = jl_alloc_array_1d(jl_array_any_type, len);  // Vector{Any} to hold df columns
  col_names = jl_alloc_array_1d(jl_array_symbol_type, len); // Vector{Symbol} to hold df names
  
  // Does putting these pointers in this Vector{Any} require a GC write barrier?
  jl_value_t **colsData = jl_array_data(col_list);
  for (int i = 0; i < len; i++) {
    jl_arrayset(col_names, (jl_value_t *)jl_symbol( CHAR(STRING_ELT(names,i)) ), i);
    SEXP data_elt = VECTOR_ELT(col_list,i);
    if (getAttrib(data_elt, R_LevelsSymbol) != R_NilValue)
      colsData[i] = R_Julia_MD_NA_Factor(data_elt, VECTOR_ELT(na,i));
    else
      colsData[i] = R_Julia_MD_NA(data_elt, VECTOR_ELT(na,i));
  }
  
  jl_function_t *func = jl_get_function(jl_base_module, "DataFrame");
  jl_value_t *ret = jl_call2(func, (jl_value_t *)col_list, (jl_value_t *)col_names);
  JL_GC_POP();
  if (jl_exception_occurred())
    {
      jl_show(jl_stderr_obj(), jl_exception_occurred());
      Rprintf("\n");
      jl_exception_clear();
      return (jl_value_t *)jl_nothing;
    }
  return ret;
}

//Convert R Type To Julia,which not contain NA
SEXP R_Julia(SEXP Var, SEXP VarName)
{
  jl_value_t *ans;
  JL_GC_PUSH1(&ans);
  ans = R_Julia_MD(Var);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), ans);
  JL_GC_POP();
  return R_NilValue;
}

//Convert R Type To Julia,which contain NA
SEXP R_JuliaNA(SEXP Var, SEXP na, SEXP VarName)
{
  jl_value_t *ans;
  JL_GC_PUSH1(&ans);
  ans = R_Julia_MD_NA(Var, na);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), ans);
  JL_GC_POP();
  return R_NilValue;
}
//Convert R factor To Julia, which contain NA
SEXP R_Julia_NA_Factor(SEXP Var, SEXP na, SEXP VarName)
{
  LoadDF();
  jl_value_t *ans;
  JL_GC_PUSH1(&ans);
  ans = R_Julia_MD_NA_Factor(Var, na);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), ans);
  JL_GC_POP();
  return R_NilValue;
}
//Convert R data frame To Julia
SEXP R_Julia_NA_DataFrame(SEXP Var, SEXP na, SEXP VarName)
{
  LoadDF();
  jl_value_t *ans;
  JL_GC_PUSH1(&ans);
  ans = R_Julia_MD_NA_DataFrame(Var, na);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), ans);
  JL_GC_POP();
  return R_NilValue;
}
