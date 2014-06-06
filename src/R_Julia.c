#include <stdio.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include "R_Julia.h"
#define pkgdebug


void R_Julia_Vector(SEXP Var,jl_value_t* ret,char* VarName)
{
  int n;
  if ((n = LENGTH(Var))!= 0) {
    switch (TYPEOF( Var)) {
      case LGLSXP:
      {
        jl_value_t* array_type =jl_apply_array_type(jl_bool_type,1); 
        jl_array_t* ret= jl_alloc_array_1d(array_type,n);
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
        jl_array_t* ret= jl_alloc_array_1d(array_type,n);
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
        jl_array_t* ret= jl_alloc_array_1d(array_type,n);
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
        jl_array_t* ret= jl_alloc_array_1d(array_type,n);
        JL_GC_PUSH1(&ret);
        char** retData = (char**)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
        {
         retData[i] =CHAR(STRING_ELT(Var, i));
           #ifdef pkgdebug
          // Rprintf("str is %d %s\n",i,retData[i]);
           #endif
       } 
       jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
       JL_GC_POP();
       break;
     }
     default:
     break; 
   }
 }
}

void R_Julia_Array(SEXP Var,jl_value_t* ret,char* VarName)
{
  
}