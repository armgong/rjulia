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
static jl_value_t *CreateArray(jl_datatype_t *type, size_t ndim, jl_tuple_t *dims)
{
  return (jl_value_t*) jl_new_array(jl_apply_array_type(type, ndim), dims);;
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
      jl_tupleset(d, i, jl_box_long(INTEGER(dims)[i]));
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

   jl_value_t *ret =NULL;
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
    }
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
    return (jl_value_t *) jl_nothing;

 jl_tuple_t *dims = RDims_JuliaTuple(Var);
 jl_value_t *ret =NULL;
 jl_value_t *ret1 =NULL;
 jl_value_t *ans=NULL;
 JL_GC_PUSH4(&ret, &ret1,&dims,&ans);
 jl_function_t *DataArray=jl_get_function(jl_main_module,"DataArray");
 jl_function_t *setindex=jl_get_function(jl_main_module,"setindex!");
 switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      ans=jl_call2(DataArray, (jl_value_t*) jl_bool_type, (jl_value_t*) dims);
      ret =jl_get_field(ans,"data");
      ret1 =jl_get_field(ans,"na");

      char *retData = (char *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (LOGICAL(Var)[i] == NA_LOGICAL)
        {
          retData[i] = 1;
          jl_call3(setindex,ret1,jl_box_bool(1),jl_box_long(i+1));
        }
        else
        {
          retData[i] = LOGICAL(Var)[i];
          jl_call3(setindex,ret1,jl_box_bool(0),jl_box_long(i+1));
        }
      }
      break;
    }
    case INTSXP:
    {
      ans=jl_call2(DataArray,(jl_value_t*) jl_int32_type,(jl_value_t*) dims);
      ret =jl_get_field(ans,"data");
      ret1 =jl_get_field(ans,"na");

      int *retData = (int *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (INTEGER(Var)[i] == NA_INTEGER)
        {
          retData[i] = 999;
          jl_call3(setindex,ret1,jl_box_bool(1),jl_box_long(i+1));
        }
        else
        {
          retData[i] = INTEGER(Var)[i];
          jl_call3(setindex,ret1,jl_box_bool(0),jl_box_long(i+1));
        }
      }
      break;
    }
    case REALSXP:
    {
      ans=jl_call2(DataArray,(jl_value_t*) jl_float64_type,(jl_value_t*) dims);
      ret =jl_get_field(ans,"data");
      ret1 =jl_get_field(ans,"na");

      double *retData = (double *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (ISNAN(REAL(Var)[i]))
        {
          retData[i] = 999.01;
          jl_call3(setindex,ret1,jl_box_bool(1),jl_box_long(i+1));
        }
        else
        {
          retData[i] = REAL(Var)[i];
          jl_call3(setindex,ret1,jl_box_bool(0),jl_box_long(i+1));
        }
      }
      break;
    }
    case STRSXP:
    {
      if (!IS_ASCII(Var))
        ans=jl_call2(DataArray,(jl_value_t*) jl_utf8_string_type,(jl_value_t*) dims);
      else
        ans=jl_call2(DataArray,(jl_value_t*) jl_ascii_string_type,(jl_value_t*) dims);

      ret =jl_get_field(ans,"data");
      ret1 =jl_get_field(ans,"na");
      jl_value_t **retData = jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (STRING_ELT(Var, i) == NA_STRING)
        {
          retData[i] = jl_cstr_to_string("999");
          jl_call3(setindex,ret1,jl_box_bool(1),jl_box_long(i+1));
        }
        else
        {
          if (!IS_ASCII(Var))
            retData[i] = jl_cstr_to_string(translateChar0(STRING_ELT(Var, i)));
          else
            retData[i] = jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
          jl_call3(setindex,ret1,jl_box_bool(0),jl_box_long(i+1));
        }
      }
      break;
    }
    default:
      ans=(jl_value_t *) jl_nothing;
      break;
    }//case end
   if (VarName!=NULL && strlen(VarName)>0)
     jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ans);
    JL_GC_POP();
    return ans;
 }

