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

jl_value_t* R_Julia_MD(SEXP Var,const char* VarName);
jl_value_t* R_Julia_MD_NA(SEXP Var,const char* VarName);
jl_value_t* R_Julia_MD_NA_DataFrame(SEXP Var,const char* VarName);