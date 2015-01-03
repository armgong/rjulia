/*
Copyright (C) 2014 by Yu Gong
*/
#include <R.h>
#include <Rinternals.h>
#include <julia.h>
#include "dataframe.h"
#include "Julia_R.h"
#include "R_Julia.h"

#ifdef __cplusplus
extern "C" {
#endif

static int jlrunning = 0;
SEXP Julia_is_running()
{
  SEXP ans;
  PROTECT(ans = allocVector(LGLSXP, 1));
  LOGICAL(ans)[0] = jlrunning;
  UNPROTECT(1);
  return ans;
}

SEXP initJulia(SEXP julia_home, SEXP DisableGC)
{
  if (jl_is_initialized())
    return R_NilValue;
  const char *s = CHAR(STRING_ELT(julia_home, 0));
  if (strlen((char *)s) == 0)
    jl_init(NULL);
  else
    jl_init((char *)s);
  //v0.3 have this,and v0.4 delete this
  #ifdef JL_SET_STACK_BASE
    JL_SET_STACK_BASE;
  #endif
  
  jlrunning = 1;
  if (jl_exception_occurred())
  {
    error("Julia not initialized");
    jlrunning = 0;
    return R_NilValue;
  }
  if (LOGICAL(DisableGC)[0])
    jl_gc_disable();
  return R_NilValue;
}

//eval but not return val
SEXP jl_void_eval(SEXP cmd)
{
  const char *s = CHAR(STRING_ELT(cmd, 0));
  jl_eval_string((char *)s);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
  }
  return R_NilValue;
}

//eval julia script and return
SEXP jl_eval(SEXP cmd)
{
  SEXP ans=R_NilValue;
  const char *s = CHAR(STRING_ELT(cmd, 0));
  jl_value_t* ret= jl_eval_string((char *)s);;
  JL_GC_PUSH1(&ret);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    JL_GC_POP();
    return ans;
  }
  ans=Julia_R(ret);
  JL_GC_POP();
  return ans;
}
#ifdef __cplusplus
}
#endif
