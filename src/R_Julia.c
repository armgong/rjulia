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

//some defs from R
#define UTF8_MASK (1<<3)
#define ASCII_MASK (1<<6)
#define IS_ASCII(x) ((x)->sxpinfo.gp & ASCII_MASK)
#define IS_UTF8(x) ((x)->sxpinfo.gp & UTF8_MASK)

//SEXP whether is ASCII encode
static bool ISASCII(SEXP Var)
{
 bool result=true;
 for (size_t i=0;i<LENGTH(Var);i++ )
  {
   if (STRING_ELT(Var, i) == NA_STRING)
     continue;
   if(!IS_ASCII(STRING_ELT(Var, i)))
   {
     result=false;
     break;
   }
  }
 return result;
}

//create an Array in julia
static jl_array_t* CreateArray(jl_datatype_t *type, size_t ndim, jl_value_t *dims)
{
  return jl_new_array(jl_apply_array_type(type, ndim), dims);;
}

//convert R SEXP dims to julia tuple
static jl_value_t *RDims_JuliaTuple(SEXP Var)
{
  /*
 jl_value_t *d = NULL;
 JL_GC_PUSH1(&d);
 SEXP dims = getAttrib(Var, R_DimSymbol);

  //array or matrix
  if (dims != R_NilValue)
  {
    int ndims = LENGTH(dims);
    d = jl_alloc_svec(ndims);
    for (size_t i = 0; i < ndims; i++)
    {
      jl_svecset(d, i, jl_box_long(INTEGER(dims)[i]));
    }
  }
  else     //vector
  {
    d = jl_alloc_svec(1);
    jl_svecset(d, 0, jl_box_long(LENGTH(Var)));
  }
  jl_value_t* newd = (jl_value_t*)jl_apply_tuple_type(d);
  JL_GC_POP();
  return newd;
  */

  SEXP dims = getAttrib(Var, R_DimSymbol);
  char evalcmd[evalsize];
  char eltcmd[eltsize];
  snprintf(evalcmd, evalsize, "%s","(");

  if (dims != R_NilValue)
  {
    int ndims = LENGTH(dims);
    for (size_t i = 0; i < ndims; i++)
    {
     snprintf(eltcmd,eltsize,"%d,",INTEGER(dims)[i]);
		 strcat(evalcmd,eltcmd);
    }
   strcat(evalcmd,")");
  }
  else     //vector
  {
    snprintf(evalcmd,evalsize,"(%d,)",LENGTH(Var));
  }

  return jl_eval_string(evalcmd);

}

