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
  return jl_new_array(jl_apply_array_type((jl_value_t*)type, ndim), dims);;
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
      break;
    }
   }
  return(eltype);
}

// Alternate array creator starting from R SEXP
static jl_array_t* NewArray(SEXP Var) {
  jl_datatype_t *eltype = ElementType(Var);
  if (isMatrix(Var)) {
    jl_value_t* array_type = jl_apply_array_type((jl_value_t*)eltype, 2);
    ret = jl_alloc_array_2d(array_type, nrows(Var), ncols(Var));
  } else if (isArray(Var)) {
    jl_value_t *dims = RDims_JuliaTuple(Var);
    return CreateArray(eltype, jl_nfields(dims), dims);
  } else { // isVector and isVectorAtomic do not mean what one would expect
     jl_value_t* array_type = jl_apply_array_type((jl_value_t*)eltype, 1);
     ret = jl_alloc_array_1d(array_type, LENGTH(Var));
  }
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
      for (size_t i = 0; i < jl_array_len(ret); i++)
	retData[i] = jl_cstr_to_string(translateCharUTF8(STRING_ELT(Var, i)));
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
   }
  JL_GC_POP();
  return (jl_value_t *)ret;
}

//convert julia array to DataArray in DataArray's package
//this is for R object contain NA, need two pass to finish
//first pass creat array then convert it to DataArray
//second pass assign NA to element
static jl_value_t *TransArrayToDataArray(jl_array_t *mArray, jl_array_t *mboolArray, const char *VarName)
{
  char evalcmd[evalsize];
  jl_set_global(jl_main_module, jl_symbol("TransVarName"), (jl_value_t *)mArray);
  jl_set_global(jl_main_module, jl_symbol("TransVarNamebool"), (jl_value_t *)mboolArray);
  snprintf(evalcmd, evalsize, "%s=DataArray(TransVarName,TransVarNamebool)", VarName);
  jl_value_t *ret = jl_eval_string(evalcmd);
  jl_eval_string("TransVarName=0;TransVarNamebool=0;");
  if (jl_exception_occurred())
  {
    jl_call2(jl_get_function(jl_base_module, "show"), jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    return (jl_value_t *) jl_nothing;
  }
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
  jl_array_t *ret1=NULL;

  ret1 = jl_alloc_array_1d(jl_apply_array_type((jl_value_t*)jl_string_type,1), LENGTH(levels));

  jl_value_t **retData1 = jl_array_data(ret1);
  JL_GC_PUSH3(&ret, &ret1, &ans);

  for (size_t i = 0; i < jl_array_len(ret1); i++)
     retData1[i] = jl_cstr_to_string(translateCharUTF8(STRING_ELT(levels, i)));

  for (size_t i = 0; i < jl_array_len(ret); i++)
    {
      ret = jl_alloc_array_1d(jl_apply_array_type((jl_value_t*)jl_uint32_type, 1), LENGTH(Var));
      int *retData = (int *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (INTEGER(Var)[i] == NA_INTEGER)
        {
          //NA in poolarray is 0
          retData[i] = 0;
        }
        else
        {
          retData[i] = INTEGER(Var)[i];
        }
      }
      ans=TransArrayToPoolDataArray(ret, ret1, LENGTH(Var), VarName);
      break;
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
static jl_value_t *R_Julia_MD_NA_DataFrame(SEXP Var, SEXP na, const char *VarName)
{
  SEXP names = getAttrib(Var, R_NamesSymbol);
  size_t len = LENGTH(Var);
  if (TYPEOF(Var) != VECSXP || len == 0 || names == R_NilValue)
    return (jl_value_t *) jl_nothing;
  char evalcmd[evalsize];
  char eltcmd[eltsize];
  const char *onename;

  SEXP elt;
  jl_value_t *temp;
  for (size_t i = 0; i < len; i++)
    {
      elt = VECTOR_ELT(Var, i);
      snprintf(eltcmd, eltsize, "%sdfelt%lu", VarName, i + 1);
      //vector is factor or not
      if (getAttrib(elt, R_LevelsSymbol) != R_NilValue) {
	temp = R_Julia_MD_NA_Factor(elt);
      } else {
	temp = R_Julia_MD_NA(elt, VECTOR_ELT(na,i));
      }
      jl_set_global(jl_main_module, jl_symbol(eltcmd), (jl_value_t *)temp);
      onename = CHAR(STRING_ELT(names, i));
      if (i == 0) {
	snprintf(evalcmd, evalsize, "%s=DataFrame(%s = %s)", VarName, onename, eltcmd);
      } else {
	snprintf(evalcmd, evalsize, "%s[Symbol(\"%s\")] = %s", VarName, onename, eltcmd);
      }
      jl_eval_string(evalcmd);

      //clear
      snprintf(eltcmd, eltsize, "%sdfelt%lu=0;", VarName, i + 1);
      jl_eval_string(eltcmd);

    if (jl_exception_occurred())
    {
      jl_call2(jl_get_function(jl_base_module, "show"), jl_stderr_obj(), jl_exception_occurred());
      Rprintf("\n");
      jl_exception_clear();
      return (jl_value_t *) jl_nothing;
    }
  return (jl_value_t *) jl_nothing;
}

//Convert R Type To Julia,which not contain NA
SEXP R_Julia(SEXP Var, SEXP VarName)
{
  jl_value_t *ret = R_Julia_MD(Var, CHAR(STRING_ELT(VarName, 0)));
  JL_GC_PUSH1(&ret);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), ret);
  JL_GC_POP();
  return R_NilValue;
}

//Convert R Type To Julia,which contain NA
SEXP R_Julia_NA(SEXP Var, SEXP na, SEXP VarName)
{
  jl_value_t *ans = R_Julia_MD_NA(Var, na);
  JL_GC_PUSH1(&ans);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), ans);
  JL_GC_POP();
  return R_NilValue;
}
//Convert R factor To Julia, which contain NA
SEXP R_Julia_NA_Factor(SEXP Var, SEXP VarName)
{
  LoadDF();
  jl_value_t *ans = R_Julia_MD_NA_Factor(Var);
  JL_GC_PUSH1(&ans);
  jl_set_global(jl_main_module, jl_symbol(CHAR(STRING_ELT(VarName, 0))), ans);
  JL_GC_POP();
  return R_NilValue;
}
//Convert R data frame To Julia
SEXP R_Julia_NA_DataFrame(SEXP Var, SEXP na, SEXP VarName)
{
  LoadDF();
  R_Julia_MD_NA_DataFrame(Var, na, CHAR(STRING_ELT(VarName, 0)));
  return R_NilValue;
}
