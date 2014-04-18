#include <julia.h>
#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
SEXP initJulia(SEXP julia_home)
{
    if (jl_is_initialized())
	 return R_NilValue;

    char* s = CHAR(STRING_ELT(julia_home, 0));
    if (strlen(s)==0)
     jl_init(NULL);
    else
      jl_init(s);
    
    JL_SET_STACK_BASE;
    if (jl_exception_occurred()) {
       error("Julia not initialized");
    }
    return R_NilValue;
}

SEXP jl_eval(SEXP cmd)
{
 char* s = CHAR(STRING_ELT(cmd, 0));
 jl_value_t *ret = jl_eval_string(s);
 double retDouble = jl_unbox_float64(ret);
 
 if (jl_is_float64(ret)) {
	double retDouble = jl_unbox_float64(ret);
	Rprintf("test value is %e",retDouble);
 }
 return R_NilValue;
}


