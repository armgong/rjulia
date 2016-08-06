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

// Handle and report exception
static int rjulia_exception_occurred() {
  if (jl_exception_occurred()) {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    return(1);
  } else {
    return(0);
  }
}

//convert R SEXP dims to julia tuple
static jl_value_t *RDims_JuliaTuple(SEXP Var)
{
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
  JL_GC_PUSH1(&ret);
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
  JL_GC_POP();
  return(ret);
}

//convert R object to julia object
//Var is R object
static jl_array_t *R_Julia_MD(SEXP Var)
{
   jl_array_t *ret = NewArray(Var);
   JL_GC_PUSH1(&ret);
   switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      char *retData = (char *)jl_array_data(ret);
      int *var_p = LOGICAL(Var);
      for (size_t i = 0; i < jl_array_len(ret); i++) // Can not be memcpy because we need to cast from int32 to int8
	retData[i] = (char)var_p[i];  // No write barrier, right?
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
      for (size_t i = 0; i < jl_array_len(ret); i++) {
	retData[i] = (jl_value_t *)jl_cstr_to_string(translateCharUTF8(STRING_ELT(Var, i)));
	jl_gc_wb(ret, retData[i]);
      }
      break;
    }
    case VECSXP:
    {
      jl_value_t **retData = jl_array_data(ret);
      for (int i = 0; i < jl_array_len(ret); i++) {
	retData[i] = (jl_value_t *)R_Julia_MD(VECTOR_ELT(Var,i));
	jl_gc_wb(ret, retData[i]);
      }
      break;
    }
    default:
    {
      jl_error("Invalid array type. rjulia supports the array types LGLSXP, INTSXP, REALSXP, STRSXP and VECSXP.\n");
    }
   }
  JL_GC_POP();
  return ret;
}

//convert R object contain NA value to Julia DataArrays
static jl_value_t *R_Julia_MD_NA(SEXP Var, SEXP na)
{
  jl_function_t *func = jl_get_function(jl_main_module, "DataArray");
  jl_array_t *ret1 = NULL;
  jl_array_t *ret2 = NULL;
  jl_value_t *ans  = NULL;
  JL_GC_PUSH3(&ret1, &ret2, &ans);
  ret1 = R_Julia_MD(Var);
  ret2 = R_Julia_MD(na);
  ans  = jl_call2(func, (jl_value_t *)ret1, (jl_value_t *)ret2);
  if (rjulia_exception_occurred())
    ans =  (jl_value_t *) jl_nothing;
  JL_GC_POP();
  return ans;
 }

//convert R factor to Julia PooledDataArray
static jl_value_t *R_Julia_MD_NA_Factor(SEXP Var)
{
  SEXP levels = getAttrib(Var, R_LevelsSymbol);
  size_t nlevels = LENGTH(levels);
  size_t len = LENGTH(Var);
  if (len == 0 || TYPEOF(Var) != INTSXP || levels == R_NilValue)
    return (jl_value_t *) jl_nothing;

  //create string array for levels in julia
  jl_value_t *ans=(jl_value_t *) jl_nothing;
  jl_array_t *ret=NULL;
  jl_array_t *new_levels=NULL;
  jl_array_t *na_vec=NULL;
  JL_GC_PUSH4(&ret, &new_levels, &ans, &na_vec);
  ret = jl_alloc_array_1d(jl_apply_array_type(jl_string_type, 1), len);
  new_levels = jl_alloc_array_1d(jl_apply_array_type(jl_string_type,1), nlevels);

  // Collect levels
  jl_value_t **retData1 = jl_array_data(new_levels);
  for (size_t i = 0; i < nlevels; i++) {
    retData1[i] = jl_cstr_to_string(translateCharUTF8(STRING_ELT(levels, i)));
    jl_gc_wb(retData1, retData1[i]);
  }

  // Collect string vector
  na_vec = jl_alloc_array_1d( jl_apply_array_type(jl_bool_type,1), LENGTH(Var));
  char *na_data = (char *)jl_array_data(na_vec);
  int level_index;

  for (size_t i = 0; i < jl_array_len(ret); i++)
    {
      level_index = INTEGER(Var)[i];
      if (INTEGER(Var)[i] == NA_INTEGER) {
	jl_arrayset(ret, retData1[0], i);  // Will get wiped out by NA
	na_data[i] = 1;
	jl_gc_wb(ret,retData1[0]);
      } else {
	jl_arrayset(ret, retData1[level_index - 1], i);
	na_data[i] = 0;
	jl_gc_wb(ret,retData1[level_index - 1]);
      }
    }

  // Make DataArray
  jl_function_t *func = jl_get_function(jl_main_module, "DataArray");
  ans = jl_call2(func, (jl_value_t *)ret, (jl_value_t *)na_vec);

  // Make PooledDataArray using DataArray and levels
  func = jl_get_function(jl_main_module, "PooledDataArray");
  ans = jl_call2(func, (jl_value_t *)ans, (jl_value_t *)new_levels);  

  if (rjulia_exception_occurred())
    ans =  (jl_value_t *) jl_nothing;

  JL_GC_POP();
  return ans;
}

