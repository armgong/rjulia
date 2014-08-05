/*
Copyright (C) 2014 by Yu Gong
*/
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

#define UTF8_MASK (1<<3)
#define ASCII_MASK (1<<6)
#define IS_ASCII(x) ((x)->sxpinfo.gp & ASCII_MASK)
#define IS_UTF8(x) ((x)->sxpinfo.gp & UTF8_MASK)
static jl_array_t *CreateArray(jl_datatype_t *type, size_t ndim, jl_tuple_t *dims)
{
  return jl_new_array(jl_apply_array_type(type, ndim), dims);;
}

static jl_tuple_t *RDims_JuliaTuple(SEXP Var)
{
  jl_tuple_t *d;
  SEXP dims = getAttrib(Var, R_DimSymbol);
  //array or matrix
  if (dims != R_NilValue)
  {
    int ndims = LENGTH(dims);
    d = jl_alloc_tuple(ndims);
    JL_GC_PUSH1(&d);
    size_t i;
    for (i = 0; i < ndims; i++)
    {
      jl_tupleset(d, i, jl_box_long(INTEGER(dims)[i]));
    }
    JL_GC_POP();
  }
  else     //vector
  {
    d = jl_alloc_tuple(1);
    JL_GC_PUSH1(&d);
    jl_tupleset(d, 0, jl_box_long(LENGTH(Var)));
    JL_GC_POP();
  }
  return d;
}

