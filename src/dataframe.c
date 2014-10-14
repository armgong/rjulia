/*
Copyright (C) 2014 by Yu Gong
*/
#include <R.h>
#include <Rinternals.h>
#include <julia.h>
#include <stdbool.h>
#include "dataframe.h"

static int DataArrayFrameInited = 0;
SEXP Julia_LoadDataArrayFrame()
{
  jl_eval_string("using DataArrays,DataFrames");
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
  }
  else
    DataArrayFrameInited = 1;
  return R_NilValue;
}

SEXP Julia_DataArrayFrameInited()
{
  SEXP ans;
  PROTECT(ans = allocVector(LGLSXP, 1));
  LOGICAL(ans)[0] = DataArrayFrameInited;
  UNPROTECT(1);
  return ans;
}
bool LoadDF()
{
  if (DataArrayFrameInited)
    return true;

  Julia_LoadDataArrayFrame();
  if (!DataArrayFrameInited)
  {
    error("DataArrays and DataFrames can't be load,please check this\n");
    return false;
  }
  return true;
}
