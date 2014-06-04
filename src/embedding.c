#include <julia.h>
#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#define pkgdebug
SEXP initJulia(SEXP julia_home)
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
    return R_NilValue;
}

SEXP jl_eval(SEXP cmd)
{
    char *s = CHAR(STRING_ELT(cmd, 0));
    jl_value_t *ret = jl_eval_string(s);
    

    if (jl_is_float64(ret))
    {
        
        double retDouble = jl_unbox_float64(ret);
        Rprintf("test value is %e\n", retDouble);
    }
    if (jl_is_int32(ret))
    {
        int retInt32 = jl_unbox_int32(ret);
        Rprintf("test value is 32bit int %d\n", retInt32);
    }
    return R_NilValue;
}

SEXP Julia_R(jl_value_t* Res)
{
   
   return R_NilValue;
}

SEXP R_Julia(SEXP Var,SEXP VarNam)
{
  int n;
  jl_value_t* ret;
  char *VarName = CHAR(STRING_ELT(VarNam, 0));
  if ((n = LENGTH(Var))!= 0) {
  switch (TYPEOF(Var)) {
  case LGLSXP:
   break;
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
  case REALSXP:{
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
    break;
  default:
   break; 
  }
}
  return R_NilValue;
}