static jl_value_t *R_Julia_MD(SEXP Var, const char *VarName)
{

  if ((LENGTH(Var)) != 0)
  {
    jl_tuple_t *dims = RDims_JuliaTuple(Var);
    switch (TYPEOF( Var))
    {
    case LGLSXP:
    {
      jl_array_t *ret = CreateArray(jl_bool_type, jl_tuple_len(dims), dims);
      JL_GC_PUSH1(&ret);
      char *retData = (char *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
        retData[i] = LOGICAL(Var)[i];
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      return (jl_value_t *) ret;
      JL_GC_POP();
      break;
    };
    case INTSXP:
    {
      jl_array_t *ret = CreateArray(jl_int32_type, jl_tuple_len(dims), dims);
      JL_GC_PUSH1(&ret);
      int *retData = (int *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
        retData[i] = INTEGER(Var)[i];
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      return (jl_value_t *) ret;
      JL_GC_POP();
      break;
    }
    case REALSXP:
    {
      jl_array_t *ret = CreateArray(jl_float64_type, jl_tuple_len(dims), dims);
      JL_GC_PUSH1(&ret);
      double *retData = (double *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
        retData[i] = REAL(Var)[i];
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      JL_GC_POP();
      return (jl_value_t *) ret;
      break;
    }
    case STRSXP:
    {
      jl_array_t *ret;
      if (!IS_ASCII(Var))
        ret = CreateArray(jl_utf8_string_type, jl_tuple_len(dims), dims);
      else
        ret = CreateArray(jl_ascii_string_type, jl_tuple_len(dims), dims);
      JL_GC_PUSH1(&ret);
      jl_value_t **retData = jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
        if (!IS_ASCII(Var))
          retData[i] = jl_cstr_to_string(translateChar0(STRING_ELT(Var, i)));
        else
          retData[i] = jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      JL_GC_POP();
      return (jl_value_t *) ret;
      break;
    }
    case VECSXP:
    {
      char eltcmd[eltsize];
      jl_tuple_t *ret = jl_alloc_tuple(length(Var));
      JL_GC_PUSH1(&ret);
      for (int i = 0; i < length(Var); i++)
      {
        snprintf(eltcmd, eltsize, "%selement%d", VarName, i);
        jl_tupleset(ret, i, R_Julia_MD(VECTOR_ELT(Var, i), eltcmd));
      }
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      JL_GC_POP();
      return (jl_value_t *) ret;
    }
    default:
    {
      return (jl_value_t *) jl_nothing;
    }
    break;
    }
    return (jl_value_t *) jl_nothing;
  }
  return (jl_value_t *) jl_nothing;
}

//first pass creat array then convert it to DataArray
//second pass assign NA to element
static jl_value_t *TransArrayToDataArray(jl_array_t *mArray, jl_array_t *mboolArray, const char *VarName)
{ 
  char evalcmd[evalsize];
  jl_set_global(jl_main_module, jl_symbol("TransVarName"), (jl_value_t *)mArray);
  jl_set_global(jl_main_module, jl_symbol("TransVarNamebool"), (jl_value_t *)mboolArray);
  snprintf(evalcmd, evalsize, "%s=DataArray(TransVarName,TransVarNamebool)", VarName);
  jl_value_t *ret = jl_eval_string(evalcmd);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    return (jl_value_t *) jl_nothing;
  }
  return ret;
}

static jl_value_t *R_Julia_MD_NA(SEXP Var, const char *VarName)
{
  if ((LENGTH(Var)) != 0)
  {
    jl_tuple_t *dims = RDims_JuliaTuple(Var);

    switch (TYPEOF(Var))
    {
    case LGLSXP:
    {
      jl_array_t *ret = CreateArray(jl_bool_type, jl_tuple_len(dims), dims);
      jl_array_t *ret1 = CreateArray(jl_bool_type, jl_tuple_len(dims), dims);
      JL_GC_PUSH(&ret, &ret1);

      char *retData = (char *)jl_array_data(ret);
      bool *retData1 = (bool *)jl_array_data(ret1);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (LOGICAL(Var)[i] == NA_LOGICAL)
        {
          retData[i] = 1;
          retData1[i] = true;
        }
        else
        {
          retData[i] = LOGICAL(Var)[i];
          retData1[i] = false;
        }
      }
      JL_GC_POP();
      return TransArrayToDataArray(ret, ret1, VarName);
      break;
    };
    case INTSXP:
    {
      jl_array_t *ret = CreateArray(jl_int32_type, jl_tuple_len(dims), dims);
      jl_array_t *ret1 = CreateArray(jl_bool_type, jl_tuple_len(dims), dims);
      JL_GC_PUSH(&ret, &ret1);

      int *retData = (int *)jl_array_data(ret);
      bool *retData1 = (bool *)jl_array_data(ret1);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (INTEGER(Var)[i] == NA_INTEGER)
        {
          retData[i] = 999;
          retData1[i] = true;
        }
        else
        {
          retData[i] = INTEGER(Var)[i];
          retData1[i] = false;
        }
      }
      JL_GC_POP();
      return TransArrayToDataArray(ret, ret1, VarName);
      break;
    }
    case REALSXP:
    {
      jl_array_t *ret = CreateArray(jl_float64_type, jl_tuple_len(dims), dims);
      jl_array_t *ret1 = CreateArray(jl_bool_type, jl_tuple_len(dims), dims);
      JL_GC_PUSH(&ret, &ret1);
      double *retData = (double *)jl_array_data(ret);
      bool *retData1 = (bool *)jl_array_data(ret1);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (ISNAN(REAL(Var)[i]))
        {
          retData[i] = 999.01;
          retData1[i] = true;
        }
        else
        {
          retData[i] = REAL(Var)[i];
          retData1[i] = false;
        }
      }
      JL_GC_POP();
      return TransArrayToDataArray(ret, ret1, VarName);
      break;
    }
    case STRSXP:
    {
      jl_array_t *ret;
      if (!IS_ASCII(Var))
        ret = CreateArray(jl_utf8_string_type, jl_tuple_len(dims), dims);
      else
        ret = CreateArray(jl_ascii_string_type, jl_tuple_len(dims), dims);

      jl_array_t *ret1 = CreateArray(jl_bool_type, jl_tuple_len(dims), dims);

      JL_GC_PUSH(&ret, &ret1);
      jl_value_t **retData = jl_array_data(ret);
      bool *retData1 = (bool *)jl_array_data(ret1);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (STRING_ELT(Var, i) == NA_STRING)
        {
          retData[i] = jl_cstr_to_string("999");
          retData1[i] = true;
        }
        else
        {
          if (!IS_ASCII(Var))
            retData[i] = jl_cstr_to_string(translateChar0(STRING_ELT(Var, i)));
          else
            retData[i] = jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
          retData1[i] = false;
        }
      }
      JL_GC_POP();
      return TransArrayToDataArray(ret, ret1, VarName);
      break;
    }
    default:
      return (jl_value_t *) jl_nothing;
      break;
    }//case end
    return (jl_value_t *) jl_nothing;
  }//if length !=0
  return (jl_value_t *) jl_nothing;
}

//basically factor in R is 1-dim INTSXP and contain levels
static jl_value_t *TransArrayToPoolDataArray(jl_array_t *mArray, jl_array_t *mpoolArray, size_t len, const char *VarName)
{
  char evalcmd[evalsize];
  jl_set_global(jl_main_module, jl_symbol("varpools"), (jl_value_t *)mpoolArray);
  jl_set_global(jl_main_module, jl_symbol("varrefs"), (jl_value_t *)mArray);
  snprintf(evalcmd, evalsize, "%s=PooledDataArray(ASCIIString,Uint32,%d)", VarName, len);
  jl_eval_string(evalcmd);
  snprintf(evalcmd, evalsize, "%s.pool=%s", VarName, "varpools");
  jl_eval_string(evalcmd);
  snprintf(evalcmd, evalsize, "%s.refs=%s", VarName, "varrefs");
  jl_eval_string(evalcmd);
  jl_value_t *ret = jl_eval_string((char *)VarName);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    return (jl_value_t *) jl_nothing;
  }
  return ret;
}

