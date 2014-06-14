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

void R_Julia_MD(SEXP Var,jl_value_t* ret,char* VarName);