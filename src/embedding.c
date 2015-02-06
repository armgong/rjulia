/*
Copyright (C) 2014, 2015 by Yu Gong
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
//use static to avoid create many small object
SEXP Julia_is_running()
{
  static SEXP ans;
  if (ans==NULL)
  {
   ans = allocVector(LGLSXP, 1);
   R_PreserveObject(ans);
  }
  LOGICAL(ans)[0] = jlrunning;
  return ans;
}

SEXP initJulia(SEXP julia_home, SEXP DisableGC)
{
  if (jl_is_initialized())
    return R_NilValue;
  const char *s = CHAR(asChar(julia_home));
  if (strlen((char *)s) == 0)
    jl_init(NULL);
  else
    jl_init((char *)s);

  //v0.3 have this,but v0.4 delete it
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
  if (asLogical(DisableGC))
    jl_gc_disable();
  return R_NilValue;
}

//eval but not return val
SEXP jl_void_eval(SEXP cmd)
{
  const char *s = CHAR(asChar(cmd));
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
  const char *s = CHAR(asChar(cmd));
  jl_value_t *ret = jl_eval_string((char *)s);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
    return R_NilValue;
  }
  return Julia_R(ret);
}

#ifdef __cplusplus
}
#endif
