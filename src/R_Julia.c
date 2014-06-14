/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include "R_Julia.h"
#define pkgdebug

jl_tuple_t* RDims_JuliaTuple(SEXP Var)
{
 jl_tuple_t* d;
 SEXP dims = getAttrib(Var, R_DimSymbol);
 //array or matrix
 if (dims!=R_NilValue)
 {
   int ndims = LENGTH(dims);
   d = jl_alloc_tuple(ndims);
   JL_GC_PUSH1(&d);
   size_t i;
   for(i=0; i < ndims; i++)
    jl_tupleset(d, i, jl_box_long(INTEGER(dims)[i]));
  JL_GC_POP();
}
  else //vector
  {
   d = jl_alloc_tuple(1);
   JL_GC_PUSH1(&d);
   jl_tupleset(d, 0, jl_box_long(LENGTH(Var)));
   JL_GC_POP();
 }
 return d;
}

void R_Julia_MD(SEXP Var,jl_value_t* ret,char* VarName)
{
  int n;
  if ((n = LENGTH(Var))!= 0) {
    jl_tuple_t* dims=RDims_JuliaTuple(Var);
    switch (TYPEOF( Var)) {
      case LGLSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_bool_type,1); 
        jl_array_t* ret= jl_new_array(array_type,dims);
        JL_GC_PUSH1(&ret);
        char* retData = (char*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =LOGICAL(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        JL_GC_POP(); 
        break;
      };
      case INTSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_int32_type,1); 
        jl_array_t* ret= jl_new_array(array_type,dims);
        JL_GC_PUSH1(&ret);
        int* retData = (int*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =INTEGER(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        JL_GC_POP(); 
        break;
      }
      case REALSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_float64_type,1); 
        jl_array_t* ret= jl_new_array(array_type,dims);
        JL_GC_PUSH1(&ret);
        double* retData = (double*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =REAL(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        JL_GC_POP();
        break;
      }
      case STRSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_ascii_string_type,1); 
        jl_array_t* ret= jl_new_array(array_type,dims);
        JL_GC_PUSH1(&ret);
        jl_value_t** retData=jl_array_data(ret); 
        for(size_t i=0; i<jl_array_len(ret); i++)
         retData[i] =jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
       jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
       JL_GC_POP();
       break;
     }
     default:
     break; 
   }
 }  
}