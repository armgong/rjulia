/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#define pkgdebug

SEXP Julia_R_Scalar(jl_value_t* Var);
SEXP Julia_R_MD(jl_value_t* Var);