static jl_value_t *R_Julia_MD_NA_Factor(SEXP Var, const char *VarName)
{
  SEXP levels = getAttrib(Var, R_LevelsSymbol);
  if (LENGTH(Var)== 0||levels == R_NilValue||TYPEOF(Var)!=INTSXP)
    return jl_nothing;
  
  //create string array for levels in julia
  jl_value_t *ans=NULL;
  jl_value_t *ret=NULL;
  jl_value_t *ret1 =NULL;
  jl_value_t **retData1=NULL;
  JL_GC_PUSH3(&ret, &ret1,&ans);
  jl_function_t *PooledDataArray=jl_get_function(jl_main_module,"PooledDataArray");
  ans=jl_call3(PooledDataArray,(jl_value_t*) jl_ascii_string_type,(jl_value_t*) jl_uint32_type,jl_box_long(LENGTH(Var)));
  ret =jl_get_field(ans,"refs");
  //int 
  int *retData = (int *)jl_array_data(ret);
  for (size_t i = 0; i < jl_array_len(ret); i++)
  {
    if (INTEGER(Var)[i] == NA_INTEGER)
      retData[i] = 0;
    else
      retData[i] = INTEGER(Var)[i];
  }

  //levels
  ret1 = (jl_value_t*) jl_alloc_array_1d(jl_apply_array_type(jl_ascii_string_type, 1), LENGTH(levels));
  retData1 = jl_array_data(ret1);
  for (size_t i = 0; i < jl_array_len(ret1); i++)
  {
   if (!IS_ASCII(Var))
     retData1[i] = jl_cstr_to_string(translateChar0(STRING_ELT(levels, i)));
   else
     retData1[i] = jl_cstr_to_string(CHAR(STRING_ELT(levels, i)));
  }
  //setlevels
  jl_function_t *setlevels=jl_get_function(jl_main_module,"setlevels!");
  jl_call2(setlevels,ans,ret1);
  if (VarName!=NULL && strlen(VarName)>0)
   jl_set_global(jl_main_module, jl_symbol(VarName), ans);
  JL_GC_POP();
  return ans;
}

static void Julia_1D_NA_Factor(jl_value_t *retelt,SEXP Var)
{
  SEXP levels = getAttrib(Var, R_LevelsSymbol);
  if (LENGTH(Var)== 0||levels == R_NilValue||TYPEOF(Var)!=INTSXP)
    return ;

  jl_value_t *ret=NULL;
  jl_value_t *ret1=NULL;
  jl_value_t **retData1 = NULL;

  JL_GC_PUSH2(&ret, &ret1);
  ret=jl_get_field(retelt,"refs");
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
  jl_array_grow_end((jl_array_t *)jl_get_field(retelt,"pool"),LENGTH(levels));
  ret1=jl_get_field(retelt,"pool");
  retData1 = jl_array_data(ret1);
  for (size_t i = 0; i < jl_array_len(ret1); i++)
  {
   if (!IS_ASCII(Var))
    retData1[i] = jl_cstr_to_string(translateChar0(STRING_ELT(levels, i)));
   else
    retData1[i] = jl_cstr_to_string(CHAR(STRING_ELT(levels, i)));
  }
  JL_GC_POP();
}