//convert R object to julia object
//Var is R object
//VarName in converted Julia object's name
static jl_value_t *R_Julia_MD(SEXP Var, const char *VarName)
{

   if ((LENGTH(Var))==0)
     return (jl_value_t *) jl_nothing;

   jl_array_t *ret =NULL;
   jl_value_t *dims=RDims_JuliaTuple(Var);
   JL_GC_PUSH2(&ret,&dims);
   switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      ret = CreateArray(jl_bool_type, jl_nfields(dims), dims);
      char *retData = (char *)jl_array_data(ret);
      int *var_p = LOGICAL(Var);
      for (size_t i = 0; i < jl_array_len(ret); i++) // Can not be memcpy because we want the implicit cast from int32 to int8
        retData[i] = var_p[i];
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      break;
    };
    case INTSXP:
    {
      ret = CreateArray(jl_int32_type, jl_nfields(dims), dims);
      int *retData = (int *)jl_array_data(ret);
      memcpy(retData, REAL(Var), jl_array_len(ret) * sizeof(retData));
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      break;
    }
    case REALSXP:
    {
      ret = CreateArray(jl_float64_type, jl_nfields(dims), dims);
      double *retData = (double *)jl_array_data(ret);
      memcpy(retData, REAL(Var), jl_array_len(ret) * sizeof(retData));
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      break;
    }
    case STRSXP:
    {
      if (!ISASCII(Var))
        ret = CreateArray(jl_utf8_string_type, jl_nfields(dims), dims);
      else
        ret = CreateArray(jl_ascii_string_type, jl_nfields(dims), dims);
      jl_value_t **retData = jl_array_data(ret);
      if (!ISASCII(Var)) {
	for (size_t i = 0; i < jl_array_len(ret); i++)
          retData[i] = jl_cstr_to_string(translateCharUTF8(STRING_ELT(Var, i)));
      } else {
	for (size_t i = 0; i < jl_array_len(ret); i++)
          retData[i] = jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
      }
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      break;
    }
    /*case VECSXP:
    {
      char eltcmd[eltsize];
      ret =(jl_value_t *) jl_alloc_svec(length(Var));
      for (int i = 0; i < length(Var); i++)
      {
        snprintf(eltcmd, eltsize, "%selement%d", VarName, i);
        jl_svecset((jl_value_t*)ret, i, R_Julia_MD(VECTOR_ELT(Var, i), eltcmd));
        //clear
        snprintf(eltcmd, eltsize, "%selement%d=0;", VarName, i);
        jl_eval_string(eltcmd);
      }
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      break;
    }*/
    case VECSXP:
    {
      char eltcmd[eltsize];
      char evalcmd[evalsize];
	  
      //get VECSXP elements in julia object array
      jl_value_t **elts;
      JL_GC_PUSHARGS(elts,length(Var));
      for (int i = 0; i < length(Var); i++)
      {
        snprintf(eltcmd, eltsize, "%selt%d", VarName, i);
        elts[i]=R_Julia_MD(VECTOR_ELT(Var,i),eltcmd);
      }
	  
      //create tuple use julia scripts, no API can create Tuple now
      snprintf(evalcmd, evalsize, "%s","(");
      for (size_t i = 0; i <length(Var); i++)
      {
       snprintf(eltcmd, eltsize, "%selt%d,", VarName, i);
       strcat(evalcmd,eltcmd);
      }
      strcat(evalcmd,")");
      ret=jl_eval_string(evalcmd);
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t *)ret);
      
      //clear tmp variable value
      for (size_t i = 0; i <length(Var); i++)
      {
       snprintf(eltcmd, eltsize, "%selt%d=0", VarName, i);
       jl_eval_string(eltcmd);
      }
	  
      JL_GC_POP();
      break;
    }

    default:
    {
      ret=(jl_value_t *)jl_nothing;
      break;
    }
   }
  JL_GC_POP();
  return (jl_value_t *)ret;
}

//convert julia array to DataArray in DataArray's package
//this is for R object cantain NA, need two pass to finish
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
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    return (jl_value_t *) jl_nothing;
  }
  return ret;
}

//convert R object cantain NA value to Julia DataArrays
static jl_value_t *R_Julia_MD_NA(SEXP Var, const char *VarName)
{
  if ((LENGTH(Var)) == 0)
  {
    return (jl_value_t *) jl_nothing;
  }//if length !=0

 jl_value_t*dims = RDims_JuliaTuple(Var);
 jl_array_t *ret =NULL;
 jl_array_t *ret1 =NULL;
 jl_value_t *ans=NULL;
 JL_GC_PUSH4(&ret, &ret1,&dims,&ans);

 switch (TYPEOF(Var))
   {
    case LGLSXP:
    {
      ret = CreateArray(jl_bool_type, jl_nfields(dims), dims);
      ret1 = CreateArray(jl_bool_type, jl_nfields(dims), dims);

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
      ans=TransArrayToDataArray(ret, ret1, VarName);
      break;
    };
    case INTSXP:
    {
      ret = CreateArray(jl_int32_type, jl_nfields(dims), dims);
      ret1 = CreateArray(jl_bool_type, jl_nfields(dims), dims);

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
      ans= TransArrayToDataArray(ret, ret1, VarName);
      break;
    }
    case REALSXP:
    {
      ret = CreateArray(jl_float64_type, jl_nfields(dims), dims);
      ret1 = CreateArray(jl_bool_type, jl_nfields(dims), dims);
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
      ans= TransArrayToDataArray(ret, ret1, VarName);
      break;
    }
    case STRSXP:
    {
      if (!ISASCII(Var))
        ret = CreateArray(jl_utf8_string_type, jl_nfields(dims), dims);
      else
        ret = CreateArray(jl_ascii_string_type, jl_nfields(dims), dims);

      ret1 = CreateArray(jl_bool_type, jl_nfields(dims), dims);

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
          if (!ISASCII(Var))
            retData[i] = jl_cstr_to_string(translateCharUTF8(STRING_ELT(Var, i)));
          else
            retData[i] = jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
          retData1[i] = false;
        }
      }
      ans= TransArrayToDataArray(ret, ret1, VarName);
      break;
    }
    default:
      ans=(jl_value_t *) jl_nothing;
      break;
    }//case end
    JL_GC_POP();
    return ans;
 }

