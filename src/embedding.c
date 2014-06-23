/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include <Rdefines.h>
#include "Julia_R.h"
#include "R_Julia.h"

#define pkgdebug
static int jlrunning=0;

static int DataArrayFrameInited=0;
SEXP Julia_LoadDataArrayFrame()
{
  jl_eval_string("using DataArrays,DataFrames");
  DataArrayFrameInited=1;
  if (jl_exception_occurred()){
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
   DataArrayFrameInited=0; 
   jl_exception_clear();
  } 
  return R_NilValue; 
}

SEXP Julia_DataArrayFrameInited()
{
  SEXP ans;
  PROTECT(ans = allocVector(LGLSXP, 1));
  LOGICAL(ans)[0]=DataArrayFrameInited;
  UNPROTECT(1);
  return ans; 
}


SEXP Julia_is_running()
{
  SEXP ans;
  PROTECT(ans = allocVector(LGLSXP, 1));
  LOGICAL(ans)[0]=jlrunning;
  UNPROTECT(1);
  return ans; 
}

SEXP initJulia(SEXP julia_home,SEXP DisableGC)
{
  if (jl_is_initialized())
    return R_NilValue;
  const char *s = CHAR(STRING_ELT(julia_home, 0));
  if (strlen((char*)s) == 0)
    jl_init(NULL);
  else
    jl_init((char*)s);
  JL_SET_STACK_BASE;
  jlrunning=1;
  if (jl_exception_occurred())
  {
    error("Julia not initialized");
  }
  if (LOGICAL(DisableGC))
    jl_gc_disable();
  return R_NilValue;
}

//Convert Julia Type To R
SEXP Julia_R(jl_value_t* Var)
{  
  SEXP ans;
  JL_GC_PUSH1(&Var);
  ans=R_NilValue;
  //Array To Vector
  if (jl_is_null(Var)||jl_is_nothing(Var))
    return ans;
  if (jl_is_array(Var))
  {
   ans=Julia_R_MD(Var); 
  }
  else if (strcmp(jl_typeof_str(Var),"DataArray")==0||
           strcmp(jl_typeof_str(Var),"DataVector")==0||
           strcmp(jl_typeof_str(Var),"DataMatrix")==0||
           strcmp(jl_typeof_str(Var),"DataFrame")==0||
           strcmp(jl_typeof_str(Var),"NAtype")==0)
   {
    //try to load DataArrays DataFrames package
    if (!DataArrayFrameInited) Julia_LoadDataArrayFrame();
    if (!DataArrayFrameInited)
    {
     error("DataArrays and DataFrames can't be load,please check this\n");
     return R_NilValue;
    }
    //Rprintf("type of %s len %d",jl_typeof_str(Var),strlen(jl_typeof_str(Var)));
    if(strcmp(jl_typeof_str(Var),"NAtype")==0)
     ans=Julia_R_Scalar_NA(Var); 
    else 
     ans=Julia_R_MD_NA(Var);
   } 
  else if (jl_is_tuple(Var))
   {
     PROTECT(ans=allocVector(VECSXP,jl_tuple_len(Var)));
     for(int i=0;i<jl_tuple_len(Var);i++)
     {
      SET_ELEMENT(ans,i,Julia_R(jl_tupleref(Var,i)));
     }
     UNPROTECT(1);
   } 
  else 
  { 
     ans=Julia_R_Scalar(Var); 
  }
 JL_GC_POP();
 return ans;
}

//Convert R Type To Julia,which not contain NA
SEXP R_Julia(SEXP Var,SEXP VarNam)
{
  const char *VarName = CHAR(STRING_ELT(VarNam, 0));
  R_Julia_MD(Var,VarName);
  return R_NilValue;
}
//Convert R Type To Julia,which contain NA
SEXP R_Julia_NA(SEXP Var,SEXP VarNam)
{
  const char *VarName = CHAR(STRING_ELT(VarNam, 0));
  R_Julia_MD_NA(Var,VarName);
  return R_NilValue;
}
//eval but not return val
SEXP jl_void_eval(SEXP cmd)
{
  const char *s = CHAR(STRING_ELT(cmd, 0));
  jl_eval_string((char*)s);
  if (jl_exception_occurred())
   { 
   jl_show(jl_stderr_obj(), jl_exception_occurred());
   Rprintf("\n");
   jl_exception_clear();
   }
  return R_NilValue;
}

//eval julia script and retrun
SEXP jl_eval(SEXP cmd)
{
  const char *s = CHAR(STRING_ELT(cmd, 0));
  jl_value_t* ret= jl_eval_string((char*)s);
  if (jl_exception_occurred()){
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
   return R_NilValue;
  }
  return Julia_R(ret);
}
