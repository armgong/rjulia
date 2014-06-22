/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include "R_Julia.h"
#define pkgdebug

jl_array_t* CreateArray(jl_datatype_t* type,size_t ndim,jl_tuple_t* dims)
{
  return jl_new_array(jl_apply_array_type(type,ndim),dims);;
}

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
   {
    jl_tupleset(d, i, jl_box_long(INTEGER(dims)[i]));
  }
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

void R_Julia_MD(SEXP Var,const char* VarName)
{
  if ((LENGTH(Var))!= 0) {
    jl_tuple_t* dims=RDims_JuliaTuple(Var);
    switch (TYPEOF( Var)) {
      case LGLSXP:
      {
        jl_array_t* ret= CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
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
        jl_array_t* ret= CreateArray(jl_int32_type,jl_tuple_len(dims),dims);
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
        jl_array_t* ret= CreateArray(jl_float64_type,jl_tuple_len(dims),dims);
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
        jl_array_t* ret= CreateArray(jl_ascii_string_type,jl_tuple_len(dims),dims);
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


//first pass creat array then convert it to DataArray
//second pass assign NA to element
void TransArrayToDataArray(jl_array_t* mArray,jl_array_t* mboolArray,const char* VarName)
{
  char evalcmd[4096]; 
  jl_set_global(jl_main_module, jl_symbol("TransVarName"), (jl_value_t*)mArray);
  jl_set_global(jl_main_module, jl_symbol("TransVarNamebool"), (jl_value_t*)mboolArray);
  sprintf(evalcmd,"%s=DataArray(TransVarName,TransVarNamebool)",VarName);
  jl_eval_string(evalcmd);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
  }
}
void R_Julia_MD_NA(SEXP Var,const char* VarName)
{
  if ((LENGTH(Var))!= 0) {
    jl_tuple_t* dims=RDims_JuliaTuple(Var);

    switch (TYPEOF(Var)) {
      case LGLSXP:
      {
        jl_array_t* ret= CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        jl_array_t* ret1=CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH(&ret,&ret1);
        
        char* retData = (char*)jl_array_data(ret);
        bool* retData1 = (bool*)jl_array_data(ret1);
        for(size_t i=0; i<jl_array_len(ret); i++)
        {
          if (LOGICAL(Var)[i]==NA_LOGICAL)
          { 
            retData[i] =1;
            retData1[i] =true;
          }
          else
          {
            retData[i] =LOGICAL(Var)[i];
            retData1[i] =false;
          }
        }   
        JL_GC_POP();
        TransArrayToDataArray(ret,ret1,VarName);
        break;
      };
      case INTSXP:
      {
        jl_array_t* ret= CreateArray(jl_int32_type,jl_tuple_len(dims),dims);
        jl_array_t* ret1=CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH(&ret,&ret1);
        
        int* retData = (int*)jl_array_data(ret);
        bool* retData1 = (bool*)jl_array_data(ret1);
        for(size_t i=0; i<jl_array_len(ret); i++)
        {
          if (INTEGER(Var)[i]==NA_INTEGER)
          {
            retData[i] =999;
            retData1[i] =true;
          }
          else
          {
            retData[i] =INTEGER(Var)[i];
            retData1[i] =false;
          }
        }  
        JL_GC_POP();   
        TransArrayToDataArray(ret,ret1,VarName);
        break;
      }
      case REALSXP:
      {
        jl_array_t* ret= CreateArray(jl_float64_type,jl_tuple_len(dims),dims);
        jl_array_t* ret1=CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH(&ret,&ret1);
        double* retData = (double*)jl_array_data(ret);
        bool* retData1 = (bool*)jl_array_data(ret1);
        for(size_t i=0; i<jl_array_len(ret); i++)
        {
          if (ISNAN(REAL(Var)[i]))
          {
           retData[i] =999.01;
           retData1[i] =true;
         }
         else
         {
          retData[i] =REAL(Var)[i]; 
          retData1[i] =false;
        }
      }   
      JL_GC_POP();
      TransArrayToDataArray(ret,ret1,VarName);
      break;
    }
    case STRSXP:
    {
      jl_array_t* ret= CreateArray(jl_ascii_string_type,jl_tuple_len(dims),dims);
      jl_array_t* ret1=CreateArray(jl_bool_type,jl_tuple_len(dims),dims);

      JL_GC_PUSH(&ret,&ret1);
      jl_value_t** retData=jl_array_data(ret); 
      bool* retData1 = (bool*)jl_array_data(ret1);
      for(size_t i=0; i<jl_array_len(ret); i++)
      {
        if (STRING_ELT(Var, i)==NA_STRING)
        {
         retData[i] =jl_cstr_to_string("999");
         retData1[i] =true;
       }
       else
       {
        retData[i] =jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
        retData1[i] =false;
      } 
    }   
    JL_GC_POP(); 
    TransArrayToDataArray(ret,ret1,VarName);
    break;
  }
  default:
  break; 
  }//case end
 }//length !=0
} 