//convert R DataFrame to Julia DataFrame in DataFrames package
static jl_value_t *R_Julia_MD_NA_DataFrame(SEXP Var, SEXP na)
{
  size_t len = LENGTH(Var);
  SEXP names = getAttrib(Var, R_NamesSymbol);

  if (TYPEOF(Var) != VECSXP || len == 0 || names == R_NilValue)
    return (jl_value_t *) jl_nothing;

  jl_array_t *col_names;
  jl_array_t *col_list;
  JL_GC_PUSH2(&col_list, &col_names);
  col_list = jl_alloc_array_1d(jl_array_any_type, len);  // Vector{Any} to hold df columns
  col_names = jl_alloc_array_1d(jl_array_symbol_type, len); // Vector{Symbol} to hold df names

  // Does putting these pointers in this Vector{Any} require a GC write barrier?
  for (int i = 0; i < len; i++) {
    jl_arrayset(col_names, (jl_value_t *)jl_symbol( CHAR(STRING_ELT(names,i)) ), i);
    SEXP data_elt = VECTOR_ELT(Var,i);
    if (isFactor(data_elt)) {
      jl_arrayset(col_list, R_Julia_MD_NA_Factor(data_elt), i);
    } else  {
      jl_arrayset(col_list, R_Julia_MD_NA(data_elt, VECTOR_ELT(na,i)), i);
    }
  }
  jl_function_t *func = jl_get_function(jl_main_module, "DataFrame");
  jl_value_t *ret = jl_call2(func, (jl_value_t *)col_list, (jl_value_t *)col_names);
      
  if (rjulia_exception_occurred())
    ret =  (jl_value_t *) jl_nothing;
  
  JL_GC_POP();  
  return ret;
}

//Convert R Type To Julia,which not contain NA
SEXP R_Julia(SEXP Var, SEXP VarName)
{
  jl_array_t *ans;
  JL_GC_PUSH1(&ans);
  ans = R_Julia_MD(Var);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), (jl_value_t *)ans);
  JL_GC_POP();
  return R_NilValue;
}

//Convert R Type To Julia,which contain NA
SEXP R_Julia_NA(SEXP Var, SEXP na, SEXP VarName)
{
  jl_value_t *ans;
  JL_GC_PUSH1(&ans);
  ans = R_Julia_MD_NA(Var, na);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), ans);
  JL_GC_POP();
  return R_NilValue;
}
//Convert R factor To Julia, which contain NA
SEXP R_Julia_NA_Factor(SEXP Var, SEXP VarName)
{
  LoadDF();
  jl_value_t *ans;
  JL_GC_PUSH1(&ans);
  ans = R_Julia_MD_NA_Factor(Var);
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