//convert julia Array to PooledDataArray in DataArrays packages
//this is for convet R factors to Julia PooledDataArray
//basically factor in R is 1-dim INTSXP and contain levels
static jl_value_t *TransArrayToPoolDataArray(bool ascii,jl_array_t *mArray, jl_array_t *mpoolArray, size_t len, const char *VarName)
{
  char evalcmd[evalsize];
  jl_set_global(jl_main_module, jl_symbol("varpools"), (jl_value_t *)mpoolArray);
  jl_set_global(jl_main_module, jl_symbol("varrefs"), (jl_value_t *)mArray);
  if (ascii)
   snprintf(evalcmd, evalsize, "%s=PooledDataArray(ASCIIString,Uint32,%d)", VarName, len);
  else
   snprintf(evalcmd, evalsize, "%s=PooledDataArray(UTF8String,Uint32,%d)" , VarName, len);
  jl_value_t *ret1=NULL;
  jl_value_t *ret2=NULL;
  jl_value_t *ret3=NULL;
  JL_GC_PUSH3(&ret1,&ret2,&ret3);
  ret1=jl_eval_string(evalcmd);
  snprintf(evalcmd, evalsize, "%s.pool=%s", VarName, "varpools");
  ret2=jl_eval_string(evalcmd);
  snprintf(evalcmd, evalsize, "%s.refs=%s", VarName, "varrefs");
  ret3=jl_eval_string(evalcmd);
  jl_value_t *ret = jl_eval_string((char *)VarName);
  JL_GC_POP();
  jl_eval_string("varpools=0;varrefs=0;");
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    return (jl_value_t *) jl_nothing;
  }
  return ret;
}

//convert R factor to Julia PooledDataArray
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
  jl_array_t *ret1=NULL;
  bool ascii=ISASCII(levels);
  if (ascii)
   ret1 = jl_alloc_array_1d(jl_apply_array_type(jl_ascii_string_type,1), LENGTH(levels));
  else
   ret1 = jl_alloc_array_1d(jl_apply_array_type(jl_utf8_string_type,1),LENGTH(levels));

  jl_value_t **retData1 = jl_array_data(ret1);
  JL_GC_PUSH3(&ret, &ret1,&ans);

  for (size_t i = 0; i < jl_array_len(ret1); i++)
   {
    if (!ascii)
     retData1[i] = jl_cstr_to_string(translateCharUTF8(STRING_ELT(levels, i)));
    else
     retData1[i] = jl_cstr_to_string(CHAR(STRING_ELT(levels, i)));
   }

  switch (TYPEOF(Var))
   {
    case INTSXP:
    {
      ret = jl_alloc_array_1d(jl_apply_array_type(jl_uint32_type, 1), LENGTH(Var));
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
      ans=TransArrayToPoolDataArray(ascii,ret, ret1, LENGTH(Var), VarName);
      break;
    }
    default:
     ans=(jl_value_t *) jl_nothing;
      break;
    }//case end
  JL_GC_POP();
  return ans;
}

//convert R DataFrame to Julia DataFrame in DataFrames package
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
    jl_eval_string(evalcmd);

    //clear
    snprintf(eltcmd, eltsize, "%sdfelt%d=0;", VarName, i + 1);
    jl_eval_string(eltcmd);

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
