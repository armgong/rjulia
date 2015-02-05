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

//Convert R Type To Julia,which not contain NA
SEXP R_Julia(SEXP Var, SEXP VarNam);
//Convert R Type To Julia,which contain NA
SEXP R_Julia_NA(SEXP Var, SEXP VarNam);
//Convert R Type To Julia,which contain NA
SEXP R_Julia_NA_Factor(SEXP Var, SEXP VarNam);
//Convert R data frame To Julia
SEXP R_Julia_NA_DataFrame(SEXP Var, SEXP VarNam);

#ifdef __cplusplus
}
#endif
#endif