static void Julia_1D_NA(jl_value_t *retelt,SEXP Var)
{
 if ((LENGTH(Var)) == 0)
    return ;

 jl_value_t *ret =NULL;
 jl_value_t *ret1 =NULL;
 JL_GC_PUSH2(&ret, &ret1);
 
 ret =jl_get_field(retelt,"data");
 ret1 =jl_get_field(retelt,"na");
 jl_function_t *setindex=jl_get_function(jl_main_module,"setindex!");
 
 switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      char *retData = (char *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (LOGICAL(Var)[i] == NA_LOGICAL)
        {
          retData[i] = 1;
          jl_call3(setindex,ret1,jl_box_bool(1),jl_box_long(i+1));
        }
        else
        {
          retData[i] = LOGICAL(Var)[i];
          jl_call3(setindex,ret1,jl_box_bool(0),jl_box_long(i+1));
        }
      }
      break;
    };
    case INTSXP:
    {
      int *retData = (int *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (INTEGER(Var)[i] == NA_INTEGER)
        {
          retData[i] = 999;
          jl_call3(setindex,ret1,jl_box_bool(1),jl_box_long(i+1));
        }
        else
        {
          retData[i] = INTEGER(Var)[i];
          jl_call3(setindex,ret1,jl_box_bool(0),jl_box_long(i+1));
        }
      }
      break;
    }
    case REALSXP:
    {
      double *retData = (double *)jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (ISNAN(REAL(Var)[i]))
        {
          retData[i] = 999.01;
          jl_call3(setindex,ret1,jl_box_bool(1),jl_box_long(i+1));
        }
        else
        {
          retData[i] = REAL(Var)[i];
          jl_call3(setindex,ret1,jl_box_bool(0),jl_box_long(i+1));
        }
      }
      break;
    }
    case STRSXP:
    {
      jl_value_t **retData = jl_array_data(ret);
      for (size_t i = 0; i < jl_array_len(ret); i++)
      {
        if (STRING_ELT(Var, i) == NA_STRING)
        {
          retData[i] = jl_cstr_to_string("999");
          jl_call3(setindex,ret1,jl_box_bool(1),jl_box_long(i+1));
        }
        else
        {
          if (!IS_ASCII(Var))
            retData[i] = jl_cstr_to_string(translateChar0(STRING_ELT(Var, i)));
          else
            retData[i] = jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
          jl_call3(setindex,ret1,jl_box_bool(0),jl_box_long(i+1));
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
  jl_array_t *retfactor=NULL;
  jl_value_t *ans=NULL;
  JL_GC_PUSH4(&ret1, &retName,&retfactor,&ans);

  ret1=jl_alloc_array_1d(jl_apply_array_type(jl_datatype_type, 1), len);
  retName=jl_alloc_array_1d(jl_apply_array_type(jl_symbol_type, 1), len);
  retfactor=jl_alloc_array_1d(jl_apply_array_type(jl_bool_type, 1), len);
  for (size_t i = 0; i < len; i++)
  {
   onename = CHAR(STRING_ELT(names, i));
   jl_arrayset(retName,(jl_value_t *)jl_symbol(onename), i);
   elt = VECTOR_ELT(Var, i);
   jl_arrayset(retfactor,jl_box_bool(0), i);
   switch (TYPEOF(elt))
   {
    case LGLSXP:
    {
      jl_arrayset(ret1,(jl_value_t *)jl_bool_type, i);
      break;
    };
    case INTSXP:
    {
      //if factor then need pooled array
      if ( getAttrib(elt, R_LevelsSymbol)!=R_NilValue)
       {
        jl_arrayset(retfactor,jl_box_bool(1), i);
        if (!IS_ASCII(elt))
         jl_arrayset(ret1,(jl_value_t *)jl_ascii_string_type, i);
        else
         jl_arrayset(ret1,(jl_value_t *)jl_utf8_string_type, i);
       }
       else  
         jl_arrayset(ret1,(jl_value_t *)jl_int32_type, i);
       break;
    }
    case REALSXP:
    {
      jl_arrayset(ret1,(jl_value_t *)jl_float64_type, i);
      break;
    }
    case STRSXP:
    {
      if (!IS_ASCII(elt))
       jl_arrayset(ret1,(jl_value_t *)jl_ascii_string_type, i);
      else
       jl_arrayset(ret1,(jl_value_t *)jl_utf8_string_type, i);
      break;
    }
    default:
      jl_arrayset(ret1,(jl_value_t *)jl_any_type, i);
      break;
    }//case end
  }
  //Create data Frame
   jl_function_t *DataFrame=jl_get_function(jl_main_module,"DataFrame");
   jl_value_t **argv;
   JL_GC_PUSHARGS(argv,4);
   argv[0]=(jl_value_t *)ret1;
   argv[1]=(jl_value_t *)retName;
   argv[2]=(jl_value_t *)retfactor;
   argv[3]=jl_box_long(rows);
   ans=jl_call(DataFrame,argv,4);
   JL_GC_POP();

   jl_function_t *getindex=jl_get_function(jl_main_module,"getindex");
  //assign value
  for (size_t i = 0; i < len; i++)
  {
    elt = VECTOR_ELT(Var, i);
    jl_value_t *retelt=jl_call2(getindex,ans,jl_box_long(i+1));
    JL_GC_PUSH1(&retelt);
    //vector is factor or not
    if (getAttrib(elt, R_LevelsSymbol) != R_NilValue)
      Julia_1D_NA_Factor(retelt,elt);
    else
      Julia_1D_NA(retelt,elt);
    JL_GC_POP();
   } 
  if (VarName!=NULL && strlen(VarName)>0)
   jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ans);
  JL_GC_POP();
  if (jl_exception_occurred())
    {
      jl_show(jl_stderr_obj(), jl_exception_occurred());
      Rprintf("\n");
      jl_exception_clear();
    }
  return (jl_value_t *) jl_nothing;;
}

//Convert R Type To Julia,which not contain NA
SEXP R_Julia(SEXP Var, SEXP VarNam)
{
  JL_TRY 
  {
    const char *VarName = CHAR(STRING_ELT(VarNam, 0));
    R_Julia_MD(Var, VarName);
    jl_exception_clear();
  }
  JL_CATCH 
  {
  }
  return R_NilValue;
}

//Convert R Type To Julia,which contain NA
SEXP R_Julia_NA(SEXP Var, SEXP VarNam)
{
  JL_TRY 
  {
   LoadDF();
   const char *VarName = CHAR(STRING_ELT(VarNam, 0));
   R_Julia_MD_NA(Var, VarName);
   jl_exception_clear();
  }
  JL_CATCH 
  {
  }  
  return R_NilValue;
}
//Convert R factor To Julia,which contain NA
SEXP R_Julia_NA_Factor(SEXP Var, SEXP VarNam)
{
  JL_TRY 
  {
   LoadDF();
   const char *VarName = CHAR(STRING_ELT(VarNam, 0));
   R_Julia_MD_NA_Factor(Var, VarName);
   jl_exception_clear();
  }
  JL_CATCH 
  {
  }    
  return R_NilValue;
}
//Convert R data frame To Julia
SEXP R_Julia_NA_DataFrame(SEXP Var, SEXP VarNam)
{
  JL_TRY 
  {
   LoadDF();
   const char *VarName = CHAR(STRING_ELT(VarNam, 0));
   R_Julia_MD_NA_DataFrame(Var, VarName);
   jl_exception_clear();
  }
  JL_CATCH 
  {
  }  
  return R_NilValue;
}
