#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#define pkgdebug

void Julia_R_Basic_Element(jl_value_t* Var,SEXP ans);
void Julia_R_1D(jl_value_t* Var,SEXP ans);
