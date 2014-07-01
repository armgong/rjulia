/*
Copyright (C) 2014 by Yu Gong
*/
#ifndef DATAFRAME_H
#define DATAFRAME_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
SEXP Julia_LoadDataArrayFrame();
SEXP Julia_DataArrayFrameInited();
bool LoadDF();
#ifdef __cplusplus
}
#endif
#endif
