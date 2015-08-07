/*
Copyright (C) 2014, 2015 by Yu Gong
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
    return ScalarLogical(DataArrayFrameInited);
}

bool LoadDF()
{
  if (DataArrayFrameInited)
    return true;

  Julia_LoadDataArrayFrame();
  if (!DataArrayFrameInited)
  {
    error("DataArrays and DataFrames can't be loaded correctly into Julia, please check this");
    return false;
  }
  return true;
}
