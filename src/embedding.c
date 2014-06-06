#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#define pkgdebug

SEXP initJulia(SEXP julia_home,SEXP DisableGC)
{
  if (jl_is_initialized())
    return R_NilValue;

  char *s = CHAR(STRING_ELT(julia_home, 0));
  if (strlen(s) == 0)
    jl_init(NULL);
  else
    jl_init(s);

  JL_SET_STACK_BASE;
  if (jl_exception_occurred())
  {
    error("Julia not initialized");
  }
  if (LOGICAL(DisableGC))
    jl_gc_disable();
  return R_NilValue;
}

SEXP Julia_R(jl_value_t* Var)
{  
  SEXP ans;
  JL_GC_PUSH1(&Var);
  ans=R_NilValue;
  //Array To Vector
  if (jl_is_array(Var))
  {
   if (jl_array_ndims(Var)==1)
   {
     jl_value_t* val=jl_arrayref(Var,0);
     if (jl_is_bool(val))
     {
      char* p=(char*) jl_array_data(Var);
      int len=jl_array_dim(Var,0);
      PROTECT(ans = allocVector(LGLSXP, len));
      for (size_t i=0;i<len;i++)
        LOGICAL(ans)[i]=p[i]; 
      UNPROTECT(1);
    }
    if (jl_is_int32(val))
    {
      int* p=(int*) jl_array_data(Var);
      int len=jl_array_dim(Var,0);
      PROTECT(ans = allocVector(INTSXP, len));
      for (size_t i=0;i<len;i++)
        INTEGER(ans)[i]=p[i]; 
      UNPROTECT(1);
    }
    //int64
    if (jl_is_int64(val))
    {
      int* p=(int*) jl_array_data(Var);
      int len=jl_array_dim(Var,0);
      PROTECT(ans = allocVector(REALSXP, len));
      for (size_t i=0;i<len;i++)
        REAL(ans)[i]=p[i]; 
      UNPROTECT(1);
    }
    //double
    if (jl_is_float64(val))
    {
      double* p=(double*) jl_array_data(Var);
      int len=jl_array_dim(Var,0);
      PROTECT(ans = allocVector(REALSXP, len));
      for (size_t i=0;i<len;i++)
        REAL(ans)[i]=p[i]; 
      UNPROTECT(1);
    }   
    //convert string array to STRSXP ,but not sure it is corret?
    if (((jl_array_t*)Var)->ptrarray)
    {
      char** p=(char**) jl_array_data(Var);
      int len=jl_array_dim(Var,0);
      PROTECT(ans = allocVector(STRSXP, len));
      for (size_t i=0;i<len;i++)
        SET_STRING_ELT(ans,i,mkChar(p[i])); 
      UNPROTECT(1);
    }   
  }
}
//Value to Vector
if (jl_is_int32(Var))
{
  PROTECT(ans = allocVector(INTSXP, 1));
  REAL(ans)[0]=jl_unbox_int32(Var); 
  UNPROTECT(1);
}
if (jl_is_int64(Var))
{
  PROTECT(ans = allocVector(REALSXP, 1));
  REAL(ans)[0]=jl_unbox_int64(Var); 
  UNPROTECT(1);
}
if (jl_is_float64(Var))
{
  PROTECT(ans = allocVector(REALSXP, 1));
  REAL(ans)[0]=jl_unbox_int64(Var); 
  UNPROTECT(1);
}
if (jl_is_bool(Var))
{
  PROTECT(ans = allocVector(LGLSXP, 1));
  LOGICAL(ans)[0]=jl_unbox_bool(Var); 
  UNPROTECT(1);
}
if (jl_is_byte_string(Var))
{
  PROTECT(ans = allocVector(STRSXP, 1));
  SET_STRING_ELT(ans,0,mkChar(jl_string_data(Var)));
  UNPROTECT(1);
}
JL_GC_POP();

return ans;
}

SEXP R_Julia(SEXP Var,SEXP VarNam)
{
  int n;
  jl_value_t* ret;
  char *VarName = CHAR(STRING_ELT(VarNam, 0));
  if ((n = LENGTH(Var))!= 0) {
    switch (TYPEOF(Var)) {
      case LGLSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_bool_type,1); 
        jl_array_t* ret= jl_alloc_array_1d(array_type,n);
        JL_GC_PUSH1(&ret);
        char* retData = (char*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =LOGICAL(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        JL_GC_POP(); 
        break;
      };
      case INTSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_int32_type,1); 
        jl_array_t* ret= jl_alloc_array_1d(array_type,n);
        JL_GC_PUSH1(&ret);
        int* retData = (int*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =INTEGER(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        JL_GC_POP(); 
        break;
      }
      case REALSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_float64_type,1); 
        jl_array_t* ret= jl_alloc_array_1d(array_type,n);
        JL_GC_PUSH1(&ret);
        double* retData = (double*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =REAL(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        JL_GC_POP();
        break;
      }
      case STRSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_ascii_string_type,1); 
        jl_array_t* ret= jl_alloc_array_1d(array_type,n);
        JL_GC_PUSH1(&ret);
        char** retData = (char**)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
        {
         retData[i] =CHAR(STRING_ELT(Var, i));
           #ifdef pkgdebug
          // Rprintf("str is %d %s\n",i,retData[i]);
           #endif
       } 
       jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
       JL_GC_POP();
       break;
     }
     default:
     break; 
   }
 }
 return R_NilValue;
}

SEXP jl_void_eval(SEXP cmd)
{
  char *s = CHAR(STRING_ELT(cmd, 0));
  jl_value_t* ret = jl_eval_string(s);
  return R_NilValue;
}

SEXP jl_eval(SEXP cmd)
{
  char *s = CHAR(STRING_ELT(cmd, 0));
  jl_value_t* ret= jl_eval_string(s);
  return Julia_R(ret);
}

