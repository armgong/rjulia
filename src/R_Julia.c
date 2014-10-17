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
 jl_tuple_t *d=NULL;
 JL_GC_PUSH1(&d);
 SEXP dims = getAttrib(Var, R_DimSymbol);
  //array or matrix
  if (dims != R_NilValue)
  {
    int ndims = LENGTH(dims);
    d = jl_alloc_tuple(ndims);
    for (size_t i = 0; i < ndims; i++)
    {
      jl_tupleset(d, i, jl_box_long(INTEGER(dims)[i]));
    }
  }
  else     //vector
  {
    d = jl_alloc_tuple(1);
    jl_tupleset(d, 0, jl_box_long(LENGTH(Var)));
  }
  JL_GC_POP();
  return d;
}

static jl_value_t *R_Julia_MD(SEXP Var, const char *VarName)
{

   if ((LENGTH(Var))==0)
     return (jl_value_t *) jl_nothing;

   jl_array_t *ret =NULL;
   jl_tuple_t *dims=RDims_JuliaTuple(Var);
   JL_GC_PUSH2(&ret,&dims);
   switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      ret = CreateArray(jl_bool_type, jl_tuple_len(dims), dims);
      char *retData = (char *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
        retData[i] = LOGICAL(Var)[i];
      break;
    };
    case INTSXP:
    {
      ret = CreateArray(jl_int32_type, jl_tuple_len(dims), dims);
      int *retData = (int *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
        retData[i] = INTEGER(Var)[i];
      break;
    }
    case REALSXP:
    {
      ret = CreateArray(jl_float64_type, jl_tuple_len(dims), dims);
      double *retData = (double *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
        retData[i] = REAL(Var)[i];
      break;
    }
    case STRSXP:
    {
      if (!IS_ASCII(Var))
        ret = CreateArray(jl_utf8_string_type, jl_tuple_len(dims), dims);
      else
        ret = CreateArray(jl_ascii_string_type, jl_tuple_len(dims), dims);
      jl_value_t **retData = jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
        if (!IS_ASCII(Var))
          retData[i] = jl_cstr_to_string(translateChar0(STRING_ELT(Var, i)));
        else
          retData[i] = jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
      break;
    }
    case VECSXP:
    {
      ret =(jl_value_t *) jl_alloc_tuple(length(Var));
      for (int i = 0; i < length(Var); i++)
      {
        jl_tupleset((jl_tuple_t *)ret, i, R_Julia_MD(VECTOR_ELT(Var, i), ""));
      }
      break;
    }
    default:
    {
      ret=(jl_value_t *)jl_nothing;
      break;
    }
   }
  JL_GC_POP();
  if (VarName!=NULL && strlen(VarName)>0)
   jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
  return (jl_value_t *)ret;
}

static jl_value_t *R_Julia_MD_NA(SEXP Var, const char *VarName)
{
  if ((LENGTH(Var)) == 0)
  {
    return (jl_value_t *) jl_nothing;
  }//if length !=0

 jl_tuple_t *dims = RDims_JuliaTuple(Var);
 jl_array_t *ret =NULL;
 jl_array_t *ret1 =NULL;
 jl_value_t *ans=NULL;
 JL_GC_PUSH4(&ret, &ret1,&dims,&ans);
 jl_function_t *DataArray=jl_get_function(jl_main_module,"DataArray");
 switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      ans=jl_call2(DataArray,jl_bool_type,dims);
      ret =jl_get_field(ans,"data");
      ret1 =jl_get_field(ans,"na");

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
      break;
    };
    case INTSXP:
    {
      ans=jl_call2(DataArray,jl_int32_type,dims);
      ret =jl_get_field(ans,"data");
      ret1 =jl_get_field(ans,"na");

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
      break;
    }
    case REALSXP:
    {
      ans=jl_call2(DataArray,jl_float64_type,dims);
      ret =jl_get_field(ans,"data");
      ret1 =jl_get_field(ans,"na");

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
      break;
    }
    case STRSXP:
    {
      if (!IS_ASCII(Var))
        ans=jl_call2(DataArray,jl_utf8_string_type,dims);
      else
        ans=jl_call2(DataArray,jl_ascii_string_type,dims);

      ret =jl_get_field(ans,"data");
      ret1 =jl_get_field(ans,"na");

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
      break;
    }
    default:
      ans=(jl_value_t *) jl_nothing;
      break;
    }//case end
   if (VarName!=NULL && strlen(VarName)>0)
     jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
    JL_GC_POP();
    return ans;
 }

static jl_value_t *R_Julia_MD_NA_Factor(SEXP Var, const char *VarName)
{
  if ((LENGTH(Var))== 0)
   return jl_nothing;
  SEXP levels = getAttrib(Var, R_LevelsSymbol);
  if (levels == R_NilValue)
    return jl_nothing;

  //create string array for levels in julia
  jl_value_t *ans=NULL;
  jl_array_t *ret=NULL;
  jl_array_t *ret1 =NULL;
  jl_value_t **retData1=NULL;
  JL_GC_PUSH3(&ret, &ret1,&ans);


  switch (TYPEOF(Var))
   {
    case INTSXP:
    {
      jl_function_t *PooledDataArray=jl_get_function(jl_main_module,"PooledDataArray");
      ans=jl_call3(PooledDataArray,jl_ascii_string_type,jl_uint32_type,jl_box_uint32(LENGTH(Var)));
      ret =jl_get_field(ans,"refs");
      int *retData = (int *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (INTEGER(Var)[i] == NA_INTEGER)
        {
          retData[i] = 0;
        }
        else
        {
          retData[i] = INTEGER(Var)[i];
        }
      }
      ret1 = jl_alloc_array_1d(jl_apply_array_type(jl_ascii_string_type, 1), LENGTH(levels));
      retData1 = jl_array_data(ret1);
      for (size_t i = 0; i < jl_array_len(ret1); i++)
      {
       if (!IS_ASCII(Var))
         retData1[i] = jl_cstr_to_string(translateChar0(STRING_ELT(levels, i)));
       else
         retData1[i] = jl_cstr_to_string(CHAR(STRING_ELT(levels, i)));
      }
      jl_function_t *setlevels=jl_get_function(jl_main_module,"setlevels");
      jl_call2(setlevels,ans,ret1);
      break;
    }
    default:
     ans=(jl_value_t *) jl_nothing;
      break;
    }//case end
  if (VarName!=NULL && strlen(VarName)>0)
   jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
  JL_GC_POP();
  return ans;
}