static jl_value_t *R_Julia_MD_NA_Factor(SEXP Var, const char *VarName)
{
  SEXP levels = getAttrib(Var, R_LevelsSymbol);
  if (levels == R_NilValue)
    return jl_nothing;

  //create string array for levels in julia
  jl_array_t *ret1 = jl_alloc_array_1d(jl_apply_array_type(jl_ascii_string_type, 1), LENGTH(levels));
  jl_value_t **retData1 = jl_array_data(ret1);

  for (size_t i = 0; i < jl_array_len(ret1); i++)
    if (!IS_ASCII(Var))
      retData1[i] = jl_cstr_to_string(translateChar0(STRING_ELT(levels, i)));
    else
      retData1[i] = jl_cstr_to_string(CHAR(STRING_ELT(levels, i)));

  if ((LENGTH(Var)) != 0)
  {
    switch (TYPEOF(Var))
    {
    case INTSXP:
    {
      jl_array_t *ret = jl_alloc_array_1d(jl_apply_array_type(jl_uint32_type, 1), LENGTH(Var));
      JL_GC_PUSH(&ret, &ret1);
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
      JL_GC_POP();
      return TransArrayToPoolDataArray(ret, ret1, LENGTH(Var), VarName);
      break;
    }
    default:
      return (jl_value_t *) jl_nothing;
      break;
    }//case end
    return (jl_value_t *) jl_nothing;
  }//if length !=0
  return (jl_value_t *) jl_nothing;
}

static jl_value_t *R_Julia_MD_NA_DataFrame(SEXP Var, const char *VarName)
{
  SEXP names = getAttrib(Var, R_NamesSymbol);
  size_t len = LENGTH(Var);
  if (TYPEOF(Var) != VECSXP || len == 0 || names == R_NilValue)
    return (jl_value_t *) jl_nothing;
  char evalcmd[evalsize];
  char eltcmd[eltsize];
  const char *onename;
  SEXP elt;
  for (size_t i = 0; i < len; i++)
  {
    snprintf(eltcmd, eltsize, "%sdfelt%d", VarName, i + 1);
    elt = VECTOR_ELT(Var, i);
    //vector is factor or not
    if (getAttrib(elt, R_LevelsSymbol) != R_NilValue)
      R_Julia_MD_NA_Factor(elt, eltcmd);
    else
      R_Julia_MD_NA(elt, eltcmd);

    onename = CHAR(STRING_ELT(names, i));
    if (i == 0)
      snprintf(evalcmd, evalsize, "%s=DataFrame(%s =%s)", VarName, onename, eltcmd);
    else
      snprintf(evalcmd, evalsize, "%s[symbol(\"%s\")]=%s", VarName, onename, eltcmd);
    //Rprintf("%s\n",evalcmd);
    jl_eval_string(evalcmd);
    if (jl_exception_occurred())
    {
      jl_show(jl_stderr_obj(), jl_exception_occurred());
      Rprintf("\n");
      jl_exception_clear();
      return (jl_value_t *) jl_nothing;
    }
  }
  return (jl_value_t *) jl_nothing;;
}

//Convert R Type To Julia,which not contain NA
SEXP R_Julia(SEXP Var, SEXP VarNam)
{
  const char *VarName = CHAR(STRING_ELT(VarNam, 0));
  R_Julia_MD(Var, VarName);
  return R_NilValue;
}

//Convert R Type To Julia,which contain NA
SEXP R_Julia_NA(SEXP Var, SEXP VarNam)
{
  LoadDF();
  const char *VarName = CHAR(STRING_ELT(VarNam, 0));
  R_Julia_MD_NA(Var, VarName);
  return R_NilValue;
}
//Convert R factor To Julia,which contain NA
SEXP R_Julia_NA_Factor(SEXP Var, SEXP VarNam)
{
  LoadDF();
  const char *VarName = CHAR(STRING_ELT(VarNam, 0));
  R_Julia_MD_NA_Factor(Var, VarName);
  return R_NilValue;
}
//Convert R data frame To Julia
SEXP R_Julia_NA_DataFrame(SEXP Var, SEXP VarNam)
{
  LoadDF();
  const char *VarName = CHAR(STRING_ELT(VarNam, 0));
  R_Julia_MD_NA_DataFrame(Var, VarName);
  return R_NilValue;
}
