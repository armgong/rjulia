/*
Copyright (C) 2014, 2015 by Yu Gong
*/
#ifndef JULIA_R_H
#define JULIA_R_H

#ifdef __cplusplus
extern "C" {
#endif

#include <R.h>
#include <julia.h>

SEXP Julia_R(jl_value_t* Var);
SEXP Julia_BigintToDouble(SEXP Var);
#ifdef __cplusplus
}
#endif
#endif