static void Julia_MD_NA_Factor(jl_value_t *retelt,SEXP Var)
{
  SEXP levels = getAttrib(Var, R_LevelsSymbol);
  jl_array_t *ret=NULL;
  jl_array_t *ret1=NULL;
  jl_array_t **retData1 = NULL;
  JL_GC_PUSH2(&ret, &ret1);
  switch (TYPEOF(Var))
   {
    case INTSXP:
    {
      ret =jl_get_field(retelt,"refs");
      int *retData = (int *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (INTEGER(Var)[i] == NA_INTEGER)
        {
          retData[i] = 0;
        }
        else
        {
          retData[i] = INTEGER(Var)[i];
        }
      }
      jl_array_grow_end(jl_get_field(retelt,"pool"),LENGTH(levels));
      ret1=jl_get_field(retelt,"pool");
      retData1 = jl_array_data(ret1);
      for (size_t i = 0; i < jl_array_len(ret1); i++)
      {
       if (!IS_ASCII(Var))
        retData1[i] = jl_cstr_to_string(translateChar0(STRING_ELT(levels, i)));
       else
        retData1[i] = jl_cstr_to_string(CHAR(STRING_ELT(levels, i)));
      }
      break;
    }
    default:
      break;
    }//case end
  JL_GC_POP();
}

static void Julia_MD_NA(jl_value_t *retelt,SEXP Var)
{
 if ((LENGTH(Var)) == 0)
    return ;

 jl_tuple_t *dims = RDims_JuliaTuple(Var);
 jl_array_t *ret =NULL;
 jl_array_t *ret1 =NULL;
 JL_GC_PUSH3(&ret, &ret1,&dims);
 ret =jl_get_field(retelt,"data");
 ret1 =jl_get_field(retelt,"na");
 switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
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
      break;
    };
    case INTSXP:
    {
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
      break;
    }
    case REALSXP:
    {
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
      break;
    }
    case STRSXP:
    {
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
      break;
    }
    default:
      break;
    }//case end
    JL_GC_POP();
}


static jl_value_t *R_Julia_MD_NA_DataFrame(SEXP Var, const char *VarName)
{
  SEXP names = getAttrib(Var, R_NamesSymbol);
  size_t len = LENGTH(Var);
  if (TYPEOF(Var) != VECSXP || len == 0 || names == R_NilValue)
    return (jl_value_t *) jl_nothing;
  //get rows
  size_t rows=LENGTH(VECTOR_ELT(Var,0));
  //read column types and name
  SEXP elt;
  const char *onename;
  jl_array_t *ret1=NULL;
  jl_array_t *retName=NULL;
  jl_value_t *ans=NULL;
  ret1=jl_alloc_array_1d(jl_apply_array_type(jl_type_type, 1), len);
  retName=jl_alloc_array_1d(jl_apply_array_type(jl_symbol_type, 1), len);
  for (size_t i = 0; i < len; i++)
  {
   onename = CHAR(STRING_ELT(names, i));
   jl_arrayset(retName,jl_symbol(onename), i);
   elt = VECTOR_ELT(Var, i);
   switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      jl_arrayset(ret1,jl_bool_type, i);
      break;
    };
    case INTSXP:
    {
      jl_arrayset(ret1,jl_int32_type, i);
      break;
    }
    case REALSXP:
    {
      jl_arrayset(ret1,jl_float64_type, i);
      break;
    }
    case STRSXP:
    {
      if (!IS_ASCII(Var))
       jl_arrayset(ret1,jl_ascii_string_type, i);
      else
       jl_arrayset(ret1,jl_utf8_string_type, i);
      break;
    }
    default:
      jl_arrayset(ret1,jl_any_type, i);
      break;
    }//case end
  }
  //Create data Frame
   jl_function_t *DataFrame=jl_get_function(jl_main_module,"DataFrame");
   ans=jl_call3(DataFrame,ret1,retName,jl_box_uint32(rows));
   jl_function_t *getindex=jl_get_function(jl_main_module,"getindex");
   jl_value_t  *allret=jl_get_field(Var,"columns");
   jl_value_t *retelt=NULL;
  //assign value
  for (size_t i = 0; i < len; i++)
  {
    elt = VECTOR_ELT(Var, i);
    retelt=jl_call1(getindex,jl_box_uint32(i));
    //vector is factor or not
    if (getAttrib(elt, R_LevelsSymbol) != R_NilValue)
      Julia_MD_NA_Factor(retelt,elt);
    else
      Julia_MD_NA(retelt,elt);
  if (VarName!=NULL && strlen(VarName)>0)
   jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ans);

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
