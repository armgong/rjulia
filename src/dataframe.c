/*
Copyright (C) 2014, 2015 by Yu Gong
*/
#include <R.h>
#include <Rinternals.h>
#include <julia.h>
#include <stdbool.h>

#include "dataframe.h"

//whether DataArrays and DataFrames packages loaded
static int DataArrayFrameInited = 0;

//try load julia DataArrays and DataFrames packages
SEXP Julia_LoadDataArrayFrame()
{
  jl_eval_string("using DataArrays,DataFrames");
  if (jl_exception_occurred())
  {
    jl_call2(jl_get_function(jl_base_module, "show"), jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
  }
  else
    DataArrayFrameInited = 1;
  return R_NilValue;
}

//whether DataArrays and DataFrames packages loaded
SEXP Julia_DataArrayFrameInited()
{
    return ScalarLogical(DataArrayFrameInited);
}

//real function for load julia DataArrays and DataFrames packages
bool LoadDF() {
  if (DataArrayFrameInited)
    return true;

  Julia_LoadDataArrayFrame();
  if (!DataArrayFrameInited) {
    error("DataArrays and DataFrames can't be loaded correctly into Julia, please check this");
    return false;
  }
  return true;
}
